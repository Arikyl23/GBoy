/**
 * @file alu.h
 * @brief File containing the API for interacting with the Arithmetic Logic Unit (ALU) module.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "DEFINES.h"

struct alu_result {
    union {
        byte byte;
        word word;
    } result;

    struct {
        bool z;
        bool n;
        bool h;
        bool c;
    } flags;
};


// 8-bit Operations
// ================
// Arithmetic Operations
// ---------------------

/**
 * @brief Increments the value by 1.
 * @param value Value to be incremented.
 * @returns An alu_result with result.byte and flags set.
 * @note The following is the generic flag result: Z 0 H -
 */
struct alu_result alu_inc_8b(const byte value);

/**
 * @brief Decrements the value by 1.
 * @param value Value to be decremented.
 * @returns An alu_result with result.byte and flags set.
 * @note The following is the generic flag result: Z 1 H -
 */
struct alu_result alu_dec_8b(const byte value);

/**
 * @brief Performs 8-bit addition.
 * @param lhs Left Hand Side. Usually the Accumulator (A).
 * @param rhs Right Hand Side. Usually another Register (r8).
 * @returns An alu_result with result.byte and flags set.
 * @note The following is the generic flag result: Z 0 H C
 */
struct alu_result alu_add_8b(const byte lhs, const byte rhs);

/**
 * @brief Performs 8-bit subtraction.
 * @param lhs Left Hand Side. Usually the Accumulator (A).
 * @param rhs Right Hand Side. Usually another Register (r8).
 * @returns An alu_result with result.byte and flags set.
 * @note The following is the generic flag result: Z 1 H C
 */
struct alu_result alu_sub_8b(const byte lhs, const byte rhs);

/**
 * @brief Performs 8-bit compare.
 * @param lhs Left Hand Side. Usually the Accumulator (A).
 * @param rhs Right Hand Side. Usually another Register (r8).
 * @returns An alu_result with flags set.
 * @note The following is the generic flag result: Z 1 H C
 */
struct alu_result alu_cp_8b(const byte lhs, const byte rhs);

/**
 * @brief Performs 8-bit addition with carry.
 * @param lhs Left Hand Side. Usually the Accumulator (A).
 * @param rhs Right Hand Side. Usually another Register (r8).
 * @param carry Current state of the carry flag.
 * @returns An alu_result with result.byte and flags set.
 * @note The following is the generic flag result: Z 0 H C
 */
struct alu_result alu_adc_8b(const byte lhs, const byte rhs, const bool carry);

/**
 * @brief Performs 8-bit subtraction with carry.
 * @param lhs Left Hand Side. Usually the Accumulator (A).
 * @param rhs Right Hand Side. Usually another Register (r8).
 * @param carry Current state of the carry flag.
 * @returns An alu_result with result.byte and flags set.
 * @note The following is the generic flag result: Z 1 H C
 */
struct alu_result alu_sbc_8b(const byte lhs, const byte rhs, const bool carry);

// ---------------------
// Arithmetic Operations

// Logical Operations
// ------------------

/**
 * @brief Performs 8-bit bitwise AND.
 * @param lhs Left Hand Side. Usually the Accumulator (A).
 * @param rhs Right Hand Side. Usually another Register (r8).
 * @returns An alu_result with result.byte and flags set.
 * @note The following is the generic flag result: Z 0 1 0
 */
struct alu_result alu_and_8b(const byte lhs, const byte rhs);

/**
 * @brief Performs 8-bit bitwise XOR.
 * @param lhs Left Hand Side. Usually the Accumulator (A).
 * @param rhs Right Hand Side. Usually another Register (r8).
 * @returns An alu_result with result.byte and flags set.
 * @note The following is the generic flag result: Z 0 0 0
 */
struct alu_result alu_xor_8b(const byte lhs, const byte rhs);

/**
 * @brief Performs 8-bit bitwise OR.
 * @param lhs Left Hand Side. Usually the Accumulator (A).
 * @param rhs Right Hand Side. Usually another Register (r8).
 * @returns An alu_result with result.byte and flags set.
 * @note The following is the generic flag result: Z 0 0 0
 */
struct alu_result alu_or_8b(const byte lhs, const byte rhs);

/**
 * @brief Performs 8-bit bitwise NOT (complement).
 * @param value Value to operate on. Usually the Accumulator (A).
 * @returns An alu_result with result.byte and flags set.
 * @note The following is the generic flag result: - 1 1 -
 */
struct alu_result alu_cpl_8b(const byte value);

// ------------------
// Logical Operations

// Bit Manipulation
// ----------------

/**
 * @brief Performs an 8-bit Rotate Left Circular operation.
 *
 * C <- b7 <- b6 <- b5 <- b4 <- b3 <- b2 <- b1 <- b0 <- b7
 * @param value Value to operate on.
 * @returns An alu_result with the result.byte and flags set.
 * @note The following is the generic flag result: Z 0 0 C
 */
struct alu_result alu_rlc_8b(const byte value);

/**
 * @brief Performs an 8-bit Rotate Right Circular operation.
 *
 * b0 -> b7 -> b6 -> b5 -> b4 -> b3 -> b2 -> b1 -> b0 -> C
 * @param value Value to operate on.
 * @returns An alu_result with the result.byte and flags set.
 * @note The following is the generic flag result: Z 0 0 C
 */
struct alu_result alu_rrc_8b(const byte value);

