//
// Created by CaptSiro on 2/14/2024.
//

#ifndef AURUM_AST_H
#define AURUM_AST_H

#include "types.h"
#include "tokenizer.h"
#include "memory.h"



typedef enum {
    AST_COMPOUND_STMT,
    AST_EXPR,
    AST_UOP,
    AST_BOP,
    AST_REF,
    AST_FN_CALL,
    AST_LITERAL_NUMBER,
} ast_e;

typedef enum {
    BOP_SUB,
    BOP_ADD,
    BOP_DIV,
    BOP_MUL,
    BOP_MOD,
    BOP_COUNT,
} bop_e;

typedef enum {
    UOP_SUB,
    UOP_COUNT,
} uop_e;

typedef struct ast_t ast_t;
DYN_ARRAY(ast_array, ast_t *)

struct ast_t {
    ast_e type;
    union {
        ast_array_t *compound_stmt;

        ast_t *expr;

        struct {
            uop_e op;
            ast_t *operand;
        } uop;

        struct {
            bop_e op;
            uint32 precedence;
            ast_t *left;
            ast_t *right;
        } bop;

        struct {
            ast_array_t *params;
            string_t *name;
        } fn_call;

        string_t *ref;

        double number;
    };
};


void ast_free(ast_t **node);
ast_t *ast_parse(tokenizer_t *tokenizer);
void ast_print(ast_t *ast, size_t level);



#endif //AURUM_AST_H
