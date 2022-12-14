#ifndef _MESH_PORT_H
#define _MESH_PORT_H

#include <stdint.h>

#include "mesh_linked_list.h"

typedef struct DTBT_mesh_msg_head
{
    mesh_linked_item_t  item;
    uint32_t src_task;
    uint32_t dst_task;
    uint32_t msg_id;
}DTBT_mesh_msg_head_t;

typedef struct Queue_mesh_msg
{
    DTBT_mesh_msg_head_t  dtmsg_head;
    uint32_t         length;
    const uint8_t   *data;
    const void      *param;
    uint16_t         conn_handle;
}Queue_mesh_msg_t;               //for FreeRTOS porting

// typedef Queue_mesh_msg_t Mesh_TimerData_TypeDef;

typedef void *os_timer_handle;

#endif
