/**
 * @file gboy.h
 * @brief API for interacting with a GBoy instance.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "display/pixel_t.h"

/**
 * @brief Initializes the GBoy internal data. This must be called before anything else.
 * @returns Status Code: 0 - OK; otherwise, FAIL
 */
bool gboy_init(void);
/**
 * @brief Cleans up all reasources used by the GBoy. This can be called from any thread as it will
 * first safely stop emulation.
 */
void gboy_cleanup(void);

/**
 * @brief "Powers On" GBoy. This effective starts the emulation thread and begins emulation at the
 * given `clock_speed`.
 *
 * @param clock_speed The initial clock speed to start emulation at.
 * @return true - OK; false - FAIL
 */
bool gboy_poweron(const size_t clock_speed);
/**
 * @brief "Powers Off" GBoy. This effectives stops the emulation thread and resets the register
 * states.
 *
 * @returns true - OK; false - FAIL.
 */
bool gboy_poweroff(void);
/**
 * @brief Load a ROM file into GBoy. This will be set as the active cartridge.
 *
 * @param rom_path Filepath to the ROM file.
 * @returns true - OK; false - FAIL
 */
bool gboy_load_rom(const char* rom_path);
/**
 * @brief Ejects the current cartridge from the GBoy.
 *
 * @returns true - OK; false - FAIL.
 */
bool gboy_eject_cart(void);

/**
 * @brief DEBUG FUNCTION : Advances the emulator by one step.
 * @note A step is defined as one CPU (M) cycle. This is also 4 PPU (T) cycles.
 * @warning This can only be used when the clock speed to set to 0.
 *
 * @returns true - OK; false - FAIL.
 */
bool     gboy_step(void);
/**
 * @brief Sets the clock speed of GBoy.
 *
 * @param clock_speed New clock speed to set.
 * @returns true - OK; false - FAIL.
 */
bool     gboy_set_clock_speed(const uint32_t clock_speed);
/**
 * @brief Gets the currently set clock speed.
 *
 * @returns Current GBoy clock speed.
 */
uint32_t gboy_get_clock_speed(void);
/**
 * @brief Fetches a copy of the internal LCD data.
 *
 * @param pixel_buffer Buffer to fill with the copied data.
 * @param size Size of `pixel_buffer`. This must match the expected size of the LCD.
 * @returns true - OK; false - FAIL.
 */
bool     gboy_get_lcd(pixel_t* pixel_buffer, const size_t size);
