# Definitions for constants
CC=gcc
CFLAGS=-I.

.PHONY: clean

all: make clean

make: shell.o commands.o queue.o
	$(CC) $(CFLAGS) -o shell shell.o commands.o queue.o

shell.o: shell.c
	$(CC) $(CFLAGS) -c shell.c

commands.o: commands.c
	$(CC) $(CFLAGS) -c commands.c

queue.o: queue.c
	$(CC) $(CFLAGS) -c queue.c


clean:
	rm -f *.o *.stackdump
