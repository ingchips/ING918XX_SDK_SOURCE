##  ----------------------------------------------------------------------------
##  Copyright Message
##  ----------------------------------------------------------------------------
##
##  INGCHIPS confidential and proprietary.
##  COPYRIGHT (c) 2018 by INGCHIPS
##
##  All rights are reserved. Reproduction in whole or in part is
##  prohibited without the written consent of the copyright owner.
##
##  ----------------------------------------------------------------------------

type
  btstack_linked_item_t* {.importc: "btstack_linked_item_t",
                          header: "btstack_linked_list.h", bycopy.} = object
    next* {.importc: "next".}: ptr btstack_linked_item_t ##  <-- next element in list, or NULL

  btstack_linked_list_t* = ptr btstack_linked_item_t
  btstack_linked_list_iterator_t* {.importc: "btstack_linked_list_iterator_t",
                                   header: "btstack_linked_list.h", bycopy.} = object
    advance_on_next* {.importc: "advance_on_next".}: cint
    prev* {.importc: "prev".}: ptr btstack_linked_item_t ##  points to the item before the current one
    curr* {.importc: "curr".}: ptr btstack_linked_item_t ##  points to the current item (to detect item removal)

