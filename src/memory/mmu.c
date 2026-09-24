/**
 * @file mmu.c
 * @brief Implementation file for the Memory Management Unit (MMU). It is responsible for handling
 * all read/write requests on the Address Bus.
 */
#include "memory/mmu.h"

#include <log.h>
#include <stddef.h>

#include "DEFINES.h"
#include "memory/bootROM.h"
#include "memory/hw_registers.h"
#include "memory/memory_map.h"

LOG_MODULE_SETUP("MMU", CONFIG_MMU_MODULE_LOG_LEVEL);

static byte (*m_cart_read)(struct cartridge* cart, const word addr)             = cartridge_read;
void (*m_cart_write)(struct cartridge* cart, const word addr, const byte value) = cartridge_write;

static struct cartridge* m_cart                                  = NULL;
static byte              m_vram[VRAM_BANK_SIZE]                  = {0};
static byte              m_wram[WRAM_BANK_COUNT][WRAM_BANK_SIZE] = {0};
static byte              m_oam[OAM_SIZE]                         = {0};
static byte              m_io_registers[IO_REGISTERS_SIZE]       = {0};
static byte              m_hram[HRAM_SIZE]                       = {0};
static byte              m_reg_ie                                = 0;

// ========== Helper Decls ==========
static byte mmu_rom_read(const enum bus source, const word addr);
static void mmu_rom_write(const enum bus source, const word addr, const byte value);

static byte mmu_vram_read(const enum bus source, const word addr);
static void mmu_vram_write(const enum bus source, const word addr, const byte value);

static byte mmu_cart_ram_read(const enum bus source, const word addr);
static void mmu_cart_ram_write(const enum bus source, const word addr, const byte value);

static byte mmu_wram_read(const enum bus source, const word addr);
static void mmu_wram_write(const enum bus source, const word addr, const byte value);

static byte mmu_echo_ram_read(const enum bus source, const word addr);
static void mmu_echo_ram_write(const enum bus source, const word addr, const byte value);

static byte mmu_oam_read(const enum bus source, const word addr);
static void mmu_oam_write(const enum bus source, const word addr, const byte value);

static byte mmu_prohibited_read(const enum bus source, const word addr);
static void mmu_prohibited_write(const enum bus source, const word addr, const byte value);

static byte mmu_io_read(const enum bus source, const word addr);
static void mmu_io_write(const enum bus source, const word addr, const byte value);

static byte mmu_hram_read(const enum bus source, const word addr);
static void mmu_hram_write(const enum bus source, const word addr, const byte value);

static byte mmu_ie_read(const enum bus source);
static void mmu_ie_write(const enum bus source, const byte value);

static byte mmu_invalid_read(const enum bus source, const word addr);
static void mmu_invalid_write(const enum bus source, const word addr);
// ========== Helper Decls ==========

// ========== API Defs ==========
byte mmu_read(const enum bus source, const word addr) {
    if (addr <= ADDR_CART_ROM_END) {
        return mmu_rom_read(source, addr);
    } else if (addr <= ADDR_VRAM_END) {
        return mmu_vram_read(source, addr);
    } else if (addr <= ADDR_CART_RAM_END) {
        return mmu_cart_ram_read(source, addr);
    } else if (addr <= ADDR_WRAM_END) {
        return mmu_wram_read(source, addr);
    } else if (addr <= ADDR_ECHO_RAM_END) {
        return mmu_echo_ram_read(source, addr);
    } else if (addr <= ADDR_OAM_END) {
        return mmu_oam_read(source, addr);
    } else if (addr <= ADDR_PROHIBITED_END) {
        return mmu_prohibited_read(source, addr);
    } else if (addr <= ADDR_IO_REGISTERS_END) {
        return mmu_io_read(source, addr);
    } else if (addr <= ADDR_HRAM_END) {
        return mmu_hram_read(source, addr);
    } else if (addr == ADDR_IE) {
        return mmu_ie_read(source);
    }

    // Invalid address
    return mmu_invalid_read(source, addr);
}
void mmu_write(const enum bus source, const word addr, const byte value) {
    if (addr <= ADDR_CART_ROM_END) {
        mmu_rom_write(source, addr, value);
        return;
    } else if (addr <= ADDR_VRAM_END) {
        mmu_vram_write(source, addr, value);
        return;
    } else if (addr <= ADDR_CART_RAM_END) {
        mmu_cart_ram_write(source, addr, value);
        return;
    } else if (addr <= ADDR_WRAM_END) {
        mmu_wram_write(source, addr, value);
        return;
    } else if (addr <= ADDR_ECHO_RAM_END) {
        mmu_echo_ram_write(source, addr, value);
        return;
    } else if (addr <= ADDR_OAM_END) {
        mmu_oam_write(source, addr, value);
        return;
    } else if (addr <= ADDR_PROHIBITED_END) {
        mmu_prohibited_write(source, addr, value);
        return;
    } else if (addr <= ADDR_IO_REGISTERS_END) {
        mmu_io_write(source, addr, value);
        return;
    } else if (addr <= ADDR_HRAM_END) {
        mmu_hram_write(source, addr, value);
        return;
    } else if (addr == ADDR_IE) {
        mmu_ie_write(source, value);
        return;
    }

    // Invalid address
    mmu_invalid_write(source, addr);
    return;
}

