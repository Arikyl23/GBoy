/**
 * @file core.c
 * @brief Implementation for core string utility
 */
#include "string/core.h"

#include <SDL3/SDL_stdinc.h>

#include "internal_core.h"

// ========== Constructor ==========
// ----- Private -----
struct string* string_alloc_head(void) {
    struct string* rtn = SDL_malloc(sizeof(struct string));
    if (rtn == NULL) { return NULL; }
    return string_reset(rtn);
}

struct string* string_init_sso(const char* cstr, const size_t len) {
    struct string* rtn = string_alloc_head();
    if (rtn == NULL) { return NULL; }
    if (len >= STRING_SSO_CAPACITY) {
        string_set_status(rtn, STRING_EINVAL);
        return rtn;
    }

    SDL_memcpy(rtn->internal.sso.data, cstr, len);
    rtn->internal.sso.data[len] = '\0';
    rtn->length                 = len;

    return rtn;
}

struct string* string_init_heap(const char* cstr, const size_t len) {
    struct string* rtn = string_alloc_head();
    if (rtn == NULL) { return NULL; }

    struct string_internal_heap heap = {0};
    heap.p_data                      = SDL_malloc(len + 1);
    if (heap.p_data == NULL) {
        string_set_status(rtn, STRING_ENOMEM);
        return rtn;
    }
    SDL_memcpy(heap.p_data, cstr, len);
    heap.p_data[len] = '\0';
    heap.capacity    = len + 1;

    *rtn = (struct string){
        .internal.heap = heap,
        .length        = len,
        .status        = STRING_OK,
        .type          = STR_TYP_HEAP,
    };

    return rtn;
}

struct string* string_init_view(const char* origin, const char* start, const size_t len) {
    if (len < STRING_SSO_CAPACITY) { return string_init_sso(start, len); }

    struct string* rtn = string_alloc_head();
    if (rtn == NULL) { return NULL; }

    *rtn = (struct string){
        .internal.view.p_origin = origin,
        .internal.view.p_data   = start,
        .length                 = len,
        .status                 = STRING_OK,
        .type                   = STR_TYP_VIEW,
    };

    return rtn;
}
// ----- Private -----

// ----- Public -----
struct string* string_create(const char* cstr) {
    if (cstr == NULL) { return string_alloc_head(); }

    const size_t len = SDL_strlen(cstr);
    if (len < STRING_SSO_CAPACITY) {
        return string_init_sso(cstr, len);
    } else {
        return string_init_heap(cstr, len);
    }
}

struct string* string_create_adopt(const char* sdl_cstr) {
    struct string* rtn = string_alloc_head();

    if (sdl_cstr == NULL) { return rtn; }

    const size_t len = SDL_strlen(sdl_cstr);
    *rtn             = (struct string){
                    .internal.heap.p_data   = sdl_cstr,
                    .internal.heap.capacity = len + 1,
                    .length                 = len,
                    .status                 = STRING_OK,
                    .type                   = STR_TYP_HEAP,
    };

    return rtn;
}

struct string* string_create_copy(const struct string* str) {
    if (STRING_OBJ_CHECK(str) == false) { return string_alloc_head(); }

    if (str->length < STRING_SSO_CAPACITY) {
        return string_init_sso(string_cstr(str), str->length);
    } else {
        return string_init_heap(string_cstr(str), str->length);
    }
}

struct string* string_create_view(const struct string* str) {
    if (STRING_OBJ_CHECK(str) == false) { return string_alloc_head(); }

    const char* cstr   = string_cstr(str);
    const char* origin = (str->type == STR_TYP_VIEW) ? str->internal.view.p_origin : cstr;
    return string_init_view(origin, cstr, str->length);
}

struct string* string_create_view_cstr(const char* origin, const char* cstr) {
    if (origin == NULL || cstr == NULL) { return string_alloc_head(); }

