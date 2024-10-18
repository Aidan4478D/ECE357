#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// can hold a max of 4096 directories in the DFS search
#define MAX_QUEUE_SIZE 4096

typedef struct {
    int front, rear, size;
    char* arr[MAX_QUEUE_SIZE];
} Queue;

Queue* create_queue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
    return queue;
}

bool is_empty(Queue* queue) {
    return queue->size == 0;
}

bool is_full(Queue* queue) {
    return queue->size == MAX_QUEUE_SIZE;
}

void enqueue(Queue* queue, char* value) {

    if(is_full(queue)) {
        printf("Queue overflow\n");
        return;
    }

    queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
    queue->arr[queue->rear] = value;
    queue->size++;
    printf("Pushed %s onto the queue\n", value);
}

char* dequeue(Queue* queue) {

    if(is_empty(queue)) {
        printf("Queue is empty!\n");
        return NULL;
    }

    char* dequeued = queue->arr[queue->front];
    queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
    queue->size--;
    

    printf("Popped %s from the queue\n", dequeued);
    return dequeued;
}

char* peek(Queue* queue) {

    if(is_empty(queue)) {
        
        printf("queue empty\n");
        return NULL;
    }

    return queue->arr[queue->front];
}

#endif
