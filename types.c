#include "types.h"



DYN_ARRAY_FN(string, char)
void string_print(const string *str) {
    for (int i = 0; i < str->length; ++i) {
        putchar(str->arr[i]);
    }
}
string *string_malloc(const char *literal) {
    string *str = string_create();
    NULL_CHECKN(str)

    for (int i = 0; literal[i] != '\0'; ++i) {
        string_push(str, literal[i]);
    }

    return str;
}
bool string_equals(const string *a, const string *b) {
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
bool string_equals_litn(const string *a, const char *b, const size_t b_len) {
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
bool string_equals_lit(const string *a, const char *b) {
    return string_equals_litn(a, b, strlen(b));
}
