/**
 * @file unit_test_registers.c
 * @brief Unit tests for the registers module.
 * @note Tests focus on the remaining API surface that contains real logic:
 *       - registers_set_AF  | F nibble mask invariant via 16-bit write
 *       - registers_set_F   | F nibble mask invariant via 8-bit write
 *       - registers_set_flags | Flag bit packing and nibble mask invariant
 *       - registers_get_flag_* | Correct bit position per flag
 *       - Register composition | Union memory layout assumptions
 */
#include <log.h>
#include <stdbool.h>

#include "DEFINES.h"
#include "cpu/registers.h"
#include "testing.h"

LOG_MODULE_SETUP_DEFAULT("UNIT TEST REGISTERS");

// Forward Declarations
// ====================

static bool test_registers_module(void);

static bool test_set_AF(void);
static bool test_set_F(void);

static bool test_set_flags(void);
static bool test_set_flags_individual_z(void);
static bool test_set_flags_individual_n(void);
static bool test_set_flags_individual_h(void);
static bool test_set_flags_individual_c(void);
static bool test_set_flags_lower_nibble(void);

static bool test_flag_getters(void);
static bool test_flag_getter_z(void);
static bool test_flag_getter_n(void);
static bool test_flag_getter_h(void);
static bool test_flag_getter_c(void);
static bool test_flag_getter_isolation(void);

static bool test_composition(void);
static bool test_composition_AF(void);
static bool test_composition_BC(void);
static bool test_composition_DE(void);
static bool test_composition_HL(void);

// ====================
// Forward Declarations

int main(void) { return (test_registers_module() == true) ? 0 : 1; }

static bool test_registers_module(void) {
    ASSERT_TRUE(test_set_AF(), "set_AF Tests Failed!");
    ASSERT_TRUE(test_set_F(), "set_F Tests Failed!");
    ASSERT_TRUE(test_set_flags(), "set_flags Tests Failed!");
    ASSERT_TRUE(test_flag_getters(), "Flag Getter Tests Failed!");
    ASSERT_TRUE(test_composition(), "Composition Tests Failed!");
    return true;
}

// set_AF
// ======

static bool test_set_AF(void) {
    struct registers regs = {0};

    // Verify the full value range, checking the nibble mask is enforced
    for (dword test_value = 0x0000; test_value <= 0xFFFF; test_value++) {
        regs = (struct registers){0};
        registers_set_AF(&regs, (word)test_value);

        word expected = (word)test_value & 0xFFF0;
        ASSERT_EQ(
            regs.AF,
            expected,
            "set_AF mask failure. Input 0x%.4X: expected 0x%.4X but got 0x%.4X",
            (word)test_value,
            expected,
            regs.AF
        );
    }

    return true;
}

// ======
// set_AF

// set_F
// =====

static bool test_set_F(void) {
    struct registers regs = {0};

    // Verify the full byte range, checking the nibble mask is enforced
    for (word test_value = 0x00; test_value <= 0xFF; test_value++) {
        regs = (struct registers){0};
        registers_set_F(&regs, (byte)test_value);

        byte expected = (byte)test_value & 0xF0;
        ASSERT_EQ(
            regs.F,
            expected,
            "set_F mask failure. Input 0x%.2X: expected 0x%.2X but got 0x%.2X",
            (byte)test_value,
            expected,
            regs.F
        );
    }

    return true;
}

// =====
// set_F

// set_flags
// =========

static bool test_set_flags(void) {
    ASSERT_TRUE(test_set_flags_individual_z(), "set_flags Z bit test failed!");
    ASSERT_TRUE(test_set_flags_individual_n(), "set_flags N bit test failed!");
    ASSERT_TRUE(test_set_flags_individual_h(), "set_flags H bit test failed!");
    ASSERT_TRUE(test_set_flags_individual_c(), "set_flags C bit test failed!");
    ASSERT_TRUE(test_set_flags_lower_nibble(), "set_flags lower nibble test failed!");
    return true;
}

