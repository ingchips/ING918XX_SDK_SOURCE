#include "ble_status.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "gap.h"
#include "bt_types.h"
#include "btstack_event.h"
#include "errno.h"
#include "stdbool.h"
#include "platform_api.h"

// conn update.
#define CPI_VAL_TO_MS(x)    ((uint16_t)(x * 5 / 4))
#define CPI_MS_TO_VAL(x)    ((uint16_t)(x * 4 / 5))
#define CPSTT_VAL_TO_MS(x)  ((uint16_t)(x * 10))
#define CPSTT_MS_TO_VAL(x)  ((uint16_t)(x / 10))


static ble_status_t ble_status;

// get conn handle.
uint16_t ble_status_connection_handle_get(void){
    return ble_status.conn_handle;
}

// get conn interval.
uint16_t ble_status_connection_interval_get(void){
    return ble_status.conn_interval_ms;
}

// get conn state.
bool ble_status_connection_state_get(void){
    return ble_status.conn_state;
}

// set scan state.
void ble_status_scan_state_set(bool state){
    ble_status.scan_state = state;
}

// get scan state.
bool ble_status_scan_state_get(void){
    return ble_status.scan_state;
}

static void hci_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (hci_event_packet_get_type(packet))
    {
        case BTSTACK_EVENT_STATE:
            if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
                break;
            break;

        case HCI_EVENT_LE_META:
            switch (hci_event_le_meta_get_subevent_code(packet))
            {
                case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:{
                        const le_meta_event_enh_create_conn_complete_t *create_conn =
                                            decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t);
                        ble_status.conn_handle = create_conn->handle;
                        ble_status.conn_interval_ms = CPI_VAL_TO_MS(create_conn->interval);
                        ble_status.conn_state = true;
                    }
                    break;
                case HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE:{
                        const le_meta_event_conn_update_complete_t *conn_update = 
                                            decode_hci_le_meta_event(packet, le_meta_event_conn_update_complete_t);
                        if(conn_update->status == ERROR_CODE_SUCCESS){
                            ble_status.conn_interval_ms = CPI_VAL_TO_MS(conn_update->interval);
                        }
                    }
                    break;
                case HCI_SUBEVENT_LE_SCAN_TIMEOUT:{
                        ble_status_scan_state_set(0);
                    }
                    break;
                default:
                    break;
            }
            break;

        case HCI_EVENT_DISCONNECTION_COMPLETE:
            ble_status.conn_handle = BLE_CONN_HANDLE_INVALID;
            ble_status.conn_state = false;
            ble_status.conn_interval_ms = 0;
            break;

        default:
            break;
    }
}

void ble_status_init(void){
    memset(&ble_status, 0, sizeof(ble_status_t));

    static btstack_packet_callback_registration_t mesh_hci_event_callback_registration;
    mesh_hci_event_callback_registration.callback = &hci_packet_handler; // Don't support ATT event.
    hci_add_event_handler(&mesh_hci_event_callback_registration);
}





