/**
 * @file mbc.h
 */
#pragma once

struct mbc {                       /** @brief Memory Bank Controller.*/
    const struct mbc_iface* iface; /** @brief Static Interace defining MBC functionality. */
    void*                   ctx;   /** @brief Opaque pointer that holds the MBC context. */
};