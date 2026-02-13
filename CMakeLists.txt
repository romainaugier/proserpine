# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 - Present Romain Augier MIT License
# All rights reserved

cmake_minimum_required(VERSION 3.16)

project(proserpine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Vulkan
find_package(Vulkan REQUIRED)

# Header-only library target
add_library(proserpine INTERFACE)
target_include_directories(proserpine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(proserpine INTERFACE Vulkan::Vulkan)

if(UNIX AND NOT APPLE)
    target_link_libraries(proserpine INTERFACE dl pthread)
elseif(APPLE)
    target_link_libraries(proserpine INTERFACE pthread)
endif()

function(proserpine_add_test name source)
    add_executable(${name} ${source})
    target_link_libraries(${name} PRIVATE proserpine)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

enable_testing()

proserpine_add_test(test_integration tests/test_integration.cpp)

# Test Compute
set(SHADER_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/tests/compute_add.comp")
set(SHADER_BINARY "${CMAKE_CURRENT_SOURCE_DIR}/tests/compute_add.comp.spv")

add_custom_command(OUTPUT ${SHADER_BINARY}
                   COMMAND ${Vulkan_GLSLC_EXECUTABLE} --target-env=vulkan1.4 -O -o ${SHADER_BINARY} ${SHADER_SOURCE}
                   COMMENT "Compiling GLSL shader: ${SHADER_SOURCE}")

add_custom_target(shaders ALL DEPENDS ${SHADER_BINARY})

proserpine_add_test(test_compute_add tests/test_compute_add.cpp)

add_dependencies(test_compute_add shaders)
