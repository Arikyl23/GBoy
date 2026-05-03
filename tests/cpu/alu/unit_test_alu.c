/**
 * @file test_alu.c
 * @brief File containing tests to verify the functionality of the Arithmetic
 * Logic Unit (ALU) module.
 */
#include <stdbool.h>

#include "DEFINES.h"
#include "cpu/alu/alu.h"
#include "testing.h"

LOG_MODULE_SETUP_DEFAULT("UNIT TEST ALU");

static bool test_alu_module(void);

static bool test_8bit_operations(void);
static bool test_16bit_operations(void);
static bool test_misc_operations(void);

static bool test_8bit_arith(void);
static bool test_8bit_arith_inc(void);
static bool test_8bit_arith_dec(void);
static bool test_8bit_arith_add(void);
static bool test_8bit_arith_sub(void);
static bool test_8bit_arith_cp(void);
static bool test_8bit_arith_adc(void);
static bool test_8bit_arith_sbc(void);

static struct alu_result long_inc_8b(const byte value);
static struct alu_result long_dec_8b(const byte value);
static struct alu_result long_add_8b(const byte lhs, const byte rhs);
static struct alu_result long_sub_8b(const byte lhs, const byte rhs);
static struct alu_result long_cp_8b(const byte lhs, const byte rhs);
static struct alu_result long_adc_8b(const byte lhs, const byte rhs, const bool carry);
static struct alu_result long_sbc_8b(const byte lhs, const byte rhs, const bool carry);

static bool test_8bit_logic(void);
static bool test_8bit_logic_and(void);
static bool test_8bit_logic_xor(void);
static bool test_8bit_logic_or(void);
static bool test_8bit_logic_cpl(void);

static struct alu_result long_and_8b(const byte lhs, const byte rhs);
static struct alu_result long_xor_8b(const byte lhs, const byte rhs);
static struct alu_result long_or_8b(const byte lhs, const byte rhs);
static struct alu_result long_cpl_8b(const byte value);

static bool test_8bit_manip(void);
static bool test_8bit_bit_manip_rlc(void);
static bool test_8bit_bit_manip_rrc(void);
static bool test_8bit_bit_manip_rl(void);
static bool test_8bit_bit_manip_rr(void);
static bool test_8bit_bit_manip_sla(void);
static bool test_8bit_bit_manip_sra(void);
static bool test_8bit_bit_manip_swap(void);
static bool test_8bit_bit_manip_srl(void);
static bool test_8bit_bit_manip_bit(void);
static bool test_8bit_bit_manip_res(void);
static bool test_8bit_bit_manip_set(void);

static struct alu_result long_rlc_8b(const byte value);
static struct alu_result long_rrc_8b(const byte value);
static struct alu_result long_rl_8b(const byte value, const bool carry);
static struct alu_result long_rr_8b(const byte value, const bool carry);
static struct alu_result long_sla_8b(const byte value);
static struct alu_result long_sra_8b(const byte value);
static struct alu_result long_swap_8b(const byte value);
static struct alu_result long_srl_8b(const byte value);
static struct alu_result long_bit_8b(const size_t bit, const byte value);
static struct alu_result long_res_8b(const size_t bit, const byte value);
static struct alu_result long_set_8b(const size_t bit, const byte value);

static bool test_16bit_inc(void);
static bool test_16bit_dec(void);
static bool test_16bit_add(void);

static struct alu_result long_inc_16b(const word value);
static struct alu_result long_dec_16b(const word value);
static struct alu_result long_add_16b(const word lhs, const word rhs);

static bool test_misc_add_16b_e8(void);
static bool test_misc_da(void);

static struct alu_result long_add_16b_e8(const word sp, const byte e8);
static struct alu_result long_da_8b(const byte a, const bool n, const bool h, const bool c);

static bool cmp_results_byte(const struct alu_result returned, const struct alu_result expected);
static bool cmp_results_word(const struct alu_result returned, const struct alu_result expected);
static bool cmp_results_flags(const struct alu_result returned, const struct alu_result expected);

