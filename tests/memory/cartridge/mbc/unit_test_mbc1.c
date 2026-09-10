/**
 * @file unit_test_mbc1.c
 * @brief Comprehensive Unit tests for the MBC1 Memory Bank Controller.
 */
#include <stdlib.h>

#include "DEFINES.h"
#include "memory/cartridge/mbc/mbc1.h"
#include "memory/cartridge/mbc/structs/mbc1_context.h"
#include "memory/cartridge/ram_size.h"
#include "memory/cartridge/rom_size.h"
#include "memory/memory_map.h"
#include "testing.h"

LOG_MODULE_SETUP_DEFAULT("UNIT TEST MBC1");

static const struct mbc_iface* fixture_iface;
static struct mbc1_context*    fixture_ctx;

static bool test_mbc1_module(void);

static bool test_instanciation(void);
static bool test_instantiate_standard(void);
static bool test_instantiate_alternative(void);
static bool test_instantiate_invalid(void);
static bool test_instantiate(const enum rom_size rom_size, const enum ram_size ram_size);

static bool test_interface(void);
static bool test_interface_get(void);
static bool test_interface_rom(void);
static bool test_interface_ram(void);
static bool test_interface_read(void);
static bool test_interface_write(void);
static bool test_interface_destroy(void);

static bool test_banking(void);
static bool test_banking_rom(void);
static bool test_banking_ram(void);

static bool fuzz_rom(const enum rom_size r_size);
static bool fuzz_ram(const enum ram_size r_size);

static void setup_clear_fixtures(void);
static int  setup_new_fixtures(const enum rom_size rom_size, const enum ram_size ram_size);

int main(void) {
    fixture_iface = NULL;
    fixture_ctx   = NULL;

    // When main returns 0, that indicates a success
    return (test_mbc1_module() == true) ? 0 : 1;
}

static bool test_mbc1_module(void) {
    ASSERT_TRUE(test_instanciation(), "MBC1 Instancation Test(s) Failed");
    ASSERT_TRUE(test_interface(), "MBC1 Interface Test(s) Failed");
    ASSERT_TRUE(test_banking(), "MBC1 Banking Test(s) Failed");
    return true;
}

static bool test_instanciation(void) {
    ASSERT_TRUE(test_instantiate_standard(), "Test Instantiate Standard Failed");
    ASSERT_TRUE(test_instantiate_alternative(), "Test Instantiate Alternative Failed");
    ASSERT_TRUE(test_instantiate_invalid(), "Test Innstancate with invalid parameters Failed");
    return true;
}

static bool test_instantiate_standard(void) {
    enum rom_size rom_sizes[]     = {ROM_32_KiB, ROM_64_KiB, ROM_128_KiB, ROM_256_KiB, ROM_512_KiB};
    size_t        rom_size_nums[] = {32, 64, 128, 256, 512};
    int           rom_size_elements = sizeof(rom_sizes) / sizeof(rom_sizes[0]);
    enum ram_size ram_sizes[]       = {RAM_NONE, RAM_UNUSED, RAM_8_KiB, RAM_32_KiB};
    size_t        ram_size_nums[]   = {0, 0, 8, 32};
    int           ram_size_elements = sizeof(ram_sizes) / sizeof(ram_sizes[0]);

    setup_clear_fixtures();

    // Iterate over all possible ROM sizes
    for (int i = 0; i < rom_size_elements; i++) {
        // Iterate over all possible RAM sizes
        for (int j = 0; j < ram_size_elements; j++) {
            ASSERT_TRUE(
                test_instantiate(rom_sizes[i], ram_sizes[j]),
                "Failed to create MBC1:\n"
                "\tROM: %.3zu [KiB]\n"
                "\tRAM: %.3zu [KiB]",
                rom_size_nums[i],
                ram_size_nums[j]
            );
        }
    }

    return true;
}

