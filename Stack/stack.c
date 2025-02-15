#include "stack.h"

stack_t *
stack_init (size_t capacity, bool (*bp_compare_function)(void *, void *))
{
    stack_t * p_stack = NULL;

    if (NULL == bp_compare_function)
    {
        goto EXIT;
    }

    p_stack = calloc(1, sizeof(stack_t));

    if (NULL != p_stack)
    {
        p_stack->pp_elements = calloc(capacity, sizeof(void *));

        if (NULL != p_stack->pp_elements)
        {
            p_stack->size                = 0;
            p_stack->capacity            = capacity;
            p_stack->bp_compare_function = bp_compare_function;
        }
        else
        {
            free(p_stack);
            p_stack = NULL;
        }
    }

EXIT:
    return p_stack;
}

void
stack_destroy (stack_t * p_stack)
{
    if (NULL != p_stack)
    {

        if (NULL != p_stack->pp_elements)
        {

            for (size_t index = 0; index < p_stack->size; index++)
            {
                p_stack->pp_elements[index] = NULL;
            }

            free(p_stack->pp_elements);
            p_stack->pp_elements = NULL;
        }

        free(p_stack);
        p_stack = NULL;
    }
}

int
stack_resize (stack_t * p_stack, size_t new_capacity)
{
    int     status  = 0;
    void ** pp_temp = realloc(p_stack->pp_elements, new_capacity);

    if (NULL != pp_temp)
    {
        p_stack->capacity    = new_capacity;
        p_stack->pp_elements = pp_temp;
    }
    else
    {
        printf("Error reallocating memory for stack elements.\n");
        status = -1;
    }

    return status;
}

void
stack_push (stack_t * p_stack, void * p_value)
{
    if ((NULL == p_stack) || (NULL == p_value))
    {
        goto EXIT;
    }

    if (p_stack->size >= p_stack->capacity)
    {
        size_t new_capacity = 2 * p_stack->capacity;
        int    status       = stack_resize(p_stack, new_capacity);

        if (-1 == status)
        {
            printf("Error resizing stack.\n");
            goto EXIT;
        }
    }

    p_stack->pp_elements[p_stack->size] = p_value;
    p_stack->size++;

EXIT:
    return;
}

void *
stack_pop (stack_t * p_stack)
{
    void * p_elem = NULL;

    if (NULL != p_stack)
    {
        if (0 >= p_stack->size)
        {
            printf("Stack is empty. Cannot pop.\n");
        }
        else
        {
            p_elem = p_stack->pp_elements[p_stack->size];
            p_stack->pp_elements[p_stack->size] = NULL;
            p_stack->size--;
        }
    }

    return p_elem;
}

void *
stack_peek (stack_t * p_stack, void * p_value)
{
    void * p_elem = NULL;

    if ((NULL != p_stack) && (NULL != p_value))
    {
        if (0 >= p_stack->size)
        {
            printf("Stack is empty. Cannot peek.\n");
        }
        else
        {
            p_elem = p_stack->pp_elements[p_stack->size];
        }
    }

    return p_elem;
}

void
stack_print (stack_t * p_stack)
{
    if (NULL != p_stack)
    {

        printf("Stack Size: %lu\nElements in the stack:\n", p_stack->size);

        for (size_t index = 0; index < p_stack->size; index++)
        {
            if (NULL != p_stack->pp_elements[index])
            {
                char * p_value = (char *)p_stack->pp_elements[index];
                printf("Index: %lu, Value: %s\n", index, p_value);
            }
        }
    }
}

bool
compare (void * p_first, void * p_second)
{
    char * p_first_str  = (char *)p_first;
    char * p_second_str = (char *)p_second;
    return 0 == strcmp(p_first_str, p_second_str);
}

int
main ()
{
    // Initialize a stack with string comparison function
    stack_t * stack = stack_init(INITIAL_CAPACITY, compare);

    // Push some elements onto the stack
    stack_push(stack, "Apple");
    stack_push(stack, "Banana");
    stack_push(stack, "Orange");

    printf("Stack after pushing elements:\n");
    stack_print(stack);

    char * popped = (char *)stack_pop(stack);
    if (popped != NULL)
    {
        printf("\nPopped element: %s\n", popped);
    }

    printf("\nPeeked element: %s\n", (char *)stack_peek(stack, "Orange"));

    printf("\nStack after popping elements:\n");
    stack_print(stack);

    stack_destroy(stack);

    return 0;
}
