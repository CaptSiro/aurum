#ifndef NULL_GUARDS_H
#define NULL_GUARDS_H

#include <stdlib.h>

#define NULL_CHECK(VAR) \
if (VAR == NULL) {    \
    return;      \
}

#define NULL_CHECK_BREAK(VAR) \
if (VAR == NULL) {    \
    break;      \
}

#define NULL_CHECKN(VAR) \
if (VAR == NULL) {    \
    return NULL;      \
}

#define NULL_CHECKR(VAR, RET) \
if (VAR == NULL) {    \
    return RET;      \
}

#define RELEASE(memory) \
if (memory != NULL) { \
    free(memory);   \
    memory = NULL;\
}

#endif