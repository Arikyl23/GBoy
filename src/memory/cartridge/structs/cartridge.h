/**
 * @file cartridge.h
 * @brief Contains the structure definition for the Cartridge
 */
#pragma once

#include "memory/cartridge/mbc/mbc.h"
#include "memory/cartridge/structs/rom_header.h"

struct cartridge {            /** @brief Gameboy Cartridge. */
    struct rom_header header; /** @brief ROM header. */
    struct mbc*       mbc;    /** @brief Memory Bank Controller */

    // Data Paths
    char* rom_path;  /** @brief Path to the file containing the ROM. @note SDL Managed Data. */
    char* ram_path;  /** @brief Path to the file containing the RAM. @note SDL Managed Data. */
    char* save_path; /** @brief Path to the save directory. @note SDL Managed Data. */
};
