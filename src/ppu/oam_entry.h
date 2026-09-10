/**
 * @file oam_entry.h
 * @brief Defines OAM Entry objects and common API.
 */
#pragma once

#include "DEFINES.h"
#include "tile_attribute_bitflag.h"

struct oam_entry {
    byte y_pos;
    byte x_pos;
    byte tile_index;
    byte attributes;
};
