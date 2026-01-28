/**
 * @file int_test_mbc.c
 * @brief Contains comprehensive tests for verifying the integration of the various Memory Bank
 * Controller implementations with their parent abstract type.
 */
#include <stdbool.h>

#include "memory/cartridge/mbc/dld.h"
#include "memory/cartridge/mbc/mbc.h"
#include "memory/cartridge/mbc/mbc1.h"
#include "memory/cartridge/mbc/structs/dld_context.h"
#include "memory/cartridge/mbc/structs/mbc.h"
#include "memory/cartridge/mbc/structs/mbc1_context.h"
#include "memory/memory_map.h"
#include "testing.h"

LOG_MODULE_SETUP_DEFAULT("INT TEST MBC");

static bool test_cartridge_module_integration(void);

static bool test_dld_integration(void);
static bool test_dld_rom_only(void);
static bool test_dld_rom_ram(void);
static bool test_dld_rom_ram_battery(void);
static bool test_dld_read_write(void);
static bool test_dld_invalid(void);

static bool test_mbc1_integration(void);
static bool test_mbc1_rom_only(void);
static bool test_mbc1_ram(void);
static bool test_mbc1_ram_battery(void);
static bool test_mbc1_read_write(void);
static bool test_mbc1_invalid(void);

static bool test_iface_eq(
    const struct mbc_iface* returned_iface,
    const struct mbc_iface* expected_iface
);

int main(void) { return (test_cartridge_module_integration() == true) ? 0 : 1; }

static bool test_cartridge_module_integration(void) {
    ASSERT_TRUE(test_dld_integration(), "DLD Integration Test(s) Failed");
    ASSERT_TRUE(test_mbc1_integration(), "MBC1 Integration Test(s) Failed");
    return true;
}

static bool test_dld_integration(void) {
    ASSERT_TRUE(test_dld_rom_only(), "Test DLD ROM Only Failed");
    ASSERT_TRUE(test_dld_rom_ram(), "Test DLD ROM RAM Failed");
    ASSERT_TRUE(test_dld_rom_ram_battery(), "Test DLD ROM RAM BATTERY Failed");
    ASSERT_TRUE(test_dld_read_write(), "Test DLD Read Write Failed");
    ASSERT_TRUE(test_dld_invalid(), "Test DLD Invalid Failed");
    return true;
}

static bool test_dld_rom_only(void) {
    struct mbc* mbc = mbc_create(CART_ROM_ONLY, ROM_32_KiB, RAM_NONE);
    ASSERT_NOT_NULL(mbc, "Failed to create valid MBC");
    ASSERT_NOT_NULL(mbc->iface, "Failed to set interface");
    ASSERT_NOT_NULL(mbc->ctx, "Failed to set Context");

    // Validate interface was correctly set
    ASSERT_TRUE(test_iface_eq(mbc->iface, dld_iface()), "DLD Interface mismatch");

    // Validate context
    struct dld_context* dld_ctx = mbc->ctx;
    ASSERT_NOT_NULL(dld_ctx->rom, "Expected ROM");
    ASSERT_EQ(dld_ctx->rom_size, CART_ROM_BANK_SIZE * 2, "Expected ROM size of 32 KiB");
    ASSERT_NULL(dld_ctx->ram, "Expected no RAM");
    ASSERT_EQ(dld_ctx->ram_size, 0, "Expected RAM size of 0");

    mbc_cleanup(&mbc);
    ASSERT_NULL(mbc, "Expected NULL");

    return true;
}

