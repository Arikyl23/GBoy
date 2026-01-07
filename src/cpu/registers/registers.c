/**
 * @file registers.c
 * @brief Compilation Unit for the CPU registers.
 */
#include <cpu/registers/registers.h>

static const word ZFLAG_BITMASK = 0x0080;
static const word NFLAG_BITMASK = 0x0040;
static const word HFLAG_BITMASK = 0x0020;
static const word CFLAG_BITMASK = 0x0010;

static word AF;
static word BC;
static word DE;
static word HL;
static word SP;
static word PC;

// Helpers
// =======

static inline void set_high_byte(word* reg, const byte high_byte) {
    *reg = (((word)high_byte) << 8) | (*reg & 0x00FF);
}
static inline void set_low_byte(word* reg, const byte low_byte) {
    *reg = ((word)low_byte) | (*reg & 0xFF00);
}

// =======
// Helpers

// Main 16-bit Registers
// =====================

void registers_set_AF(const word value) {
    AF = value & 0xFFF0;
}
void registers_set_BC(const word value) {
    BC = value;
}
void registers_set_DE(const word value) {
    DE = value;
}
void registers_set_HL(const word value) {
    HL = value;
}
void registers_set_SP(const word value) {
    SP = value;
}
void registers_set_PC(const word value) {
    PC = value;
}

word registers_get_AF(void) {
    return AF;
}
word registers_get_BC(void) {
    return BC;
}
word registers_get_DE(void) {
    return DE;
}
word registers_get_HL(void) {
    return HL;
}
word registers_get_SP(void) {
    return SP;
}
word registers_get_PC(void) {
    return PC;
}

// =====================
// Main 16-bit Registers


// Virtual 8-bit Registers
// =======================

void registers_set_A(const byte value) {
    set_high_byte(&AF, value);
    AF &= 0xFFF0;
}
void registers_set_F(const byte value) {
    set_low_byte(&AF, value);
    AF &= 0xFFF0;
}
void registers_set_B(const byte value) {
    set_high_byte(&BC, value);
}
void registers_set_C(const byte value) {
    set_low_byte(&BC, value);
}
void registers_set_D(const byte value) {
    set_high_byte(&DE, value);
}
void registers_set_E(const byte value) {
    set_low_byte(&DE, value);
}
void registers_set_H(const byte value) {
    set_high_byte(&HL, value);
}
void registers_set_L(const byte value) {
    set_low_byte(&HL, value);
}

byte registers_get_A(void) {
    return (byte)(AF >> 8);
}
byte registers_get_F(void) {
    return (byte)(AF);
}
byte registers_get_B(void) {
    return (byte)(BC >> 8);
}
byte registers_get_C(void) {
    return (byte)(BC);
}
byte registers_get_D(void) {
    return (byte)(DE >> 8);
}
byte registers_get_E(void) {
    return (byte)(DE);
}
byte registers_get_H(void) {
    return (byte)(HL >> 8);
}
byte registers_get_L(void) {
    return (byte)(HL);
}

// =======================
// Virtual 8-bit Registers


// Flags
// =====

void registers_assign_flag_z(bool state) {
    (state == true) ? registers_set_flag_z() : registers_clear_flag_z();
}
void registers_assign_flag_n(bool state) {
    (state == true) ? registers_set_flag_n() : registers_clear_flag_n();
}
void registers_assign_flag_h(bool state) {
    (state == true) ? registers_set_flag_h() : registers_clear_flag_h();
}
void registers_assign_flag_c(bool state) {
    (state == true) ? registers_set_flag_c() : registers_clear_flag_c();
}

void registers_set_flag_z(void) {
    registers_set_AF(registers_get_AF() | ZFLAG_BITMASK);
}
void registers_set_flag_n(void) {
    registers_set_AF(registers_get_AF() | NFLAG_BITMASK);
}
void registers_set_flag_h(void) {
    registers_set_AF(registers_get_AF() | HFLAG_BITMASK);
}
void registers_set_flag_c(void) {
    registers_set_AF(registers_get_AF() | CFLAG_BITMASK);
}

void registers_clear_flag_z(void) {
    registers_set_AF(registers_get_AF() & ~ZFLAG_BITMASK);
}
void registers_clear_flag_n(void) {
    registers_set_AF(registers_get_AF() & ~NFLAG_BITMASK);
}
void registers_clear_flag_h(void) {
    registers_set_AF(registers_get_AF() & ~HFLAG_BITMASK);
}
void registers_clear_flag_c(void) {
    registers_set_AF(registers_get_AF() & ~CFLAG_BITMASK);
}

bool registers_get_flag_z(void) {
    return (registers_get_AF() & ZFLAG_BITMASK) == ZFLAG_BITMASK;
}
bool registers_get_flag_n(void) {
    return (registers_get_AF() & NFLAG_BITMASK) == NFLAG_BITMASK;
}
bool registers_get_flag_h(void) {
    return (registers_get_AF() & HFLAG_BITMASK) == HFLAG_BITMASK;
}
bool registers_get_flag_c(void) {
    return (registers_get_AF() & CFLAG_BITMASK) == CFLAG_BITMASK;
}

// =====
// Flags