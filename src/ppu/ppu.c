/**
 * @file ppu.c
 * @brief Implementation of the Pixel Processing Unit (PPU).
 */
#include "ppu/ppu.h"

#include <log.h>
#include <string.h>

#include "DEFINES.h"
#include "display/pixel_t.h"
#include "lcd.h"
#include "memory/hw_registers.h"
#include "memory/memory_map.h"
#include "memory/mmu.h"
#include "ppu/fetcher.h"
#include "ppu/oam_entry.h"
#include "ppu/ppu_mode.h"
#include "utility/fifo.h"

LOG_MODULE_SETUP("PPU", DEFAULT_LOG_LEVEL);

// TODO: Refactor this into a proper design that takes "penalties" into account
#define PPU_SCANLINE_END_DOT      456
#define PPU_VBLANK_START_SCANLINE 144
#define PPU_FRAME_END_SCANLINE    153

/** @brief Packed format for PPU Pixels. */
struct ppu_pixel {
    byte obj_x_pos;
    byte tile_index;
    byte data     : 2; /** @brief Physical pixel data stored in 2bpp format. */
    bool palette  : 1; /** @brief OBJ palette. */
    bool priority : 1; /** @brief BKG-over-OBJ flag. */
};

#define PPU_BKG_FIFO_COUNT       16
#define PPU_BKG_FIFO_BUFFER_SIZE (sizeof(struct ppu_pixel) * PPU_BKG_FIFO_COUNT)
#define PPU_OBJ_FIFO_COUNT       8
#define PPU_OBJ_FIFO_BUFFER_SIZE (sizeof(struct ppu_pixel) * PPU_OBJ_FIFO_COUNT)

static struct {
    int status;
    int index;
    int dot_count;

    GB_FIFO_DECL(bkg_fifo, PPU_BKG_FIFO_BUFFER_SIZE);
    GB_FIFO_DECL(obj_fifo, PPU_OBJ_FIFO_BUFFER_SIZE);

    // --- Registers ---
    struct {
        byte lcdc;
        byte stat;
        byte scy;
        byte scx;
        byte ly;
        byte lyc;
        byte wx;
        byte wy;
    } registers;
    // ---

    struct {
    } hblank;

    struct {
        byte win_line_counter;
    } vblank;

    struct {
        /** @brief Selected objects to display on the current scanline. */
        struct oam_entry selected_objects[10];
        /** @brief Next object to process. */
        struct oam_entry next_object;
        /** @brief Number of selected objects. */
        size_t           selected_object_count;
        /** @brief OAM index of current object being processed. */
        size_t           object_index;
    } oam;

    struct {
        struct oam_entry* next_object;
        size_t            objects_fetched;
        byte              lx;
        byte              fetcher_x;
        bool              win_active;
        bool              stall_obj_fetch;
        bool              stall_fifo_underflow;
    } draw;

} m_ctx = {
    .status    = 0,
    .index     = 0,
    .dot_count = 0,
    .bkg_fifo  = GB_FIFO_INITIALIZER_CTX(m_ctx, bkg_fifo),
    .obj_fifo  = GB_FIFO_INITIALIZER_CTX(m_ctx, obj_fifo),

    .registers = {0},
    // Nothing in hblank currently
    .vblank    = {0},
    .oam       = {0},
    .draw =
        {
            .next_object          = NULL,
            .objects_fetched      = 0,
            .lx                   = 0x00,
            .fetcher_x            = 0x00,
            .win_active           = false,
            .stall_obj_fetch      = false,
            .stall_fifo_underflow = false,
        },
};

colour_t m_pallet[] =
    {{255, 255, 255, 255}, {200, 200, 200, 255}, {100, 100, 100, 255}, {0, 0, 0, 255}};

static void switch_mode(const enum ppu_mode mode);
static void fetch_registers(void);
static void write_registers(void);
static void hblank(void);
static void vblank(void);
static void oam(void);
static void draw(void);

static void set_next_obj(void);
static void obj_rendering(void);
static void bkg_rendering(void);

static bool position_has_obj(void);
static word calc_bkg_tilemap_addr(void);
static word calc_win_tilemap_addr(void);
static word calc_tile_data_addr(byte tile_index, int row, bool is_obj);
static bool object_is_visible(const struct oam_entry* obj);
static bool window_is_active(void);