static bool fuzz_8bit_r8(
    struct alu_result (*arith_func)(const byte value),
    struct alu_result (*long_arith_func)(const byte value)
);
static bool fuzz_8bit_r8_c(
    struct alu_result (*arith_func)(const byte value, const bool carry),
    struct alu_result (*long_arith_func)(const byte value, const bool carry)
);
static bool fuzz_16bit_r16_n_h_c(
    struct alu_result (*arith_func)(const byte value, const bool n, const bool h, const bool c),
    struct alu_result (*long_arith_func)(const byte value, const bool n, const bool h, const bool c)
);
static bool fuzz_8bit_zu_r8(
    struct alu_result (*arith_func)(const size_t bit, const byte value),
    struct alu_result (*long_arith_func)(const size_t bit, const byte value)
);
static bool fuzz_8bit_r8_r8(
    struct alu_result (*arith_func)(const byte lhs, const byte rhs),
    struct alu_result (*long_arith_func)(const byte lhs, const byte rhs)
);
static bool fuzz_8bit_r8_r8_c(
    struct alu_result (*arith_func)(const byte lhs, const byte rhs, const bool carry),
    struct alu_result (*long_arith_func)(const byte lhs, const byte rhs, const bool carry)
);
static bool fuzz_16bit_r16(
    struct alu_result (*arith_func)(const word value),
    struct alu_result (*long_arith_func)(const word value)
);
static bool fuzz_16bit_r16_r8(
    struct alu_result (*arith_func)(const word lhs, const byte rhs),
    struct alu_result (*long_arith_func)(const word lhs, const byte rhs)
);
static bool fuzz_16bit_r16_r16(
    struct alu_result (*arith_func)(const word lhs, const word rhs),
    struct alu_result (*long_arith_func)(const word lhs, const word rhs)
);
static bool fuzz_16bit_r16_r16_fast(
    struct alu_result (*arith_func)(const word lhs, const word rhs),
    struct alu_result (*long_arith_func)(const word lhs, const word rhs)
);

int main(void) {
    // 0 indicates a success, 1 a failure
    return (test_alu_module() == true) ? 0 : 1;
}

static bool test_alu_module(void) {
    ASSERT_TRUE(test_8bit_operations(), "8-bit Operations Test Failed!");
    ASSERT_TRUE(test_16bit_operations(), "16-bit Operations Test Failed!");
    // ASSERT_TRUE(test_misc_operations(), "Miscellaneous Operations Test Failed!");
    return true;
}

// 8-bit Operations
// ================

static bool test_8bit_operations(void) {
    ASSERT_TRUE(test_8bit_arith(), "Test 8-bit Arithmetic Operations Failed!");
    ASSERT_TRUE(test_8bit_logic(), "Test 8-bit Logical Operations Failed!");
    ASSERT_TRUE(test_8bit_manip(), "Test 8-bit Bit Manipulation Operations Failed!");
    return true;
}

// Arithmetic Operations
// ---------------------

static bool test_8bit_arith(void) {
    ASSERT_TRUE(test_8bit_arith_inc(), "Test 8-bit INC Failed");
    ASSERT_TRUE(test_8bit_arith_dec(), "Test 8-bit DEC Failed");
    ASSERT_TRUE(test_8bit_arith_add(), "TEST 8-bit ADD Failed");
    ASSERT_TRUE(test_8bit_arith_sub(), "TEST 8-bit SUB Failed");
    ASSERT_TRUE(test_8bit_arith_cp(), "TEST 8-bit CP Failed");
    ASSERT_TRUE(test_8bit_arith_adc(), "TEST 8-bit ADC Failed");
    ASSERT_TRUE(test_8bit_arith_sbc(), "TEST 8-bit SBC Failed");
    return true;
}

static bool test_8bit_arith_inc(void) { return fuzz_8bit_r8(alu_inc_8b, long_inc_8b); }
static bool test_8bit_arith_dec(void) { return fuzz_8bit_r8(alu_dec_8b, long_dec_8b); }
static bool test_8bit_arith_add(void) { return fuzz_8bit_r8_r8(alu_add_8b, long_add_8b); }
static bool test_8bit_arith_sub(void) { return fuzz_8bit_r8_r8(alu_sub_8b, long_sub_8b); }
static bool test_8bit_arith_cp(void) { return fuzz_8bit_r8_r8(alu_cp_8b, long_cp_8b); }
static bool test_8bit_arith_adc(void) { return fuzz_8bit_r8_r8_c(alu_adc_8b, long_adc_8b); }
static bool test_8bit_arith_sbc(void) { return fuzz_8bit_r8_r8_c(alu_sbc_8b, long_sbc_8b); }

static struct alu_result long_inc_8b(const byte value) {
    struct alu_result out = {0};
    // Result wraps naturally in 8-bit
    out.result.byte       = value + 1;

    out.flags.z = (out.result.byte == 0);
    out.flags.n = false; // Always 0 for INC
    // Half-carry: bit 3 carried into bit 4
    // Only happens if low nibble was 0xF (1111)
    out.flags.h = ((value & 0x0F) + 1) > 0x0F;
    // Carry (C) is unaffected by INC
    return out;
}
static struct alu_result long_dec_8b(const byte value) {
    struct alu_result out = {0};
    out.result.byte       = value - 1;

