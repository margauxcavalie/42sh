# By default, $(CC) = cc (and cc is an alias for gcc)

CFLAGS = -Wall -Wextra -pedantic -std=c99 -fsanitize=address

BIN = a.out

OBJS = mypipe.o

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(LDLIBS) -o $@ $^ -fsanitize=address

# This rule is implicit:
# %.o: %.c:
# 	$(CC) $(CFLAGS) -o $@ $^
clean:
	rm -f $(BIN) $(OBJS)

.PHONY: clean all
