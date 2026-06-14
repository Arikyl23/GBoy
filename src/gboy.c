/**
 * @file gboy.c
 * @brief Implementation for GBoy.
 */
#include "gboy.h"

#include <SDL3/SDL.h>
#include <assert.h>
#include <log.h>

#include "DEFINES.h"
#include "assets.h"
#include "cpu/cpu.h"
#include "display/textbox.h"
#include "display/window.h"
#include "gboy_internal.h"
#include "memory/hw_registers.h"
#include "memory/mmu.h"
#include "ppu/ppu.h"
#include "ppu/ppu_mode.h"

LOG_MODULE_SETUP("GBoy", CONFIG_GBOY_MODULE_LOG_LEVEL);

#define FONT_FILEPATH "assets/fonts/JetBrainsMono-Regular.ttf"

#define GBOY_IDLE_SLEEP_MS 500

#define DEBUGGER_WIDTH             620
#define DEBUGGER_HEIGHT            480
#define DEBUGGER_MODE_COUNT        2
#define DEBUGGER_MODE_FONT_SIZE    36
#define DEBUGGER_TEXT_FONT_SIZE    24
#define DEBUGGER_TEXT_COLOUR       ((colour_t){255, 255, 255, 255})
#define DEBUGGER_BKG_COLOUR        ((colour_t){35, 35, 40, 255})
#define DEBUGGER_UNSELECTED_COLOUR ((colour_t){130, 130, 130, 255})
#define DEBUGGER_SELECTED_COLOUR   ((colour_t){180, 180, 180, 255})
#define DEBUGGER_OUTLINE_COLOUR    ((colour_t){100, 100, 100, 255})
#define DEBUGGER_TEXTBOX_COUNT     16
#define DEBUGGER_BAR_HEIGHT        50
#define DEBUGGER_BAR_BOX_WIDTH     (DEBUGGER_WIDTH / DEBUGGER_MODE_COUNT)
#define DEBUGGER_SLOT_COUNT        25
#define DEBUGGER_SLOT_STRING_LEN   16

#define LOG_SDL_ERROR(msg)                                                                         \
    do {                                                                                           \
        log_error(msg "\n\tReason: %s", SDL_GetError());                                           \
        SDL_ClearError();                                                                          \
    } while (false);

static struct {
    struct cartridge* cart;

    // Control data
    SDL_RWLock* control_rw_lock;
    bool        running;
    size_t      clock_speed;

    // Emulation Thread Data
    SDL_Thread*    emulation_thread;
    SDL_Semaphore* emulation_paused_sem;

    // LCD Data
    SDL_RWLock* lcd_rw_lock;
    pixel_t     lcd[GBOY_LCD_SIZE];
} m_gboy = {0};

enum debug_slot_type {
    SLOT_BYTE,
    SLOT_WORD,
    SLOT_BOOL,
    SLOT_INT,
    SLOT_STRING
};

struct debug_slot {
    bool                 dirty;
    enum debug_slot_type type;
    union {
        byte byte;
        struct {
            word value;
            bool has_indirect;
            byte indirect;
        } word;
        bool boolean;
        int  integer;
        char str[DEBUGGER_SLOT_STRING_LEN];
    };
};

enum debug_mode {
    DEBUG_CPU = 0,
    DEBUG_PPU = 1
};

static struct {
    struct window*  window;
    struct textbox* mode_bar_labels[DEBUGGER_MODE_COUNT];
    struct textbox* textboxes[DEBUGGER_TEXTBOX_COUNT];
    size_t          static_textbox_count;

    SDL_RWLock*     mode_rw_lock;
    enum debug_mode mode;
    bool            mode_dirty;

    SDL_RWLock*       slot_values_rw_lock;
    struct debug_slot slot_values[DEBUGGER_SLOT_COUNT];
} m_debugger = {0};

static int  emulation_loop(void* arg);
static void m_cycle(void);
static void t_cycle(void);

static inline bool debugger_is_any_slot_dirty(
    const struct debug_slot* slots,
    const size_t             start_inclusive,
    const size_t             end_inclusive
);
static void debugger_update_slot_values(void);
static void debugger_event_handler(const struct event* evt);
static void debugger_window_event(const struct event_window* evt);
static void debugger_input_event(const struct event_input* evt);
static void debugger_button_input_event(const struct event_input_button* evt);

bool gboy_load_cart(const char* path) {
    int rc;

    if (m_gboy.cart != NULL) {
        log_warn("Cartridge is already loaded. Eject old one first.");
        return false;
    }

    m_gboy.cart = cartridge_create(path);
    if (m_gboy.cart == NULL) {
        log_error("Failed to create cartridge object");
        return false;
    }

    rc = mmu_load_cartridge(m_gboy.cart);
    if (rc == -1) {
        log_error("Bad cartridge. Couldn't load data.");
        return false;
    } else if (rc == -2) {
        log_warn("Cartridge is already loaded. Eject old one first.");
        return false;
    }

    return true;
}
void gboy_eject_cart(void) {
    mmu_eject_cartridge();
    cartridge_free(&m_gboy.cart);
}

bool gboy_poweron(const size_t clock_speed) {
    if (m_gboy.control_rw_lock != NULL) {
        log_warn("GBoy is already running. Call gboy_poweroff() first.");
        return false;
    }

    // Setup RW locks
    m_gboy.control_rw_lock = SDL_CreateRWLock();
    if (m_gboy.control_rw_lock == NULL) {
        LOG_SDL_ERROR("Could not power on GBoy. SDL failed to create RWLock.");
        goto err_cleanup;
    }
    m_gboy.lcd_rw_lock = SDL_CreateRWLock();
    if (m_gboy.control_rw_lock == NULL) {
        LOG_SDL_ERROR("Could not power on GBoy. SDL failed to create RWLock.");
        goto err_cleanup;
    }

    // Setup properties
    m_gboy.running     = true;
    m_gboy.clock_speed = clock_speed;
    for (size_t i = 0; i < GBOY_LCD_SIZE; i++) { m_gboy.lcd[i] = (pixel_t){0, 0, 0, 255}; }

    // Setup Thread
    m_gboy.emulation_paused_sem = SDL_CreateSemaphore(0);
    if (m_gboy.emulation_paused_sem == NULL) {
        LOG_SDL_ERROR("Could not power on GBoy. SDL failed to create emulation semaphore.");
        goto err_cleanup;
    }
    //! This must always be the last thing to setup incase our current thread gets preempted
    //! Must ensure GBoy is in a fully initialized state
    m_gboy.emulation_thread = SDL_CreateThread(emulation_loop, "gboy::emulation", NULL);
    if (m_gboy.emulation_thread == NULL) {
        LOG_SDL_ERROR("Could not power on GBoy. SDL failed to create emulation thread.");
        goto err_cleanup;
    }

    return true;

err_cleanup:
    m_gboy.running = false;
    if (m_gboy.control_rw_lock != NULL) {
        SDL_DestroyRWLock(m_gboy.control_rw_lock);
        m_gboy.control_rw_lock = NULL;
    }
    if (m_gboy.lcd_rw_lock != NULL) {
        SDL_DestroyRWLock(m_gboy.lcd_rw_lock);
        m_gboy.lcd_rw_lock = NULL;
    }
    if (m_gboy.emulation_paused_sem != NULL) {
        SDL_DestroySemaphore(m_gboy.emulation_paused_sem);
        m_gboy.emulation_paused_sem = NULL;
    }
    // Thread Must be last thing created thus if it fails it was never created in the first place
    return false;
}

