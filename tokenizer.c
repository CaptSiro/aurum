#include "tokenizer.h"

DYN_QUEUE_FN(token, token_t *, NULL)
DYN_QUEUE_FN(unit, file_unit_t, (file_unit_t) { 0 })



char *keywords[] = {
    "const", "mut",
    "fn", "return",
    "if", "else",
    "switch", "default",
    "while", "do", "for", "break", "continue",
    NULL
};

token_t *token_create(const token_e type) {
    token_t *token = allocate(sizeof(token_t));

    token->literal = string_create();
    token->type = type;
    token->column = 0;
    token->line = 0;

    return token;
}
void token_print(token_t *token) {
    switch (token->type) {
        case TOKEN_CONST:
            printf("TOKEN_CONST");
            break;
        case TOKEN_MUT:
            printf("TOKEN_MUT");
            break;
        case TOKEN_FN:
            printf("TOKEN_FN");
            break;
        case TOKEN_RETURN:
            printf("TOKEN_RETURN");
            break;
        case TOKEN_IF:
            printf("TOKEN_IF");
            break;
        case TOKEN_ELSE:
            printf("TOKEN_ELSE");
            break;
        case TOKEN_SWITCH:
            printf("TOKEN_SWITCH");
            break;
        case TOKEN_DEFAULT:
            printf("TOKEN_DEFAULT");
            break;
        case TOKEN_WHILE:
            printf("TOKEN_WHILE");
            break;
        case TOKEN_DO:
            printf("TOKEN_DO");
            break;
        case TOKEN_FOR:
            printf("TOKEN_FOR");
            break;
        case TOKEN_BREAK:
            printf("TOKEN_BREAK");
            break;
        case TOKEN_CONTINUE:
            printf("TOKEN_CONTINUE");
            break;
        case TOKEN_IDENT:
            printf("TOKEN_IDENT");
            break;
        case TOKEN_OPERATOR:
            printf("TOKEN_OPERATOR");
            break;
        case TOKEN_OPERATOR_ASSIGN:
            printf("TOKEN_OPERATOR_ASSIGN");
            break;
        case TOKEN_ASSIGN:
            printf("TOKEN_ASSIGN");
            break;
        case TOKEN_SEMICOLON:
            printf("TOKEN_SEMICOLON");
            break;
        case TOKEN_COLON:
            printf("TOKEN_COLON");
            break;
        case TOKEN_COMMA:
            printf("TOKEN_COMMA");
            break;
        case TOKEN_DOT:
            printf("TOKEN_DOT");
            break;
        case TOKEN_RANGE:
            printf("TOKEN_RANGE");
            break;
        case TOKEN_ROUND_BRACKET_LEFT:
            printf("TOKEN_ROUND_BRACKET_LEFT");
            break;
        case TOKEN_ROUND_BRACKET_RIGHT:
            printf("TOKEN_ROUND_BRACKET_RIGHT");
            break;
        case TOKEN_SQUARE_BRACKET_LEFT:
            printf("TOKEN_SQUARE_BRACKET_LEFT");
            break;
        case TOKEN_SQUARE_BRACKET_RIGHT:
            printf("TOKEN_SQUARE_BRACKET_RIGHT");
            break;
        case TOKEN_CURLY_BRACKET_LEFT:
            printf("TOKEN_CURLY_BRACKET_LEFT");
            break;
        case TOKEN_CURLY_BRACKET_RIGHT:
            printf("TOKEN_CURLY_BRACKET_RIGHT");
            break;
        case TOKEN_NULL:
            printf("TOKEN_NULL");
            break;
        case TOKEN_BOOL:
            printf("TOKEN_BOOL");
            break;
        case TOKEN_NUMBER:
            printf("TOKEN_NUMBER");
            break;
        case TOKEN_CHAR:
            printf("TOKEN_CHAR");
            break;
        case TOKEN_STRING:
            printf("TOKEN_STRING");
            break;
        case TOKEN_UNKNOWN:
            printf("TOKEN_UNKNOWN");
            break;
        case TOKEN_EOF:
            printf("TOKEN_EOF");
            break;
    }

    printf(": '");
    string_print(token->literal);
    printf("' [%llu:%llu]\n", token->line, token->column);
}
void token_free(token_t **token) {
    string_free(&(*token)->literal);
    RELEASE((*token))
}
string *token_steal_literal(token_t *token) {
    string *temp = token->literal;
    token->literal = NULL;
    return temp;
}



