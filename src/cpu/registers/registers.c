/**
 * @file registers.c
 * @brief Compilation Unit for the CPU registers.
 */
#include "cpu/registers/registers.h"

#include <stddef.h>
#include <string.h>

static const byte ZFLAG_BITMASK = 0b10000000;
static const byte NFLAG_BITMASK = 0b01000000;
static const byte HFLAG_BITMASK = 0b00100000;
static const byte CFLAG_BITMASK = 0b00010000;

static struct registers m_regs = {0};

// Main 16-bit Registers
// =====================

void registers_set_AF(const word value) {
    m_regs.AF = value & 0xFFF0;
}
void registers_set_BC(const word value) {
    m_regs.BC = value;
}
void registers_set_DE(const word value) {
    m_regs.DE = value;
}
void registers_set_HL(const word value) {
    m_regs.HL = value;
}
void registers_set_SP(const word value) {
    m_regs.SP = value;
}
void registers_set_PC(const word value) {
    m_regs.PC = value;
}

word registers_get_AF(void) {
    return m_regs.AF;
}
word registers_get_BC(void) {
    return m_regs.BC;
}
word registers_get_DE(void) {
    return m_regs.DE;
}
word registers_get_HL(void) {
    return m_regs.HL;
}
word registers_get_SP(void) {
    return m_regs.SP;
}
word registers_get_PC(void) {
    return m_regs.PC;
}

// =====================
// Main 16-bit Registers


// Virtual 8-bit Registers
// =======================

void registers_set_A(const byte value) {
    m_regs.A = value;
}
void registers_set_F(const byte value) {
    m_regs.F = value & 0xF0;
}
void registers_set_B(const byte value) {
    m_regs.B = value;
}
void registers_set_C(const byte value) {
    m_regs.C = value;
}
void registers_set_D(const byte value) {
    m_regs.D = value;
}
void registers_set_E(const byte value) {
    m_regs.E = value;
}
void registers_set_H(const byte value) {
    m_regs.H = value;
}
void registers_set_L(const byte value) {
    m_regs.L = value;
}

byte registers_get_A(void) {
    return m_regs.A;
}
byte registers_get_F(void) {
    return m_regs.F;
}
byte registers_get_B(void) {
    return m_regs.B;
}
byte registers_get_C(void) {
    return m_regs.C;
}
byte registers_get_D(void) {
    return m_regs.D;
}
byte registers_get_E(void) {
    return m_regs.E;
}
byte registers_get_H(void) {
    return m_regs.H;
}
byte registers_get_L(void) {
    return m_regs.L;
}

// =======================
// Virtual 8-bit Registers


// Flags
// =====

static inline void generic_set_bit(const byte bitmask) {
    m_regs.F |= bitmask;
}
void registers_set_flag_z(void) {
    generic_set_bit(ZFLAG_BITMASK);
}
void registers_set_flag_n(void) {
    generic_set_bit(NFLAG_BITMASK);
}
void registers_set_flag_h(void) {
    generic_set_bit(HFLAG_BITMASK);
}
void registers_set_flag_c(void) {
    generic_set_bit(CFLAG_BITMASK);
}

static inline void generic_clear_bit(const byte bitmask) {
    m_regs.F &= ~(bitmask | 0b00001111);
}
void registers_clear_flag_z(void) {
    generic_clear_bit(ZFLAG_BITMASK);
}
void registers_clear_flag_n(void) {
    generic_clear_bit(NFLAG_BITMASK);
}
void registers_clear_flag_h(void) {
    generic_clear_bit(HFLAG_BITMASK);
}
void registers_clear_flag_c(void) {
    generic_clear_bit(CFLAG_BITMASK);
}

static inline bool generic_get_bit(const byte bitmask) {
    return ((m_regs.F & bitmask) != 0);
}
bool registers_get_flag_z(void) {
    return generic_get_bit(ZFLAG_BITMASK);
}
bool registers_get_flag_n(void) {
    return generic_get_bit(NFLAG_BITMASK);
}
bool registers_get_flag_h(void) {
    return generic_get_bit(HFLAG_BITMASK);
}
bool registers_get_flag_c(void) {
    return generic_get_bit(CFLAG_BITMASK);
}

// =====
// Flags


// Test Functions
// ==============

void registers_reset(void) {
    memset(&m_regs, 0x00, sizeof(struct registers));
}

int registers_snapshot(struct registers *regs) {
    if (regs == NULL) { return -1; }
    
    // Create exact bit perfect copy
    memcpy(regs, &m_regs, sizeof(struct registers));
    return 0;
}

// ==============
// Test Functions