static bool test_dld_rom_ram(void) {
    struct mbc* mbc = mbc_create(CART_ROM_RAM, ROM_32_KiB, RAM_8_KiB);
    ASSERT_NOT_NULL(mbc, "Failed to create valid MBC");
    ASSERT_NOT_NULL(mbc->iface, "Failed to set interface");
    ASSERT_NOT_NULL(mbc->ctx, "Failed to set Context");

    // Validate interface was correctly set
    ASSERT_TRUE(test_iface_eq(mbc->iface, dld_iface()), "DLD Interface mismatch");

    // Validate context
    struct dld_context* dld_ctx = mbc->ctx;
    ASSERT_NOT_NULL(dld_ctx->rom, "Expected ROM");
    ASSERT_EQ(dld_ctx->rom_size, CART_ROM_BANK_SIZE * 2, "Expected ROM size of 32 KiB");
    ASSERT_NOT_NULL(dld_ctx->ram, "Expected RAM");
    ASSERT_EQ(dld_ctx->ram_size, CART_RAM_BANK_SIZE, "Expected RAM size of 8 KiB");

    mbc_cleanup(&mbc);
    ASSERT_NULL(mbc, "Expected NULL");

    return true;
}

static bool test_dld_rom_ram_battery(void) {
    struct mbc* mbc = mbc_create(CART_ROM_RAM_BATTERY, ROM_32_KiB, RAM_8_KiB);
    ASSERT_NOT_NULL(mbc, "Failed to create valid MBC");
    ASSERT_NOT_NULL(mbc->iface, "Failed to set interface");
    ASSERT_NOT_NULL(mbc->ctx, "Failed to set Context");

    // Validate interface was correctly set
    ASSERT_TRUE(test_iface_eq(mbc->iface, dld_iface()), "DLD Interface mismatch");

    // Validate context
    struct dld_context* dld_ctx = mbc->ctx;
    ASSERT_NOT_NULL(dld_ctx->rom, "Expected ROM");
    ASSERT_EQ(dld_ctx->rom_size, CART_ROM_BANK_SIZE * 2, "Expected ROM size of 32 KiB");
    ASSERT_NOT_NULL(dld_ctx->ram, "Expected RAM");
    ASSERT_EQ(dld_ctx->ram_size, CART_RAM_BANK_SIZE, "Expected RAM size of 8 KiB");

    mbc_cleanup(&mbc);
    ASSERT_NULL(mbc, "Expected NULL");

    return true;
}

static bool test_dld_read_write(void) {
    // Setup with 2 ROM banks and 1 RAM bank
    struct mbc* mbc = mbc_create(CART_ROM_RAM, ROM_32_KiB, RAM_8_KiB);
    ASSERT_NOT_NULL(mbc, "Failed to create MBC");

    struct dld_context* dld_ctx      = (struct dld_context*)mbc->ctx;
    const byte          test_pattern = 0xAA;
    const byte          seed_value   = 0x55;

    // Test ROM: Verify it is Read-Only
    // Seed a value directly to bypass mbc_load_rom for this specific check
    const word rom_addr             = 0x1000;
    const word internal_rom_addr    = rom_addr - ADDR_CART_ROM_START;
    dld_ctx->rom[internal_rom_addr] = seed_value;
    mbc_write(mbc, rom_addr, test_pattern); // Attempt to overwrite
    byte returned_value = dld_ctx->rom[internal_rom_addr];
    ASSERT_EQ(
        returned_value,
        seed_value,
        "MBC ROM Write Violated ROM Immutability: Expected 0x%2X but got 0x%2X",
        seed_value,
        returned_value
    );
    returned_value = mbc_read(mbc, rom_addr);
    ASSERT_EQ(
        returned_value,
        seed_value,
        "MBC ROM Read Failed: Expected 0x%2X but got 0x%2X",
        seed_value,
        returned_value
    );

    // Test RAM: Verify Read-Write functionality
    // Game Boy Cart RAM starts at 0xA000
    const word ram_addr             = 0xA000;
    const word internal_ram_addr    = ram_addr - ADDR_CART_RAM_START;
    dld_ctx->ram[internal_ram_addr] = seed_value;
    mbc_write(mbc, ram_addr, test_pattern);
    returned_value = dld_ctx->ram[internal_ram_addr];
    ASSERT_EQ(
        returned_value,
        test_pattern,
        "MBC RAM Write Failed: Expected 0x%2X but got 0x%2X",
        test_pattern,
        returned_value
    );
    returned_value = mbc_read(mbc, ram_addr);
    ASSERT_EQ(
        returned_value,
        test_pattern,
        "MBC RAM Read Failed: Expected 0x%2X but got 0x%2X",
        test_pattern,
        returned_value
    );

    // Test Undefined Read functionality
    const word test_addr = ADDR_VRAM_START;
    returned_value       = mbc_read(mbc, test_addr);
    ASSERT_EQ(
        returned_value,
        0xFF,
        "MBC Undefined Read Failed: Expected 0xFF but of 0x%2X",
        returned_value
    );

    mbc_cleanup(&mbc);
    return true;
}

