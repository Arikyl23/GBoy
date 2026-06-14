/**
 * @file texture_scale_mode.h
 * @brief Enumeration for defining a textures scaling mode.
 */
#pragma once

#define TABLE(X)                                                                                   \
    X(TEXTURE_SCALEMODE_NEAREST, 0, "NEAREST")                                                     \
    X(TEXTURE_SCALEMODE_LINEAR, 1, "LINEAR")                                                       \
    X(TEXTURE_SCALEMODE_PIXELART, 2, "PIXELART")

enum texture_scale_mode {
#define X(id, code, str) id = code,
    TABLE(X)
#undef X
};

static inline const char* texture_scale_mode_to_string(const enum texture_scale_mode value) {
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
