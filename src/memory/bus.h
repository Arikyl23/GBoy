/**
 * @file bus.h
 * @brief File containing the Enumeration definition for source buses and various utility functions
 * for it.
 */
#pragma once

#include <stdbool.h>

#define TABLE(X)                                                                                   \
    X(BUS_CPU, 0, "CPU")                                                                           \
    X(BUS_PPU, 1, "PPU")

/** @brief Enumeration representing which processor is accessing the Address Bus. */
enum bus {
#define X(id, code, str) id = code,
    TABLE(X)
#undef X
};

/**
 * @brief Converts a bus enumeration into a readable string representing its value.
 * @param value Enumeration to be converted to a readable string.
 * @returns cstring containing readable text. If enumeration doesn't exist, returns "unknown".
 */
static inline const char* bus_to_string(const enum bus value) {
    switch (value) {
#define X(id, code, str)                                                                           \
    case code:                                                                                     \
        return str;
        TABLE(X)
#undef X
    default:
        return "unknown";
    }
}

#undef TABLE
