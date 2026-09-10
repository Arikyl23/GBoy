/**
 * @file ppu.h
 * @brief API for the Gameboy's Pixel Processing Unit (PPU)
 */
#pragma once

/**
 * @brief Processes the next Pixel and pushes it to the LCD.
 * @returns Status of the PPU. 0 if okay; otherwise, non-zero.
 */
int ppu_execute(void);

/**
 * @brief Gets the current scanlines dot count.
 */
int ppu_get_scanline_dot_count(void);