/**
 * @file window_texture.h
 * @brief API for handling Textures.
 */
#pragma once

#include <stddef.h>

#include "display/pixel.h"
#include "display/rect.h"
#include "display/texture_type.h"

// Opaque Decls
struct window;
struct texture;

struct texture* texture_create(
    struct window*          window,
    const size_t            width,
    const size_t            height,
    const enum texture_type type
);
void texture_destroy(struct texture** p_texture);

int texture_get_width(const struct texture* texture);
int texture_get_height(const struct texture* texture);

int texture_update(struct texture* texture, const pixel_t* pixel_data, const size_t count);

int texture_draw(struct window* window, const struct texture* texture);
int texture_draw_at(
    const struct window*  window,
    const struct texture* texture,
    const struct rect*    dst
);
int texture_draw_region(
    const struct window*  window,
    const struct texture* texture,
    const struct rect*    src
);
int texture_draw_region_at(
    const struct window*  window,
    const struct texture* texture,
    const struct rect*    src,
    const struct rect*    dst
);
