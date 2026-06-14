/**
 * @file gboy.h
 * @brief API for interacting with a GBoy instance.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "display/pixel.h"

// Based on 4.194304 MHz clock speed (*1000^2 / 4)
#define GBOY_DEFAULT_CLOCK_SPEED 1048576

#define GBOY_LCD_WIDTH  160
#define GBOY_LCD_HEIGHT 144
#define GBOY_LCD_SIZE   (GBOY_LCD_WIDTH * GBOY_LCD_HEIGHT)

bool gboy_load_cart(const char* path);
void gboy_eject_cart(void);
bool gboy_poweron(const size_t clock_speed);
void gboy_poweroff(void);

bool   gboy_step(void);
bool   gboy_set_clock_speed(const size_t clock_speed);
size_t gboy_get_clock_speed(void);
bool   gboy_get_lcd(pixel_t* pixel_buffer, const size_t size);

bool gboy_debugger_is_open(void);
void gboy_debugger_open(void);
void gboy_debugger_close(void);
void gboy_debugger_update(void);
