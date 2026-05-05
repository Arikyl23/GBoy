/**
 * @file window.c
 * @brief Implementation for Window Objects. Backend is handled via SDL3, this is just a simple
 * wrapper that simplifies the API.
 */
#include "display/window.h"

#include <SDL3/SDL.h>
#include <log.h>

#include "display/window_internal.h"

LOG_MODULE_SETUP("WINDOW", CONFIG_WINDOW_MODULE_LOG_LEVEL);

// Sub-systems to init for each window.
// Note: INPUT sub-system is always initialized
#define WINDOW_SDL_SUBSYSTEMS (SDL_INIT_VIDEO)
#define WINDOW_DEFAULT_FLAGS  (SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_RESIZABLE)
#define WINDOW_SCALE_MODE     (SDL_LOGICAL_PRESENTATION_LETTERBOX)

#define LOG_SDL_ERROR(msg)                                                                         \
    do {                                                                                           \
        log_error(msg "\n\tReason: %s", SDL_GetError());                                           \
        SDL_ClearError();                                                                          \
    } while (0);

struct window {
    SDL_Window*   window;
    SDL_Renderer* renderer;
    SDL_WindowID  id;
};

struct window* window_create(
    const char*  title,
    const size_t width,
    const size_t height,
    const size_t scale
) {
    struct window* rtn = NULL;

    if (SDL_InitSubSystem(WINDOW_SDL_SUBSYSTEMS) == false) {
        LOG_SDL_ERROR("SDL failed to initialize required sub-systems for a window instance.");
        goto err_cleanup;
    }

    rtn = SDL_calloc(sizeof(struct window), 1);
    if (rtn == NULL) {
        LOG_SDL_ERROR("SDL failed to allocate handle for window.");
        goto err_cleanup;
    }

    // Create window with input already focused on it
    if (SDL_CreateWindowAndRenderer(
            title,
            width * scale,
            height * scale,
            WINDOW_DEFAULT_FLAGS,
            &rtn->window,
            &rtn->renderer
        ) == false) {
        LOG_SDL_ERROR("SDL failed to create window and/or it's renderer.");
        goto err_cleanup;
    }

    if (SDL_SetRenderLogicalPresentation(rtn->renderer, width, height, WINDOW_SCALE_MODE) ==
        false) {
        LOG_SDL_ERROR("SDL failed to decouple renderer size from window size.");
        goto err_cleanup;
    }

    if (SDL_SetRenderDrawColor(rtn->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE) == false) {
        LOG_SDL_ERROR("SDL failed to set render draw colour.");
        goto err_cleanup;
    }

    rtn->id = SDL_GetWindowID(rtn->window);

    return rtn;

err_cleanup:
    window_destroy(&rtn);
    return NULL;
}

void window_destroy(struct window** p_window) {
    if (p_window == NULL || *p_window == NULL) { return; }

    // Cleanup resource
    struct window* window = *p_window;
    if (window->renderer != NULL) { SDL_DestroyRenderer(window->renderer); }
    if (window->window != NULL) { SDL_DestroyWindow(window->window); }
    SDL_free(window);
    *p_window = NULL;

    // Shutdown requested SDL systems
    SDL_QuitSubSystem(WINDOW_SDL_SUBSYSTEMS);
}

int window_clear(struct window* window) {
    if (window == NULL) {
        log_error("No window to clear.");
        return -1;
    }

    if (SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE) == false) {
        LOG_SDL_ERROR("SDL failed to set render colour.");
        return -1;
    }

    if (SDL_RenderClear(window->renderer) == false) {
        LOG_SDL_ERROR("SDL failed to clear window renderer.");
        return -1;
    }

    return 0;
}

int window_present(struct window* window) {
    if (window == NULL) {
        log_error("No window to clear.");
        return -1;
    }

    if (SDL_RenderPresent(window->renderer) == false) {
        LOG_SDL_ERROR("SDL failed to present next renderer frame");
        return -1;
    }

    return 0;
}

SDL_Renderer* window_get_renderer(const struct window* window) { return window->renderer; }
