// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 - Present Romain Augier MIT License
// All rights reserved

// logger defines all macros for proserpine logging
#include "logger.hpp"

#define PROSERPINE_INCLUDE_VULKAN
#define PROSERPINE_IMPLEMENTATION
#include "proserpine.hpp"

#include "testing.hpp"

#include <filesystem>

int main(int argc, char** argv)
{
    // Callback for Expected<T>.value_or
    auto error_exit_callback = [](const proserpine::Error& err) {
        PROSERPINE_LOG_ERROR("%s", err.message.c_str());
        std::exit(1);
    };

    // Create the context
    proserpine::VulkanContext::CreateInfo create_info;
    create_info.application_name = "Test Compute Add";
    create_info.enable_validation = true;
    create_info.features.timeline_semaphore = true;
    create_info.features.buffer_device_address = true;
    create_info.descriptor_pool_info.pool_size_count = 1;
    create_info.descriptor_pool_info.max_sets = 1024;

    // Create descriptor pool sizes
    VkDescriptorPoolSize descriptor_pool_sizes[] = { { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1024 } };
    create_info.descriptor_pool_info.sizes = descriptor_pool_sizes;

    proserpine::VulkanContext ctx = proserpine::VulkanContext::create(create_info).value_or(error_exit_callback);

    // Create the two input buffers and the output buffer
    proserpine::BufferCreateInfo buffer_info{};
    buffer_info.size = 1024 * sizeof(float);
    buffer_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    buffer_info.memory_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    proserpine::Buffer A = ctx.create_buffer(buffer_info);
    proserpine::Buffer B = ctx.create_buffer(buffer_info);
    proserpine::Buffer C = ctx.create_buffer(buffer_info);

    // Upload some data
    std::vector<float> ones(1024, 1.0f);

    ctx.staging().upload_to_buffer(ones.data(), ones.size() * sizeof(float), A.handle());
    ctx.staging().upload_to_buffer(ones.data(), ones.size() * sizeof(float), B.handle());
    ctx.staging().flush();

    // Load the precompiled compute shader (see CMakeLists.txt for compile command with glslc)
    const auto shader_path = std::filesystem::path(__FILE__).parent_path() / "compute_add.comp.spv";

    auto spirv = proserpine::load_spirv_file(shader_path.generic_string().c_str());

    if(spirv.size() == 0)
    {
        PROSERPINE_LOG_ERROR("Cannot load spirv shader from file");
        return 1;
    }

    proserpine::ShaderModule compute_shader = proserpine::ShaderModule::create(ctx.device(),
                                                                               spirv,
                                                                               VK_SHADER_STAGE_COMPUTE_BIT).value_or(error_exit_callback);

    // Build the pipeline layout
    proserpine::PipelineLayoutBuilder builder(ctx.device());
    auto set_info = compute_shader.descriptor_set_layouts();
    PROSERPINE_ASSERT(!set_info.empty());
    builder.add_set(0, set_info[0]);

    proserpine::PipelineLayout layout = builder.build().value_or(error_exit_callback);

    // Create the compute pipeline
    proserpine::ComputePipeline pipeline = proserpine::create_compute_pipeline(ctx.device(),
                                                                               compute_shader,
                                                                               layout.handle(),
                                                                               "main").value_or(error_exit_callback);

    // Allocate the descriptor set and write descriptors
    proserpine::DescriptorSet set = ctx.allocate_descriptor_set(layout.set_layouts()[0]).value_or(error_exit_callback);

    set.write(0, A, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
       .write(1, B, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
       .write(2, C, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

    set.update();

    // Submit compute command
    ctx.immediate_submit(proserpine::QueueType::Compute, [&](VkCommandBuffer cmd)
    {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.handle());

        vkCmdBindDescriptorSets(cmd,
                                VK_PIPELINE_BIND_POINT_COMPUTE,
                                layout.handle(),
                                0,
                                1,
                                &set.handle(),
                                0,
                                nullptr);

        vkCmdDispatch(cmd, 1024 / 64, 1, 1);
    });

    // Read the output back
    proserpine::Buffer readback_buffer = ctx.create_buffer({
        .size = C.size(),
        .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        .memory_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    });

    ctx.immediate_submit(proserpine::QueueType::Transfer, [&](VkCommandBuffer c) {
        VkBufferCopy region{};
        region.size = C.size();
        vkCmdCopyBuffer(c, C.handle(), readback_buffer.handle(), 1, &region);
    });

    // Result is here on cpu
    const float* result = static_cast<const float*>(readback_buffer.mapped_ptr());

    PROSERPINE_LOG_INFO("result[0] = %f", *result);

    return 0;
}
