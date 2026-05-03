/**
 * @file cartridge_type.h
 * @brief Defines the Cartridge Type Enumeration.
 */
#pragma once

#include <stdbool.h>

// Sourced from: https://gbdev.io/pandocs/The_Cartridge_Header.html
// Code	Type
// ---------
// $00	ROM ONLY
// $01	MBC1
// $02	MBC1+RAM
// $03	MBC1+RAM+BATTERY
// $05	MBC2
// $06	MBC2+BATTERY
// $08	ROM+RAM 11
// $09	ROM+RAM+BATTERY 11
// $0B	MMM01
// $0C	MMM01+RAM
// $0D	MMM01+RAM+BATTERY
// $0F	MBC3+TIMER+BATTERY
// $10	MBC3+TIMER+RAM+BATTERY 12
// $11	MBC3
// $12	MBC3+RAM 12
// $13	MBC3+RAM+BATTERY 12
// $19	MBC5
// $1A	MBC5+RAM
// $1B	MBC5+RAM+BATTERY
// $1C	MBC5+RUMBLE
// $1D	MBC5+RUMBLE+RAM
// $1E	MBC5+RUMBLE+RAM+BATTERY
// $20	MBC6
// $22	MBC7+SENSOR+RUMBLE+RAM+BATTERY
// $FC	POCKET CAMERA
// $FD	BANDAI TAMA5
// $FE	HuC3
// $FF	HuC1+RAM+BATTERY

/** @brief Enumeration representing the Cartridge Type */
enum cartridge_type {
    /** @brief Discrete Logic Decoder with no RAM. */
    CART_ROM_ONLY                       = 0x00,
    /** @brief First MBC release with no RAM. */
    CART_MBC1                           = 0x01,
    /** @brief First MBC release with RAM. */
    CART_MBC1_RAM                       = 0x02,
    /** @brief First MBC release with persistent RAM. */
    CART_MBC1_RAM_BATTERY               = 0x03,
    CART_MBC2                           = 0x05,
    CART_MBC2_BATTERY                   = 0x06,
    /** @brief Discrete Logic Decoder with RAM. */
    CART_ROM_RAM                        = 0x08,
    /** @brief Discrete Logic Decoder with persistent RAM. */
    CART_ROM_RAM_BATTERY                = 0x09,
    CART_MMM01                          = 0x0B,
    CART_MMM01_RAM                      = 0x0C,
    CART_MMM01_RAM_BATTERY              = 0x0D,
    CART_MBC3_TIMER_BATTERY             = 0x0F,
    CART_MBC3_TIMER_RAM_BATTERY         = 0x10,
    CART_MBC3                           = 0x11,
    CART_MBC3_RAM                       = 0x12,
    CART_MBC3_RAM_BATTERY               = 0x13,
    CART_MBC5                           = 0x19,
    CART_MBC5_RAM                       = 0x1A,
    CART_MBC5_RAM_BATTERY               = 0x1B,
    CART_MBC5_RUMBLE                    = 0x1C,
    CART_MBC5_RUMBLE_RAM                = 0x1D,
    CART_MBC5_RUMBLE_RAM_BATTERY        = 0x1E,
    CART_MBC6                           = 0x20,
    CART_MBC7_SENSOR_RUMBLE_RAM_BATTERY = 0x22,
    CART_POCKET_CAMERA                  = 0xFC,
    CART_BANDAI_TAMA5                   = 0xFD,
    CART_HuC3                           = 0xFE,
    CART_HuC1_RAM_BATTERY               = 0xFF
};

/**
 * @brief Converts an enumeration to a readable cstring with the same name.
 * @param type Value to convert.
 * @returns Static cstring containing the readable name.
 */