int ppu_execute(void) {
    fetch_registers();

    // Check that PPU is enabled
    if ((m_ctx.registers.lcdc & REG_LCDC_PPU_ENABLE_MASK) == 0x00) { return m_ctx.status; }

    // Check LY == LYC cond
    if (m_ctx.registers.ly == m_ctx.registers.lyc) {
        // set
        m_ctx.registers.stat |= REG_STAT_LYC_LY_EQ_MASK;
    } else {
        // clear
        m_ctx.registers.stat &= ~REG_STAT_LYC_LY_EQ_MASK;
    }

    // Branch off based on PPU Mode
    enum ppu_mode mode = (enum ppu_mode)(m_ctx.registers.stat & REG_STAT_MODE_MASK);
    switch (mode) {
    case PPU_MODE_HBLANK:
        hblank();
        break;
    case PPU_MODE_VBLANK:
        vblank();
        break;
    case PPU_MODE_OAM:
        oam();
        break;
    case PPU_MODE_DRAW:
        draw();
        break;
    default:
        log_error("Unknown PPU Mode encountered: %i", mode);
        m_ctx.status = -1;
        break;
    }
    m_ctx.dot_count++;

    write_registers();

    return m_ctx.status;
}

int ppu_get_scanline_dot_count(void) { return m_ctx.dot_count; }

static void switch_mode(const enum ppu_mode mode) {
    log_debug(
        "Switching PPU Mode:\n"
        "\tOld Mode: %s\n"
        "\tNew Mode: %s",
        ppu_mode_to_string(m_ctx.registers.stat & REG_STAT_MODE_MASK),
        ppu_mode_to_string(mode)
    );

    switch (mode) {
    case PPU_MODE_HBLANK:
        break;
    case PPU_MODE_VBLANK:
        // New Scanline, reset dot count
        m_ctx.dot_count = 0;
        memset(&m_ctx.vblank, 0, sizeof(m_ctx.vblank));
        break;
    case PPU_MODE_OAM:
        // New Scanline, reset dot count
        m_ctx.dot_count = 0;
        memset(&m_ctx.oam, 0, sizeof(m_ctx.oam));
        m_ctx.draw.fetcher_x = m_ctx.registers.scx / 8;
        break;
    case PPU_MODE_DRAW:
        memset(&m_ctx.draw, 0, sizeof(m_ctx.draw));
        fifo_reset(&m_ctx.bkg_fifo);
        fifo_reset(&m_ctx.obj_fifo);
        fetcher_reset();
        break;
    default:
        log_error("ATTEMPTED TO SWTICH TO UNDEFINED MODE");
        return;
    }

    m_ctx.registers.stat &= ~REG_STAT_MODE_MASK;
    m_ctx.registers.stat |= mode;
}

static void fetch_registers(void) {
    m_ctx.registers.lcdc = mmu_read(BUS_PPU, REG_LCDC);
    m_ctx.registers.stat = mmu_read(BUS_PPU, REG_STAT);
    m_ctx.registers.scy  = mmu_read(BUS_PPU, REG_SCY);
    m_ctx.registers.scx  = mmu_read(BUS_PPU, REG_SCX);
    m_ctx.registers.ly   = mmu_read(BUS_PPU, REG_LY);
    m_ctx.registers.lyc  = mmu_read(BUS_PPU, REG_LYC);
    m_ctx.registers.wx   = mmu_read(BUS_PPU, REG_WX);
    m_ctx.registers.wy   = mmu_read(BUS_PPU, REG_WY);
}

static void write_registers(void) {
    mmu_write(BUS_PPU, REG_LCDC, m_ctx.registers.lcdc);
    mmu_write(BUS_PPU, REG_STAT, m_ctx.registers.stat);
    mmu_write(BUS_PPU, REG_SCY, m_ctx.registers.scy);
    mmu_write(BUS_PPU, REG_SCX, m_ctx.registers.scx);
    mmu_write(BUS_PPU, REG_LY, m_ctx.registers.ly);
    mmu_write(BUS_PPU, REG_LYC, m_ctx.registers.lyc);
    mmu_write(BUS_PPU, REG_WX, m_ctx.registers.wx);
    mmu_write(BUS_PPU, REG_WY, m_ctx.registers.wy);
}

