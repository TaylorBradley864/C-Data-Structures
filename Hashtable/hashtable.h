/**
 * @file hashtable.h
 * @brief Defines the structure and functions related to hash table management.
 * @author Taylor Bradley
 * @date 2023-12-18, updated 2024-03-15
 */

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "node.h"

/**
 * @brief Defines the load factor calculation at (3/4) the capacity (numerator)
 *
 */
#define LOAD_FACTOR_NUMERATOR 3

/**
 * @brief Defines the load factor calculation at (3/4) the capacity
 * (denominator)
 *
 */
#define LOAD_FACTOR_DENOMINATOR 4

/**
 * @brief Defines the initial starting capacity at hashtable creation
 *
 */
#define HASHTABLE_INITIAL_CAPACITY 10

/**
 * @brief Represents a hash table structure.
 */
typedef struct hashtable_t
{
    pthread_rwlock_t hashtable_lock; /**< Read-write lock for thread-safe access
                                        to hashtable. */
    uint32_t (*p_hash_function)(
        void *); /**< Pointer to desired item hash function */
    bool (*bp_compare_function)(
        void *, void *); /**< Pointer to desired object comparison function */
    void (*p_destroy_function)(void *); /**< Pointer to item destroy function */
    size_t    size;     /**< Number of items in the hash table. */
    size_t    capacity; /**< Capacity of the hash table. */
    node_t ** pp_items; /**< Array of pointers to singly linked list nodes in
                           the hash table. */
} hashtable_t;

/**
 * @brief Creates a new hash table with the specified capacity.
 * 
 * @param capacity The initial capacity of the hash table.
 * @return A pointer to the newly created hash table.
 * @warning Returns NULL in the event of memory allocation failure, lock
 * initialization failure, or NULL function pointer inputs.
 */
hashtable_t * hashtable_create (size_t capacity,
                                uint32_t (*p_hash_function)(void *),
                                bool (*bp_compare_function)(void *, void *),
                                void (*p_destroy_function)(void *));

/**
 * @brief Hashes a value to an index in the hash table.
 * 
 * @param p_hashtable A pointer to the hash table.
 * @param p_data Generic pointer to data value to be hashed.
 * @return The hash index for the value which is found by calling the hash
 * function in the hashtable structure and modding it by the hashtable capacity.
 * @warning Returns 0 if p_hashtable or p_data is NULL.
 */
uint32_t hashtable_hash (hashtable_t * p_hashtable, void * p_data);

/**
 * @brief Checks if the specified index in the hash table is occupied.
 * 
 * @param p_hashtable A pointer to the hash table.
 * @param index The index to check.
 * @return True if the index is occupied, false otherwise.
 * @warning Returns false if p_hashtable is NULL or index is out of range.
 */
bool hashtable_index_occupied (hashtable_t * p_hashtable, uint32_t index);

/**
 * @brief Checks if the hash table is above the load factor (3/4 full) and needs
 * rehashing.
 * 
 * @param p_hashtable A pointer to the hash table.
 * @return True if rehashing is needed, false otherwise.
 * @warning Returns false if p_hashtable is NULL.
 */
bool hashtable_above_loadfactor (hashtable_t * p_hashtable);

/**
 * @brief Rehashes the hash table to based on its new capacity.
 * 
 * @param p_hashtable A pointer to the hash table.
 * @return SUCCESS if rehashing is successful, FAILURE otherwise.
 * @warning Returns FAILURE in the event of memory allocation failure.
 */
int hashtable_rehash (hashtable_t * p_hashtable);

/**
 * @brief Reallocates the hash table to a new capacity.
 * 
 * @param p_hashtable A pointer to the hash table.
 * @param new_capacity The new capacity for the hash table.
 * @return SUCESS if reallocation is successful, FAILURE otherwise.
 * @warning Returns FAILURE in the event of reallocation failure.
 */
int hashtable_realloc (hashtable_t * p_hashtable, size_t new_capacity);

/**
 * @brief Adds an item to the hash table.
 * 
 * @param p_hashtable A pointer to the hash table.
 * @param p_item A generic pointer to the item to add.
 * @return SUCCESSFUL_OP if addition is successful, UNKNOWN_ERROR otherwise.
 * @warning Returns UNKNOWN_ERROR in the event of memory allocation error,
 * rehashing failure, lock failure, or node creation failure.
 */
uint8_t hashtable_add_item (hashtable_t * p_hashtable, void * p_item);

/**
 * @brief Removes an item from the hash table.
 * 
 * @param p_hashtable A pointer to the hash table.
 * @param p_target A pointer to the node to be removed.
 * @param hash_value The hash value associated with the node.
 * @return SUCCESSFUL_OP if removal is successful, UNKNOWN_ERROR otherwise.
 * @warning Returns UNKNOWN_FAILURE if p_hashtable is NULL or lock failure
 * occurs.
 */
uint8_t hashtable_remove_item (hashtable_t * p_hashtable,
                               node_t *      p_target,
                               uint32_t      hash_value);

/**
 * @brief Searches for data in the hash table using the compare function.
 * 
 * @param p_hashtable A pointer to the hash table.
 * @param index The index in the hash table.
 * @param p_compare A generic pointer to the item containing the value to search
 * on
 * @return A void pointer to the node's data containing the found item or NULL if not
 * found.
 * @warning Returns NULL if p_hashtable or p_compare are NULL, index is out of
 * bounds, or lock failure occurs.
 */
void * hashtable_search (hashtable_t * p_hashtable,
                         uint32_t      index,
                         void *        p_compare);

/**
 * @brief Searches for a node containing data in the hash table using the compare function.
 * 
 * @param p_hashtable Pointer to the hashtable to search.
 * @param index The index in the hash table.
 * @param p_compare A generic pointer to the item containing the value to search
 * on
 * @return node_t* to the node containing the found item or NULL if not
 * found.
 * @warning Returns NULL if p_hashtable or p_compare are NULL, index is out of
 * bounds, or lock failure occurs.
 */
node_t * hashtable_get_node (hashtable_t * p_hashtable,
                             uint32_t      index,
                             void *        p_compare);

/**
 * @brief Frees the memory allocated for the hash table.
 * 
 * @param p_hashtable A pointer to the hash table.
 */
void hashtable_destroy (hashtable_t * p_hashtable);

#endif /* HASHTABLE_H */

// End of hashtable.h
