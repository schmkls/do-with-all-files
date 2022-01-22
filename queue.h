#ifndef QUEUE_H
#define QUEUE_H

/**
 * @defgroup queue_h Queue
 *
 * @brief This module is used to perform operations on a queue.
 *
 * This module is used to perform operations to create, modify,
 * and delete a queue.
 *
 */
#define BUFSIZE 256

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "list.h"


/**
 * The type for the queue.
*/
typedef struct queue
{
    List *list;
} Queue;


/**
 * @brief Create and return an empty queue.
 *
 * This function creates an empty queue.
 *
 * @param            -
 * @return           Queue pointer to the created queue.
 */
Queue *queue_create(void);


/**
 * @brief Destroys queue.
 *
 * This function is used to free the queue and deallocate
 * its list.
 *
 * @param            Queue pointer q to queue to be destroyed.
 * @return           -
 */
void queue_destroy(Queue *q);

/**
 * @brief Enqueues the queue.
 *
 * This function creates an element at the tail of the queue
 * and adds given value to that element.
 *
 * @param            Queue pointer q to queue to be enqueued.
 * @param            Const character pointer value to value of-
                     the queues new element.
 * @return           -
 */
void queue_enqueue(Queue *q, const char *value);


/**
 * @brief Dequeues the queue.
 *
 * This function removes the element at the head of the queue
 * and deallocates the memory of that value.
 *
 * @param            Queue pointer to queue to be dequeued.
 * @return           Character pointer to value at the the removed element.
 */

char *queue_dequeue(Queue *q);

/**
 * @brief Checks if given queue is empty.
 *
 * This function returns true if given queue is empty
 * and else returns false.
 *
 * @param            Const Queue pointer to queue to be checked.
 * @return           Boolean type representing if the queue is empty.
 */
bool queue_is_empty(const Queue *q);

#endif /* QUEUE_H */