static bool test_dld_invalid(void) {
    // ROM ONLY (DLD)
    // ==========
    // ROM ONLY (DLD) must contain exactly 2 ROM banks
    // --------------------
    struct mbc* mbc = mbc_create(CART_ROM_ONLY, ROM_64_KiB, RAM_NONE);
    ASSERT_NULL(mbc, "Expected no MBC");
    // --------------------

    // ROM ONLY (DLD) Can't contain any RAM banks
    // --------------------
    mbc = mbc_create(CART_ROM_ONLY, ROM_32_KiB, RAM_8_KiB);
    ASSERT_NULL(mbc, "Expected no MBC");
    // --------------------
    // ==========

    // ROM RAM / ROM RAM BATTERY (DLD)
    // ==========
    enum cartridge_type cart_types[] = {CART_ROM_RAM, CART_ROM_RAM_BATTERY};
    for (int i = 0; i < 2; i++) {
        // ROM RAM / ROM RAM BATTERY (DLD) must contain exactly 2 ROM banks
        // --------------------
        mbc = mbc_create(cart_types[i], ROM_64_KiB, RAM_8_KiB);
        ASSERT_NULL(mbc, "Expected no MBC");
        // --------------------

        // ROM RAM / ROM RAM BATTERY (DLD) Must contain exactly 1 ROM Bank
        // --------------------
        mbc = mbc_create(cart_types[i], ROM_32_KiB, RAM_NONE);
        ASSERT_NULL(mbc, "Expected no MBC");

        mbc = mbc_create(cart_types[i], ROM_32_KiB, RAM_UNUSED);
        ASSERT_NULL(mbc, "Expected no MBC");

        mbc = mbc_create(cart_types[i], ROM_32_KiB, RAM_32_KiB);
        // --------------------
    }
    // ==========

    return true;
}

static bool test_mbc1_integration(void) {
    ASSERT_TRUE(test_mbc1_rom_only(), "Test MBC1 ROM Only Failed");
    ASSERT_TRUE(test_mbc1_ram(), "Test MBC1 RAM Failed");
    ASSERT_TRUE(test_mbc1_ram_battery(), "Test MBC1 RAM_BATTERY Failed");
    ASSERT_TRUE(test_mbc1_read_write(), "Test MBC1 Read Write Failed");
    ASSERT_TRUE(test_mbc1_invalid(), "Test MBC1 Invalid Failed");
    return true;
}

static bool test_mbc1_rom_only(void) {
    struct mbc* mbc = mbc_create(CART_MBC1, ROM_512_KiB, RAM_NONE);
    ASSERT_NOT_NULL(mbc, "Failed to create valid MBC");
    ASSERT_NOT_NULL(mbc->iface, "Failed to set interface");
    ASSERT_NOT_NULL(mbc->ctx, "Failed to set Context");

    // Validate interface was correctly set
    ASSERT_TRUE(test_iface_eq(mbc->iface, mbc1_iface()), "MBC1 Interface mismatch");

    // Validate context
    struct mbc1_context* mbc1_ctx = mbc->ctx;
    ASSERT_NOT_NULL(mbc1_ctx->rom, "Expected ROM");
    ASSERT_EQ(
        mbc1_ctx->rom_size,
        CART_ROM_BANK_SIZE * rom_size_bank_count(ROM_512_KiB),
        "Expected ROM size of %s",
        rom_size_to_string(ROM_512_KiB)
    );
    ASSERT_NULL(mbc1_ctx->ram, "Expected no RAM");
    ASSERT_EQ(mbc1_ctx->ram_size, 0, "Expected RAM size of 0");

    mbc_cleanup(&mbc);
    ASSERT_NULL(mbc, "Expected NULL");

    return true;
}

