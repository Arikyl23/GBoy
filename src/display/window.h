/**
 * @file window.h
 * @brief API for creating window instances.
 */
#pragma once

#include <stddef.h>

#include "display/colour_t.h"
#include "display/rect.h"
#include "events/event.h"

// Opaque decl
struct window;

struct window* window_create(
    const char*  title,
    const size_t width,
    const size_t height,
    const size_t scale
);
void window_destroy(struct window** p_window);

void window_register_event_handler(struct window* window, void (*cb)(const struct event* evt));

// ========== Rendering ==========

int window_clear(struct window* window);
int window_present(struct window* window);

// ----- Primatives -----

int window_fill(struct window* window, colour_t colour);
int window_draw_line(
    struct window* window,
    float          x1,
    float          y1,
    float          x2,
    float          y2,
    colour_t       colour
);
int window_draw_rect(struct window* window, struct rect rect, colour_t colour);
int window_draw_rect_filled(struct window* window, struct rect rect, colour_t colour);
int window_draw_point(struct window* window, float x, float y, colour_t colour);

// ----- Primatives -----

// ========== Rendering ==========
