#ifndef __MESH_VIRTUAL_ADDRESSES_H
#define __MESH_VIRTUAL_ADDRESSES_H

#include <stdint.h>

#include "mesh_linked_list.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    mesh_linked_item_t item;
    uint16_t pseudo_dst;
    uint16_t hash;
    uint16_t ref_count;
    uint8_t  label_uuid[16];
} mesh_virtual_address_t;

#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif
