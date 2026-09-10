/**
 * @file lcd.h
 * @brief Public API for the LCD display.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "display/pixel_t.h"

#define LCD_WIDTH  160
#define LCD_HEIGHT 144
#define LCD_SIZE   (LCD_WIDTH * LCD_HEIGHT)
#define LCD_PITCH  (LCD_WIDTH * sizeof(pixel_t))

typedef pixel_t lcd_frame_t[LCD_SIZE];

bool         lcd_init(void);
lcd_frame_t* lcd_request_draw_buffer(void);
void         lcd_write_pixel(const pixel_t* pixel, const size_t x, const size_t y);
void         lcd_frame_complete(void);
