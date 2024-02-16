//
// Created by CaptSiro on 2/14/2024.
//

#include "ast.h"

DYN_ARRAY_FN(ast_array, ast_t *)



const char *ast_literals[] = {
    "COMPOUND_STATEMENT",
    "EXPRESSION",
    "UNARY_OPERATION",
    "BINARY_OPERATION",
    "REFERENCE",
    "FUNCTION_CALL",
    "NUMBER",
};

const char *bop_literals[] = {
    "-",
    "+",
    "/",
    "*",
    "%",
};

const char *uop_literals[] = {
    "-",
};



ast_t *ast_create(ast_e type) {
    ast_t *node = allocate(sizeof(ast_t));
    node->type = type;
    return node;
}
uint32 bop_precedence(bop_e op) {
    switch (op) {
        case BOP_ADD:
        case BOP_SUB:
            return 1;
        case BOP_DIV:
        case BOP_MUL:
        case BOP_MOD:
            return 2;
        default:
            return 0;
    }
}
ast_t *ast_create_bop(token_t *token) {
    bop_e op = BOP_COUNT;
    for (int i = 0; i < BOP_COUNT; ++i) {
        if (string_equals_lit(token->literal, bop_literals[i]) == true) {
            op = i;
        }
    }
    if (op == BOP_COUNT) {
        fprintf(stderr, "Unknown operator.\n");
        token_print(token);
        exit(EC_UNEXPECTED_TOKEN);
    }

    ast_t *node = ast_create(AST_BOP);

    node->bop.op = op;
    node->bop.precedence = bop_precedence(op);
    node->bop.left = NULL;
    node->bop.right = NULL;

    return node;
}
ast_t *ast_create_uop(token_t *token) {
    uop_e op = UOP_COUNT;
    for (int i = 0; i < UOP_COUNT; ++i) {
        if (string_equals_lit(token->literal, uop_literals[i]) == true) {
            op = i;
        }
    }
    if (op == UOP_COUNT) {
        fprintf(stderr, "Unknown operator.\n");
        token_print(token);
        exit(EC_UNEXPECTED_TOKEN);
    }

    ast_t *node = ast_create(AST_UOP);

    node->uop.op = op;

    return node;
}
void ast_free(ast_t **node) {
    if (*node == NULL) {
        return;
    }

    ast_t *n = *node;

    switch (n->type) {
        case AST_COMPOUND_STMT:
            for (int i = 0; i < n->compound_stmt->length; ++i) {
                ast_free(&n->compound_stmt->arr[i]);
            }
            ast_array_free(&n->compound_stmt);
            break;
        case AST_EXPR:
            ast_free(&n->expr);
            break;
        case AST_UOP:
            ast_free(&n->uop.operand);
            break;
        case AST_BOP:
            ast_free(&n->bop.left);
            ast_free(&n->bop.right);
            break;
        case AST_FN_CALL:
            string_free(&n->fn_call.name);
            for (int i = 0; i < n->fn_call.params->length; ++i) {
                ast_free(&n->fn_call.params->arr[i]);
            }
            ast_array_free(&n->fn_call.params);
            break;
        case AST_REF:
            string_free(&n->ref);
            break;
        case AST_LITERAL_NUMBER:
            break;
    }

    RELEASE(*node)
}



