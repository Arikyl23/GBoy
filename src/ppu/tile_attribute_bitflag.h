/**
 * @file tilemap.h
 * @brief Defines generic Gameboy Tilemaps.
 */
#pragma once

enum tile_attribute_bitflag {
    TILE_ATTR_CBG_PALETTE = 0b00000111,
    TILE_ATTR_BANK        = 0b00001000,
    TILE_ATTR_DMG_PALETTE = 0b00010000,
    TILE_ATTR_FLIP_X      = 0b00100000,
    TILE_ATTR_FLIP_Y      = 0b01000000,
    TILE_ATTR_PRIORITY    = 0b10000000
};