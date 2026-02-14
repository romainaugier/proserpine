// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 - Present Romain Augier MIT License
// All rights reserved

static int g_test_count = 0;
static int g_fail_count = 0;
static const char* g_current_test = nullptr;

#define TEST_BEGIN(name)                                   \
    do {                                                   \
        g_current_test = (name);                           \
        g_test_count++;                                    \
        std::fprintf(stdout, "[TEST] [RUN] %s\n", (name)); \
    } while (0)

#define TEST_END()                                            \
    std::fprintf(stdout, "[TEST] [OK] %s\n", g_current_test); \
    std::fflush(stdout)

#define CHECK(expr)                                        \
    do {                                                   \
        if (!(expr)) {                                     \
            std::fprintf(stdout,                           \
                "[TEST] [FAIL] %s:%d: CHECK(%s) failed\n", \
                __FILE__, __LINE__, #expr);                \
            g_fail_count++;                                \
        }                                                  \
    } while (0)

#define ASSERT_EQ(lhs, rhs) CHECK((lhs) == (rhs))
#define ASSERT_NEQ(lhs, rhs) CHECK((lhs) != (rhs))
#define ASSERT_GT(lhs, rhs) CHECK((lhs) > (rhs))
#define ASSERT_GE(lhs, rhs) CHECK((lhs) >= (rhs))
#define ASSERT_LT(lhs, rhs) CHECK((lhs) < (rhs))
#define ASSERT_LE(lhs, rhs) CHECK((lhs) <= (rhs))