tokenizer_t *tokenizer_create(FILE *stream) {
    tokenizer_t *tokenizer = allocate(sizeof(tokenizer_t));

    tokenizer->tokens = token_queue_create(4);
    tokenizer->units = unit_queue_create(2);
    tokenizer->stream = stream;
    tokenizer->line = 0;
    tokenizer->column = 0;

    return tokenizer;
}
void tokenizer_free(tokenizer_t **tokenizer) {
    if (*tokenizer == NULL) {
        return;
    }

    unit_queue_free((*tokenizer)->units);

    token_queue_t *queue = (*tokenizer)->tokens;
    for (int i = 0; i < token_queue_size(queue); ++i) {
        token_t *t = token_queue_get(queue, i);
        token_free(&t);
    }
    token_queue_free((*tokenizer)->tokens);

    RELEASE(*tokenizer)
}

void print_char(const char c) {
    if (c == '\0' || c == EOF) {
        printf("EOF");
        return;
    }

    if (c == '\n') {
        printf("\\n");
        return;
    }

    if (c == '\r') {
        printf("\\r");
        return;
    }

    printf("%c [%d]", c, c);
}

file_unit_t tokenizer_read_unit(tokenizer_t *tokenizer) {
    file_unit_t fu = { 0 };

    fu.unit = fgetc(tokenizer->stream);
    fu.column = tokenizer->column;
    fu.line = tokenizer->line;

    switch (fu.unit) {
        case '\n': {
            tokenizer->line++;
            tokenizer->column = 0;
            break;
        }
        case '\r': {
            tokenizer->column = 0;
            break;
        }
        case EOF: break;
        default: {
            tokenizer->column++;
        }
    }

    return fu;
}
file_unit_t tokenizer_units_load(tokenizer_t *tokenizer) {
    file_unit_t fu = tokenizer_read_unit(tokenizer);
    unit_queue_push(tokenizer->units, fu);
    return fu;
}
file_unit_t tokenizer_units_shift(tokenizer_t *tokenizer) {
    if (unit_queue_size(tokenizer->units) == 0) {
        return tokenizer_read_unit(tokenizer);
    }

    return unit_queue_shift(tokenizer->units);
}
file_unit_t tokenizer_units_peek(tokenizer_t *tokenizer) {
    if (unit_queue_size(tokenizer->units) == 0) {
        return tokenizer_units_load(tokenizer);
    }

    return unit_queue_get(tokenizer->units, 0);
}
file_unit_t tokenizer_units_peek_at(tokenizer_t *tokenizer, const size_t index) {
    size_t size = unit_queue_size(tokenizer->units);

    if (index < size) {
        return unit_queue_get(tokenizer->units, index);
    }

    for (int i = 0; i <= size - index; ++i) {
        tokenizer_units_load(tokenizer);
    }

    return unit_queue_get(tokenizer->units, index);
}
void tokenizer_units_skip(tokenizer_t *tokenizer, const size_t new_index) {
    for (int i = 0; i < new_index; ++i) {
        tokenizer_units_shift(tokenizer);
    }
}

token_t *token_from_char(const token_e type, const char literal) {
    token_t *token = token_create(type);
    string_push(token->literal, literal);
    return token;
}
token_t *token_from_unit(const token_e type, const file_unit_t unit) {
    token_t *token = token_create(type);

    string_push(token->literal, (char) unit.unit);
    token->column = unit.column;
    token->line = unit.line;

    return token;
}
#define UNPACK_UNIT(UNIT) UNIT.line, UNIT.column
token_t *token_from_literal(const token_e type, const char *literal, uint64 line, uint64 column) {
    token_t *token = token_create(type);

    token->line = line;
    token->column = column;

    for (int i = 0; literal[i] != '\0'; ++i) {
        string_push(token->literal, literal[i]);
    }

    return token;
}

#define IN_INTERVAL(VAR, FROM, TO) (VAR >= FROM && VAR <= TO)

