/**
 * @file string_type.h
 * @brief Enumeration header for string types.
 */
#pragma once

#define STATUS_TABLE(X)                                                                            \
    X(STR_TYP_SSO, 0, "SSO")                                                                       \
    X(STR_TYP_HEAP, 1, "HEAP")                                                                     \
    X(STR_TYP_VIEW, 2, "VIEW")

/** @brief Enumeration representing the internal type of a string object. */
enum string_type {
#define X(id, code, str) id = code,
    STATUS_TABLE(X)
#undef X
};

inline static const char* string_type_to_string(const enum string_type type) {
    switch (type) {
#define X(id, code, str)                                                                           \
    case code:                                                                                     \
        return str;
        STATUS_TABLE(X)
#undef X
    default:
        return "unknown";
    }
}

#undef STATUS_TABLE
