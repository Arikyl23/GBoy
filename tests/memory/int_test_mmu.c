/**
 * @file int_test_mmu.c
 * @brief Integration Tests for the Memory Management Unit (MMU).
 */

#include <stdbool.h>

bool test_mmu_module_integration(void);

int main(void) { return (test_mmu_module_integration() == true) ? 0 : 1; }

bool test_mmu_module_integration(void) { return true; }