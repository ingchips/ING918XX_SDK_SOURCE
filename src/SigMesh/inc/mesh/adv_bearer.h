#ifndef __ADV_BEARER_H
#define __ADV_BEARER_H

#include <stdint.h>
#include <stdbool.h>

#include "btstack_defines.h"
#include "bluetooth.h"
#include "bt_types.h"


#if defined __cplusplus
extern "C" {
#endif

typedef struct {
    void * next;
    uint8_t adv_length;
    uint8_t adv_data[31];
} adv_bearer_connectable_advertisement_data_item_t;

/**
 * @brief Set Advertisement Paramters
 * @param adv_int_min
 * @param adv_int_max
 * @param adv_type
 * @param direct_address_type
 * @param direct_address
 * @param channel_map
 * @param filter_policy
 * @note own_address_type is used from gap_random_address_set_mode
 */
void adv_bearer_advertisements_set_params(uint16_t adv_int_min, uint16_t adv_int_max, uint8_t adv_type,
    uint8_t direct_address_typ, bd_addr_t direct_address, uint8_t channel_map, uint8_t filter_policy);

/**
 * @brief Set Advertisement Response Paramters
 * @param data rsp data
 * @param len max 29 bytes
 */
void adv_bearer_adv_set_scan_rsp_data(uint8_t *data, uint8_t len);

/**
 * @brief Get Advertisement Response Paramters
 * @param data rsp data
 * @param len max 29 bytes
 */
void adv_bearer_adv_get_scan_rsp_data(uint8_t *data, uint8_t *len);

/** 
 * @brief Enable/Disable Advertisements. OFF by default.
 * @param enabled
 */
void adv_bearer_advertisements_enable(int enabled);

/**
 * Send Mesh Message
 * @param data to send
 * @param data_len max 29 bytes
 * @param count number of transmissions
 * @param interval between transmission
 */
void adv_bearer_send_network_pdu(const uint8_t * network_pdu, uint16_t size, uint8_t count, uint16_t interval);

/**
 * Send particular message type:Mesh Beacon, PB-ADV
 * @param data to send
 * @param data_len max 29 bytes
 */
void adv_bearer_send_beacon(const uint8_t * beacon_update, uint16_t size);
void adv_bearer_send_provisioning_pdu(const uint8_t * pb_adv_pdu, uint16_t size); 

#if defined __cplusplus
}
#endif

#endif // __ADV_BEARER_H
