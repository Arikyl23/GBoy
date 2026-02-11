/**
 * @file search.c
 * @brief Implementation file for the search module.
 */
#include "string/search.h"

#include <SDL3/SDL_stdinc.h>

#include "internal_core.h"

// ========== Predicates ==========
static inline bool predicate_foreach(const char* cstr, const size_t len, int (*predicate)(int)) {
    if (len == 0) { return false; }
    for (size_t i = 0; i < len; i++) {
        if (predicate(cstr[i]) == 0) { return false; }
    }
    return true;
}

bool string_is_empty(const struct string* str) { return (str == NULL || str->length == 0); }

bool string_is_space(const struct string* str) {
    return (str != NULL && predicate_foreach(string_cstr(str), str->length, SDL_isspace));
}

bool string_is_alphabetic(const struct string* str) {
    return (str != NULL && predicate_foreach(string_cstr(str), str->length, SDL_isalpha));
}

bool string_is_alphanumeric(const struct string* str) {
    return (str != NULL && predicate_foreach(string_cstr(str), str->length, SDL_isalnum));
}

bool string_is_numeric(const struct string* str) {
    return (str != NULL && predicate_foreach(string_cstr(str), str->length, SDL_isdigit));
}

bool string_is_numeric_hex(const struct string* str) {
    return (str != NULL && predicate_foreach(string_cstr(str), str->length, SDL_isxdigit));
}

bool string_is_puncuation(const struct string* str) {
    return (str != NULL && predicate_foreach(string_cstr(str), str->length, SDL_ispunct));
}

bool string_is_graphical(const struct string* str) {
    return (str != NULL && predicate_foreach(string_cstr(str), str->length, SDL_isgraph));
}

bool string_is_printable(const struct string* str) {
    return (str != NULL && predicate_foreach(string_cstr(str), str->length, SDL_isprint));
}

bool string_is_control(const struct string* str) {
    return (str != NULL && predicate_foreach(string_cstr(str), str->length, SDL_iscntrl));
}

bool string_is_lower(const struct string* str) {
    return (str != NULL && predicate_foreach(string_cstr(str), str->length, SDL_islower));
}

bool string_is_upper(const struct string* str) {
    return (str != NULL && predicate_foreach(string_cstr(str), str->length, SDL_isupper));
}

bool string_is_equal(const struct string* strA, const struct string* strB) {
    if (strA == NULL && strB == NULL) { return true; }
    if ((strA == NULL && strB != NULL) || (strA != NULL && strB == NULL)) { return false; }
    if (strA->length != strB->length) { return false; }
    return (SDL_memcmp(string_cstr(strA), string_cstr(strB), strA->length) == 0);
}
// ========== Predicates ==========
