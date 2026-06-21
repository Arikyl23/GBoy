/**
 * @file debugger.h
 * @brief Public API for the GBoy Debugger.
 */
#pragma once

#include <stdbool.h>

bool debugger_is_open(void);
int  debugger_open(void);
void debugger_close(void);
