BOOK = book
HASH = hash.o
LIST = list.o
BUSINESS = business.o

CC = gcc
CFLAGS = -std=gnu99 -pthread -g

all: $(BOOK)

$(BOOK): $(HASH) $(LIST) $(BUSINESS)
	$(CC) $(CFLAGS) -o book book.c $(HASH) $(LIST) $(BUSINESS)

$(HASH): thread_hash.c thread_hash.h
	$(CC) $(CFLAGS) -c thread_hash.c -o $(HASH)

$(LIST): list.c list.h
	$(CC) $(CFLAGS) -c list.c -o $(LIST)

$(BUSINESS): business.c business.h
	$(CC) $(CFLAGS) -c business.c -o $(BUSINESS)

clean:
	rm *.o
	rm book