static bool test_mbc1_ram(void) {
    struct mbc* mbc = mbc_create(CART_MBC1_RAM, ROM_512_KiB, RAM_32_KiB);
    ASSERT_NOT_NULL(mbc, "Failed to create valid MBC");
    ASSERT_NOT_NULL(mbc->iface, "Failed to set interface");
    ASSERT_NOT_NULL(mbc->ctx, "Failed to set Context");

    // Validate interface was correctly set
    ASSERT_TRUE(test_iface_eq(mbc->iface, mbc1_iface()), "MBC1 Interface mismatch");

    // Validate context
    struct mbc1_context* mbc1_ctx = mbc->ctx;
    ASSERT_NOT_NULL(mbc1_ctx->rom, "Expected ROM");
    ASSERT_EQ(
        mbc1_ctx->rom_size,
        CART_ROM_BANK_SIZE * rom_size_bank_count(ROM_512_KiB),
        "Expected ROM size of %s",
        rom_size_to_string(ROM_512_KiB)
    );
    ASSERT_NOT_NULL(mbc1_ctx->ram, "Expected RAM");
    ASSERT_EQ(
        mbc1_ctx->ram_size,
        CART_RAM_BANK_SIZE * ram_size_bank_count(RAM_32_KiB),
        "Expected RAM size of %s",
        ram_size_to_string(RAM_32_KiB)
    );

    mbc_cleanup(&mbc);
    ASSERT_NULL(mbc, "Expected NULL");

    return true;
}

static bool test_mbc1_ram_battery(void) {
    struct mbc* mbc = mbc_create(CART_MBC1_RAM_BATTERY, ROM_512_KiB, RAM_32_KiB);
    ASSERT_NOT_NULL(mbc, "Failed to create valid MBC");
    ASSERT_NOT_NULL(mbc->iface, "Failed to set interface");
    ASSERT_NOT_NULL(mbc->ctx, "Failed to set Context");

    // Validate interface was correctly set
    ASSERT_TRUE(test_iface_eq(mbc->iface, mbc1_iface()), "MBC1 Interface mismatch");

    // Validate context
    struct mbc1_context* mbc1_ctx = mbc->ctx;
    ASSERT_NOT_NULL(mbc1_ctx->rom, "Expected ROM");
    ASSERT_EQ(
        mbc1_ctx->rom_size,
        CART_ROM_BANK_SIZE * rom_size_bank_count(ROM_512_KiB),
        "Expected ROM size of %s",
        rom_size_to_string(ROM_512_KiB)
    );
    ASSERT_NOT_NULL(mbc1_ctx->ram, "Expected RAM");
    ASSERT_EQ(
        mbc1_ctx->ram_size,
        CART_RAM_BANK_SIZE * ram_size_bank_count(RAM_32_KiB),
        "Expected RAM size of %s",
        ram_size_to_string(RAM_32_KiB)
    );

    mbc_cleanup(&mbc);
    ASSERT_NULL(mbc, "Expected NULL");

    return true;
}

