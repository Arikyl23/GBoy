/**
 * @file unit_test_core.c
 * @brief Unit tests for the core String Library API.
 */

#include <SDL3/SDL_stdinc.h>
#include <stdbool.h>

#include "internal_core.h"
#include "string/core.h"
#include "testing.h"

LOG_MODULE_SETUP_DEFAULT("STRING | UNIT TEST CORE");

bool test_core_module(void);

bool test_constructors(void);
bool test_deconstructors(void);
bool test_memory(void);
bool test_properties(void);
bool test_assignment(void);
bool test_error_handling(void);

bool test_constructors_create_basic(void);
bool test_constructors_create_copy(void);
bool test_constructors_create_view(void);
bool test_constructors_create_view_cstr(void);
bool test_constructors_create_reserve(void);

bool test_deconstructors_reset(void);
bool test_deconstructors_clear(void);
bool test_deconstructors_free(void);

bool test_memory_reserve(void);
bool test_memory_resize(void);
bool test_memory_shrink_to_fit(void);

bool test_string_equal(const struct string* strA, const struct string* strB);

// ----- Data Section -----
const char data_cstr_short[];
const char data_cstr_long[];

struct string data_init_empty_string(void);
struct string data_init_sso_short_string(void);
struct string data_init_heap_long_string(void);
void          data_free_heap_string(struct string* str);
// ----- Data Section -----

int main(void) {
    // 0 indicates a success, 1 a failure
    return (test_core_module() == true) ? 0 : 1;
}

bool test_core_module(void) {
    ASSERT_TRUE(test_constructors(), "Constructor Test(s) Failed");
    ASSERT_TRUE(test_deconstructors() == true, "Deconstructor Test(s) Failed");
    ASSERT_TRUE(test_memory() == true, "Memory Test(s) Failed");
    // ASSERT_TRUE(test_properties() == true, "Property Test(s) Failed");
    // ASSERT_TRUE(test_assignment() == true, "Assignment Test(s) Failed");
    // ASSERT_TRUE(test_error_handling() == true, "Error Handling Test(s) Failed");
    return true;
}

// ========== Constructor Tests ==========
bool test_constructors(void) {
    ASSERT_TRUE(test_constructors_create_basic(), "string_create() Test Failed");
    ASSERT_TRUE(test_constructors_create_copy(), "string_create_copy() Test Failed");
    ASSERT_TRUE(test_constructors_create_view(), "string_create_view() Test Failed");
    ASSERT_TRUE(test_constructors_create_view_cstr(), "string_create_view_cstr() Test Failed");
    ASSERT_TRUE(test_constructors_create_reserve(), "string_create_reserve() Test Failed");
    return true;
}

bool test_constructors_create_basic(void) {
    // Check NULL case:
    struct string  expected  = data_init_empty_string();
    struct string* generated = string_create(NULL);
    ASSERT_TRUE(test_string_equal(generated, &expected), "Generated string did not match expected");
    SDL_free(generated);

    // Check Empty case:
    expected  = data_init_empty_string();
    generated = string_create("");
    ASSERT_TRUE(test_string_equal(generated, &expected), "Generated string did not match expected");
    SDL_free(generated);

    // Check SSO case (short string)
    expected  = data_init_sso_short_string();
    generated = string_create(data_cstr_short);
    ASSERT_TRUE(test_string_equal(generated, &expected), "Generated string did not match expected");
    SDL_free(generated);

    // Check HEAP case (long string)
    expected  = data_init_heap_long_string();
    generated = string_create(data_cstr_long);
    ASSERT_TRUE(test_string_equal(generated, &expected), "Generated string did not match expected");
    data_free_heap_string(&expected);
    SDL_free(generated->internal.heap.p_data);
    SDL_free(generated);

    return true;
}

bool test_constructors_create_copy(void) {
    // Check NULL case:
    struct string  expected  = data_init_empty_string();
    struct string* generated = string_create_copy(NULL);
    ASSERT_TRUE(test_string_equal(generated, &expected), "Generated string did not match expected");
    SDL_free(generated);

    // Check Empty case:
    expected  = data_init_empty_string();
    generated = string_create_copy(&expected);
    ASSERT_TRUE(test_string_equal(generated, &expected), "Generated string did not match expected");
    SDL_free(generated);

    // Check SSO case (short string)
    expected  = data_init_sso_short_string();
    generated = string_create_copy(&expected);
    ASSERT_TRUE(test_string_equal(generated, &expected), "Generated string did not match expected");
    SDL_free(generated);

    // Check HEAP case (long string)
    expected  = data_init_heap_long_string();
    generated = string_create_copy(&expected);
    ASSERT_TRUE(test_string_equal(generated, &expected), "Generated string did not match expected");
    data_free_heap_string(&expected);
    data_free_heap_string(generated);
    SDL_free(generated);

    return true;
}

