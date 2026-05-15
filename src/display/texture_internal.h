/**
 * @file texture_internal.h
 * @brief Internal API for Textures. Used by other modules to pass SDL related resources.
 */
#pragma once

#include <SDL3/SDL.h>

struct texture* texture_create_from_sdl_texture(SDL_Texture* sdl_texture);