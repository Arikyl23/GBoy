/**
 * @file window_internal.h
 * @brief Internal Header for Window Objects.
 */
#pragma once

#include <SDL3/SDL.h>

struct window;

SDL_Renderer* window_get_renderer(const struct window* window);
SDL_WindowID  window_get_id(const struct window* window);

void window_handle_event(struct window* window, const SDL_Event* evt);
