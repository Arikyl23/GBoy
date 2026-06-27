/**
 * @file memory_map.h
 * @brief Defines the gameboy's memory map
 */
#pragma once

// clang-format off
// Memory Map (Sourced from: https://gbdev.io/pandocs/Memory_Map.html)
//
// Start | End  | Description                    | Notes
// ---------------------------------------------------------------------------------------------------
//  
//  0000 | 3FFF | 16 KiB ROM bank 00             | From cartridge, usually a fixed bank
//  4000 | 7FFF | 16 KiB ROM Bank 01–NN          | From cartridge, switchable bank via mapper (if any)
//  8000 | 9FFF | 8 KiB Video RAM (VRAM)         | In CGB mode, switchable bank 0/1
//  A000 | BFFF | 8 KiB External RAM             | From cartridge, switchable bank if any
//  C000 | CFFF | 4 KiB Work RAM (WRAM)          |
//  D000 | DFFF | 4 KiB Work RAM (WRAM)          | In CGB mode, switchable bank 1–7
//  E000 | FDFF | Echo RAM (mirror of C000–DDFF) | Nintendo says use of this area is prohibited.
//  FE00 | FE9F | Object attribute memory (OAM)  |
//  FEA0 | FEFF | Not Usable                     | Nintendo says use of this area is prohibited.
//  FF00 | FF7F | I/O Registers                  |
//  FF80 | FFFE | High RAM (HRAM)                |
//  FFFF | FFFF | Interrupt Enable register (IE) |
// clang-format on

#define ADDR_CART_ROM_START      0x0000 /** @brief Cartridge ROM start */
#define ADDR_CART_ROM_LOW_START  0x0000 /** @brief Cartridge ROM low bank start */
#define ADDR_BOOT_ROM_START      0x0000 /** @brief Boot ROM start */
#define ADDR_BOOT_ROM_END        0x00FF /** @brief Boot ROM end */
#define ADDR_CART_ROM_LOW_END    0x3FFF /** @brief Cartridge ROM low bank end */
#define ADDR_CART_ROM_HIGH_START 0x4000 /** @brief Cartridge ROM high bank start */
#define ADDR_CART_ROM_HIGH_END   0x7FFF /** @brief Cartridge ROM high bank end */
#define ADDR_CART_ROM_END        0x7FFF /** @brief Cartridge ROM end */
#define ADDR_VRAM_START          0x8000 /** @brief Internal Video RAM start */
#define ADDR_VRAM_TILEDATA_1     0x8000 /** @brief Unsigned Tile Data base */
#define ADDR_VRAM_TILEDATA_0     0x9000 /** @brief Signed Tile Data base */
#define ADDR_VRAM_TILEMAP_0      0x9800 /** @brief Default Tilemap base */
#define ADDR_VRAM_TILEMAP_1      0x9C00 /** @brief Alternative Tilemap base */
#define ADDR_VRAM_END            0x9FFF /** @brief Internal Video RAM end */
#define ADDR_CART_RAM_START      0xA000 /** @brief Cartridge RAM start */
#define ADDR_CART_RAM_END        0xBFFF /** @brief Cartridge RAM end */
#define ADDR_WRAM_START          0xC000 /** @brief Internal Work RAM start */
#define ADDR_WRAM_0_START        0xC000 /** @brief Internal Work RAM Bank 0 start */
#define ADDR_WRAM_0_END          0xCFFF /** @brief Internal Work RAM Bank 0 end */
#define ADDR_WRAM_N_START        0xD000 /** @brief Internal Work RAM Bank N start */
#define ADDR_WRAM_N_END          0xDFFF /** @brief Internal Work RAM Bank N end */
#define ADDR_WRAM_END            0xDFFF /** @brief Internal Work RAM end */
#define ADDR_ECHO_RAM_START      0xE000 /** @brief Internal Echo RAM start */
#define ADDR_ECHO_RAM_END        0xFDFF /** @brief Internal Echo RAM end */
#define ADDR_OAM_START           0xFE00 /** @brief Internal Object Attribute Memory start */
#define ADDR_OAM_END             0xFE9F /** @brief Internal Object Attribute Memory end */
#define ADDR_PROHIBITED_START    0xFEA0 /** @brief Start of unusable address range. Causes Bugs. */
#define ADDR_PROHIBITED_END      0xFEFF /** @brief End of unusable address range. Causes Bugs. */
#define ADDR_IO_REGISTERS_START  0xFF00 /** @brief Internal I/O registers start */
#define ADDR_IO_REGISTERS_END    0xFF7F /** @brief Internal I/O registers end */
#define ADDR_HRAM_START          0xFF80 /** @brief Internal High RAM start */
#define ADDR_HRAM_END            0xFFFE /** @brief Internal High RAM end */
#define ADDR_IE                  0xFFFF /** @brief Interrupt Enabled register */

#define CART_ROM_BANK_SIZE 0x4000
#define CART_RAM_BANK_SIZE 0x2000

/** @brief Size of a single VRAM (Video RAM) bank. */
#define VRAM_BANK_SIZE    (ADDR_VRAM_END - ADDR_VRAM_START + 1)
/** @brief Size of a single WRAM (Work RAM) bank. */
#define WRAM_BANK_SIZE    (ADDR_WRAM_0_END - ADDR_WRAM_0_START + 1)
/** @brief Defines the number of WRAM banks. */
#define WRAM_BANK_COUNT   8
/** @brief Size of the Object Attribute Memory. */
#define OAM_SIZE          (ADDR_OAM_END - ADDR_OAM_START + 1)
/** @brief Size of the I/O Registers RAM */
#define IO_REGISTERS_SIZE (ADDR_IO_REGISTERS_END - ADDR_IO_REGISTERS_START + 1)
/** @brief Size of the High RAM */
#define HRAM_SIZE         (ADDR_HRAM_END - ADDR_HRAM_START + 1)