void gboy_poweroff(void) {
    if (m_gboy.control_rw_lock == NULL) {
        log_warn("Gboy is already powered off.");
        return;
    }

    // Stop thread
    SDL_LockRWLockForWriting(m_gboy.control_rw_lock);
    m_gboy.running     = false;
    // Ensure thread can run in the event it is currently paused
    m_gboy.clock_speed = 1;
    SDL_SignalSemaphore(m_gboy.emulation_paused_sem);
    SDL_UnlockRWLock(m_gboy.control_rw_lock);
    SDL_WaitThread(m_gboy.emulation_thread, NULL);
    //* Note: Safe to NULL the thread object after it has exited as SDL will have already cleaned it
    //*       up
    m_gboy.emulation_thread = NULL;

    // Destroy remaing GBoy state
    SDL_DestroyRWLock(m_gboy.control_rw_lock);
    m_gboy.control_rw_lock = NULL;
    SDL_DestroyRWLock(m_gboy.lcd_rw_lock);
    m_gboy.lcd_rw_lock = NULL;
    SDL_DestroySemaphore(m_gboy.emulation_paused_sem);
    m_gboy.emulation_paused_sem = NULL;
    m_gboy.clock_speed          = 0;
}

bool gboy_step(void) {
    if (m_gboy.control_rw_lock == NULL) {
        log_warn("GBoy is powered off. Cannot step the emulation.");
        return false;
    }

    SDL_LockRWLockForReading(m_gboy.control_rw_lock);
    if (m_gboy.clock_speed != 0) {
        SDL_UnlockRWLock(m_gboy.control_rw_lock);
        log_warn("GBoy clock speed is non-zero. Cannot step the emulation.");
        return false;
    }
    SDL_UnlockRWLock(m_gboy.control_rw_lock);

    SDL_SignalSemaphore(m_gboy.emulation_paused_sem);

    return true;
}

bool gboy_set_clock_speed(const size_t clock_speed) {
    if (m_gboy.control_rw_lock == NULL) {
        log_warn("GBoy is powered off. Cannot step the emulation.");
        return false;
    }

    SDL_LockRWLockForWriting(m_gboy.control_rw_lock);
    m_gboy.clock_speed = clock_speed;

    if (clock_speed == 0) {
        // User is attempting to pause the emulated, ensure pause semaphore is empty by draining it
        while (SDL_TryWaitSemaphore(m_gboy.emulation_paused_sem));
    } else if (SDL_GetSemaphoreValue(m_gboy.emulation_paused_sem) == 0) {
        // User is either unpausing or change emulation speed. Ensure pause is exited (since we
        // drain on pause, this is safe).
        SDL_SignalSemaphore(m_gboy.emulation_paused_sem);
    }
    SDL_UnlockRWLock(m_gboy.control_rw_lock);

    return true;
}

size_t gboy_get_clock_speed(void) {
    if (m_gboy.control_rw_lock == NULL) {
        log_warn("GBoy is powered off. Returning clock speed of 0");
        return 0;
    }

    SDL_LockRWLockForReading(m_gboy.control_rw_lock);
    size_t clock_speed = m_gboy.clock_speed;
    SDL_UnlockRWLock(m_gboy.control_rw_lock);

    return clock_speed;
}

bool gboy_get_lcd(pixel_t* pixel_buffer, const size_t size) {
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

    if (m_gboy.control_rw_lock == NULL) {
        log_debug("GBoy is powered off. Returning blank (black) frame");
        for (size_t i = 0; i < GBOY_LCD_SIZE; i++) { pixel_buffer[i] = (pixel_t){0, 0, 0, 255}; }
        return true;
    } else {
        SDL_LockRWLockForReading(m_gboy.lcd_rw_lock);
        SDL_memcpy(pixel_buffer, m_gboy.lcd, sizeof(m_gboy.lcd));
        SDL_UnlockRWLock(m_gboy.lcd_rw_lock);
        return true;
    }
}

void gboy_set_lcd_pixel(const size_t x, const size_t y, const pixel_t pixel) {
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
        return;
    }

    if (m_gboy.control_rw_lock == NULL) {
        log_error("Managed to call set lcd pixel while gameboy is powered off!?");
        return;
    }

    size_t index = y * GBOY_LCD_WIDTH + x;
    SDL_LockRWLockForWriting(m_gboy.lcd_rw_lock);
    m_gboy.lcd[index] = pixel;
    SDL_UnlockRWLock(m_gboy.lcd_rw_lock);

    return;
}

bool gboy_debugger_is_open(void) { return m_debugger.window != NULL; }

