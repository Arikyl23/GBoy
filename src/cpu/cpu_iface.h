#pragma once

typedef struct {
    void (*execute)();
} cpu_iface_t;