static void hblank(void) {
    // Nothing to do in hblank
    // Wait for Scanline end
    if (m_ctx.dot_count >= PPU_SCANLINE_END_DOT) {
        m_ctx.registers.ly++;
        if (m_ctx.draw.win_active == true) { m_ctx.vblank.win_line_counter++; }

        if (m_ctx.registers.ly >= PPU_VBLANK_START_SCANLINE) {
            switch_mode(PPU_MODE_VBLANK);
        } else {
            switch_mode(PPU_MODE_OAM);
        }
    }
}

static void vblank(void) {
    // Nothing todo in vblank
    // Just increment dot_count and check for scanline end
    m_ctx.dot_count++;
    if (m_ctx.dot_count >= PPU_SCANLINE_END_DOT) {
        m_ctx.dot_count = 0;
        m_ctx.registers.ly++;

        if (m_ctx.registers.ly > PPU_FRAME_END_SCANLINE) {
            m_ctx.registers.ly = 0x00;
            // Signal complete frame
            lcd_frame_complete();
            switch_mode(PPU_MODE_OAM);
        }
    }
}

static void oam(void) {
    // Alternate on cycles
    // - Fetch next object
    // - Process object
    // Repeat until all 40 objects have been processed
    if (m_ctx.dot_count % 2 == 0) {
        // Fetch next object
        const word object_oam_addr = ADDR_OAM_START + 4 * m_ctx.oam.object_index;
        m_ctx.oam.next_object      = (struct oam_entry){
                 .y_pos      = mmu_read(BUS_PPU, object_oam_addr),
                 .x_pos      = mmu_read(BUS_PPU, object_oam_addr + 1),
                 .tile_index = mmu_read(BUS_PPU, object_oam_addr + 2),
                 .attributes = mmu_read(BUS_PPU, object_oam_addr + 3),
        };
        log_debug(
            "OAM Object [%i]:\n"
            "\tx: 0x%.2X\n"
            "\ty: 0x%.2X\n"
            "\tattributes:\n"
            "\t\tFLIPX | FLIPY | PRI\n"
            "\t\t  [%c]  |  [%c]  | [%c]",
            m_ctx.oam.object_index,
            m_ctx.oam.next_object.x_pos,
            m_ctx.oam.next_object.y_pos,
            m_ctx.oam.next_object.attributes & TILE_ATTR_FLIP_X ? 'x' : ' ',
            m_ctx.oam.next_object.attributes & TILE_ATTR_FLIP_Y ? 'x' : ' ',
            m_ctx.oam.next_object.attributes & TILE_ATTR_PRIORITY ? 'x' : ' '
        );
    } else {
        // Only the first 10 valid objects are considered active
        // Discard all others
        if (m_ctx.oam.selected_object_count < 10) {
            // Objects are only active if they are visible
            // This is purely based off of vertical positioning
            if (object_is_visible(&m_ctx.oam.next_object) == true) {
                // Sort array by object order on scanline
                for (int i = 0; i < m_ctx.oam.selected_object_count; i++) {
                    if (m_ctx.oam.next_object.x_pos < m_ctx.oam.selected_objects[i].x_pos) {
                        struct oam_entry tmp          = m_ctx.oam.next_object;
                        m_ctx.oam.next_object         = m_ctx.oam.selected_objects[i];
                        m_ctx.oam.selected_objects[i] = tmp;
                    }
                }
                // Insert whatever was pushed to the end into the next slot
                m_ctx.oam.selected_objects[m_ctx.oam.selected_object_count] = m_ctx.oam.next_object;
                m_ctx.oam.selected_object_count++;
            }
        }
        m_ctx.oam.object_index++;
    }

    // OAM can only hold up to 40 objects
    // OAM Mode ends after all 40 objects have been scanned
    // This is regardless of if we already have 10 active objects
    if (m_ctx.oam.object_index >= 40) {
        switch_mode(PPU_MODE_DRAW);

        log_debug(
            "OAM: Final Object order for LY 0x%.2X\n"
            "\t%i OBJs Selected:\n"
            "\t{%i, %i, %i, %i, %i, %i, %i, %i, %i, %i}",
            m_ctx.registers.ly,
            m_ctx.oam.selected_object_count,
            m_ctx.oam.selected_objects[0],
            m_ctx.oam.selected_objects[1],
            m_ctx.oam.selected_objects[2],
            m_ctx.oam.selected_objects[3],
            m_ctx.oam.selected_objects[4],
            m_ctx.oam.selected_objects[5],
            m_ctx.oam.selected_objects[6],
            m_ctx.oam.selected_objects[7],
            m_ctx.oam.selected_objects[8],
            m_ctx.oam.selected_objects[9]
        );
    }
}