    out.flags.z = (out.result.byte == 0);
    out.flags.n = true; // Always 1 for DEC
    // Half-carry (borrow): bit 4 borrowed for bit 3
    // Only happens if low nibble was 0x0 (0000)
    out.flags.h = ((int)(value & 0x0F) - 1) < 0;
    // Carry (C) is unaffected by DEC
    return out;
}
static struct alu_result long_add_8b(const byte lhs, const byte rhs) {
    struct alu_result out = {0};
    int32_t           res = (int32_t)lhs + (int32_t)rhs;

    out.result.byte = (byte)(res & 0xFF);
    out.flags.z     = (out.result.byte == 0);
    out.flags.n     = false;
    out.flags.h     = ((lhs & 0x0F) + (rhs & 0x0F)) > 0x0F;
    out.flags.c     = (res > 0xFF);
    return out;
}
static struct alu_result long_sub_8b(const byte lhs, const byte rhs) {
    struct alu_result out = {0};
    int32_t           res = (int32_t)lhs - (int32_t)rhs;

    out.result.byte = (byte)(res & 0xFF);
    out.flags.z     = (out.result.byte == 0);
    out.flags.n     = true;
    out.flags.h     = ((int32_t)(lhs & 0x0F) - (int32_t)(rhs & 0x0F)) < 0;
    out.flags.c     = (res < 0);
    return out;
}
static struct alu_result long_cp_8b(const byte lhs, const byte rhs) {
    return long_sub_8b(lhs, rhs);
}
struct alu_result long_adc_8b(const byte lhs, const byte rhs, const bool carry) {
    struct alu_result out = {0};
    int32_t           c   = carry ? 1 : 0;
    int32_t           res = (int32_t)lhs + (int32_t)rhs + c;

    out.result.byte = (byte)(res & 0xFF);
    out.flags.z     = (out.result.byte == 0);
    out.flags.n     = false;
    out.flags.h     = ((lhs & 0x0F) + (rhs & 0x0F) + c) > 0x0F;
    out.flags.c     = (res > 0xFF);
    return out;
}
struct alu_result long_sbc_8b(const byte lhs, const byte rhs, const bool carry) {
    struct alu_result out = {0};
    int32_t           c   = carry ? 1 : 0;
    int32_t           res = (int32_t)lhs - (int32_t)rhs - c;

    out.result.byte = (byte)(res & 0xFF);
    out.flags.z     = (out.result.byte == 0);
    out.flags.n     = true;
    out.flags.h     = ((int32_t)(lhs & 0x0F) - (int32_t)(rhs & 0x0F) - c) < 0;
    out.flags.c     = (res < 0);
    return out;
}

// ---------------------
// Arithmetic Operations

// Logical Operations
// ------------------

static bool test_8bit_logic(void) {
    ASSERT_TRUE(test_8bit_logic_and(), "Test 8-bit AND Failed");
    ASSERT_TRUE(test_8bit_logic_xor(), "Test 8-bit XOR Failed");
    ASSERT_TRUE(test_8bit_logic_or(), "Test 8-bit OR Failed");
    ASSERT_TRUE(test_8bit_logic_cpl(), "Test 8-bit CPL Failed");
    return true;
}

static bool test_8bit_logic_and(void) { return fuzz_8bit_r8_r8(alu_and_8b, long_and_8b); }
static bool test_8bit_logic_xor(void) { return fuzz_8bit_r8_r8(alu_xor_8b, long_xor_8b); }
static bool test_8bit_logic_or(void) { return fuzz_8bit_r8_r8(alu_or_8b, long_or_8b); }
static bool test_8bit_logic_cpl(void) { return fuzz_8bit_r8(alu_cpl_8b, long_cpl_8b); }

static struct alu_result long_and_8b(const byte lhs, const byte rhs) {
    struct alu_result out = {0};
    out.result.byte       = lhs & rhs;

    out.flags.z = (out.result.byte == 0);
    out.flags.n = false;
    out.flags.h = true; // AND always sets H to 1 on GameBoy
    out.flags.c = false;
    return out;
}

static struct alu_result long_xor_8b(const byte lhs, const byte rhs) {
    struct alu_result out = {0};
    out.result.byte       = lhs ^ rhs;

    out.flags.z = (out.result.byte == 0);
    out.flags.n = false;
    out.flags.h = false;
    out.flags.c = false;
    return out;
}

static struct alu_result long_or_8b(const byte lhs, const byte rhs) {
    struct alu_result out = {0};
    out.result.byte       = lhs | rhs;

    out.flags.z = (out.result.byte == 0);
    out.flags.n = false;
    out.flags.h = false;
    out.flags.c = false;
    return out;
}

static struct alu_result long_cpl_8b(const byte value) {
    struct alu_result out = {0};
    out.result.byte       = ~value;

