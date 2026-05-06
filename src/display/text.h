/**
 * @file text.h
 * @brief Texture API extension for rendering text into textures.
 */
#pragma once

#include "display/colour.h"
#include "display/texture.h"

struct font;

struct font* font_create(const char* path, float size);
void         font_destroy(struct font** p_font);

struct texture* text_render(
    struct window*     window,
    const struct font* font,
    const char*        text,
    colour_t           colour
);
