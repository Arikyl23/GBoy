/**
 * @file ram_size.h
 * @brief Defines the Ram Size Enumeration.
 */
#pragma once

#include <stdbool.h>

// clang-format off
// Sourced from: https://gbdev.io/pandocs/The_Cartridge_Header.html
// Code	SRAM size	Comment
// ------------------------
// 0x00 0           No RAM
// 0x01 –           Unused
// 0x02 8     KiB   1 bank
// 0x03 32    KiB   4 banks of 8 KiB each
// 0x04 128   KiB   16 banks of 8 KiB each
// 0x05 64    KiB   8 banks of 8 KiB each
// clang-format on

/** @brief Enumeration representing the RAM size. */
enum ram_size {
    /** @brief No RAM. */
    RAM_NONE    = 0x00,
    /** @brief RAM is not used. */
    RAM_UNUSED  = 0x01,
    /** @brief 1 Bank of 8 KiB. */
    RAM_8_KiB   = 0x02,
    /** @brief 4 Banks of 8 KiB. */
    RAM_32_KiB  = 0x03,
    /** @brief 16 Banks of 8 KiB. */
    RAM_128_KiB = 0x04,
    /** @brief 8 Banks of 8 KiB. */
    RAM_64_KiB  = 0x05
};

/**
 * @brief Converts an enumeration to a readable cstring with the same name.
 * @param type Value to convert.
 * @returns Static cstring containing the readable name.
 */
inline const char* ram_size_to_string(const enum ram_size size) {
    switch (size) {
    case RAM_NONE:
        return "NONE";
    case RAM_UNUSED:
        return "UNUSED";
    case RAM_8_KiB:
        return "8_KiB";
    case RAM_32_KiB:
        return "32_KiB";
    case RAM_128_KiB:
        return "128_KiB";
    case RAM_64_KiB:
        return "64_KiB";
    default:
        return "unknown";
    }
}

/**
 * @brief Validates if the given value is defined.
 * @param type Value to validate.
 * @returns true if defined; otherwise, false.
 */
inline bool ram_size_is_valid(const enum ram_size size) {
    return (int)size >= 0 && (int)size < 0x06;
}

/**
 * @brief Gets the number of banks the enumeration represents.
 * @param size Value to convert.
 * @returns The number of RAM banks.
 */
inline int ram_size_bank_count(const enum ram_size size) {
    switch (size) {
    default:
    case RAM_NONE:
    case RAM_UNUSED:
        return 0;
    case RAM_8_KiB:
        return 1;
    case RAM_32_KiB:
        return 4;
    case RAM_128_KiB:
        return 16;
    case RAM_64_KiB:
        return 8;
    }
}
