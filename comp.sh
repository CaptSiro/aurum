gcc -g -c tokenizer.h tokenizer.c
gcc -g -c null-guards.h array.h queue.h types.h types.c
gcc -g -c main.c
gcc -g -o main main.o types.o tokenizer.o