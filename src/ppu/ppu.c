/**
 * @file ppu.c
 * @brief Implementation of the Pixel Processing Unit (PPU).
 */
#include "ppu/ppu.h"

#include <log.h>

#include "DEFINES.h"
#include "display/pixel_t.h"
#include "lcd.h"
#include "memory/hw_registers.h"
#include "memory/mmu.h"
#include "ppu/ppu_mode.h"

LOG_MODULE_SETUP("PPU", LOG_DEBUG);

// TODO: Refactor this into a proper design that takes "penalties" into account
#define PPU_OAM_END               80
#define PPU_DRAW_END              (PPU_OAM_END + 160)
#define PPU_SCANLINE_END          456
#define PPU_VBLANK_START_SCANLINE 144
#define PPU_FRAME_END_SCANLINE    153

static struct {
    int status;
    int index;

    // --- Registers ---

    byte lcdc_reg;
    byte stat_reg;
    byte scy_reg;
    byte scx_reg;
    byte ly_reg;
    byte lyc_reg;

    // ---

    int dot_count;
} m_ctx = {0};

colour_t m_pallet[] = {
    {0, 0, 0, 255},
    {255, 0, 0, 255},
    {0, 255, 0, 255},
    {0, 0, 255, 255},
    {255, 0, 255, 255},
    {0, 255, 255, 255},
    {255, 255, 255, 255}
};

static void ppu_fetch_registers(void);
static void ppu_write_registers(void);
static void ppu_hblank(void);
static void ppu_vblank(void);
static void ppu_oam(void);
static void ppu_draw(void);

int ppu_execute(void) {
    ppu_fetch_registers();

    // Check that PPU is enabled
    if ((m_ctx.lcdc_reg & REG_LCDC_PPU_ENABLE_MASK) == 0x00) { return m_ctx.status; }

    // Branch off based on PPU Mode
    enum ppu_mode mode = (enum ppu_mode)(m_ctx.stat_reg & REG_STAT_MODE_MASK);
    switch (mode) {
    case PPU_MODE_HBLANK:
        ppu_hblank();
        break;
    case PPU_MODE_VBLANK:
        ppu_vblank();
        break;
    case PPU_MODE_OAM:
        ppu_oam();
        break;
    case PPU_MODE_DRAW:
        ppu_draw();
        break;
    default:
        log_error("Unknown PPU Mode encountered: %i", mode);
        m_ctx.status = -1;
        break;
    }

    ppu_write_registers();

    return m_ctx.status;
}

int ppu_get_scanline_dot_count(void) { return m_ctx.dot_count; }

static void ppu_fetch_registers(void) {
    m_ctx.lcdc_reg = mmu_read(BUS_PPU, REG_LCDC);
    m_ctx.stat_reg = mmu_read(BUS_PPU, REG_STAT);
    m_ctx.scy_reg  = mmu_read(BUS_PPU, REG_SCY);
    m_ctx.scx_reg  = mmu_read(BUS_PPU, REG_SCX);
    m_ctx.ly_reg   = mmu_read(BUS_PPU, REG_LY);
    m_ctx.lyc_reg  = mmu_read(BUS_PPU, REG_LYC);
}

static void ppu_write_registers(void) {
    mmu_write(BUS_PPU, REG_LCDC, m_ctx.lcdc_reg);
    mmu_write(BUS_PPU, REG_STAT, m_ctx.stat_reg);
    mmu_write(BUS_PPU, REG_SCY, m_ctx.scy_reg);
    mmu_write(BUS_PPU, REG_SCX, m_ctx.scx_reg);
    mmu_write(BUS_PPU, REG_LY, m_ctx.ly_reg);
    mmu_write(BUS_PPU, REG_LYC, m_ctx.lyc_reg);
}

static void ppu_hblank(void) {
    // Nothing to do in hblank
    // Just increment dot_count and check for scanline end
    m_ctx.dot_count++;
    if (m_ctx.dot_count >= PPU_SCANLINE_END) {
        m_ctx.dot_count = 0;
        m_ctx.ly_reg++;

        m_ctx.stat_reg &= ~REG_STAT_MODE_MASK;
        m_ctx.stat_reg |= (m_ctx.ly_reg >= PPU_VBLANK_START_SCANLINE) ? (byte)PPU_MODE_VBLANK
                                                                      : (byte)PPU_MODE_OAM;
    }
}

static void ppu_vblank(void) {
    // Nothing todo in vblank
    // Just increment dot_count and check for scanline end
    m_ctx.dot_count++;
    if (m_ctx.dot_count >= PPU_SCANLINE_END) {
        m_ctx.dot_count = 0;
        m_ctx.ly_reg++;

        if (m_ctx.ly_reg > PPU_FRAME_END_SCANLINE) {
            m_ctx.stat_reg &= ~REG_STAT_MODE_MASK;
            m_ctx.stat_reg |= (byte)PPU_MODE_OAM;

            m_ctx.ly_reg = 0;
            m_ctx.index  = (m_ctx.index + 1) % (sizeof(m_pallet) / sizeof(colour_t));

            // Signal complete frame
            lcd_frame_complete();
        }
    }
}

static void ppu_oam(void) {
    // TODO: Implement OAM Scan Logic

    // Increment lx and check for OAM end
    m_ctx.dot_count++;
    if (m_ctx.dot_count >= PPU_OAM_END) {
        m_ctx.stat_reg &= ~REG_STAT_MODE_MASK;
        m_ctx.stat_reg |= (byte)PPU_MODE_DRAW;
    }
}

static void ppu_draw(void) {
    static byte lx = 0;

    // TODO: Implement Draw Logic

    lcd_write_pixel(&m_pallet[m_ctx.index], lx, m_ctx.ly_reg);

    // Increment lx and dot_count. Check for HBlank start
    lx++;
    m_ctx.dot_count++;
    if (m_ctx.dot_count >= PPU_DRAW_END) {
        m_ctx.stat_reg &= ~REG_STAT_MODE_MASK;
        m_ctx.stat_reg |= (byte)PPU_MODE_HBLANK;
        lx              = 0;
    }
}
