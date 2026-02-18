// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 - Present Romain Augier MIT License
// All rights reserved

// This file shows a very simple example of how to use proserpine to set up
// a window and a graphics pipeline to render a cube, similar to vkcube

// logger defines all macros for proserpine logging
#include "logger.hpp"

#define PROSERPINE_INCLUDE_VULKAN
#define PROSERPINE_IMPLEMENTATION
#include "proserpine.hpp"

#include "testing.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <filesystem>

#define MAX_FRAMES_IN_FLIGHT 2

void glfw_error_callback(int, const char* msg)
{
    PROSERPINE_LOG_ERROR("GLFW: %s", msg);
}

int main(int argc, char** argv)
{
    auto error_exit_callback = [](const proserpine::Error& err) {
        PROSERPINE_LOG_ERROR("%s", err.message.c_str());
        std::exit(1);
    };

    glfwSetErrorCallback(glfw_error_callback);

    if(!glfwInit())
    {
        PROSERPINE_LOG_ERROR("Cannot initialize glfw");
        return 1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "vk_triangle", nullptr, nullptr);

    proserpine::VulkanContext::CreateInfo create_info;
    create_info.application_name = "Test Vulkan Triangle";
    create_info.enable_validation = true;
    create_info.features.timeline_semaphore = true;
    create_info.features.buffer_device_address = true;
    create_info.descriptor_pool_info.pool_size_count = 1;
    create_info.descriptor_pool_info.max_sets = 1024;

    VkDescriptorPoolSize descriptor_pool_sizes[] = { { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1024 } };
    create_info.descriptor_pool_info.sizes = descriptor_pool_sizes;

    std::uint32_t glfw_exts_count = 0;
    const char** glfw_exts = glfwGetRequiredInstanceExtensions(&glfw_exts_count);

    for(std::uint32_t i = 0; i < glfw_exts_count; ++i)
        create_info.extra_instance_extensions.push_back(glfw_exts[i]);

    proserpine::VulkanContext ctx = proserpine::VulkanContext::create(create_info, [&](VkInstance instance, VkSurfaceKHR* surface) {
        if(glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
            return false;
        return true;
    }).value_or(error_exit_callback);

    proserpine::SwapChain::CreateInfo sc_create_info;
    sc_create_info.present_mode = VK_PRESENT_MODE_MAILBOX_KHR;

    std::int32_t width, height;
    glfwGetFramebufferSize(window, &width, &height);
    sc_create_info.extent_width  = static_cast<std::uint32_t>(width);
    sc_create_info.extent_height = static_cast<std::uint32_t>(height);

    proserpine::SwapChain swapchain = ctx.create_swapchain(sc_create_info).value_or(error_exit_callback);
    proserpine::Image depth_image   = ctx.create_depth_image(swapchain.extent()).value_or(error_exit_callback);

    proserpine::FrameManager frame_manager = ctx.create_frame_manager<2>().value_or(error_exit_callback);

    const auto vert_shader_path = std::filesystem::path(__FILE__).parent_path() / "vk_triangle.vert.spv";
    proserpine::ShaderModule vert_shader = proserpine::ShaderModule::create_vertex_shader(ctx.device(),
                                                                                          vert_shader_path.generic_string().c_str()).value_or(error_exit_callback);

    const auto frag_shader_path = std::filesystem::path(__FILE__).parent_path() / "vk_triangle.frag.spv";
    proserpine::ShaderModule frag_shader = proserpine::ShaderModule::create_fragment_shader(ctx.device(),
                                                                                            frag_shader_path.generic_string().c_str()).value_or(error_exit_callback);

    proserpine::ShaderStages shader_stages;
    shader_stages.add(vert_shader).add(frag_shader);

    proserpine::VertexInputState vertex_input;

    proserpine::PipelineLayoutBuilder builder(ctx.device());
    proserpine::PipelineLayout layout = builder.build().value_or(error_exit_callback);

    proserpine::GraphicsPipeline::CreateInfo gp_info;
    gp_info.color_format = swapchain.format();

    proserpine::GraphicsPipeline graphics_pipeline = proserpine::create_graphics_pipeline(ctx.device(),
                                                                                          gp_info,
                                                                                          shader_stages,
                                                                                          vertex_input,
                                                                                          layout.handle()).value_or(error_exit_callback);

    auto handle_swapchain_recreation = [&]() {
        std::int32_t w, h;
        glfwGetFramebufferSize(window, &w, &h);

        auto ok = swapchain.recreate(static_cast<std::uint32_t>(w),
                                     static_cast<std::uint32_t>(h));

        if(!ok)
        {
            PROSERPINE_LOG_ERROR("Error during swapchain recreation: " __FMT_STR " (" __FMT_I32 ")",
                                 ok.error().message.c_str(),
                                 ok.error().result);
            std::exit(1);
        }

        depth_image = ctx.create_depth_image(swapchain.extent()).value_or(error_exit_callback);
    };

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        auto frame = frame_manager.begin_frame();

        std::uint32_t swapchain_image_index;
        VkResult res = swapchain.acquire_next_image(frame.image_available_semaphore,
                                                    VK_NULL_HANDLE,
                                                    &swapchain_image_index);

        if(res == VK_ERROR_OUT_OF_DATE_KHR)
        {
            handle_swapchain_recreation();
            continue;
        }
        else if(res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR)
        {
            PROSERPINE_LOG_ERROR("Failed to acquire swapchain image");
            break;
        }

        proserpine::CommandBuffer& cmd = *frame.command_buffer;

        cmd.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        cmd.set_viewport(swapchain.extent().width, swapchain.extent().height);
        cmd.set_scissor(swapchain.extent().width, swapchain.extent().height);

        cmd.transition_image(swapchain.image(swapchain_image_index),
                             swapchain.image_first_use(swapchain_image_index) ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                             VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        swapchain.image_first_use_set(swapchain_image_index);

        cmd.transition_image(depth_image.handle(),
                             VK_IMAGE_LAYOUT_UNDEFINED,
                             VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                             VK_IMAGE_ASPECT_DEPTH_BIT);

        proserpine::CommandBuffer::RenderingInfo rendering_info{};
        rendering_info.render_area = { {0, 0}, swapchain.extent() };

        rendering_info.color_attachments.push_back({
            swapchain.image_view(swapchain_image_index),
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VkClearValue{ {{0.f, 0.f, 0.f, 1.f}} }
        });

        rendering_info.depth_attachment = proserpine::CommandBuffer::RenderingAttachment{
            depth_image.view(),
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VkClearValue{ .depthStencil = {1.f, 0} }
        };

        cmd.begin_render(rendering_info);
        cmd.bind_graphics_pipeline(graphics_pipeline.handle());
        cmd.draw(3, 1, 0, 0);
        cmd.end_render();

        cmd.transition_image(swapchain.image(swapchain_image_index),
                             VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                             VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        cmd.end();

        proserpine::VulkanContext::SubmitInfo submit_info;
        submit_info.command_buffers.push_back(frame.command_buffer);
        submit_info.wait_semaphores.push_back(frame.image_available_semaphore);
        submit_info.finished_semaphores.push_back(swapchain.image_rendered_semaphore(swapchain_image_index));
        submit_info.wait_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submit_info.fence = frame_manager.current_fence();

        ctx.submit(proserpine::QueueType::Graphics, submit_info);

        proserpine::VulkanContext::PresentInfo present_info;
        present_info.swapchain = swapchain.handle();
        present_info.wait_semaphores.push_back(swapchain.image_rendered_semaphore(swapchain_image_index));
        present_info.image_index = swapchain_image_index;

        VkResult result = ctx.present(present_info);

        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            handle_swapchain_recreation();
        }
        else if(result != VK_SUCCESS)
        {
            PROSERPINE_LOG_ERROR("Failed to present swapchain image");
            break;
        }

        frame_manager.end_frame();
    }

    ctx.wait_idle();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
