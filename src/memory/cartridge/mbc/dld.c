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
#include "memory/cartridge/mbc/structs/dld_context.h"
#include "memory/memory_map.h"

LOG_MODULE_SETUP("DLD", CONFIG_DLD_MODULE_LOG_LEVEL);

static byte* dld_rom(void* ctx, size_t* size);
static byte* dld_ram(void* ctx, size_t* size);
static byte  dld_read(void* ctx, const word addr);
static void  dld_write(void* ctx, const word addr, const byte value);

static const struct mbc_iface m_dld_iface =
    {.rom = dld_rom, .ram = dld_ram, .read = dld_read, .write = dld_write, .destroy = dld_destroy};

const struct mbc_iface* dld_iface(void) { return &m_dld_iface; }

void* dld_instantiate(const size_t n_rom_banks, const size_t n_ram_banks) {
    if (n_rom_banks != 2) {
        log_error(
            "Attempted to create DLD with more/less than 2 ROM banks.\n"
            "\tRequested: %.zu",
            n_rom_banks
        );
        return NULL;
    }
    if (n_ram_banks > 1) {
        log_error(
            "Attempted to create more ram banks than supported by the DLD MBC\n"
            "\tMax Supported: 1\n"
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
    ctx->rom_size = 2 * CART_ROM_BANK_SIZE;
    if (n_ram_banks == 1) {
        ctx->ram      = malloc(CART_RAM_BANK_SIZE);
        ctx->ram_size = CART_RAM_BANK_SIZE;
    } else {
        ctx->ram      = NULL;
        ctx->ram_size = 0;
    }

    return ctx;
}

void dld_destroy(void** p_ctx) {
    if (p_ctx == NULL || *p_ctx == NULL) { return; }

    struct dld_context* dld_ctx = *p_ctx;
    free(dld_ctx->ram);
    free(dld_ctx);
    *p_ctx = NULL; // Prevent double free by forcing passed in dld context pointer to NULL

    return;
}

static byte* dld_rom(void* ctx, size_t* size) {
    if (ctx == NULL) {
        log_error("DLD context is NULL!");
        return NULL;
    }

    if (size == NULL) {
        log_error("Unable to return size. Variable pointer is NULL");
        return NULL;
    }

    struct dld_context* dld_ctx = ctx;
    *size                       = dld_ctx->rom_size;
    return dld_ctx->rom;
}

static byte* dld_ram(void* ctx, size_t* size) {
    if (ctx == NULL) {
        log_error("DLD context is NULL!");
        return NULL;
    }

    if (size == NULL) {
        log_error("Unable to return size. Variable pointer is NULL");
        return NULL;
    }

    struct dld_context* dld_ctx = ctx;
    *size                       = dld_ctx->ram_size;
    return dld_ctx->ram;
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
            "DLD ROM Read:\n"
            "\tADDR: 0x%.4X\n"
            "\tValue: 0x%.2X",
            addr,
            value
        );
        return value;
    } else if (dld_ctx->ram != NULL && addr >= ADDR_CART_RAM_START && addr <= ADDR_CART_RAM_END) {
        byte value = dld_ctx->ram[addr - ADDR_CART_RAM_START];
        log_debug(
            "DLD RAM Read:\n"
            "\tADDR: 0x%.4X\n"
            "\tValue: 0x%.2X",
            addr,
            value
        );
        return value;
    } else {
        log_warn(
            "DLD Out-of-Bounds Read:\n"
            "\tADDR: 0x%.4X\n"
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
            "DLD RAM Write:\n"
            "\tADDR: 0x%.4X\n"
            "\tValue: 0x%.2X",
            addr,
            value
        );
        dld_ctx->ram[addr - ADDR_CART_RAM_START] = value;
        return;
    } else {
        log_warn(
            "DLD Out-of-Bounds Write:\n"
            "\tADDR: 0x%.4X\n"
            "\tValue: 0x%.2X\n"
            "\tWrite Ignored.",
            addr,
            value
        );
        return;
    }
}
