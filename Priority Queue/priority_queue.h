#include <stdio.h>
#include <stdlib.h>

#define INITIAL_CAPACITY 10

typedef struct element_t
{
    int    priority;
    void * p_value;
} element_t;

typedef struct priority_queue_t
{
    element_t ** pp_elements;
    size_t       size;
    size_t       capacity;
} priority_queue_t;

priority_queue_t * priority_queue_init (size_t capacity);
void               priority_queue_destroy (priority_queue_t * p_queue);
element_t *        priority_queue_create_elem (void * p_value, int priority);
int                priority_queue_resize (priority_queue_t * p_queue, size_t new_capacity);
void               priority_queue_swap (element_t * p_first, element_t * p_second);
void               priority_queue_enqueue (priority_queue_t * p_queue, void * p_value, int priority);
element_t *        priority_queue_dequeue (priority_queue_t * p_queue);
void               priority_queue_print (priority_queue_t * p_queue);