static bool test_set_flags_individual_z(void) {
    struct registers regs = {0};

    registers_set_flags(&regs, true, false, false, false);
    ASSERT_EQ(regs.F, 0x80, "Z set: expected 0x80 but got 0x%.2X", regs.F);

    registers_set_flags(&regs, false, false, false, false);
    ASSERT_EQ(regs.F, 0x00, "Z clear: expected 0x00 but got 0x%.2X", regs.F);

    return true;
}

static bool test_set_flags_individual_n(void) {
    struct registers regs = {0};

    registers_set_flags(&regs, false, true, false, false);
    ASSERT_EQ(regs.F, 0x40, "N set: expected 0x40 but got 0x%.2X", regs.F);

    registers_set_flags(&regs, false, false, false, false);
    ASSERT_EQ(regs.F, 0x00, "N clear: expected 0x00 but got 0x%.2X", regs.F);

    return true;
}

static bool test_set_flags_individual_h(void) {
    struct registers regs = {0};

    registers_set_flags(&regs, false, false, true, false);
    ASSERT_EQ(regs.F, 0x20, "H set: expected 0x20 but got 0x%.2X", regs.F);

    registers_set_flags(&regs, false, false, false, false);
    ASSERT_EQ(regs.F, 0x00, "H clear: expected 0x00 but got 0x%.2X", regs.F);

    return true;
}

static bool test_set_flags_individual_c(void) {
    struct registers regs = {0};

    registers_set_flags(&regs, false, false, false, true);
    ASSERT_EQ(regs.F, 0x10, "C set: expected 0x10 but got 0x%.2X", regs.F);

    registers_set_flags(&regs, false, false, false, false);
    ASSERT_EQ(regs.F, 0x00, "C clear: expected 0x00 but got 0x%.2X", regs.F);

    return true;
}

static bool test_set_flags_lower_nibble(void) {
    // Verify that no combination of flag inputs can set the lower nibble
    struct registers regs = {0};

    bool values[2] = {false, true};
    for (int z = 0; z < 2; z++) {
        for (int n = 0; n < 2; n++) {
            for (int h = 0; h < 2; h++) {
                for (int c = 0; c < 2; c++) {
                    regs = (struct registers){0};
                    registers_set_flags(&regs, values[z], values[n], values[h], values[c]);

                    ASSERT_EQ(
                        (regs.F & 0x0F),
                        0x00,
                        "Lower nibble corrupted for z=%d n=%d h=%d c=%d. "
                        "F=0x%.2X",
                        values[z],
                        values[n],
                        values[h],
                        values[c],
                        regs.F
                    );
                }
            }
        }
    }

    return true;
}

// =========
// set_flags

// Flag Getters
// ============

static bool test_flag_getters(void) {
    ASSERT_TRUE(test_flag_getter_z(), "Flag getter Z failed!");
    ASSERT_TRUE(test_flag_getter_n(), "Flag getter N failed!");
    ASSERT_TRUE(test_flag_getter_h(), "Flag getter H failed!");
    ASSERT_TRUE(test_flag_getter_c(), "Flag getter C failed!");
    ASSERT_TRUE(test_flag_getter_isolation(), "Flag getter isolation failed!");
    return true;
}

static bool test_flag_getter_z(void) {
    struct registers regs = {0};

    regs.F = 0x80;
    ASSERT_TRUE(registers_get_flag_z(&regs), "Z getter: expected true with F=0x80");

    regs.F = 0x70;
    ASSERT_FALSE(registers_get_flag_z(&regs), "Z getter: expected false with F=0x70");

    return true;
}

static bool test_flag_getter_n(void) {
    struct registers regs = {0};

    regs.F = 0x40;
    ASSERT_TRUE(registers_get_flag_n(&regs), "N getter: expected true with F=0x40");

    regs.F = 0xB0;
    ASSERT_FALSE(registers_get_flag_n(&regs), "N getter: expected false with F=0xB0");

    return true;
}

static bool test_flag_getter_h(void) {
    struct registers regs = {0};

    regs.F = 0x20;
    ASSERT_TRUE(registers_get_flag_h(&regs), "H getter: expected true with F=0x20");

    regs.F = 0xD0;
    ASSERT_FALSE(registers_get_flag_h(&regs), "H getter: expected false with F=0xD0");

    return true;
}

