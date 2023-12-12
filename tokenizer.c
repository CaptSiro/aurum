#include "tokenizer.h"

DYN_QUEUE_FN(token, token_t *, NULL)
DYN_QUEUE_FN(char, int, EOF)



char *keywords[] = {
    "const", "mut",
    "fn", "return",
    "if", "else",
    "switch", "default",
    "while", "do", "for", "break", "continue",
    NULL
};

token_t *token_create(const token_e type) {
    token_t *token = malloc(sizeof(token_t *));
    NULL_CHECKN(token)

    token->literal = string_create();
    if (token->literal == NULL) {
        RELEASE(token)
        return NULL;
    }

    token->type = type;

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
        case TOKEN_COMMENT:
            printf("TOKEN_COMMENT");
            break;
    }

    printf(": ");
    string_print(token->literal);
    putchar('\n');
}
void token_free(token_t **token) {
    RELEASE((*token)->literal)
    RELEASE((*token))
}
string *token_steal_literal(token_t *token) {
    string *temp = token->literal;
    token->literal = NULL;
    return temp;
}



tokenizer_t *tokenizer_create(FILE *stream) {
    tokenizer_t *tokenizer = malloc(sizeof(tokenizer_t *));
    NULL_CHECKN(tokenizer)

    tokenizer->queue = token_queue_create(4);
    if (tokenizer->queue == NULL) {
        RELEASE(tokenizer)
        return NULL;
    }

    tokenizer->chars = char_queue_create(2);
    if (tokenizer->chars == NULL) {
        token_queue_free(tokenizer->queue);
        RELEASE(tokenizer)
        return NULL;
    }

    tokenizer->stream = stream;
    return tokenizer;
}
void tokenizer_free(tokenizer_t **tokenizer) {
    if (*tokenizer == NULL) {
        return;
    }

    token_queue_t *queue = (*tokenizer)->queue;
    for (int i = 0; i < token_queue_size(queue); ++i) {
        token_t *t = token_queue_get(queue, i);
        token_free(&t);
    }
    RELEASE((*tokenizer)->queue)

    RELEASE((*tokenizer)->chars)
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

int tokenizer_load(tokenizer_t *tokenizer) {
    int c = fgetc(tokenizer->stream);
    char_queue_push(tokenizer->chars, c);
    return c;
}
int tokenizer_shift(tokenizer_t *tokenizer) {
    if (char_queue_size(tokenizer->chars) == 0) {
        return fgetc(tokenizer->stream);
    }

    return char_queue_shift(tokenizer->chars);
}
int tokenizer_peek(tokenizer_t *tokenizer) {
    if (char_queue_size(tokenizer->chars) == 0) {
        return tokenizer_load(tokenizer);
    }

    return char_queue_get(tokenizer->chars, 0);
}
int tokenizer_peek_at(tokenizer_t *tokenizer, size_t index) {
    size_t size = char_queue_size(tokenizer->chars);

    if (index < size) {
        return char_queue_get(tokenizer->chars, index);
    }

    for (int i = 0; i <= size - index; ++i) {
        tokenizer_load(tokenizer);
    }

    return char_queue_get(tokenizer->chars, index);
}
void tokenizer_set_start(tokenizer_t *tokenizer, size_t new_index) {
    for (int i = 0; i < new_index; ++i) {
        tokenizer_shift(tokenizer);
    }
}

token_t *token_from_char(const token_e type, const char literal) {
    token_t *token = token_create(type);
    NULL_CHECKN(token)

    string_push(token->literal, literal);

    return token;
}
token_t *token_from_char_load(const token_e type, const char literal, tokenizer_t *tokenizer) {
    token_t *token = token_create(type);
    NULL_CHECKN(token)

    string_push(token->literal, literal);
    tokenizer_load(tokenizer);

    return token;
}
token_t *token_from_literal(const token_e type, const char *literal) {
    token_t *token = token_create(type);
    NULL_CHECKN(token)

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
        int c = tokenizer_peek_at(tokenizer, i);

        if (c != seq[i]) {
            return false;
        }
    }

    return true;
}

