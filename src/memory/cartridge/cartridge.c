/**
 * @file cartridge.c
 * @brief Implemenation of Cartridge Module API.
 */
#include "memory/cartridge/cartridge.h"

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_stdinc.h>
#include <log.h>
#include <stdlib.h>
#include <string.h>

#include "memory/cartridge/structs/cartridge.h"

LOG_MODULE_SETUP("CARTRIDGE", CONFIG_CARTRIDGE_MODULE_LOG_LEVEL);

static struct rom_header parse_rom_header(const byte* rom_data) {
    struct rom_header rtn = {0};

    // Copy logo data into header
    // At ROM [0x0104-0x0133]
    // 48 Bytes long
    // No conversion needed
    memcpy(rtn.logo, rom_data + 0x104, 48);

    // Copy Game Title
    // At ROM [0x0134-0x0143]
    // Max length of 16 characters
    memcpy(rtn.title, rom_data + 0x134, 16);
    rtn.title[16] = '\0';

    // Copy Manufacture Code
    // At ROM [0x013F-0x0142]
    // 4 Bytes
    memcpy(rtn.manufacturer_code, rom_data + 0x13F, 4);

    // Copy CGB Flag [0x0143]
    rtn.cgb_flag = rom_data[0x143];

    // New licensee code [0x0144-0x0145]
    memcpy(rtn.new_licensee, rom_data + 0x144, 2);

    // SGB Flag [0x0146]
    rtn.sgb_flag = rom_data[0x146];

    // Cartridge Type [0x0147]
    rtn.type = (enum cartridge_type)rom_data[0x147];

    // ROM size [0x0148]
    rtn.rom_size = (enum rom_size)rom_data[0x148];

    // RAM size [0x0149]
    rtn.ram_size = (enum ram_size)rom_data[0x149];

    // Destination Code [0x014A]
    rtn.dest_code = rom_data[0x14A];

    // Old Licensee Code [0x014B]
    rtn.old_licensee = (enum old_licensee_code)rom_data[0x14B];

    // ROM Version [0x014C]
    rtn.version = rom_data[0x14C];

    // Header Checksum [0x014D]
    rtn.checksum = rom_data[0x14D];

    // Global Checksum [0x014E-0x014F]
    rtn.global_checksum = (word)(rom_data[0x14E]) | ((word)(rom_data[0x14F]) << 8);

    return rtn;
}

struct cartridge* cartridge_create(const char* filename) {
    int               rc            = 0;
    char*             data_path     = NULL;
    byte*             rom_file_data = NULL;
    byte*             ram_file_data = NULL;
    struct cartridge* cart          = NULL;

    if (filename == NULL) {
        log_error("Passed Filename was NULL");
        return NULL;
    }

    if (strlen(filename) == 0) {
        log_warn("Filename is empty");
        return NULL;
    }

    // Allocate Cartridge
    cart = malloc(sizeof(struct cartridge));
    if (cart == NULL) {
        log_error("Failed to allocate Cartridge. Ran out of Memory.");
        goto err;
    }
    cart->mbc       = NULL;
    cart->rom_path  = NULL;
    cart->ram_path  = NULL;
    cart->save_path = NULL;

    // Get Data path
    data_path = SDL_GetPrefPath(ORG_NAME, APP_NAME);
    if (data_path == NULL) {
        log_error(
            "Failed to get platform Preferred Path.\n"
            "\tSDL Error: %s",
            SDL_GetError()
        );
        SDL_ClearError();
        goto err;
    }

    // Get Save Path
    rc = SDL_asprintf(&cart->save_path, "%ssaves", data_path);
    if (rc < 0) {
        log_error(
            "Failed to create save directory path.\n"
            "\tSDL Error: %s",
            SDL_GetError()
        );
        SDL_ClearError();
        goto err;
    }

    // Load ROM file
    rc = SDL_asprintf(&cart->rom_path, "%sroms/%s.gb", data_path, filename);
    if (rc < 0) {
        log_error(
            "Failed to create ROM file path.\n"
            "\tSDL Error: %s",
            SDL_GetError()
        );
        SDL_ClearError();
        goto err;
    }
    if (SDL_GetPathInfo(cart->rom_path, NULL) == false) {
        log_warn("File \"%s\" could not be found!", cart->rom_path);
        goto err;
    }
    size_t rom_file_size = 0;
    rom_file_data        = SDL_LoadFile(cart->rom_path, &rom_file_size);
    if (rom_file_data == NULL) {
        log_error(
            "Failed to load file \"%s\" into memory.\n"
            "\tSDL Error: %s",
            cart->rom_path,
            SDL_GetError()
        );
        SDL_ClearError();
        goto err;
    }

    // Parse ROM header
    // Ensure loaded data is large enough to parse the header
    // Header ends at 0x014F
    if (rom_file_size < 0x0150) {
        log_error(
            "File \"%s\" is too small (%i bytes). Failed to parse ROM header.",
            cart->rom_path,
            rom_file_size
        );
        goto err;
    }
    cart->header = parse_rom_header(rom_file_data);