static void draw(void) {

    // TODO: Implement Draw Logic

    // Rendering Pipeline:
    //
    // clang-format off
    //      +-----+                   +---------+               +----------+            +-------+            +-----+
    //      |     | Resolve Tile Data │         | Push 8 Pixels | OBJ FIFO | ---------> |       | Push Pixel |     |
    //      | PPU | --------------->  | Fetcher | ------------> |----------| Mix Pixels | Mixer | ---------> | LCD |
    //      |     |       ADDR        │         |    to FIFO    | BKG FIFO | ---------> |       |   to LCD   |     |
    //      +-----+                   +---------+               +----------+            +-------+            +-----+
    //         │                           |
    //    Get Tilemap                Get Tile Data
    //         │                           |
    //     ┌───┴─────────┐      ┌──────────┘
    //     │             │      |
    //    OBJ         BKG/WIN   |
    //     │             |      |
    //     v             v      v
    //  +-----+          +------+
    //  | OAM |          | VRAM |
    //  +-----+          +------+
    // clang-format on

    // --- Handle Object Rendering ---

    // Ensure we are always tracking the next object
    // This will always set draw.next_object based the count of draw.objects_fetched.
    // If there is no next object (either out of objects or OBJs arent enabled)
    //   - draw.next_object is set to NULL
    set_next_obj();

    // Cache old value as we need to determine if this the first cycle or not
    bool prev_stall_obj_fetch  = m_ctx.draw.stall_obj_fetch;
    m_ctx.draw.stall_obj_fetch = position_has_obj();
    // Are we stalled waiting for an object to be fetched?
    if (m_ctx.draw.stall_obj_fetch == true) {
        log_debug("DRAW STALL: OBJ FETCHING");

        // Is this the first obj rendering cycle, force reset the fetcher if so
        // OBJs always take priority
        if (prev_stall_obj_fetch == false) { fetcher_reset(); }
        // Perform object rendering
        // This will clear the stall flag if it succeeds in pushing OBJ pixels to OBJ FIFO
        // and no additional OBJs need to be rendered.
        obj_rendering();

        // Fetch didn't complete this cycle?
        // Do not carry out any more work
        if (m_ctx.draw.stall_obj_fetch == true) { return; }
    }

    // ---

    //! AT THIS POINT, OBJs CANNOT BE STALLING THE PIPELINE.
    //! ALL OBJs FOR THE CURRENT SCANLINE POSITION HAVE BEEN FETCHED AND PUSHED TO THE OBJ FIFO

    // --- Determine Rendering Mode (BKG/WIN) ---
    // Determine if Window is active
    bool prev_win_active  = m_ctx.draw.win_active;
    m_ctx.draw.win_active = window_is_active();
    // Did we transistion between BKG <-> WIN
    if (prev_win_active != m_ctx.draw.win_active) {
        // LCD Context Shift
        // Reset Fetcher and BKG FIFO
        fetcher_reset();
        fifo_reset(&m_ctx.bkg_fifo);
        m_ctx.draw.fetcher_x = 0; // Reset Tile column count
    }
    // ---

    // --- Produce LCD Pixel ---
    // Check BKG FIFO underflow stall condition
    m_ctx.draw.stall_fifo_underflow =
        (m_ctx.draw.win_active == true)
            ? fifo_is_empty(&m_ctx.bkg_fifo)
            : (fifo_count(&m_ctx.bkg_fifo) / sizeof(struct ppu_pixel)) <= 8;

    if (m_ctx.draw.stall_fifo_underflow == false) {
        struct ppu_pixel bkg_pixel;
        struct ppu_pixel obj_pixel;
        fifo_pop_range(
            &m_ctx.bkg_fifo,
            sizeof(struct ppu_pixel),
            &bkg_pixel,
            sizeof(struct ppu_pixel)
        );
        if (fifo_pop_range(
                &m_ctx.obj_fifo,
                sizeof(struct ppu_pixel),
                &obj_pixel,
                sizeof(struct ppu_pixel)
            ) >= 0) {
            // Perform OBJ-over-BKG mixing
            if (obj_pixel.data == 0x00) {
                // BKG wins
            } else if (obj_pixel.priority == true && bkg_pixel.data != 0x00) {
                // BKG wins
            } else {
                bkg_pixel = obj_pixel;
            }
        }

        // TODO: Implement Palettes correctly

        // Draw Pixel
        lcd_write_pixel(&m_pallet[bkg_pixel.data], m_ctx.draw.lx, m_ctx.registers.ly);
        m_ctx.draw.lx++;
    }
    // ---

    // Always perform BKG Rendering if nothing forces a stop at this point
    bkg_rendering();

    if (m_ctx.draw.lx >= LCD_WIDTH) { switch_mode(PPU_MODE_HBLANK); }
}

