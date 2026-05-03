/**
 * @file int_test_cartridge.c
 * @brief Comprehensive Integration Tests for the Cartridge.
 */
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_iostream.h>
#include <assert.h>
#include <log.h>
#include <stdbool.h>
#include <string.h>

#include "DEFINES.h"
#include "configuration.h"
#include "memory/cartridge/cartridge.h"
#include "memory/cartridge/structs/cartridge.h"
#include "memory/memory_map.h"
#include "testing.h"

LOG_MODULE_SETUP_DEFAULT("INT TEST CARTRIDGE");

// ----- Data Section Decls -----
const word m_data_addr_entry            = 0x0100;
const word m_data_addr_logo             = 0x0104;
const word m_data_addr_title            = 0x0134;
const word m_data_addr_manufacture_code = 0x013F;
const word m_data_addr_cgb_flag         = 0x0143;
const word m_data_addr_new_li_code      = 0x0144;
const word m_data_addr_sgb_flag         = 0x0146;
const word m_data_addr_cart_type        = 0x0147;
const word m_data_addr_rom_size         = 0x0148;
const word m_data_addr_ram_size         = 0x0149;
const word m_data_addr_dest_code        = 0x014A;
const word m_data_addr_old_li_code      = 0x014B;
const word m_data_addr_rom_ver          = 0x014C;
const word m_data_addr_header_checksum  = 0x014D;
const word m_data_addr_global_checksum  = 0x014E;

/** @brief Nintendo Logo (0x0104 - 0x0133) */
const byte m_data_logo[] = {0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83,
                            0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
                            0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63,
                            0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E};

/** @brief Standard title for all test ROMs. */
const char m_data_rom_title[] = "TESTROM";
static_assert(sizeof(m_data_rom_title) <= 17);

/** @brief Global DLD ROM. This is populated and written to a file upon calling
 * setup_test_dld_rom_file(). */
byte m_data_dld_rom[CART_ROM_BANK_SIZE * 2] = {0};

byte m_data_mbc1_rom[CART_ROM_BANK_SIZE * 4] = {0};
// -----

static bool test_cartridge_module_integration(void);

static bool test_instantiation(void);
static bool test_instantiate_header(void);
static bool test_instantiate_rom(void);
static bool test_instantiate_invalid(void);

static bool test_persistant_ram(void);

static int setup_test_env(void);
static int clear_test_files(void);
static int setup_test_dld_rom_file(void);
static int setup_test_mbc1_rom_file(void);
static int write_rom_to_file(const char* filename, const byte* rom, const size_t rom_size);
static SDL_EnumerationResult cb_recursive_rm_dir(
    void*       userdata,
    const char* dirname,
    const char* fname
);

int main(void) { return (test_cartridge_module_integration() == true) ? 0 : -1; }

static bool test_cartridge_module_integration(void) {
    ASSERT_OK(setup_test_env(), "Failed to setup testing environment");

    ASSERT_TRUE(test_instantiation(), "Instantiation Test(s) Failed");
    ASSERT_TRUE(test_persistant_ram(), "Persistant RAM Test(s) Failed");

    return true;
}

static bool test_instantiation(void) {
    ASSERT_TRUE(test_instantiate_header(), "Test Instantiate Valid Failed");
    ASSERT_TRUE(test_instantiate_rom(), "Test Instantiate ROM Failed");

    return true;
}

