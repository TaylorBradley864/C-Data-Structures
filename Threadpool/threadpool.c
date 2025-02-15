#include "threadpool.h"

threadpool_t *
threadpool_init (int num_threads)
{
    threadpool_t * p_pool = malloc(sizeof(threadpool_t));

    if (NULL == p_pool)
    {
        fprintf(stderr, "Memory allocation failure.\n");
        goto EXIT;
    }

    p_pool->p_tasks = malloc(sizeof(task_t) * THREAD_POOL_SIZE);

    if (NULL == p_pool->p_tasks)
    {
        free(p_pool);
        p_pool = NULL;
        fprintf(stderr, "Memory allocation failure.\n");
        goto EXIT;
    }

    p_pool->size        = 0;
    p_pool->front       = 0;
    p_pool->rear        = 0;
    p_pool->is_shutdown = 0;

    pthread_mutex_init(&p_pool->lock, NULL);
    pthread_cond_init(&p_pool->not_empty, NULL);
    pthread_cond_init(&p_pool->not_full, NULL);

    p_pool->p_threads = malloc(sizeof(pthread_t) * num_threads);

    if (NULL == p_pool->p_threads)
    {
        free(p_pool->p_tasks);
        p_pool->p_tasks = NULL;
        free(p_pool);
        p_pool = NULL;
        fprintf(stderr, "Memory allocation failure.\n");
        goto EXIT;
    }

    int count = 0;

    for (int index = 0; index < num_threads; index++)
    {
        if (0
            != pthread_create(&p_pool->p_threads[index],
                              NULL,
                              threadpool_function,
                              (void *)p_pool))
        {
            free(p_pool->p_tasks);
            p_pool->p_tasks = NULL;
            fprintf(stderr, "Thread create failure.\n");

            for (int thread = 0; thread < index; thread++)
            {
                pthread_cancel(p_pool->p_threads[thread]);
            }

            for (int thread = 0; thread < index; thread++)
            {
                pthread_join(p_pool->p_threads[thread], NULL);
            }

            free(p_pool);
            p_pool = NULL;
            goto EXIT;
        }

        count++;
    }

    p_pool->num_threads = count;

EXIT:
    return p_pool;
}

void
threadpool_destroy (threadpool_t * p_pool)
{
    if (NULL != p_pool)
    {
        printf("Wrapping up threads...\n");
        pthread_mutex_lock(&p_pool->lock);
        p_pool->is_shutdown = 1;
        pthread_mutex_unlock(&p_pool->lock);

        pthread_cond_broadcast(&p_pool->not_empty);

        for (int index = 0; index < p_pool->num_threads; index++)
        {
            pthread_join(p_pool->p_threads[index], NULL);
        }

        free(p_pool->p_threads);
        free(p_pool->p_tasks);
        p_pool->p_threads = NULL;
        p_pool->p_tasks   = NULL;

        pthread_mutex_destroy(&p_pool->lock);
        pthread_cond_destroy(&p_pool->not_empty);
        pthread_cond_destroy(&p_pool->not_full);

        free(p_pool);
        p_pool = NULL;
    }
}

int
threadpool_task_submit (threadpool_t * p_pool,
                        void (*p_task_function)(void *),
                        void * p_argument)
{
    int status = 0;

    if (NULL == p_pool)
    {
        status = -1;
        goto EXIT;
    }

    pthread_mutex_lock(&p_pool->lock);

    // Wait if the task queue is full
    while (THREAD_POOL_SIZE == p_pool->size)
    {
        pthread_cond_wait(&p_pool->not_full, &p_pool->lock);
    }

    task_t task                   = { p_task_function, p_argument };
    p_pool->p_tasks[p_pool->rear] = task;
    p_pool->rear                  = (p_pool->rear + 1) % THREAD_POOL_SIZE;
    p_pool->size++;

    pthread_cond_signal(&p_pool->not_empty);
    pthread_mutex_unlock(&p_pool->lock);

EXIT:
    return status;
}

int
threadpool_task_execute (threadpool_t * p_pool)
{
    int status = 0;

    if (NULL == p_pool)
    {
        status = -1;
        goto EXIT;
    }

    task_t task;

    pthread_mutex_lock(&p_pool->lock);

    // Wait if the task queue is empty
    while ((0 == p_pool->size) && (0 == p_pool->is_shutdown))
    {
        pthread_cond_wait(&p_pool->not_empty, &p_pool->lock);
    }

    if (1 == p_pool->is_shutdown)
    {
        pthread_mutex_unlock(&p_pool->lock);
        status = -1;
        goto EXIT;
    }

    task          = p_pool->p_tasks[p_pool->front];
    p_pool->front = (p_pool->front + 1) % THREAD_POOL_SIZE;
    p_pool->size--;

    pthread_cond_signal(&p_pool->not_full);
    pthread_mutex_unlock(&p_pool->lock);

    // Execute the task
    task.p_task_function(task.p_argument);

EXIT:
    return status;
}

void *
threadpool_function (void * arg)
{
    threadpool_t * p_pool = (threadpool_t *)arg;

    for (;;)
    {
        int status = threadpool_task_execute(p_pool);

        if (-1 == status)
        {
            break;
        }
    }

    return NULL;
}

// End of threadpool.c
