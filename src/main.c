#include <stdio.h>
#include <string/string.h>

int main(void) {
    struct string* strA = string_create("Hello");
    struct string* strB = string_create("World");
    struct string* strC = string_create(NULL);

    printf("A: %s\n", string_cstr(strA));
    printf("B: %s\n", string_cstr(strB));
    printf("C: %s\n", string_cstr(strC));

    printf("Concat A to B and store in C (with a few additional bits):\n");

    string_append_char(
        string_concat(strC, strA, string_prepend_char(string_copy(strC, strB), ' ')),
        '!'
    );

    printf("A: %s\n", string_cstr(strA));
    printf("B: %s\n", string_cstr(strB));
    printf("C: %s\n", string_cstr(strC));

    string_free(&strA);
    string_free(&strB);
    string_free(&strC);

    return 0;
}
