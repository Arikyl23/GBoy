/**
 * @file fetcher.h
 * @brief Public API for a Gameboy PPU pixel fetcher.
 */
#pragma once

#include <stdbool.h>

#include "DEFINES.h"

enum fetcher_state {
    FETCHER_IDLE,
    FETCHER_FETCHING,
    FETCHER_READY
};

static inline const char* fetcher_state_to_string(const enum fetcher_state state) {
    switch (state) {
    case FETCHER_IDLE:
        return "IDLE";
    case FETCHER_FETCHING:
        return "FETCHING";
    case FETCHER_READY:
        return "READY";
    default:
        return "unknown";
    }
}

void               fetcher_reset(void);
void               fetcher_set_fetch_address(const word addr);
enum fetcher_state fetcher_step(void);
word               fetcher_get_pixels(void);
