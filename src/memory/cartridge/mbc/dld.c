/**
 * @file dld.c
 * @brief Contains the implementation of the Discrete Logic Decoder (DLD). This is a type of Memory
 * Bank Controller (MBC).
 */
#include "memory/cartridge/mbc/dld.h"

#include <assert.h>
#include <log.h>
#include <stdlib.h>

#include "DEFINES.h"
#include "memory/cartridge/mbc/mbc_iface.h"
#include "memory/memory_map.h"

struct dld_context {
    byte  rom[2 * CART_ROM_BANK_SIZE];
    byte* ram;
};

static_assert(
    sizeof(void*) == sizeof(struct dld_context*),
    "Pointer Size Mismatch!\n\rDLD Context Pointer does not map to the same size as void pointer."
);

static byte dld_read(void* ctx, const word addr);
static void dld_write(void* ctx, const word addr, const byte value);

static const struct mbc_iface m_dld_iface =
    {.read = dld_read, .write = dld_write, .destroy = dld_destroy};

const struct mbc_iface* dld_iface(void) { return &m_dld_iface; }

void* dld_instanciate(const size_t n_ram_banks) {
    if (n_ram_banks > 1) {
        log_error(
            "Attempted to create more ram banks than supported by the DLD MBC\n\r"
            "\tMax Supported: 1\n\r"
            "\tRequested:     %zu",
            n_ram_banks
        );
        return NULL;
    }

    struct dld_context* ctx = malloc(sizeof(struct dld_context));
    if (ctx == NULL) {
        log_error("Attempted to create a new DLD context but failed to allocate memory");
        return NULL;
    }
    ctx->ram = (n_ram_banks == 1) ? malloc(CART_RAM_BANK_SIZE) : NULL;

    return ctx;
}

void dld_destroy(void** p_ctx) {
    if (p_ctx == NULL) {
        log_warn("Attempted to destroy a context that doesn't exist.\n\r"
                 "\tp_ctx was NULL");
        return;
    }

    struct dld_context* dld_ctx = *p_ctx;
    free(dld_ctx->ram);
    dld_ctx->ram = NULL; // Unnecessary since we free the whole context afterwards
    free(dld_ctx);
    *p_ctx = NULL; // Prevent double free by forcing passed in dld context pointer to NULL

    return;
}

static byte dld_read(void* ctx, const word addr) {
    if (ctx == NULL) {
        log_error("Attempted to read from a NULL DLD context! Returned 0xFF");
        return 0xFF;
    }

    struct dld_context* dld_ctx = ctx;

    if (addr >= ADDR_CART_ROM_START && addr <= ADDR_CART_ROM_END) {
        byte value = dld_ctx->rom[addr - ADDR_CART_ROM_START];
        log_debug(
            "DLD ROM Read:\n\r"
            "\tADDR: 0x%.4X\n\r"
            "\tValue: 0x%.2X",
            addr,
            value
        );
        return value;
    } else if (dld_ctx->ram != NULL && addr >= ADDR_CART_RAM_START && addr <= ADDR_CART_RAM_END) {
        byte value = dld_ctx->ram[addr - ADDR_CART_RAM_START];
        log_debug(
            "DLD RAM Read:\n\r"
            "\tADDR: 0x%.4X\n\r"
            "\tValue: 0x%.2X",
            addr,
            value
        );
        return value;
    } else {
        log_warn(
            "DLD Out-of-Bounds Read:\n\r"
            "\tADDR: 0x%.4X\n\r"
            "\tValue: 0xFF",
            addr
        );
        return 0xFF;
    }
}

static void dld_write(void* ctx, const word addr, const byte value) {
    if (ctx == NULL) {
        log_error("Attempted to write to a NULL DLD context! Write ignored.");
        return;
    }

    struct dld_context* dld_ctx = ctx;

    if (addr >= ADDR_CART_ROM_START && addr <= ADDR_CART_ROM_END) {
        log_warn("Attempted to write to DLD's ROM. Write ignored.");
        return;
    } else if (dld_ctx->ram != NULL && addr >= ADDR_CART_RAM_START && addr <= ADDR_CART_RAM_END) {
        log_debug(
            "DLD RAM Write:\n\r"
            "\tADDR: 0x%.4X\n\r"
            "\tValue: 0x%.2X",
            addr,
            value
        );
        dld_ctx->ram[addr - ADDR_CART_RAM_START] = value;
        return;
    } else {
        log_warn(
            "DLD Out-of-Bounds Write:\n\r"
            "\tADDR: 0x%.4X\n\r"
            "\tValue: 0x%.2X\n\r"
            "\tWrite Ignored.",
            addr,
            value
        );
        return;
    }
}
