#include <stdlib.h>
#include <stdio.h>
#include "tokenizer.h"
#include "ast.h"



FILE *get_stream(int argc, char *argv[]);
void close_stream(FILE *stream, int argc);



//int main(int argc, char *argv[]) {
//    FILE *in = get_stream(argc, argv);
//    if (in == NULL) {
//        printf("File pointer is NULL.\n");
//        return 1;
//    }
//
//    tokenizer_t *tokenizer = tokenizer_create(in);
//    if (tokenizer == NULL) {
//        printf("Could not create tokenizer.\n");
//        return 1;
//    }
//
//    int i = 0;
//    while (true) {
//        if (i++ == 100) {
//            break;
//        }
//
//        token_t *token = read_token(tokenizer);
//
//        if (token->type == TOKEN_EOF) {
//            token_free(&token);
//            break;
//        }
//
//        token_print(token);
//        token_free(&token);
//    }
//
//    tokenizer_free(&tokenizer);
//    fclose(in);
//
//    return 0;
//}



//int main() {
//    segment_t *segment = segment_create();
//
//    segment_load_const(segment, 69);
//    segment_load_const(segment, 351);
//
//    bytecode_push(segment->bytes, OP_BINARY_OP);
//    bytecode_push(segment->bytes, BOP_ADD);
//
//    bytecode_push(segment->bytes, OP_PRINT);
//
//    segment_load_const(segment, 20);
//
//    bytecode_push(segment->bytes, OP_BINARY_OP);
//    bytecode_push(segment->bytes, BOP_DIVIDE);
//
//    bytecode_push(segment->bytes, OP_PRINT);
//
//    vm_t *vm = vm_create();
//    vm_load(vm, segment);
//    vm_execute(vm);
//
//    segment_free(&segment);
//    vm_free(&vm);
//
//    return 0;
//}


//DYN_MAP(ci_map, int, double)
//DYN_MAP_PRINT(ci_map)
//DYN_MAP_FN(ci_map, int, double)
//DYN_MAP_FN_PRINT(ci_map, "%d", "%lf")
//
//int char_equals(int a, int b) {
//    return a == b;
//}
//
//int main() {
//    ci_map_t *map = ci_map_create(char_equals);
//
//    ci_map_set(map, 'a', 50);
//    ci_map_set(map, 'b', 420);
//    ci_map_set(map, 'a', 69);
//
//    ci_map_print(map);
//
//    double *x = ci_map_get(map, 'b');
//    if (x != NULL) {
//        printf("%lf\n", *x);
//    }
//
//    ci_map_free(&map);
//
//    return 0;
//}

int main(int argc, char *argv[]) {
    FILE *stream = get_stream(argc, argv);
    tokenizer_t *tokenizer = tokenizer_create(stream);

    ast_t *ast = ast_parse(tokenizer);
    ast_print(ast, 0);

    ast_free(&ast);
    tokenizer_free(&tokenizer);
    close_stream(stream, argc);
    return 0;
}





FILE *get_stream(int argc, char *argv[]) {
    if (argc <= 1) {
        return stdin;
    }

    return fopen(argv[1], "r");
}
void close_stream(FILE *stream, int argc) {
    if (argc <= 1) {
        return;
    }

    fclose(stream);
}
