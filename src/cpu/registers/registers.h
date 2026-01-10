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

// Main 16-bit Registers
// =====================
/**
 * @brief Sets register AF.
 * @param value Value to set register to.
 * @warning Bits 0-3 are shorted to GND. Any value being assigned to AF will be bit-masked by
 * 0xFFF0.
 * @note AF tracks the current CPU state. In particular:
 * @note - A | Accumulator: Last result from the ALU. See set_A() or get_A() for more details
 * @note - F | Flags: Current CPU flags. See set_F() or get_F() for more details
 */
void registers_set_AF(const word value);
/**
 * @brief Sets register BC.
 * @param value Value to set register to.
 * @note General purpose register.
 */
void registers_set_BC(const word value);
/**
 * @brief Sets register DE.
 * @param value Value to set register to.
 * @note General purpose register.
 */
void registers_set_DE(const word value);
/**
 * @brief Sets register HL.
 * @param value Value to set register to.
 * @note General purpose register. Also used frequently for indirect addressing mode allowing
 * 16-bit memory lookup
 */
void registers_set_HL(const word value);
/**
 * @brief Sets register SP.
 * @param value Value to set register to.
 * @note Stack Pointer. Tracks the current position of the stack.
 */
void registers_set_SP(const word value);
/**
 * @brief Sets register PC.
 * @param value Value to set register to.
 * @note Program Counter/Pointer. Tracks the current instruction.
 */
void registers_set_PC(const word value);

/**
 * @brief Gets register AF.
 * @returns The value of the register.
 * @note AF tracks the current CPU state. In particular:
 *
 * - A | Accumulator: Last result from the ALU.
 * - F | Flags: Current CPU flags. See set_F() or get_F() for more details
 */
word registers_get_AF(void);
/**
 * @brief Gets register BC.
 * @returns The value of the register.
 * @note General purpose register.
 */
word registers_get_BC(void);
/**
 * @brief Gets register DE.
 * @returns The value of the register.
 * @note General purpose register.
 */
word registers_get_DE(void);
/**
 * @brief Gets register HL.
 * @returns The value of the register.
 * @note General purpose register. Also used frequently for indirect addressing mode allowing
 * 16-bit memory lookup
 */
word registers_get_HL(void);
/**
 * @brief Gets register SP.
 * @returns The value of the register.
 * @note Stack Pointer. Tracks the current position of the stack.
 */
word registers_get_SP(void);
/**
 * @brief Gets register PC.
 * @returns The value of the register.
 * @note Program Counter/Pointer. Tracks the current instruction.
 */
word registers_get_PC(void);

// =====================
// Main 16-bit Registers

// Virtual 8-bit Registers
// =======================

/**
 * @brief Sets register A.
 * @param value Value to set the register to.
 * @note Accumulator. Stores the last result of the ALU.
 */
void registers_set_A(const byte value);
/**
 * @brief Sets register F.
 * @param value Value to set the register to.
 * @warning Bits 0-3 are shorted to GND. Any value being assigned to AF will be bit-masked by
 * 0xFFF0.
 * @note CPU Flags. Stores the current CPU flags. In particular:
 * @note - 0-3 | - | Not Used. Cannot be set. Always 0.
 * @note -  4  | c | Carry
 * @note -  5  | h | Half Carry (BCD)
 * @note -  6  | n | Subtraction (BCD)
 * @note -  7  | z | Zero
 */
void registers_set_F(const byte value);
/**
 * @brief Sets register B.
 * @param value Value to set the register to.
 * @note General purpose register
 */
void registers_set_B(const byte value);
/**
 * @brief Sets register C.
 * @param value Value to set the register to.
 * @note General purpose register
 */
void registers_set_C(const byte value);
/**
 * @brief Sets register D.
 * @param value Value to set the register to.
 * @note General purpose register
 */
void registers_set_D(const byte value);
/**
 * @brief Sets register E.
 * @param value Value to set the register to.
 * @note General purpose register
 */
