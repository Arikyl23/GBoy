#include <log.h>
#include <stdbool.h>
#include <stdio.h>
#include <threads.h>

// #include "cpu/cpu.h"
#include "display/text.h"
#include "display/texture.h"
#include "display/window.h"
#include "events/event.h"

LOG_MODULE_SETUP_DEFAULT("main");

#define FONT_FILEPATH "/usr/local/share/fonts/NerdJetBrainsMono/JetBrainsMonoNerdFont-Regular.ttf"

static bool input_runner_active;
static bool wait_on_input;

int input_handler(void* arg) {
    while (input_runner_active) {
        if (getchar() == (int)'a') { wait_on_input = false; }
    }

    return 0;
}

int main(void) {
    struct window*  window       = window_create("Main Window", 640, 480, 1);
    struct texture* bkg_texture  = texture_create(window, 640, 480, TEXTURE_TYPE_STREAMING);
    struct texture* text_texture = NULL;
    struct font*    font         = font_create(FONT_FILEPATH, 48.0f);
    struct rect     text_box     = {0};
    pixel_t         pixel_data[640 * 480] = {0};

    const colour_t colours[] = {
        {.r = 255, .g = 0, .b = 0, .a = 255},
        {.r = 0, .g = 255, .b = 0, .a = 255},
        {.r = 0, .g = 0, .b = 255, .a = 255},
        {.r = 255, .g = 255, .b = 255, .a = 255},
        {.r = 0, .g = 0, .b = 0, .a = 255}
    };
    const char text[][16] = {"RED", "GREEN", "BLUE"};

    input_runner_active = true;
    thrd_t input_thread;
    if (thrd_create(&input_thread, input_handler, NULL) != thrd_success) {
        log_error("Failed to spool up input thread");
        return -1;
    }

    for (int colour = 0; colour < 3; colour++) {
        for (int i = 0; i < 640 * 480; i++) { pixel_data[i] = colours[colour]; }
        texture_update(bkg_texture, pixel_data, 640 * 480);
        texture_destroy(&text_texture);
        text_texture = text_render(window, font, text[colour], colours[4]);
        text_box     = (struct rect){
                .w = texture_get_width(text_texture),
                .h = texture_get_height(text_texture),
                .x = (texture_get_width(bkg_texture) - texture_get_width(text_texture)) / 2,
                .y = (texture_get_height(bkg_texture) - texture_get_height(text_texture)) / 2,
        };

        wait_on_input = true;
        while (wait_on_input) {
            // Frame Update
            window_clear(window);
            texture_draw(window, bkg_texture);
            window_draw_rect_filled(window, text_box, colours[3]);
            window_draw_rect(window, text_box, colours[4]);
            texture_draw_at(window, text_texture, &text_box);
            window_present(window);

            // Poll for events
            event_poll();
        }
    }

    input_runner_active = false;
    thrd_join(input_thread, NULL);

    texture_destroy(&bkg_texture);
    window_destroy(&window);
    return 0;
}