    // CPL (Complement) is unique:
    // It does NOT affect the Z or C flags.
    // In your isolated system, Z and C should be 0/false
    // so they don't trigger a mismatch during comparison.
    out.flags.z = false;
    out.flags.n = true; // Always 1
    out.flags.h = true; // Always 1
    out.flags.c = false;
    return out;
}

// ------------------
// Logical Operations

// Bit Manipulation
// ----------------

static bool test_8bit_manip(void) {
    ASSERT_TRUE(test_8bit_bit_manip_rlc(), "Test 8-bit RLC Failed");
    ASSERT_TRUE(test_8bit_bit_manip_rrc(), "Test 8-bit RRC Failed");
    ASSERT_TRUE(test_8bit_bit_manip_rl(), "Test 8-bit RL Failed");
    ASSERT_TRUE(test_8bit_bit_manip_rr(), "Test 8-bit RR Failed");
    ASSERT_TRUE(test_8bit_bit_manip_sla(), "Test 8-bit SLA Failed");
    ASSERT_TRUE(test_8bit_bit_manip_sra(), "Test 8-bit SRA Failed");
    ASSERT_TRUE(test_8bit_bit_manip_swap(), "Test 8-bit SWAP Failed");
    ASSERT_TRUE(test_8bit_bit_manip_srl(), "Test 8-bit SRL Failed");
    ASSERT_TRUE(test_8bit_bit_manip_bit(), "Test 8-bit BIT Failed");
    ASSERT_TRUE(test_8bit_bit_manip_res(), "Test 8-bit RES Failed");
    ASSERT_TRUE(test_8bit_bit_manip_set(), "Test 8-bit SET Failed");
    return true;
}
static bool test_8bit_bit_manip_rlc(void) { return fuzz_8bit_r8(alu_rlc_8b, long_rlc_8b); }
static bool test_8bit_bit_manip_rrc(void) { return fuzz_8bit_r8(alu_rrc_8b, long_rrc_8b); }
static bool test_8bit_bit_manip_rl(void) { return fuzz_8bit_r8_c(alu_rl_8b, long_rl_8b); }
static bool test_8bit_bit_manip_rr(void) { return fuzz_8bit_r8_c(alu_rr_8b, long_rr_8b); }
static bool test_8bit_bit_manip_sla(void) { return fuzz_8bit_r8(alu_sla_8b, long_sla_8b); }
static bool test_8bit_bit_manip_sra(void) { return fuzz_8bit_r8(alu_sra_8b, long_sra_8b); }
static bool test_8bit_bit_manip_swap(void) { return fuzz_8bit_r8(alu_swap_8b, long_swap_8b); }
static bool test_8bit_bit_manip_srl(void) { return fuzz_8bit_r8(alu_srl_8b, long_srl_8b); }
static bool test_8bit_bit_manip_bit(void) { return fuzz_8bit_zu_r8(alu_bit_8b, long_bit_8b); }
static bool test_8bit_bit_manip_res(void) { return fuzz_8bit_zu_r8(alu_res_8b, long_res_8b); }
static bool test_8bit_bit_manip_set(void) { return fuzz_8bit_zu_r8(alu_set_8b, long_set_8b); }

static struct alu_result long_rlc_8b(const byte value) {
    struct alu_result out = {0};
    // RLC: Rotate Left Circular. Bit 7 moves to C and bit 0.
    out.flags.c           = (value & 0x80) != 0;
    out.result.byte       = (value << 1) | (out.flags.c ? 0x01 : 0x00);

    out.flags.z = (out.result.byte == 0);
    out.flags.n = false;
    out.flags.h = false;
    return out;
}

static struct alu_result long_rrc_8b(const byte value) {
    struct alu_result out = {0};
    // RRC: Rotate Right Circular. Bit 0 moves to C and bit 7.
    out.flags.c           = (value & 0x01) != 0;
    out.result.byte       = (value >> 1) | (out.flags.c ? 0x80 : 0x00);

    out.flags.z = (out.result.byte == 0);
    out.flags.n = false;
    out.flags.h = false;
    return out;
}

static struct alu_result long_rl_8b(const byte value, const bool carry) {
    struct alu_result out = {0};
    // RL: Rotate Left through Carry. Bit 7 goes to C, old C goes to bit 0.
    out.flags.c           = (value & 0x80) != 0;
    out.result.byte       = (value << 1) | (carry ? 0x01 : 0x00);

    out.flags.z = (out.result.byte == 0);
    out.flags.n = false;
    out.flags.h = false;
    return out;
}

