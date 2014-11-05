SEARCH = search
INDEX = index
HASH = hash.o
NODES = nodes.o
SPLITTER = splitter.o

CC = gcc
CFLAGS = -std=gnu99
DEBUG = -g

all: $(SEARCH) $(INDEX)

$(SEARCH): $(HASH) $(NODES)
	$(CC) $(CFLAGS) $(DEBUG) search.c $(HASH) $(NODES) -o $(SEARCH)

$(INDEX): $(HASH) $(NODES) $(SPLITTER)
	$(CC) $(CFLAGS) $(DEBUG) indexer.c $(HASH) $(NODES) $(SPLITTER) -o $(INDEX)

$(HASH): hash.c hash.h
	$(CC) $(CFLAGS) $(DEBUG) -c hash.c

$(NODES): nodes.c nodes.h
	$(CC) $(CFLAGS) $(DEBUG) -c nodes.c

$(SPLITTER): splitter.c splitter.h
	$(CC) $(CFLAGS) $(DEBUG) -c splitter.c

clean:
	rm $(SPLITTER)
	rm $(NODES)
	rm $(HASH)
	rm $(SEARCH)
	rm $(INDEX)
