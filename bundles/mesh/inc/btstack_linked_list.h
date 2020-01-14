// ----------------------------------------------------------------------------
// Copyright Message
// ----------------------------------------------------------------------------
//
// INGCHIPS confidential and proprietary.
// COPYRIGHT (c) 2018 by INGCHIPS
//
// All rights are reserved. Reproduction in whole or in part is
// prohibited without the written consent of the copyright owner.
//
// ----------------------------------------------------------------------------

#ifndef __BTSTACK_LINKED_LIST_H
#define __BTSTACK_LINKED_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct btstack_linked_item {
    struct btstack_linked_item *next; // <-- next element in list, or NULL
} btstack_linked_item_t;

typedef btstack_linked_item_t * btstack_linked_list_t;

typedef struct {
	int advance_on_next;
    btstack_linked_item_t * prev;	// points to the item before the current one
    btstack_linked_item_t * curr;	// points to the current item (to detect item removal)
} btstack_linked_list_iterator_t;

#ifdef __cplusplus
}
#endif

#endif // __BTSTACK_LINKED_LIST_H