bool test_constructors_create_view(void) {
    // Check NULL case:
    struct string  expected  = data_init_empty_string();
    struct string* generated = string_create_view(NULL);
    ASSERT_TRUE(test_string_equal(generated, &expected), "Generated string did not match expected");
    SDL_free(generated);

    // Check that automatic SSO promotion happens on SSO sized VIEWs
    struct string test = data_init_sso_short_string();
    expected           = data_init_sso_short_string();
    generated          = string_create_view(&test);
    ASSERT_TRUE(test_string_equal(generated, &expected), "Generated string did not match expected");
    SDL_free(generated);

    // Direct VIEW (view of a solid)
    test     = data_init_heap_long_string();
    expected = (struct string){
        .internal.view.p_origin = data_cstr_long,
        .internal.view.p_data   = data_cstr_long,
        .length                 = test.length,
        .status                 = STRING_OK,
        .type                   = STR_TYP_VIEW,
    };
    generated = string_create_view(&test);
    ASSERT_TRUE(test_string_equal(generated, &expected), "Generated string did not match expected");
    data_free_heap_string(&test);
    SDL_free(generated);

    // Indirect VIEW (view of a view)
    test = (struct string){
        .internal.view.p_origin = data_cstr_long,
        .internal.view.p_data   = data_cstr_long,
        .length                 = SDL_strlen(data_cstr_long),
        .status                 = STRING_OK,
        .type                   = STR_TYP_VIEW,
    };
    expected  = test;
    generated = string_create_view(&test);
    ASSERT_TRUE(test_string_equal(generated, &expected), "Generated string did not match expected");
    SDL_free(generated);

    // Indirect Offset VIEW (substring view)
    test = (struct string){
        .internal.view.p_origin = data_cstr_long,
        .internal.view.p_data   = data_cstr_long + 10,
        .length                 = SDL_strlen(data_cstr_long + 10),
        .status                 = STRING_OK,
        .type                   = STR_TYP_VIEW,
    };
    expected  = test;
    generated = string_create_view(&test);
    ASSERT_TRUE(test_string_equal(generated, &expected), "Generated string did not match expected");
    SDL_free(generated);

    return true;
}

bool test_constructors_create_view_cstr(void) {
    // NULL case
    struct string  expected  = data_init_empty_string();
    struct string* generated = string_create_view_cstr(NULL, NULL);
    ASSERT_TRUE(test_string_equal(generated, &expected), "Generated string did not match expected");
    SDL_free(generated);
    generated = string_create_view_cstr(NULL, data_cstr_short);
    ASSERT_TRUE(test_string_equal(generated, &expected), "Generated string did not match expected");
    SDL_free(generated);
    generated = string_create_view_cstr(data_cstr_short, NULL);
    ASSERT_TRUE(test_string_equal(generated, &expected), "Generated string did not match expected");
    SDL_free(generated);

    // Check that automatic SSO promotion happens on SSO sized VIEWs
    struct string test = data_init_sso_short_string();
    expected           = data_init_sso_short_string();
    generated          = string_create_view_cstr(data_cstr_short, data_cstr_short);
    ASSERT_TRUE(test_string_equal(generated, &expected), "Generated string did not match expected");
    SDL_free(generated);

    // Direct VIEW (view of a solid)
    expected = (struct string){
        .internal.view.p_origin = data_cstr_long,
        .internal.view.p_data   = data_cstr_long,
        .length                 = SDL_strlen(data_cstr_long),
        .status                 = STRING_OK,
        .type                   = STR_TYP_VIEW,
    };
    generated = string_create_view_cstr(data_cstr_long, data_cstr_long);
    ASSERT_TRUE(test_string_equal(generated, &expected), "Generated string did not match expected");
    SDL_free(generated);

    // Direct Offset VIEW (substring view)
    expected = (struct string){
        .internal.view.p_origin = data_cstr_long,
        .internal.view.p_data   = data_cstr_long + 10,
        .length                 = SDL_strlen(data_cstr_long + 10),
        .status                 = STRING_OK,
        .type                   = STR_TYP_VIEW,
    };
    generated = string_create_view_cstr(data_cstr_long, data_cstr_long + 10);
    ASSERT_TRUE(test_string_equal(generated, &expected), "Generated string did not match expected");
    SDL_free(generated);

    return true;
}

