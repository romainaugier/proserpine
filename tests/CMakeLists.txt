# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 - Present Romain Augier MIT License
# All rights reserved

# Utils
set(TEST_UTILS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/utils)

# Proserpine static lib
add_library(proserpine_lib STATIC proserpine_lib.cpp)
target_link_libraries(proserpine_lib PUBLIC proserpine)
target_include_directories(proserpine_lib PRIVATE ${TEST_UTILS_INCLUDE_DIR})

# GLSLC
set(GLSLC ${Vulkan_GLSLC_EXECUTABLE})
set(GLSLC_ARGS --target-env=vulkan1.4 -O)

add_subdirectory(integration)
add_subdirectory(compute)

if(BUILD_GRAPHICS_TESTS)
    add_subdirectory(graphics)
endif()
