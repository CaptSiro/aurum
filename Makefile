CC:=gcc
CFLAGS+=-g

TARGET=aurum
SRC := tokenizer.c types.c main.c
RELEASE_OBJS := $(SRC:%.c=release/%.o)

release: $(RELEASE_OBJS)

$(RELEASE_OBJS): release/%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

release:
	@$(CC) $^ -o "./release/$(TARGET)"

run: $(INPUT)
	@make bin --no-print-directory
	"./release/$(TARGET)" $(INPUT)

mleak: $(INPUT)
	@make bin --no-print-directory
	valgrind --leak-check=full --show-leak-kinds=all "./release/$(TARGET)" $(INPUT)
