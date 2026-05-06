/**
 * @file event.c
 * @brief Implementation for Event handling.
 */
#include "events/event.h"

#include <SDL3/SDL.h>
#include <log.h>
#include <stdbool.h>
#include <stdlib.h>

#include "display/window.h"
#include "display/window_internal.h"
#include "events/event_internal.h"
#include "events/keycodes.h"
#include "events/keycodes_internal.h"

LOG_MODULE_SETUP("Event", CONFIG_EVENT_MODULE_LOG_LEVEL)

#define EVENT_MAX_REGISTERED_WINDOWS 10

struct window_inst {
    bool           active;
    SDL_WindowID   id;
    struct window* window;
};
static struct window_inst m_registered_inst[EVENT_MAX_REGISTERED_WINDOWS] = {0};
static size_t             m_registered_windows                            = 0;
static void               (*m_application_event_handler)(const struct event* evt);

static SDL_WindowID   event_get_evt_window_id(const SDL_Event* evt);
static struct window* event_find_window_by_id(const SDL_WindowID id);

void event_register_application_event_handler(
    void (*application_event_handler)(const struct event* evt)
) {
    if (application_event_handler != NULL) { log_warn("Application event handler overriden"); }
    m_application_event_handler = application_event_handler;
}

void event_deregister_application_event_handler(void) { m_application_event_handler = NULL; }

void event_poll(void) {
    // SDL_PollEvent() cannot be called if there are no active window instances. There is no point
    // in calling it if there is no currently registered windows.
    if (m_registered_windows == 0) { return; }

    SDL_Event sdl_evt = {0};
    while (SDL_PollEvent(&sdl_evt) == true) {
        // Get window ID of event
        SDL_WindowID id = event_get_evt_window_id(&sdl_evt);

        // Kick all application events (Window ID of 0) up to application callback
        if (id == 0) {
            if (m_application_event_handler == NULL) {
                log_error("No Application event handler defined. Ensure "
                          "event_register_application_event_handler() is called.");
                if (sdl_evt.type == SDL_EVENT_QUIT) {
                    log_fatal("Failed to handle QUIT event gracefully due to no event handler "
                              "being set.");
                    SDL_Quit();
                    abort();
                    return;
                }
            }

            struct event evt = event_convert_sdl_evt(&sdl_evt);
            m_application_event_handler(&evt);
            continue;
        }

        // Anything with a window ID is a window specific event
        window_handle_event(event_find_window_by_id(id), &sdl_evt);
    }
}

int event_register_window(struct window* window) {
    if (window == NULL) {
        log_error("Attempted to register a NULL window for event handling.");
        return -1;
    }

    for (int i = 0; i < EVENT_MAX_REGISTERED_WINDOWS; i++) {
        if (m_registered_inst[i].active == false) {
            m_registered_inst[i] = (struct window_inst){
                .active = true,
                .id     = window_get_id(window),
                .window = window,
            };
            m_registered_windows++;
            return 0;
        }
    }

    log_error(
        "Could not register window for events. Already reached the maximum amount of allowed "
        "concurrent window instances (%i)",
        EVENT_MAX_REGISTERED_WINDOWS
    );
    return -1;
}

void event_deregister_window(const struct window* window) {
    if (window == NULL) { return; }

    for (int i = 0; i < EVENT_MAX_REGISTERED_WINDOWS; i++) {
        if (m_registered_inst[i].active == true && m_registered_inst[i].window == window) {
            m_registered_inst[i] = (struct window_inst){0};
            m_registered_windows--;
            return;
        }
    }
    return;
}