int mmu_load_cartridge(struct cartridge* cart) {
    if (cart == NULL) {
        log_error("Attempted to load a NULL cartridged!");
        return -1;
    }
    if (m_cart != NULL) {
        log_warn("Attempted to load a cartridge while one was already loaded.");
        return -2;
    }

    m_cart = cart;
    log_debug("Cartridge loaded");
    return 0;
}

struct cartridge* mmu_eject_cartridge(void) {
    struct cartridge* rtn = m_cart;
    m_cart                = NULL;
    log_debug("Cartridge ejected");
    return rtn;
}
// ========== API Defs ==========

// ========== Helper Defs ==========
static byte mmu_rom_read(const enum bus source, const word addr) {
    byte value = 0x00;
    // Boot ROM read?
    if (addr <= ADDR_BOOT_ROM_END && mmu_io_read(source, REG_BANK) == 0) {
        value = m_boot_rom[addr - ADDR_BOOT_ROM_START];
        log_debug(
            "Boot ROM Read:\n\tSource: %s\n\tAddr: 0x%.4X\n\tValue: 0x%.2X",
            bus_to_string(source),
            addr,
            value
        );
        return value;
    }

    // Cartridge ROM read
    value = m_cart_read(m_cart, addr);
    log_debug(
        "Cart ROM Read:\n\tSource: %s\n\tAddr: 0x%.4X\n\tValue: 0x%.2X",
        bus_to_string(source),
        addr,
        value
    );
    return value;
}
static void mmu_rom_write(const enum bus source, const word addr, const byte value) {
    log_warn(
        "%s attempted to write to ROM with address 0x%.4X. Write ignored.",
        bus_to_string(source),
        addr
    );
    return;
}

static inline bool mmu_vram_blocked(const enum bus source) {
    byte ppu_mode = mmu_io_read(source, REG_STAT) & 0b00000011;
    return ppu_mode == 0x03;
}
static byte mmu_vram_read(const enum bus source, const word addr) {
    byte value = 0x00;
    if (source == BUS_CPU && mmu_vram_blocked(source) == true) {
        log_warn("CPU attempted to read from VRAM while PPU has it locked. Returned 0xFF instead.");
        return 0xFF;
    }

    value = m_vram[addr - ADDR_VRAM_START];
    log_debug(
        "VRAM Read:\n\tSource: %s\n\tAddr: 0x%.4X\n\tValue: 0x%.2X",
        bus_to_string(source),
        addr,
        value
    );
    return value;
}
static void mmu_vram_write(const enum bus source, const word addr, const byte value) {
    if (source == BUS_CPU && mmu_vram_blocked(source) == true) {
        log_warn("CPU attempted to write to VRAM while PPU has it locked. Write ignored.");
        return;
    }

    byte old_value                 = m_vram[addr - ADDR_VRAM_START];
    m_vram[addr - ADDR_VRAM_START] = value;
    log_debug(
        "VRAM Write:\n\tSource: %s\n\tAddr: 0x%.4X\n\tValue (before | after): 0x%.2X | 0x%.2X",
        bus_to_string(source),
        addr,
        old_value,
        value
    );
    return;
}

static byte mmu_cart_ram_read(const enum bus source, const word addr) {
    byte value = m_cart_read(m_cart, addr);
    log_debug(
        "Cart RAM Read:\n\tSource: %s\n\tAddr: 0x%.4X\n\tValue: 0x%.2X",
        bus_to_string(source),
        addr,
        value
    );
    return value;
}
static void mmu_cart_ram_write(const enum bus source, const word addr, const byte value) {
    byte old_value = m_cart_read(m_cart, addr);
    m_cart_write(m_cart, addr, value);
    log_debug(
        "Cart RAM Write:\n\tSource: %s\n\tAddr: 0x%.4X\n\tValue (before | after): 0x%.2X | 0x%.2X",
        bus_to_string(source),
        addr,
        old_value,
        value
    );
    return;
}

