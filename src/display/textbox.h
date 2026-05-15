/**
 * @file text.h
 * @brief Texture API extension for rendering text into textures.
 */
#pragma once

#include <stdbool.h>

#include "assets.h"
#include "display/colour.h"
#include "display/window.h"

/** @brief Represents a collection of text that share a common font. */
struct textbox;

/**
 * @brief Creates a new Textbox instance.
 * @param font_asset Asset that holds the font data.
 * @param text_size Size that the text should be rendered at
 * @returns A valid textbox with "" inside.
 */
struct textbox* textbox_create(
    const struct window* window,
    const struct asset*  font_asset,
    const float          text_size,
    const colour_t       text_colour,
    const char*          text
);
/**
 * @brief Destroys a Textbox instance.
 * @param p_textbox Pointer to a Textbox instance handle.
 * @note Safe to call on already destroyed objects. Sets the handle p_textbox points to to `NULL`
 * afterwards.
 */
void textbox_destroy(struct textbox** p_textbox);

bool textbox_set_text(struct textbox* textbox, const char* text);
bool textbox_set_font(struct textbox* textbox, const struct asset* font_asset);
bool textbox_set_font_size(struct textbox* textbox, const float size);
bool textbox_set_colour(struct textbox* textbox, const colour_t colour);

const char* textbox_get_text(struct textbox* textbox);
float       textbox_get_font_size(struct textbox* textbox);
colour_t    textbox_get_colour(struct textbox* textbox);
bool        textbox_get_size(struct textbox* textbox, float* x, float* y);

bool textbox_draw(struct textbox* textbox, const float x, const float y);
