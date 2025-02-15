/**
 * @file threadpool.h
 * @brief Defines structures and functions for a simple thread pool
 * implementation.
 * @author Taylor Bradley
 * @date 2024-02-27
 */

#ifndef THREADPOOL_H
#define THREADPOOL_H

#define THREAD_POOL_SIZE 10 /**< Default size of the thread pool. */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <openssl/ssl.h>

/**
 * @brief Structure representing an argument to a task to be executed by the
 * thread pool.
 */
typedef struct task_t
{
    void (*p_task_function)(void *); /**< Pointer to the task function. */
    void * p_argument; /**< Pointer to the argument for the task function. */
} task_t;

/**
 * @brief Structure representing a simple thread pool.
 */
typedef struct threadpool_t
{
    pthread_mutex_t
        lock; /**< Mutex for thread-safe access to the thread pool. */
    pthread_cond_t
        not_empty; /**< Condition variable for signaling non-empty tasks. */
    pthread_cond_t
        not_full; /**< Condition variable for signaling non-full tasks. */
    pthread_t *
             p_threads; /**< Array of pthreads representing the thread pool. */
    task_t * p_tasks;   /**< Array of tasks to be executed. */
    int      size;      /**< Size of the task array. */
    int      front;     /**< Index of the front of the task array. */
    int      rear;      /**< Index of the rear of the task array. */
    int      num_threads; /**< Number of threads in the pool. */
    int is_shutdown; /**< Flag indicating whether the thread pool is shutdown.
                      */
} threadpool_t;

/**
 * @brief Initializes a thread pool with the specified number of threads.
 *
 * @param num_threads The number of threads to be created in the thread pool.
 * @return A pointer to the newly initialized thread pool.
 */
threadpool_t * threadpool_init (int num_threads);

/**
 * @brief Destroys a thread pool, freeing allocated memory.
 *
 * @param p_pool A pointer to the thread pool to be destroyed.
 */
void threadpool_destroy (threadpool_t * p_pool);

/**
 * @brief Submits a task to the thread pool for execution.
 *
 * @param pool A pointer to the thread pool.
 * @param task_function Pointer to the function representing the task.
 * @param p_argument Pointer to the argument for the task function.
 * @return An integer indicating the success of submitting the task.
 */
int threadpool_task_submit (threadpool_t * pool,
                            void (*task_function)(void *),
                            void * p_argument);

/**
 * @brief Executes a task from the thread pool.
 *
 * @param pool A pointer to the thread pool.
 * @return An integer indicating the success of executing the task.
 */
int threadpool_task_execute (threadpool_t * pool);

/**
 * @brief Function representing the behavior of each thread in the thread pool.
 *
 * @param p_arg A pointer to the thread pool.
 * @return A pointer to the result of the thread execution.
 */
void * threadpool_function (void * p_arg);

#endif /* THREADPOOL_H */

// End of threadpool.h