static bool test_instantiate_alternative(void) {
    enum rom_size rom_sizes[]       = {ROM_1_MiB, ROM_2_MiB};
    size_t        rom_size_nums[]   = {1, 2};
    int           rom_size_elements = sizeof(rom_sizes) / sizeof(rom_sizes[0]);
    enum ram_size ram_sizes[]       = {RAM_NONE, RAM_UNUSED, RAM_8_KiB};
    size_t        ram_size_nums[]   = {0, 0, 8};
    int           ram_size_elements = sizeof(ram_sizes) / sizeof(ram_sizes[0]);

    setup_clear_fixtures();

    // Iterate over all possible ROM sizes
    for (int i = 0; i < rom_size_elements; i++) {
        // Iterate over all possible RAM sizes
        for (int j = 0; j < ram_size_elements; j++) {
            ASSERT_TRUE(
                test_instantiate(rom_sizes[i], ram_sizes[j]),
                "Failed to create MBC1:\n"
                "\tROM: %.1zu [MiB]\n"
                "\tRAM: %.1zu [KiB]",
                rom_size_nums[i],
                ram_size_nums[j]
            );
        }
    }

    return true;
}

static bool test_instantiate_invalid(void) {
    setup_clear_fixtures();

    // No ROM
    fixture_ctx = mbc1_instantiate(0, 4);
    ASSERT_NULL(fixture_ctx, "instantiated MBC1 context with no ROM. Should have failed");

    // Can't create with one 1 ROM bank (2 minimum)
    fixture_ctx = mbc1_instantiate(1, 4);
    ASSERT_NULL(fixture_ctx, "instantiated MBC1 context with only 1 ROM Bank. Should have failed");

    // Too much RAM for standard
    fixture_ctx = mbc1_instantiate(32, 8);
    ASSERT_NULL(
        fixture_ctx, "instantiated MBC1 context with more RAM than supported. Should have failed "
    );

    // Too much RAM for alternative
    fixture_ctx = mbc1_instantiate(128, 2);
    ASSERT_NULL(
        fixture_ctx, "instantiated MBC1 context with more RAM than supported. Should have failed "
    );

    // Too much ROM
    fixture_ctx = mbc1_instantiate(256, 1);
    ASSERT_NULL(
        fixture_ctx, "instantiated MBC1 context with more ROM than supported. Should have failed "
    );

    return true;
}

static bool test_instantiate(const enum rom_size rom_size, const enum ram_size ram_size) {
    size_t n_rom_banks       = rom_size_bank_count(rom_size);
    size_t n_ram_banks       = ram_size_bank_count(ram_size);
    size_t expected_rom_size = n_rom_banks * CART_ROM_BANK_SIZE;
    size_t expected_ram_size = n_ram_banks * CART_RAM_BANK_SIZE;
    size_t expected_rom_mask = n_rom_banks - 1;
    size_t expected_ram_mask = (n_ram_banks == 0) ? 0 : n_ram_banks - 1;

    setup_clear_fixtures();

    // Create MBC1
    fixture_ctx = mbc1_instantiate(n_rom_banks, n_ram_banks);
    ASSERT_NOT_NULL(fixture_ctx, "Failed to instancate MBC1");

    // Verify ROM
    ASSERT_NOT_NULL(fixture_ctx->rom, "MBC1 context created without ROM");
    ASSERT_EQ(
        fixture_ctx->rom_size,
        expected_rom_size,
        "Expected ROM size of %zu KiB",
        expected_rom_size / 1000
    );
    ASSERT_EQ(
        fixture_ctx->rom_bank_mask,
        expected_rom_mask,
        "Expected 0x%.2X for ROM Bank Mask",
        expected_rom_mask
    );

    // Verify RAM
    if (n_ram_banks == 0) {
        ASSERT_NULL(fixture_ctx->ram, "MBC1 context created with RAM instead of without");
    } else {
        ASSERT_NOT_NULL(fixture_ctx->ram, "MBC1 context created without RAM instead of with");
        ASSERT_EQ(
            fixture_ctx->ram_size,
            expected_ram_size,
            "Expected ROM size of %zu KiB",
            expected_ram_size / 1000
        );
        ASSERT_EQ(
            fixture_ctx->ram_bank_mask,
            expected_ram_mask,
            "Expected 0x%.2X for RAM Bank Mask",
            expected_ram_mask
        );
    }

    // Verify Cleanup
    mbc1_destroy((void**)&fixture_ctx);
    ASSERT_NULL(fixture_ctx, "MBC1 destroy failed to set context pointer to NULL");

    return true;
}

