#include <stdlib.h>
#include <string.h>
#include "list.h"

/* ---------------------- Internal functions ---------------------- */

/*
* @brief Allocates string and returns it.
*
* Allocates memory for a string and copies the param string to
* the allocated memory.
*
* @param in       const char pointer pointing to string to be cloned
* @return         string pointer to clone of in-string
*/
static char *clone_string(const char *in)
{
    size_t len = strlen(in);
    char *out = calloc(len + 1, sizeof(char));
    strcpy(out, in);
    return out;
}

/*
 * @brief Creates a new node.
 *
 * Allocates memory for a new node and sets it's values
 * to the string param.
 *
 * @param value       Pointer to string to be set to value of new node.
 * @return            Pointer to new node.
 */
static struct node *make_node(const char *value)
{
  struct node *result = malloc(sizeof(struct node));
  result->value = clone_string(value);
  return result;
}


/* ---------------------- External functions ---------------------- */

List *list_create(void)
{
  List *result = malloc(sizeof(List));

  //result->head.value = NULL;
  result->head.next = &(result->head);
  result->head.prev = &(result->head);
  result->head.value = NULL;

  return result;
}


void list_destroy(List *lst)
{
  ListPos use = list_first(lst);

  while (!list_is_empty(lst))
  {
    use = list_remove(use);
  }

  free(lst);
}


bool list_is_empty(const List *lst)
{
  if (&(lst->head) == lst->head.next)
  {
    if (&(lst->head) == lst->head.prev)
    {
      return true;
    }
  }

  return false;
}


ListPos list_first(List *lst)
{
  ListPos pos = {
        .node = lst->head.next
    };

    return pos;
}


ListPos list_end(List *lst)
{
    ListPos pos = {
        .node = &lst->head
    };

    return pos;
}


bool list_pos_equal(ListPos p1, ListPos p2)
{
  bool equal = false;

  if (p1.node == p2.node)
  {
    equal = true;
  }

  return equal;
}


ListPos list_next(ListPos pos)
{
  ListPos next = {(pos.node->next)};

  return next;
}


ListPos list_prev(ListPos pos)
{
  ListPos prev = {(pos.node->prev)};

  return prev;
}


ListPos list_insert(ListPos pos, const char *value)
{
    // Create a new node.
    struct node *node = make_node(value);

    // Find nodes before and after (may be the same node: the head of the list).
    struct node *before = pos.node->prev;
    struct node *after = pos.node;

    // Link to node after.
    node->next = after;
    after->prev = node;

    // Link to node before.
    node->prev = before;
    before->next = node;

    // Return the position of the new element.
    pos.node = node;

    return pos;
}


ListPos list_remove(ListPos pos)
{
  ListPos next = {(pos.node->next)};

  (pos.node->prev)->next = (pos.node->next);
  (pos.node->next)->prev = (pos.node->prev);

  free(pos.node->value);
  free(pos.node);

  return next;
}


const char *list_inspect(ListPos pos)
{
  return pos.node->value;
}
