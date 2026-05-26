/**
 * @file registers.h
 * @brief This file contains the generic Gameboy Registers
 */
#pragma once

#include <stdbool.h>

#include "DEFINES.h"

struct registers { /** @brief Structure that housing the registers. */
    word PC;       /** @brief Program Counter [16-bit]. Points to the current instruction. */
    word SP;       /** @brief Stack Pointer [16-bit]. Points to the next empty stack slot. */

    union {
        /** @brief General Purpose 16-bit Register. Also used for 16-bit addressing mode. */
        word HL;
        struct {
            byte L; /** @brief General Purpose 8-bit Virtual Register */
            byte H; /** @brief General Purpose 8-bit Virtual Register. */
        };
    };

    union {
        word DE; /** @brief General Purpose 16-bit Reigster. */
        struct {
            byte E; /** @brief General Purpose 8-bit Virtual Register. */
            byte D; /** @brief General Purpose 8-bit Virtual Register. */
        };
    };

    union {
        word BC; /** @brief General Purpose 16-bit Reigster. */
        struct {
            byte C; /** @brief General Purpose 8-bit Virtual Register. */
            byte B; /** @brief General Purpose 8-bit Virtual Register. */
        };
    };

    union {
        /**
         * @brief Special 16-bit Register.
         * @brief - High byte represents the ALU Accumulator.
         * @brief - Low byte represents the CPU flags.
         */
        word AF;
        struct {
            /**
             * @brief Flags [8-bit]. Stores the current CPU flags.
             * @note bits (0-3) are shorted to ground and will always be 0.
             */
            byte F;
            /** @brief Accumulator [8-bit]. Stores the last APU result. */
            byte A;
        };
    };
};

/**
 * @brief Sets register AF.
 * @param reg Register structure to update.
 * @param value Value to set register to.
 * @warning Bits 0-3 are shorted to GND. Any value being assigned to AF will be bit-masked by
 * 0xFFF0.
 * @note AF tracks the current CPU state. In particular:
 * @note - A | Accumulator: Last result from the ALU.
 * @note - F | Flags: Current CPU flags. See set_F() for more details
 */
void registers_set_AF(struct registers* reg, const word value);

/**
 * @brief Sets register F.
 * @param reg Register structure to update.
 * @param value Value to set the register to.
 * @warning Bits 0-3 are shorted to GND. Any value being assigned to F will be bit-masked by
 * 0xF0.
 * @note CPU Flags. Stores the current CPU flags. In particular:
 * @note - 0-3 | - | Not Used. Cannot be set. Always 0.
 * @note -  4  | c | Carry
 * @note -  5  | h | Half Carry (BCD)
 * @note -  6  | n | Subtraction (BCD)
 * @note -  7  | z | Zero
 */
void registers_set_F(struct registers* reg, const byte value);

/**
 * @brief Sets the registers flags.
 * @param reg Register structure to update.
 * @param z Zero flag.
 * @param n Subtraction flag.
 * @param h Half-carry flag.
 * @param c Carry flag.
 * @note This differs from the individual set flag functions as it can both set and clear depending
 * on the value given.
 */
void registers_set_flags(
    struct registers* reg,
    const bool        z,
    const bool        n,
    const bool        h,
    const bool        c
);

/**
 * @brief Gets the Zero flag.
 * @param reg Register structure to update.
 * @returns The value of the flag.
 * @note Generally set when the ALU returns a 0 result.
 */
bool registers_get_flag_z(struct registers* reg);
/**
 * @brief Gets the Subtraction flag.
 * @param reg Register structure to update.
 * @returns The value of the flag.
 * @note Generally set when a BCD instruction performs a subtraction.
 */
bool registers_get_flag_n(struct registers* reg);
/**
 * @brief Gets the Half carry flag.
 * @param reg Register structure to update.
 * @returns The value of the flag.
 * @note Generally set when a BCD instruction detects an overflow.
 */
bool registers_get_flag_h(struct registers* reg);
/**
 * @brief Gets the Carry flag.
 * @param reg Register structure to update.
 * @returns The value of the flag.
 * @note Generally set when the ALU overflows.
 */
bool registers_get_flag_c(struct registers* reg);
