/**
 * @file mmu.c
 * @brief Implementation file for the Memory Management Unit (MMU). It is responsible for handling
 * all read/write requests on the Address Bus.
 */
#include "memory/mmu.h"

#include "DEFINES.h"
#include "memory/memory_map.h"

// TODO: Add cartridge type
static byte m_vram[VRAM_BANK_SIZE]                  = {0};
static byte m_wram[WRAM_BANK_COUNT][WRAM_BANK_SIZE] = {0};
static byte m_oam[OAM_SIZE]                         = {0};
static byte m_io_registers[IO_REGISTERS_SIZE]       = {0};
static byte m_hram[HRAM_SIZE]                       = {0};
static byte m_reg_IE                                = 0;

byte mmu_read(const enum bus source_bus, const word addr) { return 0xFF; }
void mmu_write(const enum bus source_bus, const word addr, const byte value) { }
