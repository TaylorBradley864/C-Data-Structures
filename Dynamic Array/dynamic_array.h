/**
 * @file dynamic_array.h
 * @author Taylor Bradley
 * @brief Defines dynamic array structure and functions related to array data
 * management.
 * @date 2024-03-15
 */

#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include "common.h"

/**
 * @brief Defines the initial starting capacity at array creation
 *
 */
#define INITIAL_ARRAY_CAPACITY 10

/**
 * @brief Defines the size ratio threshold to decrease array size by when the
 * size falls a certain level below capacity
 *
 */
#define ARRAY_SIZE_DIVISOR 2

/**
 * @brief Structure representing a dynamic array.
 *
 */
typedef struct dynamic_array_t
{
    pthread_rwlock_t array_lock; /**< Read-write lock for thread-safe access
                                                 to array. */
    void ** pp_data;             /**< Pointer to the array data */
    size_t  size;                /**< Current number of elements in the array */
    size_t  capacity;            /**< Current capacity of the array */
    bool (*bp_compare)(
        void *, void *); /**< Pointer to the comparison function for elements */
    void (*p_destroy_function)(
        void *); /**< Pointer to the function used to destroy elements */
} dynamic_array_t;

/**
 * @brief Initialize a dynamic array with the given capacity, comparison
 * function, and destroy function.
 *
 * @param capacity The initial capacity of the array.
 * @param bp_compare Pointer to the comparison function for elements.
 * @param p_destroy Pointer to the function used to destroy elements.
 * @return A pointer to the newly created dynamic array, or NULL if
 * initialization fails.
 * @warning Returns NULL in the event of memory allocation failure, lock
 * initialization failure, or NULL function pointer inputs.
 */
dynamic_array_t * dynamic_array_init (size_t capacity,
                                      bool (*bp_compare)(void *, void *),
                                      void (*p_destroy)(void *));

/**
 * @brief Frees the memory allocated for the array and its data.
 *
 * @param p_hashtable A pointer to the dynamic array.
 */
void dynamic_array_destroy (dynamic_array_t * p_array);

/**
 * @brief Resize a dynamic array to the new capacity.
 *
 * @param p_array Pointer to the dynamic array to be resized.
 * @param new_capacity The new capacity of the array.
 * @return SUCCESS on success, FAILURE on failure.
 * @warning Returns FAILURE in the event of NULL pointer inputs
 */
int dynamic_array_resize (dynamic_array_t * p_array, size_t new_capacity);

/**
 * @brief Insert an element into the dynamic array.
 *
 * @param p_array Pointer to the dynamic array.
 * @param p_element Pointer to the element to be inserted.
 * @return SUCCESS on success, FAILURE on failure.
 * @warning Returns FAILURE in the event of NULL pointer inputs, lock failure,
 * or resize failure.
 */
int dynamic_array_insert (dynamic_array_t * p_array, void * p_element);

/**
 * @brief Remove an element from the dynamic array at the specified index.
 *
 * @param p_array Pointer to the dynamic array.
 * @param index The index of the element to be removed.
 * @return SUCCESS on success, FAILURE on failure.
 * @warning Returns FAILURE in the event of NULL pointer inputs, lock failure,
 * or resize failure.
 */
int dynamic_array_remove (dynamic_array_t * p_array, size_t index);

/**
 * @brief Get the element at the specified index in the dynamic array.
 *
 * @param p_array Pointer to the dynamic array.
 * @param index The index of the element to retrieve.
 * @return A pointer to the element at the specified index, or NULL.
 * @warning Returns NULL in the event of NULL pointer inputs, lock failure, or
 * index out of bounds error.
 */
void * dynamic_array_get_element (dynamic_array_t * p_array, size_t index);

/**
 * @brief Search for an item in the dynamic array using the search function
 * provided at initialization
 *
 * @param p_array Pointer to the dynamic array.
 * @param p_item Pointer to the item to search for.
 * @return The index of the item if found, or -1 if not found.
 * @warning Returns -1 in the event of NULL pointer inputs, lock failure, or
 * index out of bounds error.
 */
int dynamic_array_search (dynamic_array_t * p_array, void * p_item);

#endif /* DYNAMIC_ARRAY_H */

// End of dynamic_array.h
