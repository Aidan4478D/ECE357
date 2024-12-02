# Definitions for constants
CC=gcc
CFLAGS=-I.

.PHONY: clean

all: make clean

make: main.o tests.o spinlock.o tas64.o
	$(CC) $(CFLAGS) -o test main.o tests.o spinlock.o tas64.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

tests.o: tests.c
	$(CC) $(CFLAGS) -c tests.c

spinlock.o: spinlock.c
	$(CC) $(CFLAGS) -c spinlock.c 

tas64.o: tas64.S
	$(CC) $(CFLAGS) -c tas64.S 


clean:
	rm -f *.o *.stackdump