static void obj_rendering(void) {
    // Step fetcher and handle result
    switch (fetcher_step()) {
    case FETCHER_IDLE: {
        // If fetcher is IDLE, queue next object and perform first step

        // OBJ.y is offset by 16 when compared to LY
        int  row = m_ctx.registers.ly - (m_ctx.draw.next_object->y_pos - 16);
        word obj_tile_addr;
        if ((m_ctx.registers.lcdc & REG_LCDC_OBJ_SIZE_MASK) == 0U) {
            // 8x8 OBJ
            // Perform flip (if needed) and calculate exact pixel data address
            if ((m_ctx.draw.next_object->attributes & TILE_ATTR_FLIP_Y) != 0U) { row = 7 - row; }
            obj_tile_addr = calc_tile_data_addr(m_ctx.draw.next_object->tile_index, row, true);
        } else {
            // 8x16 OBJ
            // Perform flip (if needed)
            if ((m_ctx.draw.next_object->attributes & TILE_ATTR_FLIP_Y) != 0U) { row = 15 - row; }
            byte tile_index;
            // Correct tile index and row to align with correct OBJ tile
            if (row < 8) {
                tile_index = m_ctx.draw.next_object->tile_index & 0xFE;
            } else {
                tile_index  = m_ctx.draw.next_object->tile_index | 0x01;
                row        -= 8;
            }
            obj_tile_addr = calc_tile_data_addr(tile_index, row, true);
        }
        fetcher_set_fetch_address(obj_tile_addr);
        // Prime first cycle, This is because IDLE is not a real step for the fetcher to be in.
        fetcher_step();
        return;
    }
    case FETCHER_FETCHING:
        // If fetcher is still FETCHING, there is nothing todo
        return;
    case FETCHER_READY: {
        // If fetcher is ready, push pixels to FIFO, this cannot block as pixels are merged with
        // current pixels in FIFO.
        const byte attributes = m_ctx.draw.next_object->attributes;

        // Get fetcher pixels and convert them to PPU Pixels
        word             pixels = fetcher_get_pixels();
        bool             flip_x = (attributes & TILE_ATTR_FLIP_X) != 0U;
        struct ppu_pixel fetcher_pixels[8];
        for (int i = 0; i < 8; i++) {
            // Construct mask based on if X is flipped or not
            // OBJs pixels are ordered L->R as MSB->LSB in pairs of 2 (2bpp)
            size_t shift_cnt  = (flip_x == true) ? i * 2 : (7 - i) * 2;
            word   mask       = 0x0003 << shift_cnt;
            fetcher_pixels[i] = (struct ppu_pixel){
                .obj_x_pos  = m_ctx.draw.next_object->x_pos,
                .tile_index = m_ctx.draw.next_object->tile_index,
                .data       = (pixels & mask) >> shift_cnt,
                .palette    = (attributes & TILE_ATTR_DMG_PALETTE) != 0U,
                .priority   = (attributes & TILE_ATTR_PRIORITY) != 0U,
            };
        }

        // Get Current FIFO Pixels
        size_t           n_pixels_in_fifo = fifo_count(&m_ctx.obj_fifo) / sizeof(struct ppu_pixel);
        struct ppu_pixel fifo_pixels[8];
        fifo_pop_range(&m_ctx.obj_fifo, n_pixels_in_fifo, fifo_pixels, sizeof(fifo_pixels));

        // Perform OBJ-over-OBJ mixing
        //! THIS IS DMG SPECIFIC LOGIC
        for (size_t i = 0; i < 8; i++) {
            if (i < n_pixels_in_fifo) {
                bool fifo_has_priority;

                // Pixel needs to be mixed
                if (fifo_pixels[i].obj_x_pos < fetcher_pixels[i].obj_x_pos) {
                    fifo_has_priority = true;
                } else if (fifo_pixels[i].obj_x_pos > fetcher_pixels[i].obj_x_pos) {
                    fifo_has_priority = false;
                } else {
                    if (fifo_pixels[i].tile_index <= fetcher_pixels[i].tile_index) {
                        fifo_has_priority = true;
                    } else {
                        fifo_has_priority = false;
                    }
                }

                if (fifo_has_priority == true) {
                    // Only replace if fifo pixel is transparent
                    if (fifo_pixels[i].data == 0x00) { fifo_pixels[i] = fetcher_pixels[i]; }
                } else {
                    // Only replace if fetcher pixel is opaque
                    if (fetcher_pixels[i].data != 0x00) { fifo_pixels[i] = fetcher_pixels[i]; }
                }
            } else {
                fifo_pixels[i] = fetcher_pixels[i];
            }
        }

        // Force push final result to OBJ FIFO
        fifo_push_range_overwrite(&m_ctx.obj_fifo, fifo_pixels, sizeof(fifo_pixels));

        // Once OBJ pixels have been pushed:
        // - Increment fetch counter
        // - Update pointer to next object
        // - If object also lies at this scanline position
        //   - true  : Recurse this function to prime the fetcher with it
        //             (worst case, 1-level recursion)
        //   - false : Clear STALL flag
        m_ctx.draw.objects_fetched++;
        set_next_obj();
        if (position_has_obj() == true) {
            obj_rendering();
            return;
        } else {
            m_ctx.draw.stall_obj_fetch = false;
            return;
        }
    }
    default:
        log_error("Impossible Fetcher state returned. Object Rendering failed.");
        return;
    }
}