void gboy_debugger_open(void) {
    if (m_debugger.window != NULL) {
        log_warn("Debugger is already open");
        return;
    }

    m_debugger.mode       = DEBUG_CPU;
    m_debugger.mode_dirty = true;

    m_debugger.mode_rw_lock        = SDL_CreateRWLock();
    m_debugger.slot_values_rw_lock = SDL_CreateRWLock();
    if (m_debugger.mode_rw_lock == NULL || m_debugger.slot_values_rw_lock == NULL) {
        LOG_SDL_ERROR("Couldn't open debugger. SDL failed to create RWLock");
        goto err_cleanup;
    }

    m_debugger.window = window_create("Debugger", DEBUGGER_WIDTH, DEBUGGER_HEIGHT, 1);
    if (m_debugger.window == NULL) {
        log_error("Couldn't open debugger. Failed to create debug window");
        goto err_cleanup;
    }

    struct asset font_file = assets_get_file(FONT_FILEPATH);

    const char* box_labels[DEBUGGER_MODE_COUNT] = {"CPU", "PPU"};
    for (int i = 0; i < DEBUGGER_MODE_COUNT; i++) {
        m_debugger.mode_bar_labels[i] = textbox_create(
            m_debugger.window,
            &font_file,
            DEBUGGER_MODE_FONT_SIZE,
            DEBUGGER_TEXT_COLOUR,
            box_labels[i]
        );
        if (m_debugger.mode_bar_labels[i] == NULL) {
            log_error("Couldn't open debugger. Failed to create labels");
            goto err_cleanup;
        }
    }

    for (int i = 0; i < DEBUGGER_TEXTBOX_COUNT; i++) {
        m_debugger.textboxes[i] = textbox_create(
            m_debugger.window,
            &font_file,
            DEBUGGER_TEXT_FONT_SIZE,
            DEBUGGER_TEXT_COLOUR,
            ""
        );
        if (m_debugger.textboxes[i] == NULL) {
            log_error("Failed to create content textboxes.");
            goto err_cleanup;
        }
    }

    window_register_event_handler(m_debugger.window, debugger_event_handler);

    // Before finishing up, check if emulator is currently paused
    // If so, we should quickly update the debuggers slot values
    if (gboy_get_clock_speed() == 0) { debugger_update_slot_values(); }

    return;

err_cleanup:
    for (int i = 0; i < DEBUGGER_MODE_COUNT; i++) {
        if (m_debugger.mode_bar_labels[i] != NULL) {
            textbox_destroy(&m_debugger.mode_bar_labels[i]);
        }
    }
    for (int i = 0; i < DEBUGGER_TEXTBOX_COUNT; i++) {
        if (m_debugger.textboxes[i] != NULL) { textbox_destroy(&m_debugger.textboxes[i]); }
    }
    if (m_debugger.window != NULL) { window_destroy(&m_debugger.window); }
    if (m_debugger.slot_values_rw_lock != NULL) {
        SDL_DestroyRWLock(m_debugger.slot_values_rw_lock);
        m_debugger.slot_values_rw_lock = NULL;
    }
    if (m_debugger.mode_rw_lock != NULL) {
        SDL_DestroyRWLock(m_debugger.mode_rw_lock);
        m_debugger.mode_rw_lock = NULL;
    }
    return;
}

void gboy_debugger_close(void) {
    for (int i = 0; i < DEBUGGER_MODE_COUNT; i++) {
        if (m_debugger.mode_bar_labels[i] != NULL) {
            textbox_destroy(&m_debugger.mode_bar_labels[i]);
        }
    }
    for (int i = 0; i < DEBUGGER_TEXTBOX_COUNT; i++) {
        if (m_debugger.textboxes[i] != NULL) { textbox_destroy(&m_debugger.textboxes[i]); }
    }
    if (m_debugger.window != NULL) { window_destroy(&m_debugger.window); }
    if (m_debugger.slot_values_rw_lock != NULL) {
        SDL_DestroyRWLock(m_debugger.slot_values_rw_lock);
        m_debugger.slot_values_rw_lock = NULL;
    }
    if (m_debugger.mode_rw_lock != NULL) {
        SDL_DestroyRWLock(m_debugger.mode_rw_lock);
        m_debugger.mode_rw_lock = NULL;
    }

    return;
}

