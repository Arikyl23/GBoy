/**
 * @file internal_core.h
 * @brief Internal Header containing core string definition and util
 */
#pragma once

#include <stdint.h>

#include "string/string_status.h"
#include "string_type.h"

/** @brief Internal Heap Structure. */
struct string_internal_heap {
    /**
     * @brief cstring buffer. Storing as a cstring ensures safety and allows easy interfacing
     * with std libs.
     */
    char*  p_data;
    /** @brief Current allocation capacity. */
    size_t capacity;
};

/** @brief Internal View Structure. */
struct string_internal_view {
    /** @brief Pointer to VIEW cstring. This is the window the VIEW represents. */
    const char* p_data;
    /** @brief Pointer to the start of the buffer the VIEW is referencing. */
    const char* p_origin;
};

/** @brief Temporary Type that is not actually used. Helps define SSO size. */
union string_internal_storage {
    struct string_internal_heap heap;
    struct string_internal_view view;
};

#define STRING_SSO_CAPACITY sizeof(union string_internal_storage)
/** @brief Internal SSO Structure. */
struct string_internal_sso {
    /** @brief Static cstring array. */
    char data[STRING_SSO_CAPACITY];
};

/** @brief String structure object for holding strings. */
struct string {
    /** @brief Length of the string excluding the NULL terminator. */
    size_t             length;
    /** @brief Internal data type. Tracks which union member for `data` is currently in use. */
    enum string_type   type;
    /** @brief String object status. If a value other than 0, a warning or error occured during the
     * last operation. */
    enum string_status status;

    /** @brief Internal data union. Check `type` before accessing this field. */
    union {
        /** @brief Heap Allocated String. */
        struct string_internal_heap heap;
        /** @brief View (Reference) String */
        struct string_internal_view view;
        /** @brief Static Short String. */
        struct string_internal_sso  sso;
    } internal;
};

/** @brief Validates a string object. */
#define STRING_OBJ_CHECK(str_obj) (str_obj != NULL && string_is_error(str_obj) == false)

// ========== Constructor ==========
/**
 * @brief Creates a new string object handle.
 * @returns Handle to a new string object. `NULL` if memory allocation failed.
 */
struct string* string_alloc_head(void);
/**
 * @brief Creates a new SSO string object from a cstring.
 * @param cstr C-string to copy into string object.
 * @param len Length of `cstr`.
 * @returns Valid string object with str copied into it. `NULL` if new handle could not be created.
 */
struct string* string_init_sso(const char* cstr, const size_t len);
/**
 * @brief Creates a new HEAP string object from a cstring.
 * @param cstr CString to copy into string object.
 * @param len Length of `str`.
 * @returns Valid string object with str copied into it. `NULL` if new handle could not be created.
 * If handle is created but memory allocation fails, empty SSO string is returned with non-zero
 * status.
 */
struct string* string_init_heap(const char* cstr, const size_t len);
/**
 * @brief Creates a new VIEW string object from a cstring.
 * @param origin Originating buffer.
 * @param start Start of VIEW window into `origin`.
 * @param len Length of VIEW window (effectively the length of `start`).
 * @returns String view object with a reference to the original cstring. `NULL` if new handle could
 * not be created.
 * @warning View string objects are temporary readonly references. If the original source string
 * mutates, this should be considered stale. Any attempt to mutate a VIEW will automatically trigger
 * a copy-on-write converting the VIEW into a normal fully solid string.
 */
struct string* string_init_view(const char* origin, const char* start, const size_t len);
// ========== Constructor ==========

// ========== Deconstructor ==========
/**
 * @brief Resets the string object setting it back to an empty string.
 * @param str String Object to reset.
 * @returns The resulting string object.
 * @warning This zeros out the struct and does not free allocation. Use `string_clear()` or
 * `string_free()` to free managed memory.
 */
struct string* string_reset(struct string* str);
// ========== Deconstructor ==========

// ========== Properties ==========
/**
 * @brief Attempts to set the status of a string.
 * @param str String object to set status of.
 * @note Overwrites error using the following logic:
 * @note - WARNING can only overwrite OK
 * @note - ERROR can only overwrite OK and WARNING
 * @note - Nothing can overwrite an ERROR
 */
void  string_set_status(struct string* str, const enum string_status status);
/**
 * @brief Fetches a mutable buffer from the string object.
 * @param str String Object to get mutable buffer for.
 * @returns Temporary reference to internal cstring.
 * @warning This function returns `NULL` when called on VIEW type string objects.
 */
char* string_cstr_mutable(struct string* str);
/**
 * @brief Gets a mutable buffer with an offset applied from the string object.
 * @param str String object to get cstring from.
 * @param index Index to start cstring at.
 * @returns Temporary reference to internal cstring.
 * @warning This function returns `NULL` when called on VIEW type string objects.
 */
char* string_cstr_at_mutable(struct string* str, const size_t index);
// ========== Properties ==========

// ========== Utility ==========
/**
 * @brief Determines if a string object is a SOLID type
 * @param str String object to check.
 * @returns `true` if object is SOLID; otherwise, `false`
 * @note Generally a non-solid type is a VIEW type.
 */
bool string_is_solid(const struct string* str);
/**
 * @brief Determines if two cstrings are overlapping
 * @param str1 First cstring.
 * @param len1 Length of `str1`.
 * @param str2 Second cstring.
 * @param len2 Length of `str2`.
 * @returns `true` if buffers overlap; otherwise, false.
 */
bool string_is_overlapping_cstr(
    const char*  str1,
    const size_t len1,
    const char*  str2,
    const size_t len2
);
// ========== Utility ==========
