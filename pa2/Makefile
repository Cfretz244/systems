sorted-list:
	gcc -std=c99 -g -c sorted-list.c -o sorted-list.o
	ar cr libsl.a sorted-list.o
	rm sorted-list.o
	gcc -std=c99 -g -o sl main.c libsl.a

clean:
	rm sl
	rm libsl.a
