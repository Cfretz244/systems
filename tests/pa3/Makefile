build:
	gcc -std=gnu99 -g -c nodes.c
	gcc -std=gnu99 -g -c hash.c
	gcc -std=gnu99 -g -c splitter.c
	gcc -std=gnu99 -g -o index indexer.c nodes.o hash.o splitter.o

clean:
	rm index
	rm *.o
