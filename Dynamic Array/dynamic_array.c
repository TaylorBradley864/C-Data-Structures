#include "dynamic_array.h"

dynamic_array_t *
dynamic_array_init (size_t capacity,
                    bool (*bp_compare)(void *, void *),
                    void (*p_destroy)(void *))
{
    dynamic_array_t * p_array = NULL;

    if ((NULL == bp_compare) || (NULL == p_destroy))
    {
        fprintf(stderr, "Provided NULL function pointers to array.\n");
        goto EXIT;
    }

    p_array = calloc(1, sizeof(dynamic_array_t));

    if (NULL == p_array)
    {
        fprintf(stderr, GP_MEMORY_MESSAGE, __LINE__, __func__);
        goto EXIT;
    }

    if (0 != pthread_rwlock_init(&p_array->array_lock, NULL))
    {
        fprintf(stderr, "Dynamic array lock initialization failed.\n");
        free(p_array);
        p_array = NULL;
        goto EXIT;
    }

    p_array->pp_data = calloc(capacity, sizeof(void *));

    if (NULL == p_array->pp_data)
    {
        fprintf(stderr, GP_MEMORY_MESSAGE, __LINE__, __func__);
        free(p_array);
        p_array = NULL;
        goto EXIT;
    }

    p_array->size               = 0;
    p_array->capacity           = capacity;
    p_array->bp_compare         = bp_compare;
    p_array->p_destroy_function = p_destroy;

EXIT:
    return p_array;
}

void
dynamic_array_destroy (dynamic_array_t * p_array)
{
    fprintf(stdout, "Cleaning up array...\n");

    if (NULL == p_array)
    {
        goto EXIT;
    }

    pthread_rwlock_wrlock(&p_array->array_lock);

    if (NULL != p_array->pp_data)
    {

        for (size_t index = 0; index < p_array->size; index++)
        {
            if (NULL != p_array->pp_data[index])
            {
                p_array->p_destroy_function(p_array->pp_data[index]);
            }
        }

        free(p_array->pp_data);
        p_array->pp_data = NULL;
    }

    pthread_rwlock_unlock(&p_array->array_lock);
    pthread_rwlock_destroy(&p_array->array_lock);

    free(p_array);
    p_array = NULL;

EXIT:
    return;
}

int
dynamic_array_resize (dynamic_array_t * p_array, size_t new_capacity)
{
    int status = SUCCESS;

    if ((NULL == p_array) || (NULL == p_array->pp_data))
    {
        fprintf(stderr, GP_POINTER_MESSAGE, __LINE__, __func__);
        status = FAILURE;
        goto EXIT;
    }

    void ** pp_temp = realloc(
        p_array->pp_data, new_capacity * p_array->capacity * sizeof(void *));

    if (NULL == pp_temp)
    {
        fprintf(stderr, "Memory reallocation error\n");
        status = FAILURE;
        goto EXIT;
    }

    p_array->pp_data  = pp_temp;
    p_array->capacity = new_capacity;

EXIT:
    return status;
}

int
dynamic_array_insert (dynamic_array_t * p_array, void * p_element)
{
    int status = SUCCESS;

    if ((NULL == p_array) || (NULL == p_array->pp_data))
    {
        fprintf(stderr, GP_POINTER_MESSAGE, __LINE__, __func__);
        status = FAILURE;
        goto EXIT;
    }

    if (0 != pthread_rwlock_wrlock(&p_array->array_lock))
    {
        fprintf(stderr, GP_POINTER_MESSAGE, __LINE__, __func__);
        status = FAILURE;
        goto EXIT;
    }

    if (p_array->size >= p_array->capacity)
    {
        size_t new_capacity = DOUBLE * p_array->capacity;
        status              = dynamic_array_resize(p_array, new_capacity);

        if (SUCCESS != status)
        {
            goto EXIT_UNLOCK;
        }
    }

    p_array->pp_data[p_array->size] = p_element;
    p_array->size++;

EXIT_UNLOCK:
    pthread_rwlock_unlock(&p_array->array_lock);

EXIT:
    return status;
}

int
dynamic_array_remove (dynamic_array_t * p_array, size_t index)
{
    int status = SUCCESS;

    if ((NULL == p_array) || (NULL == p_array->pp_data))
    {
        fprintf(stderr, GP_POINTER_MESSAGE, __LINE__, __func__);
        status = FAILURE;
        goto EXIT;
    }

    if (0 != pthread_rwlock_wrlock(&p_array->array_lock))
    {
        fprintf(stderr, GP_POINTER_MESSAGE, __LINE__, __func__);
        status = FAILURE;
        goto EXIT;
    }

    if (p_array->size <= index)
    {
        fprintf(stderr, "Array index out of bounds.\n");
        status = FAILURE;
        goto EXIT_UNLOCK;
    }

    p_array->p_destroy_function(p_array->pp_data[index]);

    for (size_t count = index; count < p_array->size - 1; count++)
    {
        p_array->pp_data[count] = p_array->pp_data[count + 1];
    }

    p_array->size--;

    if (p_array->size < (p_array->size / ARRAY_SIZE_DIVISOR))
    {
        size_t new_capacity = p_array->size / ARRAY_SIZE_DIVISOR;

        status = dynamic_array_resize(p_array, new_capacity);

        if (SUCCESS != status)
        {
            goto EXIT_UNLOCK;
        }

        p_array->capacity = new_capacity;
    }

EXIT_UNLOCK:
    pthread_rwlock_unlock(&p_array->array_lock);

EXIT:
    return status;
}

void *
dynamic_array_get_element (dynamic_array_t * p_array, size_t index)
{
    void * p_item = NULL;

    if ((NULL == p_array) || (NULL == p_array->pp_data))
    {
        fprintf(stderr, GP_POINTER_MESSAGE, __LINE__, __func__);
        goto EXIT;
    }

    if (0 == pthread_rwlock_rdlock(&p_array->array_lock))
    {

        if (p_array->size <= index)
        {
            fprintf(stderr, "Array index out of bounds.\n");
        }
        else
        {
            p_item = p_array->pp_data[index];
        }
    }

    pthread_rwlock_unlock(&p_array->array_lock);

EXIT:
    return p_item;
}

int
dynamic_array_search (dynamic_array_t * p_array, void * p_item)
{
    int found_index = -1;

    if ((NULL == p_array) || (NULL == p_array->pp_data)
        || (NULL == p_array->bp_compare))
    {
        fprintf(stderr, GP_POINTER_MESSAGE, __LINE__, __func__);
        goto EXIT;
    }

    if (0 != pthread_rwlock_rdlock(&p_array->array_lock))
    {
        fprintf(stderr, GP_POINTER_MESSAGE, __LINE__, __func__);
        goto EXIT;
    }

    for (size_t index = 0; index < p_array->size; index++)
    {
        if (p_array->bp_compare(p_array->pp_data[index], p_item))
        {
            found_index = index;
            break;
        }
    }

    pthread_rwlock_unlock(&p_array->array_lock);

EXIT:
    return found_index;
}

// End of dynamic_array.c