static bool test_interface(void) {
    ASSERT_TRUE(test_interface_get(), "Test Interface Get Failed");
    ASSERT_TRUE(test_interface_rom(), "Test Interface ROM Failed");
    ASSERT_TRUE(test_interface_ram(), "Test Interface RAM Failed");
    ASSERT_TRUE(test_interface_read(), "Test Interface Read Failed");
    ASSERT_TRUE(test_interface_write(), "Test Interface Write Failed");
    ASSERT_TRUE(test_interface_destroy(), "Test Interface Destroy Failed");
    return true;
}

static bool test_interface_get(void) {
    setup_clear_fixtures();

    fixture_iface = mbc1_iface();
    ASSERT_NOT_NULL(fixture_iface, "Failed to get interface");
    ASSERT_NOT_NULL(fixture_iface->rom, "Interface does not define rom()");
    ASSERT_NOT_NULL(fixture_iface->ram, "Interface does not define ram()");
    ASSERT_NOT_NULL(fixture_iface->read, "Interface does not define read()");
    ASSERT_NOT_NULL(fixture_iface->write, "Interface does not define write()");
    ASSERT_NOT_NULL(fixture_iface->destroy, "Interface does not define destroy()");

    return true;
}

static bool test_interface_rom(void) {
    setup_new_fixtures(ROM_512_KiB, RAM_32_KiB);
    ASSERT_NOT_NULL(fixture_ctx, "Failed to create MBC1");

    size_t size = 0;
    byte*  rom  = fixture_iface->rom(fixture_ctx, &size);

    ASSERT_EQ(rom, fixture_ctx->rom, "Returned ROM pointer did not match internal ROM pointer");
    ASSERT_EQ(size, fixture_ctx->rom_size, "Returned ROM size did not match internal ROM size");

    return true;
}

static bool test_interface_ram(void) {
    setup_new_fixtures(ROM_512_KiB, RAM_32_KiB);
    ASSERT_NOT_NULL(fixture_ctx, "Failed to create MBC1");

    size_t size = 0;
    byte*  ram  = fixture_iface->ram(fixture_ctx, &size);

    ASSERT_EQ(ram, fixture_ctx->ram, "Returned RAM pointer did not match internal RAM pointer");
    ASSERT_EQ(size, fixture_ctx->ram_size, "Returned RAM size did not match internal RAM size");

    return true;
}

static bool test_interface_read(void) {
    setup_new_fixtures(ROM_512_KiB, RAM_32_KiB);
    ASSERT_NOT_NULL(fixture_ctx, "Failed to create MBC1");
    fixture_ctx->ram_enable      = 0x00;
    fixture_ctx->rom_bank_number = 0x01;
    fixture_ctx->ram_bank_number = 0x00;
    fixture_ctx->banking_mode    = 0x00;

    const word addr_rom          = 0x0000;
    const word addr_ram          = 0xA000;
    const byte seed_value_rom    = 0xAA;
    const byte seed_value_ram    = 0x55;
    const byte expected_bad_read = 0xFF;

    // Seed value into ROM and RAM
    fixture_ctx->rom[addr_rom]                       = seed_value_rom;
    fixture_ctx->ram[addr_ram - ADDR_CART_RAM_START] = seed_value_ram;

    // Verify ROM read
    byte result = fixture_iface->read(fixture_ctx, addr_rom);
    ASSERT_EQ(
        result,
        seed_value_rom,
        "Failed to read ROM. Expected 0x%.2X but got 0x%.2X",
        seed_value_rom,
        result
    );

    // Verify Disabled RAM read
    fixture_ctx->ram_enable = 0x00;
    result                  = fixture_iface->read(fixture_ctx, addr_ram);
    ASSERT_EQ(
        result,
        expected_bad_read,
        "Failed to read Disabled RAM. Expected 0x%.2X but got 0x%.2X",
        expected_bad_read,
        result
    );

    // Verify Enabled RAM read
    fixture_ctx->ram_enable = 0x0A;
    result                  = fixture_iface->read(fixture_ctx, addr_ram);
    ASSERT_EQ(
        result,
        seed_value_ram,
        "Failed to read Enabled RAM. Expected 0x%.2X but got 0x%.2X",
        seed_value_ram,
        result
    );

    // Attempt out of bounds reads
    result = fixture_iface->read(fixture_ctx, ADDR_VRAM_START);
    ASSERT_EQ(
        result,
        expected_bad_read,
        "Bad read returned incorrect value. Expected 0x%.2X but got 0x%.2X",
        expected_bad_read,
        result
    );

    return true;
}

