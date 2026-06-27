/**
 * @file fetcher.c
 * @brief Implementation for a Gameboy PPU pixel Fetcher.
 */
#include "ppu/fetcher.h"

#include <log.h>
#include <string.h>

#include "memory/mmu.h"

LOG_MODULE_SETUP("Fetcher", CONFIG_FETCHER_MODULE_LOG_LEVEL);

static struct {
    enum fetcher_state state;
    int                step_count;
    word               addr;
    word               pixels;
    byte               lo;
    byte               hi;
} m_ctx = {0};

void fetcher_reset(void) {
    memset(&m_ctx, 0, sizeof(m_ctx));
    m_ctx.state = FETCHER_IDLE;
    log_debug("Fetcher reset");
}

void fetcher_set_fetch_address(const word addr) {
    fetcher_reset();
    m_ctx.state = FETCHER_FETCHING;
    m_ctx.addr  = addr;
    log_debug("Fetching started at address: 0x%.4X", addr);
}

enum fetcher_state fetcher_step(void) {
    log_debug(
        "Stepping Fetcher:\n"
        "\tstate: %s",
        fetcher_state_to_string(m_ctx.state)
    );

    if (m_ctx.state == FETCHER_FETCHING) {
        m_ctx.step_count++;
        log_debug("Step Count: %i", m_ctx.step_count);
        switch (m_ctx.step_count) {
        case 2:
            // Fetch Low Byte
            m_ctx.lo = mmu_read(BUS_PPU, m_ctx.addr);
            log_debug(
                "Fetched Tile Low Byte:\n"
                "\taddr: 0x%.4X\n"
                "\tbyte: 0x%.2X",
                m_ctx.addr,
                m_ctx.lo
            );
            m_ctx.addr++;
            break;
        case 4:
            // Fetch High Byte
            m_ctx.hi = mmu_read(BUS_PPU, m_ctx.addr);
            log_debug(
                "Fetched Tile High Byte:\n"
                "\taddr: 0x%.4X\n"
                "\tbyte: 0x%.2X",
                m_ctx.addr,
                m_ctx.hi
            );
            m_ctx.addr++;
            break;
        case 6:
            // Process bytes into Pixels
            // Place the high bit followed by the low bit
            // Example:
            //   hi = 0x56 | 0 1 0 1 0 1 1 0
            //   lo = 0x7C | 0 1 1 1 1 1 0 0
            //   Pixels =  |------------------------
            //      0x3778 | 00 11 01 11 01 11 10 00
            m_ctx.pixels = 0x0000;
            for (int i = 0; i < 8; i++) {
                word pixel     = ((m_ctx.hi >> (7 - i)) & 0x01);
                pixel        <<= 1;
                pixel         |= (m_ctx.lo >> (7 - i)) & 0x01;
                m_ctx.pixels  |= pixel << ((7 - i) * 2);
            }
            log_debug(
                "Tile Data Processed:\n"
                "\tLow: 0x%.2X\n"
                "\tHigh: 0x%.2X\n"
                "\tPixels: 0x%.4X",
                m_ctx.lo,
                m_ctx.hi,
                m_ctx.pixels
            );
            break;
        case 7:
            // Fetcher is now ready
            // Technically the hardware fetcher would have pushed these pixels last cycle
            // This means it is safe to immediately restart the pipeline when this triggers
            log_debug("Fetcher Ready");
            m_ctx.state = FETCHER_READY;
            break;
        default:
            break;
        }
    }

    return m_ctx.state;
}

word fetcher_get_pixels(void) {
    m_ctx.state = FETCHER_IDLE;
    return m_ctx.pixels;
}