    // Validate Header Checksum
    byte expected_checksum = 0;
    for (word addr = 0x0134; addr <= 0x014C; addr++) {
        expected_checksum = expected_checksum - rom_file_data[addr] - 1;
    }
    if (expected_checksum != cart->header.checksum) {
        log_warn(
            "Header Checksum Validation Failed:\n"
            "\tHeader Value  : 0x%2X\n"
            "\tExpected Value: 0x%2X",
            cart->header.checksum,
            expected_checksum
        );
    }

    // Create MBC
    if (cartridge_type_is_valid(cart->header.type) == false) {
        log_error("File \"%s\" has an unknown Cartridge Type!", cart->rom_path);
        goto err;
    }
    cart->mbc = mbc_create(cart->header.type, cart->header.rom_size, cart->header.ram_size);
    if (cart->mbc == NULL) {
        log_error("Failed to create Cartridge from file \"%s\"", cart->rom_path);
        goto err;
    }

    // Load into ROM
    rc = mbc_load_rom(cart->mbc, rom_file_data, rom_file_size);
    if (rc != 0) {
        log_error("Failed to load file \"%s\" data into ROM space", cart->rom_path);
        goto err;
    }

    // Check for RAM data
    if (cartridge_type_contains_battery(cart->header.type) == false) {
        log_debug("No Battery. Skipping RAM load.");
        goto exit;
    }

    // Get RAM path
    rc = SDL_asprintf(&cart->ram_path, "%s/%s.sav", cart->save_path, filename);
    if (rc < 0) {
        log_error(
            "Failed to create RAM file path.\n"
            "\tSDL ERROR: %s",
            SDL_GetError()
        );
        SDL_ClearError();
        goto err;
    }

    // Check for RAM file
    if (SDL_GetPathInfo(cart->ram_path, NULL) == false) {
        log_warn("File \"%s\" not found. No RAM data to load. Skipping load.", cart->ram_path);
        goto exit;
    }

    // Load file
    size_t ram_file_size = 0;
    ram_file_data        = SDL_LoadFile(cart->ram_path, &ram_file_size);
    if (ram_file_data == NULL) {
        log_error(
            "Failed to load file \"%s\" into memory.\n"
            "\tSDL Error: %s",
            cart->ram_path,
            SDL_GetError()
        );
        SDL_ClearError();
        goto err;
    }

    // Load into RAM
    rc = mbc_load_ram(cart->mbc, ram_file_data, ram_file_size);
    if (rc != 0) {
        log_error("Failed to load file \"%s\" data into RAM space.", cart->ram_path);
        goto err;
    }

exit:
    // Free SDL data
    SDL_free(data_path);
    SDL_free(rom_file_data);
    SDL_free(ram_file_data);
    return cart;
err:
    // Free SDL data
    SDL_free(data_path);
    SDL_free(rom_file_data);
    SDL_free(ram_file_data);

    // Free internal data
    cartridge_free(&cart);
    return NULL;
}

byte cartridge_read(struct cartridge* cart, const word addr) {
    if (cart == NULL) {
        log_error("Invalid Cartridge Object.");
        return 0xFF;
    }

    return mbc_read(cart->mbc, addr);
}

void cartridge_write(struct cartridge* cart, const word addr, const byte value) {
    if (cart == NULL) {
        log_error("Invalid Cartridge Object.");
        return;
    }

    mbc_write(cart->mbc, addr, value);
}

int cartridge_save(struct cartridge* cart) {
    if (cart == NULL || cart->mbc == NULL) {
        log_error("Invalid Cartridge Object.");
        return -1;
    }

    if (cart->ram_path == NULL) {
        log_debug("Ignoring requested RAM save since cartridge does not contain persistent RAM");
        return 0;
    }

    // Fetch RAM data from MBC
    size_t ram_data_size = 0;
    byte*  ram_data      = mbc_dump_ram(cart->mbc, &ram_data_size);
    if (ram_data == NULL || ram_data_size == 0) {
        log_warn("No RAM data to save.");
        return 0;
    }

    // Write RAM to file
    if (SDL_CreateDirectory(cart->save_path) == false) {
        log_error(
            "Failed to create save directory: \"%s\".\n"
            "\tSDL Error: %s",
            cart->save_path,
            SDL_GetError()
        );
        SDL_ClearError();
        goto err;
    }
    if (SDL_SaveFile(cart->ram_path, ram_data, ram_data_size) == false) {
        log_error(
            "Failed to write RAM data to file \"%s\".\n"
            "\tSDL Error: %s",
            cart->ram_path,
            SDL_GetError()
        );
        SDL_ClearError();
        goto err;
    }

    free(ram_data);
    log_info("RAM data saved to \"%s\"", cart->ram_path);
    return 0;

err:
    free(ram_data);
    log_info("Failed to write RAM data to \"%s\"", cart->ram_path);
    return -1;
}

void cartridge_free(struct cartridge** p_cart) {
    if (p_cart == NULL || *p_cart == NULL) { return; }

    // Free SDL Data
    SDL_free((*p_cart)->rom_path);
    SDL_free((*p_cart)->ram_path);
    SDL_free((*p_cart)->save_path);

    mbc_cleanup(&((*p_cart)->mbc));
    free(*p_cart);
    *p_cart = NULL;

    return;
}
