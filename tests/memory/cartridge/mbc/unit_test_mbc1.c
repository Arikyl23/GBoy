/**
 * @file unit_test_mbc1.c
 * @brief Comprehensive Unit tests for the MBC1 Memory Bank Controller.
 */
#include "DEFINES.h"
#include "memory/cartridge/mbc/mbc1.h"
#include "memory/memory_map.h"
#include "testing.h"

#include "memory/cartridge/mbc/mbc1.c"

static const struct mbc_iface* fixture_iface;
static struct mbc1_context*    fixture_ctx;

static bool test_mbc1_module(void);

static bool test_instanciation(void);
static bool test_instanciate_standard(void);
static bool test_instanciate_alternative(void);
static bool test_instanciate_invalid(void);

static bool test_interface(void);
static bool test_interface_get(void);
static bool test_interface_read(void);
static bool test_interface_write(void);
static bool test_interface_destroy(void);

static bool fuzz_read(const byte expected_rom, const byte expected_ram);
static bool fuzz_write(const byte expected_ram, const byte test_value);

static void setup_clear_fixtures(void);
static int  setup_fill_fixtures(const size_t n_rom_banks, const size_t n_ram_banks);

int main(void) {
    // Mask all logs lower than this (we will get spammed with debug/warn logs otherwise)
    log_set_level(LOG_ERROR);

    fixture_iface = NULL;
    fixture_ctx   = NULL;

    // When main returns 0, that indicates a success
    return (test_mbc1_module() == true) ? 0 : 1;
}

static bool test_mbc1_module(void) {
    ASSERT_TRUE(test_instanciation(), "MBC1 Instancation Test(s) Failed");
    ASSERT_TRUE(test_interface(), "MBC1 Interface Test(s) Failed");

    return true;
}

static bool test_instanciation(void) {
    ASSERT_TRUE(test_instanciate_standard(), "Test Instancaite Standard Failed");
    ASSERT_TRUE(test_instanciate_alternative(), "Test Instancate Alternative Failed");
    ASSERT_TRUE(test_instanciate_invalid(), "Test Innstancate with invalid parameters Failed");
    return true;
}

static bool test_instanciate_standard(void) {
    setup_clear_fixtures();

    // No RAM
    // ----------
    fixture_ctx = mbc1_instanciate(64, 0);
    ASSERT_NOT_NULL(fixture_ctx, "Failed to instancate MBC1");

    ASSERT_NOT_NULL(fixture_ctx->rom, "MBC1 context created without ROM");
    ASSERT_EQ(fixture_ctx->rom_size, 64 * CART_ROM_BANK_SIZE, "Expected ROM size of 128 KiB");
    ASSERT_EQ(fixture_ctx->rom_bank_mask, 0x3F, "Expected 0x3F for ROM Bank Mask (64 Banks)");
    ASSERT_NULL(fixture_ctx->ram, "MBC1 context created with RAM instead of without");

    mbc1_destroy((void**)&fixture_ctx);
    ASSERT_NULL(fixture_ctx, "MBC1 destroy failed to set context pointer to NULL");
    // ----------

    // With RAM
    // ----------
    fixture_ctx = mbc1_instanciate(64, 4);
    ASSERT_NOT_NULL(fixture_ctx, "Failed to instancate MBC1");

    ASSERT_NOT_NULL(fixture_ctx->rom, "MBC1 context created without ROM");
    ASSERT_EQ(fixture_ctx->rom_size, 64 * CART_ROM_BANK_SIZE, "Expected ROM size of 128 KiB");
    ASSERT_EQ(fixture_ctx->rom_bank_mask, 0x3F, "Expected 0x3F for ROM Bank Mask (64 Banks)");
    ASSERT_NOT_NULL(fixture_ctx->ram, "MBC1 context created without RAM instead of with");
    ASSERT_EQ(fixture_ctx->ram_size, 4 * CART_RAM_BANK_SIZE, "Expected ROM size of 32 KiB");
    ASSERT_EQ(fixture_ctx->ram_bank_mask, 0x03, "Expected 0x03 for RAM Bank Mask (4 Banks)");

    mbc1_destroy((void**)&fixture_ctx);
    ASSERT_NULL(fixture_ctx, "MBC1 destroy failed to set context pointer to NULL");
    // ----------

    return true;
}

