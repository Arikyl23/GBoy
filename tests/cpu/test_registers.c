/**
 * @file test_registers.c
 * @brief File containing tests to verify the functionality of the registers module
 */
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "ASSERT.h"
#include "DEFINES.h"
#include "cpu/registers/registers.h"

bool validate_memory_isolation(
    const byte* p_reg_before,
    const byte* p_reg_after,
    const size_t reg_offset,
    const size_t block_size
);

bool test_registers_module(void);

bool test_core_registers(void);
bool test_core_register_AF(void);
bool test_core_register_BC(void);
bool test_core_register_DE(void);
bool test_core_register_HL(void);
bool test_core_register_SP(void);
bool test_core_register_PC(void);
bool test_core_register(
    word (*reg_get)(void),
    void (*reg_set)(const word value),
    const size_t reg_offset,
    const word bitmask
);

bool test_virtual_registers(void);
bool test_virtual_register_A(void);
bool test_virtual_register_F(void);
bool test_virtual_register_B(void);
bool test_virtual_register_C(void);
bool test_virtual_register_D(void);
bool test_virtual_register_E(void);
bool test_virtual_register_H(void);
bool test_virtual_register_L(void);
bool test_virtual_register(
    byte (*reg_get)(void),
    void(*reg_set)(const byte value),
    const size_t reg_offset,
    const byte bitmask
);

bool test_composition(void);
bool test_composition_AF(void);
bool test_composition_BC(void);
bool test_composition_DE(void);
bool test_composition_HL(void);
bool test_register_composition(
    void (*core_reg_set)(const word value),
    word (*core_reg_get)(void),
    void (*virt_reg_hi_set)(const byte value),
    byte (*virt_reg_hi_get)(void),
    void (*virt_reg_lo_set)(const byte value),
    byte (*virt_reg_lo_get)(void),
    const word bitmask
);

bool test_flags(void);
bool test_flag_z(void);
bool test_flag_n(void);
bool test_flag_h(void);
bool test_flag_c(void);
bool test_flag(
    bool (*flag_get)(),
    void (*flag_set)(),
    void (*flag_clear)(),
    const int bit_index
);

int main(void) {
    return (test_registers_module() == true) ? 0 : 1; // 0 indicates a success, 1 a failure
}

bool validate_memory_isolation(
    const byte* p_reg_before,
    const byte* p_reg_after,
    const size_t reg_offset,
    const size_t block_size
) {
    for (size_t i = 0; i < sizeof(struct registers); i++) {
        if (i < reg_offset || i >= (reg_offset + block_size)) {
            ASSERT_EQ(p_reg_before[i], p_reg_after[i],
                "Isolation Failure at byte %zu. Should have been 0x%.2X but was 0x%.2X",
                i, p_reg_before[i], p_reg_after[i]
            );
        }
    }

    return true;
}

bool test_registers_module(void) {
    ASSERT_TRUE(test_core_registers(), "Core Registers Test Failed!");
    ASSERT_TRUE(test_virtual_registers(), "Virtual Registers Test Failed!");
    ASSERT_TRUE(test_composition(), "Composition Test Failed!");
    ASSERT_TRUE(test_flags(), "Flags Test Failed!");
    return true;
}

// Core Registers
// ==============

bool test_core_registers(void) {
    ASSERT_TRUE(test_core_register_AF(), "Test Core Register AF Failed!");
    ASSERT_TRUE(test_core_register_BC(), "Test Core Register BC Failed!");
    ASSERT_TRUE(test_core_register_DE(), "Test Core Register DE Failed!");
    ASSERT_TRUE(test_core_register_HL(), "Test Core Register HL Failed!");
    ASSERT_TRUE(test_core_register_SP(), "Test Core Register SP Failed!");
    ASSERT_TRUE(test_core_register_PC(), "Test Core Register PC Failed!");
    return true;
}

