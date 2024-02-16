//
// Created by CaptSiro on 1/11/2024.
//

#include "vm.h"

DYN_ARRAY_FN(value_stack, value_t)
DYN_ARRAY_POP(value_stack, value_t, 0)
DYN_ARRAY_FN(segment_array, segment_t *)



void stack_push(vm_t *vm, value_t value) {
    value_stack_push(vm->stack, value);
}
value_t stack_pop(vm_t *vm) {
    return value_stack_pop(vm->stack);
}
value_t *stack_peek(vm_t *vm) {
    return &vm->stack->arr[vm->stack->length - 1];
}

value_t interpret_bop(value_t, value_t, bop_e);



vm_t *vm_create() {
    vm_t *vm = allocate(sizeof(vm_t));

    vm->ip = 0;
    vm->stack = value_stack_create();
    vm->code = segment_array_create();

    return vm;
}
void vm_free(vm_t **vm) {
    if (*vm == NULL) {
        return;
    }

    value_stack_free(&(*vm)->stack);
    segment_array_free(&(*vm)->code);

    RELEASE(*vm)
}
void vm_load(vm_t *vm, segment_t *segment) {
    segment_array_push(vm->code, segment);
}
void vm_execute(vm_t *vm) {
    for (int i = 0; i < vm->code->length; ++i) {
        segment_t *seg = vm->code->arr[i];
        vm->ip = 0;

        printf("Executing:\n");
        segment_print(seg);

        for (; vm->ip < seg->bytes->length; ++vm->ip) {
            opcode_e op = seg->bytes->arr[vm->ip];

            switch (op) {
                case OP_LOAD_CONST: {
                    stack_push(vm, seg->values->arr[seg->bytes->arr[++vm->ip]]);
                    break;
                }
                case OP_PRINT:
                    printf("%-10.3f\n", *stack_peek(vm));
                    break;
                case OP_BINARY_OP: {
                    value_t right = stack_pop(vm);
                    value_t left = stack_pop(vm);
                    stack_push(vm, interpret_bop(left, right, seg->bytes->arr[++vm->ip]));
                    break;
                }
            }
        }
    }
}



value_t interpret_bop(value_t left, value_t right, bop_e operator) {
    switch (operator) {
        case BOP_ADD:
            return left + right;
        case BOP_SUBTRACT:
            return left - right;
        case BOP_DIVIDE:
            // todo check for division by zero
            return left / right;
        case BOP_MULTIPLY:
            return left * right;
        case BOP_MOD:
            // todo make better
            return (value_t)((long long)(left) % (long long)(right));
    }
}
