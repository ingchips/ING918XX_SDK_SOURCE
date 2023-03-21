#ifndef __BLE_STATUS_H
#define __BLE_STATUS_H

#include <stdint.h>
#include "stdbool.h"

#define BLE_CONN_HANDLE_INVALID     0xFFFF

typedef struct le_meta_event_adv_set_terminate
{
    uint8_t             status;
    uint8_t             adv_handle;
    uint16_t            conn_handle;
    uint8_t             num_complete_ext_adv;
} le_meta_event_adv_set_terminate_t;

typedef struct ble_status{
uint32_t conn_state: 1;
uint32_t scan_state: 1;
uint32_t reserved  : 30;
uint16_t conn_handle;
uint16_t conn_interval_ms;


} ble_status_t;

// func
uint16_t ble_status_connection_handle_get(void);
uint16_t ble_status_connection_interval_get(void);
bool ble_status_connection_state_get(void);
void ble_status_scan_state_set(bool state);
bool ble_status_scan_state_get(void);
void ble_status_init(void);

#endif