bool test_constructors_create_reserve(void) {
    // SSO cases (0 - STRING_SSO_CAPACITY)
    struct string expected = {
        .internal.sso.data[0] = '\0',
        .length               = 0,
        .status               = STRING_OK,
        .type                 = STR_TYP_SSO,
    };
    struct string* generated = NULL;
    for (size_t i = 0; i <= STRING_SSO_CAPACITY; i++) {
        generated = string_create_reserve(i);
        ASSERT_TRUE(
            test_string_equal(generated, &expected),
            "Generated string did not match expected"
        );
        SDL_free(generated);
    }

    // HEAP case (> STRING_SSO_CAPACITY)
    const size_t test_capacity = 2 * STRING_SSO_CAPACITY;
    expected                   = (struct string){
                          .internal.heap.p_data   = SDL_malloc(test_capacity),
                          .internal.heap.capacity = test_capacity,
                          .length                 = 0,
                          .status                 = STRING_OK,
                          .type                   = STR_TYP_HEAP,
    };
    ASSERT_NOT_NULL(
        expected.internal.heap.p_data,
        "Failed to allocated memory to test structures."
    );
    expected.internal.heap.p_data[0] = '\0';
    generated                        = string_create_reserve(test_capacity);
    ASSERT_TRUE(test_string_equal(generated, &expected), "Generated string did not match expected");
    ASSERT_EQ(
        generated->internal.heap.capacity,
        expected.internal.heap.capacity,
        "Generated capacity was \"%zu\". Expected \"%zu\"",
        generated->internal.heap.capacity,
        expected.internal.heap.capacity
    );
    data_free_heap_string(&expected);
    data_free_heap_string(generated);
    SDL_free(generated);

    return true;
}
// ========== Constructor Tests ==========

// ========== Deconstructor Tests ==========
bool test_deconstructors(void) {
    ASSERT_TRUE(test_deconstructors_reset(), "string_reset() Test Failed");
    ASSERT_TRUE(test_deconstructors_clear(), "string_clear() Test Failed");
    ASSERT_TRUE(test_deconstructors_free(), "string_free() Test Failed");
    return true;
}

bool test_deconstructors_reset(void) {
    struct string  expected  = data_init_empty_string();
    struct string* generated = string_create(data_cstr_short);
    struct string* returned  = string_reset(generated);
    ASSERT_EQ(generated, returned, "Return value pointer did not match input pointer");
    ASSERT_TRUE(
        test_string_equal(generated, &expected),
        "Generated string did not match the expected"
    );
    SDL_free(generated);
    return true;
}

bool test_deconstructors_clear(void) {
    struct string  expected  = data_init_empty_string();
    struct string* generated = string_create(data_cstr_short);
    struct string* returned  = string_clear(generated);
    ASSERT_EQ(generated, returned, "Returned handle did not match input handle");
    ASSERT_TRUE(
        test_string_equal(generated, &expected),
        "Generated string did not match the expected"
    );
    SDL_free(generated);
    return true;
}

bool test_deconstructors_free(void) {
    struct string* generated = string_create(data_cstr_short);
    string_free(&generated);
    ASSERT_NULL(generated, "Failed to clear handle");
    return true;
}
// ========== Deconstructor Tests ==========

// ========== Memory Tests ==========
bool test_memory(void) {
    ASSERT_TRUE(test_memory_reserve(), "string_reserve() Test Failed");
    ASSERT_TRUE(test_memory_resize(), "string_resize() Test Failed");
    ASSERT_TRUE(test_memory_shrink_to_fit(), "string_shrink_to_fit() Test Failed");
    return true;
}

