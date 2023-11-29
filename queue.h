#include "null-guards.h"
#include <stdlib.h>



#define DYN_QUEUE(TYPE, ITEM) \
typedef struct { \
    int size; \
    int buffer_size; \
    ITEM *buffer; \
 \
    int read; \
    int write; \
} TYPE##_queue_t; \
 \
TYPE##_queue_t *TYPE##_queue_create(int capacity); \
void TYPE##_queue_free(TYPE##_queue_t *queue); \
void TYPE##_queue_push(TYPE##_queue_t *queue, ITEM item); \
ITEM TYPE##_queue_shift(TYPE##_queue_t *queue); \
ITEM TYPE##_queue_get(TYPE##_queue_t *queue, size_t index); \
size_t TYPE##_queue_size(TYPE##_queue_t *queue);   \



#define BUFFER_INDEX(queue, i) ((i + queue->read) % queue->buffer_size)



#define DYN_QUEUE_FN(TYPE, ITEM, NULL_ITEM) \
TYPE##_queue_t *TYPE##_queue_create(int capacity){ \
    TYPE##_queue_t *queue = malloc(sizeof(TYPE##_queue_t));\
    if (queue == NULL) {\
        return NULL;\
    }\
    \
    queue->buffer = (ITEM *)malloc(sizeof(ITEM) * capacity);\
    if (queue->buffer == NULL) {\
        return NULL;\
    }\
    \
    queue->size = 0;\
    queue->buffer_size = capacity;\
    queue->read = 0;\
    queue->write = 0;\
    \
    return queue;\
} \
                                 \
void TYPE##_queue_free(TYPE##_queue_t *queue) {     \
     RELEASE(queue->buffer)      \
     RELEASE(queue)\
}                                \
                                 \
void TYPE##_queue_push(TYPE##_queue_t *queue, ITEM item) { \
    if (queue->read == queue->write && queue->size > 0) { \
        int buffer_size = queue->buffer_size * 2; \
        ITEM *temp = (ITEM *)malloc(sizeof(ITEM) * buffer_size); \
        if (temp == NULL) { \
            return; \
        } \
                                 \
        for (int i = 0; i < queue->size && i < buffer_size; ++i) { \
            temp[i] = queue->buffer[BUFFER_INDEX(queue, i)]; \
        } \
        \
        RELEASE(queue->buffer); \
        queue->buffer = temp; \
        queue->buffer_size = buffer_size; \
        \
        queue->read = 0; \
        queue->write = queue->size; \
    } \
                                 \
    queue->buffer[queue->write++] = item; \
    queue->write %= queue->buffer_size; \
    queue->size++; \
} \
                                 \
ITEM TYPE##_queue_shift(TYPE##_queue_t *queue) {   \
    if (queue->size == 0) { \
        return NULL_ITEM; \
    } \
    \
    ITEM item = queue->buffer[queue->read++]; \
    queue->read %= queue->buffer_size; \
    queue->size--; \
    \
    if (queue->size != 0 && queue->size < queue->buffer_size / 3) {\
        int buffer_size = queue->buffer_size / 2;          \
        ITEM *temp = malloc((size_t)(sizeof(ITEM) * buffer_size)); \
        if (temp == NULL) { \
            return item; \
        } \
                                            \
        for (int i = 0; i < queue->size && i < buffer_size; ++i) { \
            temp[i] = queue->buffer[BUFFER_INDEX(queue, i)]; \
        } \
        \
        RELEASE(queue->buffer); \
        queue->buffer = temp; \
        queue->buffer_size = buffer_size; \
        \
        queue->read = 0; \
        queue->write = queue->size; \
    }                                       \
                                            \
    return item; \
} \
                                 \
ITEM TYPE##_queue_get(TYPE##_queue_t *queue, size_t index) {      \
    if (queue->size == 0 || index >= queue->size || index < 0) { \
        return NULL_ITEM; \
    } \
    \
    return queue->buffer[BUFFER_INDEX(queue, index)]; \
} \
                                 \
size_t TYPE##_queue_size(TYPE##_queue_t *queue) {  \
    return queue->size; \
}                                           \

