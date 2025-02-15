#include "priority_queue.h"

priority_queue_t *
priority_queue_init (size_t capacity)
{
    priority_queue_t * p_queue = calloc(1, sizeof(priority_queue_t));

    if (NULL != p_queue)
    {
        p_queue->pp_elements = calloc(capacity, sizeof(element_t *));

        if (NULL != p_queue->pp_elements)
        {
            p_queue->size     = 0;
            p_queue->capacity = capacity;
        }
        else
        {
            free(p_queue);
            p_queue = NULL;
        }
    }

    return p_queue;
}

void
priority_queue_destroy (priority_queue_t * p_queue)
{
    if (NULL != p_queue)
    {
        if (NULL != p_queue->pp_elements)
        {
            for (size_t index = 0; index < p_queue->size; index++)
            {
                free(p_queue->pp_elements[index]);
                p_queue->pp_elements[index] = NULL;
            }

            free(p_queue->pp_elements);
            p_queue->pp_elements = NULL;
        }

        free(p_queue);
        p_queue = NULL;
    }
}

element_t *
priority_queue_create_elem (void * p_value, int priority)
{
    element_t * p_element = calloc(1, sizeof(element_t));

    if (NULL != p_element)
    {
        p_element->priority = priority;
        p_element->p_value  = p_value;
    }

    return p_element;
}

int
priority_queue_resize (priority_queue_t * p_queue, size_t new_capacity)
{
    int          status  = 0;
    element_t ** pp_temp = realloc(p_queue->pp_elements, new_capacity);

    if (NULL != pp_temp)
    {
        p_queue->capacity    = new_capacity;
        p_queue->pp_elements = pp_temp;
    }
    else
    {
        printf("Error reallocating memory for queue elements.\n");
        status = -1;
    }

    return status;
}

void
priority_queue_swap (element_t * p_first, element_t * p_second)
{
    element_t temp = *p_first;
    *p_first       = *p_second;
    *p_second      = temp;
}

void
priority_queue_enqueue (priority_queue_t * p_queue,
                        void *             p_value,
                        int                priority)
{
    if ((NULL == p_queue) || (NULL == p_value))
    {
        goto EXIT;
    }

    if (p_queue->size >= p_queue->capacity)
    {
        size_t new_capacity = 2 * p_queue->capacity;
        int    status       = priority_queue_resize(p_queue, new_capacity);

        if (-1 == status)
        {
            goto EXIT;
        }
    }

    element_t * p_element = priority_queue_create_elem(p_value, priority);

    if (NULL != p_element)
    {
        p_queue->pp_elements[p_queue->size] = p_element;
        size_t index                        = p_queue->size;
        p_queue->size++;

        while (index > 0)
        {

            if (p_queue->pp_elements[index]->priority
                > p_queue->pp_elements[index - 1]->priority)
            {
                priority_queue_swap(p_queue->pp_elements[index],
                                    p_queue->pp_elements[index - 1]);
            }

            index--;
        }
    }

EXIT:
    return;
}

element_t *
priority_queue_dequeue (priority_queue_t * p_queue)
{
    element_t * p_element = NULL;

    if (p_queue->size > 0)
    {
        p_element = p_queue->pp_elements[0];
        p_queue->size--;
    }

    return p_element;
}

void
priority_queue_print (priority_queue_t * p_queue)
{
    printf("Elements in the priority queue:\n");

    if (NULL != p_queue)
    {
        for (size_t index = 0; index < p_queue->size; index++)
        {
            if (NULL != p_queue->pp_elements[index])
            {
                char * p_value = (char *)p_queue->pp_elements[index]->p_value;
                printf("Priority: %u, Value: %s\n",
                       p_queue->pp_elements[index]->priority,
                       p_value);
            }
        }
    }
}

int
main ()
{
    priority_queue_t * pq = priority_queue_init(INITIAL_CAPACITY);

    // Enqueue elements into the priority queue
    priority_queue_enqueue(pq, "apple", 5);
    priority_queue_enqueue(pq, "banana", 3);
    priority_queue_enqueue(pq, "orange", 7);
    priority_queue_enqueue(pq, "grape", 2);
    priority_queue_enqueue(pq, "mango", 9);

    priority_queue_print(pq);

    // Dequeue elements from the priority queue
    printf("\nDequeuing elements from the priority queue:\n");
    while (pq->size > 0)
    {
        element_t * max = priority_queue_dequeue(pq);
        printf("Dequeued element: Priority = %d, Value = %s\n",
               max->priority,
               (char *)max->p_value);
        free(max);
    }

    priority_queue_destroy(pq);

    return 0;
}
