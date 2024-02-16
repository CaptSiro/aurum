#ifndef TYPES_H
#define TYPES_H

#include <string.h>
#include <stdio.h>
#include "array.h"
#include "stdbool.h"



#define COLOR_RED     "\x1b[31m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"



#define EC_TODO 1
#define EC_UNEXPECTED_TOKEN 2



DYN_ARRAY(string, char)



typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;



#define TODO(WHAT) \
fprintf(stderr, "TODO: %s [%s:%d]\n", WHAT, __FUNCTION__, __LINE__); \
exit(EC_TODO);



void string_print(const string_t *str);
string_t *string_allocate(const char *literal);
bool string_equals(const string_t *a, const string_t *b);
bool string_equals_lit(const string_t *a, const char *b);
bool string_equals_litn(const string_t *a, const char *b, size_t b_len);
void string_to_cstrn(const string_t *str, char *buf, size_t size);

#define STRING_TO_CSTR(STRING, OUT_NAME) \
char OUT_NAME[STRING->length + 1]; \
string_to_cstrn(STRING, OUT_NAME, STRING->length + 1) \


typedef double value_t;

#endif