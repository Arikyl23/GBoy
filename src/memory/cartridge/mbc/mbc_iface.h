/**
 *
 */
#pragma once

#include "DEFINES.h"

struct mbc_iface {
    byte (*read)(void* ctx, const word addr);
    void (*write)(void* ctx, const word addr, const byte value);
    void (*destroy)(void** ctx);
};