static void bkg_rendering(void) {
    // Step Fetcher and handle result
    switch (fetcher_step()) {
    case FETCHER_IDLE:
        // Queue next BKG Tile
        int  row;
        word bkg_tilemap_addr;
        if (m_ctx.draw.win_active == false) {
            // BKG Active
            row              = (m_ctx.registers.ly + m_ctx.registers.scy) % 8;
            bkg_tilemap_addr = calc_bkg_tilemap_addr();
        } else {
            // WIN Active
            row              = (m_ctx.registers.ly + m_ctx.registers.scy) % 8;
            bkg_tilemap_addr = calc_win_tilemap_addr();
        }
        byte tile_index         = mmu_read(BUS_PPU, bkg_tilemap_addr);
        word bkg_tile_data_addr = calc_tile_data_addr(tile_index, row, false);
        fetcher_set_fetch_address(bkg_tile_data_addr);
        // Prime Fetcher as IDLE isnt a proper Fetcher state
        fetcher_step();
        // No Pixels to pass to FIFO since the Fetcher was IDLE
        break;
    case FETCHER_FETCHING:
        // No Pixels to pass to FIFO since the Fetcher is still FETCHING
        break;
    case FETCHER_READY: {
        // Check for space in BKG FIFO before obtaining pixels from Fetcher
        size_t remaining_space = (fifo_capacity(&m_ctx.bkg_fifo) - fifo_count(&m_ctx.bkg_fifo)) /
                                 sizeof(struct ppu_pixel);
        // Kick out if there isnt enough space (nothing left todo)
        if (remaining_space < 8) { return; }

        // Convert Pixels into PPU format
        word             pixels = fetcher_get_pixels();
        struct ppu_pixel bkg_pixels[8];
        for (int i = 0; i < 8; i++) {
            size_t shift_cnt = (7 - i) * 2;
            word   mask      = 0x0003 << shift_cnt;
            bkg_pixels[i]    = (struct ppu_pixel){
                   .data = (pixels & mask) >> shift_cnt,
            };
        }

        // Push to FIFO
        fifo_push_range(&m_ctx.bkg_fifo, bkg_pixels, sizeof(bkg_pixels));

        // Since a tile was successfully fetched and pushed
        // Increment fetcher_x
        m_ctx.draw.fetcher_x++;

        // Perform one round of recursion to prime fetching with next BKG tile
        bkg_rendering();
        break;
    }
    default:
        log_error("Impossible Fetcher state returned. Background Rendering failed.");
        return;
    }
}