/**
 * @brief Performs an 8-bit Rotate Left operation.
 *
 * C <- b7 <- b6 <- b5 <- b4 <- b3 <- b2 <- b1 <- b0 <- C
 * @param value Value to operate on.
 * @param carry Current state of the carry flag.
 * @returns An alu_result with the result.byte and flags set.
 * @note The following is the generic flag result: Z 0 0 C
 */
struct alu_result alu_rl_8b(const byte value, const bool carry);

/**
 * @brief Performs an 8-bit Rotate Right operation.
 *
 * C -> b7 -> b6 -> b5 -> b4 -> b3 -> b2 -> b1 -> b0 -> C
 * @param value Value to operate on.
 * @param carry Current state of the carry flag.
 * @returns An alu_result with the result.byte and flags set.
 * @note The following is the generic flag result: Z 0 0 C
 */
struct alu_result alu_rr_8b(const byte value, const bool carry);

/**
 * @brief Performs an 8-bit Shift Left Arithmetically operation.
 *
 * C <- b7 <- b6 <- b5 <- b4 <- b3 <- b2 <- b1 <- b0 <- 0
 * @param value Value to operate on.
 * @returns An alu_result with the result.byte and flags set.
 * @note The following is the generic flag result: Z 0 0 C
 */
struct alu_result alu_sla_8b(const byte value);

/**
 * @brief Performs an 8-bit Shift Right Arithmetically operation.
 *
 * b7 -> b6 -> b5 -> b4 -> b3 -> b2 -> b1 -> b0 -> C
 * @param value Value to operate on.
 * @returns An alu_result with the result.byte and flags set.
 * @note The following is the generic flag result: Z 0 0 C
 */
struct alu_result alu_sra_8b(const byte value);

/**
 * @brief Performs an 8-bit SWAP operation. This swaps the low nibble with the high nibble. 
 *
 * b7-4 <-> b3-0
 * @param value Value to operate on.
 * @returns An alu_result with the result.byte and flags set.
 * @note The following is the generic flag result: Z 0 0 0
 */
struct alu_result alu_swap_8b(const byte value);

/**
 * @brief Performs an 8-bit Shift Right Logically operation.
 *
 * 0 -> b7 -> b6 -> b5 -> b4 -> b3 -> b2 -> b1 -> b0 -> C
 * @param value Value to operate on.
 * @returns An alu_result with the result.byte and flags set.
 * @note The following is the generic flag result: Z 0 0 C
 */
struct alu_result alu_srl_8b(const byte value);

/**
 * @brief Tests the specified bit.
 * @param bit Bit to test.
 * @param value Value to operate on.
 * @returns An alu_result with the flags set.
 * @note The following is the generic flag result: Z 0 1 -
 */
struct alu_result alu_bit_8b(const size_t bit, const byte value);

/**
 * @brief Resets/Clears the specified bit.
 * @param bit Bit to reset/clear.
 * @param value Value to operate on.
 * @returns An alu_result with the result.byte and flags set.
 * @note The following is the generic flag result: - - - -
 */
struct alu_result alu_res_8b(const size_t bit, const byte value);

/**
 * @brief Sets the specified bit.
 * @param bit Bit to set.
 * @param value Value to operate on.
 * @returns An alu_result with the result.byte and flags set.
 * @note The following is the generic flag result: - - - -
 */
struct alu_result alu_set_8b(const size_t bit, const byte value);

// ----------------
// Bit Manipulation
// ================
// 8-bit Operations


// 16-bit Operations
// =================

/**
 * @brief Increments the value by 1.
 * @param value Value to be incremented.
 * @returns An alu_result with result.word set.
 * @note The following is the generic flag result: - - - -
 */
struct alu_result alu_inc_16b(const word value);

/**
 * @brief Decrements the value by 1.
 * @param value Value to be decremented.
 * @returns An alu_result with result.word set.
 * @note The following is the generic flag result: - - - -
 */
struct alu_result alu_dec_16b(const word value);

/**
 * @brief Performs 16-bit addition.
 * @param lhs Left Hand Side. Usually a Register (r16)
 * @param rhs Right Hand Side. Usually a Register (r16).
 * @returns An alu_result with result.word and flags set.
 * @note The following is the generic flag result: - 0 H C
 */
struct alu_result alu_add_16b(const word lhs, const word rhs);

// =================
// 16-bit Operations

// Miscellaneous Operations
// ========================

/**
 * @brief Performs 16-bit addition against a signed byte.
 * @param lhs Left Hand Side. Usually the Stack Pointer (SP)
 * @param rhs Right Hand Side. Usually signed immediate data (e8).
 * @returns An alu_result with result.word and flags set.
 * @note The following is the generic flag result: 0 0 H C
 */
struct alu_result alu_add_16b_e8(const word lhs, const byte rhs);

/**
 * @brief Performs Decimal Adjustment on a Binary-Coded Decimal (BCD) value. Corrects the BCD value following a standard arithmetic operation.
 * @param bcd_value Binary-Coded Decimal (BCD) value to correct.
 * @param n_flag Current state of the subtraction flag.
 * @param h_flag Current state of the half carry flag.
 * @param c_flag Current state of the carry flag.
 * @returns An alu_result with result.byte and flags set.
 * @note The following is the generic flag result: Z - 0 C
 */
struct alu_result alu_da(const byte bcd_value, const bool n_flag, const bool h_flag, const bool c_flag);

// ========================
// Miscellaneous Operations