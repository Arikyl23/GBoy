/**
 * @file mbc1_context.h
 */
#pragma once

#include <assert.h>

#include "DEFINES.h"
#include "memory/memory_map.h"

struct mbc1_context {
    byte*  rom;
    byte*  ram;
    size_t rom_size;
    size_t ram_size;
    byte   ram_enable;
    byte   rom_bank_number;
    byte   ram_bank_number;
    byte   banking_mode;
    dword  rom_bank_mask;
    dword  ram_bank_mask;
};

static_assert(
    sizeof(void*) == sizeof(struct mbc1_context*),
    "Pointer Size Mismatch!\nMBC1 Context pointer does not map to the same size as void pointer."
);