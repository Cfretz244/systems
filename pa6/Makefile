TEST = test
MALLOC = mem_alloc.o

CC = gcc
CFLAGS = -std=gnu99 -lm -g

all: $(TEST)

$(TEST): $(MALLOC)
	$(CC) $(CFLAGS) -o test test.c $(MALLOC)

$(MALLOC): mem_alloc.c mem_alloc.h
	$(CC) $(CFLAGS) -c mem_alloc.c -o $(MALLOC)

clean:
	rm *.o
	rm test