static bool test_instanciate_alternative(void) {
    setup_clear_fixtures();

    // No RAM
    // ----------
    fixture_ctx = mbc1_instanciate(128, 0);
    ASSERT_NOT_NULL(fixture_ctx, "Failed to instancate MBC1");

    ASSERT_NOT_NULL(fixture_ctx->rom, "MBC1 context created without ROM");
    ASSERT_EQ(fixture_ctx->rom_size, 128 * CART_ROM_BANK_SIZE, "Expected ROM size of 2 MiB");
    ASSERT_EQ(fixture_ctx->rom_bank_mask, 0x7F, "Expected 0x7F for ROM Bank Mask (128 Banks)");
    ASSERT_NULL(fixture_ctx->ram, "MBC1 context created with RAM instead of without");

    mbc1_destroy((void**)&fixture_ctx);
    ASSERT_NULL(fixture_ctx, "MBC1 destroy failed to set context pointer to NULL");
    // ----------

    // With RAM
    // ----------
    fixture_ctx = mbc1_instanciate(128, 1);
    ASSERT_NOT_NULL(fixture_ctx, "Failed to instancate MBC1");

    ASSERT_NOT_NULL(fixture_ctx->rom, "MBC1 context created without ROM");
    ASSERT_EQ(fixture_ctx->rom_size, 128 * CART_ROM_BANK_SIZE, "Expected ROM size of 2 MiB");
    ASSERT_EQ(fixture_ctx->rom_bank_mask, 0x7F, "Expected 0x7F for ROM Bank Mask (128 Banks)");
    ASSERT_NOT_NULL(fixture_ctx->ram, "MBC1 context created without RAM instead of with");
    ASSERT_EQ(fixture_ctx->ram_size, 1 * CART_RAM_BANK_SIZE, "Expected ROM size of 8 KiB");
    ASSERT_EQ(fixture_ctx->ram_bank_mask, 0x00, "Expected 0x00 for RAM Bank Mask (1 Banks)");

    mbc1_destroy((void**)&fixture_ctx);
    ASSERT_NULL(fixture_ctx, "MBC1 destroy failed to set context pointer to NULL");
    // ----------

    return true;
}

static bool test_instanciate_invalid(void) {
    setup_clear_fixtures();

    // No ROM
    fixture_ctx = mbc1_instanciate(0, 4);
    ASSERT_NULL(fixture_ctx, "Instanciated MBC1 context with no ROM. Should have failed");

    setup_clear_fixtures();

    // Too much RAM for standard
    fixture_ctx = mbc1_instanciate(32, 8);
    ASSERT_NULL(
        fixture_ctx, "Instanciated MBC1 context with more RAM than supported. Should have failed"
    );

    setup_clear_fixtures();

    // Too much RAM for alternative
    fixture_ctx = mbc1_instanciate(128, 2);
    ASSERT_NULL(
        fixture_ctx, "Instanciated MBC1 context with more RAM than supported. Should have failed"
    );

    setup_clear_fixtures();

    // Too much ROM
    fixture_ctx = mbc1_instanciate(256, 1);
    ASSERT_NULL(
        fixture_ctx, "Instanciated MBC1 context with more ROM than supported. Should have failed"
    );

    return true;
}

static bool test_interface(void) {
    ASSERT_TRUE(test_interface_get(), "Test Interface Get Failed");
    ASSERT_TRUE(test_interface_read(), "Test Interface Read Failed");
    ASSERT_TRUE(test_interface_write(), "Test Interface Write Failed");
    ASSERT_TRUE(test_interface_destroy(), "Test Interface Destroy Failed");
    return true;
}

