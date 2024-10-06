# Definitions for constants
CC=gcc
CFLAGS=-I.

.PHONY: clean

all: make clean

make: q3.o
	$(CC) $(CFLAGS) -o q3 q3.o

q3.o: q3.c inode_counts.h
	$(CC) $(CFLAGS) -c q3.c

clean:
	rm -f *.o *.stackdump
