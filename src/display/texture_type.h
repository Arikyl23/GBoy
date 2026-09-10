/**
 * @file texture_type.h
 * @brief Enumeration for defining the type of texture.
 */
#pragma once

#define TABLE(X)                                                                                   \
    X(TEXTURE_TYPE_STATIC, 0, "STATIC")                                                            \
    X(TEXTURE_TYPE_STREAMING, 1, "STREAMING")                                                      \
    X(TEXTURE_TYPE_TARGET, 2, "TARGET")

enum texture_type {
#define X(id, code, str) id = code,
    TABLE(X)
#undef X
};

static inline const char* texture_type_to_string(const enum texture_type value) {
    switch (value) {
#define X(id, code, str)                                                                           \
    case code:                                                                                     \
        return str;
        TABLE(X)
#undef X
    default:
        return "unknown";
    }
}

#undef TABLE