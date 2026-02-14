// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 - Present Romain Augier MIT License
// All rights reserved

// If no Vulkan device is available the test exits with code 0 (skip)

// logger defines all macros for proserpine logging
#include "logger.hpp"

#define PROSERPINE_INCLUDE_VULKAN
#define PROSERPINE_IMPLEMENTATION
#include "proserpine.hpp"

#include "testing.hpp"

#include <cstdio>
#include <cstring>
#include <vector>

static bool g_gpu_available = false;
static proserpine::VulkanContext* g_ctx = nullptr;

static auto error_exit_callback = [](const proserpine::Error& err) -> void {
    std::fprintf(stdout, "Error: %s", err.message.c_str());
    std::exit(1);
};

// Try to create a minimal context. Returns false if no device is found.
bool try_create_context(proserpine::VulkanContext& ctx)
{
    proserpine::VulkanContext::CreateInfo ci;
    ci.application_name  = "proserpine_integration_test";
    ci.enable_validation = true;

    ci.device_filter.required_extensions = {};

    auto result = proserpine::VulkanContext::create(ci);

    if(!result)
    {
        std::fprintf(stdout, "[SKIP] No Vulkan device: %s\n",
                     result.error().message.c_str());

        return false;
    }

    ctx = result.value();

    return true;
}

void test_context_creation()
{
    TEST_BEGIN("VulkanContext creation");

    CHECK(g_ctx->instance()        != VK_NULL_HANDLE);
    CHECK(g_ctx->physical_device() != VK_NULL_HANDLE);
    CHECK(g_ctx->device()          != VK_NULL_HANDLE);
    CHECK(g_ctx->has_queue(proserpine::QueueType::Graphics));

    TEST_END();
}

void test_feature_queries()
{
    TEST_BEGIN("Feature queries");

    // Default profile enables these
    CHECK(g_ctx->is_feature_supported(proserpine::Feature::TimelineSemaphore));
    CHECK(g_ctx->is_feature_supported(proserpine::Feature::BufferDeviceAddress));
    CHECK(g_ctx->is_feature_supported(proserpine::Feature::DynamicRendering));

    // Ray tracing is NOT enabled by default
    CHECK(!g_ctx->is_feature_supported(proserpine::Feature::RayQuery));

    TEST_END();
}

void test_device_properties()
{
    TEST_BEGIN("Device properties populated");

    auto& props = g_ctx->device_properties();
    CHECK(props.properties.apiVersion >= VK_API_VERSION_1_3);
    CHECK(props.properties.deviceName[0] != '\0');
    CHECK(!props.queue_families.empty());

    std::fprintf(stdout, "        Device: %s\n", props.properties.deviceName);

    TEST_END();
}

void test_buffer_creation()
{
    TEST_BEGIN("Buffer creation and BDA");

    proserpine::Buffer::CreateInfo bci;
    bci.size  = 1024;
    bci.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    bci.memory_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    auto buf = g_ctx->create_buffer(bci).value_or(error_exit_callback);
    CHECK(buf.handle() != VK_NULL_HANDLE);
    CHECK(buf.size() == 1024);
    CHECK(buf.device_address() != 0);  // BDA should be non-zero

    TEST_END();
}

void test_host_visible_buffer()
{
    TEST_BEGIN("Host-visible buffer with mapped pointer");

    proserpine::Buffer::CreateInfo bci;
    bci.size  = 256;
    bci.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bci.memory_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    auto buf = g_ctx->create_buffer(bci).value_or(error_exit_callback);
    CHECK(buf.handle() != VK_NULL_HANDLE);
    CHECK(buf.mapped_ptr() != nullptr);

    // Write to mapped memory
    std::memset(buf.mapped_ptr(), 0xAB, 256);
    CHECK(static_cast<uint8_t*>(buf.mapped_ptr())[0] == 0xAB);
    CHECK(static_cast<uint8_t*>(buf.mapped_ptr())[255] == 0xAB);

    TEST_END();
}

void test_buffer_move()
{
    TEST_BEGIN("Buffer move semantics");

    proserpine::Buffer::CreateInfo bci;
    bci.size  = 512;
    bci.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    auto a = g_ctx->create_buffer(bci).value_or(error_exit_callback);
    VkBuffer raw = a.handle();
    CHECK(raw != VK_NULL_HANDLE);

    proserpine::Buffer b = std::move(a);
    CHECK(a.handle() == VK_NULL_HANDLE);  // source emptied
    CHECK(b.handle() == raw);             // destination has it

    TEST_END();
}

