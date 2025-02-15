#include "../include/node.h"

node_t *
node_create (void * p_node_data)
{
    node_t * p_node = NULL;

    if (NULL == p_node_data)
    {
        printf("Pointer to data cannot be NULL. Cannot create node.\n");
        goto EXIT;
    }

    p_node = malloc(sizeof(node_t));

    if (NULL == p_node)
    {
        printf("Error allocating memory for linked list node creation.\n");
        goto EXIT;
    }

    p_node->p_data = p_node_data;
    p_node->p_next = NULL;

EXIT:
    return p_node;
}

node_t *
node_insert_front (node_t * p_head, void * p_data)
{
    if (NULL == p_data)
    {
        printf("Pointer to data cannot be NULL. Cannot insert node.\n");
        goto EXIT;
    }

    node_t * p_new_node = node_create(p_data);

    if (NULL == p_new_node)
    {
        printf("Node creation failed, insertion not completed.\n");
        goto EXIT;
    }

    if (NULL == p_head)
    {
        p_head = p_new_node;
    }
    else
    {
        p_new_node->p_next = p_head;
        p_head             = p_new_node;
    }

EXIT:
    return p_head;
}

node_t *
node_insert_back (node_t * p_head, void * p_data)
{
    if (NULL == p_data)
    {
        printf("Pointer to data cannot be NULL. Cannot insert node.\n");
        goto EXIT;
    }

    node_t * p_new_node = node_create(p_data);

    if (NULL == p_head)
    {
        p_head = p_new_node;
    }
    else
    {
        node_t * p_temp_node = p_head;

        while (NULL != p_temp_node->p_next)
        {
            p_temp_node = p_temp_node->p_next;
        }

        p_temp_node->p_next = p_new_node;
    }

EXIT:
    return p_head;
}

node_t *
node_insert_at (node_t * p_prev_node, void * p_data)
{
    if (NULL == p_data)
    {
        printf("Pointer to data cannot be NULL. Cannot insert node.\n");
        goto EXIT;
    }

    node_t * p_new_node = node_create(p_data);

    if (NULL != p_prev_node)
    {
        p_new_node->p_next  = p_prev_node->p_next;
        p_prev_node->p_next = p_new_node;
    }
    else
    {
        p_new_node->p_next = p_prev_node;
        p_prev_node        = p_new_node;
    }

EXIT:
    return p_prev_node;
}

void
node_print (node_t * p_node)
{
    printf("Linked List: ");

    while (NULL != p_node)
    {
        printf("%p -> ", p_node->p_data);
        p_node = p_node->p_next;
    }
    printf("\n");
}

int
node_size (node_t * p_node)
{
    int size = 0;

    while (NULL != p_node)
    {
        size++;
        p_node = p_node->p_next;
    }

    return size;
}

node_t *
node_search (node_t * p_node, void * p_value)
{
    node_t * p_return = NULL;

    if (NULL == p_value)
    {
        printf(
            "Pointer to search value cannot be NULL. Cannot search linked "
            "list.\n");
        goto EXIT;
    }

    while (NULL != p_node)
    {
        if (p_value == p_node->p_data)
        {
            p_return = p_node;
            break;
        }
        p_node = p_node->p_next;
    }

EXIT:
    return p_return;
}

node_t *
node_delete (node_t * p_head, node_t * p_del_node)
{
    node_t * p_temp_node = p_head;
    node_t * p_prev_node = NULL;

    if (NULL == p_head)
    {
        printf("Cannot remove item from a NULL linked list.\n");
        goto EXIT;
    }

    if (NULL != p_del_node)
    {
        if (p_head == p_del_node)
        {
            p_head = p_head->p_next;
            free(p_del_node);
            p_del_node = NULL;
        }
        else
        {
            while (NULL != p_temp_node && p_temp_node != p_del_node)
            {
                p_prev_node = p_temp_node;
                p_temp_node = p_temp_node->p_next;
            }

            if (NULL != p_temp_node)
            {
                p_prev_node->p_next = p_del_node->p_next;
                free(p_del_node);
                p_del_node = NULL;
            }
        }
    }

EXIT:
    return p_head;
}

void
node_free (node_t * p_head)
{
    node_t * p_temp_node = p_head;

    while (NULL != p_head)
    {
        p_temp_node = p_head;
        p_head      = p_head->p_next;
        free(p_temp_node);
    }
}

// End of node.c
