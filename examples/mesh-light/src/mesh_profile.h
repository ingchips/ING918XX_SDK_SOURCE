#ifndef _MESH_PROFILE_H_
#define _MESH_PROFILE_H_

#include <stdint.h>
#include "stdbool.h"
#include "bluetooth.h"
#include "adv_bearer.h"


// adv
void mesh_profile_adv_proxy_set_params( uint16_t  adv_int_min, 
                                        uint16_t  adv_int_max, 
                                        uint8_t   adv_type,
                                        uint8_t   direct_address_typ, 
                                        bd_addr_t direct_address, 
                                        uint8_t   channel_map, 
                                        uint8_t   filter_policy);
void mesh_profile_adv_pb_set_params(uint16_t  adv_int_min, 
                                    uint16_t  adv_int_max, 
                                    uint8_t   adv_type,
                                    uint8_t   direct_address_typ, 
                                    bd_addr_t direct_address, 
                                    uint8_t   channel_map, 
                                    uint8_t   filter_policy);
void mesh_profile_adv_proxy_set_data(uint8_t advertising_data_length, uint8_t * advertising_data);
void mesh_profile_adv_pb_set_data(uint8_t advertising_data_length, uint8_t * advertising_data);
void mesh_profile_adv_proxy_set_scan_rsp_data(const uint16_t length, const uint8_t *data);
void mesh_profile_adv_proxy_enable(int enabled);
void mesh_profile_adv_pb_enable(int enabled);

// scan
void mesh_profile_scan_addr_set(void);
void mesh_profile_scan_param_set(uint16_t interval_ms, uint16_t window_ms);
void mesh_profile_scan_duty_start(void);
void mesh_profile_scan_single_start(uint16_t scan_timeout_ms);
void mesh_profile_scan_stop(void);

// conn params.
void ble_set_conn_interval_ms(uint16_t interval_ms);

// system
void mesh_gatt_adv_addr_set(bd_addr_t addr);
void mesh_gatt_adv_addr_get(bd_addr_t addr);
void mesh_beacon_adv_addr_set(bd_addr_t addr);
void mesh_beacon_adv_addr_get(bd_addr_t addr);
void mesh_setup_adv(void);
void mesh_server_restart(void);

// init.
uint32_t setup_profile(void *data, void *user_data);

#endif