void test_image_creation()
{
    TEST_BEGIN("Image creation");

    proserpine::Image::CreateInfo ici;
    ici.extent = {64, 64, 1};
    ici.format = VK_FORMAT_R8G8B8A8_UNORM;
    ici.usage  = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    auto img = g_ctx->create_image(ici).value_or(error_exit_callback);
    CHECK(img.handle() != VK_NULL_HANDLE);
    CHECK(img.view()   != VK_NULL_HANDLE);
    CHECK(img.format()  == VK_FORMAT_R8G8B8A8_UNORM);
    CHECK(img.extent().width  == 64);
    CHECK(img.extent().height == 64);

    TEST_END();
}

void test_depth_image()
{
    TEST_BEGIN("Depth image creation");

    proserpine::Image::CreateInfo ici;
    ici.extent = {128, 128, 1};
    ici.format = VK_FORMAT_D32_SFLOAT;
    ici.usage  = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    auto img = g_ctx->create_image(ici).value_or(error_exit_callback);
    CHECK(img.handle() != VK_NULL_HANDLE);
    CHECK(img.view()   != VK_NULL_HANDLE);
    CHECK(img.format()  == VK_FORMAT_D32_SFLOAT);

    TEST_END();
}

void test_timeline_semaphore()
{
    TEST_BEGIN("Timeline semaphore creation and signaling");

    auto sem = g_ctx->create_timeline_semaphore(0).value_or(error_exit_callback);
    CHECK(sem.handle() != VK_NULL_HANDLE);
    CHECK(sem.counter() == 0);

    // CPU signal
    sem.signal(5);
    CHECK(sem.counter() == 5);

    // CPU wait (already signaled, should return immediately)
    VkResult r = sem.wait(5, 1000000000ULL /* 1s */);
    CHECK(r == VK_SUCCESS);

    TEST_END();
}

void test_timeline_semaphore_move()
{
    TEST_BEGIN("Timeline semaphore move");

    auto a = g_ctx->create_timeline_semaphore(10).value_or(error_exit_callback);
    VkSemaphore raw = a.handle();

    proserpine::TimelineSemaphore b = std::move(a);
    CHECK(a.handle() == VK_NULL_HANDLE);
    CHECK(b.handle() == raw);
    CHECK(b.counter() == 10);

    TEST_END();
}

void test_fence_pool()
{
    TEST_BEGIN("FencePool acquire and release");

    auto& pool = g_ctx->fence_pool();

    VkFence f1 = pool.acquire(false);
    CHECK(f1 != VK_NULL_HANDLE);

    VkFence f2 = pool.acquire(true);
    CHECK(f2 != VK_NULL_HANDLE);

    pool.release(f1);
    pool.release(f2);

    // Re-acquire should recycle
    VkFence f3 = pool.acquire(false);
    CHECK(f3 != VK_NULL_HANDLE);
    CHECK(f3 == f2 || f3 == f1); // recycled from pool

    pool.release(f3);

    TEST_END();
}

void test_immediate_submit()
{
    TEST_BEGIN("Immediate submit");

    bool cmd_was_recorded = false;

    g_ctx->immediate_submit(proserpine::QueueType::Graphics,
        [&](VkCommandBuffer cmd) {
            CHECK(cmd != VK_NULL_HANDLE);
            // Just a pipeline barrier to exercise the command buffer
            VkMemoryBarrier barrier{VK_STRUCTURE_TYPE_MEMORY_BARRIER};
            barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            vkCmdPipelineBarrier(cmd,
                VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                0, 1, &barrier, 0, nullptr, 0, nullptr);
            cmd_was_recorded = true;
        });

    CHECK(cmd_was_recorded);

    TEST_END();
}

void test_staging_buffer_upload()
{
    TEST_BEGIN("StagingBufferManager upload to buffer");

    proserpine::Buffer::CreateInfo bci;
    bci.size  = 256;
    bci.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    bci.memory_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    auto dst = g_ctx->create_buffer(bci).value_or(error_exit_callback);
    CHECK(dst.handle() != VK_NULL_HANDLE);

    std::vector<uint8_t> data(256, 0xCD);
    g_ctx->staging().upload_to_buffer(data.data(), data.size(), dst.handle());
    g_ctx->staging().flush();

    // We can't easily read back from device-local, but no crash = success
    TEST_END();
}

