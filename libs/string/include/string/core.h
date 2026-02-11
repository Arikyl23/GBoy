/**
 * @file core.h
 * @brief Public header containing the core functionality for strings.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "string/string_status.h"

struct string;

// ========== Constructor ==========
/**
 * @brief Creates a new String Object by deep copying the passed cstring.
 * @param cstr C-string to copy. `NULL` returns an empty string object.
 * @returns New String Object. `NULL` when handle creation fails. Empty with negative status when
 * assignment fails.
 */
struct string* string_create(const char* cstr);
/**
 * @brief Adopts a already allocated SDL3 cstring.
 * @param sdl_cstr SDL3 cstring to take ownership of.
 * @returns New String Object. `NULL` when handle creation fails. Empty with negative status when
 * assignment fails.
 */
struct string* string_create_adopt(const char* sdl_cstr);
/**
 * @brief Creates a new String Object from another.
 * @param str String Object to copy. `NULL` returns an empty string object.
 * @returns New String Object. `NULL` when handle creation fails. Empty with negative status when
 * assignment fails.
 */
struct string* string_create_copy(const struct string* str);
/**
 * @brief Creates a String Object that "views" another.
 * @param str String Object to copy. `NULL` returns a normal string object that is empty.
 * @returns New String Object.
 * @note Views, unlike normal string objects do not need to be freed since they don't own any
 * memory. It is still safe and good practice to call `string_free()` on them.
 * @warning View string objects are temporary readonly references. If the original source string
 * mutates, this should be considered stale. Any attempt to mutate a VIEW will automatically trigger
 * a copy-on-write converting the VIEW into a normal fully solid string.
 */
struct string* string_create_view(const struct string* str);
/**
 * @brief Creates a new VIEW string object from a cstring.
 * @param origin Originating buffer.
 * @param start Start of VIEW window into `origin`.
 * @returns New String Object.
 * @note Views, unlike normal string objects do not need to be freed since they don't own any
 * memory. It is still safer and good practice to call `string_free()` on them.
 * @warning View string objects are temporary readonly references. If the original source string
 * mutates, this should be considered stale. Any attempt to mutate a VIEW will automatically trigger
 * a copy-on-write converting the VIEW into a normal fully solid string.
 */
struct string* string_create_view_cstr(const char* origin, const char* start);
/**
 * @brief Creates a new String Object with the specified capacity. Content is set to an empty
 * string.
 * @param capacity Required Capacity.
 * @returns New String Object.
 */
struct string* string_create_reserve(const size_t capacity);
// ========== Constructor ==========

// ========== Deconstructor ==========
/**
 * @brief Clears the string object setting it back to an empty string.
 * @param str String Object to clear.
 * @returns The resulting string object.
 * @note Semantically the same as `string_free()` except it returns the emptied string afterwards
 * allowing operation channing.
 */
struct string* string_clear(struct string* str);
/**
 * @brief Cleans up memory used by String Object and resets it to a uninitialized state. Safe to
 * call on already freed objects.
 * @param p_str String Object to clean up.
 */
void           string_free(struct string** p_str);
// ========== Deconstructor ==========

// ========== Memory ==========
/**
 * @brief Ensures String Object has reserved at least up to the requested capacity.
 * @param str String Object to reserve additional memory for.
 * @param capacity Requested capacity. Will only allocate if current capacity is smaller than this.
 * @returns Resulting String Object.
 * @note If a allocation is required, will attempt to reserve the exact requested amount. If
 * multiple resizes are being done, it is better to call `string_resize()`.
 */
struct string* string_reserve(struct string* str, const size_t capacity);
/**
 * @brief Ensures String Object has enough capacity to fit the requested size.
 * @param str String Object to resize.
 * @param size Requested minimum size. Will grow allocation to fit this size.
 * @returns Resulting String Object.
 * @note If an allocation is require, will use a growth factor first. If this fails, will reserve
 * the exact requested amount. If the exact allocation size is already known. It is better to call
 * `string_reserve()` to reserve that exact size once.
 */
struct string* string_resize(struct string* str, const size_t size);
/**
 * @brief Shrinks the String Object by releasing unused memory up to the length of the stored
 * string.
 * @param str String Object to shrink.
 * @returns Resulting String Object.
 */
struct string* string_shrink_to_fit(struct string* str);
/**
 * @brief Solidifies a VIEW type String Object.
 * @param view_str VIEW type String Object.
 * @returns Resulting String Object.
 * @note Promotes a VIEW into either an SSO and HEAP depending on length of contents referenced.
 */
struct string* string_solidify(struct string* view_str);
// ========== Memory ==========

