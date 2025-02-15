#include <stdio.h>
#include <stdlib.h>

typedef struct node_t
{
    void *          p_value;
    struct node_t * p_next;
} node_t;

typedef struct cll_t
{
    node_t * p_head;
    node_t * p_tail;
    size_t   size;
} cll_t;

cll_t *  cll_init (void);
node_t * cll_node_create (void * p_value);
void     cll_destroy (cll_t * p_cll);
void     cll_insert_front (cll_t * p_cll, void * p_value);
void     cll_insert_back (cll_t * p_cll, void * p_value);
void     cll_delete_front (cll_t * p_cll);
void     cll_delete_back (cll_t * p_cll);
void     cll_print (node_t * p_head);
