/**
 * @file mbc1.h
 * @brief Defines the public API for the first revision of the Memory Bank Controller (MBC1). This
 * is a type of Memory Bank Controller (MBC).
 */
#pragma once

#include <stddef.h>

#include "memory/cartridge/mbc/mbc_iface.h"

/**
 * @brief Fetches the MBC interface for a MBC1 child object.
 * @returns A constant mbc_iface pointer to static memory containing the interface.
 */
const struct mbc_iface* mbc1_iface(void);
/**
 * @brief Instanciation of a new MBC1 type MBC.
 * @param n_rom_banks Specifies the number of ROM banks this MBC1 controls. Must be [1, 128]
 * @param n_ram_banks Specifies the number of RAM banks this MBC1 controls. Must be [0,4].
 * @returns A pointer to the created MBC1 context. A value of `NULL` means the instancation failed.
 */
void*                   mbc1_instantiate(const size_t n_rom_banks, const size_t n_ram_banks);
/**
 * @brief Destroy an instance of a MBC1 type MBC.
 * @param p_ctx Pointer to a MBC1 context pointer. Must not be `NULL`.
 * @note This sets the MBC1 context pointer to `NULL` destroying its internal memory.
 */
void                    mbc1_destroy(void** p_ctx);
