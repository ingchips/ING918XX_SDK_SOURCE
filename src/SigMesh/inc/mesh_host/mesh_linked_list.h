/**
 * @title Linked List
 *
 */

#ifndef MESH_LINKED_LIST_H
#define MESH_LINKED_LIST_H

#include "stdbool.h"

/* API_START */
    
typedef struct mesh_linked_item {
    struct mesh_linked_item *next; // <-- next element in list, or NULL
} mesh_linked_item_t;

typedef mesh_linked_item_t * mesh_linked_list_t;

typedef struct {
    int advance_on_next;
    mesh_linked_item_t * prev;    // points to the item before the current one
    mesh_linked_item_t * curr;    // points to the current item (to detect item removal)
} mesh_linked_list_iterator_t;


/**
 * @brief Test if list is empty.
 * @param list
 * @return true if list is empty
 */
bool mesh_linked_list_empty(mesh_linked_list_t * list);

/**
 * @brief Add item to list as first element.
 * @param list
 * @param item
 * @return true if item was added, false if item already in list
 */
bool mesh_linked_list_add(mesh_linked_list_t * list, mesh_linked_item_t *item);

/**
 * @brief Add item to list as last element.
 * @param list
 * @param item
 * @return true if item was added, false if item already in list
 */
bool mesh_linked_list_add_tail(mesh_linked_list_t * list, mesh_linked_item_t *item);

/**
 * @brief Pop (get + remove) first element.
 * @param list
 * @return first element or NULL if list is empty
 */
mesh_linked_item_t * mesh_linked_list_pop(mesh_linked_list_t * list);

/**
 * @brief Remove item from list
 * @param list
 * @param item
 * @return true if item was removed, false if it is no't in list
 */
bool mesh_linked_list_remove(mesh_linked_list_t * list, mesh_linked_item_t *item);

/**
 * @brief Get first element.
 * @param list
 * @return first element or NULL if list is empty
 */
mesh_linked_item_t * mesh_linked_list_get_first_item(mesh_linked_list_t * list);

/**
 * @brief Get last element.
 * @param list
 * @return first element or NULL if list is empty
 */
mesh_linked_item_t * mesh_linked_list_get_last_item(mesh_linked_list_t * list);   

/**
 * @brief Counts number of items in list
 * @return number of items in list
 */
int mesh_linked_list_count(mesh_linked_list_t * list);



/**
 * @brief Initialize Linked List Iterator
 * @note robust against removal of current element by mesh_linked_list_remove
 * @param it iterator context
 * @param list
 */
void mesh_linked_list_iterator_init(mesh_linked_list_iterator_t * it, mesh_linked_list_t * list);

/**
 * @brief Has next element
 * @param it iterator context
 * @return true if next element is available
 */
bool mesh_linked_list_iterator_has_next(mesh_linked_list_iterator_t * it);

/**
 * @brief Get next list eleemnt
 * @param it iterator context
 * @return list element
 */
mesh_linked_item_t * mesh_linked_list_iterator_next(mesh_linked_list_iterator_t * it);

/**
 * @brief Remove current list element from list
 * @param it iterator context
 */
void mesh_linked_list_iterator_remove(mesh_linked_list_iterator_t * it);

/* API_END */



#endif // MESH_LINKED_LIST_H
