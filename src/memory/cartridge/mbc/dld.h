/**
 * @file dld.h
 * @brief Defines the public API for a Discrete Logic Decoder (DLD). This is a type of
 * Memory Bank Controller (MBC).
 */
#pragma once

#include <stddef.h>

#include "memory/cartridge/mbc/mbc_iface.h"

/**
 * @brief Fetches the MBC interface for a DLD child object.
 * @returns A constant mbc_iface pointer to static memory containing the interface.
 */
const struct mbc_iface* dld_iface(void);
/**
 * @brief Instanciation of a new DLD type MBC.
 * @param n_ram_banks Specifies the number of RAM banks this DLD controls. Must be [0,1].
 * @returns A pointer to the created DLD context. A value of `NULL` means the instancation failed.
 */
void*                   dld_instanciate(const size_t n_ram_banks);
/**
 * @brief Destroy an instance of a DLD type MBC.
 * @param p_ctx Pointer to a DLD context pointer. Must not be NULL.
 * @note This sets the DLD context pointer to `NULL` destroying its internal memory.
 */
void                    dld_destroy(void** p_ctx);
