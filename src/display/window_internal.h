/**
 * @file window_internal.h
 * @brief Internal Header for Window Objects.
 */
#pragma once

#include <SDL3/SDL.h>

struct window;

SDL_Renderer* window_get_renderer(const struct window* window);