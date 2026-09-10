/**
 * @file event_internal.h
 * @brief Internal Header for handling SDL events and dispatching to the correct resource (aka
 * window).
 */
#pragma once

#include <SDL3/SDL.h>

#include "events/event.h"

enum sdl_event_simple {
    SDL_EVENT_SIMPLE_UNKNOWN     = 0,
    SDL_EVENT_SIMPLE_APPLICATION = 1,
    SDL_EVENT_SIMPLE_DISPLAY,
    SDL_EVENT_SIMPLE_WINDOW,
    SDL_EVENT_SIMPLE_KEY,
    SDL_EVENT_SIMPLE_BUTTON, // Refers to mouse button
    SDL_EVENT_SIMPLE_MOTION, // Refers to mouse motion
    SDL_EVENT_SIMPLE_JOYSTICK,
    SDL_EVENT_SIMPLE_GAMEPAD,
    SDL_EVENT_SIMPLE_RENDER
};

int  event_register_window(struct window* window);
void event_deregister_window(const struct window* window);

enum sdl_event_simple event_get_simple_sdl_event_type(const SDL_EventType type);
struct event          event_convert_sdl_evt(const SDL_Event* evt);
