//
// Created by CaptSiro on 12/19/2023.
//

#ifndef AURUM_MEMORY_H
#define AURUM_MEMORY_H

#include <stdlib.h>
#include <stdio.h>

#ifndef MEMORY_ERROR_CODE
#define MEMORY_ERROR_CODE 2
#endif



void *allocate(size_t size);
void *reallocate(void *memory, size_t new_size);

#define RELEASE(memory) \
if (memory != NULL) {   \
    free(memory);       \
    memory = NULL;      \
}



#endif //AURUM_MEMORY_H
