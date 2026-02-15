# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 - Present Romain Augier MIT License
# All rights reserved

function(proserpine_add_graphics_test SOURCE)
    # Add test and link proserpine
    get_filename_component(TEST_NAME ${SOURCE} NAME_WLE)
    message(STATUS "Building graphics test : ${TEST_NAME}")
    add_executable(${TEST_NAME} ${SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE proserpine)
    target_link_libraries(${TEST_NAME} PRIVATE glfw)
    target_include_directories(${TEST_NAME} PRIVATE ${TEST_UTILS_INCLUDE_DIR})

    # Maybe use Python to run tests ? All graphics tests could render a single
    # fixed image and we could compare pixels to make sure no regressions have
    # been made ?
    # Run test -> capture window -> compare with validated result -> success or fail
    # add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    # Compile associated shaders and add as target of test
    string(REPLACE "test_" "" SHADER_BASE_NAME ${TEST_NAME})

    file(GLOB_RECURSE
         TEST_SHADERS
         ${SHADER_BASE_NAME}*.vert
         ${SHADER_BASE_NAME}*.geom
         ${SHADER_BASE_NAME}*.frag)

    foreach(SHADER ${TEST_SHADERS})
        get_filename_component(SHADER_NAME ${SHADER} NAME_WE)

        set(SHADER_SOURCE ${SHADER})
        set(SHADER_BINARY "${SHADER}.spv")

        add_custom_command(
            OUTPUT ${COMP_SHADER_BINARY}
            COMMAND ${GLSLC} ${GLSLC_ARGS} -o ${SHADER_BINARY} ${SHADER_SOURCE}
            COMMENT "Compiling GLSL shader: ${SHADER_SOURCE}"
        )

        add_custom_target(${SHADER_NAME} ALL DEPENDS ${SHADER_BINARY})

        add_dependencies(${TEST_NAME} ${SHADER_NAME})
    endforeach()
endfunction()

# Get glfw
include(FetchContent)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.9
)

FetchContent_MakeAvailable(glfw)

# build tests
file(GLOB_RECURSE TEST_FILES *.cpp)

foreach(TEST_FILE ${TEST_FILES})
    proserpine_add_graphics_test(${TEST_FILE})
endforeach()
