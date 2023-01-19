#ifndef _MESH_PORT_PB_H_
#define _MESH_PORT_PB_H_

#include <stdint.h>
#include "bt_types.h"

void mesh_port_proxy_disconnect(hci_con_handle_t con_handle);
int mesh_att_write_prov_proxy_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, const uint8_t *buffer, uint16_t buffer_size);
void mesh_att_handle_prov_proxy_init(   uint16_t prov_in_hdl, uint16_t prov_out_hdl, uint16_t prov_desp_hdl,
                                        uint16_t prxy_in_hdl, uint16_t prxy_out_hdl, uint16_t prxy_desp_hdl);
void mesh_att_service_trigger_can_send_now(void);

#endif

