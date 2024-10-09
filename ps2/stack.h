#ifndef _STACK_H
#define _STACK_H

#include <stdio.h>
#include <stdbool.h>

// can hold a max of 4096 directories in the DFS search
#define MAX_STACK_SIZE 4096

// heavy inspiration taken from
// https://www.geeksforgeeks.org/implement-stack-in-c/#
// to implement this stack. 
// Don't worry, we understand everything going on here! 

typedef struct {

    char* arr[MAX_STACK_SIZE];
    int top;
} Stack;

void initialize(Stack *stack) {
    stack->top = -1;  // empty stack
}

bool is_empty(Stack *stack) {
    return stack->top == -1;
}

bool is_full(Stack *stack) {
    return stack->top == MAX_STACK_SIZE - 1;
}

void push(Stack *stack, char* value) {

    if(is_full(stack)) {
        printf("Stack overflow\n");
        return;
    }

    stack->arr[++stack->top] = value;
    //printf("Pushed %s onto the stack\n", value);
}

char* pop(Stack *stack) {

    if(is_empty(stack)) {
        printf("Stack is empty!\n");
        return NULL; 
    }

    char* popped = stack->arr[stack->top];
    stack->top--;

    //printf("Popped %s from the stack\n", popped);
    return popped;
}

char* peek(Stack *stack) {

    if(is_empty(stack)) {
        
        printf("stack empty\n");
        return NULL;
    }

    return stack->arr[stack->top];
}

#endif
