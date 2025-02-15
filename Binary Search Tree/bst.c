#include "bst.h"

void
trunk_show (trunk_t * p_trunk)
{
    if (p_trunk == NULL)
    {
        return;
    }

    trunk_show(p_trunk->prev);
    printf("%s", p_trunk->str);
}

void
tree_print (node_t * p_node, trunk_t * p_prev, int is_left)
{
    if (p_node == NULL)
    {
        return;
    }

    char *    prev_str = "    ";
    trunk_t * trunk    = malloc(sizeof(trunk_t));
    trunk->prev        = p_prev;
    trunk->str         = prev_str;
    tree_print(p_node->p_right, trunk, 1);

    if (!p_prev)
    {
        trunk->str = "───";
    }
    else if (is_left)
    {
        trunk->str = "┌───";
        prev_str   = "   │";
    }
    else
    {
        trunk->str  = "└───";
        p_prev->str = prev_str;
    }

    trunk_show(trunk);
    printf("%d\n", *((int *)(p_node->p_value)));

    if (p_prev)
    {
        p_prev->str = prev_str;
    }

    trunk->str = "   │";
    tree_print(p_node->p_left, trunk, 0);
    free(trunk);
}

node_t *
create_node (void * value, int (*p_compare)(void *, void *))
{
    node_t * p_node = NULL;

    if (NULL == p_compare)
    {
        fprintf(stderr, "NULL comparison function pointer provided.\n");
        goto EXIT;
    }

    p_node = calloc(1, sizeof(node_t));

    if (NULL == p_node)
    {
        fprintf(stderr, "Error: Memory not allocated!\n");
    }
    else
    {
        p_node->p_value   = value;
        p_node->p_left    = NULL;
        p_node->p_right   = NULL;
        p_node->p_compare = p_compare;
    }

EXIT:
    return p_node;
}

node_t *
insert (node_t * p_node, void * p_value, int (*p_compare)(void *, void *))
{
    node_t * p_return = NULL;

    if (NULL == p_node)
    {
        p_return = create_node(p_value, p_compare);

        if (NULL == p_return)
        {
            goto EXIT;
        }
    }
    else
    {

        int cmp_result = p_node->p_compare(p_node->p_value, p_value);

        if (cmp_result > 0)
        {
            p_node->p_right = insert(p_node->p_right, p_value, p_compare);
            p_return        = p_node;
        }
        else if (cmp_result < 0)
        {
            p_node->p_left = insert(p_node->p_left, p_value, p_compare);
            p_return       = p_node;
        }
        else
        {
            fprintf(stderr,
                    "Value already exists in BST. Keys must be distinct.\n");
        }
    }

EXIT:
    return p_return;
}

node_t *
find (node_t * p_curr_node, void * p_val)
{
    node_t * p_temp = NULL;

    if (NULL != p_curr_node)
    {
        int cmp_result = p_curr_node->p_compare(p_val, p_curr_node->p_value);

        if (0 == cmp_result)
        {
            p_temp = p_curr_node;
        }
        else
        {
            if (cmp_result > 0)
            {
                p_temp = p_curr_node->p_left;
            }
            else
            {
                p_temp = p_curr_node->p_right;
            }

            p_temp = find(p_temp, p_val);
        }
    }

    return p_temp;
}

bool
is_leaf (node_t * p_node)
{
    return ((NULL == p_node->p_left) && (NULL == p_node->p_right));
}

bool
has_only_right (node_t * p_node)
{
    return ((NULL == p_node->p_left) && (NULL != p_node->p_right));
}

bool
has_only_left (node_t * p_node)
{
    return ((NULL != p_node->p_left) && (NULL == p_node->p_right));
}

bool
is_only_child (node_t * p_node)
{
    return (has_only_left(p_node) || has_only_right(p_node));
}

node_t *
get_parent (node_t * p_root, node_t * p_child)
{
    node_t * p_return = NULL;

    if ((NULL == p_root) || (NULL == p_child) || (p_child == p_root))
    {
        p_return = NULL;
    }
    else
    {
        node_t * p_current = p_root;

        while ((NULL != p_current) && (p_current != p_child))
        {
            p_return = p_current;
            int cmp_result
                = p_current->p_compare(p_child->p_value, p_current->p_value);

            if (0 < cmp_result)
            {
                p_current = p_current->p_left;
            }
            else
            {
                p_current = p_current->p_right;
            }
        }
    }

    return p_return;
}

