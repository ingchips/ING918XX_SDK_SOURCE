#ifndef _MESH_PROFILE_H_
#define _MESH_PROFILE_H_

#include <stdint.h>
#include "stdbool.h"
#include "bluetooth.h"
#include "adv_bearer.h"

// adv.
typedef struct {

	void (*proxy_set_params)(   uint16_t  adv_int_min, 
                                uint16_t  adv_int_max, 
                                uint8_t   adv_type,
                                uint8_t   direct_address_typ, 
                                bd_addr_t direct_address, 
                                uint8_t   channel_map, 
                                uint8_t   filter_policy);
    void (*pb_set_params)(  uint16_t  adv_int_min, 
                            uint16_t  adv_int_max, 
                            uint8_t   adv_type,
                            uint8_t   direct_address_typ, 
                            bd_addr_t direct_address, 
                            uint8_t   channel_map, 
                            uint8_t   filter_policy);
    void (*proxy_set_data)(uint8_t len, uint8_t * data);
    void (*pb_set_data)(uint8_t len, uint8_t * data);
    void (*proxy_set_scan_rsp_data)(const uint16_t len, const uint8_t *data);
    void (*proxy_enable)(int enabled);
    void (*pb_enable)(int enabled);

} mesh_profile_api_adv_t;

// scan.
typedef struct {
    
    void (*param_set)(uint16_t interval_ms, uint16_t window_ms);
    void (*duty_start)(void);
    void (*single_start)(uint16_t scan_timeout_ms);
    void (*stop)(void);
    
} mesh_profile_api_scan_t;

// event.
typedef struct {
	uint8_t *adv_data;
	uint8_t adv_length;
	uint8_t type;
	uint8_t count;
	uint16_t interval_ms;
} mesh_adv_bearer_send_msg_begin_t;

typedef struct {
    
    void (*send_msg_begin)(mesh_adv_bearer_send_msg_begin_t *msg);
    void (*send_msg_end)(void);
    
} mesh_profile_api_adv_bearer_event_t;

// ATT.
typedef struct {
    
    int (*att_server_notify)(hci_con_handle_t con_handle, uint16_t attribute_handle, uint8_t *value, uint16_t value_len);
    void (*att_req_can_send_now_evt)(hci_con_handle_t con_handle);
    
} mesh_profile_api_att_t;

// CONN.
typedef struct {
    
    void (*conn_interval_update)(uint16_t interval_ms);
    
} mesh_profile_api_conn_t;

// api.
typedef struct {
    
    const mesh_profile_api_adv_t  * adv;
    const mesh_profile_api_scan_t * scan;
    const mesh_profile_api_adv_bearer_event_t * adv_bearer_evt;
    const mesh_profile_api_att_t * att;
    const mesh_profile_api_conn_t * conn;

} mesh_profile_api_t;

/**
 * @brief Register user callback.
 * 
 * @param api 
 */
void mesh_profile_register(const mesh_profile_api_t * api);

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
void mesh_profile_scan_param_set(uint16_t interval_ms, uint16_t window_ms);
void mesh_profile_scan_duty_start(void);
void mesh_profile_scan_single_start(uint16_t scan_timeout_ms);
void mesh_profile_scan_stop(void);
void mesh_setup_scan(void);

// conn params.
void mesh_profile_conn_interval_update_ms(uint16_t interval_ms);

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
