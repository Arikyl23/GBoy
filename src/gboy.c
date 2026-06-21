/**
 * @file gboy.c
 * @brief Implementation for GBoy.
 */
#include "gboy.h"

#include <SDL3/SDL.h>
#include <log.h>

#include "DEFINES.h"
#include "assets.h"
#include "cpu/cpu.h"
#include "debugger.h"
#include "debugger_internal.h"
#include "display/renderer.h"
#include "display/texture.h"
#include "display/window.h"
#include "memory/mmu.h"
#include "ppu/ppu.h"

LOG_MODULE_SETUP("GBoy", CONFIG_GBOY_MODULE_LOG_LEVEL);

#define GBOY_WINDOW_TITLE "GBoy"
#define GBOY_WINDOW_SCALE 4

#define GBOY_LCD_WIDTH  160
#define GBOY_LCD_HEIGHT 144
#define GBOY_LCD_SIZE   (GBOY_LCD_WIDTH * GBOY_LCD_HEIGHT)

#define GBOY_EMULATION_THREAD_NAME "emulation"

static struct {
    bool init;

    struct cartridge* cart;

    // Control data
    SDL_AtomicInt running;
    SDL_AtomicU32 clock_speed;

    // Emulation Thread Data
    SDL_Thread*    emulation_thread;
    SDL_Semaphore* emulation_paused_sem;

    // LCD Data
    SDL_RWLock* lcd_rw_lock;
    pixel_t     lcd[GBOY_LCD_SIZE];

    // Window Data
    struct window*       window;
    struct texture*      pixel_texture;
    renderer_cb_handle_t renderer_handle;
} m_ctx = {
    .init = false,
};

/**
 * @brief Initialization Guard. Immediately logs an error and returns if GBoy is not initialized.
 */
#define INIT_CHECK()                                                                               \
    do {                                                                                           \
        if (m_ctx.init == false) {                                                                 \
            log_error("GBoy is not initialized yet.");                                             \
            return false;                                                                          \
        }                                                                                          \
    } while (false);

/**
 * @brief Macro for quickly logging a formatted SDL error.
 */
#define LOG_SDL_ERROR(msg)                                                                         \
    do {                                                                                           \
        log_error("SDL Error: " msg "\n\tReason: %s", SDL_GetError());                             \
        SDL_ClearError();                                                                          \
    } while (false);

static int  emulation_loop(void* arg);
static void m_cycle(void);
static void t_cycle(void);
static void window_update(void);
static void window_event_handler(const struct event* evt);
static void window_handle_input(const struct event_input* evt);

bool gboy_init(void) {
    if (m_ctx.init == true) {
        log_warn("GBoy is already initialized");
        return false;
    }

    log_info("Initializing GBoy...");

    m_ctx.cart = NULL;

    SDL_SetAtomicInt(&m_ctx.running, false);
    SDL_SetAtomicU32(&m_ctx.clock_speed, 0);

    m_ctx.emulation_thread     = NULL;
    m_ctx.emulation_paused_sem = SDL_CreateSemaphore(0);
    if (m_ctx.emulation_paused_sem == NULL) {
        LOG_SDL_ERROR("Failed to create Emulation thread paused Semaphore");
        goto cleanup;
    }

    m_ctx.lcd_rw_lock = SDL_CreateRWLock();
    if (m_ctx.lcd_rw_lock == NULL) {
        LOG_SDL_ERROR("Failed to create LCD RWLock");
        goto cleanup;
    }
    SDL_memset(m_ctx.lcd, 0, sizeof(m_ctx.lcd));

    m_ctx.window =
        window_create(GBOY_WINDOW_TITLE, GBOY_LCD_WIDTH, GBOY_LCD_HEIGHT, GBOY_WINDOW_SCALE);
    if (m_ctx.window == NULL) {
        log_error("Failed to create Window");
        goto cleanup;
    }
    m_ctx.pixel_texture = texture_create(
        m_ctx.window,
        GBOY_LCD_WIDTH,
        GBOY_LCD_HEIGHT,
        TEXTURE_TYPE_STREAMING,
        TEXTURE_SCALEMODE_PIXELART
    );
    if (m_ctx.pixel_texture == NULL) {
        log_error("Failed to create window pixel texture");
        goto cleanup;
    }
    m_ctx.renderer_handle = renderer_register_render_cb(window_update);
    if (m_ctx.renderer_handle < 0) {
        log_error("Failed to register renderer callback");
        goto cleanup;
    }
    window_register_event_handler(m_ctx.window, window_event_handler);

    m_ctx.init = true;
    log_info("GBoy Initialized");
    return true;

cleanup:
    if (m_ctx.emulation_paused_sem != NULL) {
        SDL_DestroySemaphore(m_ctx.emulation_paused_sem);
        m_ctx.emulation_paused_sem = NULL;
    }
    if (m_ctx.lcd_rw_lock != NULL) {
        SDL_DestroyRWLock(m_ctx.lcd_rw_lock);
        m_ctx.lcd_rw_lock = NULL;
    }
    renderer_deregister_render_cb(m_ctx.renderer_handle);
    texture_destroy(&m_ctx.pixel_texture);
    window_destroy(&m_ctx.window);

    return false;
}

