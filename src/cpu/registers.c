/**
 * @file registers.c
 * @brief Compilation Unit for the CPU registers.
 */
#include "cpu/registers.h"

#include <stddef.h>

static const byte ZFLAG_BITMASK = 0b10000000;
static const byte NFLAG_BITMASK = 0b01000000;
static const byte HFLAG_BITMASK = 0b00100000;
static const byte CFLAG_BITMASK = 0b00010000;

void registers_set_AF(struct registers* reg, const word value) { reg->AF = value & 0xFFF0; }
void registers_set_F(struct registers* reg, const byte value) { reg->F = value & 0xF0; }

void registers_set_flags(
    struct registers* reg,
    const bool        z,
    const bool        n,
    const bool        h,
    const bool        c
) {
    reg->F = (((byte)z) << 7) + (((byte)n) << 6) + (((byte)h) << 5) + (((int)c) << 4);
}

static inline bool generic_get_bit(struct registers* reg, const byte bitmask) {
    return ((reg->F & bitmask) != 0);
}
bool registers_get_flag_z(struct registers* reg) { return generic_get_bit(reg, ZFLAG_BITMASK); }
bool registers_get_flag_n(struct registers* reg) { return generic_get_bit(reg, NFLAG_BITMASK); }
bool registers_get_flag_h(struct registers* reg) { return generic_get_bit(reg, HFLAG_BITMASK); }
bool registers_get_flag_c(struct registers* reg) { return generic_get_bit(reg, CFLAG_BITMASK); }
