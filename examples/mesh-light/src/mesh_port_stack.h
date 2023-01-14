#ifndef _MESH_PORT_STACK_H_
#define _MESH_PORT_STACK_H_

#include <stdint.h>
#include "bt_types.h"
#include "btstack_event.h"

// function define.
int mesh_att_write_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, const uint8_t *buffer, uint16_t buffer_size);
uint16_t mesh_att_read_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, uint8_t *buffer, uint16_t buffer_size);
void mesh_stack_ready(void);
void mesh_connected(uint16_t conn_handle);
void mesh_disconnected(uint16_t conn_handle, uint8_t reason);
void mesh_gatt_can_send_now(uint16_t conn_handle);
void mesh_port_init(void);

#endif