static bool test_flag_getter_c(void) {
    struct registers regs = {0};

    regs.F = 0x10;
    ASSERT_TRUE(registers_get_flag_c(&regs), "C getter: expected true with F=0x10");

    regs.F = 0xE0;
    ASSERT_FALSE(registers_get_flag_c(&regs), "C getter: expected false with F=0xE0");

    return true;
}

static bool test_flag_getter_isolation(void) {
    // Verify each getter only responds to its own bit and not its neighbours
    struct registers regs = {0};

    regs.F = 0x80;
    ASSERT_TRUE(registers_get_flag_z(&regs), "Isolation: Z should be set   with F=0x80");
    ASSERT_FALSE(registers_get_flag_n(&regs), "Isolation: N should be clear with F=0x80");
    ASSERT_FALSE(registers_get_flag_h(&regs), "Isolation: H should be clear with F=0x80");
    ASSERT_FALSE(registers_get_flag_c(&regs), "Isolation: C should be clear with F=0x80");

    regs.F = 0x40;
    ASSERT_FALSE(registers_get_flag_z(&regs), "Isolation: Z should be clear with F=0x40");
    ASSERT_TRUE(registers_get_flag_n(&regs), "Isolation: N should be set   with F=0x40");
    ASSERT_FALSE(registers_get_flag_h(&regs), "Isolation: H should be clear with F=0x40");
    ASSERT_FALSE(registers_get_flag_c(&regs), "Isolation: C should be clear with F=0x40");

    regs.F = 0x20;
    ASSERT_FALSE(registers_get_flag_z(&regs), "Isolation: Z should be clear with F=0x20");
    ASSERT_FALSE(registers_get_flag_n(&regs), "Isolation: N should be clear with F=0x20");
    ASSERT_TRUE(registers_get_flag_h(&regs), "Isolation: H should be set   with F=0x20");
    ASSERT_FALSE(registers_get_flag_c(&regs), "Isolation: C should be clear with F=0x20");

    regs.F = 0x10;
    ASSERT_FALSE(registers_get_flag_z(&regs), "Isolation: Z should be clear with F=0x10");
    ASSERT_FALSE(registers_get_flag_n(&regs), "Isolation: N should be clear with F=0x10");
    ASSERT_FALSE(registers_get_flag_h(&regs), "Isolation: H should be clear with F=0x10");
    ASSERT_TRUE(registers_get_flag_c(&regs), "Isolation: C should be set   with F=0x10");

    return true;
}

// ============
// Flag Getters

// Composition Tests
// =================
// These tests validate the union memory layout assumptions.
// They are platform/layout assertions, not API tests.

static bool test_composition(void) {
    ASSERT_TRUE(test_composition_AF(), "AF composition test failed!");
    ASSERT_TRUE(test_composition_BC(), "BC composition test failed!");
    ASSERT_TRUE(test_composition_DE(), "DE composition test failed!");
    ASSERT_TRUE(test_composition_HL(), "HL composition test failed!");
    return true;
}

static bool test_composition_AF(void) {
    struct registers regs = {0};

    // Core -> Virtual: writing AF should be visible in A and F (with mask)
    for (dword test_value = 0x0000; test_value <= 0xFFFF; test_value++) {
        regs = (struct registers){0};
        registers_set_AF(&regs, (word)test_value);

        byte expected_a = (byte)((word)test_value >> 8);
        byte expected_f = (byte)((word)test_value & 0xF0);
        ASSERT_EQ(
            regs.A,
            expected_a,
            "AF->A composition failure. Input 0x%.4X: expected 0x%.2X but got 0x%.2X",
            (word)test_value,
            expected_a,
            regs.A
        );
        ASSERT_EQ(
            regs.F,
            expected_f,
            "AF->F composition failure. Input 0x%.4X: expected 0x%.2X but got 0x%.2X",
            (word)test_value,
            expected_f,
            regs.F
        );
    }

    // Virtual -> Core: writing A and F independently should compose into AF
    for (dword test_value = 0x0000; test_value <= 0xFFFF; test_value++) {
        regs   = (struct registers){0};
        regs.A = (byte)((word)test_value >> 8);
        registers_set_F(&regs, (byte)test_value);

        word expected = ((word)regs.A << 8) | regs.F;
        ASSERT_EQ(
            regs.AF,
            expected,
            "A+F->AF composition failure. Expected 0x%.4X but got 0x%.4X",
            expected,
            regs.AF
        );
    }

    return true;
}

