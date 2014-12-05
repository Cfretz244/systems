TEST = test
MALLOC = mem_alloc.o
ARRAY = array.o

CC = gcc
CFLAGS = -std=gnu99 -lm -g

all: $(TEST)

$(TEST): $(MALLOC) $(ARRAY)
	$(CC) $(CFLAGS) -o test test.c $(MALLOC) $(ARRAY)

$(MALLOC): mem_alloc.c mem_alloc.h
	$(CC) $(CFLAGS) -c mem_alloc.c -o $(MALLOC)

$(ARRAY): array.h array.c
	$(CC) $(CFLAGS) -c array.c -o $(ARRAY)

clean:
	rm *.o
	rm test