static bool test_interface_write(void) {
    setup_new_fixtures(ROM_512_KiB, RAM_32_KiB);
    ASSERT_NOT_NULL(fixture_ctx, "Failed to create MBC1");
    fixture_ctx->ram_enable      = 0x00;
    fixture_ctx->rom_bank_number = 0x01;
    fixture_ctx->ram_bank_number = 0x00;
    fixture_ctx->banking_mode    = 0x00;

    const word addr_rom          = 0x0000;
    const word addr_ram          = 0xA000;
    const byte seed_value_rom    = 0xAA;
    const byte seed_value_ram    = 0x55;
    const byte write_value       = 0xCC;
    const byte expected_bad_read = 0xFF;

    // Seed value into ROM and RAM
    fixture_ctx->rom[addr_rom]                       = seed_value_rom;
    fixture_ctx->ram[addr_ram - ADDR_CART_RAM_START] = seed_value_ram;

    // Verify ROM Write fails
    fixture_iface->write(fixture_ctx, addr_rom, write_value);
    ASSERT_EQ(
        fixture_ctx->rom[addr_rom],
        seed_value_rom,
        "Wrote to ROM. Should have failed. Expected 0x%.2X but got 0x%.2X",
        seed_value_rom,
        fixture_ctx->rom[addr_rom]
    );

    // Verify Disabled RAM write fails
    fixture_ctx->ram_enable = 0x00;
    fixture_iface->write(fixture_ctx, addr_ram, write_value);
    ASSERT_EQ(
        fixture_ctx->ram[addr_ram - ADDR_CART_RAM_START],
        seed_value_ram,
        "Wrote to Disabled RAM. Should have failed. Expected 0x%.2X but got 0x%.2X",
        seed_value_ram,
        fixture_ctx->ram[addr_ram - ADDR_CART_RAM_START]
    );

    // Verify Enabled RAM write
    fixture_ctx->ram_enable = 0x0A;
    fixture_iface->write(fixture_ctx, addr_ram, write_value);
    ASSERT_EQ(
        fixture_ctx->ram[addr_ram - ADDR_CART_RAM_START],
        write_value,
        "Failed to write to enabled RAM. Expected 0x%.2X but got 0x%.2X",
        write_value,
        fixture_ctx->ram[addr_ram - ADDR_CART_RAM_START]
    );

    return true;
}

static bool test_interface_destroy(void) {
    setup_new_fixtures(ROM_512_KiB, RAM_32_KiB);
    ASSERT_NOT_NULL(fixture_ctx, "Failed to create MBC1");

    fixture_iface->destroy((void**)&fixture_ctx);
    ASSERT_NULL(fixture_ctx, "Destroy failed to NULL ctx");

    return true;
}

static bool test_banking(void) {
    ASSERT_TRUE(test_banking_rom(), "Test Banking ROM Failed");
    ASSERT_TRUE(test_banking_ram(), "Test Banking RAM Failed");
    return true;
}

static bool test_banking_rom(void) {
    enum rom_size rom_sizes[] = {
        ROM_32_KiB, ROM_64_KiB, ROM_128_KiB, ROM_256_KiB, ROM_512_KiB, ROM_1_MiB, ROM_2_MiB
    };

    const int elements = sizeof(rom_sizes) / sizeof(rom_sizes[0]);
    for (int i = 0; i < elements; i++) {
        ASSERT_TRUE(
            fuzz_rom(rom_sizes[i]),
            "Failed to bank ROM size \"%s\" correctly",
            rom_size_to_string(rom_sizes[i])
        );
    }

    return true;
}

static bool test_banking_ram(void) {
    enum ram_size ram_sizes[] = {RAM_8_KiB, RAM_32_KiB};

    const int elements = sizeof(ram_sizes) / sizeof(ram_sizes[0]);
    for (int i = 0; i < elements; i++) {
        ASSERT_TRUE(
            fuzz_ram(ram_sizes[i]),
            "Failed to bank ROM size \"%s\" correctly",
            ram_size_to_string(ram_sizes[i])
        );
    }

    return true;
}