static struct alu_result long_rr_8b(const byte value, const bool carry) {
    struct alu_result out = {0};
    // RR: Rotate Right through Carry. Bit 0 goes to C, old C goes to bit 7.
    out.flags.c           = (value & 0x01) != 0;
    out.result.byte       = (value >> 1) | (carry ? 0x80 : 0x00);

    out.flags.z = (out.result.byte == 0);
    out.flags.n = false;
    out.flags.h = false;
    return out;
}

static struct alu_result long_sla_8b(const byte value) {
    struct alu_result out = {0};
    // SLA: Shift Left Arithmetic. Bit 7 goes to C, bit 0 is 0.
    out.flags.c           = (value & 0x80) != 0;
    out.result.byte       = value << 1;

    out.flags.z = (out.result.byte == 0);
    out.flags.n = false;
    out.flags.h = false;
    return out;
}

static struct alu_result long_sra_8b(const byte value) {
    struct alu_result out = {0};
    // SRA: Shift Right Arithmetic. Bit 0 goes to C, Bit 7 (sign bit) is preserved.
    out.flags.c           = (value & 0x01) != 0;
    // Casting to a signed type ensures sign extension during the right shift in C
    out.result.byte       = ((int8_t)value >> 1) | (value & 0x80);

    out.flags.z = (out.result.byte == 0);
    out.flags.n = false;
    out.flags.h = false;
    return out;
}

static struct alu_result long_swap_8b(const byte value) {
    struct alu_result out = {0};
    // SWAP: Swaps upper and lower nibbles.
    out.result.byte       = ((value & 0xF0) >> 4) | ((value & 0x0F) << 4);

    out.flags.z = (out.result.byte == 0);
    out.flags.n = false;
    out.flags.h = false;
    out.flags.c = false;
    return out;
}

static struct alu_result long_srl_8b(const byte value) {
    struct alu_result out = {0};
    // SRL: Shift Right Logical. Bit 0 goes to C, bit 7 is 0.
    out.flags.c           = (value & 0x01) != 0;
    out.result.byte       = value >> 1; // Unsigned shift naturally puts 0 in bit 7

    out.flags.z = (out.result.byte == 0);
    out.flags.n = false;
    out.flags.h = false;
    return out;
}

static struct alu_result long_bit_8b(const size_t bit, const byte value) {
    struct alu_result out = {0};
    // BIT: Tests if the specified bit is set. Result stored only in flags.

    // No Result is returned
    // We preserve the 'don't care' state by not setting it
    // It should be defaulted to 0

    // Z flag is set if the tested bit is 0
    out.flags.z = !((value >> bit) & 0x01);
    out.flags.n = false;
    out.flags.h = true; // BIT always sets H to 1

    // C flag is unaffected by the BIT instruction.
    // Assuming 'out.flags.c' starts at 'false' (0),
    // it preserves the 'don't care' state for your comparator.
    return out;
}

static struct alu_result long_res_8b(const size_t bit, const byte value) {
    struct alu_result out = {0};
    // RES: Resets (clears) the specified bit.
    out.result.byte       = value & ~(1 << bit);

    // RES clears N and H, leaves Z and C untouched (relative to input state)
    // Your framework expects 0 for Z/C if they are untouched in the ALU logic
    out.flags.z = false;
    out.flags.n = false;
    out.flags.h = false;
    out.flags.c = false;
    return out;
}

static struct alu_result long_set_8b(const size_t bit, const byte value) {
    struct alu_result out = {0};
    // SET: Sets the specified bit.
    out.result.byte       = value | (1 << bit);

    // SET clears N and H, leaves Z and C untouched (relative to input state)
    out.flags.z = false;
    out.flags.n = false;
    out.flags.h = false;
    out.flags.c = false;
    return out;
}

// ----------------
// Bit Manipulation
// ================
// 8-bit Operations

// 16-bit Operations
// =================

static bool test_16bit_operations(void) {
    ASSERT_TRUE(test_16bit_inc(), "Test 16-bit INC Failed!");
    ASSERT_TRUE(test_16bit_dec(), "Test 16-bit DEC Failed!");
    ASSERT_TRUE(test_16bit_add(), "Test 16-bit ADD Failed!");
    return true;
}

static bool test_16bit_inc(void) { return fuzz_16bit_r16(alu_inc_16b, long_inc_16b); }
static bool test_16bit_dec(void) { return fuzz_16bit_r16(alu_dec_16b, long_dec_16b); }
static bool test_16bit_add(void) { return fuzz_16bit_r16_r16_fast(alu_add_16b, long_add_16b); }

static struct alu_result long_inc_16b(const word value) {
    struct alu_result out = {0};
    out.result.word       = value + 1;

