/**
 * @file test_alu.c
 * @brief File containing tests to verify the functionality of the Arithmetic
 * Logic Unit (ALU) module.
 */
#include <stdbool.h>

#include "DEFINES.h"
#include "testing.h"

#include "cpu/alu/alu.h"

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

static bool test_8bit_logic(void);
static bool test_8bit_logic_and(void);
static bool test_8bit_logic_xor(void);
static bool test_8bit_logic_or(void);
static bool test_8bit_logic_cpl(void);

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

static bool test_16bit_inc(void);
static bool test_16bit_dec(void);
static bool test_16bit_add(void);

static bool test_misc_add_16b_e8(void);
static bool test_misc_da(void);

static bool
    fuzz_8bit_r8_arith(struct alu_result (*arith_func)(const byte value));
static bool fuzz_8bit_r8_r8_arith(
    struct alu_result (*arith_func)(const byte lhs, const byte rhs));
static bool fuzz_8bit_r8_r8_c_arith(struct alu_result (*arith_func)(
    const byte lhs, const byte rhs, const bool carry));

int main(void) {
  // 0 indicates a success, 1 a failure
  return (test_alu_module() == true) ? 0 : 1;
}

static bool test_alu_module(void) {
  ASSERT_TRUE(test_8bit_operations(), "8-bit Operations Test Failed!");
  ASSERT_TRUE(test_16bit_operations(), "16-bit Operations Test Failed!");
  ASSERT_TRUE(test_misc_operations(), "Miscellaneous Operations Test Failed!");
  return true;
}

// 8-bit Operations
// ================

static bool test_8bit_operations(void) {
  ASSERT_TRUE(test_8bit_arith(), "Test 8-bit Arithmetic Operations Failed!");
  ASSERT_TRUE(test_8bit_logic(), "Test 8-bit Logical Operations Failed!");
  ASSERT_TRUE(test_8bit_manip(),
              "Test 8-bit Bit Manipulation Operations Failed!");
  return true;
}

// Arithmetic Operations
// ---------------------

static bool test_8bit_arith_inc(void) {}
static bool test_8bit_arith_dec(void);
static bool test_8bit_arith_add(void);
static bool test_8bit_arith_sub(void);
static bool test_8bit_arith_cp(void);
static bool test_8bit_arith_adc(void);
static bool test_8bit_arith_sbc(void);

// ---------------------
// Arithmetic Operations

// Logical Operations
// ------------------

// ------------------
// Logical Operations

// Bit Manipulation
// ----------------

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

// =================
// 16-bit Operations

// Miscellaneous Operations
// ========================

static bool test_misc_operations(void) {
  ASSERT_TRUE(test_misc_add_16b_e8(), "Test Miscellaneous ADD Failed!");
  ASSERT_TRUE(test_misc_da(), "Test Miscellaneous DA Failed!");
  return true;
}

// ========================
// Miscellaneous Operations

// Helper Fxns
// ===========

static bool cmp_results_byte(const struct alu_result returned,
                             const struct alu_result expected) {
  ASSERT_EQ(returned.result.byte, expected.result.byte,
            "Bad Result. Value should have been 0x%.2X but was 0x%.2X.",
            expected.result.byte, returned.result.byte);
  return cmp_results_flags(returned, expected);
}

static bool cmp_results_word(const struct alu_result returned,
                             const struct alu_result expected) {
  ASSERT_EQ(returned.result.word, expected.result.word,
            "Bad Result. Value should have been 0x%.4X but was 0x%.4X.",
            expected.result.word, returned.result.word);
  return cmp_results_flags(returned, expected);
}

static bool cmp_results_flags(const struct alu_result returned,
                              const struct alu_result expected) {
  ASSERT_EQ(returned.flags.z, expected.flags.z,
            "Bad Result. Z flag should have been [%c] but was [%c].",
            expected.flags.z ? 'x' : ' ', returned.flags.z ? 'x' : ' ');
  ASSERT_EQ(returned.flags.n, expected.flags.n,
            "Bad Result. N flag should have been [%c] but was [%c].",
            expected.flags.n ? 'x' : ' ', returned.flags.n ? 'x' : ' ');
  ASSERT_EQ(returned.flags.h, expected.flags.h,
            "Bad Result. H flag should have been [%c] but was [%c].",
            expected.flags.h ? 'x' : ' ', returned.flags.h ? 'x' : ' ');
  ASSERT_EQ(returned.flags.c, expected.flags.c,
            "Bad Result. C flag should have been [%c] but was [%c].",
            expected.flags.c ? 'x' : ' ', returned.flags.c ? 'x' : ' ');
  return true;
}

static bool
fuzz_8bit_r8_arith(struct alu_result (*arith_func)(const byte value)) {
  struct alu_result returned_result;
  struct alu_result expected_result;

  for (word test_value = 0x00; test_value < 0xFF; test_value++) {
    returned_result = arith_func(test_value);
    expected_result.result.byte = (byte)test_value + 1;
    expected_result.flags.z = (expected_result.result.byte == 0);
    expected_result.flags.n = false;
    expected_result.flags.h = ((test_value & 0x0F) + 1) > 0x0F;
    expected_result.flags.c = returned_result.flags.c;
    ASSERT_TRUE(cmp_results_byte(returned_result, expected_result),
                "Result Mismatch");
  }

  return true;
}
static bool fuzz_8bit_r8_r8_arith(
    struct alu_result (*arith_func)(const byte lhs, const byte rhs)) {
  struct alu_result returned_result;
  struct alu_result expected_result;

  for (word test_value_a = 0x00; test_value_a < 0xFF; test_value_a++) {
    for (word test_value_b = 0x00; test_value_b < 0xFF; test_value_b++) {
      returned_result = arith_func(test_value_a, test_value_b);
    }
  }
}
static bool fuzz_8bit_r8_r8_c_arith(struct alu_result (*arith_func)(
    const byte lhs, const byte rhs, const bool carry));

// Helper Fxns
// ===========