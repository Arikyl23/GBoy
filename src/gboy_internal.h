/**
 * @file gboy_internal.h
 * @brief Internal API for GBoy Modules.
 */
#pragma once

#include "display/pixel.h"

void gboy_set_lcd_pixel(const size_t x, const size_t y, const pixel_t pixel);