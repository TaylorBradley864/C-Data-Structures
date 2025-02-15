#include "hashtable.h"

hashtable_t *
hashtable_create (size_t capacity,
                  uint32_t (*p_hash_function)(void *),
                  bool (*bp_compare_function)(void *, void *),
                  void (*p_destroy_function)(void *))
{
    hashtable_t * p_hashtable = NULL;

    if ((NULL == bp_compare_function) || (NULL == p_hash_function)
        || (NULL == p_destroy_function))
    {
        fprintf(stderr, "Provided NULL function pointers to hashtable.\n");
        goto EXIT;
    }

    p_hashtable = calloc(1, sizeof(hashtable_t));

    if (NULL == p_hashtable)
    {
        fprintf(stderr, GP_MEMORY_MESSAGE, __LINE__, __func__);
        goto EXIT;
    }

    if (0 != pthread_rwlock_init(&p_hashtable->hashtable_lock, NULL))
    {
        fprintf(stderr, "Hashtable lock initialization failed.\n");
        free(p_hashtable);
        p_hashtable = NULL;
        goto EXIT;
    }

    p_hashtable->size                = 0;
    p_hashtable->capacity            = capacity;
    p_hashtable->p_hash_function     = p_hash_function;
    p_hashtable->bp_compare_function = bp_compare_function;
    p_hashtable->p_destroy_function  = p_destroy_function;
    p_hashtable->pp_items            = calloc(capacity, sizeof(node_t *));

    if (NULL == p_hashtable->pp_items)
    {
        free(p_hashtable);
        p_hashtable = NULL;
        goto EXIT;
    }

EXIT:
    return p_hashtable;
}

uint32_t
hashtable_hash (hashtable_t * p_hashtable, void * p_data)
{
    uint32_t hash_index = 0;

    if ((NULL != p_hashtable) && (NULL != p_data))
    {
        hash_index = p_hashtable->p_hash_function(p_data);
        hash_index %= p_hashtable->capacity;
    }

    return hash_index;
}

bool
hashtable_index_occupied (hashtable_t * p_hashtable, uint32_t index)
{
    bool b_return = false;

    if ((NULL != p_hashtable) && (p_hashtable->capacity > index))
    {
        b_return = (NULL != p_hashtable->pp_items[index]);
    }

    return b_return;
}

bool
hashtable_above_loadfactor (hashtable_t * p_hashtable)
{
    bool b_return = false;

    if (NULL != p_hashtable)
    {
        size_t acceptable_load
            = ((LOAD_FACTOR_NUMERATOR * p_hashtable->capacity)
               / LOAD_FACTOR_DENOMINATOR);
        b_return = (p_hashtable->size) >= (acceptable_load);
    }

    return b_return;
}

int
hashtable_rehash (hashtable_t * p_hashtable)
{
    int status = SUCCESS;

    if (NULL == p_hashtable)
    {
        status = FAILURE;
        goto EXIT;
    }

    node_t ** pp_new_items = calloc(p_hashtable->capacity, sizeof(node_t *));

    if (NULL == pp_new_items)
    {
        fprintf(stderr, GP_MEMORY_MESSAGE, __LINE__, __func__);
        status = FAILURE;
        goto EXIT;
    }

    for (size_t index = 0; index < p_hashtable->capacity; index++)
    {
        node_t * p_current = p_hashtable->pp_items[index];
        node_t * p_temp    = p_current;

        while (NULL != p_temp)
        {
            node_t * p_new_item = node_create(p_temp->p_data);

            if (NULL == p_new_item)
            {
                fprintf(stderr,
                        "Node creation failed. Cannot rehash hashtable.\n");
                free(pp_new_items);
                pp_new_items = NULL;
                status       = FAILURE;
                goto EXIT;
            }

            uint32_t new_hash = hashtable_hash(p_hashtable, p_new_item->p_data);

            if (NULL == pp_new_items[new_hash])
            {
                pp_new_items[new_hash] = p_new_item;
            }
            else
            {
                node_t * p_new_head = node_insert_front(pp_new_items[new_hash],
                                                        p_new_item->p_data);
                pp_new_items[new_hash] = p_new_head;
                node_free(p_new_item);
            }

            p_temp = p_temp->p_next;
        }

        node_free(p_current);
    }

    free(p_hashtable->pp_items);
    p_hashtable->pp_items = pp_new_items;

EXIT:
    return status;
}

