/**
 * @file event_input.h
 * @brief Extension header for event.h defining Input Events.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "events/keycodes.h"

enum event_input_type {
    EVENT_INPUT_UNKNOWN = 0,
    EVENT_INPUT_BUTTON,
    EVENT_INPUT_MOTION,
    EVENT_INPUT_AXIS,
};

enum device {
    DEVICE_UNKNOWN  = 0,
    DEVICE_KEYBOARD = 1,
    DEVICE_MOUSE    = 2,
    DEVICE_GAMEPAD  = 3
};

struct event_input_button {
    enum device                  device;
    uint32_t                     device_id;
    uint32_t                     button;
    enum keycodes_modifier_flags modifier;
    bool                         down;
    bool                         repeat;
};

struct event_input_motion {
    enum device device;
    uint32_t    device_id;
    float       x;
    float       y;
    float       x_relative;
    float       y_relative;
};

struct event_input_axis {
    enum device device;
    uint32_t    device_id;
    uint8_t     axis;
    float       x;
    float       y;
};

struct event_input {
    enum event_input_type type;
    union {
        struct event_input_button button;
        struct event_input_motion motion;
        struct event_input_axis   axis;
    };
};