static byte mmu_wram_read(const enum bus source, const word addr) {
    byte value = 0x00;

    if (addr <= ADDR_WRAM_0_END) {
        // WRAM bank 0
        value = m_wram[0][addr - ADDR_WRAM_0_START];
    } else {
        // WRAM bank N
        byte svbk = mmu_io_read(source, REG_SVBK) & 0b00000111;
        value     = m_wram[svbk + 1][addr - ADDR_WRAM_N_START];
    }

    log_debug(
        "WRAM Read:\n\tSource: %s\n\tAddr: 0x%.4X\n\tValue: 0x%.2X",
        bus_to_string(source),
        addr,
        value
    );
    return value;
}
static void mmu_wram_write(const enum bus source, const word addr, const byte value) {
    byte old_value = 0x00;

    if (addr <= ADDR_WRAM_0_END) {
        // WRAM bank 0
        old_value                           = m_wram[0][addr - ADDR_WRAM_0_START];
        m_wram[0][addr - ADDR_WRAM_0_START] = value;
    } else {
        // WRAM bank N
        byte svbk = mmu_io_read(source, REG_SVBK) & 0b00000111;
        old_value = m_wram[svbk + 1][addr - ADDR_WRAM_N_START];

        m_wram[svbk + 1][addr - ADDR_WRAM_N_START] = value;
    }

    log_debug(
        "WRAM Write:\n\tSource: %s\n\tAddr: 0x%.4X\n\tValue (before | after): 0x%.2X | 0x%.2X",
        bus_to_string(source),
        addr,
        old_value,
        value
    );
    return;
}

static byte mmu_echo_ram_read(const enum bus source, const word addr) {
    if (source == BUS_EXTERN) { return 0xFF; }

    // ECHO RAM address needs to be adjusted for WRAM
    byte value = mmu_wram_read(source, addr - (ADDR_ECHO_RAM_START - ADDR_WRAM_START));
    log_warn(
        "ECHO RAM Read:\n\tSource: %s\n\tAddr: 0x%.4X\n\tValue: 0x%.2X",
        bus_to_string(source),
        addr,
        value
    );
    return value;
}
static void mmu_echo_ram_write(const enum bus source, const word addr, const byte value) {
    // ECHO RAM address needs to be adjusted for WRAM
    word wram_addr = addr - (ADDR_ECHO_RAM_START - ADDR_WRAM_START);
    byte old_value = mmu_wram_read(source, wram_addr);
    mmu_wram_write(source, wram_addr, value);
    log_warn(
        "ECHO RAM Write:\n\tSource: %s\n\tAddr: 0x%.4X\n\tValue (before | after): 0x%.2X | 0x%.2X",
        bus_to_string(source),
        addr,
        old_value,
        value
    );
    return;
}

static inline bool mmu_oam_blocked(const enum bus source) {
    byte ppu_mode = mmu_io_read(source, REG_STAT) & 0b00000011;
    return (ppu_mode == 0x02 || ppu_mode == 0x03);
}
static byte mmu_oam_read(const enum bus source, const word addr) {
    byte value = 0x00;
    if (source == BUS_CPU && mmu_oam_blocked(source) == true) {
        log_warn("CPU attempted to read from OAM while PPU has it locked. Returned 0xFF instead.");
        return 0xFF;
    }

    value = m_oam[addr - ADDR_OAM_START];
    log_debug(
        "OAM Read:\n\tSource: %s\n\tAddr: 0x%.4X\n\tValue: 0x%.2X",
        bus_to_string(source),
        addr,
        value
    );
    return value;
}
static void mmu_oam_write(const enum bus source, const word addr, const byte value) {
    if (source == BUS_CPU && mmu_oam_blocked(source) == true) {
        log_warn("CPU attempted to write to OAM while PPU has it locked. Write ignored.");
        return;
    }

    byte old_value               = m_oam[addr - ADDR_OAM_START];
    m_oam[addr - ADDR_OAM_START] = value;
    log_debug(
        "OAM Write:\n\tSource: %s\n\tAddr: 0x%.4X\n\tValue (before | after): 0x%.2X | 0x%.2X",
        bus_to_string(source),
        addr,
        old_value,
        value
    );
    return;
}