token_t *read_string(tokenizer_t *tokenizer) {
    token_t *token = token_create(TOKEN_STRING);
    NULL_CHECKN(token)

    int current = tokenizer_shift(tokenizer);
    while (current != EOF) {
        if (current == '"') {
            return token;
        }

        if (current == '\\') {
            current = tokenizer_shift(tokenizer);

            switch (current) {
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
                    return token_from_char(TOKEN_UNKNOWN, (char) current);
                }
            }

            current = tokenizer_shift(tokenizer);
            continue;
        }

        string_push(token->literal, (char) current);
        current = tokenizer_shift(tokenizer);
    }

    token_free(&token);
    return token_from_char(TOKEN_EOF, '\0');
}
token_t *read_number(tokenizer_t *tokenizer, char first_digit) {
    token_t *number = token_from_char(TOKEN_NUMBER, first_digit);
    NULL_CHECKN(number)

    bool has_not_loaded_decimal_dot = true;
    while (true) {
        int c = tokenizer_load(tokenizer);

        if (is_numeric((char) c) == true) {
            string_push(number->literal, (char) tokenizer_shift(tokenizer));
            continue;
        }

        if (c == '.' && has_not_loaded_decimal_dot) {
            has_not_loaded_decimal_dot = false;

            int next = tokenizer_load(tokenizer);
            if (is_numeric((char) next)) {
                string_push(number->literal, (char) tokenizer_shift(tokenizer)); // .
                string_push(number->literal, (char) tokenizer_shift(tokenizer)); // 0-9
                continue;
            }
        }

        break;
    }

    return number;
}
token_t *read_ident(tokenizer_t *tokenizer, char current) {
    if (!(is_alpha(current) || current == '_')) {
        return token_from_char(TOKEN_UNKNOWN, current);
    }

    token_t *token = token_create(TOKEN_IDENT);
    NULL_CHECKN(token)

    string_push(token->literal, current);

    while (true) {
        int c = tokenizer_load(tokenizer);

        if (c == EOF || !is_ident_valid((char) c)) {
            break;
        }

        string_push(token->literal, (char) tokenizer_shift(tokenizer));
    }

    return token;
}
token_t *read_comment_line(tokenizer_t *tokenizer) {
    while (true) {
        int c = tokenizer_peek(tokenizer);

        if (c == '\n' || c == EOF) {
            break;
        }

        tokenizer_shift(tokenizer);
    }

    return read_token(tokenizer);
}
token_t *read_comment_block(tokenizer_t *tokenizer) {
    while (true) {
        int c = tokenizer_peek(tokenizer);

        if (match_seq(tokenizer, "*/") == true) {
            tokenizer_set_start(tokenizer, 2);

            if (match_seq(tokenizer, "/*") == true) {
                tokenizer_set_start(tokenizer, 2);
                continue;
            }

            break;
        }

        if (c == EOF) {
            return token_from_literal(TOKEN_UNKNOWN, "/*");
        }

        tokenizer_shift(tokenizer);
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
        int c = tokenizer_peek(tokenizer);

        if (!is_whitespace(c)) {
            break;
        }

        tokenizer_shift(tokenizer);
    }
}

token_t *tokenize_op_assignment(const char c, tokenizer_t *tokenizer) {
    const char next = (char) tokenizer_shift(tokenizer);

    if (next == '=') {
        return token_from_char_load(TOKEN_OPERATOR_ASSIGN, c, tokenizer);
    }

    return token_from_char_load(TOKEN_OPERATOR, c, tokenizer);
}



