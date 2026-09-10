/**
 * @file rom_header.h
 * @brief Contains the structure definition for the ROM Header
 */
#pragma once

#include "memory/cartridge/cartridge_type.h"
#include "memory/cartridge/old_licensee_code.h"
#include "memory/cartridge/ram_size.h"
#include "memory/cartridge/rom_size.h"

struct rom_header {                           /** @brief ROM Header for a Cartridge. */
    byte                logo[48];             /** @brief 0x0104-0x0133: Nintendo Logo. */
    char                title[17];            /** @brief 0x0134-0x0143: Title (NULL terminated). */
    byte                manufacturer_code[4]; /** @brief 0x013F-0x0142: Newer carts only. */
    byte                cgb_flag;             /** @brief 0x0143: GBC support level. */
    char                new_licensee[3];      /** @brief 0x0144-0x0145: Two-character code. */
    byte                sgb_flag;             /** @brief 0x0146: SGB support. */
    enum cartridge_type type;                 /** @brief 0x0147: Reported Cartridge Type. */
    enum rom_size       rom_size;             /** @brief 0x0148: Reported ROM size. */
    enum ram_size       ram_size;             /** @brief 0x0149: Reported RAM size. */
    byte                dest_code;            /** @brief 0x014A: Japan/Non-Japan. */
    byte                old_licensee;         /** @brief 0x014B: Older licensee code. */
    byte                version;              /** @brief 0x014C: Mask ROM version. */
    byte                checksum;             /** @brief 0x014D: Header checksum. */
    word                global_checksum;      /** @brief 0x014E-0x014F: Global checksum. */
};
