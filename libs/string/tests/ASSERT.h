/**
 * @file ASSERT.h
 * @brief Testing header that contains various forms of ASSERT macros for quickly terminating a
 * test.
 */
#pragma once

#include <log.h>
#include <stdarg.h>
#include <stdbool.h>

#define ASSERT(...)                                                                                \
    do {                                                                                           \
        log_error(__VA_ARGS__);                                                                    \
        return false;                                                                              \
    } while (0)

#define ASSERT_NULL(ptr, ...)                                                                      \
    do {                                                                                           \
        if (ptr != NULL) { ASSERT(__VA_ARGS__); }                                                  \
    } while (0)

#define ASSERT_NOT_NULL(ptr, ...)                                                                  \
    do {                                                                                           \
        if (ptr == NULL) { ASSERT(__VA_ARGS__); }                                                  \
    } while (0)

#define ASSERT_OK(rc, ...)                                                                         \
    do {                                                                                           \
        if (rc != 0) { ASSERT(__VA_ARGS__); }                                                      \
    } while (0)

#define ASSERT_NOT_OK(rc, ...)                                                                     \
    do {                                                                                           \
        if (rc == 0) { ASSERT(__VA_ARGS__); }                                                      \
    } while (0)

#define ASSERT_TRUE(cond, ...)                                                                     \
    do {                                                                                           \
        if (cond == false) { ASSERT(__VA_ARGS__); }                                                \
    } while (0)

#define ASSERT_FALSE(cond, ...)                                                                    \
    do {                                                                                           \
        if (cond == true) { ASSERT(__VA_ARGS__); }                                                 \
    } while (0)

#define ASSERT_EQ(lhs, rhs, ...)                                                                   \
    do {                                                                                           \
        if (lhs != rhs) { ASSERT(__VA_ARGS__); }                                                   \
    } while (0)

#define ASSERT_NQ(lhs, rhs, ...)                                                                   \
    do {                                                                                           \
        if (lhs == rhs) { ASSERT(__VA_ARGS__); }                                                   \
    } while (0)

#define ASSERT_GT(lhs, rhs, ...)                                                                   \
    do {                                                                                           \
        if (lhs <= rhs) { ASSERT(__VA_ARGS__); }                                                   \
    }

#define ASSERT_GTEQ(lhs, rhs, ...)                                                                 \
    do {                                                                                           \
        if (lhs < rhs) { ASSERT(__VA_ARGS__); }                                                    \
    } while (0)

#define ASSERT_LT(lhs, rhs, ...)                                                                   \
    do {                                                                                           \
        if (lhs >= rhs) { ASSERT(__VA_ARGS__); }                                                   \
    } while (0)

#define ASSERT_LTEQ(lhs, rhs, ...)                                                                 \
    do {                                                                                           \
        if (lhs > rhs) { ASSERT(__VA_ARGS__); }                                                    \
    } while (0)

;
