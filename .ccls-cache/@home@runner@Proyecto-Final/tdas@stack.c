#include <stdio.h>
#include <stdlib.h>
#include "stack.h"  // Include the stack.h header
#include "list.h"   // Include the list.h header

// Function implementations for the Stack data structure

// Create a new stack
Stack *stack_create() {
    return list_create();
}

// Push an element onto the stack
void stack_push(Stack *stack, void *data) { 
    list_pushFront(stack, data);
}

// Get the top element of the stack
void *stack_top(Stack *stack) {
    return list_first(stack);
}

// Remove and return the top element of the stack
void *stack_pop(Stack *stack) {
    return list_popFront(stack);
}

// Clear the stack
void stack_clean(Stack *stack) { 
    list_clean(stack); 
}