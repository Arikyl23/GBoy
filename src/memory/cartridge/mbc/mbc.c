/**
 * @file mbc.c
 * @brief Contains the implementation of the Memory Bank Controller.
 */
#include "memory/cartridge/mbc/mbc.h"

#include <log.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "memory/cartridge/mbc/dld.h"
#include "memory/cartridge/mbc/mbc1.h"
#include "memory/cartridge/mbc/mbc_iface.h"
#include "memory/cartridge/mbc/structs/mbc.h"

LOG_MODULE_SETUP("MBC", CONFIG_MBC_MODULE_LOG_LEVEL);

/**
 * @brief Resets the internal MBC data to NULL
 * @param mbc MBC to reset.
 * @warning This should only be called when MBC is not pointing at allocated data. Ensure data is
 * freed before using this function.
 */
static void reset_mbc(struct mbc* mbc) {
    mbc->iface = NULL;
    mbc->ctx   = NULL;
    return;
}

/**
 * @brief Check if the mbc contains any NULL definitions.
 * @param mbc MBC to check.
 * @returns true if definition contains a NULL pointer.
 */
static bool null_mbc_check(struct mbc* mbc) {
    if (mbc == NULL) {
        log_error("MBC was NULL");
        return true;
    }

    if (mbc->iface == NULL || mbc->ctx == NULL) {
        log_error("MBC context is empty");
        return true;
    }

    return false;
}

struct mbc* mbc_create(
    const enum cartridge_type cart_type,
    const enum rom_size       rom_size,
    const enum ram_size       ram_size
) {
    if (!cartridge_type_is_valid(cart_type)) {
        log_error("Invalid Cartridge Type");
        return NULL;
    }
    if (!rom_size_is_valid(rom_size)) {
        log_error("Invalid ROM size");
        return NULL;
    }
    if (!ram_size_is_valid(ram_size)) {
        log_error("Invalid RAM size");
        return NULL;
    }

    size_t n_rom_banks = rom_size_bank_count(rom_size);
    size_t n_ram_banks = ram_size_bank_count(ram_size);

    struct mbc* mbc = malloc(sizeof(struct mbc));
    if (mbc == NULL) {
        log_error("Failed to allocate MBC. Ran out of memory.");
        return NULL;
    }

    reset_mbc(mbc);
    switch (cart_type) {
    case CART_ROM_ONLY:
        if (n_ram_banks != 0) {
            log_error(
                "Attempted to create MBC for ROM ONLY cart but specified more than 0 RAM banks"
            );
            break;
        }

        mbc->iface = dld_iface();
        mbc->ctx   = dld_instantiate(n_rom_banks, 0);
    case CART_ROM_RAM:
    case CART_ROM_RAM_BATTERY:
        if (n_ram_banks == 0) {
            log_error("Attempted to create MBC for ROM_RAM / ROM_RAM_BATTERY cart but specified no "
                      "RAM banks");
            break;
        }

        mbc->iface = dld_iface();
        mbc->ctx   = dld_instantiate(n_rom_banks, n_ram_banks);
        break;
    case CART_MBC1:
        if (n_ram_banks != 0) {
            log_error(
                "Attempted to create MBC for MBC1 ROM ONLY cart but specified more than 0 RAM banks"
            );
            break;
        }

        mbc->iface = mbc1_iface();
        mbc->ctx   = mbc1_instantiate(n_rom_banks, n_ram_banks);
        break;
    case CART_MBC1_RAM:
    case CART_MBC1_RAM_BATTERY:
        if (n_ram_banks == 0) {
            log_error("Attempted to create MBC for MBC1 ROM_RAM / ROM_RAM_BATTERY cart but "
                      "specified no RAM banks");
            break;
        }

        mbc->iface = mbc1_iface();
        mbc->ctx   = mbc1_instantiate(n_rom_banks, n_ram_banks);
        break;
    default:
    case CART_MBC2:
    case CART_MBC2_BATTERY:
    case CART_MMM01:
    case CART_MMM01_RAM:
    case CART_MMM01_RAM_BATTERY:
    case CART_MBC3_TIMER_BATTERY:
    case CART_MBC3_TIMER_RAM_BATTERY:
    case CART_MBC3:
    case CART_MBC3_RAM:
    case CART_MBC3_RAM_BATTERY:
    case CART_MBC5:
    case CART_MBC5_RAM:
    case CART_MBC5_RAM_BATTERY:
    case CART_MBC5_RUMBLE:
    case CART_MBC5_RUMBLE_RAM:
    case CART_MBC5_RUMBLE_RAM_BATTERY:
    case CART_MBC6:
    case CART_MBC7_SENSOR_RUMBLE_RAM_BATTERY:
    case CART_POCKET_CAMERA:
    case CART_BANDAI_TAMA5:
    case CART_HuC3:
    case CART_HuC1_RAM_BATTERY:
        log_error("Unsupported Cartridge Type");
        break;
    };

    if (mbc->iface == NULL || mbc->ctx == NULL) {
        log_error("Failed to setup MBC");
        mbc_cleanup(&mbc);
        return NULL;
    }

    return mbc;
}