static bool test_mbc1_read_write(void) {
    struct mbc* mbc = mbc_create(CART_MBC1_RAM, ROM_512_KiB, RAM_32_KiB);
    ASSERT_NOT_NULL(mbc, "Failed to create MBC");

    struct mbc1_context* mbc1_ctx      = (struct mbc1_context*)mbc->ctx;
    const byte           test_pattern  = 0xAA;
    const byte           seed_values[] = {0x01 ^ 0xFF, 0x02 ^ 0xFF};

    const word addr_reg_ram_enabled  = 0x0000;
    const word addr_reg_rom_bank_num = 0x2000;
    const word addr_reg_ram_bank_num = 0x4000;
    const word addr_reg_banking_mode = 0x6000;

    byte returned_value;

    // Setup Registers
    mbc_write(mbc, addr_reg_ram_enabled, 0x0A);
    ASSERT_EQ(mbc1_ctx->ram_enable, 0x0A, "Failed to set RAM Enable to 0x0A");
    mbc_write(mbc, addr_reg_rom_bank_num, 0x01);
    ASSERT_EQ(mbc1_ctx->rom_bank_number, 0x01, "Failed to set ROM Bank Number to 0x01");
    mbc_write(mbc, addr_reg_ram_bank_num, 0x00);
    ASSERT_EQ(mbc1_ctx->ram_bank_number, 0x00, "Failed to set RAM Bank Number to 0x00");
    mbc_write(mbc, addr_reg_banking_mode, 0x01);
    ASSERT_EQ(mbc1_ctx->banking_mode, 0x01, "Failed to set Banking Mode to 0x01");

    // Test ROM: Verify it is Read-Only and that banking works
    const word addr_rom_high       = ADDR_CART_ROM_HIGH_START;
    const word addr_rom_internal[] = {
        addr_rom_high - ADDR_CART_ROM_START,
        addr_rom_high - ADDR_CART_ROM_START + CART_ROM_BANK_SIZE
    };
    for (int i = 0; i < 2; i++) {
        mbc1_ctx->rom[addr_rom_internal[i]] = seed_values[i];
        mbc_write(mbc, addr_rom_high, test_pattern);
        returned_value = mbc1_ctx->rom[addr_rom_internal[i]];
        ASSERT_EQ(
            returned_value,
            seed_values[i],
            "MBC ROM Write Violated ROM Immutability: Expected 0x%2X but got 0x%2X",
            seed_values[i],
            returned_value
        );
        returned_value = mbc_read(mbc, addr_rom_high);
        ASSERT_EQ(
            returned_value,
            seed_values[i],
            "MBC ROM Read Failed: Expected 0x%2X but got 0x%2X",
            seed_values[i],
            returned_value
        );
        mbc_write(mbc, addr_reg_rom_bank_num, 0x02);
        ASSERT_EQ(mbc1_ctx->rom_bank_number, 0x02, "Failed to set ROM bank number to 0x02");
    }

    // Ensure Registers weren't overridden by accident
    mbc_write(mbc, addr_reg_ram_enabled, 0x0A);
    ASSERT_EQ(mbc1_ctx->ram_enable, 0x0A, "Failed to set RAM Enable to 0x0A");
    mbc_write(mbc, addr_reg_rom_bank_num, 0x01);
    ASSERT_EQ(mbc1_ctx->rom_bank_number, 0x01, "Failed to set ROM Bank Number to 0x01");
    mbc_write(mbc, addr_reg_ram_bank_num, 0x00);
    ASSERT_EQ(mbc1_ctx->ram_bank_number, 0x00, "Failed to set RAM Bank Number to 0x00");
    mbc_write(mbc, addr_reg_banking_mode, 0x01);
    ASSERT_EQ(mbc1_ctx->banking_mode, 0x01, "Failed to set Banking Mode to 0x01");

    // Test RAM: Verify Read-Write functionality
    const word addr_ram            = ADDR_CART_RAM_START;
    const word addr_ram_internal[] = {
        addr_ram - ADDR_CART_RAM_START, addr_ram - ADDR_CART_RAM_START + CART_RAM_BANK_SIZE
    };
    for (int i = 0; i < 2; i++) {
        mbc1_ctx->ram[addr_ram_internal[i]] = seed_values[i];
        mbc_write(mbc, addr_ram, test_pattern);
        returned_value = mbc1_ctx->ram[addr_ram_internal[i]];
        ASSERT_EQ(
            returned_value,
            test_pattern,
            "MBC RAM Write Failed: Expected 0x%2X but got 0x%2X",
            test_pattern,
            returned_value
        );
        returned_value = mbc_read(mbc, addr_ram);
        ASSERT_EQ(
            returned_value,
            test_pattern,
            "MBC RAM Read Failed: Expected 0x%2X but got 0x%2X",
            test_pattern,
            returned_value
        );
        mbc_write(mbc, addr_reg_ram_bank_num, 0x01);
    }

    // Test Undefined Read
    returned_value = mbc_read(mbc, ADDR_VRAM_START);
    ASSERT_EQ(
        returned_value,
        0xFF,
        "MBC Undefined Read Failed: Expected 0xFF but of 0x%2X",
        returned_value
    );

    mbc_cleanup(&mbc);
    return true;
}

