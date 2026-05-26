/**
 * @file text.c
 * @brief Implementation for Text rendering.
 */
#include "display/textbox.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <log.h>

#include "display/rect.h"
#include "display/texture_internal.h"
#include "display/window_internal.h"

LOG_MODULE_SETUP("Textbox", CONFIG_TEXTBOX_MODULE_LOG_LEVEL)

#define LOG_SDL_ERROR(msg)                                                                         \
    do {                                                                                           \
        log_error(msg "\n\tReason: %s", SDL_GetError());                                           \
        SDL_ClearError();                                                                          \
    } while (0);

struct textbox {
    TTF_Text* sdl_text;
    TTF_Font* sdl_font;
    float     size;
    colour_t  colour;
};

struct textbox* textbox_create(
    const struct window* window,
    const struct asset*  font_asset,
    const float          text_size,
    const colour_t       text_colour,
    const char*          text
) {
    if (font_asset == NULL) {
        log_error("Failed to create Textbox. No font asset provided.");
        return NULL;
    }

    if (TTF_Init() == false) {
        LOG_SDL_ERROR("Failed to initialize SDL_TTF");
        return NULL;
    }

    struct textbox* textbox = SDL_calloc(1, sizeof(struct textbox));
    if (textbox == NULL) {
        LOG_SDL_ERROR("Failed to create Textbox. SDL failed to allocate handle.");
        textbox_destroy(&textbox);
        return NULL;
    }

    SDL_IOStream* font_file_stream = SDL_IOFromConstMem(font_asset->data, font_asset->size);
    if (font_file_stream == NULL) {
        LOG_SDL_ERROR("Failed to create Textbox. SDL failed to create font.");
        textbox_destroy(&textbox);
        return NULL;
    }
    textbox->sdl_font = TTF_OpenFontIO(font_file_stream, true, text_size);
    if (textbox->sdl_font == NULL) {
        LOG_SDL_ERROR("Failed to create Textbox. SDL failed to create font.");
        textbox_destroy(&textbox);
        return NULL;
    }

    textbox->sdl_text = TTF_CreateText(window_get_text_engine(window), textbox->sdl_font, text, 0);
    if (textbox->sdl_text == NULL) {
        LOG_SDL_ERROR("Failed to create Textbox. SDL failed to create text object.");
        textbox_destroy(&textbox);
        return NULL;
    }

    textbox->size   = text_size;
    textbox->colour = text_colour;

    return textbox;
}

void textbox_destroy(struct textbox** p_textbox) {
    if (p_textbox == NULL || *p_textbox == NULL) { return; }

    struct textbox* textbox = *p_textbox;
    if (textbox->sdl_text != NULL) { TTF_DestroyText(textbox->sdl_text); }
    if (textbox->sdl_font != NULL) { TTF_CloseFont(textbox->sdl_font); }
    SDL_free(textbox);
    *p_textbox = NULL;

    TTF_Quit();

    return;
}

bool textbox_set_text(struct textbox* textbox, const char* text) {
    if (textbox == NULL) {
        log_error("Failed to set text. No textbox given.");
        return false;
    }
    if (text == NULL) { log_warn("No text given. Defaulting to \"\""); }

    // Safe to pass text as NULL in this function (uses default "" string)
    if (TTF_SetTextString(textbox->sdl_text, text, 0) == false) {
        LOG_SDL_ERROR("Failed to set text.");
        return false;
    }

    return true;
}

bool textbox_set_font(struct textbox* textbox, const struct asset* font_asset) {
    if (textbox == NULL) {
        log_error("Failed to set font. No textbox given.");
        return false;
    }
    if (font_asset == NULL) {
        log_error("Failed to set font. No font asset given.");
        return false;
    }

    SDL_IOStream* stream = SDL_IOFromConstMem(font_asset->data, font_asset->size);
    if (stream == NULL) {
        LOG_SDL_ERROR("Failed to set font. Could not open new font.");
        return false;
    }

    TTF_Font* font = TTF_OpenFontIO(stream, true, textbox->size);
    if (font == NULL) {
        LOG_SDL_ERROR("Failed to set font. Could not create new font.");
        return false;
    }

    if (TTF_SetTextFont(textbox->sdl_text, font) == false) {
        LOG_SDL_ERROR("Failed to set font.");
        TTF_CloseFont(font);
        return false;
    }

    TTF_CloseFont(textbox->sdl_font);
    textbox->sdl_font = font;

    return true;
}

bool textbox_set_font_size(struct textbox* textbox, const float size) {
    if (textbox == NULL) {
        log_error("Failed to set text size. No textbox given.");
        return false;
    }

    if (TTF_SetFontSize(textbox->sdl_font, size) == false) {
        LOG_SDL_ERROR("Failed to set text size.");
        return false;
    }

    textbox->size = size;

    return true;
}

bool textbox_set_colour(struct textbox* textbox, const colour_t colour) {
    if (textbox == NULL) {
        log_error("Failed to set text colour. No textbox given.");
        return false;
    }

    if (TTF_SetTextColor(textbox->sdl_text, colour.r, colour.g, colour.b, colour.a) == false) {
        LOG_SDL_ERROR("Failed to set text colour.");
        return false;
    }

    textbox->colour = colour;

    return true;
}

const char* textbox_get_text(struct textbox* textbox) {
    if (textbox == NULL) { return ""; }
    return textbox->sdl_text->text;
}

float textbox_get_font_size(struct textbox* textbox) {
    if (textbox == NULL) { return 0; }
    return textbox->size;
}

colour_t textbox_get_colour(struct textbox* textbox) {
    if (textbox == NULL) { return (colour_t){0}; }
    return textbox->colour;
}

bool textbox_get_size(struct textbox* textbox, float* width, float* height) {
    if (textbox == NULL) {
        log_error("Failed to get size of textbox. No textbox was given.");
        return false;
    }

    int w, h;
    if (TTF_GetTextSize(textbox->sdl_text, &w, &h) == false) {
        LOG_SDL_ERROR("Failed to get size of textbox.");
        return false;
    }

    if (width != NULL) { *width = w; }
    if (height != NULL) { *height = h; }

    return true;
}

bool textbox_draw(struct textbox* textbox, const float x, const float y) {
    if (textbox == NULL) {
        log_error("Failed to draw Textbox. No Textbox to draw.");
        return false;
    }

    if (TTF_DrawRendererText(textbox->sdl_text, x, y) == false) {
        LOG_SDL_ERROR("Failed to draw Textbox.");
        return false;
    }

    return true;
}
