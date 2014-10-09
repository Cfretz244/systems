build:
	gcc -std=gnu99 -g -c nodes.c
	gcc -std=gnu99 -g -c hash.c
	gcc -std=gnu99 -g -c splitter.c
	gcc -std=gnu99 -g -o main main.c nodes.o hash.o splitter.o
	gcc -std=gnu99 -g -o index indexer.c nodes.o hash.o splitter.o
	rm *.o

clean:
	rm main
	rm index