bool is_alpha(const char c) {
    return IN_INTERVAL(c, 'a', 'z')
        || IN_INTERVAL(c, 'A', 'Z');
}
bool is_numeric(const char c) {
    return IN_INTERVAL(c, '0', '9');
}
bool is_ident_valid(const char c) {
    return is_alpha(c)
        || is_numeric(c)
        || c == '_';
}
bool match_seq(tokenizer_t *tokenizer, const char *seq) {
    for (int i = 0; seq[i] != '\0'; ++i) {
        file_unit_t fu = tokenizer_units_peek_at(tokenizer, i);

        if (fu.unit != seq[i]) {
            return false;
        }
    }

    return true;
}

token_t *read_string(tokenizer_t *tokenizer) {
    token_t *token = token_create(TOKEN_STRING);

    file_unit_t current = tokenizer_units_shift(tokenizer);
    while (current.unit != EOF) {
        if (current.unit == '"') {
            return token;
        }

        if (current.unit == '\\') {
            current = tokenizer_units_shift(tokenizer);

            switch (current.unit) {
                case 'n': {
                    string_push(token->literal, '\n');
                    break;
                }
                case 'r': {
                    string_push(token->literal, '\r');
                    break;
                }
                case 't': {
                    string_push(token->literal, '\t');
                    break;
                }
                case 'v': {
                    string_push(token->literal, '\v');
                    break;
                }
                case 'f': {
                    string_push(token->literal, '\f');
                    break;
                }
                case '\\': {
                    string_push(token->literal, '\\');
                    break;
                }
                default: {
                    token_free(&token);
                    return token_from_unit(TOKEN_UNKNOWN, current);
                }
            }

            current = tokenizer_units_shift(tokenizer);
            continue;
        }

        string_push(token->literal, (char) current.unit);
        current = tokenizer_units_shift(tokenizer);
    }

    token_free(&token);
    return token_from_char(TOKEN_EOF, '\0');
}
token_t *read_number(tokenizer_t *tokenizer, file_unit_t first_digit) {
    token_t *number = token_from_unit(TOKEN_NUMBER, first_digit);

    bool has_not_loaded_decimal_dot = true;
    while (true) {
        file_unit_t fu = tokenizer_units_load(tokenizer);

        if (is_numeric((char) fu.unit) == true) {
            tokenizer_units_skip(tokenizer, 1);
            string_push(number->literal, (char) fu.unit);
            continue;
        }

        if (fu.unit == '.' && has_not_loaded_decimal_dot) {
            has_not_loaded_decimal_dot = false;

            file_unit_t next = tokenizer_units_load(tokenizer);
            if (is_numeric((char) next.unit)) {
                file_unit_t dot = tokenizer_units_shift(tokenizer);
                file_unit_t digit = tokenizer_units_shift(tokenizer);

                string_push(number->literal, (char) dot.unit);
                string_push(number->literal, (char) digit.unit);
                continue;
            }
        }

        break;
    }

    return number;
}
token_t *read_ident(tokenizer_t *tokenizer, file_unit_t current) {
    if (!(is_alpha((char) current.unit) || current.unit == '_')) {
        return token_from_unit(TOKEN_UNKNOWN, current);
    }

    token_t *token = token_from_unit(TOKEN_IDENT, current);

    while (true) {
        file_unit_t fu = tokenizer_units_load(tokenizer);

        if (fu.unit == EOF || !is_ident_valid((char) fu.unit)) {
            break;
        }

        tokenizer_units_skip(tokenizer, 1);
        string_push(token->literal, (char) fu.unit);
    }

    return token;
}
token_t *read_comment_line(tokenizer_t *tokenizer) {
    while (true) {
        file_unit_t fu = tokenizer_units_peek(tokenizer);

        if (fu.unit == '\n' || fu.unit == EOF) {
            break;
        }

        tokenizer_units_skip(tokenizer, 1);
    }

    return read_token(tokenizer);
}
token_t *read_comment_block(tokenizer_t *tokenizer) {
    while (true) {
        file_unit_t fu = tokenizer_units_peek(tokenizer);

        if (match_seq(tokenizer, "*/") == true) {
            tokenizer_units_skip(tokenizer, 2);

            if (match_seq(tokenizer, "/*") == true) {
                tokenizer_units_skip(tokenizer, 2);
                continue;
            }

            break;
        }

        if (fu.unit == EOF) {
            return token_from_literal(TOKEN_UNKNOWN, "/*", UNPACK_UNIT(fu));
        }

        tokenizer_units_shift(tokenizer);
    }

    return read_token(tokenizer);
}