#define READ_NEXT char next = (char) tokenizer_peek(tokenizer);
#define DEFAULT(TYPE, LITERAL) return token_from_literal(TYPE, LITERAL);
#define IF_FOLLOWED_BY(CHAR, TYPE, LITERAL) \
if (next == CHAR) {                 \
    tokenizer_shift(tokenizer);      \
    return token_from_literal(TYPE, LITERAL); \
}



token_t *read_token(tokenizer_t *tokenizer) {
    skip_whitespace(tokenizer);
    int current = tokenizer_shift(tokenizer);

    if (current == EOF) {
        return token_create(TOKEN_EOF);
    }

    switch (current) {
        case ';': return token_from_char_load(TOKEN_SEMICOLON, ';', tokenizer);
        case '(': return token_from_char_load(TOKEN_ROUND_BRACKET_LEFT, '(', tokenizer);
        case ')': return token_from_char_load(TOKEN_ROUND_BRACKET_RIGHT, ')', tokenizer);
        case '{': return token_from_char_load(TOKEN_CURLY_BRACKET_LEFT, '{', tokenizer);
        case '}': return token_from_char_load(TOKEN_CURLY_BRACKET_RIGHT, '}', tokenizer);
        case '[': return token_from_char_load(TOKEN_SQUARE_BRACKET_LEFT, '[', tokenizer);
        case ']': return token_from_char_load(TOKEN_SQUARE_BRACKET_RIGHT, ']', tokenizer);
        case ':': return token_from_char_load(TOKEN_COLON, ':', tokenizer);
        case ',': return token_from_char_load(TOKEN_COMMA, ',', tokenizer);
        case '?': return token_from_char_load(TOKEN_OPERATOR, '?', tokenizer);
        case '~': return token_from_char_load(TOKEN_OPERATOR, '~', tokenizer);

        case '*': return tokenize_op_assignment('*', tokenizer);
        case '%': return tokenize_op_assignment('%', tokenizer);
        case '^': return tokenize_op_assignment('^', tokenizer);

        case '/': {
            READ_NEXT
            IF_FOLLOWED_BY('=', TOKEN_OPERATOR_ASSIGN, "/");

            if (next == '/') {
                tokenizer_set_start(tokenizer, 1);
                return read_comment_line(tokenizer);
            }

            if (next == '*') {
                tokenizer_set_start(tokenizer, 1);
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

            if (next == '<') {
                next = (char) tokenizer_shift(tokenizer);

                if (next == '=') {
                    return token_from_literal(TOKEN_OPERATOR_ASSIGN, "<<=");
                }

                DEFAULT(TOKEN_OPERATOR, "<<")
            }

            IF_FOLLOWED_BY('=', TOKEN_OPERATOR, "<=")
            DEFAULT(TOKEN_OPERATOR, "<")
        }
        case '>': {
            READ_NEXT

            if (next == '>') {
                next = (char) tokenizer_shift(tokenizer);

                if (next == '=') {
                    return token_from_literal(TOKEN_OPERATOR_ASSIGN, ">>=");
                }

                return token_from_literal(TOKEN_OPERATOR, ">>");
            }

            IF_FOLLOWED_BY('=', TOKEN_OPERATOR, ">=")
            DEFAULT(TOKEN_OPERATOR, ">")
        }

        case '\'': {
            int c = tokenizer_shift(tokenizer);

            if (c == EOF) {
                return token_from_char(TOKEN_UNKNOWN, '\0');
            }

            const char closing = (char) tokenizer_shift(tokenizer);

            if (closing != '\'') {
                return token_from_char(TOKEN_UNKNOWN, closing);
            }

            tokenizer_shift(tokenizer);
            return token_from_char(TOKEN_CHAR, (char) c);
        }
        case '"': {
            return read_string(tokenizer);
        }

        default: {
            if (is_numeric((char) current)) {
                return read_number(tokenizer, (char) current);
            }

            token_t *ident = read_ident(tokenizer, (char) current);
            NULL_CHECKN(ident)

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
