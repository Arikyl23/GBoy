/**
 * @file texture.c
 * @brief Implementation for Textures.
 */
#include "display/texture.h"

#include <SDL3/SDL.h>
#include <log.h>

#include "display/texture_internal.h"
#include "display/window_internal.h"

LOG_MODULE_SETUP("Texture", CONFIG_TEXTURE_MODULE_LOG_LEVEL)

#define TEXTURE_PIXEL_FORMAT (SDL_PIXELFORMAT_RGBA8888)

#define LOG_SDL_ERROR(msg)                                                                         \
    do {                                                                                           \
        log_error(msg "\n\tReason: %s", SDL_GetError());                                           \
        SDL_ClearError();                                                                          \
    } while (0);

struct texture {
    SDL_Texture* sdl_texture;
    size_t       width;
    size_t       height;
    size_t       size;
};

struct texture* texture_create(
    struct window*                window,
    const size_t                  width,
    const size_t                  height,
    const enum texture_type       type,
    const enum texture_scale_mode scale_mode
) {
    struct texture* rtn = NULL;

    if (window == NULL) {
        log_error("Attempted to create a texture from a NULL instance of a window");
        goto err_cleanup;
    }

    rtn = SDL_calloc(sizeof(struct texture), 1);
    if (rtn == NULL) {
        LOG_SDL_ERROR("SDL failed to allocate handle for texture.");
        goto err_cleanup;
    }

    rtn->sdl_texture = SDL_CreateTexture(
        window_get_renderer(window),
        TEXTURE_PIXEL_FORMAT,
        (SDL_TextureAccess)type,
        width,
        height
    );
    if (rtn->sdl_texture == NULL) {
        LOG_SDL_ERROR("SDL failed to create the texture");
        goto err_cleanup;
    }

    if (SDL_SetTextureScaleMode(rtn->sdl_texture, (SDL_ScaleMode)scale_mode) == false) {
        LOG_SDL_ERROR("SDL failed to set texture scale mode");
        goto err_cleanup;
    }

    rtn->height = height;
    rtn->width  = width;
    rtn->size   = height * width;

    return rtn;

err_cleanup:
    texture_destroy(&rtn);
    return NULL;
}

void texture_destroy(struct texture** p_texture) {
    if (p_texture == NULL || *p_texture == NULL) { return; }

    struct texture* texture = *p_texture;
    if (texture->sdl_texture != NULL) { SDL_DestroyTexture(texture->sdl_texture); }
    SDL_free(texture);
    *p_texture = NULL;
}

int texture_get_width(const struct texture* texture) { return texture->width; }
int texture_get_height(const struct texture* texture) { return texture->height; }

int texture_update(struct texture* texture, const pixel_t* pixel_data, const size_t count) {
    if (texture == NULL) {
        log_error("Attempted to update a NULL texture.");
        return -1;
    }
    if (pixel_data == NULL || count == 0) {
        log_warn("No pixel data to update");
        return 0;
    }
    if (count != texture->size) {
        log_error("Incomplete pixel data. Expected a size of %i", texture->size);
        return -1;
    }

    void* sdl_pixel_data = NULL;
    int   pitch          = 0;
    if (SDL_LockTexture(texture->sdl_texture, NULL, &sdl_pixel_data, &pitch) == false) {
        LOG_SDL_ERROR("SDL failed to lock the texture.");
        return -1;
    }

    // Copy pixels depending on format
    const SDL_PixelFormatDetails* fmt = SDL_GetPixelFormatDetails(TEXTURE_PIXEL_FORMAT);
    if (fmt == NULL) {
        LOG_SDL_ERROR("SDL failed to get pixel format details.");
        return -1;
    }
    for (size_t row = 0; row < texture->height; row++) {
        uint8_t*       dst_row = (uint8_t*)sdl_pixel_data + row * pitch;
        const pixel_t* src_row = pixel_data + row * texture->width;

        for (size_t col = 0; col < texture->width; col++) {
            uint32_t mapped = SDL_MapRGBA(
                fmt,
                NULL,
                src_row[col].r,
                src_row[col].g,
                src_row[col].b,
                src_row[col].a
            );

            // Write only as many bytes as the format actually uses
            uint8_t* dst_pixel = dst_row + col * fmt->bytes_per_pixel;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            SDL_memcpy(
                dst_pixel,
                (uint8_t*)&mapped + (4 - fmt->bytes_per_pixel),
                fmt->bytes_per_pixel
            );
#else
            SDL_memcpy(dst_pixel, &mapped, fmt->bytes_per_pixel);
#endif
        }
    }

    SDL_UnlockTexture(texture->sdl_texture);

    return 0;
}

int texture_draw(struct window* window, const struct texture* texture) {
    return texture_draw_region_at(window, texture, NULL, NULL);
}

int texture_draw_at(
    const struct window*  window,
    const struct texture* texture,
    const struct rect*    dst
) {
    return texture_draw_region_at(window, texture, NULL, dst);
}

int texture_draw_region(
    const struct window*  window,
    const struct texture* texture,
    const struct rect*    src
) {
    return texture_draw_region_at(window, texture, src, NULL);
}

int texture_draw_region_at(
    const struct window*  window,
    const struct texture* texture,
    const struct rect*    src,
    const struct rect*    dst
) {
    SDL_FRect s_rect = {0};
    SDL_FRect d_rect = {0};

    if (window == NULL) {
        log_error("Attempted to draw to a NULL window");
        return -1;
    }
    if (texture == NULL) {
        log_error("Attempted to draw a NULL texture.");
        return -1;
    }
    if (src != NULL) {
        s_rect = (SDL_FRect){
            .h = src->h,
            .w = src->w,
            .x = src->x,
            .y = src->y,
        };
    }
    if (dst != NULL) {
        d_rect = (SDL_FRect){
            .h = dst->h,
            .w = dst->w,
            .x = dst->x,
            .y = dst->y,
        };
    }

    if (SDL_RenderTexture(
            window_get_renderer(window),
            texture->sdl_texture,
            (src == NULL) ? NULL : &s_rect,
            (dst == NULL) ? NULL : &d_rect
        ) == false) {
        LOG_SDL_ERROR("SDL failed to render texture.");
        return -1;
    }

    return 0;
}

struct texture* texture_create_from_sdl_texture(SDL_Texture* sdl_texture) {
    if (sdl_texture == NULL) {
        log_error("Failed to create texture from NULL sdl texture");
        return NULL;
    }

    struct texture* rtn = SDL_calloc(sizeof(struct texture), 1);
    if (rtn == NULL) {
        LOG_SDL_ERROR("SDL failed to allocate handle for texture.");
        return NULL;
    }

    *rtn = (struct texture){
        .sdl_texture = sdl_texture,
        .width       = sdl_texture->w,
        .height      = sdl_texture->h,
        .size        = sdl_texture->w * sdl_texture->h,
    };

    return rtn;
}
