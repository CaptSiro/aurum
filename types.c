#include "types.h"



DYN_ARRAY_FN(string, char)
void string_print(const string_t *str) {
    for (int i = 0; i < str->length; ++i) {
        putchar(str->arr[i]);
    }
}
string_t *string_allocate(const char *literal) {
    string_t *str = string_create();

    for (int i = 0; literal[i] != '\0'; ++i) {
        string_push(str, literal[i]);
    }

    return str;
}
bool string_equals(const string_t *a, const string_t *b) {
    if (a->length != b->length) {
        return false;
    }

    for (int i = 0; i < a->length; ++i) {
        if (a->arr[i] != b->arr[i]) {
            return false;
        }
    }

    return true;
}
bool string_equals_litn(const string_t *a, const char *b, const size_t b_len) {
    if (a->length != b_len) {
        return false;
    }

    for (int i = 0; i < a->length; ++i) {
        if (a->arr[i] != b[i]) {
            return false;
        }
    }

    return true;
}
bool string_equals_lit(const string_t *a, const char *b) {
    return string_equals_litn(a, b, strlen(b));
}
void string_to_cstrn(const string_t *str, char *const buf, size_t size) {
    for (int i = 0; i < str->length && i < size - 1; ++i) {
        buf[i] = str->arr[i];
    }
    buf[size - 1] = '\0';
}