void gboy_debugger_update(void) {
    const struct rect content_box = {
        .x = 0,
        .y = DEBUGGER_BAR_HEIGHT,
        .w = DEBUGGER_WIDTH,
        .h = DEBUGGER_HEIGHT - DEBUGGER_BAR_HEIGHT
    };

    // Fetch mode without potentially blocking other threads
    SDL_LockRWLockForReading(m_debugger.mode_rw_lock);
    enum debug_mode mode       = m_debugger.mode;
    bool            mode_dirty = m_debugger.mode_dirty;
    SDL_UnlockRWLock(m_debugger.mode_rw_lock);

    // Only redraw bar if the mode is dirty
    if (mode_dirty == true) {
        // Full window needs to be redrawn
        window_fill(m_debugger.window, DEBUGGER_BKG_COLOUR);

        // Draw top bar
        struct rect box = {
            .x = 0,
            .y = 0,
            .w = DEBUGGER_BAR_BOX_WIDTH,
            .h = DEBUGGER_BAR_HEIGHT,
        };
        int         selected_box = (int)mode;
        struct rect box_outline  = {0};
        struct rect box_fill     = {0};
        for (int i = 0; i < DEBUGGER_MODE_COUNT; i++) {
            box_outline  = box;
            box_fill     = box;
            box_fill.h  -= 6;
            box_fill.w  -= 6;
            box_fill.x  += 3;
            box_fill.y  += 3;

            // Draw Box Rect
            window_draw_rect_filled(m_debugger.window, box_outline, DEBUGGER_OUTLINE_COLOUR);
            window_draw_rect_filled(
                m_debugger.window,
                box_fill,
                (i == selected_box) ? DEBUGGER_SELECTED_COLOUR : DEBUGGER_UNSELECTED_COLOUR
            );

            // Draw Box Label
            float label_width  = 0;
            float label_height = 0;
            textbox_get_size(m_debugger.mode_bar_labels[i], &label_width, &label_height);
            float text_x = box.x + (DEBUGGER_BAR_BOX_WIDTH - label_width) / 2;
            float text_y = (DEBUGGER_BAR_HEIGHT - label_height) / 2;
            textbox_draw(m_debugger.mode_bar_labels[i], text_x, text_y);

            // Move to next box
            box.x += DEBUGGER_BAR_BOX_WIDTH;
        }

        // Set "static" textboxes for new mode
        switch (mode) {
        case DEBUG_CPU:
            textbox_set_text(m_debugger.textboxes[0], "CORE");
            textbox_set_text(m_debugger.textboxes[1], "16-BIT REGISTERS");
            textbox_set_text(m_debugger.textboxes[2], "8-BIT REGISTERS");
            textbox_set_text(m_debugger.textboxes[3], "FLAGS");
            m_debugger.static_textbox_count = 4;
            break;

        case DEBUG_PPU:
            textbox_set_text(m_debugger.textboxes[0], "LCDC Register");
            textbox_set_text(m_debugger.textboxes[1], "STAT Register");
            textbox_set_text(m_debugger.textboxes[2], "Position");
            textbox_set_text(m_debugger.textboxes[3], "Palette");
            m_debugger.static_textbox_count = 4;
            break;
        default:
            log_warn("Static draw for this Mode is not implemented");
            break;
        }

        // Clear dirty flag
        SDL_LockRWLockForWriting(m_debugger.mode_rw_lock);
        m_debugger.mode_dirty = false;
        SDL_UnlockRWLock(m_debugger.mode_rw_lock);
    } else {
        // Only content has to be redrawn
        window_draw_rect_filled(m_debugger.window, content_box, DEBUGGER_BKG_COLOUR);
    }

    // Draw mode content
    const char*       textbox_fmts[DEBUGGER_TEXTBOX_COUNT];
    const size_t      textbox_text_capacity = 256;
    char              textbox_text_buf[textbox_text_capacity];
    struct debug_slot slots[DEBUGGER_SLOT_COUNT] = {0};
    const float       indent                     = 5;
    const float       divider_thickness          = 5;
    float             x                          = content_box.x;
    float             y                          = content_box.y;
    float             section_width              = 0;
    float             section_height             = 0;
    float             title_width                = 0;
    float             title_height               = 0;
    struct rect       rect                       = {0};
    switch (mode) {
    // Draw CPU debug screen
    case DEBUG_CPU:
        // Content Window:
        //
        //           Control               |  Flags
        // PC: 0x0000 -> 0x00 | 0x00  0x00 |
        // SP: 0x0000 -> 0x00 | 0x00  0x00 | Z: [x]
        // --------------------------------| N: [x]
        //            16-bit               | H: [x]
        // BC: 0x0000 (00000) -> 0x00 (000)| C: [x]
        // DE: 0x0000 (00000) -> 0x00 (000)|
        // HL: 0x0000 (00000) -> 0x00 (000)|
        // AF: 0x0000 (00000)              |
        // --------------------------------|
        //             8-bit               |
        //  B: 0x00   (000)   C: 0x00 (000)|
        //  D: 0x00   (000)   E: 0x00 (000)|
        //  H: 0x00   (000)   L: 0x00 (000)|
        //  A: 0x00   (000)   F: 0x00 (000)|

        // CPU mode Slots:
        // - Core Section [0-5]:
        //  - PC w/ indirect  [0]
        //  - PC indirect + 1 [1]
        //  - PC indirect + 2 [2]
        //  - SP w/ indirect  [3]
        //  - SP indirect + 1 [4]
        //  - SP indirect + 2 [5]
        // - 16-bit Section [6-9]:
        //  - BC w/ indirect [6]
        //  - DE w/ indirect [7]
        //  - HL w/ indirect [8]
        //  - AF             [9]
        // - 8-bit Section [10-17]:
        //  - B [10]
        //  - C [11]
        //  - D [12]
        //  - E [13]
        //  - H [14]
        //  - L [15]
        //  - A [16]
        //  - F [17]
        // - Flags Section [18-21]:
        //  - Z [18]
        //  - N [19]
        //  - H [20]
        //  - C [21]
        // 21 total slots in use

        const size_t cpu_slots_in_use = 21;

        textbox_fmts[0] =
            "PC: 0x%1$.4X -> 0x%2$.2X | 0x%3$.2X  0x%4$.2X\nSP: 0x%5$.4X -> 0x%6$.2X | 0x%7$.2X  "
            "0x%8$.2X";
        textbox_fmts[1] = "BC: 0x%1$.4X (%1$05u) -> 0x%2$.2X (%2$03u)\n"
                          "DE: 0x%3$.4X (%3$05u) -> 0x%4$.2X (%4$03u)\n"
                          "HL: 0x%5$.4X (%5$05u) -> 0x%6$.2X (%6$03u)\n"
                          "AF: 0x%7$.4X (%7$05u)";
        textbox_fmts[2] = " B: 0x%1$.2X (%1$03u)     C: 0x%2$.2X (%2$03u)\n"
                          " D: 0x%3$.2X (%3$03u)     E: 0x%4$.2X (%4$03u)\n"
                          " H: 0x%5$.2X (%5$03u)     L: 0x%6$.2X (%6$03u)\n"
                          " A: 0x%7$.2X (%7$03u)     F: 0x%8$.2X (%8$03u)";
        textbox_fmts[3] = "Z: [%1$c]\n"
                          "N: [%2$c]\n"
                          "H: [%3$c]\n"
                          "C: [%4$c]";

        // Make a copy of the slots to prevent halting the emulation loop
        SDL_LockRWLockForWriting(m_debugger.slot_values_rw_lock);
        SDL_memcpy(slots, m_debugger.slot_values, sizeof(struct debug_slot) * cpu_slots_in_use);
        for (int i = 0; i < cpu_slots_in_use; i++) { m_debugger.slot_values[i].dirty = false; }
        SDL_UnlockRWLock(m_debugger.slot_values_rw_lock);

        // Update Core section text if any dependant slots [0-5] are dirty (or if mode was dirty)
        if (mode_dirty == true || debugger_is_any_slot_dirty(slots, 0, 5)) {
            int required_length = SDL_snprintf(
                textbox_text_buf,
                textbox_text_capacity,
                textbox_fmts[0],
                slots[0].word.value,
                slots[0].word.indirect,
                slots[1].byte,
                slots[2].byte,
                slots[3].word.value,
                slots[3].word.indirect,
                slots[4].byte,
                slots[5].byte
            );
            if (required_length >= textbox_text_capacity) {
                log_error("Debugger: Attempted to format full slot line but overran the buffer. "
                          "Slot line was truncated.");
            }
            textbox_set_text(
                m_debugger.textboxes[m_debugger.static_textbox_count + 0],
                textbox_text_buf
            );
        }

        // Update 16-bit register section text if any dependant slots [6-9] are dirty (or if mode
        // was dirty)
        if (mode_dirty == true || debugger_is_any_slot_dirty(slots, 6, 9)) {
            int required_length = SDL_snprintf(
                textbox_text_buf,
                textbox_text_capacity,
                textbox_fmts[1],
                slots[6].word.value,
                slots[6].word.indirect,
                slots[7].word.value,
                slots[7].word.indirect,
                slots[8].word.value,
                slots[8].word.indirect,
                slots[9].word.value
            );
            if (required_length >= textbox_text_capacity) {
                log_error("Debugger: Attempted to format full slot line but overran the buffer. "
                          "Slot line was truncated.");
            }
            textbox_set_text(
                m_debugger.textboxes[m_debugger.static_textbox_count + 1],
                textbox_text_buf
            );
        }

        // Update 8-bit register section text if any dependant slots [10-17] are dirty (or if mode
        // was dirty)
        if (mode_dirty == true || debugger_is_any_slot_dirty(slots, 10, 17)) {
            int required_length = SDL_snprintf(
                textbox_text_buf,
                textbox_text_capacity,
                textbox_fmts[2],
                slots[10].byte,
                slots[11].byte,
                slots[12].byte,
                slots[13].byte,
                slots[14].byte,
                slots[15].byte,
                slots[16].byte,
                slots[17].byte
            );
            if (required_length >= textbox_text_capacity) {
                log_error("Debugger: Attempted to format full slot line but overran the buffer. "
                          "Slot line was truncated.");
            }
            textbox_set_text(
                m_debugger.textboxes[m_debugger.static_textbox_count + 2],
                textbox_text_buf
            );
        }

        // Update flags section text if any dependant slots [18-21] are dirty (or if mode was dirty)
        if (mode_dirty == true || debugger_is_any_slot_dirty(slots, 18, 21)) {
            int required_length = SDL_snprintf(
                textbox_text_buf,
                textbox_text_capacity,
                textbox_fmts[3],
                (slots[18].boolean == true) ? 'x' : ' ',
                (slots[19].boolean == true) ? 'x' : ' ',
                (slots[20].boolean == true) ? 'x' : ' ',
                (slots[21].boolean == true) ? 'x' : ' '
            );
            if (required_length >= textbox_text_capacity) {
                log_error("Debugger: Attempted to format full slot line but overran the buffer. "
                          "Slot line was truncated.");
            }
            textbox_set_text(
                m_debugger.textboxes[m_debugger.static_textbox_count + 3],
                textbox_text_buf
            );
        }

        // Section width should be set to the largest section of the first 3
        for (int i = 0; i < 3; i++) {
            float temp_width = 0;
            textbox_get_size(m_debugger.textboxes[i], &temp_width, NULL);
            temp_width    += 2 * indent;
            section_width  = (temp_width > section_width) ? temp_width : section_width;
            textbox_get_size(
                m_debugger.textboxes[m_debugger.static_textbox_count + i],
                &temp_width,
                NULL
            );
            temp_width    += 2 * indent;
            section_width  = (temp_width > section_width) ? temp_width : section_width;
        }

        // Draw Left size content (3 sections)
        for (int i = 0; i < 3; i++) {
            // Get section dimensions
            textbox_get_size(m_debugger.textboxes[i], &title_width, &title_height);
            textbox_get_size(
                m_debugger.textboxes[m_debugger.static_textbox_count + i],
                NULL, // Ignore actual section width
                &section_height
            );

            // Draw section divider (5 pixel line)
            rect = (struct rect){
                .x = content_box.x, // start at far left of content box
                .y = y,             // start after previous section
                .w = section_width, // draw across section
                .h = divider_thickness,
            };
            window_draw_rect_filled(m_debugger.window, rect, DEBUGGER_OUTLINE_COLOUR);

            // Label
            x  = content_box.x + (section_width - title_width) / 2; // Center on line
            y += divider_thickness;                                 // Push down by divider
            textbox_draw(m_debugger.textboxes[i], x, y);

            // Content
            x  = content_box.x + indent; // Left align on indent
            y += title_height;           // push down by title
            textbox_draw(m_debugger.textboxes[m_debugger.static_textbox_count + i], x, y);

            y += section_height;
        }

        // Draw vertical section divider
        rect = (struct rect){
            .x = content_box.x + section_width, // Start at end of left section
            .y = content_box.y,                 // Start at top of content box
            .w = divider_thickness,
            .h = content_box.h, // End at bottom of content box
        };
        window_draw_rect_filled(m_debugger.window, rect, DEBUGGER_OUTLINE_COLOUR);

        float left_content_width = section_width + divider_thickness; // Store left end pos
        section_width =
            content_box.w - section_width; // invert width (right should be whatever is remaining)

        // Draw Flag Section
        textbox_get_size(m_debugger.textboxes[3], &title_width, &title_height);
        textbox_get_size(
            m_debugger.textboxes[m_debugger.static_textbox_count + 3],
            NULL, // Ignore actual section width
            &section_height
        );
        rect = (struct rect){
            .x = content_box.x + left_content_width,
            .y = content_box.y,
            .w = section_width,
            .h = divider_thickness,
        };
        window_draw_rect_filled(m_debugger.window, rect, DEBUGGER_OUTLINE_COLOUR);
        x = (content_box.x + left_content_width) + (section_width - title_width) / 2;
        y = content_box.y + divider_thickness;
        textbox_draw(m_debugger.textboxes[3], x, y);
        x  = content_box.x + left_content_width + indent;
        y += title_height;
        textbox_draw(m_debugger.textboxes[m_debugger.static_textbox_count + 3], x, y);

        // All CPU mode drawing should be done by this point
        break;

    // Draw PPU debug screen
    case DEBUG_PPU:
        // Content Window:
        //
        //                 LCDC Register
        // LCD|WTM|WIN|TDA|BTM|OBJ|OBE|BGW
        // [x]| 0 |[x]| 0 | 0 | 0 |[x]|[x]
        // ---------------------------------------------
        //                 STAT Register
        //  7 |LYI|OAI|VBI|HBI|LYF|  MODE
        // ---|[x]|[x]|[x]|[x]|[x]| OAM SCAN
        // ---------------------------------------------
        //                   Position
        // SCN DOT: 000    | TOT DOT: 0000
        // LY:  0x00 (000) | LYC: 0x00 (000)
        // SCX: 0x00 (000) | WX:  0x00 (000)
        // SCY: 0x00 (000) | WY:  0x00 (000)
        // ---------------------------------------------
        //                   Palette?
        //

        // CPU mode Slots:
        // - LCDC Section [0]:
        //  - LCDC [0]
        // - STAT Section [1]:
        //  - STAT [1]
        // - Position Section [2-8]:
        //  - Dots [2]
        //  - LY   [3]
        //  - LYC  [4]
        //  - SCX  [5]
        //  - SCY  [6]
        //  - WX   [7]
        //  - WY   [8]
        // - Palette Section [9-?]:
        //  - ? [9]
        // 9-? total slots in use

        const size_t ppu_slots_in_use = 9;

        textbox_fmts[0] = "LCD|WTM|WIN|TDA|BTM|OBJ|OBE|PRI\n"
                          "[%c]|[%c]|[%c]|[%c]|[%c]|[%c]|[%c]|[%c]";
        textbox_fmts[1] = " 7 |LYI|OAI|VBI|HBI|LYF| MODE \n"
                          "---|[%c]|[%c]|[%c]|[%c]|[%c]|%s";
        textbox_fmts[2] = "SCN DOT: %1$03u    | TOT DOT: %2$05u\n"
                          "LY:  0x%3$.2X (%3$03u) | LYC: 0x%4$.2X (%4$03u)\n"
                          "SCX: 0x%5$.2X (%5$03u) | WX:  0x%7$.2X (%7$03u)\n"
                          "SCY: 0x%6$.2X (%6$03u) | WY:  0x%8$.2X (%8$03u)";
        textbox_fmts[3] = "%s";

        // Make a copy of the slots to prevent halting the emulation loop
        SDL_LockRWLockForWriting(m_debugger.slot_values_rw_lock);
        SDL_memcpy(slots, m_debugger.slot_values, sizeof(struct debug_slot) * ppu_slots_in_use);
        for (int i = 0; i < ppu_slots_in_use; i++) { m_debugger.slot_values[i].dirty = false; }
        SDL_UnlockRWLock(m_debugger.slot_values_rw_lock);

        // Update LCDC section text if any dependant slots [0] are dirty (or if mode was dirty)
        if (mode_dirty == true || debugger_is_any_slot_dirty(slots, 0, 0)) {
            int required_length = SDL_snprintf(
                textbox_text_buf,
                textbox_text_capacity,
                textbox_fmts[0],
                ((slots[0].byte & REG_LCDC_PPU_ENABLE_MASK) != 0x00) ? 'x' : ' ',
                ((slots[0].byte & REG_LCDC_WIN_TILE_MAP_MASK) != 0x00) ? 'x' : ' ',
                ((slots[0].byte & REG_LCDC_WIN_ENABLE_MASK) != 0x00) ? 'x' : ' ',
                ((slots[0].byte & REG_LCDC_TILE_DATA_MASK) != 0x00) ? 'x' : ' ',
                ((slots[0].byte & REG_LCDC_BKG_TILE_MAP_MASK) != 0x00) ? 'x' : ' ',
                ((slots[0].byte & REG_LCDC_OBJ_SIZE_MASK) != 0x00) ? 'x' : ' ',
                ((slots[0].byte & REG_LCDC_OBJ_ENABLE_MASK) != 0x00) ? 'x' : ' ',
                ((slots[0].byte & REG_LCDC_PRIORITY_MASK) != 0x00) ? 'x' : ' '
            );
            if (required_length >= textbox_text_capacity) {
                log_error("Debugger: Attempted to format full slot line but overran the buffer. "
                          "Slot line was truncated.");
            }
            textbox_set_text(
                m_debugger.textboxes[m_debugger.static_textbox_count + 0],
                textbox_text_buf
            );
        }

        // Update STAT section text if any dependant slots [0] are dirty (or if mode was dirty)
        if (mode_dirty == true || debugger_is_any_slot_dirty(slots, 1, 1)) {
            int required_length = SDL_snprintf(
                textbox_text_buf,
                textbox_text_capacity,
                textbox_fmts[1],
                ((slots[1].byte & REG_STAT_LYC_INT_SEL_MASK) != 0x00) ? 'x' : ' ',
                ((slots[1].byte & REG_STAT_MODE2_INT_SEL_MASK) != 0x00) ? 'x' : ' ',
                ((slots[1].byte & REG_STAT_MODE1_INT_SEL_MASK) != 0x00) ? 'x' : ' ',
                ((slots[1].byte & REG_STAT_MODE0_INT_SEL_MASK) != 0x00) ? 'x' : ' ',
                ((slots[1].byte & REG_STAT_LYC_LY_EQ_MASK) != 0x00) ? 'x' : ' ',
                ppu_mode_to_string(slots[1].byte & REG_STAT_MODE_MASK)
            );
            if (required_length >= textbox_text_capacity) {
                log_error("Debugger: Attempted to format full slot line but overran the buffer. "
                          "Slot line was truncated.");
            }
            textbox_set_text(
                m_debugger.textboxes[m_debugger.static_textbox_count + 1],
                textbox_text_buf
            );
        }

        // Update Position section text if any dependant slots [0] are dirty (or if mode was dirty)
        if (mode_dirty == true || debugger_is_any_slot_dirty(slots, 2, 8)) {
            int required_length = SDL_snprintf(
                textbox_text_buf,
                textbox_text_capacity,
                textbox_fmts[2],
                slots[2].integer,
                slots[2].integer + 456 * (int)slots[3].byte,
                slots[3].byte,
                slots[4].byte,
                slots[5].byte,
                slots[6].byte,
                slots[7].byte,
                slots[8].byte
            );
            if (required_length >= textbox_text_capacity) {
                log_error("Debugger: Attempted to format full slot line but overran the buffer. "
                          "Slot line was truncated.");
            }
            textbox_set_text(
                m_debugger.textboxes[m_debugger.static_textbox_count + 2],
                textbox_text_buf
            );
        }

        // Update Palette section text if any dependant slots [9-?] are dirty (or if mode was dirty)
        if (mode_dirty == true || debugger_is_any_slot_dirty(slots, 9, 9)) {
            int required_length =
                SDL_snprintf(textbox_text_buf, textbox_text_capacity, textbox_fmts[3], "");
            if (required_length >= textbox_text_capacity) {
                log_error("Debugger: Attempted to format full slot line but overran the buffer. "
                          "Slot line was truncated.");
            }
            textbox_set_text(
                m_debugger.textboxes[m_debugger.static_textbox_count + 3],
                textbox_text_buf
            );
        }

        // Section width should be set to the largest section
        for (int i = 0; i < 4; i++) {
            float temp_width = 0;
            textbox_get_size(m_debugger.textboxes[i], &temp_width, NULL);
            temp_width    += 2 * indent;
            section_width  = (temp_width > section_width) ? temp_width : section_width;
            textbox_get_size(
                m_debugger.textboxes[m_debugger.static_textbox_count + i],
                &temp_width,
                NULL
            );
            temp_width    += 2 * indent;
            section_width  = (temp_width > section_width) ? temp_width : section_width;
        }

        // Draw content (4 sections)
        for (int i = 0; i < 4; i++) {
            // Get section dimensions
            textbox_get_size(m_debugger.textboxes[i], &title_width, &title_height);
            textbox_get_size(
                m_debugger.textboxes[m_debugger.static_textbox_count + i],
                NULL, // Ignore actual section width
                &section_height
            );

            // Draw section divider (5 pixel line)
            rect = (struct rect){
                .x = content_box.x, // start at far left of content box
                .y = y,             // start after previous section
                .w = content_box.w, // draw across section
                .h = divider_thickness,
            };
            window_draw_rect_filled(m_debugger.window, rect, DEBUGGER_OUTLINE_COLOUR);

            // Label
            x  = content_box.x + (section_width - title_width) / 2; // Center on line
            y += divider_thickness;                                 // Push down by divider
            textbox_draw(m_debugger.textboxes[i], x, y);

            // Content
            x  = content_box.x + indent; // Left align on indent
            y += title_height;           // push down by title
            textbox_draw(m_debugger.textboxes[m_debugger.static_textbox_count + i], x, y);

            y += section_height;
        }

        // All PPU mode drawing should be done by this point
        break;

    // Unknown mode
    default:
        log_error("Invalid debug mode set");
        break;
    }

    window_present(m_debugger.window);

    return;
}

