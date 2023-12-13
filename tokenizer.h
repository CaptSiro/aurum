#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdio.h>
#include "queue.h"
#include "types.h"



typedef enum {
    // keywords
    TOKEN_CONST,
    TOKEN_MUT,
    TOKEN_FN,
    TOKEN_RETURN,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_SWITCH,
    TOKEN_DEFAULT,
    TOKEN_WHILE,
    TOKEN_DO,
    TOKEN_FOR,
    TOKEN_BREAK,
    TOKEN_CONTINUE,

    TOKEN_IDENT,
    TOKEN_OPERATOR,

    TOKEN_OPERATOR_ASSIGN,

    TOKEN_ASSIGN,
    TOKEN_SEMICOLON,
    TOKEN_COLON,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_RANGE,

    TOKEN_ROUND_BRACKET_LEFT,
    TOKEN_ROUND_BRACKET_RIGHT,
    TOKEN_SQUARE_BRACKET_LEFT,
    TOKEN_SQUARE_BRACKET_RIGHT,
    TOKEN_CURLY_BRACKET_LEFT,
    TOKEN_CURLY_BRACKET_RIGHT,

    TOKEN_NULL,
    TOKEN_BOOL,
    TOKEN_NUMBER,
    TOKEN_CHAR,
    TOKEN_STRING,

    TOKEN_COMMENT,
    TOKEN_UNKNOWN,
    TOKEN_EOF,
} token_e;
typedef struct {
    int unit;
    size_t line;
    size_t column;
} file_unit_t;

typedef struct {
    token_e type;
    string *literal;
    uint64 line;
    uint64 column;
} token_t;

void token_print(token_t *token);
void token_free(token_t **token);
string *token_steal_literal(token_t *token);



DYN_QUEUE(unit, file_unit_t)
DYN_QUEUE(token, token_t *)

typedef struct {
    FILE *stream;
    unit_queue_t *units;
    token_queue_t *queue;

    uint64 line;
    uint64 column;
} tokenizer_t;

tokenizer_t *tokenizer_create(FILE *stream);
void tokenizer_free(tokenizer_t **tokenizer);
token_t *read_token(tokenizer_t *tokenizer);

#endif
