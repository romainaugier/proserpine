// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 - Present Romain Augier MIT License
// All rights reserved

#include <cstdio>

#define PROSERPINE_LOG_FORMAT_PRINTF

#define PROSERPINE_LOG_ERROR(fmt, ...) std::fprintf(stdout, "[ERROR] [proserpine] : " fmt "\n", ##__VA_ARGS__)
#define PROSERPINE_LOG_WARN(fmt, ...) std::fprintf(stdout, "[WARN] [proserpine] : " fmt "\n", ##__VA_ARGS__)
#define PROSERPINE_LOG_INFO(fmt, ...) std::fprintf(stdout, "[INFO] [proserpine] : " fmt "\n", ##__VA_ARGS__)
#define PROSERPINE_LOG_DEBUG(fmt, ...) std::fprintf(stdout, "[DEBUG] [proserpine] : " fmt "\n", ##__VA_ARGS__)
#define PROSERPINE_LOG_TRACE(fmt, ...) std::fprintf(stdout, "[TRACE] [proserpine] : " fmt "\n", ##__VA_ARGS__)
