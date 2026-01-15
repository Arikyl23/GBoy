/**
 * @file mbc1.c
 * @brief Contains the implementation of the first revision of the Memory Bank Controller (MBC1).
 * This is a type of Memory Bank Controller (MBC).
 */
#include "memory/cartridge/mbc/mbc1.h"

#include <assert.h>
#include <log.h>
#include <stddef.h>
#include <stdlib.h>

#include "DEFINES.h"
#include "memory/memory_map.h"

#define IS_POW2(x) (x > 0 && (x & (x - 1)) == 0)

struct mbc1_context {
    byte* rom;
    byte* ram;
    byte  ram_enable;
    byte  rom_bank_number;
    byte  ram_bank_number;
    byte  banking_mode;
    dword rom_bank_mask;
    dword ram_bank_mask;
};

static_assert(
    sizeof(void*) == sizeof(struct mbc1_context*),
    "Pointer Size Mismatch!\n\rMBC1 Context pointer does not map to the same size as void pointer."
);

static byte mbc1_read(void* ctx, const word addr);
static void mbc1_write(void* ctx, const word addr, const byte value);

static dword calc_rom_addr(const struct mbc1_context* ctx, const word addr);
static dword calc_ram_addr(const struct mbc1_context* ctx, const word addr);

static const struct mbc_iface m_mbc1_iface =
    {.read = mbc1_read, .write = mbc1_write, .destroy = mbc1_destroy};

const struct mbc_iface* mbc1_iface(void) { return &m_mbc1_iface; }

void* mbc1_instanciate(const size_t n_rom_banks, const size_t n_ram_banks) {
    // ROM/RAM bank count must be a power of 2 (RAM is allowed to be 0)
    if (!IS_POW2(n_rom_banks) || (!IS_POW2(n_ram_banks) && n_ram_banks != 0)) {
        log_error(
            "Attempted to create a MBC1 with a non-power of 2 bank count.\n\r"
            "\tRequested ROM Banks: %zu\n\r"
            "\tRequested RAM Banks: %zu",
            n_rom_banks,
            n_ram_banks
        );
        return NULL;
    }

    if (n_rom_banks <= 64) {
        // Standard configuration
        // ROM must exist and cannot be larger than 512 KiB [1,64]
        // RAM cannot be larger than 32  KiB [0,4]
        if (n_rom_banks == 0 || n_ram_banks > 4) {
            log_error(
                "Attempted to create a MBC1 in 512 KiB ROM configuration but specified more/less "
                "ROM/RAM banks than what is supported.\n\r"
                "\t[Max,Min] Supported ROM Banks: 64, 1\n\r"
                "\tRequested ROM Banks:           %zu\n\r"
                "\t[Max,Min] Supported RAM Banks: 4,  0\n\r"
                "\tRequested RAM Banks:           %zu",
                n_rom_banks,
                n_ram_banks
            );
            return NULL;
        }
    } else { // n_rom_banks > 64
        // 2 MiB Configuration
        // ROM must exist and cannot be larger than 2 MiB [1,128]
        // RAM cannot be larger than 8 KiB [0,1]
        if (n_rom_banks > 128 || n_ram_banks > 1) {
            log_error(
                "Attempted to create a MBC1 in 2 MiB ROM configuration but specified more/less "
                "ROM/RAM banks than what is supported.\n\r"
                "\t[Max,Min] Supported ROM Banks: 128, 1\n\r"
                "\tRequested ROM Banks:           %zu\n\r"
                "\t[Max,Min] Supported RAM Banks: 1,   0\n\r"
                "\tRequested RAM Banks:           %zu",
                n_rom_banks,
                n_ram_banks
            );
            return NULL;
        }
    }

    // Allocate main context structure
    struct mbc1_context* ctx = malloc(sizeof(struct mbc1_context));
    if (ctx == NULL) {
        log_error("Attempted to create a new MBC1 context but ran out of memory");
        return NULL;
    }
    // Set inital values
    ctx->rom             = NULL;
    ctx->ram             = NULL;
    ctx->ram_enable      = 0x00;
    ctx->rom_bank_number = 0x01;
    ctx->ram_bank_number = 0x00;
    ctx->banking_mode    = 0x00;
    ctx->rom_bank_mask   = n_rom_banks - 1;
    ctx->ram_bank_mask   = (n_ram_banks == 0) ? 0 : n_ram_banks - 1;

    // Allocate dynamic resources
    ctx->rom = malloc(n_rom_banks * CART_ROM_BANK_SIZE);
    if (ctx->rom == NULL) {
        log_error("Attempted to allocate ROM but ran out of memory");
        goto err;
    }
    ctx->ram = (n_ram_banks > 0) ? malloc(n_ram_banks * CART_RAM_BANK_SIZE) : NULL;
    if (n_ram_banks > 0 && ctx->ram == NULL) {
        log_error("Attempted to allocate RAM but ran out of memory");
        goto err;
    }

    return ctx;

err:
    if (ctx->ram != NULL) { free(ctx->ram); }
    if (ctx->rom != NULL) { free(ctx->rom); }
    free(ctx);
    return NULL;
}

void mbc1_destroy(void** p_ctx) {
    if (p_ctx == NULL || *p_ctx == NULL) {
        log_warn("Attempted to destroy a NULL MBC1 context.\n\r"
                 "\tp_ctx was NULL");
        return;
    }

    struct mbc1_context* mbc1_ctx = *p_ctx;
    free(mbc1_ctx->rom);
    free(mbc1_ctx->ram);
    free(mbc1_ctx);
    *p_ctx = NULL; // Prevent double free by forcing passed in dld context pointer to NULL

    return;
}

