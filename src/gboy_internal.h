/**
 * @file gboy_internal.h
 * @brief Internal API for GBoy Modules.
 */
#pragma once

#include <stdbool.h>

#include "display/pixel_t.h"

// Based on 4.194304 MHz clock speed (*1000^2 / 4)
#define GBOY_DEFAULT_CLOCK_SPEED 1048576

bool gboy_set_lcd_pixel(const size_t x, const size_t y, const pixel_t pixel);