static bool test_instantiate_header(void) {
    struct cartridge* cart = cartridge_create("dld_test_rom");
    ASSERT_NOT_NULL(cart, "Failed to create cartridge from valid ROM");

    // ----- Verify Header -----

    // Logo
    ASSERT_OK(
        memcmp(cart->header.logo, m_data_logo, sizeof(m_data_logo)), "Nintendo logo mismatch:"
    );

    // Title
    ASSERT_OK(
        strncmp((char*)cart->header.title, m_data_rom_title, strlen(m_data_rom_title)),
        "Title mismatch:\n"
        "\tExpected: %s\n"
        "\tReceived: %.16s", // Limit print to 16 chars to avoid garbage
        m_data_rom_title,
        cart->header.title
    );

    // Cartridge Type (0x0147)
    ASSERT_EQ(
        cart->header.type,
        m_data_dld_rom[m_data_addr_cart_type],
        "Incorrect cartridge type:\n"
        "\tExpected %s\n"
        "\tReceived %s",
        cartridge_type_to_string(m_data_dld_rom[m_data_addr_cart_type]),
        cartridge_type_to_string(cart->header.type)
    );

    // ROM Size (0x0148)
    ASSERT_EQ(
        cart->header.rom_size,
        m_data_dld_rom[m_data_addr_rom_size],
        "Incorrect ROM size flag:\n"
        "\tExpected %s\n"
        "\tReceived %s",
        rom_size_to_string(m_data_dld_rom[m_data_addr_rom_size]),
        rom_size_to_string(cart->header.rom_size)
    );

    // RAM Size (0x0149)
    ASSERT_EQ(
        cart->header.ram_size,
        m_data_dld_rom[m_data_addr_ram_size],
        "Incorrect RAM size flag:\n"
        "\tExpected %s\n"
        "\tReceived %s",
        ram_size_to_string(m_data_dld_rom[m_data_addr_ram_size]),
        ram_size_to_string(cart->header.ram_size)
    );

    // Header Checksum (0x014D)
    ASSERT_EQ(
        cart->header.checksum,
        m_data_dld_rom[m_data_addr_header_checksum],
        "Checksum mismatch:\n"
        "\tExpected %02X\n"
        "\tReceived %02X",
        m_data_dld_rom[m_data_addr_header_checksum],
        cart->header.checksum
    );

    // -----

    // Verify Cleanup
    cartridge_free(&cart);
    ASSERT_NULL(cart, "Failed to destroy cart");

    return true;
}

static bool test_instantiate_rom(void) {
    const word addr_rom_bank_num  = 0x2000;
    const word addr_bank_mode_sel = 0x6000;

    struct cartridge* cart = cartridge_create("mbc1_test_rom");
    ASSERT_NOT_NULL(cart, "Failed to create cartridge from valid ROM");

    // Force mode 0
    cartridge_write(cart, addr_bank_mode_sel, 0x00);
    // Verify ROM is seeded correctly and banking works
    for (int i = 0; i < 4; i++) {
        cartridge_write(cart, addr_rom_bank_num, i);
        byte expected_value_lo = 0 ^ 0xFF;         // Never changes in mode 0
        int  bank_hi           = (i == 0) ? 1 : i; // cannot be bank 0
        byte expected_value_hi = bank_hi ^ 0xFF;
        byte received_value    = cartridge_read(cart, ADDR_CART_ROM_LOW_START);
        ASSERT_EQ(
            received_value,
            expected_value_lo,
            "Low ROM Mismatch:\n"
            "\tExpected: 0x%2X\n"
            "\tReceived: 0x%2X",
            expected_value_lo,
            received_value
        );
        received_value = cartridge_read(cart, ADDR_CART_ROM_HIGH_START);
        ASSERT_EQ(
            received_value,
            expected_value_hi,
            "High ROM Mismatch:\n"
            "\tExpected: 0x%2X\n"
            "\tReceived: 0x%2X",
            expected_value_hi,
            received_value
        );
    }

    return true;
}

static bool test_persistant_ram(void) {
    struct cartridge* cart = cartridge_create("mbc1_test_rom");
    ASSERT_NOT_NULL(cart, "Failed to create Cartridge.");

    // Setup Cart
    const word addr_ram_enable    = 0x0000;
    const word addr_ram_bank_num  = 0x4000;
    const word addr_bank_mode_sel = 0x6000;
    cartridge_write(cart, addr_ram_enable, 0x0A);
    cartridge_write(cart, addr_bank_mode_sel, 0x01);

    // Seed Cart
    for (int i = 0; i < 4; i++) {
        cartridge_write(cart, addr_ram_bank_num, i);
        byte seed_value = i ^ 0xFF;
        cartridge_write(cart, ADDR_CART_RAM_START, seed_value);
    }

    // Write Cart to file
    ASSERT_OK(cartridge_save(cart), "Failed to Write RAM to file");

    // Reload Cart
    cartridge_free(&cart);
    ASSERT_NULL(cart, "Failed to destroy Cart");
    cart = cartridge_create("mbc1_test_rom");

    // Validate correct ROM values

    return true;
}

