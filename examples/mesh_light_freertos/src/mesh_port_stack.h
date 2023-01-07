#ifndef _MESH_PORT_STACK_H_
#define _MESH_PORT_STACK_H_

#include <stdint.h>
#include "bt_types.h"
#include "btstack_event.h"

#define ATT_HANDLE_INVALID 0xFFFF
#define CON_HANDLE_INVALID 0xFFFF

#define USER_MSG_ID_MESH_ATT_NOTIFY  0xA5
#define USER_MSG_ID_MESH_PROXY_ADV   0xA6


//#define is_curr_host_task()    (bool)(platform_get_task_handle(PLATFORM_TASK_HOST) == (uintptr_t)xTaskGetCurrentTaskHandle())
//#define ble_memory_alloc(x)     btstack_memory_alloc(x)
//#define ble_memory_free(x)      btstack_memory_free(x)


// define: host to mesh msg id.
#define MESH_MSG_MESH_DATA_IN        0xF0
#define MESH_MSG_MESH_STACK_READY    0xF1
#define MESH_MSG_MESH_CONNECTED      0xF2
#define MESH_MSG_MESH_DISCONNECTED   0xF3

// struct define.
typedef void (*f_mesh_process)(void *cb_context);
typedef struct
{
    // for message transport control
    f_mesh_process cb_process;
    void * cb_context;
    // for application.
    uint32_t id;
    uint8_t *data;
    uint16_t data_len;
    hci_con_handle_t conn_handle;
    uint16_t att_handle;
    uint8_t reason;    
}MeshMsg_t;

// function define.
int mesh_att_write_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, const uint8_t *buffer, uint16_t buffer_size);
uint16_t mesh_att_read_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, uint8_t *buffer, uint16_t buffer_size);
void mesh_stack_ready(void);
void mesh_connected(uint16_t conn_handle);
void mesh_disconnected(uint16_t conn_handle, uint8_t reason);

void mesh_ble_connect_callback(uint16_t handle);
void mesh_ble_disconnect_callback(uint16_t handle);
void mesh_conn_params_update_complete_callback(uint8_t status, uint16_t handle, uint16_t interval, uint16_t sup_timeout);
void mesh_on_off_server_control_callback(void);

void mesh_gatt_can_send_now(uint16_t conn_handle);
void mesh_port_node_reset_handler(void);
void mesh_port_init(void);

#endif

