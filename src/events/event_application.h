/**
 * @file event_application.h
 * @brief Extension header for event.h defining Application Events.
 */
#pragma once

enum event_application_type {
    EVENT_APPLICATION_UNKNOWN = 0,
    EVENT_APPLICATION_QUIT,
    EVENT_APPLICATION_TERMINATING,
    EVENT_APPLICATION_LOW_MEMORY,
};

struct event_application {
    enum event_application_type type;
};