void registers_set_E(const byte value);
/**
 * @brief Sets register H.
 * @param value Value to set the register to.
 * @note General purpose register
 */
void registers_set_H(const byte value);
/**
 * @brief Sets register L.
 * @param value Value to set the register to.
 * @note General purpose register
 */
void registers_set_L(const byte value);

/**
 * @brief Gets register A.
 * @returns The value of the register.
 * @note Accumulator. Stores the last result of the ALU.
 */
byte registers_get_A(void);
/**
 * @brief Gets register F.
 * @returns The value of the register.
 * @note CPU Flags. Stores the current CPU flags. In particular:
 * @note - 0-3 | - | Not Used. Cannot be set. Always 0.
 * @note -  4  | c | Carry
 * @note -  5  | h | Half Carry (BCD)
 * @note -  6  | n | Subtraction (BCD)
 * @note -  7  | z | Zero
 */
byte registers_get_F(void);
/**
 * @brief Gets register B.
 * @returns The value of the register.
 * @note General purpose register.
 */
byte registers_get_B(void);
/**
 * @brief Gets register C.
 * @returns The value of the register.
 * @note General purpose register.
 */
byte registers_get_C(void);
/**
 * @brief Gets register D.
 * @returns The value of the register.
 * @note General purpose register.
 */
byte registers_get_D(void);
/**
 * @brief Gets register E.
 * @returns The value of the register.
 * @note General purpose register.
 */
byte registers_get_E(void);
/**
 * @brief Gets register H.
 * @returns The value of the register.
 * @note General purpose register.
 */
byte registers_get_H(void);
/**
 * @brief Gets register L.
 * @returns The value of the register.
 * @note General purpose register.
 */
byte registers_get_L(void);

// =======================
// Virtual 8-bit Registers

// Flags
// =====

/**
 * @brief Sets the Zero flag to 1.
 * @note Generally set when the ALU returns a 0 result.
 */
void registers_set_flag_z(void);
/**
 * @brief Sets the Subtraction flag to 1.
 * @note Generally set when a BCD instruction performs a subtraction.
 */
void registers_set_flag_n(void);
/**
 * @brief Sets the Half carry flag to 1.
 * @note Generally set when a BCD instruction detects an overflow.
 */
void registers_set_flag_h(void);
/**
 * @brief Sets the Carry flag to 1.
 * @note Generally set when the ALU overflows.
 */
void registers_set_flag_c(void);

/** @brief Clears the Zero flag to 0. */
void registers_clear_flag_z(void);
/** @brief Clears the Subtraction flag to 0. */
void registers_clear_flag_n(void);
/** @brief Clears the Half carry flag to 0. */
void registers_clear_flag_h(void);
/** @brief Clears the Carry flag to 0. */
void registers_clear_flag_c(void);

/**
 * @brief Gets the Zero flag.
 * @returns The value of the flag.
 * @note Generally set when the ALU returns a 0 result.
 */
bool registers_get_flag_z(void);
/**
 * @brief Gets the Subtraction flag.
 * @returns The value of the flag.
 * @note Generally set when a BCD instruction performs a subtraction.
 */
bool registers_get_flag_n(void);
/**
 * @brief Gets the Half carry flag.
 * @returns The value of the flag.
 * @note Generally set when a BCD instruction detects an overflow.
 */
bool registers_get_flag_h(void);
/**
 * @brief Gets the Carry flag.
 * @returns The value of the flag.
 * @note Generally set when the ALU overflows.
 */
bool registers_get_flag_c(void);

// =====
// Flags

// Test Functions
// ==============

/**
 * @brief Resets all registers back to 0x00.
 * @note This performs a memset on the entire memory block.
 */
void registers_reset(void);

/**
 * @brief Creates a snapshot of the current internal registers state.
 * @param regs Pointer to where to store the snapshot. Must not be NULL.
 * @note This performs a memcpy on the entire memory block.
 */
int registers_snapshot(struct registers* regs);

// ==============
// Test Functions
