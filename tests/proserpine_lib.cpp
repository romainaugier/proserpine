// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 - Present Romain Augier MIT License
// All rights reserved

// Compile a static library from this file to avoid rebuilding all tests everytime
// The more tests there will be the more time the compilation takes...

// logger defines all macros for proserpine logging
#include "logger.hpp"

#define PROSERPINE_INCLUDE_VULKAN
#define PROSERPINE_IMPLEMENTATION
#define PROSERPINE_AS_STATIC_LIB
#include "proserpine.hpp"
