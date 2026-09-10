/**
 * @file fifo.c
 * @brief Implementation for a First-In-First-Out (FIFO) buffer.
 */
#include "utility/fifo.h"

#include <string.h>

static inline void ptr_add(struct fifo* fifo, uint8_t** ptr, const size_t count) {
    *ptr = fifo->p_buffer + ((*ptr - fifo->p_buffer) + count) % fifo->capacity;
}

void fifo_drop(struct fifo* fifo, const size_t count) {
    if (fifo == NULL || count == 0) { return; }
    if (count >= fifo->count) {
        fifo_reset(fifo);
        return;
    }

    ptr_add(fifo, &fifo->p_read, count);
    fifo->count -= count;
}

int fifo_push(struct fifo* fifo, const uint8_t* p_byte) {
    if (fifo == NULL || p_byte == NULL) { return FIFO_ERROR; }
    if (fifo_is_full(fifo) == true) { return FIFO_FULL; }

    *(fifo->p_write) = *p_byte;
    ptr_add(fifo, &fifo->p_write, 1);
    fifo->count++;
    return 0;
}

void fifo_push_overwrite(struct fifo* fifo, const uint8_t* p_byte) {
    if (fifo == NULL || p_byte == NULL) { return; }

    if (fifo_is_full(fifo) == false) {
        fifo_push(fifo, p_byte);
        return;
    } else {
        fifo_drop(fifo, 1);
        fifo_push(fifo, p_byte);
        return;
    }
}

int fifo_pop(struct fifo* fifo, uint8_t* p_byte) {
    // Peek logic is fundamentally the same apart from dropping the read byte
    int rc = fifo_peek(fifo, p_byte);
    if (rc < 0) { return rc; }
    fifo_drop(fifo, 1);
    return (int)fifo->count;
}

int fifo_peek(const struct fifo* fifo, uint8_t* p_byte) {
    if (fifo == NULL || p_byte == NULL) { return FIFO_ERROR; }
    if (fifo_is_empty(fifo) == true) { return FIFO_EMPTY; }

    *p_byte = *(fifo->p_read);
    return (int)fifo->count;
}

int fifo_push_range(struct fifo* fifo, const void* data, const size_t data_size) {
    if (fifo == NULL || data == NULL) { return FIFO_ERROR; }
    if (fifo->capacity - fifo->count < data_size) { return FIFO_NOMEM; }
    if (data_size == 0) { return FIFO_OK; }

    const size_t bytes_till_wrap = (size_t)(fifo->p_buffer_end - fifo->p_write);
    if (data_size <= bytes_till_wrap) {
        memcpy(fifo->p_write, data, data_size);
    } else {
        memcpy(fifo->p_write, data, bytes_till_wrap);
        const size_t remaining_bytes = data_size - bytes_till_wrap;
        memcpy(fifo->p_buffer, data + bytes_till_wrap, remaining_bytes);
    }

    ptr_add(fifo, &fifo->p_write, data_size);
    fifo->count += data_size;
    return FIFO_OK;
}

int fifo_push_range_overwrite(struct fifo* fifo, const void* data, const size_t data_size) {
    if (fifo == NULL || data == NULL) { return FIFO_ERROR; }

    if (data_size >= fifo->capacity) {
        // Complete overflow, only bother will last capacity size worth of bytes
        fifo_reset(fifo);
        return fifo_push_range(fifo, data + data_size - fifo->capacity, fifo->capacity);
    } else if (data_size >= fifo->capacity - fifo->count) {
        // Partial overflow, clear requried space and push data in
        const size_t additional_space = data_size - (fifo->capacity - fifo->count);
        fifo_drop(fifo, additional_space);
        return fifo_push_range(fifo, data, data_size);
    } else {
        // Fits
        return fifo_push_range(fifo, data, data_size);
    }
}

int fifo_pop_range(struct fifo* fifo, const size_t count, void* data, const size_t data_size) {
    // Peek logic is fundamentally the same apart from dropping the read bytes
    int rc = fifo_peek_range(fifo, count, data, data_size);
    if (rc < 0) { return rc; }
    fifo_drop(fifo, count);
    return fifo->count;
}

int fifo_peek_range(
    const struct fifo* fifo,
    const size_t       count,
    void*              data,
    const size_t       data_size
) {
    if (fifo == NULL || data == NULL) { return FIFO_ERROR; }
    if (fifo_is_empty(fifo) == true) { return FIFO_EMPTY; }
    if (count > data_size || count > fifo->count || count > fifo->capacity) { return FIFO_NOMEM; }

    const size_t bytes_till_wrap = (size_t)(fifo->p_buffer_end - fifo->p_read);
    if (count <= bytes_till_wrap) {
        memcpy(data, fifo->p_read, count);
    } else {
        memcpy(data, fifo->p_read, bytes_till_wrap);
        const size_t remaining_bytes = count - bytes_till_wrap;
        memcpy(data + bytes_till_wrap, fifo->p_buffer, remaining_bytes);
    }

    return (int)fifo->count;
}
