# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 - Present Romain Augier MIT License
# All rights reserved

# GLSLC
set(GLSLC ${Vulkan_GLSLC_EXECUTABLE})
set(GLSLC_ARGS --target-env=vulkan1.4 -O)

# Utils
set(TEST_UTILS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/utils)

add_subdirectory(integration)

add_subdirectory(compute)

# Test vk_cube (we don't add it to tests since it will open a Window, needs to be run manually)
if(BUILD_GRAPHICS_TESTS)
    add_subdirectory(graphics)
endif()