static int setup_test_env(void) {
    // Clear test files
    ASSERT_OK(clear_test_files(), "Failed to clear test files");

    // Generate Test ROMs
    ASSERT_OK(setup_test_dld_rom_file(), "Failed to generate test DLD ROM");
    ASSERT_OK(setup_test_mbc1_rom_file(), "Failed to generate test MBC1 ROM");

    return 0;
}

static int clear_test_files(void) {
    char* data_path = NULL;

    // Get data path
    data_path = SDL_GetPrefPath(ORG_NAME, APP_NAME);
    if (data_path == NULL) {
        log_error(
            "Failed to get prefered data path.\n"
            "\tSDL Error: %s",
            SDL_GetError()
        );
        SDL_ClearError();
        goto err;
    }

    if (SDL_EnumerateDirectory(data_path, cb_recursive_rm_dir, NULL) == false) {
        log_error("Recursive Directory removal failed on %s", data_path);
        goto err;
    }

    SDL_free(data_path);
    return 0;
err:
    SDL_free(data_path);
    return -1;
}

static int setup_test_dld_rom_file(void) {
    // Entry Point (0x0100 - 0x0103)
    // Common: NOP (0x00) then JP 0x0150 (0xC3 0x50 0x01)
    m_data_dld_rom[m_data_addr_entry]     = 0x00;
    m_data_dld_rom[m_data_addr_entry + 1] = 0xC3;
    m_data_dld_rom[m_data_addr_entry + 2] = 0x50;
    m_data_dld_rom[m_data_addr_entry + 3] = 0x01;

    // Nintendo Logo (0x0104 - 0x0133)
    memcpy(&m_data_dld_rom[m_data_addr_logo], m_data_logo, sizeof(m_data_logo));

    // Title (0x0134 - 0x0143)
    memcpy(&m_data_dld_rom[m_data_addr_title], m_data_rom_title, strlen(m_data_rom_title));

    // Cartridge Settings
    m_data_dld_rom[m_data_addr_cart_type] = 0x09; // DLD + RAM + BATTERY
    m_data_dld_rom[m_data_addr_rom_size]  = 0x00; // 32KB ROM
    m_data_dld_rom[m_data_addr_ram_size]  = 0x02; // 8KB RAM

    // Calculate Header Checksum (0x014D)
    // Formula: x = 0; for i = 0x134 to 0x14C: x = x - rom[i] - 1;
    byte checksum = 0;
    for (int i = 0x0134; i <= 0x014C; i++) { checksum = checksum - m_data_dld_rom[i] - 1; }
    m_data_dld_rom[m_data_addr_header_checksum] = checksum;

    // Seed predictable values into ROM based on their bank number
    for (int i = 0; i < 2; i++) { m_data_dld_rom[CART_ROM_BANK_SIZE * i] = i ^ 0xFF; }

    return write_rom_to_file("dld_test_rom", m_data_dld_rom, sizeof(m_data_dld_rom));
}

static int setup_test_mbc1_rom_file(void) {
    // Entry Point (0x0100 - 0x0103)
    // Common: NOP (0x00) then JP 0x0150 (0xC3 0x50 0x01)
    m_data_mbc1_rom[m_data_addr_entry]     = 0x00;
    m_data_mbc1_rom[m_data_addr_entry + 1] = 0xC3;
    m_data_mbc1_rom[m_data_addr_entry + 2] = 0x50;
    m_data_mbc1_rom[m_data_addr_entry + 3] = 0x01;

    // Nintendo Logo (0x0104 - 0x0133)
    memcpy(&m_data_mbc1_rom[m_data_addr_logo], m_data_logo, sizeof(m_data_logo));

    // Title (0x0134 - 0x0143)
    memcpy(&m_data_mbc1_rom[m_data_addr_title], m_data_rom_title, strlen(m_data_rom_title));

    // Cartridge Settings
    m_data_mbc1_rom[m_data_addr_cart_type] = 0x03; // MBC1 + RAM + BATTERY
    m_data_mbc1_rom[m_data_addr_rom_size]  = 0x01; // 64KB ROM
    m_data_mbc1_rom[m_data_addr_ram_size]  = 0x03; // 32KB RAM

    // Calculate Header Checksum (0x014D)
    // Formula: x = 0; for i = 0x134 to 0x14C: x = x - rom[i] - 1;
    byte checksum = 0;
    for (int i = 0x0134; i <= 0x014C; i++) { checksum = checksum - m_data_mbc1_rom[i] - 1; }
    m_data_mbc1_rom[m_data_addr_header_checksum] = checksum;

    // Seed predictable values into ROM based on their bank number
    for (int i = 0; i < 4; i++) { m_data_mbc1_rom[CART_ROM_BANK_SIZE * i] = i ^ 0xFF; }

    return write_rom_to_file("mbc1_test_rom", m_data_mbc1_rom, sizeof(m_data_mbc1_rom));
}

