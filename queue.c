#include "queue.h"
#include <stdio.h>


Queue *queue_create(void)
{
  Queue *qp = malloc(sizeof(Queue));

  qp->list = list_create();
  qp->list->head.value = NULL;

  return qp;
}


void queue_destroy(Queue *q)
{
  list_destroy(q->list);
  free(q);
}


void queue_enqueue(Queue *q, const char *value)
{
  list_insert(list_end(q->list), value);
}


char *queue_dequeue(Queue *q)
{
  const char *dequed_val = list_inspect(list_first(q->list));

  char *temp_str = malloc(strlen(dequed_val) + 1);

  strncpy(temp_str, dequed_val, strlen(dequed_val) + 1);

  list_remove(list_first(q->list));

  return temp_str;
}


bool queue_is_empty(const Queue *q)
{
  return list_is_empty(q->list);
}
