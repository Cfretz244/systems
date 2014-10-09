build:
	gcc -std=c99 -g -c nodes.c
	gcc -std=c99 -g -c hash.c
	gcc -std=c99 -g -c splitter.c
	gcc -std=c99 -g -o main main.c nodes.o hash.o splitter.o
	rm *.o

clean:
	rm main