node_t *
get_min (node_t * p_root)
{
    node_t * p_return = p_root;

    if (NULL != p_root->p_left)
    {
        p_return = get_min(p_root->p_left);
    }

    return p_return;
}

node_t *
delete_node (node_t * p_root, void * p_value)
{
    node_t * p_return = p_root;

    if ((NULL == p_root) || (NULL == p_value))
    {
        p_return = NULL;
    }
    else
    {

        int cmp_result = p_root->p_compare(p_root->p_value, p_value);

        if (0 < cmp_result)
        {
            p_root->p_left = delete_node(p_root->p_left, p_value);
        }
        else if (0 > cmp_result)
        {
            p_root->p_right = delete_node(p_root->p_right, p_value);
        }
        else
        {

            if (is_leaf(p_root))
            {
                p_return = NULL;
                free(p_root);
            }
            else if (is_only_child(p_root))
            {
                p_return
                    = has_only_left(p_root) ? p_root->p_left : p_root->p_right;
                free(p_root);
            }
            else
            {
                node_t * p_min  = get_min(p_root->p_right);
                p_root->p_value = p_min->p_value;
                p_root->p_right = delete_node(p_root->p_right, p_min->p_value);
            }
        }
    }

    return p_return;
}

void
print_postorder (node_t * p_node)
{
    if (p_node != NULL)
    {
        print_postorder(p_node->p_left);
        print_preorder(p_node->p_right);
        printf("%d ", *((int *)(p_node->p_value)));
    }
    else
    {
        printf("\n");
    }
}

void
print_inorder (node_t * p_node)
{
    if (p_node != NULL)
    {
        print_inorder(p_node->p_left);
        printf("%d ", *((int *)(p_node->p_value)));
        print_inorder(p_node->p_right);
    }
    else
    {
        printf("\n");
    }
}

void
print_preorder (node_t * p_node)
{
    if (p_node != NULL)
    {
        printf("%d ", *((int *)(p_node->p_value)));
        print_postorder(p_node->p_left);
        print_preorder(p_node->p_right);
    }
    else
    {
        printf("\n");
    }
}

void
free_bst (node_t * p_node)
{
    if (p_node == NULL)
    {
        return;
    }

    free_bst(p_node->p_left);
    free_bst(p_node->p_right);
    free(p_node);
}

int
compare_int (void * a, void * b)
{
    int * int_a      = (int *)a;
    int * int_b      = (int *)b;
    int   return_val = 0;

    if (*int_a > *int_b)
    {
        return_val = 1;
    }
    else if (*int_a < *int_b)
    {
        return_val = -1;
    }

    return return_val;
}

int
main (void)
{
    node_t * root = create_node(&(int){ 50 }, compare_int);
    insert(root, &(int){ 45 }, compare_int);
    insert(root, &(int){ 60 }, compare_int);
    insert(root, &(int){ 30 }, compare_int);
    insert(root, &(int){ 47 }, compare_int);
    insert(root, &(int){ 58 }, compare_int);
    insert(root, &(int){ 70 }, compare_int);
    insert(root, &(int){ 68 }, compare_int);

    int val = 32;

    if (find(root, &val) != NULL)
    {
        printf("Found %d!\n", 32);
    }
    else
    {
        printf("%d not found!\n", 32);
    }

    val = 60;
    if (find(root, &val) != NULL)
    {
        printf("Found %d!\n", 60);
    }
    else
    {
        printf("%d not found!\n", 60);
    }

    printf("\nIn Order:\n");
    print_inorder(root);
    printf("\nPost Order:\n");
    print_postorder(root);
    printf("\nPre Order:\n");
    print_preorder(root);
    tree_print(root, NULL, 0);

    val             = 30;
    node_t * f      = find(root, &val);
    node_t * parent = get_parent(root, f);

    if (parent != NULL)
    {
        printf("Parent value: %d\n", *((int *)(parent->p_value)));
    }
    else
    {
        printf("Node has no parent or is the root of the tree.\n");
    }

    root = delete_node(root, &(int){ 50 });
    root = delete_node(root, &(int){ 60 });
    root = delete_node(root, &(int){ 70 });
    root = delete_node(root, &(int){ 45 });
    root = delete_node(root, &(int){ 47 });
    root = delete_node(root, &(int){ 58 });
    root = delete_node(root, &(int){ 68 });
    root = delete_node(root, &(int){ 99 });
    root = delete_node(root, &(int){ 30 });

    tree_print(root, NULL, 0);
    print_inorder(root);

    free_bst(root);
    return 0;
}
