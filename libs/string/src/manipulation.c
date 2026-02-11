/**
 * @file manipulation.c
 * @brief Implementation for string manipulation operations.
 */
#include "string/manipulation.h"

#include <SDL3/SDL_stdinc.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "internal_core.h"
#include "internal_manipulation.h"
#include "string/core.h"
#include "string/string_status.h"

// ----- Public -----
struct string* string_append(struct string* str, const struct string* suffix) {
    if (STRING_OBJ_CHECK(str) == false) { return str; }
    if (STRING_OBJ_CHECK(suffix) == false) {
        if (suffix == NULL) {
            string_set_status(str, STRING_ENULL);
        } else {
            string_set_status(str, STRING_EINVAL);
        }
        return str;
    }

    // Nothing to append?
    if (suffix->length == 0) { return str; }

    // Overlapping?
    bool requires_rebase = string_requires_rebase(str, suffix);

    // Calculate boundaries
    char*           str_cstr    = string_cstr_mutable(str);
    const char*     suffix_cstr = string_cstr(suffix);
    const size_t    old_length  = str->length;
    const size_t    new_length  = old_length + suffix->length;
    const ptrdiff_t offset      = (requires_rebase == true) ? suffix_cstr - str_cstr : 0;

    // Ensure capacity
    // May cause realloc (refind pointers)
    if (string_is_error(string_resize(str, new_length + 1)) == true) { return str; }
    str_cstr = string_cstr_mutable(str);

    // Peform appendment
    if (requires_rebase == true) {
        SDL_memmove(str_cstr + old_length, str_cstr + offset, suffix->length);
    } else {
        SDL_memcpy(str_cstr + old_length, suffix_cstr, suffix->length);
    }
    str_cstr[new_length] = '\0';
    str->length          = new_length;

    return str;
}

struct string* string_append_cstr(struct string* str, const char* suffix) {
    if (STRING_OBJ_CHECK(str) == false) { return str; }

    // No appendment todo
    if (suffix == NULL) { return str; }

    struct string tmp = {
        .internal.view.p_origin = suffix,
        .internal.view.p_data   = suffix,
        .length                 = SDL_strlen(suffix),
        .status                 = STRING_OK,
        .type                   = STR_TYP_VIEW,
    };

    // No appendment todo
    if (tmp.length == 0) { return str; }

    return string_append(str, &tmp);
}

struct string* string_append_char(struct string* str, const char c) {
    if (STRING_OBJ_CHECK(str) == false) { return str; }

    if (string_is_error(string_resize(str, str->length + 2)) == true) { return str; }

    char* cstr        = string_cstr_mutable(str);
    cstr[str->length] = c;
    str->length++;
    cstr[str->length] = '\0';

    return str;
}

struct string* string_prepend(struct string* str, const struct string* prefix) {
    if (STRING_OBJ_CHECK(str) == false) { return str; }
    if (STRING_OBJ_CHECK(prefix) == false) {
        if (prefix == NULL) {
            string_set_status(str, STRING_ENULL);
        } else {
            string_set_status(str, STRING_EINVAL);
        }
        return str;
    }

    // Nothing to prepend?
    if (prefix->length == 0) { return str; }

    // Overlapping?
    bool requires_rebase = string_requires_rebase(str, prefix);

    // Calculate boundaries
    char*           str_cstr    = string_cstr_mutable(str);
    const char*     prefix_cstr = string_cstr(prefix);
    const size_t    old_length  = str->length;
    const size_t    new_length  = old_length + prefix->length;
    const ptrdiff_t offset      = (requires_rebase == true) ? prefix_cstr - str_cstr : 0;

    // Ensure capacity
    // May cause realloc (refind pointer)
    if (string_is_error(string_resize(str, new_length + 1)) == true) { return str; }
    str_cstr = string_cstr_mutable(str);

    // Shift original data over first then perform prependment
    SDL_memmove(str_cstr + prefix->length, str_cstr, old_length);
    if (requires_rebase == true) {
        SDL_memmove(str_cstr, str_cstr + prefix->length + offset, prefix->length);
    } else {
        SDL_memcpy(str_cstr, prefix_cstr, prefix->length);
    }
    str_cstr[new_length] = '\0';
    str->length          = new_length;

    return str;
}

struct string* string_prepend_cstr(struct string* str, const char* prefix) {
    if (STRING_OBJ_CHECK(str) == false) { return str; }

    // No prependment todo
    if (prefix == NULL) { return str; }

    struct string tmp = {
        .internal.view.p_origin = prefix,
        .internal.view.p_data   = prefix,
        .length                 = SDL_strlen(prefix),
        .status                 = STRING_OK,
        .type                   = STR_TYP_VIEW,
    };

    // No prependment todo
    if (tmp.length == 0) { return str; }

    return string_prepend(str, &tmp);
}

struct string* string_prepend_char(struct string* str, const char c) {
    if (STRING_OBJ_CHECK(str) == false) { return str; }

