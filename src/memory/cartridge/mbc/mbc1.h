/**
 * @file mbc1.h
 * @brief Defines the public API for the first revision of the Memory Bank Controller (MBC1). This
 * is a type of Memory Bank Controller (MBC).
 */
#pragma once

#include <stddef.h>

#include "memory/cartridge/mbc/mbc_iface.h"

const struct mbc_iface* mbc1_iface(void);
void*                   mbc1_instanciate(const size_t n_rom_banks, const size_t n_ram_banks);
void                    mbc1_destroy(void** p_ctx);
