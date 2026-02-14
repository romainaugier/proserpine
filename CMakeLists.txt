# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 - Present Romain Augier MIT License
# All rights reserved

cmake_minimum_required(VERSION 3.16)

project(proserpine LANGUAGES CXX)

option(BUILD_GRAPHICS_TESTS "Build testing suite of the graphics system" OFF)

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

enable_testing()

add_subdirectory(tests)
