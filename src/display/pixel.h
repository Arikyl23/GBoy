/**
 * @file pixel.h
 * @brief Defines the pixel type.
 */
#pragma once

#include <stdint.h>

/** @brief Standard RGBA8888 Pixel. */
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} pixel_t;
