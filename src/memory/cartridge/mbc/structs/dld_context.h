/**
 * @file dld_context.h
 */
#pragma once

#include <assert.h>

#include "DEFINES.h"
#include "memory/memory_map.h"

struct dld_context {
    byte   rom[2 * CART_ROM_BANK_SIZE];
    byte*  ram;
    size_t rom_size;
    size_t ram_size;
};

static_assert(
    sizeof(void*) == sizeof(struct dld_context*),
    "Pointer Size Mismatch!\n\rDLD Context Pointer does not map to the same size as void pointer."
);