static bool fuzz_rom(const enum rom_size r_size) {
    const word addr_rom_low          = ADDR_CART_ROM_LOW_START;
    const word addr_rom_high         = ADDR_CART_ROM_HIGH_START;
    const word addr_reg_rom_bank_num = 0x2000;
    const word addr_reg_ram_bank_num = 0x4000;
    const word addr_reg_banking_mode = 0x6000;

    setup_new_fixtures(r_size, RAM_NONE);
    ASSERT_NOT_NULL(fixture_ctx, "Failed to create MBC1");

    // Seed ONLY the physical memory we actually have
    int phys_banks = rom_size_bank_count(r_size);
    for (int i = 0; i < phys_banks; i++) {
        fixture_ctx->rom[i * CART_ROM_BANK_SIZE] = (byte)(i ^ 0xFF);
    }

    // First test in Mode 0
    // ROM low should be locked to bank 00
    // ROM high is only banked with rom banking number
    // ROM high cannot be mapped to bank 00
    fixture_iface->write(fixture_ctx, addr_reg_banking_mode, 0x00);
    ASSERT_EQ(fixture_ctx->banking_mode, 0x00, "Failed to set banking mode to 0");

    // Iterate over ALL 128 possible banks
    for (int bank = 0; bank < 128; bank++) {
        // Select Bank
        fixture_iface->write(fixture_ctx, addr_reg_rom_bank_num, bank & 0x1F);
        ASSERT_EQ(
            fixture_ctx->rom_bank_number, (byte)(bank & 0x1F), "Failed to set ROM banking number"
        );
        fixture_iface->write(fixture_ctx, addr_reg_ram_bank_num, (bank >> 5) & 0x03);
        ASSERT_EQ(
            fixture_ctx->ram_bank_number, (bank >> 5) & 0x03, "Failed to set RAM banking number"
        );

        // Get expected values
        int requested_high = bank;
        if ((requested_high & 0x1F) == 0) { requested_high++; } // Quirk: 0->1
        int  masked_high       = requested_high % phys_banks;
        byte expected_high_val = (byte)(masked_high ^ 0xFF);
        byte expected_low_val  = (byte)(0x00 ^ 0xFF); // cant be anything other than bank 00

        // Verify
        ASSERT_EQ(
            fixture_iface->read(fixture_ctx, addr_rom_low),
            expected_low_val,
            "Remapped low ROM in mode 0: Bank %i on %i-bank ROM. Expected bank 0",
            bank,
            phys_banks
        );
        ASSERT_EQ(
            fixture_iface->read(fixture_ctx, addr_rom_high),
            expected_high_val,
            "Masking Error High in mode 0: Bank %i on %i-bank ROM. Expected bank %i",
            bank,
            phys_banks,
            masked_high
        );
    }

    // Test Mode 1
    // ROM low/high can both be remapped now
    fixture_iface->write(fixture_ctx, addr_reg_banking_mode, 0x01);
    ASSERT_EQ(fixture_ctx->banking_mode, 0x01, "Failed to set banking mode to 1");

    // Iterate over ALL 128 possible banks
    for (int bank = 0; bank < 128; bank++) {
        // Select bank
        // First 5 bits are written to ROM bank number
        // Last 2 bits are written to RAM bank number
        fixture_iface->write(fixture_ctx, addr_reg_rom_bank_num, bank & 0x1F);
        fixture_iface->write(fixture_ctx, addr_reg_ram_bank_num, (bank >> 5) & 0x03);

        // Get expected values
        int requested_high = bank;
        if ((requested_high & 0x1F) == 0) { requested_high++; } // Quirk: 0->1
        int  masked_high       = requested_high % phys_banks;
        byte expected_high_val = (byte)(masked_high ^ 0xFF);
        int  requested_low     = bank & 0x60;
        int  masked_low        = requested_low % phys_banks;
        byte expected_low_val  = (byte)(masked_low ^ 0xFF);

        // Verify
        ASSERT_EQ(
            fixture_iface->read(fixture_ctx, addr_rom_low),
            expected_low_val,
            "Masking Error Low: Bank %i on %i-bank ROM. Expected bank %i",
            bank,
            phys_banks,
            masked_low
        );

        ASSERT_EQ(
            fixture_iface->read(fixture_ctx, addr_rom_high),
            expected_high_val,
            "Masking Error High: Bank %i on %i-bank ROM. Expected bank %i",
            bank,
            phys_banks,
            masked_high
        );
    }

    return true;
}