static void set_next_obj(void) {
    if ((m_ctx.registers.lcdc & REG_LCDC_OBJ_ENABLE_MASK) == 0U) {
        m_ctx.draw.next_object = NULL;
        return;
    }

    if (m_ctx.draw.objects_fetched >= m_ctx.oam.selected_object_count) {
        m_ctx.draw.next_object = NULL;
        return;
    }

    m_ctx.draw.next_object = &m_ctx.oam.selected_objects[m_ctx.draw.objects_fetched];
}

static bool position_has_obj(void) {
    // Position cant have an object if there is no object to render
    if (m_ctx.draw.next_object == NULL) { return false; }

    const byte obj_x = m_ctx.draw.next_object->x_pos;

    // Catch inital offscreen left objs
    if (obj_x < 8 && m_ctx.draw.lx == 0) { return true; }
    // Does the object start at this scanline position?
    return obj_x - 8 == m_ctx.draw.lx;
}

static word calc_bkg_tilemap_addr(void) {
    word base_tilemap_addr = ((m_ctx.registers.lcdc & REG_LCDC_BKG_TILE_MAP_MASK) != 0U)
                                 ? ADDR_VRAM_TILEMAP_1
                                 : ADDR_VRAM_TILEMAP_0;
    word tile_x  = ((m_ctx.registers.scx / 8) + m_ctx.draw.fetcher_x) & 0x001F; // wraps 0-31
    word tile_y  = (m_ctx.registers.ly + m_ctx.registers.scy) & 0x00FF;         // wraps 0-255
    word map_row = tile_y / 8;                                                  // wraps 0-31
    return base_tilemap_addr + (map_row * 32) + tile_x;
}

static word calc_win_tilemap_addr(void) {
    word base_tilemap_addr = ((m_ctx.registers.lcdc & REG_LCDC_WIN_TILE_MAP_MASK) != 0U)
                                 ? ADDR_VRAM_TILEMAP_1
                                 : ADDR_VRAM_TILEMAP_0;
    word win_x = m_ctx.draw.fetcher_x - ((m_ctx.registers.wx - 7) / 8); // Tile column within window
    word win_y = m_ctx.vblank.win_line_counter; // Increments on each rendered window line
    word map_row = win_y / 8;
    return base_tilemap_addr + (map_row * 32) + win_x;
}

static word calc_tile_data_addr(byte tile_index, int row, bool is_obj) {
    // Sanatize row input
    // row must always be [0,7]
    if (row >= 8) {
        row %= 8;
        log_warn("Tile row was greater than 8. Clamped value.");
    }

    if (is_obj == true || ((m_ctx.registers.lcdc & REG_LCDC_TILE_DATA_MASK) != 0U)) {
        // Unsigned Tile Addressing Mode
        // OBJs always use Standard addressing
        const word base_tile_data_addr = ADDR_VRAM_TILEDATA_1;
        word       offset              = (word)tile_index * 16;
        return base_tile_data_addr + offset + row * 2;
    } else {
        // Signed Tile Addressing Mode
        const s_dword base_tile_data_addr = ADDR_VRAM_TILEDATA_0;
        s_byte        signed_tile_index;
        memcpy(&signed_tile_index, &tile_index, sizeof(byte));
        s_dword offset = (s_dword)signed_tile_index * 16;
        // row is too small for unexpected casting to occur
        // result should safely fit in the positive region of s_dword
        // narrowing cast is safe here
        return (word)(base_tile_data_addr + offset + row * 2);
    }
}

static bool object_is_visible(const struct oam_entry* obj) {
    const byte height = (m_ctx.registers.lcdc & REG_LCDC_OBJ_SIZE_MASK != 0U) ? 16 : 8;
    return m_ctx.registers.ly + 16 >= obj->y_pos && m_ctx.registers.ly + 16 < obj->y_pos + height;
}

static bool window_is_active(void) {
    return ((m_ctx.registers.lcdc & REG_LCDC_WIN_ENABLE_MASK) != 0U) &&
           (m_ctx.registers.ly == m_ctx.registers.wy) && (m_ctx.registers.wx <= m_ctx.draw.lx);
}
