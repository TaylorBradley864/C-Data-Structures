#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define INITIAL_CAPACITY 5

typedef struct stack_t
{
    size_t  size;
    size_t  capacity;
    void ** pp_elements;
    bool (*bp_compare_function)(void *, void *);
} stack_t;

stack_t * stack_init (size_t capacity, bool (*bp_compare_function)(void *, void *));
void      stack_destroy (stack_t * p_stack);
int       stack_resize (stack_t * p_stack, size_t new_capacity);
void      stack_push (stack_t * p_stack, void * p_value);
void *    stack_pop (stack_t * p_stack);
void *    stack_peek (stack_t * p_stack, void * p_value);
void      stack_print (stack_t * p_stack);
bool      compare (void * p_first, void * p_second);
