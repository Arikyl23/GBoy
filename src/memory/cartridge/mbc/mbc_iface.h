/**
 * @file mbc_iface.h
 * @brief Defines the Memory Bank Controller interface that all implementations must conform to.
 */
#pragma once

#include "DEFINES.h"

/** @brief Memory Bank Controller Interface. */
struct mbc_iface {
    /**
     * @brief Performs a read on the MBC implmentation.
     * @param ctx Pointer to the MBC implmentations context structure.
     * @param addr Address to read from the MBC implmentation.
     * @returns Value read from given address. If an attempt to read a bad address is made, 0xFF is
     * returned instead.
     */
    byte (*read)(void* ctx, const word addr);
    /**
     * @brief Performs a write on the MBC implementation.
     * @param ctx Pointer to the MBC implmentations context structure.
     * @param addr Address to read from the MBC implmentation.
     * @param value Value to write to the MBC implmentation at the given address.
     */
    void (*write)(void* ctx, const word addr, const byte value);
    /**
     * @brief Destroys the resources allocated by the MBC implementation.
     * @param p_ctx Pointer to a pointer of the MBC implementation context.
     * @note The MBC implementation context should automatically be set to NULL after this function.
     */
    void (*destroy)(void** p_ctx);
};