    return string_init_view(origin, cstr, SDL_strlen(cstr));
}

struct string* string_create_reserve(const size_t capacity) {
    return string_reserve(string_alloc_head(), capacity);
}
// ----- Public -----
// ========== Constructor ==========

// ========== Deconstructor ==========
// ----- Private -----
struct string* string_reset(struct string* str) {
    if (str == NULL) { return NULL; }

    SDL_memset(str, 0, sizeof(struct string));
    *str = (struct string){
        .internal.sso.data[0] = '\0',
        .length               = 0,
        .status               = STRING_OK,
        .type                 = STR_TYP_SSO,
    };
    return str;
}
// ----- Private -----

// ----- Public -----
struct string* string_clear(struct string* str) {
    if (str == NULL) { return NULL; }

    if (str->type == STR_TYP_HEAP) { SDL_free(str->internal.heap.p_data); }

    return string_reset(str);
}
void string_free(struct string** p_str) {
    if (p_str == NULL) { return; }
    if (*p_str == NULL) { return; }

    string_clear(*p_str);
    SDL_free(*p_str);
    *p_str = NULL;

    return;
}
// ----- Public -----
// ========== Deconstructor ==========

// ========== Memory ==========
// ----- Public -----
struct string* string_reserve(struct string* str, size_t capacity) {
    if (STRING_OBJ_CHECK(str) == false) { return str; }

    const size_t length     = str->length;
    char*        new_p_data = NULL;

    // Always promote VIEW's if this functions is called
    if (str->type == STR_TYP_VIEW) {
        capacity = (capacity > length + 1) ? capacity : length + 1;
        if (capacity <= STRING_SSO_CAPACITY) {
            goto promote_to_sso;
        } else {
            goto promote_to_heap;
        }
    }

    // Don't mess with SSO or HEAP types if capacity is already good
    if (capacity <= string_capacity(str)) { return str; }

    if (str->type == STR_TYP_SSO) {
        if (capacity <= STRING_SSO_CAPACITY) {
            return str;
        } else {
            goto promote_to_heap;
        }
    }
    // Invalid Type? Safeguard for addition of new types
    if (str->type != STR_TYP_HEAP) { goto err_notsup; }

    // Expand HEAP type
    new_p_data = SDL_realloc(string_cstr_mutable(str), capacity);
    if (new_p_data == NULL) { goto err_nomem; }
    str->internal.heap = (struct string_internal_heap){
        .capacity = capacity,
        .p_data   = new_p_data,
    };
    return str;

promote_to_sso:
    // Promote VIEW into SSO
    struct string_internal_sso sso = {0};
    SDL_memcpy(sso.data, string_cstr(str), length);
    sso.data[length] = '\0';
    string_reset(str);
    *str = (struct string){
        .internal.sso = sso,
        .length       = length,
        .status       = STRING_OK,
        .type         = STR_TYP_SSO,
    };
    return str;

promote_to_heap:
    // Promote SSO or VIEW into HEAP
    new_p_data = SDL_malloc(capacity);
    if (new_p_data == NULL) { goto err_nomem; }
    SDL_memcpy(new_p_data, string_cstr(str), length);
    new_p_data[length] = '\0';
    string_reset(str);
    *str = (struct string){
        .internal.heap.capacity = capacity,
        .internal.heap.p_data   = new_p_data,
        .length                 = length,
        .status                 = STRING_OK,
        .type                   = STR_TYP_HEAP,
    };
    return str;

err_nomem:
    string_set_status(str, STRING_ENOMEM);
    return str;
err_notsup:
    string_set_status(str, STRING_ENOTSUP);
    return str;
}

struct string* string_resize(struct string* str, const size_t size) {
    if (STRING_OBJ_CHECK(str) == false) { return str; }

    const size_t old_capacity = string_capacity(str);
    if (size <= old_capacity) { return str; }