static bool fuzz_ram(const enum ram_size r_size) {
    const word addr_ram              = ADDR_CART_RAM_START;
    const word addr_reg_ram_enabled  = 0x0000;
    const word addr_reg_ram_bank_num = 0x4000;
    const word addr_reg_banking_mode = 0x6000;

    setup_new_fixtures(ROM_512_KiB, r_size);
    ASSERT_NOT_NULL(fixture_ctx, "Failed to create MBC1");

    // Seed ONLY the physical memory we actually have
    int phys_banks = ram_size_bank_count(r_size);
    for (int i = 0; i < phys_banks; i++) {
        fixture_ctx->ram[i * CART_RAM_BANK_SIZE] = (byte)(i ^ 0xFF);
    }

    // Enable RAM
    // Functionality of the enable register is tested in a previous test
    fixture_iface->write(fixture_ctx, addr_reg_ram_enabled, 0x0A);
    ASSERT_EQ(fixture_ctx->ram_enable, 0x0A, "Failed to set RAM enable to 0x0A");

    // First test in Mode 0
    // RAM should be locked to bank 00
    fixture_iface->write(fixture_ctx, addr_reg_banking_mode, 0x00);
    ASSERT_EQ(fixture_ctx->banking_mode, 0x00, "Failed to set banking mode to 0");

    // Iterate over ALL 4 possible banks
    for (int bank = 0; bank < 4; bank++) {
        // Select Bank
        fixture_iface->write(fixture_ctx, addr_reg_ram_bank_num, bank & 0x03);
        ASSERT_EQ(
            fixture_ctx->ram_bank_number, (byte)(bank & 0x03), "Failed to set RAM banking number"
        );

        // Get expected values
        byte expected_val = (byte)(0x00 ^ 0xFF); // cant be anything other than bank 00

        // Verify
        ASSERT_EQ(
            fixture_iface->read(fixture_ctx, addr_ram),
            expected_val,
            "Remapped RAM in mode 0: Bank %i on %i-bank RAM. Expected bank 0",
            bank,
            phys_banks
        );
    }

    // Test Mode 1
    // RAM can be remapped now
    fixture_iface->write(fixture_ctx, addr_reg_banking_mode, 0x01);
    ASSERT_EQ(fixture_ctx->banking_mode, 0x01, "Failed to set banking mode to 1");

    // Iterate over ALL 4 possible banks
    for (int bank = 0; bank < 4; bank++) {
        // Select Bank
        fixture_iface->write(fixture_ctx, addr_reg_ram_bank_num, bank & 0x03);
        ASSERT_EQ(
            fixture_ctx->ram_bank_number, (byte)(bank & 0x03), "Failed to set RAM banking number"
        );

        // Get expected values
        int  masked_bank  = bank % phys_banks;
        byte expected_val = (byte)(masked_bank ^ 0xFF); // cant be anything other than bank 00

        // Verify
        ASSERT_EQ(
            fixture_iface->read(fixture_ctx, addr_ram),
            expected_val,
            "Remapped RAM in mode 1: Bank %i on %i-bank RAM. Expected bank %i",
            bank,
            phys_banks,
            masked_bank
        );
    }

    return true;
}

static void setup_clear_fixtures(void) {
    fixture_iface = NULL; // This is safe since it points to static memory
    if (fixture_ctx != NULL) {
        free(fixture_ctx->rom);
        free(fixture_ctx->ram);
        free(fixture_ctx);
        fixture_ctx = NULL; // Prevent Double Frees
    }

    return;
}

static int setup_new_fixtures(const enum rom_size rom_size, const enum ram_size ram_size) {
    setup_clear_fixtures();

    size_t n_rom_banks = rom_size_bank_count(rom_size);
    size_t n_ram_banks = ram_size_bank_count(ram_size);

    fixture_iface = mbc1_iface();
    fixture_ctx   = mbc1_instantiate(n_rom_banks, n_ram_banks);
    if (fixture_ctx == NULL) { return -1; }

    return 0;
}