bool test_memory_reserve(void) {
    // Generate SSO string and verify string does not grow on size smaller than SSO capacity
    struct string* generated = string_create("");
    struct string* returned  = string_reserve(generated, STRING_SSO_CAPACITY);
    ASSERT_EQ(generated, returned, "Returned handle did not match input handle");
    ASSERT_EQ(generated->type, STR_TYP_SSO, "Promoted to HEAP when it shouldn't have");

    // Force HEAP promotion (SSO -> HEAP)
    size_t test_capacity = STRING_SSO_CAPACITY * 3;
    string_reserve(generated, test_capacity);
    ASSERT_EQ(generated->type, STR_TYP_HEAP, "Failed to promote SSO -> HEAP");
    ASSERT_EQ(
        generated->internal.heap.capacity,
        test_capacity,
        "Capacity was \"%zu\" but expected \"%zu\"",
        generated->internal.heap.capacity,
        test_capacity
    );

    // Verify HEAP doesn't grow/shrink when already large enough
    string_reserve(generated, STRING_SSO_CAPACITY);
    ASSERT_EQ(generated->type, STR_TYP_HEAP, "Demoted from HEAP when it shouldn't have");
    ASSERT_EQ(
        generated->internal.heap.capacity,
        test_capacity,
        "Capacity was \"%zu\" but expected \"%zu\"",
        generated->internal.heap.capacity,
        test_capacity
    );
    string_free(&generated);

    // Verify VIEWs of HEAP size are automatically promoted to HEAP
    generated = string_create_view_cstr(data_cstr_long, data_cstr_long);
    // This should force a VIEW->HEAP promotion. It is also a nop on HEAPs meaing the resulting
    // capacity should be the exact size of the buffer needed to store the string.
    string_reserve(generated, STRING_SSO_CAPACITY);
    ASSERT_EQ(generated->type, STR_TYP_HEAP, "Failed to promote VIEW -> HEAP");
    test_capacity = SDL_strlen(data_cstr_long) + 1;
    ASSERT_EQ(
        generated->internal.heap.capacity,
        test_capacity,
        "Capacity was \"%zu\" but expected \"%zu\"",
        generated->internal.heap.capacity,
        test_capacity
    );
    string_free(&generated);

    return true;
}

bool test_memory_resize(void) {
    // Generate SSO string and verify string does not grow on size smaller than SSO capacity
    struct string* generated = string_create("");
    struct string* returned  = string_resize(generated, STRING_SSO_CAPACITY);
    ASSERT_EQ(generated, returned, "Returned handle did not match input handle");
    ASSERT_EQ(generated->type, STR_TYP_SSO, "Promoted to HEAP when it shouldn't have");

    // Force HEAP promotion (SSO -> HEAP)
    size_t test_capacity = STRING_SSO_CAPACITY * 3;
    string_resize(generated, test_capacity);
    ASSERT_EQ(generated->type, STR_TYP_HEAP, "Failed to promote SSO -> HEAP");
    ASSERT_GTEQ(
        generated->internal.heap.capacity,
        test_capacity,
        "Capacity was \"%zu\" but expected at least \"%zu\".",
        generated->internal.heap.capacity,
        test_capacity
    );

    // Verify HEAP doesn't grow/shrink when already large enough
    string_reserve(generated, STRING_SSO_CAPACITY);
    ASSERT_EQ(generated->type, STR_TYP_HEAP, "Demoted from HEAP when it shouldn't have");
    ASSERT_GTEQ(
        generated->internal.heap.capacity,
        test_capacity,
        "Capacity was \"%zu\" but expected at least \"%zu\".",
        generated->internal.heap.capacity,
        test_capacity
    );
    string_free(&generated);

    // Verify VIEWs of HEAP size are automatically promoted to HEAP
    generated = string_create_view_cstr(data_cstr_long, data_cstr_long);
    // This should force a VIEW->HEAP promotion. It is also a nop on HEAPs meaing the resulting
    // capacity should be the exact size of the buffer needed to store the string.
    string_reserve(generated, STRING_SSO_CAPACITY);
    ASSERT_EQ(generated->type, STR_TYP_HEAP, "Failed to promote VIEW -> HEAP");
    test_capacity = SDL_strlen(data_cstr_long) + 1;
    ASSERT_GTEQ(
        generated->internal.heap.capacity,
        test_capacity,
        "Capacity was \"%zu\" but expected at least \"%zu\".",
        generated->internal.heap.capacity,
        test_capacity
    );
    string_free(&generated);

    return true;
}
bool test_memory_shrink_to_fit(void) {
    // Test oversize HEAP -> exact HEAP
    struct string* generated         = string_create(data_cstr_long);
    size_t         expected_capacity = SDL_strlen(data_cstr_long) + 1;
    size_t         test_oversize     = expected_capacity * 2;
    string_reserve(generated, test_oversize);
    ASSERT_EQ(generated->type, STR_TYP_HEAP, "Expected HEAP string object");
    ASSERT_EQ(
        generated->internal.heap.capacity,
        test_oversize,
        "Capacity was \"%zu\" but expected \"%zu\".",
        generated->internal.heap.capacity,
        test_oversize
    );
    struct string* returned = string_shrink_to_fit(generated);
    ASSERT_EQ(generated, returned, "Returned handle did not match input handle");
    ASSERT_EQ(generated->type, STR_TYP_HEAP, "Expected HEAP string object");
    ASSERT_EQ(
        generated->internal.heap.capacity,
        expected_capacity,
        "Capacity was \"%zu\" but expected \"%zu\".",
        generated->internal.heap.capacity,
        expected_capacity
    );
    string_free(&generated);

    // Test HEAP -> SSO
    generated = string_create(data_cstr_long);
    ASSERT_EQ(generated->type, STR_TYP_HEAP, "Expected HEAP string object");
    // Force to empty string
    generated->internal.heap.p_data[0] = '\0';
    generated->length                  = 0;
    string_shrink_to_fit(generated);
    ASSERT_EQ(generated->type, STR_TYP_SSO, "Expected HEAP -> SSO decay");
    string_free(&generated);

    // Test nop on VIEW
    generated = string_create_view_cstr(data_cstr_long, data_cstr_long);
    string_shrink_to_fit(generated);
    ASSERT_EQ(generated->type, STR_TYP_VIEW, "Expected VIEW string object");

    return true;
}
// ========== Memory Tests ==========

