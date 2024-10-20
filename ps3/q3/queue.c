#include "headers/queue.h"

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
    /*printf("Pushed %s onto the queue\n", value);*/
}

char* dequeue(Queue* queue) {

    if(is_empty(queue)) {
        printf("Queue is empty!\n");
        return NULL;
    }

    char* dequeued = queue->arr[queue->front];
    queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
    queue->size--;
    
    /*printf("Popped %s from the queue\n", dequeued);*/
    return dequeued;
}

char* peek(Queue* queue) {

    if(is_empty(queue)) {
        
        printf("queue empty\n");
        return NULL;
    }

    return queue->arr[queue->front];
}

int get_size(Queue* queue) {

    return queue->size; 
}

