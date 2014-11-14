BOOK = book
HASH = hash.o
ARRAY = array.o
LIST = list.o
BUSINESS = business.o

CC = gcc
CFLAGS = -std=gnu99 -pthread -g

all: $(BOOK)

$(BOOK): $(HASH) $(ARRAY) $(LIST) $(BUSINESS)
	$(CC) $(CFLAGS) -o book book.c $(HASH) $(ARRAY) $(LIST) $(BUSINESS)

$(HASH): hash.c hash.h
	$(CC) $(CFLAGS) -c hash.c -o $(HASH)

$(ARRAY): array.c array.h
	$(CC) $(CFLAGS) -c array.c -o $(ARRAY)

$(LIST): list.c list.h
	$(CC) $(CFLAGS) -c list.c -o $(LIST)

$(BUSINESS): business.c business.h
	$(CC) $(CFLAGS) -c business.c -o $(BUSINESS)

clean:
	rm *.o
	rm book