ast_t *pe_set_unary(ast_t *unary, ast_t *node) {
    if (unary == NULL) {
        return node;
    }

    unary->uop.operand = node;
    return unary;
}
ast_t *pe_find_spot(ast_t *node) {
    if (node->type != AST_BOP) {
        return NULL;
    }

    while (node->type == AST_BOP && node->bop.right != NULL) {
        node = node->bop.right;
    }

    return node;
}
void pe_append_branch(ast_t **current, ast_t **unary, ast_t *node) {
    if (*current == NULL) {
        *current = pe_set_unary(*unary, node);
        *unary = NULL;
        return;
    }

    if ((*current)->type == AST_BOP) {
        ast_t *spot = pe_find_spot(*current);
        spot->bop.right = pe_set_unary(*unary, node);
    }

    *unary = NULL;
}
ast_t *pe_handle_ident(tokenizer_t *tokenizer, token_t *ident) {
    token_t *after = tokenizer_peek(tokenizer, 0);

    switch (after->type) {
        case TOKEN_DOT: {
            TODO("Handle dot function call.")
        }
        // TODO add token access '::'
//        case TOKEN_ACCESS: {
//
//        }
        case TOKEN_ROUND_BRACKET_LEFT: {
            TODO("Handle normal function call.")
        }
        case TOKEN_OPERATOR: {
            ast_t *ref = ast_create(AST_REF);
            ref->ref = token_steal_literal(ident);
            return ref;
        }
        default: {
            token_print(tokenizer_next(tokenizer));
            token_print(tokenizer_next(tokenizer));
            fprintf(stderr, "Unexpected token after identifier.\n");
            exit(EC_UNEXPECTED_TOKEN);
        }
    }
}
ast_t *parse_expression(tokenizer_t *tokenizer, const token_e *stop_token) {
    ast_t *root = ast_create(AST_EXPR);
    root->expr = NULL;

    ast_t *unary = NULL;
    ast_t *current = NULL;
    token_t *token = NULL;

    while (true) {
        token_free(&token);
        token = tokenizer_next(tokenizer);

        if (stop_token != NULL && token->type == *stop_token) {
            root->expr = current;
        }

        switch (token->type) {
            case TOKEN_SEMICOLON:
            case TOKEN_EOF: {
                root->expr = current;
                break;
            }

            case TOKEN_NUMBER: {
                ast_t *number = ast_create(AST_LITERAL_NUMBER);
                STRING_TO_CSTR(token->literal, buf);
                number->number = strtod(buf, NULL);
                pe_append_branch(&current, &unary, number);
                break;
            }

            case TOKEN_IDENT: {
                ast_t *resolved = pe_handle_ident(tokenizer, token);
                pe_append_branch(&current, &unary, resolved);
                break;
            }

            case TOKEN_OPERATOR: {
                if (current == NULL) {
                    unary = ast_create_uop(token);
                    break;
                }

                ast_t *bop = ast_create_bop(token);
//                tokenizer_skip(tokenizer, 2);

                if (current->type != AST_BOP) {
                    bop->bop.left = current;
                    current = bop;
                    break;
                }

                if (current->bop.precedence >= bop->bop.precedence) {
                    bop->bop.left = current;
                    current = bop;
                    break;
                }

                // until child of parent does not have higher precedence
                ast_t *parent = current;
                while ((parent->bop.right->type == AST_BOP)
                    && (parent->bop.right->bop.precedence < bop->bop.precedence)) {
                    parent = parent->bop.right;
                }

                bop->bop.left = parent->bop.right;
                parent->bop.right = bop;
                break;
            }

            default: {
                fprintf(stderr, "Unexpected token for expression.\n");
                token_print(token);
                exit(EC_UNEXPECTED_TOKEN);
            }
        }

        if (root->expr != NULL) {
            token_free(&token);

            if (unary != NULL) {
                ast_free(&unary);
            }

            break;
        }
    }

    return root;
}

ast_t *ast_parse(tokenizer_t *tokenizer) {
    ast_t *root = ast_create(AST_COMPOUND_STMT);
    root->compound_stmt = ast_array_create();

    token_t *token = NULL;
    while (true) {
        token = tokenizer_peek(tokenizer, 0);

        switch (token->type) {
            case TOKEN_OPERATOR:
            case TOKEN_NUMBER: {
                ast_array_push(root->compound_stmt, parse_expression(tokenizer, NULL));
                break;
            }
            case TOKEN_EOF: {
                goto end;
            }
            default: {
                token_print(token);
                TODO("Handle token.")
            }
        }
    }

    end:
    return root;
}
void ast_print(ast_t *ast, size_t level) {
    for (int i = 0; i < level * 2; ++i) {
        putchar(' ');
    }

    if (ast == NULL) {
        printf(COLOR_RED "NULL\n" COLOR_RESET);
        return;
    }

    switch (ast->type) {
        case AST_COMPOUND_STMT:
            printf(COLOR_YELLOW "%s\n" COLOR_RESET, ast_literals[ast->type]);
            for (int i = 0; i < ast->compound_stmt->length; ++i) {
                ast_print(ast->compound_stmt->arr[i], level + 1);
            }
            break;
        case AST_BOP:
            printf(COLOR_YELLOW "%s" COLOR_RESET COLOR_CYAN " '%s'\n" COLOR_RESET,
                ast_literals[ast->type],
                bop_literals[ast->bop.op]);

            ast_print(ast->bop.left, level + 1);
            ast_print(ast->bop.right, level + 1);
            break;
        case AST_UOP:
            printf(COLOR_YELLOW "%s" COLOR_RESET COLOR_CYAN " '%s'\n" COLOR_RESET,
                ast_literals[ast->type],
                uop_literals[ast->uop.op]);

            ast_print(ast->uop.operand, level + 1);
            break;
        case AST_LITERAL_NUMBER:
            printf(COLOR_YELLOW "%s" COLOR_RESET COLOR_CYAN " '%lf'\n" COLOR_RESET,
                ast_literals[ast->type],
                ast->number);
            break;
        case AST_EXPR:
            printf(COLOR_YELLOW "%s\n" COLOR_RESET,
                ast_literals[ast->type]);

            ast_print(ast->expr, level + 1);
            break;
        case AST_REF:
            printf(COLOR_YELLOW "%s" COLOR_RESET COLOR_CYAN " '",
                ast_literals[ast->type]);
            string_print(ast->ref);
            printf("'\n" COLOR_RESET);
            break;
        case AST_FN_CALL:
            break;
    }
}
