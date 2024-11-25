# Definitions for constants
CC=gcc
CFLAGS=-I.

.PHONY: clean

all: make clean

make: main.o tests.o 
	$(CC) $(CFLAGS) -o test main.o tests.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

tests.o: tests.c
	$(CC) $(CFLAGS) -c tests.c

clean:
	rm -f *.o *.stackdump
