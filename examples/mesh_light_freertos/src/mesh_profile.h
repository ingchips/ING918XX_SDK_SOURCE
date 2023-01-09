#ifndef _MESH_PROFILE_H_
#define _MESH_PROFILE_H_

#include <stdint.h>
#include "stdbool.h"
#include "bluetooth.h"

// adv.
void mesh_proxy_adv_set_params( uint16_t  adv_int_min, 
                                uint16_t  adv_int_max, 
                                uint8_t   adv_type,
                                uint8_t   direct_address_typ, 
                                bd_addr_t direct_address, 
                                uint8_t   channel_map, 
                                uint8_t   filter_policy);
void mesh_pb_adv_set_params(uint16_t  adv_int_min, 
                            uint16_t  adv_int_max, 
                            uint8_t   adv_type,
                            uint8_t   direct_address_typ, 
                            bd_addr_t direct_address, 
                            uint8_t   channel_map, 
                            uint8_t   filter_policy);
void mesh_proxy_adv_set_data(uint8_t advertising_data_length, uint8_t * advertising_data);
void mesh_pb_adv_set_data(uint8_t advertising_data_length, uint8_t * advertising_data);
void mesh_proxy_scan_rsp_set_data(const uint16_t length, const uint8_t *data);
void mesh_proxy_adv_enable(int enabled);
void mesh_pb_adv_enable(int enabled);

// scan.
void mesh_scan_param_set(uint16_t interval_ms, uint8_t window_ms);
void mesh_scan_run_set(uint8_t en);
void mesh_scan_start(void);
void mesh_scan_stop(void);
void mesh_setup_scan(void);

// conn params.
void ble_set_conn_interval_ms(uint16_t interval_ms);

// system
void mesh_gatt_adv_addr_set(bd_addr_t addr);
void mesh_beacon_adv_addr_set(bd_addr_t addr);
void mesh_setup_adv(void);
void mesh_server_restart(void);

// status.
uint16_t ble_get_curr_conn_interval_ms(void);
uint16_t ble_get_curr_conn_handle(void);
bool Is_ble_curr_conn_valid(void);

// init.
uint32_t setup_profile(void *data, void *user_data);

#endif
