#include <stdio.h>

// #include "cpu/cpu.h"
#include "display/texture.h"
#include "display/window.h"

int main(void) {
    struct window*  window  = window_create("Main Window", 720, 640, 1);
    struct texture* texture = texture_create(window, 640, 480, TEXTURE_TYPE_STREAMING);
    pixel_t         pixel_data[640 * 480] = {0};

    const pixel_t colours[] = {
        {.r = 255, .g = 0, .b = 0, .a = 255},
        {.r = 0, .g = 255, .b = 0, .a = 255},
        {.r = 0, .g = 0, .b = 255, .a = 255},
    };

    for (int colour = 0; colour < 3; colour++) {
        for (int i = 0; i < 640 * 480; i++) { pixel_data[i] = colours[colour]; }
        texture_update(texture, pixel_data, 640 * 480);

        window_clear(window);
        texture_draw(window, texture);
        window_present(window);
        getchar();
    }

    window_destroy(&window);
    return 0;
}
