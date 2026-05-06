/**
 * @file event_window.h
 * @brief Extension header for event.h defining Window Events.
 */
#pragma once

#include <stdint.h>

enum event_window_type {
    EVENT_WINDOW_UNKNOWN = 0,
    EVENT_WINDOW_SHOWN,
    EVENT_WINDOW_HIDDEN,
    EVENT_WINDOW_EXPOSED,
    EVENT_WINDOW_MOVED,
    EVENT_WINDOW_RESIZED,
    EVENT_WINDOW_MINIMIZED,
    EVENT_WINDOW_MAXIMIZED,
    EVENT_WINDOW_MOUSE_ENTER,
    EVENT_WINDOW_MOUSE_LEAVE,
    EVENT_WINDOW_FOCUS_GAINED,
    EVENT_WINDOW_FOCUS_LOST,
    EVENT_WINDOW_CLOSE_REQUESTED,
    EVENT_WINDOW_ENTER_FULLSCREEN,
    EVENT_WINDOW_LEAVE_FULLSCREEN,
    EVENT_WINDOW_DESTROYED
};

struct event_window {
    enum event_window_type type;
    int32_t                x;
    int32_t                y;
};