    if (string_is_error(string_resize(str, str->length + 2)) == true) { return str; }

    char* cstr = string_cstr_mutable(str);
    SDL_memmove(cstr + 1, cstr, str->length);
    cstr[0] = c;
    str->length++;
    cstr[str->length] = '\0';

    return str;
}

struct string* string_concat(
    struct string*       dest,
    const struct string* lhs,
    const struct string* rhs
) {
    if (STRING_OBJ_CHECK(dest) == false) { return dest; }
    if (STRING_OBJ_CHECK(lhs) == false || STRING_OBJ_CHECK(rhs) == false) {
        if (lhs == NULL || rhs == NULL) {
            string_set_status(dest, STRING_ENULL);
        } else {
            string_set_status(dest, STRING_EINVAL);
        }
        return dest;
    }

    // If dest and lhs are ever the same object (regardless of rhs)
    // The operation simplifies to just an append
    if (lhs == dest) { return string_append(dest, rhs); }

    // ----- After this (dest != lhs) -----

    // If dest != lhs but dest == rhs
    // Operation simplifies to prepend
    if (dest == rhs) { return string_prepend(dest, lhs); }

    // ----- After this, all arguments are unique -----

    // Overlapping?
    const bool lhs_requires_rebase = string_requires_rebase(dest, lhs);
    const bool rhs_requires_rebase = string_requires_rebase(dest, rhs);

    // Calculate Boundaries
    char*           dest_cstr  = string_cstr_mutable(dest);
    const char*     lhs_cstr   = string_cstr(lhs);
    const char*     rhs_cstr   = string_cstr(rhs);
    const size_t    new_length = lhs->length + rhs->length;
    const ptrdiff_t lhs_offset = (lhs_requires_rebase == true) ? lhs_cstr - dest_cstr : 0;
    const ptrdiff_t rhs_offset = (rhs_requires_rebase == true) ? rhs_cstr - dest_cstr : 0;

    // Ensure capacity
    // May cause realloc (refind pointer)
    if (string_is_error(string_resize(dest, new_length + 1)) == true) { return dest; }
    dest_cstr = string_cstr_mutable(dest);

    // 3 Possible Cases:
    // - Dual Overlap (Both LHS and RHS are substrs of DEST)
    //   - This requries a temporary buffer
    // - Single Overlap
    //   - Move the overlap data first, then copy other data
    // - No Overlap
    //   - copy both

    // Dual overlap case requires temporary buffer
    if (lhs_requires_rebase == true && rhs_requires_rebase == true) {
        if (lhs->length < rhs->length) {
            char* p_data_lhs = SDL_malloc(lhs->length);
            if (p_data_lhs == NULL) {
                string_set_status(dest, STRING_ENOMEM);
                return dest;
            }
            SDL_memcpy(p_data_lhs, dest_cstr + lhs_offset, lhs->length);
            SDL_memmove(dest_cstr + lhs->length, dest_cstr + rhs_offset, rhs->length);
            SDL_memcpy(dest_cstr, p_data_lhs, lhs->length);
            SDL_free(p_data_lhs);
        } else {
            char* p_data_rhs = SDL_malloc(rhs->length);
            if (p_data_rhs == NULL) {
                string_set_status(dest, STRING_ENOMEM);
                return dest;
            }
            SDL_memcpy(p_data_rhs, dest_cstr + rhs_offset, rhs->length);
            SDL_memmove(dest_cstr, dest_cstr + lhs_offset, lhs->length);
            SDL_memcpy(dest_cstr + lhs->length, p_data_rhs, rhs->length);
            SDL_free(p_data_rhs);
        }
    } else if (lhs_requires_rebase == true) {
        SDL_memmove(dest_cstr, dest_cstr + lhs_offset, lhs->length);
        SDL_memcpy(dest_cstr + lhs->length, rhs_cstr, rhs->length);
    } else if (rhs_requires_rebase == true) {
        SDL_memmove(dest_cstr + lhs->length, dest_cstr + rhs_offset, rhs->length);
        SDL_memcpy(dest_cstr, lhs_cstr, lhs->length);
    } else {
        SDL_memcpy(dest_cstr, lhs_cstr, lhs->length);
        SDL_memcpy(dest_cstr + lhs->length, rhs_cstr, rhs->length);
    }
    dest_cstr[new_length] = '\0';
    dest->length          = new_length;

    return dest;
}
// ----- Public -----

// ----- Private -----
bool string_requires_rebase(const struct string* dest, const struct string* src) {
    if (!(string_is_solid(dest) == true && src->type == STR_TYP_VIEW)) { return false; }
    // dest is SOLID and src is VIEW

    // Does src originate from dest?
    if (string_cstr(dest) == src->internal.view.p_origin) { return true; }

    // Potential user malformed VIEW (origin was not correctly tracked)
    // Double check boundaries don't actually overlap
    return string_is_overlapping_cstr(
        string_cstr(dest),
        dest->length,
        src->internal.view.p_data,
        src->length
    );
}
// ----- Private -----
