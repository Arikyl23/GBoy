/**
 * @file alu.c
 * @brief File containing the core logic for the Arithmetic Logic Unit (ALU)
 * module.
 */
#include "alu.h"

// 8-bit Operations
// ================
// Arithmetic Operations
// ---------------------

struct alu_result alu_inc_8b(const byte value) {
    struct alu_result rtn = {0};
    rtn.result.byte       = value + 1;
    rtn.flags.z           = (rtn.result.byte == 0);
    rtn.flags.n           = false;
    rtn.flags.h           = (value & 0x0F) == 0x0F;
    return rtn;
}

struct alu_result alu_dec_8b(const byte value) {
    struct alu_result rtn = {0};
    rtn.result.byte       = value - 1;
    rtn.flags.z           = (rtn.result.byte == 0);
    rtn.flags.n           = true;
    rtn.flags.h           = (value & 0x0F) == 0x00;
    return rtn;
}

struct alu_result alu_add_8b(const byte lhs, const byte rhs) {
    struct alu_result rtn = {0};
    rtn.result.byte       = lhs + rhs;
    rtn.flags.z           = (rtn.result.byte == 0);
    rtn.flags.n           = false;
    rtn.flags.h           = ((lhs & 0x0F) + (rhs & 0x0F)) > 0x0F;
    rtn.flags.c           = (((word)lhs + (word)rhs) > 0xFF);
    return rtn;
}

struct alu_result alu_sub_8b(const byte lhs, const byte rhs) {
    struct alu_result rtn = {0};
    rtn.result.byte       = lhs - rhs;
    rtn.flags.z           = (rtn.result.byte == 0);
    rtn.flags.n           = true;
    rtn.flags.h           = (lhs & 0x0F) < (rhs & 0x0F);
    rtn.flags.c           = (lhs < rhs);
    return rtn;
}

struct alu_result alu_cp_8b(const byte lhs, const byte rhs) { return alu_sub_8b(lhs, rhs); }

struct alu_result alu_adc_8b(const byte lhs, const byte rhs, const bool carry) {
    struct alu_result rtn = {0};
    rtn.result.byte       = lhs + rhs + ((carry == true) ? 1 : 0);
    rtn.flags.z           = (rtn.result.byte == 0);
    rtn.flags.n           = false;
    rtn.flags.h           = ((lhs & 0x0F) + (rhs & 0x0F) + ((carry == true) ? 1 : 0)) > 0x0F;
    rtn.flags.c           = (((word)lhs + (word)rhs + ((carry == true) ? 1 : 0)) > 0xFF);
    return rtn;
}

struct alu_result alu_sbc_8b(const byte lhs, const byte rhs, const bool carry) {
    struct alu_result rtn = {0};
    rtn.result.byte       = lhs - rhs - ((carry == true) ? 1 : 0);
    rtn.flags.z           = (rtn.result.byte == 0);
    rtn.flags.n           = true;
    rtn.flags.h           = (lhs & 0x0F) < ((rhs & 0x0F) + ((carry == true) ? 1 : 0));
    rtn.flags.c           = ((word)(lhs) < ((word)(rhs) + (word)((carry == true) ? 1 : 0)));
    return rtn;
}

// ---------------------
// Arithmetic Operations

// Logical Operations
// ------------------

struct alu_result alu_and_8b(const byte lhs, const byte rhs) {
    struct alu_result rtn = {0};
    rtn.result.byte       = lhs & rhs;
    rtn.flags.z           = (rtn.result.byte == 0);
    rtn.flags.n           = false;
    rtn.flags.h           = true;
    rtn.flags.c           = false;
    return rtn;
}

struct alu_result alu_xor_8b(const byte lhs, const byte rhs) {
    struct alu_result rtn = {0};
    rtn.result.byte       = lhs ^ rhs;
    rtn.flags.z           = (rtn.result.byte == 0);
    rtn.flags.n           = false;
    rtn.flags.h           = false;
    rtn.flags.c           = false;
    return rtn;
}

struct alu_result alu_or_8b(const byte lhs, const byte rhs) {
    struct alu_result rtn = {0};
    rtn.result.byte       = lhs | rhs;
    rtn.flags.z           = (rtn.result.byte == 0);
    rtn.flags.n           = false;
    rtn.flags.h           = false;
    rtn.flags.c           = false;
    return rtn;
}

struct alu_result alu_cpl_8b(const byte value) {
    struct alu_result rtn = {0};
    rtn.result.byte       = ~value;
    rtn.flags.n           = true;
    rtn.flags.h           = true;
    return rtn;
}

// ------------------
// Logical Operations

// Bit Manipulation
// ----------------

struct alu_result alu_rlc_8b(const byte value) {
    struct alu_result rtn = {0};
    rtn.flags.c           = ((value & 0x80) != 0);
    rtn.result.byte       = ((value << 1) | ((rtn.flags.c == true) ? 1 : 0));
    rtn.flags.z           = (rtn.result.byte == 0);
    rtn.flags.n           = false;
    rtn.flags.h           = false;
    return rtn;
}

struct alu_result alu_rrc_8b(const byte value) {
    struct alu_result rtn = {0};
    rtn.flags.c           = ((value & 0x01) != 0);
    rtn.result.byte       = (value >> 1) | ((rtn.flags.c == true) ? 0x80 : 0);
    rtn.flags.z           = (rtn.result.byte == 0);
    rtn.flags.n           = false;
    rtn.flags.h           = false;
    return rtn;
}