static inline const char* cartridge_type_to_string(const enum cartridge_type type) {
    switch (type) {
    case CART_ROM_ONLY:
        return "ROM_ONLY";
    case CART_MBC1:
        return "MBC1";
    case CART_MBC1_RAM:
        return "MBC1_RAM";
    case CART_MBC1_RAM_BATTERY:
        return "MBC1_RAM_BATTERY";
    case CART_MBC2:
        return "MBC2";
    case CART_MBC2_BATTERY:
        return "MBC2_BATTERY";
    case CART_ROM_RAM:
        return "ROM_RAM";
    case CART_ROM_RAM_BATTERY:
        return "ROM_RAM_BATTERY";
    case CART_MMM01:
        return "MMM01";
    case CART_MMM01_RAM:
        return "MMM01_RAM";
    case CART_MMM01_RAM_BATTERY:
        return "MMM01_RAM_BATTERY";
    case CART_MBC3_TIMER_BATTERY:
        return "MBC3_TIMER_BATTERY";
    case CART_MBC3_TIMER_RAM_BATTERY:
        return "MBC3_TIMER_RAM_BATTERY";
    case CART_MBC3:
        return "MBC3";
    case CART_MBC3_RAM:
        return "MBC3_RAM";
    case CART_MBC3_RAM_BATTERY:
        return "MBC3_RAM_BATTERY";
    case CART_MBC5:
        return "MBC5";
    case CART_MBC5_RAM:
        return "MBC5_RAM";
    case CART_MBC5_RAM_BATTERY:
        return "MBC5_RAM_BATTERY";
    case CART_MBC5_RUMBLE:
        return "MBC5_RUMBLE";
    case CART_MBC5_RUMBLE_RAM:
        return "MBC5_RUMBLE_RAM";
    case CART_MBC5_RUMBLE_RAM_BATTERY:
        return "MBC5_RUMBLE_RAM_BATTERY";
    case CART_MBC6:
        return "MBC6";
    case CART_MBC7_SENSOR_RUMBLE_RAM_BATTERY:
        return "MBC7_SENSOR_RUMBLE_RAM_BATTERY";
    case CART_POCKET_CAMERA:
        return "POCKET_CAMERA";
    case CART_BANDAI_TAMA5:
        return "BANDAI_TAMA5";
    case CART_HuC3:
        return "HuC3";
    case CART_HuC1_RAM_BATTERY:
        return "HuC1_RAM_BATTERY";
    default:
        return "unknown";
    }
}

/**
 * @brief Validates if the given value is defined.
 * @param type Value to validate.
 * @returns true if defined; otherwise, false.
 */
static inline bool cartridge_type_is_valid(const enum cartridge_type type) {
    switch (type) {
    case CART_ROM_ONLY:
    case CART_MBC1:
    case CART_MBC1_RAM:
    case CART_MBC1_RAM_BATTERY:
    case CART_MBC2:
    case CART_MBC2_BATTERY:
    case CART_ROM_RAM:
    case CART_ROM_RAM_BATTERY:
    case CART_MMM01:
    case CART_MMM01_RAM:
    case CART_MMM01_RAM_BATTERY:
    case CART_MBC3_TIMER_BATTERY:
    case CART_MBC3_TIMER_RAM_BATTERY:
    case CART_MBC3:
    case CART_MBC3_RAM:
    case CART_MBC3_RAM_BATTERY:
    case CART_MBC5:
    case CART_MBC5_RAM:
    case CART_MBC5_RAM_BATTERY:
    case CART_MBC5_RUMBLE:
    case CART_MBC5_RUMBLE_RAM:
    case CART_MBC5_RUMBLE_RAM_BATTERY:
    case CART_MBC6:
    case CART_MBC7_SENSOR_RUMBLE_RAM_BATTERY:
    case CART_POCKET_CAMERA:
    case CART_BANDAI_TAMA5:
    case CART_HuC3:
    case CART_HuC1_RAM_BATTERY:
        return true;
    default:
        return false;
    }
}

/**
 * @brief Checks if the Cartridge Type contains a battery.
 * @param type Value to check.
 * @returns true if it contains a battery, false otherwise.
 */
static inline bool cartridge_type_contains_battery(const enum cartridge_type type) {
    switch (type) {
    case CART_MBC1_RAM_BATTERY:
    case CART_MBC2_BATTERY:
    case CART_ROM_RAM_BATTERY:
    case CART_MMM01_RAM_BATTERY:
    case CART_MBC3_TIMER_BATTERY:
    case CART_MBC3_TIMER_RAM_BATTERY:
    case CART_MBC3_RAM_BATTERY:
    case CART_MBC5_RAM_BATTERY:
    case CART_MBC5_RUMBLE_RAM_BATTERY:
    case CART_MBC7_SENSOR_RUMBLE_RAM_BATTERY:
    case CART_HuC1_RAM_BATTERY:
        return true;
    case CART_ROM_ONLY:
    case CART_MBC1:
    case CART_MBC1_RAM:
    case CART_MBC2:
    case CART_ROM_RAM:
    case CART_MMM01:
    case CART_MMM01_RAM:
    case CART_MBC3:
    case CART_MBC3_RAM:
    case CART_MBC5:
    case CART_MBC5_RAM:
    case CART_MBC5_RUMBLE:
    case CART_MBC5_RUMBLE_RAM:
    case CART_MBC6:
    case CART_POCKET_CAMERA:
    case CART_BANDAI_TAMA5:
    case CART_HuC3:
    default:
        return false;
    }
}
