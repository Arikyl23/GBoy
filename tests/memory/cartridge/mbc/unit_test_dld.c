/**
 * @file unit_test_dld.c
 * @brief Comprehensive Unit tests for the DLD Memory Bank Controller.
 */
#include "DEFINES.h"
#include "memory/cartridge/mbc/dld.h"
#include "memory/memory_map.h"
#include "testing.h"

#include "memory/cartridge/mbc/dld.c"

static const struct mbc_iface* fixture_iface;
static struct dld_context*     fixture_ctx;

static bool test_dld_module(void);

static bool test_instanciation(void);
static bool test_instanciate_no_ram(void);
static bool test_instanciate_ram(void);
static bool test_instanciate_invalid(void);

static bool test_interface(void);
static bool test_interface_get(void);
static bool test_interface_read(void);
static bool test_interface_write(void);
static bool test_interface_destroy(void);

static void setup_clear_fixtures(void);
static int  setup_fill_fixtures(const size_t n_ram_banks);

int main(void) {
    // Mask all logs lower than this (we will get spammed with debug/warn logs otherwise)
    log_set_level(LOG_ERROR);

    // When main returns 0, that indicates a success
    return (test_dld_module() == true) ? 0 : 1;
}

static bool test_dld_module(void) {
    ASSERT_TRUE(test_instanciation(), "DLD Instancation Test(s) Failed");
    ASSERT_TRUE(test_interface(), "DLD Interface Test(s) Failed");

    return true;
}

static bool test_instanciation(void) {
    ASSERT_TRUE(test_instanciate_no_ram(), "Test Instancaite with no RAM Failed");
    ASSERT_TRUE(test_instanciate_ram(), "Test Instancate with RAM Failed");
    ASSERT_TRUE(test_instanciate_invalid(), "Test Innstancate with invalid parameters Failed");
    return true;
}

static bool test_instanciate_no_ram(void) {
    setup_clear_fixtures();

    fixture_ctx = dld_instanciate(0);
    ASSERT_NOT_NULL(fixture_ctx, "Failed to instancate DLD MBC with no RAM");

    ASSERT_NULL(fixture_ctx->ram, "DLD context created with RAM instead of without");

    dld_destroy((void**)&fixture_ctx);
    ASSERT_NULL(fixture_ctx, "DLD destroy failed to set context pointer to NULL");

    return true;
}

static bool test_instanciate_ram(void) {
    setup_clear_fixtures();

    fixture_ctx = dld_instanciate(1);
    ASSERT_NOT_NULL(fixture_ctx, "Failed to instancate DLD MBC with RAM");

    ASSERT_NOT_NULL(fixture_ctx->ram, "DLD context created without RAM instead of with");

    dld_destroy((void**)&fixture_ctx);
    ASSERT_NULL(fixture_ctx, "DLD destroy failed to set context pointer to NULL");

    return true;
}

static bool test_instanciate_invalid(void) {
    setup_clear_fixtures();

    fixture_ctx = dld_instanciate(2);
    ASSERT_NULL(fixture_ctx, "Instanciated DLD context with invalid parameters");

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

    fixture_iface = dld_iface();
    ASSERT_NOT_NULL(fixture_iface, "Failed to get iface");

    return true;
}

static bool test_interface_read(void) {
    // First with no ram
    // -----------------
    setup_fill_fixtures(0);

    for (dword test_addr = 0x0000; test_addr < 0xFFFF; test_addr++) {
        byte received_value = fixture_iface->read(fixture_ctx, test_addr);
        byte expected_value;

        if (test_addr >= ADDR_CART_ROM_START && test_addr <= ADDR_CART_ROM_END) {
            expected_value = 0xAA;
        } else if (test_addr >= ADDR_CART_RAM_START && test_addr <= ADDR_CART_RAM_END) {
            expected_value = 0xFF;
        } else {
            expected_value = 0xFF;
        }

        ASSERT_EQ(
            received_value,
            expected_value,
            "Read value did not match expected\n\r"
            "\tADDR: 0x%.4X\n\r"
            "\tRecieved: 0x%.2X\n\r"
            "\tExpected: 0x%.2X",
            received_value,
            expected_value
        );
    }
    // -----------------

    // Test with RAM
    // -------------
    setup_fill_fixtures(1);

    for (dword test_addr = 0x0000; test_addr < 0xFFFF; test_addr++) {
        byte received_value = fixture_iface->read(fixture_ctx, test_addr);
        byte expected_value;

        if (test_addr >= ADDR_CART_ROM_START && test_addr <= ADDR_CART_ROM_END) {
            expected_value = 0xAA;
        } else if (test_addr >= ADDR_CART_RAM_START && test_addr <= ADDR_CART_RAM_END) {
            expected_value = 0xAA;
        } else {
            expected_value = 0xFF;
        }

        ASSERT_EQ(
            received_value,
            expected_value,
            "Read value did not match expected\n\r"
            "\tADDR: 0x%.4X\n\r"
            "\tRecieved: 0x%.2X\n\r"
            "\tExpected: 0x%.2X",
            received_value,
            expected_value
        );
    }
    // -------------

    return true;
}

