#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct node_t
{
    void * p_value;
    int (*p_compare)(void *, void *);
    struct node_t * p_right;
    struct node_t * p_left;
} node_t;

typedef struct trunk_t
{
    struct trunk_t * prev;
    char *           str;
} trunk_t;

node_t * create_node (void * value,
                      int (*p_compare)(void *, void *));
node_t * insert (node_t * p_node,
                 void *   p_value,
                 int (*p_compare)(void *, void *));
node_t * find (node_t * p_curr_node, void * p_val);
bool     is_leaf (node_t * p_node);
bool     has_only_right (node_t * p_node);
bool     has_only_left (node_t * p_node);
bool     is_only_child (node_t * p_node);
node_t * get_parent (node_t * p_root, node_t * p_child);
node_t * get_min (node_t * p_root);
node_t * delete_node (node_t * p_root, void * p_value);
void     print_postorder (node_t * p_node);
void     print_inorder (node_t * p_node);
void     print_preorder (node_t * p_node);
void     free_bst (node_t * p_node);