void gboy_cleanup(void) {
    if (m_ctx.init == false) { return; }

    if (debugger_is_open() == true) { debugger_close(); }

    if (SDL_GetAtomicInt(&m_ctx.running) == true) { gboy_poweroff(); }
    SDL_DestroySemaphore(m_ctx.emulation_paused_sem);
    m_ctx.emulation_paused_sem = NULL;

    // TODO: Handle Cartridges correctly. Whether that means saving the RAM data to a file or
    // TODO: however we decide to handle it.
    gboy_eject_cart();

    // TODO: Reset CPU state? Either here or during init

    SDL_DestroyRWLock(m_ctx.lcd_rw_lock);
    m_ctx.lcd_rw_lock = NULL;

    renderer_deregister_render_cb(m_ctx.renderer_handle);
    texture_destroy(&m_ctx.pixel_texture);
    window_destroy(&m_ctx.window);

    m_ctx.init = false;

    log_debug("GBoy Cleanned Up");

    return;
}

bool gboy_poweron(const size_t clock_speed) {
    INIT_CHECK();

    if (SDL_GetAtomicInt(&m_ctx.running) == true) {
        log_warn("GBoy is already running. Call gboy_poweroff() first.");
        return false;
    }

    renderer_enable_cb(m_ctx.renderer_handle, true);
    SDL_SetAtomicU32(&m_ctx.clock_speed, clock_speed);

    // Drain Semaphore (catch for it the semaphore somehow had a value larger than zero)
    while (SDL_TryWaitSemaphore(m_ctx.emulation_paused_sem));
    SDL_SetAtomicInt(&m_ctx.running, true);
    //! This must come after running is set to true
    //! If this thread gets preempted by the newly created thread before running is set, it will
    //! prematurely exit before we get the change to set running
    m_ctx.emulation_thread = SDL_CreateThread(emulation_loop, GBOY_EMULATION_THREAD_NAME, NULL);
    if (m_ctx.emulation_thread == NULL) {
        LOG_SDL_ERROR("Could not power on GBoy. SDL failed to create emulation thread.");
        SDL_SetAtomicInt(&m_ctx.running, false);
        return false;
    }

    return true;
}

bool gboy_poweroff(void) {
    INIT_CHECK();

    if (SDL_GetAtomicInt(&m_ctx.running) == false) {
        log_warn("Gboy is already powered off.");
        return true;
    }

    renderer_enable_cb(m_ctx.renderer_handle, false);
    SDL_SetAtomicInt(&m_ctx.running, false);
    SDL_SignalSemaphore(m_ctx.emulation_paused_sem);
    SDL_WaitThread(m_ctx.emulation_thread, NULL);
    m_ctx.emulation_thread = NULL;

    return true;
}

bool gboy_load_rom(const char* path) {
    INIT_CHECK();

    if (m_ctx.cart != NULL) {
        log_warn("Cartridge is already loaded. Eject old one first.");
        return false;
    }

    if (path == NULL) {
        log_error("No path given to load ROM from.");
        return false;
    }

    m_ctx.cart = cartridge_create(path);
    if (m_ctx.cart == NULL) {
        log_error("Failed to create cartridge object");
        return false;
    }

    int rc = mmu_load_cartridge(m_ctx.cart);
    if (rc == -1) {
        log_error("Bad cartridge. Couldn't load data.");
        goto cleanup;
    } else if (rc == -2) {
        log_warn("Cartridge is already loaded. Eject old one first.");
        goto cleanup;
    }

    return true;

cleanup:
    cartridge_free(&m_ctx.cart);
    return false;
}

bool gboy_eject_cart(void) {
    INIT_CHECK();

    mmu_eject_cartridge();
    cartridge_free(&m_ctx.cart);
}