static bool test_interface_get(void) {
    setup_clear_fixtures();

    fixture_iface = mbc1_iface();
    ASSERT_NOT_NULL(fixture_iface, "Failed to get interface");
    ASSERT_NOT_NULL(fixture_iface->read, "Interface does not define read()");
    ASSERT_NOT_NULL(fixture_iface->write, "Interface does not define write()");
    ASSERT_NOT_NULL(fixture_iface->destroy, "Interface does not define destroy()");

    return true;
}

static bool test_interface_read(void) {
    // No ram
    // -----------------
    setup_fill_fixtures(64, 0);
    ASSERT_TRUE(fuzz_read(0xAA, 0xFF), "No RAM read failed");
    // -----------------

    // Test with RAM
    // -------------
    setup_fill_fixtures(64, 4);
    // RAM Disabled
    ASSERT_TRUE(fuzz_read(0xAA, 0xFF), "Disabled RAM read failed");
    fixture_ctx->ram_enable = 0x0A;
    // RAM Enabled
    ASSERT_TRUE(fuzz_read(0xAA, 0xAA), "Enabled RAM read failed");
    // -------------

    // Test with large ROM
    // -------------
    setup_fill_fixtures(128, 1);
    // RAM Disabled
    ASSERT_TRUE(fuzz_read(0xAA, 0xFF), "Disabled RAM read failed");
    fixture_ctx->ram_enable = 0x0A;
    // RAM Enabled
    ASSERT_TRUE(fuzz_read(0xAA, 0xAA), "Enabled RAM read failed");
    // -------------

    return true;
}

static bool test_interface_write(void) {
    // No ram
    // -----------------
    setup_fill_fixtures(64, 0);
    ASSERT_TRUE(fuzz_write(0xFF, 0x55), "No RAM write failed");
    // -----------------

    // Test with RAM
    // -------------
    setup_fill_fixtures(64, 4);
    // RAM Disabled
    ASSERT_TRUE(fuzz_write(0xFF, 0x55), "Disabled RAM write failed");
    fixture_ctx->ram_enable = 0x0A;
    // RAM Enabled
    ASSERT_TRUE(fuzz_write(0x55, 0x55), "Enabled RAM write failed");
    // -------------

    // Test with large ROM
    // -------------
    setup_fill_fixtures(128, 1);
    // RAM Disabled
    ASSERT_TRUE(fuzz_write(0xFF, 0x55), "Disabled RAM write failed");
    fixture_ctx->ram_enable = 0x0A;
    // RAM Enabled
    ASSERT_TRUE(fuzz_write(0x55, 0x55), "Enabled RAM write failed");
    // -------------

    return true;
}

static bool test_interface_destroy(void) {
    setup_fill_fixtures(32, 0);

    fixture_iface->destroy((void**)&fixture_ctx);
    ASSERT_NULL(fixture_ctx, "Destroy failed to set context to NULL");

    setup_fill_fixtures(32, 4);
    fixture_iface->destroy((void**)&fixture_ctx);
    ASSERT_NULL(fixture_ctx, "Destroy failed to set context to NULL");

    setup_fill_fixtures(128, 0);
    fixture_iface->destroy((void**)&fixture_ctx);
    ASSERT_NULL(fixture_ctx, "Destroy failed to set context to NULL");

    setup_fill_fixtures(128, 1);
    fixture_iface->destroy((void**)&fixture_ctx);
    ASSERT_NULL(fixture_ctx, "Destroy failed to set context to NULL");

    return true;
}

