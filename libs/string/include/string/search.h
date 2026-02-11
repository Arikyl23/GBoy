/**
 * @file search.h
 * @brief Public Header containing search and predicate operations.
 */
#pragma once

#include <stdbool.h>

#include "string/core.h"

// ========== Predicates ==========
bool string_is_empty(const struct string* str);
bool string_is_space(const struct string* str);
bool string_is_alphabetic(const struct string* str);
bool string_is_alphanumeric(const struct string* str);
bool string_is_numeric(const struct string* str);
bool string_is_numeric_hex(const struct string* str);
bool string_is_puncuation(const struct string* str);
bool string_is_graphical(const struct string* str);
bool string_is_printable(const struct string* str);
bool string_is_control(const struct string* str);
bool string_is_lower(const struct string* str);
bool string_is_upper(const struct string* str);
bool string_is_equal(const struct string* strA, const struct string* strB);
// ========== Predicates ==========
