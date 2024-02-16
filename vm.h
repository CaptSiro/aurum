//
// Created by CaptSiro on 1/11/2024.
//

#ifndef AURUM_VM_H
#define AURUM_VM_H

#include "array.h"
#include "types.h"
#include "bytecode.h"



DYN_ARRAY(value_stack, value_t)
DYN_ARRAY(segment_array, segment_t *)

typedef struct {
    value_stack_t *stack;
    uint64 ip;
    segment_array_t *code;
} vm_t;

vm_t *vm_create();
void vm_free(vm_t **);
void vm_load(vm_t *, segment_t *);
void vm_execute(vm_t *);


#endif