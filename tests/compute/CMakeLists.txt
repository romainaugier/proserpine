# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 - Present Romain Augier MIT License
# All rights reserved

function(proserpine_add_compute_test SOURCE)
    # Add test and link proserpine
    get_filename_component(TEST_NAME ${SOURCE} NAME_WLE)
    message(STATUS "Adding compute test : ${TEST_NAME}")
    add_executable(${TEST_NAME} ${SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE proserpine_lib)
    target_include_directories(${TEST_NAME} PRIVATE ${TEST_UTILS_INCLUDE_DIR})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    # Compile associated shader and add as target of test
    string(REPLACE "test_" "" SHADER_NAME ${TEST_NAME})

    set(COMP_SHADER_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_NAME}.comp")
    set(COMP_SHADER_BINARY "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_NAME}.comp.spv")

    add_custom_command(
        OUTPUT ${COMP_SHADER_BINARY}
        COMMAND ${GLSLC} ${GLSLC_ARGS} -o ${COMP_SHADER_BINARY} ${COMP_SHADER_SOURCE}
        COMMENT "Compiling GLSL shader: ${COMP_SHADER_SOURCE}"
    )

    add_custom_target(${SHADER_NAME} ALL DEPENDS ${COMP_SHADER_BINARY})

    add_dependencies(${TEST_NAME} ${SHADER_NAME})
endfunction()

file(GLOB_RECURSE TEST_FILES *.cpp)

foreach(TEST_FILE ${TEST_FILES})
    proserpine_add_compute_test(${TEST_FILE})
endforeach()
