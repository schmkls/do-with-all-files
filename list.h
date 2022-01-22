#ifndef LIST_H
#define LIST_H

/**
 * @defgroup list_h List
 *
 * @brief This module is used to perform operations on a list.
 *
 * This module is used to perform operations to create, modify,
 * and delete doubly linked lists.
 *
 */


#include <stdbool.h>


/**
 * The type for a node in the list
*/
struct node
{
    struct node *next;
    struct node *prev;
    char *value;
};

/**
 * The type for a list
*/
typedef struct list
{
    struct node head;
} List;

/**
 * The type for a list position
*/
typedef struct list_pos
{
    struct node *node;
} ListPos;


/**
 * @brief Create and return an empty list.
 *
 * This function creates an empty list whos next and previous-pointers
 * points to its own head. The value of the head is set to NULL.
 *
 * @param            -
 *
 * @return           List pointer to the created list.
 */
List *list_create(void);


/**
 * @brief Deallocates the list and all of it's values (if any).
 *
 * Unlinks all links directly or indirectly linked to the list pointed to
 * by list pointer passed as param. Also deallocates all nodes in
 * the list and deallocates the lists.
 *
 * @param lst        Pointer to list structure.
 * @return           -
 */
void list_destroy(List *lst);


/**
 * @brief Ccheks if the list is empty.
 *
 * Returns true if the list pointed to by the param is empty
 * (meaning the head node is not linked to any other node).
 * Else returns false.
 *
 * @param lst        pointer to list to be checked.
 * @return           Boolean true if list is empty, else boolean false.
 */
bool list_is_empty(const List *lst);


/**
 * @brief Gets the first position of the list.
 *
 * Returns a ListPos whose node pointer points to the head of node
 * after the node in the List pointed to by the param.
 *
 * @param lst        Pointer to lst whose first position is to be
 *                   returned.
 * @return           A ListPos with a pointer to the node after the
                     last node in the list.
 *
 */
ListPos list_first(List *lst);

/**
 * @brief Gets the position after the last element.
 *
 * Returns a ListPos whose node pointer points to the head of node
 * in List pointed to by the param.
 *
 * @param lst        Pointer to lst whose first position is to be
 *                   returned.
 * @return           A ListPos with a pointer to the node after the
                     last node in the list.
 *
 */
 ListPos list_end(List *lst);


 /**
  * @brief Checks equality between two positions.
  *
  * Compares to list positions and returns true if their node pointers point
  * to the same adress.
  *
  * @param p1        First list position.
  * @param p2        Second list position.
  * @return          Bool true if the positions are equal, else bool false.
  *
  */
bool list_pos_equal(ListPos p1, ListPos p2);


/**
 * @brief Forwards to the next position.
 *
 * Returns ListPos whose node pointer points to the next node
 * from the node in the ListPos in the param.
 *
 * @param pos       ListPos list position.
 * @return          ListPos next list position.
 *
 */
ListPos list_next(ListPos pos);

/**
 * @brief Backwards to the previous position.
 *
 * Returns ListPos whose node pointer points to the previous node
 * from the node in the ListPos in the param.
 *
 * @param pos       ListPos list position.
 * @return          ListPos previous list position.
 *
 */
ListPos list_prev(ListPos pos);


/**
 * @brief Inserts the value before the position and return the
 * position of the new element.
 *
 * Creates a new node and inserts it at position in param pos by
 * linking pos to the new node and the new node to what used to be
 * the node after pos.
 *
 * @param pos       ListPos list position where new node is inserted.
 * @param value     pointer to char at beginning of string value of new node.
 * @return          ListPos list position of new node.
 *
 */
ListPos list_insert(ListPos pos, const char *value);


/**
 * @brief Remove the value at the position and return the position of
 * the next element.
 *
 * Removes node at list position by linking the previous and next
 * nodes to each other. Deallocates the node.
 *
 * @param pos       ListPos list position to be removed.
 * @return          ListPos next list position from pos.
 *
 */
ListPos list_remove(ListPos pos);


/**
 * @brief Gets the value at list position.
 *
 * Returns value string in node pointed to by node pointer of pos.
 *
 * @param pos       ListPos list position whose value is returned.
 * @return          Pointer to char at beginning of value string.
 *
 */
const char *list_inspect(ListPos pos);


#endif /* LIST_H */