struct alu_result alu_rl_8b(const byte value, const bool carry) {
    struct alu_result rtn = {0};
    rtn.flags.c           = ((value & 0x80) != 0);
    rtn.result.byte       = ((value << 1) | ((carry == true) ? 1 : 0));
    rtn.flags.z           = (rtn.result.byte == 0);
    rtn.flags.n           = false;
    rtn.flags.h           = false;
    return rtn;
}

struct alu_result alu_rr_8b(const byte value, const bool carry) {
    struct alu_result rtn = {0};
    rtn.flags.c           = ((value & 0x01) != 0);
    rtn.result.byte       = (value >> 1) | ((carry == true) ? 0x80 : 0);
    rtn.flags.z           = (rtn.result.byte == 0);
    rtn.flags.n           = false;
    rtn.flags.h           = false;
    return rtn;
}

struct alu_result alu_sla_8b(const byte value) {
    struct alu_result rtn = {0};
    rtn.flags.c           = ((value & 0x80) != 0);
    rtn.result.byte       = (value << 1);
    rtn.flags.z           = (rtn.result.byte == 0);
    rtn.flags.n           = false;
    rtn.flags.h           = false;
    return rtn;
}

struct alu_result alu_sra_8b(const byte value) {
    struct alu_result rtn = {0};
    rtn.flags.c           = ((value & 0x01) != 0);
    rtn.result.byte       = (value & 0x80) | (value >> 1);
    rtn.flags.z           = (rtn.result.byte == 0);
    rtn.flags.n           = false;
    rtn.flags.h           = false;
    return rtn;
}

struct alu_result alu_swap_8b(const byte value) {
    struct alu_result rtn = {0};
    rtn.result.byte       = (value >> 4) | (value << 4);
    rtn.flags.z           = (rtn.result.byte == 0);
    rtn.flags.n           = false;
    rtn.flags.h           = false;
    rtn.flags.c           = false;
    return rtn;
}

struct alu_result alu_srl_8b(const byte value) {
    struct alu_result rtn = {0};
    rtn.flags.c           = ((value & 0x01) != 0);
    rtn.result.byte       = (value >> 1);
    rtn.flags.z           = (rtn.result.byte == 0);
    rtn.flags.n           = false;
    rtn.flags.h           = false;
    return rtn;
}

struct alu_result alu_bit_8b(const size_t bit, const byte value) {
    struct alu_result rtn = {0};
    rtn.flags.z           = ((value & (1 << bit)) == 0);
    rtn.flags.n           = false;
    rtn.flags.h           = true;
    return rtn;
}

struct alu_result alu_res_8b(const size_t bit, const byte value) {
    struct alu_result rtn = {0};
    rtn.result.byte       = (value & ~(1 << bit));
    return rtn;
}

struct alu_result alu_set_8b(const size_t bit, const byte value) {
    struct alu_result rtn = {0};
    rtn.result.byte       = (value | (1 << bit));
    return rtn;
}

// ----------------
// Bit Manipulation
// ================
// 8-bit Operations

// 16-bit Operations
// =================

struct alu_result alu_inc_16b(const word value) {
    struct alu_result rtn = {0};
    rtn.result.word       = value + 1;
    return rtn;
}

struct alu_result alu_dec_16b(const word value) {
    struct alu_result rtn = {0};
    rtn.result.word       = value - 1;
    return rtn;
}

struct alu_result alu_add_16b(const word lhs, const word rhs) {
    struct alu_result rtn_low  = alu_add_8b(lhs, rhs);
    struct alu_result rtn_high = alu_adc_8b(lhs >> 8, rhs >> 8, rtn_low.flags.c);
    struct alu_result rtn;
    rtn.result.word = (rtn_high.result.byte << 8) | (rtn_low.result.byte);
    rtn.flags       = rtn_high.flags;
    rtn.flags.z     = false;
    return rtn;
}

// =================
// 16-bit Operations

// Miscellaneous Operations
// ========================

struct alu_result alu_add_16b_e8(const word lhs, const byte rhs) {
    struct alu_result rtn         = {0};
    s_dword           wide_result = (s_dword)lhs + (s_dword)(s_byte)(rhs);
    rtn.result.word               = (word)wide_result;
    rtn.flags.z                   = false;
    rtn.flags.n                   = false;
    rtn.flags.h                   = ((lhs & 0x000F) + (rhs & 0x0F)) > 0x0F;
    rtn.flags.c                   = ((lhs & 0x00FF) + ((word)rhs & 0x00FF)) > 0x00FF;
    return rtn;
}

struct alu_result alu_da(
    const byte bcd_value,
    const bool n_flag,
    const bool h_flag,
    const bool c_flag
) {
    struct alu_result rtn = {0};
    rtn.flags.c           = c_flag;
    byte adjustment       = 0;
    if (n_flag == true) {
        adjustment      += ((h_flag == true) ? 0x06 : 0x00);
        adjustment      += ((c_flag == true) ? 0x60 : 0x00);
        rtn.result.byte  = bcd_value - adjustment;
    } else {
        adjustment      += ((h_flag == true || bcd_value & 0x0F > 0x09) ? 0x06 : 0x00);
        adjustment      += ((c_flag == true || bcd_value & 0xFF > 0x99) ? 0x60 : 0x00);
        rtn.result.byte  = bcd_value + adjustment;
        rtn.flags.c      = ((((word)bcd_value + (word)adjustment) > 0x00FF) ? true : c_flag);
    }
    rtn.flags.z = (rtn.result.byte == 0);
    rtn.flags.h = false;
    return rtn;
}

// ========================
// Miscellaneous Operations