int
hashtable_realloc (hashtable_t * p_hashtable, size_t new_capacity)
{
    int status = SUCCESS;

    if (NULL == p_hashtable)
    {
        fprintf(stderr, GP_POINTER_MESSAGE, __LINE__, __func__);
        status = FAILURE;
        goto EXIT;
    }

    size_t    old_capacity = p_hashtable->capacity;
    node_t ** pp_temp
        = realloc(p_hashtable->pp_items, new_capacity * sizeof(node_t *));

    if (NULL == pp_temp)
    {
        fprintf(stderr, "Failed to reallocate hashtable.\n");
        status = FAILURE;
        goto EXIT;
    }

    p_hashtable->pp_items = pp_temp;
    p_hashtable->capacity = new_capacity;

    for (size_t index = old_capacity; index < new_capacity; index++)
    {
        p_hashtable->pp_items[index] = NULL;
    }

EXIT:
    return status;
}

uint8_t
hashtable_add_item (hashtable_t * p_hashtable, void * p_item)
{
    int status = SUCCESSFUL_OP;

    if ((NULL == p_hashtable) || (NULL == p_item))
    {
        fprintf(stderr, GP_POINTER_MESSAGE, __LINE__, __func__);
        status = UNKNOWN_FAILURE;
        goto EXIT;
    }

    if (0 != pthread_rwlock_wrlock(&p_hashtable->hashtable_lock))
    {
        fprintf(stderr, GP_MUTEX_MESSAGE, __LINE__, __func__);
        status = UNKNOWN_FAILURE;
        goto EXIT;
    }

    if (hashtable_above_loadfactor(p_hashtable))
    {
        size_t new_capacity   = DOUBLE * p_hashtable->capacity;
        int    realloc_status = hashtable_realloc(p_hashtable, new_capacity);

        if (SUCCESS != realloc_status)
        {
            status = UNKNOWN_FAILURE;
            goto EXIT_UNLOCK;
        }

        int rehash_status = hashtable_rehash(p_hashtable);

        if (SUCCESS != rehash_status)
        {
            status = UNKNOWN_FAILURE;
            goto EXIT_UNLOCK;
        }
    }

    uint32_t new_hash = hashtable_hash(p_hashtable, p_item);

    if (!hashtable_index_occupied(p_hashtable, new_hash))
    {
        node_t * p_new_item = node_create(p_item);

        if (NULL == p_new_item)
        {
            status = UNKNOWN_FAILURE;
            goto EXIT_UNLOCK;
        }

        p_hashtable->pp_items[new_hash] = p_new_item;
    }
    else
    {
        node_t * p_new_head
            = node_insert_front(p_hashtable->pp_items[new_hash], p_item);
        p_hashtable->pp_items[new_hash] = p_new_head;
    }

    p_hashtable->size++;

EXIT_UNLOCK:
    pthread_rwlock_unlock(&p_hashtable->hashtable_lock);

EXIT:
    return status;
}

uint8_t
hashtable_remove_item (hashtable_t * p_hashtable,
                       node_t *      p_target,
                       uint32_t      hash_value)
{
    int status = SUCCESSFUL_OP;

    if (NULL == p_hashtable)
    {
        fprintf(stderr, GP_POINTER_MESSAGE, __LINE__, __func__);
        status = UNKNOWN_FAILURE;
        goto EXIT;
    }

    if (0 != pthread_rwlock_wrlock(&p_hashtable->hashtable_lock))
    {
        fprintf(stderr, GP_MUTEX_MESSAGE, __LINE__, __func__);
        status = UNKNOWN_FAILURE;
        goto EXIT;
    }

    node_t * p_head                   = p_hashtable->pp_items[hash_value];
    node_t * p_temp                   = node_delete(p_head, p_target);
    p_hashtable->pp_items[hash_value] = p_temp;
    p_hashtable->size--;

    pthread_rwlock_unlock(&p_hashtable->hashtable_lock);

EXIT:
    return status;
}

