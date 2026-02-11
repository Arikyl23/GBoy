/**
 * @file internal_manipulation.h
 * @brief Internal Header containing internal string manipulation operations.
 */
#pragma once

#include <stdlib.h>

#include "string/core.h"

bool string_requires_rebase(const struct string* dest, const struct string* src);