bool test_core_register_AF(void) {
    return test_core_register(
        registers_get_AF,
        registers_set_AF,
        offsetof(struct registers, AF),
        0xFFF0
    );
}

bool test_core_register_BC(void) {
    return test_core_register(
        registers_get_BC,
        registers_set_BC,
        offsetof(struct registers, BC),
        0xFFFF
    );
}

bool test_core_register_DE(void) {
    return test_core_register(
        registers_get_DE,
        registers_set_DE,
        offsetof(struct registers, DE),
        0xFFFF
    );
}

bool test_core_register_HL(void) {
    return test_core_register(
        registers_get_HL,
        registers_set_HL,
        offsetof(struct registers, HL),
        0xFFFF
    );
}

bool test_core_register_SP(void) {
    return test_core_register(
        registers_get_SP,
        registers_set_SP,
        offsetof(struct registers, SP),
        0xFFFF
    );
}

bool test_core_register_PC(void) {
    return test_core_register(
        registers_get_PC,
        registers_set_PC,
        offsetof(struct registers, PC),
        0xFFFF
    );
}

bool test_core_register(
    word (*reg_get)(void),
    void (*reg_set)(const word value),
    const size_t reg_offset,
    const word bitmask
) {
    int rc;
    struct registers reg_before;
    struct registers reg_after;
    byte *p_reg_before = (byte*)(&reg_before);
    byte *p_reg_after = (byte*)(&reg_after);
    
    for (dword test_value = 0x0000; test_value <= 0xFFFF; test_value++) {
        // Setup Test
        registers_reset();
        rc = registers_snapshot(&reg_before);
        ASSERT_OK(rc, "Register Snapshot Failed");

        // Perform Set and get changes
        word expected_value = test_value & bitmask;
        reg_set(test_value);
        rc = registers_snapshot(&reg_after);
        ASSERT_OK(rc, "Register Snapshot Failed");
        
        // Validate Memory is correctly set at offset passed in
        // This both validates memory layout and the offset being correct
        word set_value;
        memcpy(&set_value, p_reg_after + reg_offset, sizeof(word));
        ASSERT_EQ(set_value, expected_value,
            "Physical Memory Mismatch at offset %zu. Should have been 0x%.4X but was 0x%.4X.",
            reg_offset, expected_value, set_value
        );
            
        // Validate Getter returns correct value
        word returned_value = reg_get();
        ASSERT_EQ(returned_value, expected_value,
            "Getter Mismatch. Should have been 0x%.4X but was 0x%.4X",
            expected_value, returned_value
        );

        // Validate Isolation
        // This checks that memory outside of the expected register wasn't changed
        ASSERT_TRUE(
            validate_memory_isolation(p_reg_before, p_reg_after, reg_offset, sizeof(word)),
            "Isolation Validation Failed"
        );
    }

    return true;
}

// ==============
// Core Registers


// Virtual Registers
// =================

bool test_virtual_registers(void) {
    ASSERT_TRUE(test_virtual_register_A(), "Test Virtual Register A Failed!");
    ASSERT_TRUE(test_virtual_register_F(), "Test Virtual Register F Failed!");
    ASSERT_TRUE(test_virtual_register_B(), "Test Virtual Register B Failed!");
    ASSERT_TRUE(test_virtual_register_C(), "Test Virtual Register C Failed!");
    ASSERT_TRUE(test_virtual_register_D(), "Test Virtual Register D Failed!");
    ASSERT_TRUE(test_virtual_register_E(), "Test Virtual Register E Failed!");
    ASSERT_TRUE(test_virtual_register_H(), "Test Virtual Register H Failed!");
    ASSERT_TRUE(test_virtual_register_L(), "Test Virtual Register L Failed!");
    return true;
}

bool test_virtual_register_A(void) {
    return test_virtual_register(
        registers_get_A,
        registers_set_A,
        offsetof(struct registers, A),
        0xFF
    );
}