bool gboy_step(void) {
    INIT_CHECK();

    if (SDL_GetAtomicInt(&m_ctx.running) == false) {
        log_warn("GBoy is powered off. Cannot step the emulation.");
        return false;
    }

    if (SDL_GetAtomicU32(&m_ctx.clock_speed) != 0) {
        log_warn("GBoy clock speed is non-zero. Cannot step the emulation.");
        return false;
    }

    SDL_SignalSemaphore(m_ctx.emulation_paused_sem);

    return true;
}

bool gboy_set_clock_speed(const uint32_t clock_speed) {
    INIT_CHECK();

    if (SDL_GetAtomicInt(&m_ctx.running) == false) {
        log_warn("GBoy is powered off. Cannot step the emulation.");
        return false;
    }

    SDL_SetAtomicU32(&m_ctx.clock_speed, clock_speed);

    if (clock_speed == 0) {
        // User is attempting to pause the emulated, ensure pause semaphore is empty by draining it
        while (SDL_TryWaitSemaphore(m_ctx.emulation_paused_sem));
    } else if (SDL_GetSemaphoreValue(m_ctx.emulation_paused_sem) == 0) {
        // User is either unpausing or changing emulation speed. Ensure pause is exited.
        //* Since we always drain the semaphore on pause, this is safe.
        SDL_SignalSemaphore(m_ctx.emulation_paused_sem);
    }

    return true;
}

uint32_t gboy_get_clock_speed(void) {
    //* This line returns `false` which is technically #define 0. This aligns with the
    //* expected fail value of 0.
    INIT_CHECK();

    if (SDL_GetAtomicInt(&m_ctx.running) == false) {
        log_warn("GBoy is powered off. Returning clock speed of 0");
        return 0;
    }

    return SDL_GetAtomicU32(&m_ctx.clock_speed);
}

bool gboy_get_lcd(pixel_t* pixel_buffer, const size_t size) {
    INIT_CHECK();

    if (pixel_buffer == NULL) {
        log_error("No pixel buffer to work on.");
        return false;
    }
    if (size != GBOY_LCD_SIZE) {
        log_error(
            "Invalid sized pixel buffer. Buffer must be %zux%zu or %zu",
            GBOY_LCD_WIDTH,
            GBOY_LCD_HEIGHT,
            GBOY_LCD_SIZE
        );
        return false;
    }

    if (SDL_GetAtomicInt(&m_ctx.running) == false) {
        log_debug("GBoy is powered off. Returning blank (black) frame");
        for (size_t i = 0; i < GBOY_LCD_SIZE; i++) { pixel_buffer[i] = (pixel_t){0, 0, 0, 255}; }
        return true;
    } else {
        SDL_LockRWLockForReading(m_ctx.lcd_rw_lock);
        SDL_memcpy(pixel_buffer, m_ctx.lcd, sizeof(m_ctx.lcd));
        SDL_UnlockRWLock(m_ctx.lcd_rw_lock);
        return true;
    }
}

bool gboy_set_lcd_pixel(const size_t x, const size_t y, const pixel_t pixel) {
    INIT_CHECK();

    if (x >= GBOY_LCD_WIDTH || y >= GBOY_LCD_HEIGHT) {
        log_warn(
            "Attempted to set a pixel outside the physical LCD boundaries. Ignored "
            "request.\n"
            "\tx: %zu | Width: %zu\n"
            "\ty: %zu | Height: %zu",
            x,
            GBOY_LCD_WIDTH,
            y,
            GBOY_LCD_HEIGHT
        );
        return false;
    }

    if (SDL_GetAtomicInt(&m_ctx.running) == false) {
        log_error("Managed to call set lcd pixel while gameboy is powered off!?");
        return false;
    }

    size_t index = y * GBOY_LCD_WIDTH + x;
    SDL_LockRWLockForWriting(m_ctx.lcd_rw_lock);
    m_ctx.lcd[index] = pixel;
    SDL_UnlockRWLock(m_ctx.lcd_rw_lock);

    return true;
}