    // INC 16-bit instructions (e.g., INC BC)
    // do not affect any flags on the GameBoy.
    out.flags.z = false;
    out.flags.n = false;
    out.flags.h = false;
    out.flags.c = false;
    return out;
}

static struct alu_result long_dec_16b(const word value) {
    struct alu_result out = {0};
    out.result.word       = value - 1;

    // DEC 16-bit instructions (e.g., DEC BC)
    // do not affect any flags on the GameBoy.
    out.flags.z = false;
    out.flags.n = false;
    out.flags.h = false;
    out.flags.c = false;
    return out;
}

static struct alu_result long_add_16b(const word lhs, const word rhs) {
    struct alu_result out = {0};
    // Use 32-bit int to catch the 16-bit carry
    uint32_t          res = (uint32_t)lhs + (uint32_t)rhs;

    out.result.word = (word)(res & 0xFFFF);

    // ADD HL, RR Flag rules:
    // Z is not affected (CPU preserves existing Z)
    out.flags.z = false;
    out.flags.n = false; // Always cleared

    // H: Half-carry from bit 11 to bit 12
    out.flags.h = ((lhs & 0x0FFF) + (rhs & 0x0FFF)) > 0x0FFF;

    // C: Carry from bit 15 to bit 16
    out.flags.c = (res > 0xFFFF);

    return out;
}

// =================
// 16-bit Operations

// Miscellaneous Operations
// ========================

static bool test_misc_operations(void) {
    ASSERT_TRUE(test_misc_add_16b_e8(), "Test Miscellaneous ADD Failed!");
    ASSERT_TRUE(test_misc_da(), "Test Miscellaneous DA Failed!");
    return true;
}

static bool test_misc_add_16b_e8(void) {
    return fuzz_16bit_r16_r8(alu_add_16b_e8, long_add_16b_e8);
}
static bool test_misc_da(void) { return fuzz_16bit_r16_n_h_c(alu_da, long_da_8b); }

static struct alu_result long_add_16b_e8(const word sp, const byte e8) {
    struct alu_result out = {0};

    // The result is 16-bit
    out.result.word = sp + (int8_t)e8;

    out.flags.z = false; // Z is always cleared
    out.flags.n = false; // N is always cleared

    // IMPORTANT: Flags are based on the 8-bit addition of SP_low and e8
    // H: Carry from bit 3 to 4
    out.flags.h = ((sp & 0x0F) + (e8 & 0x0F)) > 0x0F;
    // C: Carry from bit 7 to 8
    out.flags.c = ((sp & 0xFF) + (e8 & 0xFF)) > 0xFF;

    return out;
}

static struct alu_result long_da_8b(const byte a, const bool n, const bool h, const bool c) {
    struct alu_result out   = {0};
    byte              reg_a = a;
    bool              carry = c;

    if (!n) { // After an ADD operation
        if (c || reg_a > 0x99) {
            reg_a += 0x60;
            carry  = true;
        }
        if (h || (reg_a & 0x0F) > 0x09) { reg_a += 0x06; }
    } else { // After a SUB operation
        if (c) { reg_a -= 0x60; }
        if (h) { reg_a -= 0x06; }
    }

    out.result.byte = reg_a;
    out.flags.z     = (reg_a == 0);
    out.flags.h     = false; // H is always cleared
    out.flags.c     = carry; // C updated based on adjustment
    out.flags.n     = n;     // N is preserved
    return out;
}

// ========================
// Miscellaneous Operations

// Helper Fxns
// ===========

static bool cmp_results_byte(const struct alu_result returned, const struct alu_result expected) {
    ASSERT_EQ(
        returned.result.byte,
        expected.result.byte,
        "Bad Result. Value should have been 0x%.2X but was 0x%.2X.",
        expected.result.byte,
        returned.result.byte
    );
    return cmp_results_flags(returned, expected);
}

static bool cmp_results_word(const struct alu_result returned, const struct alu_result expected) {
    ASSERT_EQ(
        returned.result.word,
        expected.result.word,
        "Bad Result. Value should have been 0x%.4X but was 0x%.4X.",
        expected.result.word,
        returned.result.word
    );
    return cmp_results_flags(returned, expected);
}

static bool cmp_results_flags(const struct alu_result returned, const struct alu_result expected) {
    ASSERT_EQ(
        returned.flags.z,
        expected.flags.z,
        "Bad Result. Z flag should have been [%c] but was [%c].",
        expected.flags.z ? 'x' : ' ',
        returned.flags.z ? 'x' : ' '
    );
    ASSERT_EQ(
        returned.flags.n,
        expected.flags.n,
        "Bad Result. N flag should have been [%c] but was [%c].",
        expected.flags.n ? 'x' : ' ',
        returned.flags.n ? 'x' : ' '
    );
    ASSERT_EQ(
        returned.flags.h,
        expected.flags.h,
        "Bad Result. H flag should have been [%c] but was [%c].",
        expected.flags.h ? 'x' : ' ',
        returned.flags.h ? 'x' : ' '
    );
    ASSERT_EQ(
        returned.flags.c,
        expected.flags.c,
        "Bad Result. C flag should have been [%c] but was [%c].",
        expected.flags.c ? 'x' : ' ',
        returned.flags.c ? 'x' : ' '
    );
    return true;
}