bool test_virtual_register_F(void) {
    return test_virtual_register(
        registers_get_F,
        registers_set_F,
        offsetof(struct registers, F),
        0xF0
    );
}

bool test_virtual_register_B(void) {
    return test_virtual_register(
        registers_get_B,
        registers_set_B,
        offsetof(struct registers, B),
        0xFF
    );
}

bool test_virtual_register_C(void) {
    return test_virtual_register(
        registers_get_C,
        registers_set_C,
        offsetof(struct registers, C),
        0xFF
    );
}

bool test_virtual_register_D(void) {
    return test_virtual_register(
        registers_get_D,
        registers_set_D,
        offsetof(struct registers, D),
        0xFF
    );
}

bool test_virtual_register_E(void) {
    return test_virtual_register(
        registers_get_E,
        registers_set_E,
        offsetof(struct registers, E),
        0xFF
    );
}

bool test_virtual_register_H(void) {
    return test_virtual_register(
        registers_get_H,
        registers_set_H,
        offsetof(struct registers, H),
        0xFF
    );
}

bool test_virtual_register_L(void) {
    return test_virtual_register(
        registers_get_L,
        registers_set_L,
        offsetof(struct registers, L),
        0xFF
    );
}

bool test_virtual_register(
    byte (*reg_get)(void),
    void(*reg_set)(const byte value),
    const size_t reg_offset,
    const byte bitmask
) {
    int rc;
    struct registers reg_before;
    struct registers reg_after;
    byte *p_reg_before = (byte*)(&reg_before);
    byte *p_reg_after = (byte*)(&reg_after);

    for (word test_value = 0x00; test_value <= 0xFF; test_value++) {
        // Setup Test
        registers_reset();
        rc = registers_snapshot(&reg_before);
        ASSERT_OK(rc, "Register Snapshot Failed");

        // Perform Set and get changes
        byte expected_value = test_value & bitmask;
        reg_set(test_value);
        rc = registers_snapshot(&reg_after);
        ASSERT_OK(rc, "Register Snapshot Failed");

        // Validate Memory is correctly set at offset passed in
        // This both validates memory layout and the offset being correct
        byte set_value = p_reg_after[reg_offset];
        ASSERT_EQ(set_value, expected_value,
            "Physical Memory Mismatch at offset %zu. Should have been 0x%.2X but was 0x%.2X",
            reg_offset, expected_value, set_value
        );
        
        // Validate Getter returns correct value
        byte returned_value = reg_get();
        ASSERT_EQ(returned_value, expected_value,
            "Getter Mismatch. Should have been 0x%.2X but was 0x%.2X",
            expected_value, returned_value
        );

        // Validate Isolation
        // This checks that memory outside of the expected register wasn't changed
        ASSERT_TRUE(
            validate_memory_isolation(p_reg_before, p_reg_after, reg_offset, sizeof(byte)),
            "Isolation Validation Failed"
        );
    }

    return true;
}

// =================
// Virtual Registers


// Composition Tests
// =================

bool test_composition(void) {
    ASSERT_TRUE(test_composition_AF(), "Test AF Composition Failed!");
    ASSERT_TRUE(test_composition_BC(), "Test BC Composition Failed!");
    ASSERT_TRUE(test_composition_DE(), "Test DE Composition Failed!");
    ASSERT_TRUE(test_composition_HL(), "Test HL Composition Failed!");
    return true;
}

bool test_composition_AF(void) {
    return test_register_composition(
        registers_set_AF,
        registers_get_AF,
        registers_set_A,
        registers_get_A,
        registers_set_F,
        registers_get_F,
        0xFFF0
    );
}

bool test_composition_BC(void) {
    return test_register_composition(
        registers_set_BC,
        registers_get_BC,
        registers_set_B,
        registers_get_B,
        registers_set_C,
        registers_get_C,
        0xFFFF
    );
}

