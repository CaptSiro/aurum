//
// Created by CaptSiro on 1/12/2024.
//

#ifndef AURUM_MAP_H
#define AURUM_MAP_H

#include "array.h"



#define DYN_MAP(TYPE, KEY, ITEM) \
typedef struct {\
    KEY key;\
    ITEM value;\
} TYPE##_entry_t;\
\
DYN_ARRAY(TYPE##_entry_array, TYPE##_entry_t)\
typedef struct {\
    TYPE##_entry_array *array;\
    int(* equals)(const KEY, const KEY);\
} TYPE##_t;\
\
TYPE##_t *TYPE##_create(int (* equals) (const KEY, const KEY));\
void TYPE##_free(TYPE##_t **);\
void TYPE##_set(TYPE##_t *, KEY, ITEM);\
ITEM *TYPE##_get(TYPE##_t *, KEY);\
void TYPE##_print(TYPE##_t *);\



#define DYN_MAP_FN(TYPE, KEY, ITEM) \
DYN_ARRAY_FN(TYPE##_entry_array, TYPE##_entry_t) \
TYPE##_t *TYPE##_create(int (* equals) (const KEY, const KEY)) {\
    TYPE##_t *map = allocate(sizeof(TYPE##_t));\
\
    map->array = TYPE##_entry_array_create();\
    map->equals = equals;\
\
    return map;\
}\
                                    \
void TYPE##_free(TYPE##_t **map) {\
    if (*map == NULL) {\
        return;\
    }\
\
    TYPE##_entry_array_free(&(*map)->array);\
    RELEASE(*map)\
}\
                                    \
void TYPE##_set(TYPE##_t *map, KEY key, ITEM value) {\
    for (int i = 0; i < map->array->length; ++i) {\
        if (map->equals(map->array->arr[i].key, key)) {\
            map->array->arr[i].value = value;\
            return;\
        }\
    }\
\
    TYPE##_entry_array_push(map->array, (TYPE##_entry_t) {\
        .key = key,\
        .value = value\
    });\
}\
                                    \
ITEM *TYPE##_get(TYPE##_t *map, KEY key) {\
    for (int i = 0; i < map->array->length; ++i) {\
        if (map->equals(map->array->arr[i].key, key)) {\
            return &map->array->arr[i].value;\
        }\
    }\
\
    return NULL;\
}                                   \



#define DYN_MAP_PRINT(TYPE, KEY_PRINTABLE, ITEM_PRINTABLE) \
void TYPE##_print(TYPE##_t *map) {\
    if (!(map && map->array && map->array->arr)) {\
        puts("{}\n");\
        return;\
    }\
\
    puts("{");\
    for (int i = 0; i < map->array->length; ++i) {\
        if (i != 0) {\
            putchar('\n');\
        }\
        printf("\t'" KEY_PRINTABLE "': " ITEM_PRINTABLE ",", map->array->arr[i].key, map->array->arr[i].value);\
    }\
    puts("\n}");\
}                                   \



#endif //AURUM_MAP_H
