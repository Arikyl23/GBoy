/**
 * @file ppu_mode.h
 * @brief Enumeration for defining the Pixel Processing Unit's MODE.
 */
#pragma once

#define TABLE(X)                                                                                   \
    X(PPU_MODE_HBLANK, 0, "HBLANK")                                                                \
    X(PPU_MODE_VBLANK, 1, "VBLANK")                                                                \
    X(PPU_MODE_OAM, 2, "OAM SCAN")                                                                 \
    X(PPU_MODE_DRAW, 3, "DRAW")

enum ppu_mode {
#define X(id, code, str) id = code,
    TABLE(X)
#undef X
};

static inline const char* ppu_mode_to_string(const enum ppu_mode value) {
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
