#include "memory.h"



#ifndef ARRAY_H
#define ARRAY_H
#define DYN_ARRAY_INITIAL_LENGTH 4

#define DYN_ARRAY(TYPE, ITEM_TYPE) \
typedef struct {                     \
    int length;                   \
    int max_length;               \
    ITEM_TYPE *arr; \
} TYPE##_t;\
TYPE##_t *TYPE##_create();    \
void TYPE##_free(TYPE##_t **arr); \
void TYPE##_push(TYPE##_t *arr, ITEM_TYPE item); \

#define DYN_ARRAY_FN(TYPE, ITEM_TYPE) \
TYPE##_t *TYPE##_create() {           \
    TYPE##_t *t = allocate(sizeof(TYPE##_t)); \
                                      \
    t->arr = allocate(sizeof(ITEM_TYPE) * DYN_ARRAY_INITIAL_LENGTH); \
    t->length = 0;                \
    t->max_length = DYN_ARRAY_INITIAL_LENGTH;\
                                  \
    return t;\
}                                 \
                                  \
void TYPE##_free(TYPE##_t **t) {       \
    RELEASE((*t)->arr)                 \
    RELEASE(*t)                      \
}\
                                  \
void TYPE##_push(TYPE##_t *t, ITEM_TYPE item) {\
    if (t->length == t->max_length) {\
        int new_length = t->max_length * 2;\
                                  \
        t->arr = reallocate(t->arr, sizeof(ITEM_TYPE) * new_length); \
        t->max_length = new_length;\
    }                             \
                                  \
    t->arr[t->length++] = item;   \
}                                 \

#define DYN_ARRAY_POP(TYPE, ITEM_TYPE, NULL_TYPE) \
ITEM_TYPE TYPE##_pop(TYPE##_t *t) {  \
    if (t->length == 0) {         \
        return NULL_TYPE;\
    }                                         \
    t->length--;                              \
    if (t->length < t->max_length / 3) {      \
        int new_length = t->max_length / 2;\
                                  \
        t->arr = reallocate(t->arr, sizeof(ITEM_TYPE) * new_length); \
        t->max_length = new_length;\
    }                                  \
    \
    return t->arr[t->length];\
}\

#define DYN_ARRAY_PRINT(TYPE, PRINTF) \
void TYPE##_print(TYPE##_t *t) {      \
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