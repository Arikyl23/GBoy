/**
 * @file cpu.h
 * @brief Header for the Gameboy's CPU.
 */
#pragma once

#include "cpu/registers.h"

/**
 * @brief Executes the next CPU instruction.
 * @returns Status of the CPU. 0 if okay; otherwise, non-zero.
 */
int cpu_execute(void);

/**
 * @brief Snapshot the current CPU registers.
 * @returns A copy of the CPU registers.
 */
struct registers cpu_snapshot_registers(void);