#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_QUEUE_SIZE 4096

// I know using a queue is unnecessary but I made it and don't want it to 
// go to waste </3

typedef struct {
    int front, rear, size;
    char* arr[MAX_QUEUE_SIZE];
} Queue;

Queue* create_queue();
bool is_empty(Queue* queue);
bool is_full(Queue* queue);
void enqueue(Queue* queue, char* value); 
char* dequeue(Queue* queue);
char* peek(Queue* queue); 
int get_size(Queue* queue);

#endif
