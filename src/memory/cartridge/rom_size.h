/**
 * @file rom_size.h
 * @brief Defines the Rom Size Enumeration.
 */
#pragma once

#include <stdbool.h>

// clang-format off
// Sourced from: https://gbdev.io/pandocs/The_Cartridge_Header.html
// Value	ROM size	Number of ROM banks
// ----------------------------------------
// 0x00     32   KiB     2 (no banking)
// 0x01     64   KiB     4
// 0x02     128  KiB     8
// 0x03     256  KiB     16
// 0x04     512  KiB     32
// 0x05     1    MiB     64
// 0x06     2    MiB     128
// 0x07     4    MiB     256
// 0x08     8    MiB     512
// 0x52     1.1  MiB	 72
// 0x53     1.2  MiB	 80
// 0x54     1.5  MiB	 96
// clang-format on

/** @brief Enumeration representing the ROM size. */
enum rom_size {
    /** @brief 2 Banks of 16 KiB. */
    ROM_32_KiB  = 0x00,
    /** @brief 4 Banks of 16 KiB. */
    ROM_64_KiB  = 0x01,
    /** @brief 8 Banks of 16 KiB. */
    ROM_128_KiB = 0x02,
    /** @brief 16 Banks of 16 KiB. */
    ROM_256_KiB = 0x03,
    /** @brief 32 Banks of 16 KiB. */
    ROM_512_KiB = 0x04,
    /** @brief 64 Banks of 16 KiB. */
    ROM_1_MiB   = 0x05,
    /** @brief 128 Banks of 16 KiB. */
    ROM_2_MiB   = 0x06,
    /** @brief 256 Banks of 16 KiB. */
    ROM_4_MiB   = 0x07,
    /** @brief 512 Banks of 16 KiB. */
    ROM_8_MiB   = 0x08,
    /** @brief 72 Banks of 16 KiB. */
    ROM_1_1_MiB = 0x52,
    /** @brief 80 Banks of 16 KiB. */
    ROM_1_2_MiB = 0x53,
    /** @brief 96 Banks of 16 KiB. */
    ROM_1_5_MiB = 0x54
};

/**
 * @brief Converts an enumeration to a readable cstring with the same name.
 * @param type Value to convert.
 * @returns Static cstring containing the readable name.
 */
static inline const char* rom_size_to_string(const enum rom_size size) {
    switch (size) {
    case ROM_32_KiB:
        return "32 KiB";
    case ROM_64_KiB:
        return "64 KiB";
    case ROM_128_KiB:
        return "128 KiB";
    case ROM_256_KiB:
        return "256 KiB";
    case ROM_512_KiB:
        return "512 KiB";
    case ROM_1_MiB:
        return "1 MiB";
    case ROM_2_MiB:
        return "2 MiB";
    case ROM_4_MiB:
        return "4 MiB";
    case ROM_8_MiB:
        return "8 MiB";
    case ROM_1_1_MiB:
        return "1.1 MiB";
    case ROM_1_2_MiB:
        return "1.2 MiB";
    case ROM_1_5_MiB:
        return "1.5 MiB";
    default:
        return "unknown";
    }
}

/**
 * @brief Validates if the given value is defined.
 * @param type Value to validate.
 * @returns true if defined; otherwise, false.
 */
static inline bool rom_size_is_valid(const enum rom_size size) {
    switch (size) {
    case ROM_32_KiB:
    case ROM_64_KiB:
    case ROM_128_KiB:
    case ROM_256_KiB:
    case ROM_512_KiB:
    case ROM_1_MiB:
    case ROM_2_MiB:
    case ROM_4_MiB:
    case ROM_8_MiB:
    case ROM_1_1_MiB:
    case ROM_1_2_MiB:
    case ROM_1_5_MiB:
        return true;
    default:
        return false;
    }
}

/**
 * @brief Gets the number of banks the enumeration represents.
 * @param size Value to convert.
 * @returns The number of ROM banks.
 */
static inline int rom_size_bank_count(const enum rom_size size) {
    switch (size) {
    case ROM_32_KiB:
        return 2;
    case ROM_64_KiB:
        return 4;
    case ROM_128_KiB:
        return 8;
    case ROM_256_KiB:
        return 16;
    case ROM_512_KiB:
        return 32;
    case ROM_1_MiB:
        return 64;
    case ROM_2_MiB:
        return 128;
    case ROM_4_MiB:
        return 256;
    case ROM_8_MiB:
        return 512;
    case ROM_1_1_MiB:
        return 72;
    case ROM_1_2_MiB:
        return 80;
    case ROM_1_5_MiB:
        return 96;
    default:
        return 0;
    }
}