static bool fuzz_8bit_r8(
    struct alu_result (*arith_func)(const byte value),
    struct alu_result (*long_arith_func)(const byte value)
) {
    struct alu_result returned_result;
    struct alu_result expected_result;

    for (word test_value = 0x00; test_value <= 0xFF; test_value++) {
        returned_result = arith_func(test_value);
        expected_result = long_arith_func(test_value);
        ASSERT_TRUE(
            cmp_results_byte(returned_result, expected_result),
            "Result Mismatch. Input: Test value [0x%.2X]",
            test_value
        );
    }

    return true;
}
static bool fuzz_8bit_r8_c(
    struct alu_result (*arith_func)(const byte value, const bool carry),
    struct alu_result (*long_arith_func)(const byte value, const bool carry)
) {
    struct alu_result returned_result;
    struct alu_result expected_result;

    for (word test_value = 0x00; test_value <= 0xFF; test_value++) {
        for (int c = 0; c < 2; c++) {
            returned_result = arith_func(test_value, c);
            expected_result = long_arith_func(test_value, c);
            ASSERT_TRUE(
                cmp_results_byte(returned_result, expected_result),
                "Result Mismatch. Input: Value [0x%.2X], C flag [%c]",
                test_value,
                (c == 1) ? 'x' : ' '
            );
        }
    }

    return true;
}
static bool fuzz_16bit_r16_n_h_c(
    struct alu_result (*arith_func)(const byte value, const bool n, const bool h, const bool c),
    struct alu_result (*long_arith_func)(const byte value, const bool n, const bool h, const bool c)
) {
    struct alu_result returned_result;
    struct alu_result expected_result;

    for (word test_value = 0x00; test_value <= 0xFF; test_value++) {
        for (int n = 0; n < 2; n++) {
            for (int h = 0; h < 2; h++) {
                for (int c = 0; c < 2; c++) {
                    returned_result = arith_func(test_value, n, h, c);
                    expected_result = long_arith_func(test_value, n, h, c);
                    ASSERT_TRUE(
                        cmp_results_byte(returned_result, expected_result),
                        "Result Mismatch. Input: value [0x%.2X], N flag [%c], H flag [%c], C flag "
                        "[%c]",
                        test_value,
                        (n == 1) ? 'x' : ' ',
                        (h == 1) ? 'x' : ' ',
                        (c == 1) ? 'x' : ' '
                    );
                }
            }
        }
    }

    return true;
}
static bool fuzz_8bit_zu_r8(
    struct alu_result (*arith_func)(const size_t bit, const byte value),
    struct alu_result (*long_arith_func)(const size_t bit, const byte value)
) {
    struct alu_result returned_result;
    struct alu_result expected_result;

    for (word test_value = 0x00; test_value <= 0xFF; test_value++) {
        for (size_t test_bit = 0; test_bit < 8; test_bit++) {
            returned_result = arith_func(test_bit, test_value);
            expected_result = long_arith_func(test_bit, test_value);
            ASSERT_TRUE(
                cmp_results_byte(returned_result, expected_result),
                "Result Mismatch. Input: Bit [%zu], Value [0x%.2X]",
                test_bit,
                test_value
            );
        }
    }

    return true;
}
static bool fuzz_8bit_r8_r8(
    struct alu_result (*arith_func)(const byte lhs, const byte rhs),
    struct alu_result (*long_arith_func)(const byte lhs, const byte rhs)
) {
    struct alu_result returned_result;
    struct alu_result expected_result;

    for (word test_value_a = 0x00; test_value_a <= 0xFF; test_value_a++) {
        for (word test_value_b = 0x00; test_value_b <= 0xFF; test_value_b++) {
            returned_result = arith_func(test_value_a, test_value_b);
            expected_result = long_arith_func(test_value_a, test_value_b);
            ASSERT_TRUE(
                cmp_results_byte(returned_result, expected_result),
                "Result Mismatch. Input: lhs [0x%.2X], rhs [0x%.2X]",
                test_value_a,
                test_value_b
            );
        }
    }

    return true;
}
static bool fuzz_8bit_r8_r8_c(
    struct alu_result (*arith_func)(const byte lhs, const byte rhs, const bool carry),
    struct alu_result (*long_arith_func)(const byte lhs, const byte rhs, const bool carry)
) {
    struct alu_result returned_result;
    struct alu_result expected_result;

    for (word test_value_a = 0x00; test_value_a <= 0xFF; test_value_a++) {
        for (word test_value_b = 0x00; test_value_b <= 0xFF; test_value_b++) {
            for (int c = 0; c < 2; c++) {
                returned_result = arith_func(test_value_a, test_value_b, (bool)c);
                expected_result = long_arith_func(test_value_a, test_value_b, (bool)c);
                ASSERT_TRUE(
                    cmp_results_byte(returned_result, expected_result),
                    "Result Mismatch. Input: lhs [0x%.2X], rhs [0x%.2X], C flag [%c]",
                    test_value_a,
                    test_value_b,
                    (c == 1) ? 'x' : ' '
                );
            }
        }
    }

    return true;
}
static bool fuzz_16bit_r16(
    struct alu_result (*arith_func)(const word value),
    struct alu_result (*long_arith_func)(const word value)
) {
    struct alu_result returned_result;
    struct alu_result expected_result;

    for (dword test_value = 0x0000; test_value <= 0xFFFF; test_value++) {
        returned_result = arith_func(test_value);
        expected_result = long_arith_func(test_value);
        ASSERT_TRUE(
            cmp_results_byte(returned_result, expected_result),
            "Result Mismatch. Input: value [0x%.4X]",
            test_value
        );
    }

    return true;
}
static bool fuzz_16bit_r16_r8(
    struct alu_result (*arith_func)(const word lhs, const byte rhs),
    struct alu_result (*long_arith_func)(const word lhs, const byte rhs)
) {
    struct alu_result returned_result;
    struct alu_result expected_result;

    for (dword test_value_a = 0x0000; test_value_a <= 0xFFFF; test_value_a++) {
        for (word test_value_b = 0x00; test_value_b <= 0xFF; test_value_b++) {
            returned_result = arith_func(test_value_a, test_value_b);
            expected_result = long_arith_func(test_value_a, test_value_b);
            ASSERT_TRUE(
                cmp_results_byte(returned_result, expected_result),
                "Result Mismatch. Input: lhs [0x%.4X], rhs [0x%.2X]",
                test_value_a,
                test_value_b
            );
        }
    }

    return true;
}
static bool fuzz_16bit_r16_r16(
    struct alu_result (*arith_func)(const word lhs, const word rhs),
    struct alu_result (*long_arith_func)(const word lhs, const word rhs)
) {
    struct alu_result returned_result;
    struct alu_result expected_result;

    for (dword test_value_a = 0x0000; test_value_a <= 0xFFFF; test_value_a++) {
        for (dword test_value_b = 0x0000; test_value_b <= 0xFFFF; test_value_b++) {
            returned_result = arith_func(test_value_a, test_value_b);
            expected_result = long_arith_func(test_value_a, test_value_b);
            ASSERT_TRUE(
                cmp_results_byte(returned_result, expected_result),
                "Result Mismatch. Input: lhs [0x%.4X], rhs [0x%.4X]",
                test_value_a,
                test_value_b
            );
        }
    }

    return true;
}
static bool fuzz_16bit_r16_r16_fast(
    struct alu_result (*arith_func)(const word lhs, const word rhs),
    struct alu_result (*long_arith_func)(const word lhs, const word rhs)
) {
    struct alu_result returned_result;
    struct alu_result expected_result;

    for (dword a = 0; a <= 0xFFFF; a++) {
        // 1. Always test the "Critical" edge cases for B
        word critical_b[] = {0, 1, 0x0FFF, 0x1000, 0x7FFF, 0x8000, 0xFFFF};

        for (int i = 0; i < 7; i++) {
            word b          = critical_b[i];
            returned_result = arith_func(a, b);
            expected_result = long_arith_func(a, b);
            ASSERT_TRUE(
                cmp_results_byte(returned_result, expected_result),
                "Result Mismatch. Input: lhs [0x%.4X], rhs [0x%.4X]",
                a,
                b
            );
        }

        // 2. Use a Prime Stride to sample the rest of the space
        // This reduces 65536 iterations per 'a' to ~417, but hits different bits
        for (dword b = 0; b <= 0xFFFF; b += 157) {
            returned_result = arith_func(a, (word)b);
            expected_result = long_arith_func(a, (word)b);
            ASSERT_TRUE(
                cmp_results_byte(returned_result, expected_result),
                "Result Mismatch. Input: lhs [0x%.4X], rhs [0x%.4X]",
                a,
                b
            );
        }
    }
    return true;
}

// Helper Fxns
// ===========