bool is_whitespace(const int c) {
    switch (c) {
        case '\n':
        case '\t':
        case '\r':
        case '\v':
        case '\f':
        case ' ':
        case 160:
            return true;
        default:
            return false;
    }
}
void skip_whitespace(tokenizer_t *tokenizer) {
    while (true) {
        file_unit_t fu = tokenizer_units_peek(tokenizer);

        if (!is_whitespace((char) fu.unit)) {
            break;
        }

        tokenizer_units_skip(tokenizer, 1);
    }
}

token_t *tokenize_op_assignment(file_unit_t operator, tokenizer_t *tokenizer) {
    file_unit_t fu = tokenizer_units_shift(tokenizer);

    if (fu.unit == '=') {
        return token_from_unit(TOKEN_OPERATOR_ASSIGN, operator);
    }

    return token_from_unit(TOKEN_OPERATOR, operator);
}



#define READ_NEXT file_unit_t next = tokenizer_units_peek(tokenizer);
#define DEFAULT(TYPE, LITERAL) return token_from_literal(TYPE, LITERAL, UNPACK_UNIT(next));
#define IF_FOLLOWED_BY(CHAR, TYPE, LITERAL) \
if (next.unit == CHAR) {                 \
    tokenizer_units_shift(tokenizer);      \
    return token_from_literal(TYPE, LITERAL, UNPACK_UNIT(next)); \
}



