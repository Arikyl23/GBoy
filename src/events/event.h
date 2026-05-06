/**
 * @file event.h
 * @brief API for handling Events.
 */
#pragma once

#include "events/event_application.h"
#include "events/event_input.h"
#include "events/event_window.h"

enum event_type {
    EVENT_UNKNOWN = 0,
    EVENT_APPLICATION,
    EVENT_WINDOW,
    EVENT_INPUT
};

struct event {
    enum event_type type;
    union {
        struct event_application application;
        struct event_window      window;
        struct event_input       input;
    };
};

void event_register_application_event_handler(
    void (*application_event_handler)(const struct event* evt)
);
void event_deregister_application_event_handler(void);
void event_poll(void);
