/**
 * @file cartridge.h
 * @brief Cartridge API for interacting with Cartridge Objects.
 */
#pragma once

#include "DEFINES.h"

/** @brief Gameboy Cartridge. */
struct cartridge;

/**
 * @brief Creates a new Cartridge Object.
 * @param filename Name of the ROM (and optionally RAM) file to load. Should not contain extensions.
 * Must not be NULL.
 * @returns A valid Cartridge Object with the file data loaded into it. NULL if creation fails.
 */
struct cartridge* cartridge_create(const char* filename);

/**
 * @brief Performs a read operation on the cartridge.
 * @param cart Cartridge to read from.
 * @param addr Address to read from inside the cartridge.
 * @returns Value at the specified address. 0xFF is returned on undefined reads.
 */
byte cartridge_read(struct cartridge* cart, const word addr);

/**
 * @brief Performs a write operation on the cartridge.
 * @param cart Cartridge to write to.
 * @param addr Address to write to inside the cartridge.
 * @param value Value to write.
 * @note Writes to undefined addresses are ignored (do not corrupt).
 */
void cartridge_write(struct cartridge* cart, const word addr, const byte value);

/**
 * @brief Saves the Cartridges RAM to a .sav file.
 * @param cart Cartridge to save RAM data.
 * @returns Integer indicating success: 0 for success; otherwise, non-zero value for failure.
 */
int cartridge_save(struct cartridge* cart);

/**
 * @brief Frees and cleans up all resources allocated.
 * @param cart [REF] Reference to the cartridge handle. Set to NULL upon exiting.
 * @note Should not throw/fail unexpectedly. Always succeeds.
 */
void cartridge_free(struct cartridge** p_cart);
