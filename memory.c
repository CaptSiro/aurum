//
// Created by CaptSiro on 12/19/2023.
//

#include "memory.h"



void *allocate(size_t size) {
    void *memory = malloc(size);

    if (memory == NULL) {
        fprintf(stderr, "Failed to allocate chunk of size %zu B.\nTip: Buy or download more RAM.\n", size);
        exit(MEMORY_ERROR_CODE);
    }

    return memory;
}
void *reallocate(void *memory, size_t new_size) {
    void *t = realloc(memory, new_size);

    if (t == NULL) {
        fprintf(stderr, "Failed to reallocate chunk to size %zu B.\nTip: Buy or download more RAM.\n", new_size);
        exit(MEMORY_ERROR_CODE);
    }

    return t;
}
