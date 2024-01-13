#ifndef TYPES_H
#define TYPES_H

#include <string.h>
#include <stdio.h>
#include "array.h"
#include "stdbool.h"

DYN_ARRAY(string, char)



typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

void string_print(const string *str);
string *string_allocate(const char *literal);
bool string_equals(const string *a, const string *b);
bool string_equals_lit(const string *a, const char *b);
bool string_equals_litn(const string *a, const char *b, size_t b_len);



typedef double value_t;

#endif