struct event event_convert_sdl_evt(const SDL_Event* evt) {
    switch (evt->type) {
    /* Application events */
    case SDL_EVENT_QUIT:
        return (struct event){
            .type             = EVENT_APPLICATION,
            .application.type = EVENT_APPLICATION_QUIT,
        };
    case SDL_EVENT_TERMINATING:
        return (struct event){
            .type             = EVENT_APPLICATION,
            .application.type = EVENT_APPLICATION_TERMINATING,
        };
    case SDL_EVENT_LOW_MEMORY:
        return (struct event){
            .type             = EVENT_APPLICATION,
            .application.type = EVENT_APPLICATION_LOW_MEMORY,
        };
    case SDL_EVENT_WILL_ENTER_BACKGROUND:
    case SDL_EVENT_DID_ENTER_BACKGROUND:
    case SDL_EVENT_WILL_ENTER_FOREGROUND:
    case SDL_EVENT_DID_ENTER_FOREGROUND:
    case SDL_EVENT_LOCALE_CHANGED:
    case SDL_EVENT_SYSTEM_THEME_CHANGED:
        return (struct event){
            .type             = EVENT_APPLICATION,
            .application.type = EVENT_APPLICATION_UNKNOWN,
        };

    /* Display events */
    case SDL_EVENT_DISPLAY_ORIENTATION:
    case SDL_EVENT_DISPLAY_ADDED:
    case SDL_EVENT_DISPLAY_REMOVED:
    case SDL_EVENT_DISPLAY_MOVED:
    case SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED:
    case SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED:
    case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED:
    case SDL_EVENT_DISPLAY_USABLE_BOUNDS_CHANGED:
        return (struct event){
            .type = EVENT_UNKNOWN,
        };

    /* Window events */
    case SDL_EVENT_WINDOW_SHOWN:
        return (struct event){
            .type        = EVENT_WINDOW,
            .window.type = EVENT_WINDOW_SHOWN,
            .window.x    = evt->window.data1,
            .window.y    = evt->window.data2,
        };
    case SDL_EVENT_WINDOW_HIDDEN:
        return (struct event){
            .type        = EVENT_WINDOW,
            .window.type = EVENT_WINDOW_HIDDEN,
            .window.x    = evt->window.data1,
            .window.y    = evt->window.data2,
        };
    case SDL_EVENT_WINDOW_EXPOSED:
        return (struct event){
            .type        = EVENT_WINDOW,
            .window.type = EVENT_WINDOW_EXPOSED,
            .window.x    = evt->window.data1,
            .window.y    = evt->window.data2,
        };
    case SDL_EVENT_WINDOW_MOVED:
        return (struct event){
            .type        = EVENT_WINDOW,
            .window.type = EVENT_WINDOW_MOVED,
            .window.x    = evt->window.data1,
            .window.y    = evt->window.data2,
        };
    case SDL_EVENT_WINDOW_RESIZED:
        return (struct event){
            .type        = EVENT_WINDOW,
            .window.type = EVENT_WINDOW_RESIZED,
            .window.x    = evt->window.data1,
            .window.y    = evt->window.data2,
        };
    case SDL_EVENT_WINDOW_MINIMIZED:
        return (struct event){
            .type        = EVENT_WINDOW,
            .window.type = EVENT_WINDOW_MINIMIZED,
            .window.x    = evt->window.data1,
            .window.y    = evt->window.data2,
        };
    case SDL_EVENT_WINDOW_MAXIMIZED:
        return (struct event){
            .type        = EVENT_WINDOW,
            .window.type = EVENT_WINDOW_MAXIMIZED,
            .window.x    = evt->window.data1,
            .window.y    = evt->window.data2,
        };
    case SDL_EVENT_WINDOW_MOUSE_ENTER:
        return (struct event){
            .type        = EVENT_WINDOW,
            .window.type = EVENT_WINDOW_MOUSE_ENTER,
            .window.x    = evt->window.data1,
            .window.y    = evt->window.data2,
        };
    case SDL_EVENT_WINDOW_MOUSE_LEAVE:
        return (struct event){
            .type        = EVENT_WINDOW,
            .window.type = EVENT_WINDOW_MOUSE_LEAVE,
            .window.x    = evt->window.data1,
            .window.y    = evt->window.data2,
        };
    case SDL_EVENT_WINDOW_FOCUS_GAINED:
        return (struct event){
            .type        = EVENT_WINDOW,
            .window.type = EVENT_WINDOW_FOCUS_GAINED,
            .window.x    = evt->window.data1,
            .window.y    = evt->window.data2,
        };
    case SDL_EVENT_WINDOW_FOCUS_LOST:
        return (struct event){
            .type        = EVENT_WINDOW,
            .window.type = EVENT_WINDOW_FOCUS_LOST,
            .window.x    = evt->window.data1,
            .window.y    = evt->window.data2,
        };
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        return (struct event){
            .type        = EVENT_WINDOW,
            .window.type = EVENT_WINDOW_CLOSE_REQUESTED,
            .window.x    = evt->window.data1,
            .window.y    = evt->window.data2,
        };
    case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
        return (struct event){
            .type        = EVENT_WINDOW,
            .window.type = EVENT_WINDOW_ENTER_FULLSCREEN,
            .window.x    = evt->window.data1,
            .window.y    = evt->window.data2,
        };
    case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
        return (struct event){
            .type        = EVENT_WINDOW,
            .window.type = EVENT_WINDOW_LEAVE_FULLSCREEN,
            .window.x    = evt->window.data1,
            .window.y    = evt->window.data2,
        };
    case SDL_EVENT_WINDOW_DESTROYED:
        return (struct event){
            .type        = EVENT_WINDOW,
            .window.type = EVENT_WINDOW_DESTROYED,
            .window.x    = evt->window.data1,
            .window.y    = evt->window.data2,
        };
    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
    case SDL_EVENT_WINDOW_METAL_VIEW_RESIZED:
    case SDL_EVENT_WINDOW_RESTORED:
    case SDL_EVENT_WINDOW_HIT_TEST:
    case SDL_EVENT_WINDOW_ICCPROF_CHANGED:
    case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
    case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
    case SDL_EVENT_WINDOW_SAFE_AREA_CHANGED:
    case SDL_EVENT_WINDOW_OCCLUDED:
    case SDL_EVENT_WINDOW_HDR_STATE_CHANGED:
        return (struct event){
            .type        = EVENT_WINDOW,
            .window.type = EVENT_WINDOW_UNKNOWN,
            .window.x    = evt->window.data1,
            .window.y    = evt->window.data2,
        };

    /* Keyboard events */
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
        return (struct event){
            .type                   = EVENT_INPUT,
            .input.type             = EVENT_INPUT_BUTTON,
            .input.button.device    = DEVICE_KEYBOARD,
            .input.button.device_id = evt->key.which,
            .input.button.button    = keycode_from_sdl_scancode(evt->key.scancode),
            .input.button.modifier  = keycode_modifier_from_sdl(evt->key.mod),
            .input.button.down      = evt->key.down,
            .input.button.repeat    = evt->key.repeat,
        };
    case SDL_EVENT_TEXT_EDITING:
    case SDL_EVENT_TEXT_INPUT:
    case SDL_EVENT_KEYMAP_CHANGED:
    case SDL_EVENT_KEYBOARD_ADDED:
    case SDL_EVENT_KEYBOARD_REMOVED:
    case SDL_EVENT_TEXT_EDITING_CANDIDATES:
    case SDL_EVENT_SCREEN_KEYBOARD_SHOWN:
    case SDL_EVENT_SCREEN_KEYBOARD_HIDDEN:
        return (struct event){
            .type       = EVENT_INPUT,
            .input.type = EVENT_INPUT_UNKNOWN,
        };

    /* Mouse events */
    case SDL_EVENT_MOUSE_MOTION:
        return (struct event){
            .type                    = EVENT_INPUT,
            .input.type              = EVENT_INPUT_MOTION,
            .input.motion.device     = DEVICE_MOUSE,
            .input.motion.device_id  = evt->motion.which,
            .input.motion.x          = evt->motion.x,
            .input.motion.y          = evt->motion.y,
            .input.motion.x_relative = evt->motion.xrel,
            .input.motion.y_relative = evt->motion.yrel,
        };
    case SDL_EVENT_MOUSE_BUTTON_UP:
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        return (struct event){
            .type                   = EVENT_INPUT,
            .input.type             = EVENT_INPUT_BUTTON,
            .input.button.device    = DEVICE_MOUSE,
            .input.button.device_id = evt->button.which,
            .input.button.button    = keycode_from_sdl_mouse_button(evt->button.button),
            .input.button.modifier  = KEYCODE_MODIFER_NONE,
            .input.button.down      = evt->button.down,
            .input.button.repeat    = (evt->button.clicks == 2),
        };
    case SDL_EVENT_MOUSE_WHEEL:
        return (struct event){
            .type                 = EVENT_INPUT,
            .input.type           = EVENT_INPUT_AXIS,
            .input.axis.device    = DEVICE_MOUSE,
            .input.axis.device_id = evt->wheel.which,
            .input.axis.x         = evt->wheel.x,
            .input.axis.y         = evt->wheel.y,
        };
    case SDL_EVENT_MOUSE_ADDED:
    case SDL_EVENT_MOUSE_REMOVED:
        return (struct event){
            .type       = EVENT_INPUT,
            .input.type = EVENT_INPUT_UNKNOWN,
        };

    case SDL_EVENT_JOYSTICK_AXIS_MOTION:
        return (struct event){
            .type                 = EVENT_INPUT,
            .input.type           = EVENT_INPUT_AXIS,
            .input.axis.device    = DEVICE_GAMEPAD,
            .input.axis.device_id = evt->jaxis.which,
            .input.axis.axis      = evt->jaxis.axis,
            .input.axis.x         = evt->jaxis.value,
            .input.axis.y         = 0,
        };
    case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
    case SDL_EVENT_JOYSTICK_BUTTON_UP:
        return (struct event){
            .type                   = EVENT_INPUT,
            .input.type             = EVENT_INPUT_BUTTON,
            .input.button.device    = DEVICE_GAMEPAD,
            .input.button.device_id = evt->jbutton.which,
            .input.button.button    = keycode_from_sdl_gamepad(evt->jbutton.button),
            .input.button.modifier  = KEYCODE_MODIFER_NONE,
            .input.button.down      = evt->jbutton.down,
            .input.button.repeat    = false,
        };
    case SDL_EVENT_JOYSTICK_BALL_MOTION:
    case SDL_EVENT_JOYSTICK_HAT_MOTION:
    case SDL_EVENT_JOYSTICK_ADDED:
    case SDL_EVENT_JOYSTICK_REMOVED:
    case SDL_EVENT_JOYSTICK_BATTERY_UPDATED:
    case SDL_EVENT_JOYSTICK_UPDATE_COMPLETE:
        return (struct event){
            .type       = EVENT_INPUT,
            .input.type = EVENT_INPUT_UNKNOWN,
        };

    case SDL_EVENT_GAMEPAD_AXIS_MOTION:
        return (struct event){
            .type                 = EVENT_INPUT,
            .input.type           = EVENT_INPUT_AXIS,
            .input.axis.device    = DEVICE_GAMEPAD,
            .input.axis.device_id = evt->gaxis.which,
            .input.axis.axis      = evt->gaxis.axis,
            .input.axis.x         = evt->gaxis.value,
            .input.axis.y         = 0,
        };
    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
    case SDL_EVENT_GAMEPAD_BUTTON_UP:
        return (struct event){
            .type                   = EVENT_INPUT,
            .input.type             = EVENT_INPUT_BUTTON,
            .input.button.device    = DEVICE_GAMEPAD,
            .input.button.device_id = evt->gbutton.which,
            .input.button.button    = keycode_from_sdl_gamepad(evt->gbutton.button),
            .input.button.modifier  = KEYCODE_MODIFER_NONE,
            .input.button.down      = evt->gbutton.down,
            .input.button.repeat    = false,
        };
    case SDL_EVENT_GAMEPAD_ADDED:
    case SDL_EVENT_GAMEPAD_REMOVED:
    case SDL_EVENT_GAMEPAD_REMAPPED:
    case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
    case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
    case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
    case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:
    case SDL_EVENT_GAMEPAD_UPDATE_COMPLETE:
    case SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED:
        return (struct event){
            .type       = EVENT_INPUT,
            .input.type = EVENT_INPUT_UNKNOWN,
        };

    case SDL_EVENT_FINGER_DOWN:
    case SDL_EVENT_FINGER_UP:
    case SDL_EVENT_FINGER_MOTION:
    case SDL_EVENT_FINGER_CANCELED:
        return (struct event){
            .type       = EVENT_INPUT,
            .input.type = EVENT_INPUT_UNKNOWN,
        };

    case SDL_EVENT_PINCH_BEGIN:
    case SDL_EVENT_PINCH_UPDATE:
    case SDL_EVENT_PINCH_END:
        return (struct event){
            .type       = EVENT_INPUT,
            .input.type = EVENT_INPUT_UNKNOWN,
        };

    case SDL_EVENT_CLIPBOARD_UPDATE:
        return (struct event){
            .type = EVENT_UNKNOWN,
        };

    case SDL_EVENT_DROP_FILE:
    case SDL_EVENT_DROP_TEXT:
    case SDL_EVENT_DROP_BEGIN:
    case SDL_EVENT_DROP_COMPLETE:
    case SDL_EVENT_DROP_POSITION:
        return (struct event){
            .type = EVENT_UNKNOWN,
        };

    case SDL_EVENT_AUDIO_DEVICE_ADDED:
    case SDL_EVENT_AUDIO_DEVICE_REMOVED:
    case SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED:
        return (struct event){
            .type = EVENT_UNKNOWN,
        };

    case SDL_EVENT_SENSOR_UPDATE:
        return (struct event){
            .type = EVENT_UNKNOWN,
        };

    case SDL_EVENT_PEN_PROXIMITY_IN:
    case SDL_EVENT_PEN_PROXIMITY_OUT:
    case SDL_EVENT_PEN_DOWN:
    case SDL_EVENT_PEN_UP:
    case SDL_EVENT_PEN_BUTTON_DOWN:
    case SDL_EVENT_PEN_BUTTON_UP:
    case SDL_EVENT_PEN_MOTION:
    case SDL_EVENT_PEN_AXIS:
        return (struct event){
            .type = EVENT_UNKNOWN,
        };

    case SDL_EVENT_CAMERA_DEVICE_ADDED:
    case SDL_EVENT_CAMERA_DEVICE_REMOVED:
    case SDL_EVENT_CAMERA_DEVICE_APPROVED:
    case SDL_EVENT_CAMERA_DEVICE_DENIED:
        return (struct event){
            .type = EVENT_UNKNOWN,
        };

    case SDL_EVENT_RENDER_TARGETS_RESET:
    case SDL_EVENT_RENDER_DEVICE_RESET:
    case SDL_EVENT_RENDER_DEVICE_LOST:
        return (struct event){
            .type = EVENT_UNKNOWN,
        };

    default:
        return (struct event){
            .type = EVENT_UNKNOWN,
        };
    }
}

