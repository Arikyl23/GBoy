/**
 * @file lcd.c
 * @brief Implementation for the LCD display.
 */
#include "lcd.h"

#include <SDL3/SDL.h>
#include <log.h>
#include <string.h>

#include "gboy.h"

LOG_MODULE_SETUP("LCD", CONFIG_LCD_MODULE_LOG_LEVEL);

/** @brief Boundary clock speed. Above this normal LCD operation is used (complete frames are
 * buffered). Below debug LCD operation is used (direct frame streaming).
 */
#define LCD_DEBUG_CLOCK_SPEED 100000

static struct {
    bool init;

    /** @brief Data mutex used during Debug operation to protect the buffer during direct frame
     * streaming.
     */
    struct SDL_Mutex* data_mutex;
    /** @brief Internal Frame data buffers. All three frame buffers: ready, write, draw; exist
     * inside this buffer.
     */
    lcd_frame_t       frame_buffers[3];
    /** @brief Boolean that tracks if the current buffer pointed to by the ready index is stale.
     *  @brief The index is considered stale after swapping draw <-> ready in normal mode.
     *  @brief It is considered fresh when swapping write <-> ready in normal mode.
     */
    SDL_AtomicU32     ready_stale;
    /** @brief Index to the ready frame buffer. This is atomic is allow fast and safe buffer swaps.
     */
    SDL_AtomicU32     ready_index;
    /** @brief Index to the write frame buffer. This is the buffer actively being written to. */
    uint32_t          write_index;
    /** @brief Index to the draw frame buffer. This is the buffer actively being drawn to. */
    uint32_t          draw_index;
} m_ctx = {
    .init = false,
};

bool lcd_init(void) {
    if (m_ctx.init == true) {
        log_warn("LCD is already initialized");
        return true;
    }

    log_info("Initializing LCD...");

    m_ctx.data_mutex = SDL_CreateMutex();
    if (m_ctx.data_mutex == NULL) {
        log_error("SDL Error: Failed to create data mutex.\n\tReason: %s", SDL_GetError());
        SDL_ClearError();
        return false;
    }

    memset(m_ctx.frame_buffers, 0, sizeof(m_ctx.frame_buffers));
    SDL_SetAtomicU32(&m_ctx.ready_index, 0);
    m_ctx.write_index = 1;
    m_ctx.draw_index  = 2;

    m_ctx.init = true;
    log_info("LCD Initialized");
    return true;
}

lcd_frame_t* lcd_request_draw_buffer(void) {
    if (gboy_get_clock_speed() > LCD_DEBUG_CLOCK_SPEED) {
        // Normal Mode:
        //  Frame Buffering
        //  Atomically swap ready <-> draw when the ready buffer isn't stale
        //! NOTE:
        //!  There is a small race condition here that is harmless but may happen if
        //!  lcd_frame_complete() is called while this function is in flight. This function will
        //!  always invalidate whatever frame is in the ready buffer whether that is actually the
        //!  frame we return from this function or not.
        //!
        //! Exact Issue:
        //!  READY_STALE == false -> checkout current READY frame ->
        //!  preempt by thread that completes next frame ->
        //!  this thread resumes and forces READY_STALE to true
        //!
        //!  New frame is silently dropped as the next call to this function will skip the current
        //!  ready frame.
        //!  This has been considered acceptable behaviour as it is incredibly narrow and has
        //!  virtually no percevable effect.
        if (SDL_GetAtomicU32(&m_ctx.ready_stale) == false) {
            m_ctx.draw_index = SDL_SetAtomicU32(&m_ctx.ready_index, m_ctx.draw_index);
            SDL_SetAtomicU32(&m_ctx.ready_stale, true);
        }
        return &m_ctx.frame_buffers[m_ctx.draw_index];
    } else {
        // Debug Mode:
        //  Direct Frame Streaming
        //  1. Lock Mutex
        //  2. Copy write -> draw
        //  3. Unlock Mutex
        SDL_LockMutex(m_ctx.data_mutex);
        memcpy(
            m_ctx.frame_buffers[m_ctx.draw_index],
            m_ctx.frame_buffers[m_ctx.write_index],
            sizeof(lcd_frame_t)
        );
        SDL_UnlockMutex(m_ctx.data_mutex);
        return &m_ctx.frame_buffers[m_ctx.draw_index];
    }
}

void lcd_write_pixel(const pixel_t* pixel, const size_t x, const size_t y) {
    size_t pixel_index = LCD_WIDTH * y + x;

    if (gboy_get_clock_speed() > LCD_DEBUG_CLOCK_SPEED) {
        // Normal Mode:
        //  Write pixel to write buffer
        m_ctx.frame_buffers[m_ctx.write_index][pixel_index] = *pixel;
    } else {
        // Debug Mode:
        //  1. Lock Mutex
        //  2. Write pixel to write buffer
        //  3. Unlock Mutex
        SDL_LockMutex(m_ctx.data_mutex);
        m_ctx.frame_buffers[m_ctx.write_index][pixel_index] = *pixel;
        SDL_UnlockMutex(m_ctx.data_mutex);
    }
}

void lcd_frame_complete(void) {
    if (gboy_get_clock_speed() > LCD_DEBUG_CLOCK_SPEED) {
        // Normal Mode:
        //  Atomically swap ready <-> write clear stale condition
        m_ctx.write_index = SDL_SetAtomicU32(&m_ctx.ready_index, m_ctx.write_index);
        SDL_SetAtomicU32(&m_ctx.ready_stale, false);
    } else {
        // Debug Mode:
        //  Do nothing
    }
}