static int write_rom_to_file(const char* filename, const byte* rom, const size_t rom_size) {
    int   rc                 = 0;
    char* data_path          = NULL;
    char* rom_directory_path = NULL;
    char* rom_path           = NULL;

    // Get data path
    data_path = SDL_GetPrefPath(ORG_NAME, APP_NAME);
    if (data_path == NULL) {
        log_error(
            "Failed to get prefered data path.\n"
            "\tSDL Error: %s",
            SDL_GetError()
        );
        SDL_ClearError();
        goto err;
    }

    // Check/Create ROM directory
    rc = SDL_asprintf(&rom_directory_path, "%sroms/", data_path);
    if (rc < 0) {
        log_error(
            "Failed to create rom directory path.\n"
            "\tSDL Error: %s",
            SDL_GetError()
        );
        SDL_ClearError();
        goto err;
    }
    if (SDL_CreateDirectory(rom_directory_path) == false) {
        log_error(
            "Failed to create rom directory: \"%s\".\n"
            "\tSDL Error: %s",
            rom_directory_path,
            SDL_GetError()
        );
        SDL_ClearError();
        goto err;
    }

    // Write to file
    rc = SDL_asprintf(&rom_path, "%s%s.gb", rom_directory_path, filename);
    if (rc < 0) {
        log_error(
            "Failed to create rom path.\n"
            "SDL Error: %s",
            SDL_GetError()
        );
        SDL_ClearError();
        goto err;
    }
    if (SDL_SaveFile(rom_path, rom, rom_size) == false) {
        log_error(
            "Failed to write test rom to \"%s\" file.\n"
            "\tSDL Error: %s",
            rom_path,
            SDL_GetError()
        );
        SDL_ClearError();
        goto err;
    }

    SDL_free(data_path);
    SDL_free(rom_directory_path);
    SDL_free(rom_path);
    return 0;

err:
    SDL_free(data_path);
    SDL_free(rom_directory_path);
    SDL_free(rom_path);
    return -1;
}

static SDL_EnumerationResult cb_recursive_rm_dir(
    void*       userdata,
    const char* dirname,
    const char* fname
) {
    int   rc;
    char* full_path = NULL;

    // Construct full file path
    rc = SDL_asprintf(&full_path, "%s%s", dirname, fname);
    if (rc < 0) {
        log_error("Failed to construct full path while recursively removing director %s", dirname);
        return SDL_ENUM_FAILURE;
    }

    // Get path info
    SDL_PathInfo info = {0};
    if (SDL_GetPathInfo(full_path, &info) == false) {
        log_error("Enumerated path %s does not exist or an error was encountered.", full_path);
        return SDL_ENUM_FAILURE;
    }

    switch (info.type) {
    case SDL_PATHTYPE_FILE:
        if (SDL_RemovePath(full_path) == false) {
            log_error(
                "Failed to remove file %s\n"
                "\tSDL Error: %s",
                full_path,
                SDL_GetError()
            );
            SDL_ClearError();
            return SDL_ENUM_FAILURE;
        }
        return SDL_ENUM_CONTINUE;
    case SDL_PATHTYPE_DIRECTORY:
        return SDL_EnumerateDirectory(full_path, cb_recursive_rm_dir, NULL) ? SDL_ENUM_CONTINUE
                                                                            : SDL_ENUM_FAILURE;
    case SDL_PATHTYPE_OTHER:
        log_warn("Encountered unknown object %s", full_path);
        return SDL_ENUM_CONTINUE;
    default:
        log_warn("Impossible state reached!");
        return SDL_ENUM_CONTINUE;
    }
}