bool test_composition_DE(void) {
    return test_register_composition(
        registers_set_DE,
        registers_get_DE,
        registers_set_D,
        registers_get_D,
        registers_set_E,
        registers_get_E,
        0xFFFF
    );
}

bool test_composition_HL(void) {
    return test_register_composition(
        registers_set_HL,
        registers_get_HL,
        registers_set_H,
        registers_get_H,
        registers_set_L,
        registers_get_L,
        0xFFFF
    );
}

bool test_register_composition(
    void (*core_reg_set)(const word value),
    word (*core_reg_get)(void),
    void (*virt_reg_hi_set)(const byte value),
    byte (*virt_reg_hi_get)(void),
    void (*virt_reg_lo_set)(const byte value),
    byte (*virt_reg_lo_get)(void),
    const word bitmask
) {
    for (dword test_value = 0x0000; test_value <= 0xFFFF; test_value++) {
        word expected_value = test_value & bitmask;
        byte expected_value_hi = (byte)(expected_value >> 8);
        byte expected_value_lo = (byte)expected_value;

        // Test Core -> Virtual
        registers_reset();
        core_reg_set(test_value);
        byte returned_value_hi = virt_reg_hi_get();
        byte returned_value_lo = virt_reg_lo_get();
        ASSERT_EQ(returned_value_hi, expected_value_hi,
            "Composition Failure. High byte mismatch. Expected 0x%.2X but was 0x%.2X",
            expected_value_hi, returned_value_hi
        );
        ASSERT_EQ(returned_value_lo, expected_value_lo,
            "Composition Failure. Low byte mismatch. Expected 0x%.2X but was 0x%.2X",
            expected_value_lo, returned_value_lo
        );

        // Test Virtual -> Core
        registers_reset();
        virt_reg_hi_set((byte)(test_value >> 8));
        virt_reg_lo_set((byte)test_value);
        word returned_value = core_reg_get();
        ASSERT_EQ(returned_value, expected_value,
            "Composition Failure. Word mismatch. Expected 0x%.4X but was 0x%.4X",
            expected_value, returned_value
        );
    }

    return true;
}

// =================
// Composition Tests


// Flags
// =====

bool test_flags(void) {
    ASSERT_TRUE(test_flag_z(), "Test Flag Z (Zero) Failed!");
    ASSERT_TRUE(test_flag_n(), "Test Flag N (BCD Subtraction) Failed!");
    ASSERT_TRUE(test_flag_h(), "Test Flag H (BCD Half Carry) Failed!");
    ASSERT_TRUE(test_flag_c(), "Test Flag C (Carry) Failed!");
    return true;
}

bool test_flag_z(void) {
    return test_flag(
        registers_get_flag_z,
        registers_set_flag_z,
        registers_clear_flag_z,
        7
    );
}


bool test_flag_n(void) {
    return test_flag(
        registers_get_flag_n,
        registers_set_flag_n,
        registers_clear_flag_n,
        6
    );
}

bool test_flag_h(void) {
    return test_flag(
        registers_get_flag_h,
        registers_set_flag_h,
        registers_clear_flag_h,
        5
    );
}

bool test_flag_c(void) {
    return test_flag(
        registers_get_flag_c,
        registers_set_flag_c,
        registers_clear_flag_c,
        4
    );
}