int mbc_load_rom(struct mbc* mbc, const byte* data, const size_t size) {
    if (null_mbc_check(mbc)) {
        log_error("Invalid MBC context");
        return -1;
    }

    if (data == NULL) {
        log_warn("No data to copy");
        return 0;
    }

    size_t rom_size = 0;
    byte*  rom      = mbc->iface->rom(mbc->ctx, &rom_size);

    size_t size_to_copy = (size > rom_size) ? rom_size : size;
    memcpy(rom, data, size_to_copy);

    return 0;
}

int mbc_load_ram(struct mbc* mbc, const byte* data, const size_t size) {
    if (null_mbc_check(mbc)) {
        log_error("Invalid MBC context");
        return -1;
    }

    if (data == NULL) {
        log_warn("No data to copy");
        return 0;
    }

    size_t ram_size = 0;
    byte*  ram      = mbc->iface->ram(mbc->ctx, &ram_size);

    if (ram == NULL || ram_size == 0) {
        log_warn("No RAM to load data into");
        return 0;
    }

    size_t size_to_copy = (size > ram_size) ? ram_size : size;
    memcpy(ram, data, size_to_copy);

    return 0;
}

int mbc_dump_rom(struct mbc* mbc, byte* data, const size_t size) {
    if (null_mbc_check(mbc)) {
        log_error("Invalid MBC context");
        return -1;
    }

    size_t rom_size = 0;
    byte*  rom      = mbc->iface->rom(mbc->ctx, &rom_size);

    if (data == NULL || size < rom_size) {
        log_error("Insuffient data buffer size.");
        return -1;
    }

    memcpy(data, rom, rom_size);

    return 0;
}

int mbc_dump_ram(struct mbc* mbc, byte* data, const size_t size) {
    if (null_mbc_check(mbc)) {
        log_error("Invalid MBC context");
        return -1;
    }

    size_t ram_size = 0;
    byte*  ram      = mbc->iface->ram(mbc->ctx, &ram_size);

    if (data == NULL || size < ram_size) {
        log_error("Insuffient data buffer size.");
        return -1;
    }

    if (ram == NULL || ram_size == 0) {
        log_warn("No RAM to dump");
        return 0;
    }

    memcpy(data, ram, ram_size);

    return 0;
}

byte mbc_read(struct mbc* mbc, const word addr) {
    return (null_mbc_check(mbc) == false) ? mbc->iface->read(mbc->ctx, addr) : 0xFF;
}

void mbc_write(struct mbc* mbc, const word addr, const byte value) {
    if (null_mbc_check(mbc) == false) { mbc->iface->write(mbc->ctx, addr, value); }
    return;
}

void mbc_cleanup(struct mbc** p_mbc) {

    if (p_mbc == NULL || *p_mbc == NULL) {
        log_warn("No MBC to cleanup");
        return;
    }

    struct mbc* mbc = *p_mbc;
    if (mbc->iface != NULL) { mbc->iface->destroy(&mbc->ctx); }
    reset_mbc(mbc); // Unnecessary
    free(mbc);
    *p_mbc = NULL;

    return;
}