static bool test_interface_write(void) {
    // First with no ram
    // -----------------
    setup_fill_fixtures(0);

    for (dword test_addr = 0x0000; test_addr < 0xFFFF; test_addr++) {
        byte test_value = 0x55;
        byte received_value;
        byte expected_value;

        fixture_iface->write(fixture_ctx, test_addr, test_value);
        received_value = fixture_iface->read(fixture_ctx, test_addr);

        if (test_addr >= ADDR_CART_ROM_START && test_addr <= ADDR_CART_ROM_END) {
            expected_value = 0xAA; // Cant write to ROM, should expect seeded value
        } else if (test_addr >= ADDR_CART_RAM_START && test_addr <= ADDR_CART_RAM_END) {
            expected_value = 0xFF; // No RAM connected
        } else {
            expected_value = 0xFF; // Not part of the cartridge (bad write addr)
        }

        ASSERT_EQ(
            received_value,
            expected_value,
            "Read value did not match expected\n\r"
            "\tADDR: 0x%.4X\n\r"
            "\tRecieved: 0x%.2X\n\r"
            "\tExpected: 0x%.2X",
            test_addr,
            received_value,
            expected_value
        );
    }
    // -----------------

    // Test with RAM
    // -------------
    setup_fill_fixtures(1);

    for (dword test_addr = 0x0000; test_addr < 0xFFFF; test_addr++) {
        byte test_value = 0x55;
        byte received_value;
        byte expected_value;

        fixture_iface->write(fixture_ctx, test_addr, test_value);
        received_value = fixture_iface->read(fixture_ctx, test_addr);

        if (test_addr >= ADDR_CART_ROM_START && test_addr <= ADDR_CART_ROM_END) {
            expected_value = 0xAA; // Can't write to ROM, should expect seeded value
        } else if (test_addr >= ADDR_CART_RAM_START && test_addr <= ADDR_CART_RAM_END) {
            expected_value = test_value;
        } else {
            expected_value = 0xFF; // Not part of the cartridge (bad write addr)
        }

        ASSERT_EQ(
            received_value,
            expected_value,
            "Read value did not match expected\n\r"
            "\tADDR: 0x%.4X\n\r"
            "\tRecieved: 0x%.2X\n\r"
            "\tExpected: 0x%.2X",
            test_addr,
            received_value,
            expected_value
        );
    }
    // -------------

    return true;
}
static bool test_interface_destroy(void) {
    setup_fill_fixtures(0);

    fixture_iface->destroy((void**)&fixture_ctx);
    ASSERT_NULL(fixture_ctx, "Destroy failed to set context to NULL");

    setup_fill_fixtures(1);
    fixture_iface->destroy((void**)&fixture_ctx);
    ASSERT_NULL(fixture_ctx, "Destroy failed to set context to NULL");

    return true;
}

static void setup_clear_fixtures(void) {
    fixture_iface = NULL; // This is safe since it points to static memory
    if (fixture_ctx != NULL) {
        if (fixture_ctx->ram != NULL) {
            free(fixture_ctx->ram);
            fixture_ctx->ram = NULL;
        }

        free(fixture_ctx);
        fixture_ctx = NULL;
    }

    return;
}

static int setup_fill_fixtures(const size_t n_ram_banks) {
    setup_clear_fixtures();

    fixture_iface = dld_iface();
    fixture_ctx   = dld_instanciate(n_ram_banks);
    if (fixture_ctx == NULL) { return -1; }

    for (int i = 0; i < CART_ROM_BANK_SIZE * 2; i++) {
        fixture_ctx->rom[i] = 0xAA;

        if (fixture_ctx->ram != NULL && i < CART_RAM_BANK_SIZE) { fixture_ctx->ram[i] = 0xAA; }
    }

    return 0;
}