bool test_flag(
    bool (*flag_get)(),
    void (*flag_set)(),
    void (*flag_clear)(),
    const int bit_index
) {
    int rc;
    struct registers reg_before;
    struct registers reg_after;
    byte *p_reg_before = (byte*)(&reg_before);
    byte *p_reg_after = (byte*)(&reg_after);
    byte bitmask = (1 << bit_index);
    byte inverted_bitmask = ~(bitmask | 0x0F);
    size_t reg_offset = offsetof(struct registers, F);

    // Corner 1: Set while F = 0x00 (Clean Set)
    // ----------------------------------------
    // Setup Test
    registers_reset();
    rc = registers_snapshot(&reg_before);
    ASSERT_OK(rc, "Register Snapshot Failed");

    // Perform Set
    flag_set();
    rc = registers_snapshot(&reg_after);
    ASSERT_OK(rc, "Register Snapshot Failed");
    
    // Validate Getter
    ASSERT_TRUE(flag_get(), "Getter Mismatch. Should have been set but was not set");

    // Validate Register F
    ASSERT_EQ(reg_after.F, bitmask,
        "Register F Corruption. Should have been 0x%.2X but was 0x%.2X",
        bitmask, reg_after.F
    );

    // Validate Isolation
    // This checks that memory outside of the expected register wasn't changed
    ASSERT_TRUE(
        validate_memory_isolation(p_reg_before, p_reg_after, reg_offset, sizeof(byte)),
        "Isolation Validation Failed"
    );
    // ----------------------------------------

    // Corner 2: Set while F = bitmask (Clean Clear)
    // ---------------------------------------------
    // Setup Test
    rc = registers_snapshot(&reg_before);
    ASSERT_OK(rc, "Register Snapshot Failed");

    // Perform Clear
    flag_clear();
    rc = registers_snapshot(&reg_after);
    ASSERT_OK(rc, "Register Snapshot Failed");
    
    // Validate Getter
    ASSERT_FALSE(flag_get(), "Getter Mismatch. Should have been not set but was set");

    // Validate Register F
    ASSERT_EQ(reg_after.F, 0x00,
        "Register F Corruption. Should have been 0x00 but was 0x%.2X",
        reg_after.F
    );

    // Validate Isolation
    // This checks that memory outside of the expected register wasn't changed
    ASSERT_TRUE(
        validate_memory_isolation(p_reg_before, p_reg_after, reg_offset, sizeof(byte)),
        "Isolation Validation Failed"
    );
    // ---------------------------------------------

    // Corner 3: Set while F = inverted_bitmask (Crowed Set)
    // -----------------------------------------------------
    // Setup Test
    registers_set_F(inverted_bitmask);
    rc = registers_snapshot(&reg_before);
    ASSERT_OK(rc, "Register Snapshot Failed");

    // Perform Set
    flag_set();
    rc = registers_snapshot(&reg_after);
    ASSERT_OK(rc, "Register Snapshot Failed");
    
    // Validate Getter
    ASSERT_TRUE(flag_get(), "Getter Mismatch. Should have been set but was not set");

    // Validate Register F
    ASSERT_EQ(reg_after.F, 0xF0,
        "Register F Corruption. Should have been 0xF0 but was 0x%.2X",
        reg_after.F
    );

    // Validate Isolation
    // This checks that memory outside of the expected register wasn't changed
    ASSERT_TRUE(
        validate_memory_isolation(p_reg_before, p_reg_after, reg_offset, sizeof(byte)),
        "Isolation Validation Failed"
    );
    // -----------------------------------------------------

    // Corner 4: Clear while F = 0xF0 (Crowed Clear)
    // ---------------------------------------------
    // Setup Test
    rc = registers_snapshot(&reg_before);
    ASSERT_OK(rc, "Register Snapshot Failed");

    // Perform Clear
    flag_clear();
    rc = registers_snapshot(&reg_after);
    ASSERT_OK(rc, "Register Snapshot Failed");
    
    // Validate Getter
    ASSERT_FALSE(flag_get(), "Getter Mismatch. Should have been not set but was set");

    // Validate Register F
    ASSERT_EQ(reg_after.F, inverted_bitmask,
        "Register F Corruption. Should have been 0x%.2X but was 0x%.2X",
        inverted_bitmask, reg_after.F
    );

    // Validate Isolation
    // This checks that memory outside of the expected register wasn't changed
    ASSERT_TRUE(
        validate_memory_isolation(p_reg_before, p_reg_after, reg_offset, sizeof(byte)),
        "Isolation Validation Failed"
    );
    // ---------------------------------------------

    return true;
}

// =====
// Flags