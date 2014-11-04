#This Makefile is used to compile our sorted list implementation into a library called libsl.a and an executable called sl
#where sl runs main.c

#define compiler
CC = gcc

#define flags
CFLAGS = -Wall
DEBUGFLAG = -g

#ultimate executable
MAIN = sl

#library files
LIBS_FILES = sorted-list.o

#libraries libsl.a
LIBS = libsl.a

#C sourcefiles
SRCS = sorted-list.c test_main.c

#object files
OBJS = $(SRCS:.c=.o)

#header files
HEAD = sorted-list.h
#includes
INCLUDES = -I.

#just running 'make' will invoke the first target entry in the file, 
#so we have this as the default target entry
all: $(MAIN)

debug: $(LIBS) test_main.c
	$(CC) $(CFLAGS) $(DEBUGFLAG) test_main.c $(LIBS) -o $(MAIN)

#create the executal 'sl'
$(MAIN) : $(LIBS) test_main.c
	$(CC) $(CFLAGS) test_main.c $(LIBS) -o $(MAIN) 

#Create the library
$(LIBS): $(LIBS_FILES)
	ar cr $(LIBS) $(LIBS_FILES)
	ranlib $(LIBS)

#create object file
sorted-list.o: sorted-list.c sorted-list.h
	$(CC) $(CFLAGS) $(DEBUGFLAG) -c sorted-list.c

#clean entry
clean:
	rm -f *.o
	rm -f *.a
	rm -f $(MAIN)