enum sdl_event_simple event_get_simple_sdl_event_type(const SDL_EventType type) {
    switch (type) {
    /* Application events */
    case SDL_EVENT_QUIT:
    case SDL_EVENT_TERMINATING:
    case SDL_EVENT_LOW_MEMORY:
    case SDL_EVENT_WILL_ENTER_BACKGROUND:
    case SDL_EVENT_DID_ENTER_BACKGROUND:
    case SDL_EVENT_WILL_ENTER_FOREGROUND:
    case SDL_EVENT_DID_ENTER_FOREGROUND:
    case SDL_EVENT_LOCALE_CHANGED:
    case SDL_EVENT_SYSTEM_THEME_CHANGED:
        return SDL_EVENT_SIMPLE_APPLICATION;

    /* Display events */
    case SDL_EVENT_DISPLAY_ORIENTATION:
    case SDL_EVENT_DISPLAY_ADDED:
    case SDL_EVENT_DISPLAY_REMOVED:
    case SDL_EVENT_DISPLAY_MOVED:
    case SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED:
    case SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED:
    case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED:
    case SDL_EVENT_DISPLAY_USABLE_BOUNDS_CHANGED:
        return SDL_EVENT_SIMPLE_DISPLAY;

    /* Window events */
    case SDL_EVENT_WINDOW_SHOWN:
    case SDL_EVENT_WINDOW_HIDDEN:
    case SDL_EVENT_WINDOW_EXPOSED:
    case SDL_EVENT_WINDOW_MOVED:
    case SDL_EVENT_WINDOW_RESIZED:
    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
    case SDL_EVENT_WINDOW_METAL_VIEW_RESIZED:
    case SDL_EVENT_WINDOW_MINIMIZED:
    case SDL_EVENT_WINDOW_MAXIMIZED:
    case SDL_EVENT_WINDOW_RESTORED:
    case SDL_EVENT_WINDOW_MOUSE_ENTER:
    case SDL_EVENT_WINDOW_MOUSE_LEAVE:
    case SDL_EVENT_WINDOW_FOCUS_GAINED:
    case SDL_EVENT_WINDOW_FOCUS_LOST:
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
    case SDL_EVENT_WINDOW_HIT_TEST:
    case SDL_EVENT_WINDOW_ICCPROF_CHANGED:
    case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
    case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
    case SDL_EVENT_WINDOW_SAFE_AREA_CHANGED:
    case SDL_EVENT_WINDOW_OCCLUDED:
    case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
    case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
    case SDL_EVENT_WINDOW_DESTROYED:
    case SDL_EVENT_WINDOW_HDR_STATE_CHANGED:
        return SDL_EVENT_SIMPLE_WINDOW;

    /* Keyboard events */
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
    case SDL_EVENT_TEXT_EDITING:
    case SDL_EVENT_TEXT_INPUT:
    case SDL_EVENT_KEYMAP_CHANGED:
    case SDL_EVENT_KEYBOARD_ADDED:
    case SDL_EVENT_KEYBOARD_REMOVED:
    case SDL_EVENT_TEXT_EDITING_CANDIDATES:
    case SDL_EVENT_SCREEN_KEYBOARD_SHOWN:
    case SDL_EVENT_SCREEN_KEYBOARD_HIDDEN:
        return SDL_EVENT_SIMPLE_KEY;

    /* Mouse events */
    case SDL_EVENT_MOUSE_MOTION:
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
    case SDL_EVENT_MOUSE_WHEEL:
    case SDL_EVENT_MOUSE_ADDED:
    case SDL_EVENT_MOUSE_REMOVED:
        return SDL_EVENT_SIMPLE_BUTTON;

    /* Joystick events */
    case SDL_EVENT_JOYSTICK_AXIS_MOTION:
    case SDL_EVENT_JOYSTICK_BALL_MOTION:
    case SDL_EVENT_JOYSTICK_HAT_MOTION:
    case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
    case SDL_EVENT_JOYSTICK_BUTTON_UP:
    case SDL_EVENT_JOYSTICK_ADDED:
    case SDL_EVENT_JOYSTICK_REMOVED:
    case SDL_EVENT_JOYSTICK_BATTERY_UPDATED:
    case SDL_EVENT_JOYSTICK_UPDATE_COMPLETE:
        return SDL_EVENT_SIMPLE_JOYSTICK;

    /* Gamepad events */
    case SDL_EVENT_GAMEPAD_AXIS_MOTION:
    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
    case SDL_EVENT_GAMEPAD_BUTTON_UP:
    case SDL_EVENT_GAMEPAD_ADDED:
    case SDL_EVENT_GAMEPAD_REMOVED:
    case SDL_EVENT_GAMEPAD_REMAPPED:
    case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
    case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
    case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
    case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:
    case SDL_EVENT_GAMEPAD_UPDATE_COMPLETE:
    case SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED:
        return SDL_EVENT_SIMPLE_GAMEPAD;

    /* Render events */
    case SDL_EVENT_RENDER_TARGETS_RESET:
    case SDL_EVENT_RENDER_DEVICE_RESET:
    case SDL_EVENT_RENDER_DEVICE_LOST:
        return SDL_EVENT_SIMPLE_RENDER;

    default:
        return SDL_EVENT_SIMPLE_UNKNOWN;
    }
}

