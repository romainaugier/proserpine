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

#define TEST_SIZE (1024 * 1024)

void glfw_error_callback(int, const char* msg)
{
    PROSERPINE_LOG_ERROR("GLFW: %s", msg);
}

int main(int argc, char** argv)
{
    // Callback for Expected<T>.value_or
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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "vk_cube", nullptr, nullptr);

    // Create the context
    proserpine::VulkanContext::CreateInfo create_info;
    create_info.application_name = "Test Vulkan Cube";
    create_info.enable_validation = true;
    create_info.features.timeline_semaphore = true;
    create_info.features.buffer_device_address = true;
    create_info.descriptor_pool_info.pool_size_count = 1;
    create_info.descriptor_pool_info.max_sets = 1024;

    // Create descriptor pool sizes
    VkDescriptorPoolSize descriptor_pool_sizes[] = { { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1024 } };
    create_info.descriptor_pool_info.sizes = descriptor_pool_sizes;

    std::uint32_t glfw_exts_count = 0;
    const char** glfw_exts = glfwGetRequiredInstanceExtensions(&glfw_exts_count);

    for(std::uint32_t i = 0; i < glfw_exts_count; ++i)
        create_info.extra_instance_extensions.push_back(glfw_exts[i]);

    // Create Context
    proserpine::VulkanContext ctx = proserpine::VulkanContext::create(create_info).value_or(error_exit_callback);

    ctx.create_surface([&](VkInstance instance, VkSurfaceKHR* surface) {
        if(glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
            return false;

        return true;
    });

    // Create SwapChain
    proserpine::SwapChain::CreateInfo sc_create_info;
    sc_create_info.present_mode = VK_PRESENT_MODE_MAILBOX_KHR;

    std::int32_t width, height;
    glfwGetFramebufferSize(window, &width, &height);

    sc_create_info.extent_width = static_cast<std::uint32_t>(width);
    sc_create_info.extent_height = static_cast<std::uint32_t>(height);

    proserpine::SwapChain swapchain = ctx.create_swapchain(sc_create_info).value_or(error_exit_callback);

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