token_t *read_token(tokenizer_t *tokenizer) {
    skip_whitespace(tokenizer);
    file_unit_t current = tokenizer_units_shift(tokenizer);

    if (current.unit == EOF) {
        return token_create(TOKEN_EOF);
    }

    switch (current.unit) {
        case ';': return token_from_unit(TOKEN_SEMICOLON, current);
        case '(': return token_from_unit(TOKEN_ROUND_BRACKET_LEFT, current);
        case ')': return token_from_unit(TOKEN_ROUND_BRACKET_RIGHT, current);
        case '{': return token_from_unit(TOKEN_CURLY_BRACKET_LEFT, current);
        case '}': return token_from_unit(TOKEN_CURLY_BRACKET_RIGHT, current);
        case '[': return token_from_unit(TOKEN_SQUARE_BRACKET_LEFT, current);
        case ']': return token_from_unit(TOKEN_SQUARE_BRACKET_RIGHT, current);
        case ':': return token_from_unit(TOKEN_COLON, current);
        case ',': return token_from_unit(TOKEN_COMMA, current);

        case '?':
        case '~': return token_from_unit(TOKEN_OPERATOR, current);

        case '*':
        case '%':
        case '^': return tokenize_op_assignment(current, tokenizer);

        case '/': {
            READ_NEXT
            IF_FOLLOWED_BY('=', TOKEN_OPERATOR_ASSIGN, "/")

            if (next.unit == '/') {
                tokenizer_units_skip(tokenizer, 1);
                return read_comment_line(tokenizer);
            }

            if (next.unit == '*') {
                tokenizer_units_skip(tokenizer, 1);
                return read_comment_block(tokenizer);
            }

            DEFAULT(TOKEN_OPERATOR, "/")
        }
        case '+': {
            READ_NEXT
            IF_FOLLOWED_BY('+', TOKEN_OPERATOR, "++")
            IF_FOLLOWED_BY('=', TOKEN_OPERATOR_ASSIGN, "+")
            DEFAULT(TOKEN_OPERATOR, "+")
        }
        case '-': {
            READ_NEXT
            IF_FOLLOWED_BY('-', TOKEN_OPERATOR, "--")
            IF_FOLLOWED_BY('=', TOKEN_OPERATOR_ASSIGN, "-")
            DEFAULT(TOKEN_OPERATOR, "-")
        }

        case '.': {
            READ_NEXT
            IF_FOLLOWED_BY('.', TOKEN_RANGE, "..")
            DEFAULT(TOKEN_DOT, ".")
        }
        case '!': {
            READ_NEXT
            IF_FOLLOWED_BY('=', TOKEN_OPERATOR, "!=")
            DEFAULT(TOKEN_OPERATOR, "!")
        }
        case '=': {
            READ_NEXT
            IF_FOLLOWED_BY('=', TOKEN_OPERATOR, "==")
            DEFAULT(TOKEN_ASSIGN, "=")
        }
        case '&': {
            READ_NEXT
            IF_FOLLOWED_BY('&', TOKEN_OPERATOR, "&&")
            IF_FOLLOWED_BY('=', TOKEN_OPERATOR_ASSIGN, "&")
            DEFAULT(TOKEN_OPERATOR, "&")
        }
        case '|': {
            READ_NEXT
            IF_FOLLOWED_BY('|', TOKEN_OPERATOR, "||")
            IF_FOLLOWED_BY('=', TOKEN_OPERATOR_ASSIGN, "|")
            DEFAULT(TOKEN_OPERATOR, "|")
        }

        case '<': {
            READ_NEXT

            if (next.unit == '<') {
                next = tokenizer_units_shift(tokenizer);

                if (next.unit == '=') {
                    return token_from_literal(TOKEN_OPERATOR_ASSIGN, "<<=", UNPACK_UNIT(next));
                }

                DEFAULT(TOKEN_OPERATOR, "<<")
            }

            IF_FOLLOWED_BY('=', TOKEN_OPERATOR, "<=")
            DEFAULT(TOKEN_OPERATOR, "<")
        }
        case '>': {
            READ_NEXT

            if (next.unit == '>') {
                next = tokenizer_units_shift(tokenizer);

                if (next.unit == '=') {
                    return token_from_literal(TOKEN_OPERATOR_ASSIGN, ">>=", UNPACK_UNIT(next));
                }

                DEFAULT(TOKEN_OPERATOR, ">>")
            }

            IF_FOLLOWED_BY('=', TOKEN_OPERATOR, ">=")
            DEFAULT(TOKEN_OPERATOR, ">")
        }

        case '\'': {
            file_unit_t fu = tokenizer_units_shift(tokenizer);

            if (fu.unit == EOF) {
                fu.unit = '\0';
                return token_from_unit(TOKEN_UNKNOWN, fu);
            }

            file_unit_t closing = tokenizer_units_shift(tokenizer);

            if (closing.unit != '\'') {
                return token_from_unit(TOKEN_UNKNOWN, closing);
            }

            tokenizer_units_skip(tokenizer, 1);
            return token_from_unit(TOKEN_CHAR, fu);
        }
        case '"': {
            return read_string(tokenizer);
        }

        default: {
            if (is_numeric((char) current.unit)) {
                return read_number(tokenizer, current);
            }

            token_t *ident = read_ident(tokenizer, current);

            if (ident->type == TOKEN_UNKNOWN) {
                return ident;
            }

            if (string_equals_lit(ident->literal, "null")) {
                ident->type = TOKEN_NULL;
                return ident;
            }

            if (string_equals_lit(ident->literal, "true")
                || string_equals_lit(ident->literal, "false")) {
                ident->type = TOKEN_BOOL;
                return ident;
            }

            for (size_t i = 0; keywords[i] != NULL; ++i) {
                if (string_equals_lit(ident->literal, keywords[i])) {
                    ident->type = i;
                    return ident;
                }
            }

            return ident;
        }
    }
}



token_t *tokenizer_next(tokenizer_t *tokenizer) {
    if (token_queue_size(tokenizer->tokens) != 0) {
        return token_queue_shift(tokenizer->tokens);
    }

    return read_token(tokenizer);
}
token_t *tokenizer_peek(tokenizer_t *tokenizer, size_t index) {
    size_t size = token_queue_size(tokenizer->tokens);

    if (index < size) {
        return token_queue_get(tokenizer->tokens, index);
    }

    for (int i = 0; i <= index - size; ++i) {
        token_queue_push(tokenizer->tokens, read_token(tokenizer));
    }

    return token_queue_get(tokenizer->tokens, index);
}
