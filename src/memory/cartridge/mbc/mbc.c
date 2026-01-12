/**
 * @file mbc.c
 * @brief Contains the implementation of the Memory Bank Controller.
 */
#include "memory/cartridge/mbc/mbc.h"

#include <log.h>
#include <stddef.h>

#include "memory/cartridge/mbc/dld.h"
#include "memory/cartridge/mbc/mbc_iface.h"

/**
 * @brief Resets the internal MBC data to NULL
 * @param mbc MBC to reset.
 * @warning This should only be called when MBC is not pointing at allocated data. Ensure data is
 * freed before using this function.
 */
static void reset_mbc(struct mbc* mbc) {
    mbc->iface = NULL;
    mbc->ctx   = NULL;
    return;
}

int mbc_setup(struct mbc* mbc) {
    int rc;

    if (mbc == NULL) {
        log_error("Cannot setup a NULL MBC!");
        return -1;
    }

    reset_mbc(mbc);
    mbc->ctx = dld_instanciate(1);
    if (mbc->ctx == NULL) {
        log_error("Failed to setup MBC");
        return -1;
    }

    return 0;
}

void mbc_cleanup(struct mbc* mbc) {
    int rc;

    if (mbc == NULL) {
        log_warn("No MBC to cleanup (MBC was NULL)");
        return;
    }

    if (mbc->iface == NULL || mbc->ctx == NULL) {
        log_warn("MBC is already empty");
        return;
    }

    mbc->iface->destroy(mbc->ctx);
    reset_mbc(mbc);

    return;
}
