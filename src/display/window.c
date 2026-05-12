/**
 * @file window.c
 * @brief Implementation for Window Objects. Backend is handled via SDL3, this is just a simple
 * wrapper that simplifies the API.
 */
#include "display/window.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <log.h>
#include <stdbool.h>

#include "display/window_internal.h"
#include "events/event_internal.h"

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
    SDL_Window*  window;
    SDL_WindowID id;

    SDL_Renderer*   renderer;
    TTF_TextEngine* text_engine;

    void (*event_handler)(const struct event* evt);
};

static inline bool window_is_app_event(const SDL_Event* evt);

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

    rtn->text_engine = TTF_CreateRendererTextEngine(rtn->renderer);
    if (rtn->text_engine == NULL) {
        LOG_SDL_ERROR("Couldn't create window. SDL failed to create TextEngine.");
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

    rtn->id            = SDL_GetWindowID(rtn->window);
    rtn->event_handler = NULL;

    if (event_register_window(rtn) != 0) {
        log_error("Failed to register window events");
        goto err_cleanup;
    }

    return rtn;

err_cleanup:
    window_destroy(&rtn);
    return NULL;
}

void window_destroy(struct window** p_window) {
    if (p_window == NULL || *p_window == NULL) { return; }
    struct window* window = *p_window;

    // Stop Event Callbacks
    event_deregister_window(window);

    // Cleanup resource
    if (window->text_engine != NULL) { TTF_DestroyRendererTextEngine(window->text_engine); }
    if (window->renderer != NULL) { SDL_DestroyRenderer(window->renderer); }
    if (window->window != NULL) { SDL_DestroyWindow(window->window); }
    SDL_free(window);
    *p_window = NULL;

    // Shutdown requested SDL systems
    SDL_QuitSubSystem(WINDOW_SDL_SUBSYSTEMS);
}

// ========== Events ==========

void window_register_event_handler(struct window* window, void (*cb)(const struct event* evt)) {
    if (window->window == NULL) { return; }
    window->event_handler = cb;
}

void window_handle_event(struct window* window, const SDL_Event* sdl_evt) {
    struct event evt = {0};

    if (window == NULL || sdl_evt == NULL) { return; }

    switch (event_get_simple_sdl_event_type(sdl_evt->type)) {
    // If INPUT type event, kick up to user's window event handler
    case SDL_EVENT_SIMPLE_KEY:
    case SDL_EVENT_SIMPLE_BUTTON:
    case SDL_EVENT_SIMPLE_MOTION:
    case SDL_EVENT_SIMPLE_JOYSTICK:
    case SDL_EVENT_SIMPLE_GAMEPAD:
        if (window->event_handler == NULL) { return; }
        evt = event_convert_sdl_evt(sdl_evt);
        window->event_handler(&evt);
        return;

    // Handle WINDOW events locally
    case SDL_EVENT_SIMPLE_WINDOW:
        switch (sdl_evt->type) {
        // Drop these event types
        // Either not required or already handled else where
        case SDL_EVENT_WINDOW_SHOWN:
        case SDL_EVENT_WINDOW_HIDDEN:
        case SDL_EVENT_WINDOW_EXPOSED:
        case SDL_EVENT_WINDOW_MOVED:
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        case SDL_EVENT_WINDOW_METAL_VIEW_RESIZED:
        case SDL_EVENT_WINDOW_MINIMIZED:
        case SDL_EVENT_WINDOW_MAXIMIZED:
        case SDL_EVENT_WINDOW_RESTORED:
        case SDL_EVENT_WINDOW_MOUSE_ENTER:
        case SDL_EVENT_WINDOW_MOUSE_LEAVE:
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
        case SDL_EVENT_WINDOW_FOCUS_LOST:
        case SDL_EVENT_WINDOW_HIT_TEST:
        case SDL_EVENT_WINDOW_ICCPROF_CHANGED:
        case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
        case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
        case SDL_EVENT_WINDOW_SAFE_AREA_CHANGED:
        case SDL_EVENT_WINDOW_OCCLUDED:
        case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
        case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
        case SDL_EVENT_WINDOW_DESTROYED:
        default:
            return;

        case SDL_EVENT_WINDOW_RESIZED:
            return;

        // Bubble close requests to user space
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            if (window->event_handler == NULL) { return; }
            evt = event_convert_sdl_evt(sdl_evt);
            window->event_handler(&evt);
            return;
        }
    }
}

