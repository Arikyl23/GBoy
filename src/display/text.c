/**
 * @file text.c
 * @brief Implementation for Text rendering.
 */
#include "display/text.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <log.h>

#include "display/texture_internal.h"
#include "display/window_internal.h"

LOG_MODULE_SETUP("Text", CONFIG_TEXT_MODULE_LOG_LEVEL)

#define LOG_SDL_ERROR(msg)                                                                         \
    do {                                                                                           \
        log_error(msg "\n\tReason: %s", SDL_GetError());                                           \
        SDL_ClearError();                                                                          \
    } while (0);

struct font {
    TTF_Font* sdl_font;
};

struct font* font_create(const char* font_data, const size_t font_data_len, const float font_size) {
    struct font*  rtn        = NULL;
    SDL_IOStream* filestream = NULL;

    if (font_data == NULL) {
        log_error("Font data was NULL.");
        return NULL;
    }
    if (font_data_len == 0) {
        log_error("No font data to load");
        return NULL;
    }

    if (TTF_Init() == false) {
        LOG_SDL_ERROR("SDL TTF failed to initialize.");
        return NULL;
    }

    rtn = SDL_calloc(sizeof(struct font), 1);
    if (rtn == NULL) {
        LOG_SDL_ERROR("SDL failed to allocate handle for font.");
        goto err_cleanup;
    }

    filestream = SDL_IOFromConstMem(font_data, font_data_len);
    if (filestream == NULL) {
        LOG_SDL_ERROR("SDL failed to construct a stream from the font data");
        goto err_cleanup;
    }

    // NOTE: No need to cleanup SDL_IOStream object as TTF_OpenFontIO takes ownership on call.
    //       Even in the event of this function failing, the IOStream will be cleaned up.
    rtn->sdl_font = TTF_OpenFontIO(filestream, true, font_size);
    if (rtn->sdl_font == NULL) {
        LOG_SDL_ERROR("SDL failed to open the requested font.");
        goto err_cleanup;
    }

    return rtn;

err_cleanup:
    font_destroy(&rtn);
    return NULL;
}

void font_destroy(struct font** p_font) {
    if (p_font == NULL || *p_font == NULL) { return; }

    struct font* font = *p_font;
    if (font->sdl_font != NULL) { TTF_CloseFont(font->sdl_font); }
    SDL_free(font);
    *p_font = NULL;
}

struct texture* text_render(
    struct window*     window,
    const struct font* font,
    const char*        text,
    const colour_t     colour
) {
    SDL_Surface*    sdl_surface = NULL;
    SDL_Texture*    sdl_texture = NULL;
    struct texture* texture     = NULL;

    if (window == NULL) {
        log_error("No window specified to target text render to.");
        return NULL;
    }
    if (font == NULL) {
        log_error("No font specified to render text with.");
        return NULL;
    }
    if (text == NULL) {
        log_error("No text specified to render.");
        return NULL;
    }

    SDL_Color sdl_color = {
        .r = colour.r,
        .g = colour.g,
        .b = colour.b,
        .a = colour.a,
    };
    sdl_surface = TTF_RenderText_Blended(font->sdl_font, text, SDL_strlen(text), sdl_color);
    if (sdl_surface == NULL) {
        LOG_SDL_ERROR("TTF failed to render text to a surface.");
        return NULL;
    }

    sdl_texture = SDL_CreateTextureFromSurface(window_get_renderer(window), sdl_surface);
    if (sdl_texture == NULL) {
        LOG_SDL_ERROR("SDL failed to create texture from surface.");
        SDL_DestroySurface(sdl_surface);
        return NULL;
    }
    SDL_DestroySurface(sdl_surface);

    return texture_create_from_sdl_texture(sdl_texture);
}
