#include "null-guards.h"



#ifndef ARRAY_H
#define ARRAY_H
#define DYN_ARRAY_INITIAL_LENGTH 4

#define DYN_ARRAY(TYPE, ITEM_TYPE) \
typedef struct {                     \
    int length;                   \
    int max_length;               \
    ITEM_TYPE* arr; \
} TYPE;\
TYPE* TYPE##_create();    \
void TYPE##_free(TYPE* arr); \
int TYPE##_push(TYPE* arr, ITEM_TYPE item); \

#define DYN_ARRAY_FN(TYPE, ITEM_TYPE) \
TYPE* TYPE##_create() {           \
    TYPE* t = malloc(sizeof(TYPE)); \
    NULL_CHECKN(t)                \
                                  \
    t->arr = malloc(sizeof(ITEM_TYPE) * DYN_ARRAY_INITIAL_LENGTH); \
    if (t->arr == NULL) {         \
        free(t);                  \
        return NULL;              \
    }                             \
    \
    t->length = 0;                \
    t->max_length = DYN_ARRAY_INITIAL_LENGTH;\
                                  \
    return t;\
}                                 \
                                  \
void TYPE##_free(TYPE* t) {       \
    RELEASE(t->arr)                 \
    RELEASE(t)                      \
}\
                                  \
int TYPE##_push(TYPE* t, ITEM_TYPE item) {\
    if (t->length == t->max_length) {\
        int new_length = t->max_length * 2;\
                                  \
        ITEM_TYPE* ptr = realloc(t->arr, sizeof(ITEM_TYPE) * new_length); \
        NULL_CHECKR(ptr, 0)       \
                                  \
        t->arr = ptr;             \
        t->max_length = new_length;\
    }                             \
                                  \
    t->arr[t->length++] = item;   \
                                  \
    return 1;\
}                                 \

#define DYN_ARRAY_POP(TYPE, ITEM_TYPE, NULL_TYPE) \
ITEM_TYPE TYPE##_pop(TYPE* t) {  \
    if (t->length == 0) {         \
        return NULL_TYPE;\
    }                                         \
    t->length--;                              \
    if (t->length < t->max_length / 3) {      \
        int new_length = t->max_length / 2;\
                                  \
        ITEM_TYPE* ptr = realloc(t->arr, sizeof(ITEM_TYPE) * new_length); \
        NULL_CHECKR(ptr, NULL_TYPE)\
                                  \
        t->arr = ptr;             \
        t->max_length = new_length;\
    }                                  \
    \
    return t->arr[t->length];\
}\

#define DYN_ARRAY_PRINT(TYPE, PRINTF) \
void TYPE##_print(TYPE* t) {      \
    putchar('[');                  \
    if (t->length != 0) {         \
        printf(PRINTF, t->arr[0]);\
    }                             \
                                  \
    for (int i = 1; i < t->length; ++i) {\
        printf(", " PRINTF, t->arr[i]);\
    }                             \
                                  \
    putchar(']');\
    putchar('\n');\
}                                     \

#endif