static SDL_WindowID event_get_evt_window_id(const SDL_Event* evt) {
    // A value of 0 is an invalid id in SDL. We can use this to
    // indicate events that dont belong to any one window

    if (evt == NULL) { return 0; }

    switch (event_get_simple_sdl_event_type(evt->type)) {
    case SDL_EVENT_SIMPLE_APPLICATION:
        return 0;
    case SDL_EVENT_SIMPLE_DISPLAY:
        // TODO: Verify this is correct
        return 0;
    case SDL_EVENT_SIMPLE_WINDOW:
        return evt->window.windowID;
    case SDL_EVENT_SIMPLE_KEY:
        return evt->key.windowID;
    case SDL_EVENT_SIMPLE_BUTTON:
        return evt->button.windowID;
    case SDL_EVENT_SIMPLE_MOTION:
        return evt->motion.windowID;
    case SDL_EVENT_SIMPLE_JOYSTICK:
        // TODO: Handle JOYSTICKS by associating them to a window. Could potentially throw to all
        // TODO: windows and let them figure it out.
        return 0;
    case SDL_EVENT_SIMPLE_GAMEPAD:
        // TODO: Handle GAMEPADs correctly by associating them to a window. Could potentially throw
        // TODO: to all windows and let them figure it out.
        return 0;
    case SDL_EVENT_SIMPLE_RENDER:
        return evt->render.windowID;
    }
}

static struct window* event_find_window_by_id(const SDL_WindowID id) {
    if (m_registered_windows == 0) { return NULL; }

    for (int i = 0; i < EVENT_MAX_REGISTERED_WINDOWS; i++) {
        if (m_registered_inst[i].active == true && m_registered_inst[i].id == id) {
            return m_registered_inst[i].window;
        }
    }

    return NULL;
}
