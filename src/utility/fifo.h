/**
 * @file fifo.h
 * @brief Public API for a First-In-First-Out (FIFO) buffer.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "utility/macros.h"

enum fifo_return_code {
    FIFO_OK    = 0,
    FIFO_EMPTY = -1,
    FIFO_FULL  = -2,
    FIFO_NOMEM = -3,
    FIFO_ERROR = -4
};

/**
 * @brief Structure that represents a FIFO.
 */
struct fifo {
    uint8_t* p_buffer;     /** @brief Pointer to the start of the internal buffer. */
    uint8_t* p_buffer_end; /** @brief Pointer to the end of the internal buffer. */
    size_t   capacity;     /** @brief Capacity of the internal buffer. */
    size_t   count;        /** @brief Count of used bytes. */
    uint8_t* p_write;      /** @brief Pointer to current write position. */
    uint8_t* p_read;       /** @brief Pointer to current read position. */
};

/** @brief Generates the identifier for the internal buffer. This is only valid if the object was
 * created with DECL or DEFINE. */
#define GB_HIDDEN_FIFO_BUFFER_NAME(name) GB_CONCAT_WITH_SPACER(name, buffer)

/** @brief Inline declares a new FIFO along side its fixed-size internal buffer. Safe for
 * use inside struct declarations. */
#define GB_FIFO_DECL(name, size)                                                                   \
    GB_BUFFER_DECL(GB_HIDDEN_FIFO_BUFFER_NAME(name), size);                                        \
    GB_HIDDEN_QUALIFIED_DECL(, struct fifo, name)

/** @brief Inline declares a new static FIFO along side its fixed-size internal buffer.
 */
#define GB_FIFO_STATIC_DECL(name, size)                                                            \
    GB_STATIC_BUFFER_DECL(GB_HIDDEN_FIFO_BUFFER_NAME(name), size);                                 \
    GB_HIDDEN_QUALIFIED_DECL(static, struct fifo, name)

/** @brief Generates the initializer statement for a FIFO. Can be used inside contexts.
 */
#define GB_HIDDEN_FIFO_INITIALIZER(ctx_dot, name)                                                  \
    (struct fifo) {                                                                                \
        .p_buffer     = ctx_dot GB_HIDDEN_FIFO_BUFFER_NAME(name),                                  \
        .p_buffer_end = ctx_dot GB_HIDDEN_FIFO_BUFFER_NAME(name) +                                 \
                        sizeof(ctx_dot GB_HIDDEN_FIFO_BUFFER_NAME(name)),                          \
        .capacity = sizeof(ctx_dot GB_HIDDEN_FIFO_BUFFER_NAME(name)), .count = 0,                  \
        .p_read  = ctx_dot GB_HIDDEN_FIFO_BUFFER_NAME(name),                                       \
        .p_write = ctx_dot GB_HIDDEN_FIFO_BUFFER_NAME(name),                                       \
    }

/** @brief Generates the initializer statement for a FIFO. Meant for use with a
 * previously declared object that is not inside a context. */
#define GB_FIFO_INITIALIZER(name) GB_HIDDEN_FIFO_INITIALIZER(, name)

/** @brief Generates the initializer statement for a FIFO. Meant for use with a
 * previously declared object that is inside a context. */
#define GB_FIFO_INITIALIZER_CTX(ctx, name) GB_HIDDEN_FIFO_INITIALIZER(ctx., name)

/** @brief Initializes a generic FIFO with a generic fixed-size buffer. */
#define GB_FIFO_INIT(fifo, buffer)                                                                 \
    fifo = (struct fifo) {                                                                         \
        .p_buffer = buffer, .p_buffer_end = buffer + sizeof(buffer), .capacity = sizeof(buffer),   \
        .count = 0, .p_write = buffer, .p_read = buffer,                                           \
    }

/**
 * @brief Declares and Defines a FIFO and its fixed-size internal buffer.
 * @note Internal buffer is not cleared. This is not an issue.
 */
#define GB_FIFO_DEFINE(name, size) GB_FIFO_DECL(name, size) = GB_FIFO_INITIALIZER(name)

/**
 * @brief Statically Declares and Defines a FIFO and its fixed-size internal buffer.
 * @note Internal buffer is not cleared. This is not an issue.
 */
#define GB_FIFO_STATIC_DEFINE(name, size)                                                          \
    GB_FIFO_STATIC_DECL(name, size) = GB_FIFO_INITIALIZER(name)

/** @brief Resets a FIFO clearing all data currently stored. */
static inline void fifo_reset(struct fifo* fifo) {
    if (fifo == NULL) { return; }

    fifo->p_read  = fifo->p_buffer;
    fifo->p_write = fifo->p_buffer;
    fifo->count   = 0;
}

/** @brief Checks if the FIFO is full. */
static inline bool fifo_is_full(const struct fifo* fifo) {
    if (fifo == NULL) { return false; }
    return fifo->count == fifo->capacity;
}

/** @brief Checks if the FIFO is empty. */
static inline bool fifo_is_empty(const struct fifo* fifo) {
    if (fifo == NULL) { return false; }
    return fifo->count == 0;
}

/** @brief Gets the Capacity of the FIFO. */
static inline size_t fifo_capacity(const struct fifo* fifo) {
    if (fifo == NULL) { return 0; }
    return fifo->capacity;
}

