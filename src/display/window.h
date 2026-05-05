/**
 * @file window.h
 * @brief API for creating window instances.
 */
#pragma once

#include <stddef.h>

// Opaque decl
struct window;

struct window* window_create(
    const char*  title,
    const size_t width,
    const size_t height,
    const size_t scale
);
void window_destroy(struct window** p_window);

int window_clear(struct window* window);
int window_present(struct window* window);
