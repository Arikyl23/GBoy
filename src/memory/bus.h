/**
 * @file bus.h
 * @brief File containing the Enumeration definition for source buses and various utility functions
 * for it.
 */
#pragma once

#include <stdbool.h>

enum bus { /** @brief Enumeration representing which processor is accessing the Address Bus. */
    BUS_CPU = 0, /** @brief Address Bus access from the CPU. */
    BUS_PPU = 1, /** @brief Address Bus access from the PPU*/
    BUS_MAX = 2  /** @brief Max Enumeration Value. @warning Not a real value. Do not use. */
};

/**
 * @brief Converts a bus enumeration into a readable string representing its value.
 * @param value Enumeration to be converted to a readable string.
 * @returns cstring containing readable text. If enumeration doesn't exist, returns "unknown".
 */
static inline const char* bus_to_string(const enum bus value) {
    switch (value) {
    case BUS_CPU:
        return "CPU";
    case BUS_PPU:
        return "PPU";
    default:
        return "unknown";
    }
}

/**
 * @brief Validates if the value is a defined bus enumeration value.
 * @param value Value to validate.
 * @returns `true` if defined; otherwise, `false`
 */
static inline bool bus_is_valid(const enum bus value) { return value >= 0 && value < BUS_MAX; }
