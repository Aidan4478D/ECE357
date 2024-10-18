# Definitions for constants
CC=gcc
CFLAGS=-I.

.PHONY: clean

all: make clean

make: shell.o
	$(CC) $(CFLAGS) -o shell shell.o

shell.o: shell.c
	$(CC) $(CFLAGS) -c shell.c

clean:
	rm -f *.o *.stackdump