static int emulation_loop(void* arg) {

    //! NOTE SLEEPING ON A SEMAPHORE IS TOO SLOW. OS TAKES WAY TOO LONG TO WAKE THE THREAD BACK UP
    //! (0.6 [ms])
    // TODO: Switch loop to work as a spin loop and force the kernel to preempt us instead.
    while (true) {
        SDL_LockRWLockForReading(m_gboy.control_rw_lock);
        // Immediately exit if no longer running
        if (m_gboy.running == false) { break; }
        if (m_gboy.clock_speed == 0) {
            // Light sleep and restart loop
            SDL_UnlockRWLock(m_gboy.control_rw_lock);
            SDL_WaitSemaphore(m_gboy.emulation_paused_sem);

            // Double check if we should exit (prevents executing an additional step before
            // exiting)
            SDL_LockRWLockForReading(m_gboy.control_rw_lock);
            if (m_gboy.running == false) { break; }
            SDL_UnlockRWLock(m_gboy.control_rw_lock);
        } else {
            // Full sleep based on clock speed
            Uint64 sleep_ns = 1000000000 / (Uint64)m_gboy.clock_speed;
            SDL_UnlockRWLock(m_gboy.control_rw_lock);
            SDL_DelayPrecise(sleep_ns);
        }

        // Step GBoy
        // 4 t cycles for every m cycle
        m_cycle();
        t_cycle();
        t_cycle();
        t_cycle();
        t_cycle();
        debugger_update_slot_values();
    }
    SDL_UnlockRWLock(m_gboy.control_rw_lock);

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

static inline bool debugger_is_any_slot_dirty(
    const struct debug_slot* slots,
    const size_t             start_inclusive,
    const size_t             end_inclusive
) {
    for (size_t i = start_inclusive; i <= end_inclusive; i++) {
        if (slots[i].dirty == true) { return true; }
    }
    return false;
}

static void debugger_update_slot_values(void) {
    if (gboy_debugger_is_open() == false) { return; }

    SDL_LockRWLockForReading(m_debugger.mode_rw_lock);
    enum debug_mode mode = m_debugger.mode;
    SDL_UnlockRWLock(m_debugger.mode_rw_lock);

    switch (mode) {
    case DEBUG_CPU:
        // Read from CPU
        struct registers reg          = cpu_snapshot_registers();
        word             r16_values[] = {reg.PC, reg.SP, reg.BC, reg.DE, reg.HL, reg.AF};
        byte             indirects[]  = {
            mmu_read(BUS_EXTERN, reg.PC),
            mmu_read(BUS_EXTERN, reg.PC + 1),
            mmu_read(BUS_EXTERN, reg.PC + 2),
            mmu_read(BUS_EXTERN, reg.SP),
            mmu_read(BUS_EXTERN, reg.SP - 1),
            mmu_read(BUS_EXTERN, reg.SP - 2),
            mmu_read(BUS_EXTERN, reg.BC),
            mmu_read(BUS_EXTERN, reg.DE),
            mmu_read(BUS_EXTERN, reg.HL)
        };
        byte r8_values[] = {reg.B, reg.C, reg.D, reg.E, reg.H, reg.L, reg.A, reg.F};
        bool flags[]     = {
            registers_get_flag_z(&reg),
            registers_get_flag_n(&reg),
            registers_get_flag_h(&reg),
            registers_get_flag_c(&reg)
        };

        SDL_LockRWLockForWriting(m_debugger.slot_values_rw_lock);
        // PC
        if (m_debugger.slot_values[0].word.value != r16_values[0] ||
            m_debugger.slot_values[0].word.indirect != indirects[0]) {
            m_debugger.slot_values[0] = (struct debug_slot){
                .word.value        = r16_values[0],
                .word.has_indirect = true,
                .word.indirect     = indirects[0],
                .dirty             = true,
            };
        }
        // Additional PC indirects
        for (int i = 0; i < 2; i++) {
            if (m_debugger.slot_values[1 + i].byte != indirects[1 + i]) {
                m_debugger.slot_values[1 + i] = (struct debug_slot){
                    .byte  = indirects[1 + i],
                    .dirty = true,
                };
            }
        }

        // SP
        if (m_debugger.slot_values[3].word.value != r16_values[1] ||
            m_debugger.slot_values[3].word.indirect != indirects[3]) {
            m_debugger.slot_values[3] = (struct debug_slot){
                .word.value        = r16_values[1],
                .word.has_indirect = true,
                .word.indirect     = indirects[3],
                .dirty             = true,
            };
        }
        // Additional SP indirects
        for (int i = 0; i < 2; i++) {
            if (m_debugger.slot_values[4 + i].byte != indirects[4 + i]) {
                m_debugger.slot_values[4 + i] = (struct debug_slot){
                    .byte  = indirects[4 + i],
                    .dirty = true,
                };
            }
        }

        // BC, DE, HL
        for (int i = 0; i < 3; i++) {
            if (m_debugger.slot_values[6 + i].word.value != r16_values[2 + i] ||
                m_debugger.slot_values[6 + i].word.indirect != indirects[6 + i]) {
                m_debugger.slot_values[6 + i] = (struct debug_slot){
                    .word.value        = r16_values[2 + i],
                    .word.has_indirect = true,
                    .word.indirect     = indirects[6 + i],
                    .dirty             = true,
                };
            }
        }

        // AF
        if (m_debugger.slot_values[9].word.value != r16_values[5]) {
            m_debugger.slot_values[9] = (struct debug_slot){
                .word.value        = r16_values[5],
                .word.has_indirect = false,
                .word.indirect     = 0x00,
                .dirty             = true,
            };
        }

        // B, C, D, E, H, L, A, F
        for (int i = 0; i < 8; i++) {
            if (m_debugger.slot_values[10 + i].byte != r8_values[i]) {
                m_debugger.slot_values[10 + i] = (struct debug_slot){
                    .byte  = r8_values[i],
                    .dirty = true,
                };
            }
        }

        // Z, N, H, C flags
        for (int i = 0; i < 4; i++) {
            if (m_debugger.slot_values[18 + i].byte != flags[i]) {
                m_debugger.slot_values[18 + i] = (struct debug_slot){
                    .boolean = flags[i],
                    .dirty   = true,
                };
            }
        }
        SDL_UnlockRWLock(m_debugger.slot_values_rw_lock);
        return;

    case DEBUG_PPU:
        word reg_addrs[] = {REG_LCDC, REG_STAT, REG_LY, REG_LYC, REG_SCX, REG_SCY, REG_WX, REG_WY};
        byte bytes[8];
        for (size_t i = 0; i < 8; i++) { bytes[i] = mmu_read(BUS_EXTERN, reg_addrs[i]); }
        int dot_count = ppu_get_scanline_dot_count();

        SDL_LockRWLockForWriting(m_debugger.slot_values_rw_lock);

        // LCDC and STAT
        for (size_t i = 0; i < 2; i++) {
            if (m_debugger.slot_values[i].byte != bytes[i]) {
                m_debugger.slot_values[i] = (struct debug_slot){
                    .byte  = bytes[i],
                    .dirty = true,
                };
            }
        }

        // DOT count
        if (m_debugger.slot_values[2].integer != dot_count) {
            m_debugger.slot_values[2] = (struct debug_slot){
                .integer = dot_count,
                .dirty   = true,
            };
        }

        // LY, LYC, SCX, SCY, WX, WY
        for (size_t i = 0; i < 6; i++) {
            if (m_debugger.slot_values[3 + i].byte != bytes[2 + i]) {
                m_debugger.slot_values[3 + i] = (struct debug_slot){
                    .byte  = bytes[2 + i],
                    .dirty = true,
                };
            }
        }
        SDL_UnlockRWLock(m_debugger.slot_values_rw_lock);
        return;

    default:
        log_warn("Unsupported mode. No update slot loop written");
        return;
    }
}

static void debugger_event_handler(const struct event* evt) {
    switch (evt->type) {
    case EVENT_INPUT:
        debugger_input_event(&evt->input);
        return;
    case EVENT_WINDOW:
        debugger_window_event(&evt->window);
        return;
    default:
        log_warn("Unhandled Event");
        return;
    }
}

static void debugger_window_event(const struct event_window* evt) {
    switch (evt->type) {
    case EVENT_WINDOW_CLOSE_REQUESTED:
        gboy_debugger_close();
        return;
    case EVENT_WINDOW_DESTROYED:
        return;
    default:
        return;
    }
}

static void debugger_input_event(const struct event_input* evt) {
    switch (evt->type) {
    case EVENT_INPUT_BUTTON:
        debugger_button_input_event(&evt->button);
        return;

    // Ignore mouse motion inputs
    case EVENT_INPUT_MOTION:
        return;

    default:
        log_warn("Unhandled input event");
    }
}

static void debugger_button_input_event(const struct event_input_button* evt) {
    switch (evt->device) {
    case DEVICE_KEYBOARD:
        // Close Debugger
        if (evt->button == KEYCODE_GRAVE) {
            // Ignore both keyup and repeat events
            if (evt->down == false || evt->repeat == true) { return; }

            gboy_debugger_close();
            return;
        }

        // CPU Debug Mode
        if (evt->button == KEYCODE_F1) {
            // Ignore both keyup and repeat events
            if (evt->down == false || evt->repeat == true) { return; }
            SDL_LockRWLockForWriting(m_debugger.mode_rw_lock);
            m_debugger.mode       = DEBUG_CPU;
            m_debugger.mode_dirty = true;
            SDL_UnlockRWLock(m_debugger.mode_rw_lock);

            // If paused, force debugger update
            if (gboy_get_clock_speed() == 0) {
                debugger_update_slot_values();
                gboy_debugger_update();
            }

            return;
        }

        // PPU Debug Mode
        if (evt->button == KEYCODE_F2) {
            // Ignore both keyup and repeat events
            if (evt->down == false || evt->repeat == true) { return; }
            SDL_LockRWLockForWriting(m_debugger.mode_rw_lock);
            m_debugger.mode       = DEBUG_PPU;
            m_debugger.mode_dirty = true;
            SDL_UnlockRWLock(m_debugger.mode_rw_lock);

            // If paused, force debugger update
            if (gboy_get_clock_speed() == 0) {
                debugger_update_slot_values();
                gboy_debugger_update();
            }
            return;
        }

        // Pause / Play
        if (evt->button == KEYCODE_SPACE) {
            static size_t cached_clock_speed = 0;

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

        if (evt->button == KEYCODE_LEFT) { }

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

        // Run at standard speed / 1 step/s
        if (evt->button == KEYCODE_1) {
            // Only capture first press
            if (evt->down == false && evt->repeat == true) { return; }

            if (evt->modifier == KEYCODE_MODIFER_LSHIFT) {
                // If LSHIFT modifier, run at 1 step/s
                gboy_set_clock_speed(1);
            } else {
                // Else run at standard speed
                gboy_set_clock_speed(GBOY_DEFAULT_CLOCK_SPEED);
            }
        }
        return;

        // Run at half speed
        if (evt->button == KEYCODE_2) {
            if (evt->down == true && evt->repeat == false) {
                gboy_set_clock_speed(GBOY_DEFAULT_CLOCK_SPEED / 2);
            }
            return;
        }

        // Run at 1/3 speed
        if (evt->button == KEYCODE_3) {
            if (evt->down == true && evt->repeat == false) {
                gboy_set_clock_speed(GBOY_DEFAULT_CLOCK_SPEED / 3);
            }
            return;
        }

        // Run at quater speed
        if (evt->button == KEYCODE_4) {
            if (evt->down == true && evt->repeat == false) {
                gboy_set_clock_speed(GBOY_DEFAULT_CLOCK_SPEED / 4);
            }
            return;
        }

        // Ignore all other keyboard inputs
        return;

    default:
        log_warn("Unhandled Device Button Input");
        return;
    }
}
