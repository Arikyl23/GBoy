/**
 * @file mmu.h
 * @brief Memory Management Unit (MMU). It is responsible for handling
 * all read/write requests on the Address Bus.
 */
#pragma once

#include "DEFINES.h"
#include "memory/bus.h"

byte mmu_read(enum bus source_bus, const word addr);
void mmu_write(enum bus source_bus, const word addr, const byte value);