    size_t new_capacity = old_capacity + old_capacity / 2;
    new_capacity        = (size > new_capacity) ? size : new_capacity;
    return string_reserve(str, new_capacity);
}

struct string* string_shrink_to_fit(struct string* str) {
    if (STRING_OBJ_CHECK(str) == false) { return str; }

    if (str->type == STR_TYP_VIEW || str->type == STR_TYP_SSO) { return str; }

    const size_t length     = str->length;
    char*        old_p_data = string_cstr_mutable(str);

    if (length < STRING_SSO_CAPACITY) {
        // Decay to SSO
        struct string_internal_sso sso = {0};
        SDL_memcpy(sso.data, old_p_data, length);
        sso.data[length] = '\0';
        SDL_free(old_p_data);
        str->internal.sso = sso;
        str->type         = STR_TYP_SSO;
        return str;
    } else {
        // Shrink HEAP allocation
        const size_t new_capacity = length + 1;
        char*        new_p_data   = SDL_realloc(old_p_data, new_capacity);
        if (new_p_data == NULL) {
            string_set_status(str, STRING_WAGAIN);
            return str;
        }
        str->internal.heap = (struct string_internal_heap){
            .capacity = new_capacity,
            .p_data   = new_p_data,
        };
        return str;
    }
}

struct string* string_solidify(struct string* str) {
    if (STRING_OBJ_CHECK(str) == false) { return str; }
    if (str->type == STR_TYP_SSO || str->type == STR_TYP_HEAP) { return str; }
    return string_reserve(str, str->length + 1);
}
// ----- Public -----
// ========== Memory ==========

// ========== Properties ==========
// ----- Private -----
void string_set_status(struct string* str, const enum string_status status) {
    // OK set WARNING or ERROR
    if (str->status == STRING_OK) {
        str->status = status;
        return;
    }

    // WARNING set ERROR
    if (string_status_is_warning(str->status) == true && string_status_is_error(status) == true) {
        str->status = status;
        return;
    }

    // ERROR set ERROR (no set)
    return;
}

char* string_cstr_mutable(struct string* str) {
    if (str == NULL) { return NULL; }

    switch (str->type) {
    case STR_TYP_SSO:
        return str->internal.sso.data;
    case STR_TYP_HEAP:
        return str->internal.heap.p_data;
    default:
    case STR_TYP_VIEW:
        string_set_status(str, STRING_ENOTSUP);
        return NULL;
    }
}

char* string_cstr_at_mutable(struct string* str, const size_t index) {
    if (str == NULL) { return NULL; }
    if (index >= str->length) {
        string_set_status(str, STRING_EINVAL);
        return NULL;
    }

    switch (str->type) {
    case STR_TYP_SSO:
        return str->internal.sso.data + index;
    case STR_TYP_HEAP:
        return str->internal.heap.p_data + index;
    default:
    case STR_TYP_VIEW:
        string_set_status(str, STRING_ENOTSUP);
        return NULL;
    }
}
// ----- Private -----

// ----- Public -----
size_t string_length(const struct string* str) { return (str == NULL) ? 0 : str->length; }

size_t string_capacity(const struct string* str) {
    if (str == NULL) { return 0; }

    switch (str->type) {
    case STR_TYP_SSO:
        return STRING_SSO_CAPACITY;
    case STR_TYP_HEAP:
        return str->internal.heap.capacity;
    default:
    case STR_TYP_VIEW:
        return 0;
    }
}

const char* string_cstr(const struct string* str) {
    if (str == NULL) { return NULL; }

    switch (str->type) {
    case STR_TYP_SSO:
        return str->internal.sso.data;
    case STR_TYP_HEAP:
        return str->internal.heap.p_data;
    case STR_TYP_VIEW:
        return str->internal.view.p_data;
    default:
        return NULL;
    }
}