static byte mmu_prohibited_read(const enum bus source, const word addr) {
    if (source == BUS_EXTERN) { return 0xFF; }

    if (source == BUS_CPU && mmu_oam_blocked(source) == true) {
        log_warn(
            "CPU attempted to read at prohibited address 0x%.4X. Returned 0xFF instead.",
            addr
        );
        return 0xFF;
    }

    // TODO: Implement actual OAM corruption bug
    //*Information on the OAM corruption bug can be found here:
    //*https://gbdev.io/pandocs/OAM_Corruption_Bug.html
    log_error(
        "%s attempted to read from prohibited address 0x%.4X. Possible OAM Corruption. Returned "
        "0xFF.",
        bus_to_string(source),
        addr
    );
    return 0xFF;
}
static void mmu_prohibited_write(const enum bus source, const word addr, const byte value) {
    if (source == BUS_CPU && mmu_oam_blocked(source) == true) {
        log_warn("CPU attempted to write to prohibited address 0x%.4X. Write ignored.", addr);
        return;
    }

    // TODO: Implement actual OAM corruption bug
    //*Information on the OAM corruption bug can be found here:
    //*https://gbdev.io/pandocs/OAM_Corruption_Bug.html
    log_error(
        "%s attempted to write to prohibited address 0x%.4X. Possible OAM Corruption. Write "
        "ignored.",
        bus_to_string(source),
        addr
    );
    return;
}

static byte mmu_io_read(const enum bus source, const word addr) {
    byte value = m_io_registers[addr - ADDR_IO_REGISTERS_START];
    log_debug(
        "Hardware Register Read:\n\tSource: %s\n\tAddr: 0x%.4X\n\tRegister Name: %s\n\tValue: "
        "0x%.2X",
        bus_to_string(source),
        addr,
        hw_reg_to_string(addr),
        value
    );
    return value;
}
static void mmu_io_write(const enum bus source, const word addr, const byte value) {
    byte old_value                                 = m_io_registers[addr - ADDR_IO_REGISTERS_START];
    m_io_registers[addr - ADDR_IO_REGISTERS_START] = value;
    log_debug(
        "Hardware Register Write:\n\tSource: %s\n\tAddr: 0x%.4X\n\tRegister Name: %s\n\tValue "
        "(before | after): 0x%.2X | 0x%.2X",
        bus_to_string(source),
        addr,
        hw_reg_to_string(addr),
        old_value,
        value
    );
    return;
}

static byte mmu_hram_read(const enum bus source, const word addr) {
    byte value = m_hram[addr - ADDR_HRAM_START];
    log_debug(
        "HRAM Read:\n\tSource: %s\n\tAddr: 0x%.4X\n\tValue: 0x%.2X",
        bus_to_string(source),
        addr,
        value
    );
    return value;
}
static void mmu_hram_write(const enum bus source, const word addr, const byte value) {
    byte old_value                 = m_hram[addr - ADDR_HRAM_START];
    m_hram[addr - ADDR_HRAM_START] = value;
    log_debug(
        "HRAM Write:\n\tSource: %s\n\tAddr: 0x%.4X\n\tValue (before | after): 0x%.2X | 0x%.2X",
        bus_to_string(source),
        addr,
        old_value,
        value
    );
    return;
}

static byte mmu_ie_read(const enum bus source) {
    log_debug("IE Register Read:\n\tSource: %s\n\tValue: 0x%.2X", bus_to_string(source), m_reg_ie);
    return m_reg_ie;
}
static void mmu_ie_write(const enum bus source, const byte value) {
    byte old_value = m_reg_ie;
    m_reg_ie       = value;
    log_debug(
        "IE Register Write:\n\tSource: %s\n\tValue (before | after): 0x%.2X | 0x%.2X",
        bus_to_string(source),
        old_value,
        value
    );
    return;
}

static byte mmu_invalid_read(const enum bus source, const word addr) {
    if (source == BUS_EXTERN) { return 0xFF; }

    log_warn(
        "%s attempted to read at invalid address 0x%.4X. Returned 0xFF instead.",
        bus_to_string(source),
        addr
    );
    return 0xFF;
}
static void mmu_invalid_write(const enum bus source, const word addr) {
    log_warn(
        "%s attempted to write at invalid address 0x%.4X. Write ignored.",
        bus_to_string(source),
        addr
    );
}
// ========== Helper Defs ==========

// ========== Testing API Defs ==========
#ifdef TESTING
void mmu_testing_set_cartridge_hooks(
    byte (*cart_read)(struct cartridge* cart, const word addr),
    void (*cart_write)(struct cartridge* cart, const word addr, const byte value)
) {
    m_cart_read  = (cart_read == NULL) ? cartridge_read : cart_read;
    m_cart_write = (cart_write == NULL) ? cartridge_write : cart_write;
    return;
}
struct cartridge* mmu_testing_get_cart(void) { return m_cart; }
byte*             mmu_testing_get_vram(void) { return m_vram; }
byte*             mmu_testing_get_wram(const size_t bank) { return m_wram[bank]; }
byte*             mmu_testing_get_oam(void) { return m_oam; }
byte*             mmu_testing_get_io_registers(void) { return m_io_registers; }
byte*             mmu_testing_get_hram(void) { return m_hram; }
byte*             mmu_testing_get_reg_ie(void) { return &m_reg_ie; }
#endif
// ========== Testing API Defs ==========