void test_staging_image_upload()
{
    TEST_BEGIN("StagingBufferManager upload to image");

    proserpine::Image::CreateInfo ici;
    ici.extent = {16, 16, 1};
    ici.format = VK_FORMAT_R8G8B8A8_UNORM;
    ici.usage  = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    auto img = g_ctx->create_image(ici).value_or(error_exit_callback);
    CHECK(img.handle() != VK_NULL_HANDLE);

    std::vector<uint8_t> pixels(16 * 16 * 4, 0xFF);
    g_ctx->staging().upload_to_image(pixels.data(), ici.extent, ici.format, img.handle());
    g_ctx->staging().flush();

    TEST_END();
}

void test_timeline_callback_system()
{
    TEST_BEGIN("TimelineCallbackSystem deferred execution");

    auto sem = g_ctx->create_timeline_semaphore(0).value_or(error_exit_callback);
    auto& cbs = g_ctx->timeline_callbacks();

    std::atomic<bool> called{false};
    cbs.enqueue(sem.handle(), 1, [&called]() {
        called.store(true, std::memory_order_release);
    });

    sem.signal(1);

    for(int i = 0; i < 100 && !called.load(std::memory_order_acquire); ++i)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    CHECK(called.load(std::memory_order_acquire));

    TEST_END();
}

void test_wait_idle()
{
    TEST_BEGIN("wait_idle");

    g_ctx->immediate_submit(proserpine::QueueType::Graphics, [](VkCommandBuffer) { /* noop */ });

    g_ctx->wait_idle();

    TEST_END();
}

void test_multiple_buffer_lifecycle()
{
    TEST_BEGIN("Multiple buffers created and destroyed");

    std::vector<proserpine::Buffer> buffers;

    for(int i = 0; i < 16; ++i)
    {
        proserpine::Buffer::CreateInfo buffer_create_info;
        buffer_create_info.size  = 1024 * (i + 1);
        buffer_create_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        buffers.push_back(g_ctx->create_buffer(buffer_create_info).value_or(error_exit_callback));
        CHECK(buffers.back().handle() != VK_NULL_HANDLE);
    }

    buffers.erase(buffers.begin(), buffers.begin() + 8);
    CHECK(buffers.size() == 8);

    for(auto& b : buffers)
    {
        CHECK(b.handle() != VK_NULL_HANDLE);
    }

    TEST_END();
}

void test_renderdoc_integration()
{
    TEST_BEGIN("RenderDocIntegration (may not be available)");

    auto& rd = g_ctx->renderdoc();

    if(rd.is_available())
        std::fprintf(stdout, "RenderDoc: available\n");
    else
        std::fprintf(stdout, "RenderDoc: not loaded\n");

    TEST_END();
}

int main()
{
    std::fprintf(stdout, "============================\n");
    std::fprintf(stdout, "proserpine integration tests\n");
    std::fprintf(stdout, "============================\n\n");

    proserpine::VulkanContext ctx;

    if(!try_create_context(ctx))
    {
        std::fprintf(stdout, "Cannot find a Vulkan device, skipping integration tests");
        return 0;
    }

    g_ctx = &ctx;

    test_context_creation();
    test_feature_queries();
    test_device_properties();
    test_buffer_creation();
    test_host_visible_buffer();
    test_buffer_move();
    test_image_creation();
    test_depth_image();
    test_timeline_semaphore();
    test_timeline_semaphore_move();
    test_fence_pool();
    test_immediate_submit();
    test_staging_buffer_upload();
    test_staging_image_upload();
    test_timeline_callback_system();
    test_wait_idle();
    test_multiple_buffer_lifecycle();
    test_renderdoc_integration();

    g_ctx = nullptr;

    std::fprintf(stdout, "\n============================\n");
    std::fprintf(stdout, "%d/%d tests passed\n", g_test_count - g_fail_count, g_test_count);
    std::fprintf(stdout, "\n============================\n");

    return g_fail_count > 0 ? 1 : 0;
}
