MAIN = search
HASH = hash.o
NODES = nodes.o

CC = gcc
CFLAGS = -std=gnu99
DEBUG = -g

all: $(MAIN)

$(MAIN) : $(HASH) $(NODES)
	$(CC) $(CFLAGS) $(DEBUG) search.c $(HASH) $(NODES) -o $(MAIN)

$(HASH): hash.c hash.h
	$(CC) $(CFLAGS) $(DEBUG) -c hash.c

$(NODES): nodes.c nodes.h
	$(CC) $(CFLAGS) $(DEBUG) -c nodes.c

clean:
	rm $(HASH)
	rm $(NODES)
	rm $(MAIN)