static bool test_mbc1_invalid(void) {
    // ----- ROM ONLY (MBC1) -----
    // Cannot contain more than 2 MiB of ROM
    struct mbc* mbc = mbc_create(CART_MBC1, ROM_4_MiB, RAM_NONE);
    ASSERT_NULL(mbc, "Expected no MBC");

    // Cannot contain any ram
    mbc = mbc_create(CART_MBC1, ROM_512_KiB, RAM_8_KiB);
    ASSERT_NULL(mbc, "Expected no MBC");
    // -----

    // ----- ROM RAM (MBC1) -----
    enum cartridge_type cart_types[] = {CART_MBC1_RAM, CART_MBC1_RAM_BATTERY};
    for (int i = 0; i < 2; i++) {
        // ROM RAM / ROM RAM BATTERY (MBC1) cannot contain more than 2 MiB of ROM
        mbc = mbc_create(cart_types[i], ROM_4_MiB, RAM_8_KiB);
        ASSERT_NULL(mbc, "Expected no MBC");

        // ROM RAM / ROM RAM BATTER (MBC1) Must contain at least 8 KiB of RAM
        mbc = mbc_create(cart_types[i], ROM_512_KiB, RAM_NONE);
        ASSERT_NULL(mbc, "Expected no MBC");
        mbc = mbc_create(cart_types[i], ROM_512_KiB, RAM_UNUSED);
        ASSERT_NULL(mbc, "Expected no MBC");

        // ROM RAM / ROM RAM BATTERY (MBC1) if ROM <= 512 KiB, RAM must be < 32 KiB
        mbc = mbc_create(cart_types[i], ROM_512_KiB, RAM_64_KiB);
        ASSERT_NULL(mbc, "Expected no MBC");

        // ROM RAM / ROM RAM BATTERY (MBC1) if ROM > 512 KiB, RAM must be 8 KiB
        mbc = mbc_create(cart_types[i], ROM_2_MiB, RAM_32_KiB);
        ASSERT_NULL(mbc, "Expected no MBC");
    }

    return true;
}

static bool test_iface_eq(
    const struct mbc_iface* returned_iface,
    const struct mbc_iface* expected_iface
) {
    ASSERT_EQ(returned_iface->rom, expected_iface->rom, "rom Function Pointer not set correctly");
    ASSERT_EQ(returned_iface->ram, expected_iface->ram, "ram Function Pointer not set correctly");
    ASSERT_EQ(
        returned_iface->read, expected_iface->read, "read Function Pointer not set correctly"
    );
    ASSERT_EQ(
        returned_iface->write, expected_iface->write, "write Function Pointer not set correctly"
    );
    ASSERT_EQ(
        returned_iface->destroy,
        expected_iface->destroy,
        "destroy Function Pointer not set correctly"
    );
    return true;
}