// ========== Properties ==========
/**
 * @brief Gets the length of the string.
 * @param str String object to get length of.
 * @returns The length of the held string (excluding `NULL` terminator).
 */
size_t             string_length(const struct string* str);
/**
 * @brief Gets the capacity of the string object including the `NULL` terminator.
 * @param str String object to get capacity of.
 * @returns The total buffer size in bytes.
 */
size_t             string_capacity(const struct string* str);
/**
 * @brief Gets the held cstring.
 * @param str String object to get cstring from.
 * @returns Temporary reference to internal cstring.
 * @warning - Do not store result. If an operation on the string object causes a reallocation, this
 * pointer will become stale. This is only meant for quick interaction with the internal buffer.
 * @warning - While all strings are guaranteed to be null-terminated, a non-solid type like a
 * VIEW may point to a sub-string of another string. The returned cstring is NOT guaranteed to be
 * null-terminated at length; however, the overall buffer is still null-terminated.
 */
const char*        string_cstr(const struct string* str);
/**
 * @brief Gets the held cstring with an offset applied.
 * @param str String object to get cstring from.
 * @param index Index to start cstring at.
 * @returns Temporary reference to internal cstring.
 * @warning - Do not store result. If an operation on the string object causes a reallocation,
 * this pointer will become stale. This is only meant for quick interaction with the internal
 * buffer.
 * @warning - While all strings are guaranteed to be null-terminated, a non-solid type like a
 * VIEW may point to a sub-string of another string. The returned cstring is NOT guaranteed to be
 * null-terminated at length; however, the overall buffer is still null-terminated.
 */
const char*        string_cstr_at(struct string* str, const size_t index);
/**
 * @brief Gets the specified character within the string.
 * @param str String object to get character from.
 * @param index Position within the string to get character from.
 * @returns The found character.
 */
char               string_get_char(const struct string* str, const size_t index);
/**
 * @brief Sets the specified character within the string.
 * @param str String object to set character in.
 * @param index Position within the string to set character.
 * @param c Character to set.
 * @returns Same string object with changes made to it. Object's status flag will be non-zero in the
 * event of an error.
 * @note If string object is of type VIEW, this will trigger promotion to SSO or HEAP.
 */
struct string*     string_set_char(struct string* str, const size_t index, const char c);
/**
 * @brief Gets the current status of the string object.
 * @param str String object to get status of.
 * @returns The status code of the string object.
 */
enum string_status string_status(const struct string* str);
// ========== Properties ==========

// ========== Assignment ==========
/**
 * @brief Assign the string object to the value of a c-string.
 * @param str String object to set value of.
 * @param cstr C-string to assign string object to.
 * @returns Resulting string object.
 * @note Assignment via deep copy.
 */
struct string* string_assign(struct string* str, const char* cstr);
/**
 * @brief Assign the string object to the value of another string object.
 * @param dest Destination string object.
 * @param src Source string object.
 * @returns Resulting `dest` string object.
 * @note Performs a deep copy.
 */
struct string* string_copy(struct string* dest, const struct string* src);
/**
 * @brief Moves the internal buffer from one string object to another reseting the original.
 * @param dest Destination string object.
 * @param src Source string object.
 * @returns Resulting `dest` string object.
 * @note The ownership over the buffer inside `src` is transfered to `dest`. `src` is then reset to
 * an empty string.
 */
struct string* string_move(struct string* dest, struct string* src);
/**
 * @brief Swaps the internal buffers between two string objects.
 * @param strA String Object A.
 * @param strB String Object B.
 */
void           string_swap(struct string* strA, struct string* strB);
// ========== Assignment ==========

// ========== Error Handling ==========
/**
 * @brief Checks if a string is valid.
 * @param str String object to check.
 * @return `true` when string is valid; otherwise, `false`.
 * @note A string is considered to be valid when its status is `STRING_OK`. This means both warnings
 * and errors will trigger a `false` result. If only error checking is wanted, use
 * `string_is_error()`
 */
bool string_is_valid(const struct string* str);
/**
 * @brief Checks if a string is in an error state.
 * @param str String object to check.
 * @return `true` when string is in an error state; otherwise, `false`.
 * @note This only checks for error status codes. If warnings are wanted as well, use
 * `string_is_valid()`
 */
bool string_is_error(const struct string* str);
/**
 * @brief Clear the current status resetting the string object to a valid state.
 * @param str String object to clear the status us.
 * @note This only resets the status allowing the string object to be used in API calls again. Most
 * API functions should not mutate if an error is encountered but this is not guranteed.
 */
void string_clear_status(struct string* str);
// ========== Error Handling ==========
