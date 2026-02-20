# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 - Present Romain Augier MIT License
# All rights reserved

function(proserpine_add_integration_test SOURCE)
    get_filename_component(TEST_NAME ${SOURCE} NAME_WLE)
    message(STATUS "Adding integration test : ${TEST_NAME}")
    add_executable(${TEST_NAME} ${SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE proserpine_lib)
    target_include_directories(${TEST_NAME} PRIVATE ${TEST_UTILS_INCLUDE_DIR})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

file(GLOB_RECURSE TEST_FILES *.cpp)

foreach(TEST_FILE ${TEST_FILES})
    proserpine_add_integration_test(${TEST_FILE})
endforeach()
