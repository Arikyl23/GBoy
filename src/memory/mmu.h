/**
 * @file mmu.h
 * @brief Memory Management Unit (MMU). It is responsible for handling
 * all read/write requests on the Address Bus.
 */
#pragma once

#include "DEFINES.h"
#include "memory/bus.h"
#include "memory/cartridge/cartridge.h"

// ========== I/O ==========
/**
 * @brief Reads the value at the specified memory address.
 * @param source Bus that initiated the call.
 * @param addr Memory address to read from.
 * @returns Byte stored at memory address. `0xFF` is returned on blocked or invalid addresses.
 */
byte mmu_read(const enum bus source, const word addr);

/**
 * @brief Writes a value to the specified memory address.
 * @param source Bus that initiated the call.
 * @param addr Memory addres to write to.
 * @param value Byte to write.
 * @note Writes are ignored on addresses that are either blocked or invalid.
 */
void mmu_write(const enum bus source, const word addr, const byte value);
// ========== I/O ==========

// ========== Carts ==========
/**
 * @brief Loads a cartridge into the MMU.
 * @param cart Cartridge to load.
 * @returns Status code:
 * @returns - `0` | Success
 * @returns - `-1` | Invalid Cartridge
 * @returns - `-2` | Cartridge already loaded
 */
int mmu_load_cartridge(struct cartridge* cart);

/**
 * @brief Ejects a cartridge from the MMU.
 * @returns The ejected cartridge. This should either be saved to file or discarded.
 */
struct cartridge* mmu_eject_cartridge(void);
// ========== Carts ==========