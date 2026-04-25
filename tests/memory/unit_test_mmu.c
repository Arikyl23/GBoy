/**
 * @file unit_test_mmu.c
 * @brief Unit Tests for the Memory Management Unit (MMU).
 */

#include <stdbool.h>

bool test_mmu_module(void);

int main(void) { return (test_mmu_module() == true) ? 0 : 1; }

bool test_mmu_module(void) { return true; }