static bool test_composition_BC(void) {
    struct registers regs = {0};

    // Core -> Virtual
    for (dword test_value = 0x0000; test_value <= 0xFFFF; test_value++) {
        regs    = (struct registers){0};
        regs.BC = (word)test_value;

        byte expected_b = (byte)((word)test_value >> 8);
        byte expected_c = (byte)test_value;
        ASSERT_EQ(
            regs.B,
            expected_b,
            "BC->B composition failure. Input 0x%.4X: expected 0x%.2X but got 0x%.2X",
            (word)test_value,
            expected_b,
            regs.B
        );
        ASSERT_EQ(
            regs.C,
            expected_c,
            "BC->C composition failure. Input 0x%.4X: expected 0x%.2X but got 0x%.2X",
            (word)test_value,
            expected_c,
            regs.C
        );
    }

    // Virtual -> Core
    for (dword test_value = 0x0000; test_value <= 0xFFFF; test_value++) {
        regs   = (struct registers){0};
        regs.B = (byte)((word)test_value >> 8);
        regs.C = (byte)test_value;

        ASSERT_EQ(
            regs.BC,
            (word)test_value,
            "B+C->BC composition failure. Expected 0x%.4X but got 0x%.4X",
            (word)test_value,
            regs.BC
        );
    }

    return true;
}

static bool test_composition_DE(void) {
    struct registers regs = {0};

    // Core -> Virtual
    for (dword test_value = 0x0000; test_value <= 0xFFFF; test_value++) {
        regs    = (struct registers){0};
        regs.DE = (word)test_value;

        byte expected_d = (byte)((word)test_value >> 8);
        byte expected_e = (byte)test_value;
        ASSERT_EQ(
            regs.D,
            expected_d,
            "DE->D composition failure. Input 0x%.4X: expected 0x%.2X but got 0x%.2X",
            (word)test_value,
            expected_d,
            regs.D
        );
        ASSERT_EQ(
            regs.E,
            expected_e,
            "DE->E composition failure. Input 0x%.4X: expected 0x%.2X but got 0x%.2X",
            (word)test_value,
            expected_e,
            regs.E
        );
    }

    // Virtual -> Core
    for (dword test_value = 0x0000; test_value <= 0xFFFF; test_value++) {
        regs   = (struct registers){0};
        regs.D = (byte)((word)test_value >> 8);
        regs.E = (byte)test_value;

        ASSERT_EQ(
            regs.DE,
            (word)test_value,
            "D+E->DE composition failure. Expected 0x%.4X but got 0x%.4X",
            (word)test_value,
            regs.DE
        );
    }

    return true;
}

static bool test_composition_HL(void) {
    struct registers regs = {0};

    // Core -> Virtual
    for (dword test_value = 0x0000; test_value <= 0xFFFF; test_value++) {
        regs    = (struct registers){0};
        regs.HL = (word)test_value;

        byte expected_h = (byte)((word)test_value >> 8);
        byte expected_l = (byte)test_value;
        ASSERT_EQ(
            regs.H,
            expected_h,
            "HL->H composition failure. Input 0x%.4X: expected 0x%.2X but got 0x%.2X",
            (word)test_value,
            expected_h,
            regs.H
        );
        ASSERT_EQ(
            regs.L,
            expected_l,
            "HL->L composition failure. Input 0x%.4X: expected 0x%.2X but got 0x%.2X",
            (word)test_value,
            expected_l,
            regs.L
        );
    }

    // Virtual -> Core
    for (dword test_value = 0x0000; test_value <= 0xFFFF; test_value++) {
        regs   = (struct registers){0};
        regs.H = (byte)((word)test_value >> 8);
        regs.L = (byte)test_value;

        ASSERT_EQ(
            regs.HL,
            (word)test_value,
            "H+L->HL composition failure. Expected 0x%.4X but got 0x%.4X",
            (word)test_value,
            regs.HL
        );
    }

    return true;
}

// =================
// Composition Tests