const char* string_cstr_at(struct string* str, const size_t index) {
    if (str == NULL) { return NULL; }
    if (index >= str->length) {
        string_set_status(str, STRING_EINVAL);
        return NULL;
    }

    switch (str->type) {
    case STR_TYP_SSO:
        return str->internal.sso.data + index;
    case STR_TYP_HEAP:
        return str->internal.heap.p_data + index;
    case STR_TYP_VIEW:
        return str->internal.view.p_data + index;
    default:
        return NULL;
    }
}

char string_get_char(const struct string* str, const size_t index) {
    if (str == NULL || index >= str->length) { return '\0'; }

    return string_cstr(str)[index];
}

struct string* string_set_char(struct string* str, const size_t index, const char c) {
    if (STRING_OBJ_CHECK(str) == false) { return str; }
    if (index >= str->length) {
        string_set_status(str, STRING_EINVAL);
        return str;
    }

    if (string_status_is_error(string_solidify(str)->status) == true) { return str; }
    string_cstr_mutable(str)[index] = c;

    return str;
}

enum string_status string_status(const struct string* str) {
    return (str == NULL) ? STRING_ENULL : str->status;
}
// ----- Public -----
// ========== Properties ==========

// ========== Assignment ==========
// ----- Public -----
struct string* string_assign(struct string* str, const char* cstr) {
    if (STRING_OBJ_CHECK(str) == false) { return str; }
    if (cstr == NULL) { return string_clear(str); }

    struct string src = {
        .internal.view.p_origin = cstr,
        .internal.view.p_data   = cstr,
        .length                 = SDL_strlen(cstr),
        .status                 = STRING_OK,
        .type                   = STR_TYP_VIEW,
    };
    string_copy(str, &src);

    return str;
}

struct string* string_copy(struct string* dest, const struct string* src) {
    if (STRING_OBJ_CHECK(dest) == false) { return dest; }
    if (STRING_OBJ_CHECK(src) == false) {
        string_set_status(dest, STRING_EINVAL);
        return dest;
    }

    if (string_is_error(string_reserve(dest, src->length + 1)) == true) { return dest; }

    char* p_data = string_cstr_mutable(dest);
    SDL_memcpy(p_data, string_cstr(src), src->length);
    p_data[src->length] = '\0';
    dest->length        = src->length;

    return dest;
}

struct string* string_move(struct string* dest, struct string* src) {
    if (STRING_OBJ_CHECK(dest) == false) { return dest; }
    if (STRING_OBJ_CHECK(src) == false) {
        string_set_status(dest, STRING_EINVAL);
        return dest;
    }

    string_clear(dest);
    *dest = *src;
    string_reset(src);

    return dest;
}

void string_swap(struct string* strA, struct string* strB) {
    if (STRING_OBJ_CHECK(strA) == false || STRING_OBJ_CHECK(strB) == false) { return; }

    struct string swp = *strA;
    *strA             = *strB;
    *strB             = swp;

    return;
}
// ----- Public -----
// ========== Assignment ==========

// ========== Error Handling ==========
// ----- Public -----
bool string_is_valid(const struct string* str) { return str != NULL && str->status == STRING_OK; }
bool string_is_error(const struct string* str) {
    return str == NULL || string_status_is_error(str->status);
}
void string_clear_status(struct string* str) {
    if (str != NULL) { str->status = STRING_OK; }
}
// ----- Public -----
// ========== Error Handling ==========

// ========== Utility ==========
// ----- Private -----
bool string_is_solid(const struct string* str) { return str->type != STR_TYP_VIEW; }

bool string_is_overlapping_cstr(
    const char*  str1,
    const size_t len1,
    const char*  str2,
    const size_t len2
) {
    if (str1 == NULL || str2 == NULL || len1 == 0 || len2 == 0) { return false; }
    return (str1 <= str2 && str1 + len1 >= str2) || (str2 <= str1 && str2 + len2 >= str1);
}
// ----- Private -----
// ========== Utility ==========
