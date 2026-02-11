/**
 * @file string_status.h
 * @brief Contains the list of status codes and useful functions for interacting with them.
 */
#pragma once

#include <stdbool.h>

// Tri-state boundaries
#define STATUS_WARNING 100
#define STATUS_ERROR   1000

// Warning Ranges
#define STATUS_WARNING_COMMON STATUS_WARNING

// Error Ranges
#define STATUS_ERROR_COMMON STATUS_ERROR
#define STATUS_ERROR_MEMORY STATUS_ERROR + 100

#define STATUS_TABLE(X)                                                                            \
    X(STRING_OK, 0, "OK", "Success")                                                               \
    X(STRING_WALREADY, STATUS_WARNING_COMMON + 1, "WALREADY", "Already performed")                 \
    X(STRING_WAGAIN, STATUS_WARNING_COMMON + 2, "WAGAIN", "Resource temporarily unavailable")      \
    X(STRING_EALREADY, STATUS_ERROR_COMMON + 1, "EALREADY", "Already performed")                   \
    X(STRING_EAGAIN, STATUS_ERROR_COMMON + 2, "EAGAIN", "Resource temporarily unavailable")        \
    X(STRING_ENULL, STATUS_ERROR_COMMON + 3, "ENULL", "Null Pointer Argument")                     \
    X(STRING_EINVAL, STATUS_ERROR_COMMON + 4, "EINVAL", "Invalid Argument")                        \
    X(STRING_EBAD, STATUS_ERROR_COMMON + 5, "EBAD", "Bad Usage")                                   \
    X(STRING_ENOTSUP, STATUS_ERROR_COMMON + 6, "ENOTSUP", "Operation not supported")               \
    X(STRING_ENOMEM, STATUS_ERROR_MEMORY + 1, "ENOMEM", "Out of Memory")

/** @brief Status representing the internal state of a string object. */
enum string_status {
#define X(id, code, str, desc) id = code,
    STATUS_TABLE(X)
#undef X
};

inline static bool string_status_is_error(const enum string_status status) {
    return status >= STATUS_ERROR;
}

inline static bool string_status_is_warning(const enum string_status status) {
    return (status >= STATUS_WARNING && !string_status_is_error(status));
}

inline static const char* string_status_to_string(const enum string_status status) {
    switch (status) {
#define X(id, code, str, desc)                                                                     \
    case code:                                                                                     \
        return str;
        STATUS_TABLE(X)
#undef X
    default:
        return "unknown";
    }
}

inline static const char* string_status_get_desc(const enum string_status status) {
    switch (status) {
#define X(id, code, str, desc)                                                                     \
    case code:                                                                                     \
        return desc;
        STATUS_TABLE(X)
#undef X
    default:
        return "unknown";
    }
}

#undef STATUS_WARNING
#undef STATUS_ERROR
#undef STATUS_WARNING_COMMON
#undef STATUS_ERROR_COMMON
#undef STATUS_ERROR_MEMORY
#undef STATUS_TABLE
