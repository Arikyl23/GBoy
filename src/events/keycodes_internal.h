#pragma once

#include <SDL3/SDL.h>
#include <stdint.h>

#include "events/keycodes.h"

uint32_t                     keycode_from_sdl_scancode(const SDL_Scancode code);
uint32_t                     keycode_from_sdl_mouse_button(const SDL_MouseButtonFlags code);
uint32_t                     keycode_from_sdl_gamepad(const SDL_GamepadButton code);
enum keycodes_modifier_flags keycode_modifier_from_sdl(const SDL_Keymod mod);