/** @brief Gets the number of used bytes in the FIFO. */
static inline size_t fifo_count(const struct fifo* fifo) {
    if (fifo == NULL) { return 0; }
    return fifo->count;
}

/**
 * @brief Drops a given number of bytes from the FIFO. This is equivalent to popping them but
 * without actually retrieving the data.
 *
 * @param fifo FIFO to act on.
 * @param count Number of bytes to drop.
 * @note If `count` is larger than FIFO's `count`, this is functionally equivalent to
 * `fifo_reset()`.
 */
void fifo_drop(struct fifo* fifo, const size_t count);
/**
 * @brief Attempts to push a byte to the FIFO. Does nothing if the buffer is full.
 *
 * @param fifo FIFO to act on.
 * @param byte Byte to push into FIFO.
 * @returns Status Code:
 * @returns - `FIFO_OK` OK
 * @returns - `FIFO_FULL` Buffer is full
 * @returns - `FIFO_ERROR` Error
 */
int  fifo_push(struct fifo* fifo, const uint8_t* p_byte);
/**
 * @brief Same as `fifo_push()` until the buffer is full. When full, this function forces the byte
 * into the FIFO by dropping the oldest byte in the buffer.
 *
 * @param fifo FIFO to act on.
 * @param byte Byte to push into FIFO.
 */
void fifo_push_overwrite(struct fifo* fifo, const uint8_t* p_byte);
/**
 * @brief Pops a byte off the FIFO.
 *
 * @param fifo FIFO to act on.
 * @param p_byte [OUT] Filled with the popped byte. Must not be `NULL`.
 * @returns  Number of bytes in the buffer after popping:
 * @returns - `>=0`| Byte popped
 * @returns - `FIFO_EMPTY`| FIFO was empty
 * @returns - `FIFO_ERROR`| Error
 */
int  fifo_pop(struct fifo* fifo, uint8_t* p_byte);
/**
 * @brief Peeks at the next byte in the FIFO. This does not pop it out of the FIFO.
 *
 * @param fifo FIFO to act on.
 * @param p_byte [OUT] Filled with the peeked byte. Must not be `NULL`.
 * @returns Number of bytes in the buffer:
 * @returns - `>0`| Byte Peeked
 * @returns - `FIFO_EMPTY`| FIFO was empty
 * @returns - `FIFO_ERROR`| Error
 */
int  fifo_peek(const struct fifo* fifo, uint8_t* p_byte);
/**
 * @brief Pushes an array of bytes into the buffer if there is space.
 *
 * @param fifo FIFO to act on.
 * @param data Byte array to push into buffer.
 * @param data_size Size of `data` array.
 * @returns Status Code:
 * @returns - `FIFO_OK`| OK
 * @returns - `FIFO_NOMEM`| Not enough space to fit array
 * @returns - `FIFO_ERROR`| Error
 */
int  fifo_push_range(struct fifo* fifo, const void* data, const size_t data_size);
/**
 * @brief Same as `fifo_push_range()` until the buffer is full. When full, this function forces the
 * additional bytes into the buffer by dropping the oldest bytes in the buffer.
 *
 * @param fifo FIFO to act on.
 * @param data Byte array to push into buffer.
 * @param data_size Size of `data` array.
 * @returns Status Code:
 * @returns - `FIFO_OK`| OK
 * @returns - `FIFO_ERROR`| Error
 * @note If `data_size` is larger than the FIFO buffer's `capacity`, this will only write the last
 * `capacity` worth of bytes into the FIFO. All other data is dropped.
 */
int  fifo_push_range_overwrite(struct fifo* fifo, const void* data, const size_t data_size);
/**
 * @brief Pops a range of bytes out of the buffer.
 *
 * @param fifo FIFO to act on.
 * @param count Number of bytes to pop. Must be smaller or equal to `data_size`, and the buffer's
 * `capacity` and `count`.
 * @param data [OUT] Array to store popped bytes in. Bytes are stored in the order they were popped.
 * Must not be `NULL`.
 * @param data_size Size of the array `data` points at.
 * @returns Number of bytes in the buffer after popping:
 * @returns - `>=0`| Number of bytes still in the buffer
 * @returns - `FIFO_NOMEM`| Attempted to pop more bytes than supplied buffer can hold or than
 * actually exist inside the buffer
 * @returns - `FIFO_EMPTY`| Buffer was empty
 * @returns - `FIFO_ERROR`| Error
 */
int  fifo_pop_range(struct fifo* fifo, const size_t count, void* data, const size_t data_size);
/**
 * @brief Peeks a range of bytes in the buffer. This does not pop them from the buffer.
 *
 * @param fifo FIFO to act on.
 * @param count Number of bytes to peek. Must be smaller or equal to `data_size`, and the buffer's
 * `capacity` and `count`.
 * @param data [OUT] Array to store peeked bytes in. Bytes are stored in the order they were peeked.
 * Must not be `NULL`.
 * @param data_size Size of the array `data` points at.
 * @returns Number of bytes in the buffer:
 * @returns - `>=0`| Number of bytes in the buffer
 * @returns - `FIFO_NOMEM`| Attempted to peek more bytes than supplied buffer can hold or than
 * actual exist inside the buffer
 * @returns - `FIFO_EMPTY`| Buffer was empty
 * @returns - `FIFO_ERROR`| Error
 */
int  fifo_peek_range(
     const struct fifo* fifo,
     const size_t       count,
     void*              data,
     const size_t       data_size
 );
