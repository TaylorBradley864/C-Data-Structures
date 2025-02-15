/**
 * @file node.h
 * @brief Defines the structure and functions related to singly linked list
 * nodes.
 * @author Taylor Bradley
 * @date 2023-12-18
 */

#ifndef NODE_H
#define NODE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * @brief Represents a node in a singly linked list.
 */
typedef struct node_t
{
    void * p_data; /**< Generic pointer to the data stored in the node. */
    struct node_t * p_next; /**< Pointer to the next node in the linked list. */
} node_t;

/**
 * @brief Creates a new node with the given data.
 * @param p_node_data Pointer to the data to be stored in the node.
 * @return A pointer to the newly created node.
 * @warning Returns NULL if p_node_data is NULL or in the event of memory
 * allocation failure.
 */
node_t * node_create (void * p_node_data);

/**
 * @brief Inserts a new node with data at the front of the linked list.
 * @param p_head Pointer to the head of the linked list.
 * @param p_data Pointer to the data to be stored in the new node.
 * @return A pointer to the head of the modified linked list.
 * @warning Does not execute and prints warning if p_data is NULL.
 */
node_t * node_insert_front (node_t * p_head, void * p_data);

/**
 * @brief Inserts a new node with data at the back of the linked list.
 * @param p_head Pointer to the head of the linked list.
 * @param p_data Pointer to the data to be stored in the new node.
 * @return A pointer to the head of the modified linked list.
 * @warning Does not execute and prints warning if p_data is NULL.
 */
node_t * node_insert_back (node_t * p_head, void * p_data);

/**
 * @brief Inserts a new node with data after a specified node.
 * @param p_prev_node Pointer to the node after which the new node should be
 * inserted.
 * @param p_data Pointer to the data to be stored in the new node.
 * @return A pointer to the head of the modified linked list.
 * @warning Does not execute and prints warning if p_data is NULL.
 */
node_t * node_insert_at (node_t * p_prev_node, void * p_data);

/**
 * @brief Prints the data of each node in the linked list.
 * @param p_node Pointer to the head of the linked list.
 */
void node_print (node_t * p_node);

/**
 * @brief Calculates the length of the linked list.
 * @param p_node Pointer to the head of the linked list.
 * @return The number of nodes in the linked list.
 */
int node_size (node_t * p_node);

/**
 * @brief Searches for a node with a specific value in the linked list.
 * @param p_node Pointer to the head of the linked list.
 * @param p_value Pointer to the value to search for.
 * @return A pointer to the found node or NULL if not found.
 * @warning Returns NULL and prints warning if p_data is NULL.
 */
node_t * node_search (node_t * p_node, void * p_value);

/**
 * @brief Deletes a specified node from the linked list.
 * @param p_head Pointer to the head of the linked list.
 * @param p_del_node Pointer to the node to be deleted.
 * @return A pointer to the head of the modified linked list.
 * @warning Prints warning if p_head is NULL.
 */
node_t * node_delete (node_t * p_head, node_t * p_del_node);

/**
 * @brief Frees the memory allocated for the linked list.
 * @param p_head Pointer to the head of the linked list.
 */
void node_free (node_t * p_head);

#endif /* NODE_H */

// End of node.h