bool test_string_equal(const struct string* generated, const struct string* expected) {
    // Test generic properties first
    ASSERT_EQ(
        generated->length,
        expected->length,
        "Generated length was \"%zu\". Expected \"%zu\"",
        string_type_to_string(generated->length),
        string_type_to_string(expected->length)
    );
    ASSERT_EQ(
        generated->status,
        expected->status,
        "Generated status was \"%s\". Expected \"%s\"",
        string_status_to_string(generated->status),
        string_status_to_string(expected->status)
    );
    ASSERT_EQ(
        generated->type,
        expected->type,
        "Generated type was \"%s\". Expected \"%s\"",
        string_type_to_string(generated->type),
        string_type_to_string(expected->type)
    );

    // After this point types are the same, handle string comparison based on type
    switch (generated->type) {
    case STR_TYP_SSO:
        ASSERT_TRUE(
            SDL_strcmp(generated->internal.sso.data, expected->internal.sso.data) == 0,
            "Generated string was \"%s\". Expected \"%s\"",
            generated->internal.sso.data,
            expected->internal.sso.data
        );
        return true;
    case STR_TYP_HEAP:
        ASSERT_TRUE(
            SDL_strcmp(generated->internal.heap.p_data, expected->internal.heap.p_data) == 0,
            "Generated string was \"%s\". Expected \"%s\"",
            generated->internal.heap.p_data,
            expected->internal.heap.p_data
        );
        // CAPACITY SHOULD BE CHECKED SEPERATELY IF VALUE MATTERS. NOT NORMALLY AN INDICATOR OF
        // EQUALITY.
        return true;
    case STR_TYP_VIEW:
        ASSERT_TRUE(
            SDL_strcmp(generated->internal.view.p_origin, expected->internal.view.p_origin) == 0,
            "Generated string origin was \"%s\". Expected \"%s\"",
            generated->internal.view.p_origin,
            expected->internal.view.p_origin
        );
        ASSERT_TRUE(
            SDL_strcmp(generated->internal.view.p_data, expected->internal.view.p_data) == 0,
            "Generated string view was \"%s\". Expected \"%s\"",
            generated->internal.view.p_data,
            expected->internal.view.p_data
        );
        return true;
    default:
        ASSERT(
            "Generate and Expected types matched but handling logic is missing. Unknown type: "
            "\"%s\"",
            string_type_to_string(generated->type)
        );
    }
}

// ========== DATA SECTION ==========
const char data_cstr_short[] = "Hello";
const char data_cstr_long[] =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec mollis "
    "lobortis dapibus. Morbi commodo lorem et cursus porttitor.";

struct string data_init_empty_string(void) {
    return (struct string){
        .internal.sso.data[0] = '\0',
        .length               = 0,
        .status               = STRING_OK,
        .type                 = STR_TYP_SSO,
    };
}

struct string data_init_sso_short_string(void) {
    struct string rtn = data_init_empty_string();
    rtn.length        = SDL_strlen(data_cstr_short);
    SDL_memcpy(rtn.internal.sso.data, data_cstr_short, rtn.length);
    rtn.internal.sso.data[rtn.length] = '\0';

    return rtn;
}

struct string data_init_heap_long_string(void) {
    const size_t  len = SDL_strlen(data_cstr_long);
    struct string rtn = {
        .internal.heap.p_data   = SDL_malloc(len + 1),
        .internal.heap.capacity = len + 1,
        .length                 = len,
        .status                 = STRING_OK,
        .type                   = STR_TYP_HEAP,
    };
    SDL_memcpy(rtn.internal.heap.p_data, data_cstr_long, len);
    rtn.internal.heap.p_data[len] = '\0';

    return rtn;
}

void data_free_heap_string(struct string* str) {
    if (str->type != STR_TYP_HEAP) { return; }

    SDL_free(str->internal.heap.p_data);
    return;
}
// ========== DATA SECTION ==========