static bool fuzz_read(const byte expected_rom, const byte expected_ram) {
    for (size_t rom_bank_num = 0; rom_bank_num < 128; rom_bank_num++) {
        fixture_ctx->rom_bank_number = rom_bank_num;
        for (size_t ram_bank_num = 0; ram_bank_num < 4; ram_bank_num++) {
            fixture_ctx->ram_bank_number = ram_bank_num;
            for (dword test_addr = 0x0000; test_addr < 0xFFFF; test_addr += 7) {
                byte received_value = fixture_iface->read(fixture_ctx, test_addr);
                byte expected_value;

                if (test_addr >= ADDR_CART_ROM_START && test_addr <= ADDR_CART_ROM_END) {
                    expected_value = expected_rom;
                } else if (test_addr >= ADDR_CART_RAM_START && test_addr <= ADDR_CART_RAM_END) {
                    expected_value = expected_ram;
                } else {
                    expected_value = 0xFF;
                }

                ASSERT_EQ(
                    received_value,
                    expected_value,
                    "Read value did not match expected\n"
                    "\tADDR: 0x%.4X\n"
                    "\tRecieved: 0x%.2X\n"
                    "\tExpected: 0x%.2X",
                    test_addr,
                    received_value,
                    expected_value
                );
            }
        }
    }

    return true;
}

static bool fuzz_write(const byte expected_ram, const byte test_value) {
    // Test each register seperately (and reset back to previous value)
    byte reg = fixture_ctx->ram_enable;
    fixture_iface->write(fixture_ctx, 0x0000, 0xFF);
    ASSERT_EQ(fixture_ctx->ram_enable, 0xFF, "Failed to write to ram enabled register");
    fixture_ctx->ram_enable = reg;

    reg = fixture_ctx->rom_bank_number;
    fixture_iface->write(fixture_ctx, 0x2000, 0xFF);
    ASSERT_EQ(fixture_ctx->rom_bank_number, 0x1F, "Failed to write to rom bank number register");
    fixture_ctx->rom_bank_number = reg;

    reg = fixture_ctx->ram_bank_number;
    fixture_iface->write(fixture_ctx, 0x4000, 0xFF);
    ASSERT_EQ(fixture_ctx->ram_bank_number, 0x03, "Failed to write to ram bank number register");
    fixture_ctx->ram_bank_number = reg;

    reg = fixture_ctx->banking_mode;
    fixture_iface->write(fixture_ctx, 0x6000, 0xFF);
    ASSERT_EQ(fixture_ctx->banking_mode, 0x01, "Failed to write to banking_mode register");
    fixture_ctx->banking_mode = reg;

    // Test everything else but ROM
    for (size_t rom_bank_num = 0; rom_bank_num < 128; rom_bank_num++) {
        fixture_ctx->rom_bank_number = rom_bank_num;
        for (size_t ram_bank_num = 0; ram_bank_num < 4; ram_bank_num++) {
            fixture_ctx->ram_bank_number = ram_bank_num;
            for (dword test_addr = ADDR_CART_ROM_END + 1; test_addr < 0xFFFF; test_addr += 7) {
                byte received_value;
                byte expected_value;

                fixture_iface->write(fixture_ctx, test_addr, test_value);
                received_value = fixture_iface->read(fixture_ctx, test_addr);

                if (test_addr >= ADDR_CART_RAM_START && test_addr <= ADDR_CART_RAM_END) {
                    expected_value = expected_ram;
                } else {
                    expected_value = 0xFF; // Not part of the cartridge (bad write addr)
                }

                ASSERT_EQ(
                    received_value,
                    expected_value,
                    "Read value did not match expected\n"
                    "\tADDR: 0x%.4X\n"
                    "\tRecieved: 0x%.2X\n"
                    "\tExpected: 0x%.2X",
                    test_addr,
                    received_value,
                    expected_value
                );
            }
        }
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

static int setup_fill_fixtures(const size_t n_rom_banks, const size_t n_ram_banks) {
    setup_clear_fixtures();

    fixture_iface = mbc1_iface();
    fixture_ctx   = mbc1_instanciate(n_rom_banks, n_ram_banks);
    if (fixture_ctx == NULL) { return -1; }

    for (int i = 0; i < fixture_ctx->rom_size; i++) { fixture_ctx->rom[i] = 0xAA; }
    for (int i = 0; i < fixture_ctx->ram_size; i++) { fixture_ctx->ram[i] = 0xAA; }

    return 0;
}