static inline bool window_is_app_event(const SDL_Event* evt) { return false; }

// ========== Events ==========

// ========== Rendering ==========

int window_clear(struct window* window) {
    if (window == NULL) {
        log_error("No window to clear.");
        return -1;
    }

    return window_fill(window, (colour_t){0, 0, 0, 255});
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

// ----- Primatives -----

int window_fill(struct window* window, colour_t colour) {
    if (window == NULL) {
        log_error("No window to fill");
        return -1;
    }

    if (SDL_SetRenderDrawColor(window->renderer, colour.r, colour.g, colour.b, colour.a) == false) {
        LOG_SDL_ERROR("SDL failed to set fill colour.");
        return -1;
    }
    if (SDL_RenderClear(window->renderer) == false) {
        LOG_SDL_ERROR("SDL failed to fill window to expected colour.");
        return -1;
    }

    return 0;
}

int window_draw_line(
    struct window* window,
    float          x1,
    float          y1,
    float          x2,
    float          y2,
    colour_t       colour
) {
    if (window == NULL) {
        log_error("No window to draw on.");
        return -1;
    }

    if (SDL_SetRenderDrawColor(window->renderer, colour.r, colour.g, colour.b, colour.a) == false) {
        LOG_SDL_ERROR("SDL failed to set fill colour.");
        return -1;
    }
    if (SDL_RenderLine(window->renderer, x1, y1, x2, y2) == false) {
        LOG_SDL_ERROR("SDL failed to draw line.");
        return -1;
    }

    return 0;
}

int window_draw_rect(struct window* window, struct rect rect, colour_t colour) {
    if (window == NULL) {
        log_error("No window to draw on.");
        return -1;
    }

    if (SDL_SetRenderDrawColor(window->renderer, colour.r, colour.g, colour.b, colour.a) == false) {
        LOG_SDL_ERROR("SDL failed to set fill colour.");
        return -1;
    }

    SDL_FRect sdl_rect = {
        .h = rect.h,
        .w = rect.w,
        .x = rect.x,
        .y = rect.y,
    };
    if (SDL_RenderRect(window->renderer, &sdl_rect) == false) {
        LOG_SDL_ERROR("SDL failed to draw a rectangle");
        return -1;
    }

    return 0;
}
int window_draw_rect_filled(struct window* window, struct rect rect, colour_t colour) {
    if (window == NULL) {
        log_error("No window to draw on.");
        return -1;
    }

    if (SDL_SetRenderDrawColor(window->renderer, colour.r, colour.g, colour.b, colour.a) == false) {
        LOG_SDL_ERROR("SDL failed to set fill colour.");
        return -1;
    }

    SDL_FRect sdl_rect = {
        .h = rect.h,
        .w = rect.w,
        .x = rect.x,
        .y = rect.y,
    };
    if (SDL_RenderFillRect(window->renderer, &sdl_rect) == false) {
        LOG_SDL_ERROR("SDL failed to draw a filled rectangle");
        return -1;
    }

    return 0;
}

int window_draw_point(struct window* window, float x, float y, colour_t colour) {
    if (window == NULL) {
        log_error("No window to draw on.");
        return -1;
    }

    if (SDL_SetRenderDrawColor(window->renderer, colour.r, colour.g, colour.b, colour.a) == false) {
        LOG_SDL_ERROR("SDL failed to set fill colour.");
        return -1;
    }

    if (SDL_RenderPoint(window->renderer, x, y) == false) {
        LOG_SDL_ERROR("SDL failed to draw a point");
        return -1;
    }

    return 0;
}

// ----- Primatives -----

// ========== Rendering ==========

SDL_Renderer* window_get_renderer(const struct window* window) {
    if (window == NULL) { return NULL; }
    return window->renderer;
}

TTF_TextEngine* window_get_text_engine(const struct window* window) {
    if (window == NULL) { return NULL; }
    return window->text_engine;
}

SDL_WindowID window_get_id(const struct window* window) {
    // A value of 0 is invalid as per SDL3 documentation of SDL_WindowID
    if (window == NULL) { return 0; }

    return window->id;
}
