/**
 * @file mbc.h
 * @brief Contains the definition of the Memory Bank Controller.
 */
#pragma once

#include "DEFINES.h"

struct mbc {                       /** @brief Memory Bank Controller.*/
    const struct mbc_iface* iface; /** @brief Static Interace defining MBC functionality. */
    void*                   ctx;   /** @brief Opaque pointer that holds the MBC context. */
};

/**
 * @brief Sets up a MBC with a new context.
 * @param mbc MBC to setup. Must not be NULL.
 * @returns An integer indicating success:
 * @returns - 0: Success
 * @returns - 1: Failure
 * @note This function automatically allocates the necessary heap space needed for the MBC.
 */
int  mbc_setup(struct mbc* mbc);
/**
 * @brief Cleans up the resources allocated by an MBC context.
 * @param mbc MBC to cleanup.
 * @note Guranteed to not fail.
 */
void mbc_cleanup(struct mbc* mbc);