static byte mbc1_read(void* ctx, const word addr) {
    if (ctx == NULL) {
        log_error("Attempted to read from a NULL MBC1 context! Returned 0xFF");
        return 0xFF;
    }

    struct mbc1_context* mbc1_ctx = ctx;

    if (addr >= ADDR_CART_ROM_START && addr <= ADDR_CART_ROM_END) {
        byte value = mbc1_ctx->rom[calc_rom_addr(mbc1_ctx, addr)];
        log_debug(
            "MBC1 ROM Read:\n\r"
            "\tADDR: 0x%.4X\n\r"
            "\tValue: 0x%.2X",
            addr,
            value
        );
        return value;
    } else if (mbc1_ctx->ram != NULL && addr >= ADDR_CART_RAM_START && addr <= ADDR_CART_RAM_END) {
        if ((mbc1_ctx->ram_enable & 0x0A) != 0x0A) {
            log_debug("MBC1 RAM Read:\n\r"
                      "\tRAM is disabled. Returned 0xFF");
            return 0xFF;
        }

        byte value = mbc1_ctx->ram[calc_ram_addr(mbc1_ctx, addr)];
        log_debug(
            "MBC1 RAM Read:\n\r"
            "\tADDR: 0x%.4X\n\r"
            "\tValue: 0x%.2X",
            addr,
            value
        );
        return value;
    } else {
        log_warn(
            "MBC1 Out-of-Bounds Read:\n\r"
            "\tADDR: 0x%.4X\n\r"
            "\tValue: 0xFF",
            addr
        );
        return 0xFF;
    }
}

static void mbc1_write(void* ctx, const word addr, const byte value) {
    if (ctx == NULL) {
        log_error("Attempted to write to a NULL MBC1 context! Write ignored.");
        return;
    }

    struct mbc1_context* mbc1_ctx = ctx;

    if (addr >= ADDR_CART_ROM_START && addr <= ADDR_CART_ROM_END) {
        if (addr < 0x2000) {
            mbc1_ctx->ram_enable = value;
            log_debug(
                "MBC1 Register [ram_enable] Write:\n\r"
                "\tADDR:  0x%.4X\n\r"
                "\tvalue: 0x%.2X",
                addr,
                mbc1_ctx->ram_enable
            );
            return;
        } else if (addr < 0x4000) {
            mbc1_ctx->rom_bank_number = value & 0x1F;
            log_debug(
                "MBC1 Register [rom_bank_number] Write:\n\r"
                "\tADDR:  0x%.4X\n\r"
                "\tvalue: 0x%.2X",
                addr,
                mbc1_ctx->rom_bank_number
            );
            return;
        } else if (addr < 0x6000) {
            mbc1_ctx->ram_bank_number = value & 0x03;
            log_debug(
                "MBC1 Register [ram_bank_number] Write:\n\r"
                "\tADDR:  0x%.4X\n\r"
                "\tvalue: 0x%.2X",
                addr,
                mbc1_ctx->ram_bank_number
            );
            return;
        } else if (addr < 0x8000) {
            mbc1_ctx->banking_mode = value & 0x01;
            log_debug(
                "MBC1 Register [banking_mode] Write:\n\r"
                "\tADDR:  0x%.4X\n\r"
                "\tvalue: 0x%.2X",
                addr,
                mbc1_ctx->banking_mode
            );
            return;
        } else {
            log_error("MBC1: UNREACHABLE STATE");
            return;
        }
    } else if (mbc1_ctx->ram != NULL && addr >= ADDR_CART_RAM_START && addr <= ADDR_CART_RAM_END) {
        if ((mbc1_ctx->ram_enable & 0x0A) != 0x0A) {
            log_debug("MBC1 RAM Write:\n\r"
                      "\tRAM is disabled. Write ignored");
            return;
        }

        log_debug(
            "MBC1 RAM Write:\n\r"
            "\tADDR: 0x%.4X\n\r"
            "\tValue: 0x%.2X",
            addr,
            value
        );
        mbc1_ctx->ram[calc_ram_addr(mbc1_ctx, addr)] = value;
        return;
    } else {
        log_warn(
            "MBC1 Out-of-Bounds Write:\n\r"
            "\tADDR: 0x%.4X\n\r"
            "\tValue: 0x%.2X\n\r"
            "\tWrite Ignored.",
            addr,
            value
        );
        return;
    }
}

static dword calc_rom_addr(const struct mbc1_context* ctx, const word addr) {
    dword bank;

    if (addr < 0x4000) { // LOW ROM ADDR
        bank  = (ctx->banking_mode == 0) ? 0 : (dword)(ctx->ram_bank_number) << 5;
        bank &= ctx->rom_bank_mask;
        return ((dword)bank * CART_ROM_BANK_SIZE) + (dword)(addr - ADDR_CART_ROM_LOW_START);
    } else { // HIGH ROM ADDR
        bank = (ctx->ram_bank_number << 5) |
               ((ctx->rom_bank_number == 0x00) ? 0x01 : ctx->rom_bank_number);
        bank &= ctx->rom_bank_mask;
        return ((dword)bank * CART_ROM_BANK_SIZE) + (dword)(addr - ADDR_CART_ROM_HIGH_START);
    }
}

static dword calc_ram_addr(const struct mbc1_context* ctx, const word addr) {
    dword bank  = (ctx->banking_mode == 0) ? 0 : ctx->ram_bank_number;
    bank       &= ctx->ram_bank_mask;
    return ((dword)bank * CART_RAM_BANK_SIZE) + (dword)(addr - ADDR_CART_RAM_START);
}
