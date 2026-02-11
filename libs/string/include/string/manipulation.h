/**
 * @file manipulation.h
 * @brief Public header containing string manipulation operations.
 */
#pragma once

#include <stdlib.h>

#include "string/core.h"

/**
 * @brief Appends a string to the end of another.
 * @param str String object to append `suffix` to.
 * @param suffix String object to append.
 * @returns `str` string object with changes made to it. Object's status flag will be non-zero in
 * the event of an error.
 * @note Self-assignment is safe
 */
struct string* string_append(struct string* str, const struct string* suffix);
/**
 * @brief Appends a string to the end of another.
 * @param str String object to append `suffix` to.
 * @param suffix CString to append.
 * @returns `str` string object with changes made to it. Object's status flag will be non-zero in
 * the event of an error.
 * @note Self-assignment is safe
 */
struct string* string_append_cstr(struct string* str, const char* suffix);
/**
 * @brief Appends a character to the end of a string.
 * @param str String object to append character to.
 * @param c Character to append.
 * @returns `str` string object with changes made to it. Object's status flag will be non-zero in
 * the event of an error.
 */
struct string* string_append_char(struct string* str, const char c);
/**
 * @brief Prepends a string to the start of another.
 * @param str String object to prepend `prefix` to.
 * @param prefix String object to prepend.
 * @returns `str` string object with changes made to it. Object's status flag will be non-zero in
 * the event of an error.
 * @note Self-assignment is safe
 */
struct string* string_prepend(struct string* str, const struct string* prefix);
/**
 * @brief Prepends a string to the start of another.
 * @param str String object to prepend `prefix` to.
 * @param prefix CString to prepend.
 * @returns `str` string object with changes made to it. Object's status flag will be non-zero in
 * the event of an error.
 * @note Self-assignment is safe
 */
struct string* string_prepend_cstr(struct string* str, const char* prefix);
/**
 * @brief Prepends a character to the end of a string.
 * @param str String object to prepend character to.
 * @param c Character to prepend.
 * @returns `str` string object with changes made to it. Object's status flag will be non-zero in
 * the event of an error.
 * @note Self-assignment is safe
 */
struct string* string_prepend_char(struct string* str, const char c);
/**
 * @brief Concatenates `lhs` and `rhs` storing the result in `dest`.
 * @param dest Destination string object. The previous value is overwritten.
 * @param lhs Left hand side string object. If this is the same as `dest`, it will append `rhs` to
 * `dest`.
 * @param rhs Right hand size string object. If this is the same as `dest`, it will prepend `lhs` to
 * `dest`.
 * @returns `dest` string object with changes made to it. Object's status flag will be non-zero in
 * the event of an error.
 * @note If `lhs` and `rhs` match, the effect is `lhs` being appended to itself.
 */
struct string* string_concat(
    struct string*       dest,
    const struct string* lhs,
    const struct string* rhs
);
/**
 * @brief Concatenates `lhs` and `rhs` storing the result in `dest`.
 * @param dest Destination string object. The previous value is overwritten.
 * @param lhs Left hand side cstring. If this is the same internal cstring as `dest`, it will append
 * `rhs` to `dest`.
 * @param rhs Right hand size cstring. If this is the same internal cstring as `dest`, it will
 * prepend `lhs` to `dest`.
 * @returns `dest` string object with changes made to it. Object's status flag will be non-zero in
 * the event of an error.
 * @note If `lhs` and `rhs` match, the effect is `lhs` being appended to itself.
 */
struct string* string_concat_cstr(struct string* dest, const char* lhs, const char* rhs);