static int emulation_loop(void* arg) {
    Uint32 prev_clock_speed = SDL_GetAtomicU32(&m_ctx.clock_speed);
    Uint64 target           = SDL_GetPerformanceCounter();
    Uint64 now;
    Uint64 tick_increment =
        (prev_clock_speed == 0) ? 0 : SDL_GetPerformanceFrequency() / (Uint64)prev_clock_speed;
    Uint64 tick_remainder =
        (prev_clock_speed == 0) ? 0 : SDL_GetPerformanceFrequency() % (Uint64)prev_clock_speed;
    Uint64 frac_accum = 0;

    log_info("Emulation Thread Starting");

    while (true) {
        // Should we be paused?
        if (SDL_GetAtomicU32(&m_ctx.clock_speed) == 0) {
            SDL_WaitSemaphore(m_ctx.emulation_paused_sem);
            // Force target to be now instead of some point in the past
            target = SDL_GetPerformanceCounter();
        }

        // Always check this before doing anything. Allows thread to kick out independent of whether
        // the thread is currently spinning or just woke up from being paused
        if (SDL_GetAtomicInt(&m_ctx.running) == false) { break; }

        // Check if clock speed has updated since last cached value
        // Update increment if so
        if (prev_clock_speed != SDL_GetAtomicU32(&m_ctx.clock_speed)) {
            prev_clock_speed = SDL_GetAtomicU32(&m_ctx.clock_speed);
            // tick_increment is meaningless if the clock speed is 0
            if (prev_clock_speed != 0) {
                tick_increment = SDL_GetPerformanceFrequency() / (Uint64)prev_clock_speed;
                tick_remainder = SDL_GetPerformanceFrequency() % (Uint64)prev_clock_speed;
                frac_accum     = 0;
            }
        }

        // Get the current tick the step starts at
        now = SDL_GetPerformanceCounter();
        // Wait until taget is reached before continuing by reruning the above code
        if (now < target) {
            SDL_CPUPauseInstruction();
            continue;
        }

        // Execute the step (1M to 4T cycles)
        m_cycle();
        for (int i = 0; i < 4; i++) { t_cycle(); }
        debugger_update_slot_values();

        // Update target based on cached clock speed
        // This is where we also correct for drift (integer division loses accuracy)
        // Note: This can be skipped if the clock speed is currently 0
        if (prev_clock_speed != 0) {
            target     += tick_increment;
            frac_accum += tick_remainder;
            if (frac_accum >= prev_clock_speed) {
                frac_accum -= prev_clock_speed;
                target++;
            }
        }
    }

    log_info("Emulation Thread Stopping");

    return 0;
}

static void m_cycle(void) {
    if (cpu_execute() != 0) {
        log_warn("CPU reported an error. Pausing emulation");
        // if cpu reports an error, immediately lock up the emulation thread by pausing it
        gboy_set_clock_speed(0);
    }
}

static void t_cycle(void) {
    if (ppu_execute() != 0) {
        log_warn("PPU reported an error. Pausing emulation");
        // if ppu reports an error, immediately lock up the emulation thread by pausing it
        gboy_set_clock_speed(0);
    }
}

static void window_update(void) {
    static pixel_t frame_data[GBOY_LCD_SIZE];

    window_clear(m_ctx.window);

    gboy_get_lcd(frame_data, GBOY_LCD_SIZE);
    texture_update(m_ctx.pixel_texture, frame_data, GBOY_LCD_SIZE);
    texture_draw(m_ctx.window, m_ctx.pixel_texture);
    window_present(m_ctx.window);
}

static void window_event_handler(const struct event* evt) {
    switch (evt->type) {
    case EVENT_APPLICATION:
        log_error("Application events should not be thrown by a window");
        return;
    case EVENT_INPUT:
        window_handle_input(&evt->input);
        return;
    case EVENT_WINDOW:
        switch (evt->window.type) {
        case EVENT_WINDOW_CLOSE_REQUESTED:
            log_info("GBoy window requested to close");
            // Only power off and ensure an APPLICATION quit event is posted
            // Main will handle proper shutdown
            gboy_poweroff();
            SDL_Event quit_evt;
            SDL_zero(quit_evt);
            quit_evt.type = SDL_EVENT_QUIT;
            SDL_PushEvent(&quit_evt);
            return;
        case EVENT_WINDOW_DESTROYED:
            log_info("GBoy window destroyed");
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

static void window_handle_input(const struct event_input* evt) {
    switch (evt->type) {
    case EVENT_INPUT_BUTTON:
        const struct event_input_button* button_input = &evt->button;

        if (button_input->device == DEVICE_KEYBOARD) {
            if (button_input->button == KEYCODE_GRAVE) {
                // Open Debugger
                if (button_input->down == false || button_input->repeat == true) { return; }
                if (debugger_is_open() == true) { return; }
                debugger_open();
                return;
            }

            return;
        } else {
            log_debug("Unhandled Device Input");
            return;
        }
        return;
    case EVENT_INPUT_MOTION:
        return;
    default:
        log_debug("Unhandled Input Event");
        return;
    }
}
