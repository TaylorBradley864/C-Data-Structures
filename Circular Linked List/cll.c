#include "cll.h"

cll_t *
cll_init (void)
{
    cll_t * p_cll = calloc(1, sizeof(cll_t));

    if (NULL != p_cll)
    {
        p_cll->p_head = NULL;
        p_cll->p_tail = NULL;
        p_cll->size   = 0;
    }

    return p_cll;
}

node_t *
cll_node_create (void * p_value)
{
    node_t * p_node = calloc(1, sizeof(node_t));

    if (NULL != p_node)
    {
        p_node->p_value = p_value;
        p_node->p_next  = NULL;
    }

    return p_node;
}

void
cll_destroy (cll_t * p_cll)
{
    if (NULL != p_cll)
    {
        node_t * p_current = p_cll->p_head;

        if (p_current != NULL)
        {
            do
            {
                node_t * p_temp = p_current;
                p_current       = p_current->p_next;
                free(p_temp);
            } while (p_current != p_cll->p_head);
        }

        free(p_cll);
        p_cll = NULL;
    }
}

void
cll_insert_front (cll_t * p_cll, void * p_value)
{
    node_t * p_node = cll_node_create(p_value);

    if (NULL != p_node)
    {
        if (NULL == p_cll->p_head)
        {
            p_node->p_next = p_node;
            p_cll->p_head  = p_node;
            p_cll->p_tail  = p_node;
        }
        else
        {
            p_node->p_next        = p_cll->p_head;
            p_cll->p_tail->p_next = p_node;
            p_cll->p_head         = p_node;
        }

        p_cll->size++;
    }
}

void
cll_insert_back (cll_t * p_cll, void * p_value)
{
    node_t * p_node = cll_node_create(p_value);

    if (NULL != p_node)
    {
        if (NULL == p_cll->p_head)
        {
            p_node->p_next = p_node;
            p_cll->p_head  = p_node;
            p_cll->p_tail  = p_node;
        }
        else
        {
            p_cll->p_tail->p_next = p_node;
            p_node->p_next        = p_cll->p_head;
            p_cll->p_tail         = p_node;
        }

        p_cll->size++;
    }
}

void
cll_delete_front (cll_t * p_cll)
{
    if (NULL != p_cll)
    {
        if (0 == p_cll->size)
        {
            printf("Cannot remove from empty CLL.\n");
        }
        else if (1 == p_cll->size)
        {
            free(p_cll->p_head);
            p_cll->p_head = NULL;
            p_cll->p_tail = NULL;
            p_cll->size--;
        }
        else
        {
            node_t * p_temp       = p_cll->p_head;
            p_cll->p_tail->p_next = p_temp->p_next;
            p_cll->p_head         = p_temp->p_next;
            free(p_temp);
            p_cll->size--;
        }
    }
}

void
cll_delete_back (cll_t * p_cll)
{
    if (NULL != p_cll)
    {
        if (0 == p_cll->size)
        {
            printf("Cannot remove from empty CLL.\n");
        }
        else if (1 == p_cll->size)
        {
            free(p_cll->p_head);
            p_cll->p_head = NULL;
            p_cll->p_tail = NULL;
            p_cll->size--;
        }
        else
        {
            node_t * p_current = p_cll->p_head;

            while (p_current->p_next != p_cll->p_tail)
            {
                p_current = p_current->p_next;
            }

            node_t * p_temp       = p_current->p_next;
            p_cll->p_tail         = p_current;
            p_cll->p_tail->p_next = p_cll->p_head;
            free(p_temp);
            p_cll->size--;
        }
    }
}

void
cll_print (node_t * p_head)
{
    if (NULL != p_head)
    {
        node_t * p_current = p_head;

        do
        {
            printf("%p -> ", p_current->p_value);
            p_current = p_current->p_next;
        } while (p_current != p_head);

        printf("\n");
    }
}

int
main ()
{
    cll_t * my_cll = cll_init();

    int values[]   = { 10, 20, 30, 40, 50 };
    int num_values = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < num_values; ++i)
    {
        cll_insert_back(my_cll, &values[i]);
    }

    printf("Circular Linked List:\n");
    cll_print(my_cll->p_head);

    cll_delete_front(my_cll);

    printf("Circular Linked List after Front deletion:\n");
    cll_print(my_cll->p_head);

    cll_delete_back(my_cll);

    printf("Circular Linked List after Rear deletion:\n");
    cll_print(my_cll->p_head);

    cll_destroy(my_cll);

    return 0;
}