void *
hashtable_search (hashtable_t * p_hashtable, uint32_t index, void * p_compare)
{
    void * p_return = NULL;

    if ((NULL == p_hashtable) || (NULL == p_compare))
    {
        fprintf(stderr, GP_POINTER_MESSAGE, __LINE__, __func__);
        goto EXIT;
    }

    if (0 != pthread_rwlock_rdlock(&p_hashtable->hashtable_lock))
    {
        fprintf(stderr, GP_MUTEX_MESSAGE, __LINE__, __func__);
        goto EXIT;
    }

    if (index >= p_hashtable->capacity)
    {
        fprintf(stderr, "Hashtable index out of bounds.\n");
        goto EXIT_UNLOCK;
    }

    node_t * p_item = p_hashtable->pp_items[index];

    while (NULL != p_item)
    {
        if (p_hashtable->bp_compare_function(p_item->p_data, p_compare))
        {
            p_return = p_item->p_data;
            break;
        }

        p_item = p_item->p_next;
    }

EXIT_UNLOCK:
    pthread_rwlock_unlock(&p_hashtable->hashtable_lock);

EXIT:
    return p_return;
}

node_t *
hashtable_get_node (hashtable_t * p_hashtable, uint32_t index, void * p_compare)
{
    node_t * p_return = NULL;

    if ((NULL == p_hashtable) || (NULL == p_compare))
    {
        fprintf(stderr, GP_POINTER_MESSAGE, __LINE__, __func__);
        goto EXIT;
    }

    if (0 != pthread_rwlock_rdlock(&p_hashtable->hashtable_lock))
    {
        fprintf(stderr, GP_MUTEX_MESSAGE, __LINE__, __func__);
        goto EXIT;
    }

    if (index >= p_hashtable->capacity)
    {
        fprintf(stderr, "Hashtable index out of bounds.\n");
        goto EXIT_UNLOCK;
    }

    node_t * p_item = p_hashtable->pp_items[index];

    while (NULL != p_item)
    {
        if (p_hashtable->bp_compare_function(p_item->p_data, p_compare))
        {
            p_return = p_item;
            break;
        }

        p_item = p_item->p_next;
    }

EXIT_UNLOCK:
    pthread_rwlock_unlock(&p_hashtable->hashtable_lock);

EXIT:
    return p_return;
}

void
hashtable_destroy (hashtable_t * p_hashtable)
{
    fprintf(stdout, "Freeing hashtable...\n");

    if (NULL != p_hashtable)
    {

        if (0 != pthread_rwlock_wrlock(&p_hashtable->hashtable_lock))
        {
            fprintf(stderr, GP_MUTEX_MESSAGE, __LINE__, __func__);
            goto EXIT;
        }

        for (size_t index = 0; index < p_hashtable->capacity; index++)
        {
            node_t * p_current = p_hashtable->pp_items[index];
            node_t * p_temp    = p_current;

            while (NULL != p_temp)
            {
                p_hashtable->p_destroy_function(p_temp->p_data);
                p_temp = p_temp->p_next;
            }

            node_free(p_current);
        }

        free(p_hashtable->pp_items);
        p_hashtable->pp_items = NULL;

        pthread_rwlock_unlock(&p_hashtable->hashtable_lock);
        pthread_rwlock_destroy(&p_hashtable->hashtable_lock);

        free(p_hashtable);
        p_hashtable = NULL;
    }

EXIT:
    return;
}

// End of hashtable.c
