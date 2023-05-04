// ----------------------------------------------------------------------------
// Copyright Message
// ----------------------------------------------------------------------------
//
// INGCHIPS confidential and proprietary.
// COPYRIGHT (c) 2018 by INGCHIPS
//
// All rights are reserved. Reproduction in whole or in part is
// prohibited without the written consent of the copyright owner.
//
// ----------------------------------------------------------------------------

#ifndef __BT_TYPES_H
#define __BT_TYPES_H
#include <stdint.h>
#include "bluetooth.h"
#include "btstack_linked_list.h"

#define BIT(pos) (1UL<<(pos))

typedef enum {
    GAP_RANDOM_ADDRESS_OFF = 0,
    GAP_RANDOM_ADDRESS_NON_RESOLVABLE,
    GAP_RANDOM_ADDRESS_RESOLVABLE,
} gap_random_address_type_t;

/**
 * @brief 128 bit key used with AES128 in Security Manager
 */
typedef uint8_t sm_key_t[16];


// packet handler
typedef void (*btstack_packet_handler_t) (uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size);

// packet callback supporting multiple registrations
typedef struct {
    btstack_linked_item_t    item;
    btstack_packet_handler_t callback;
} btstack_packet_callback_registration_t;

// packet handler
typedef void (*user_packet_handler_t) (uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size);

//macro definition:

/**
 *  state of Bluetooth controller
 */
typedef enum {
    HCI_POWER_OFF = 0,
    HCI_POWER_ON,
    HCI_POWER_SLEEP
} HCI_POWER_MODE;

/**
 * State of BTstack
 */
typedef enum {
    HCI_STATE_OFF = 0,
    HCI_STATE_INITIALIZING,
    HCI_STATE_WORKING,
    HCI_STATE_HALTING,
    HCI_STATE_SLEEPING,
    HCI_STATE_FALLING_ASLEEP
} HCI_STATE;

/**
 * @format 1
 * @param state
 */
#define BTSTACK_EVENT_STATE                                   0x60

/**
 * @format
 */
#define ATT_EVENT_CAN_SEND_NOW                                0xB7

#define BTSTACK_EVENT_USER_MSG                                0xFF

#endif

