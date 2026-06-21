/**
 * @file colour_t.h
 * @brief Defines the RGB colours.
 */
#pragma once

#include <stdint.h>

/** @brief Defines an RGBA8888 colour. */
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} colour_t;
