#include <log.h>
#include <stdbool.h>
#include <stdio.h>
#include <threads.h>

// #include "cpu/cpu.h"
#include "assets.h"
#include "display/texture.h"
#include "display/window.h"
#include "events/event.h"
#include "gboy.h"

LOG_MODULE_SETUP("main", LOG_DEBUG);

#define FONT_FILEPATH "assets/fonts/JetBrainsMono-Regular.ttf"

static bool            m_active                      = true;
static struct window*  m_gboy_window                 = NULL;
static struct texture* m_gboy_pixel_texture          = NULL;
static pixel_t         m_pixel_buffer[GBOY_LCD_SIZE] = {0};

static void application_event_handler(const struct event* evt) {
    switch (evt->type) {
    case EVENT_APPLICATION:
        switch (evt->application.type) {
        case EVENT_APPLICATION_QUIT:
            log_debug("Application Quit Reqested");
            m_active = false;
            return;
        case EVENT_APPLICATION_TERMINATING:
            log_debug("Application Terminating");
            m_active = false;
            return;
        case EVENT_APPLICATION_LOW_MEMORY:
            log_warn("Application has LOW MEMORY");
            return;
        default:
            log_error("Unhandled Application Event");
            return;
        };
    default:
        log_error("Only application events should be thrown here");
        return;
    };
}

static void gboy_window_handle_button_input(const struct event_input_button* evt) {
    static size_t cached_clock_speed = 0;

    if (evt->device == DEVICE_KEYBOARD) {
        if (evt->button == KEYCODE_GRAVE) {
            // Open Debugger
            if (evt->down == false || evt->repeat == true) { return; }
            if (gboy_debugger_is_open() == true) { return; }
            gboy_debugger_open();
            return;
        }

        if (evt->button == KEYCODE_SPACE) {
            // Play/Pause Emulation
            if (evt->down == false || evt->repeat == true) { return; }
            size_t current_clock_speed = gboy_get_clock_speed();
            if (current_clock_speed == 0) {
                gboy_set_clock_speed(cached_clock_speed);
            } else {
                gboy_set_clock_speed(0);
                cached_clock_speed = current_clock_speed;
            }
            return;
        }

        // Step / Speed up emulation
        if (evt->button == KEYCODE_RIGHT) {
            // Only down events
            if (evt->down == false) { return; }
            size_t clock_speed = gboy_get_clock_speed();
            // Is gboy paused?
            if (clock_speed == 0) {
                // Only step the first press
                if (evt->repeat == true) { return; }
                gboy_step();
            } else {
                gboy_set_clock_speed(clock_speed + 1);
            }
            return;
        }

        if (evt->button == KEYCODE_NUMPAD_0) {
            // Run at 1 step/sec
            if (evt->down == true && evt->repeat == false) { gboy_set_clock_speed(1); }
            return;
        }

        if (evt->button == KEYCODE_NUMPAD_1) {
            // Run at standard speed
            if (evt->down == true && evt->repeat == false) {
                gboy_set_clock_speed(GBOY_DEFAULT_CLOCK_SPEED);
            }
            return;
        }

        if (evt->button == KEYCODE_NUMPAD_2) {
            // Run at half speed
            if (evt->down == true && evt->repeat == false) {
                gboy_set_clock_speed(GBOY_DEFAULT_CLOCK_SPEED / 2);
            }
            return;
        }

        if (evt->button == KEYCODE_NUMPAD_3) {
            // Run at 1/3 speed
            if (evt->down == true && evt->repeat == false) {
                gboy_set_clock_speed(GBOY_DEFAULT_CLOCK_SPEED / 3);
            }
            return;
        }

        if (evt->button == KEYCODE_NUMPAD_4) {
            // Run at quater speed
            if (evt->down == true && evt->repeat == false) {
                gboy_set_clock_speed(GBOY_DEFAULT_CLOCK_SPEED / 4);
            }
            return;
        }

        log_debug("Unhandled Keyboard Input Event");
        return;
    } else {
        log_debug("Unhandled Device Input");
        return;
    }
}

static void gboy_window_handle_input(const struct event_input* evt) {
    switch (evt->type) {
    case EVENT_INPUT_BUTTON:
        gboy_window_handle_button_input(&evt->button);
        return;
    case EVENT_INPUT_MOTION:
        return;
    default:
        log_debug("Unhandled Input Event");
        return;
    }
}

static void gboy_window_event_handler(const struct event* evt) {
    switch (evt->type) {
    case EVENT_APPLICATION:
        log_error("Application events should not be thrown by a window");
        return;
    case EVENT_INPUT:
        gboy_window_handle_input(&evt->input);
        return;
    case EVENT_WINDOW:
        switch (evt->window.type) {
        case EVENT_WINDOW_CLOSE_REQUESTED:
            log_debug("GBoy window requested to close");
            m_active = false;
            return;
        case EVENT_WINDOW_DESTROYED:
            log_debug("GBoy window destroyed");
            return;
        default:
            log_debug("Unhandled Window Event");
            return;
        }
    default:
        log_debug("Unhandled Event");
        return;
    }

    return;
}

static void update_frame(void) {
    window_clear(m_gboy_window);

    gboy_get_lcd(m_pixel_buffer, GBOY_LCD_SIZE);
    texture_update(m_gboy_pixel_texture, m_pixel_buffer, GBOY_LCD_SIZE);
    texture_draw(m_gboy_window, m_gboy_pixel_texture);
    window_present(m_gboy_window);
}

int main(void) {
    m_gboy_window = window_create("GBoy", GBOY_LCD_WIDTH, GBOY_LCD_HEIGHT, 4);
    m_gboy_pixel_texture =
        texture_create(m_gboy_window, GBOY_LCD_WIDTH, GBOY_LCD_HEIGHT, TEXTURE_TYPE_STREAMING);

    // Setup event handlers
    event_register_application_event_handler(application_event_handler);
    window_register_event_handler(m_gboy_window, gboy_window_event_handler);

    // Enabled GBoy
    gboy_poweron(0);
    gboy_get_lcd(m_pixel_buffer, GBOY_LCD_SIZE);
    update_frame();
    gboy_debugger_open();

    // Just loop for now
    while (m_active == true && m_gboy_window != NULL) {
        event_poll();
        update_frame();
        if (gboy_debugger_is_open() == true) { gboy_debugger_update(); }
    }

    // Cleanup
    if (gboy_debugger_is_open() == true) { gboy_debugger_close(); };
    gboy_poweroff();
    texture_destroy(&m_gboy_pixel_texture);
    window_destroy(&m_gboy_window);

    return 0;
}
