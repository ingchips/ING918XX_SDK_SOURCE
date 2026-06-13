// ----------------------------------------------------------------------------
// Copyright Message
// ----------------------------------------------------------------------------
//
// INGCHIPS confidential and proprietary.
// COPYRIGHT (c) 2018-2023 by INGCHIPS
//
// All rights are reserved. Reproduction in whole or in part is
// prohibited without the written consent of the copyright owner.
//
// ----------------------------------------------------------------------------

#ifndef __BTSTACK_DEFINES_H
#define __BTSTACK_DEFINES_H

#include <stdint.h>

// DEFINES

// L2CAP data
#define L2CAP_DATA_PACKET       0x06

// Attribute protocol data
#define ATT_DATA_PACKET         0x08

// Security Manager protocol data
#define SM_DATA_PACKET          0x09

// ERRORS
// we start with 0x51 for Bluetooth LE stack errors
#define BTSTACK_BUSY                                       0x55
#define BTSTACK_MEMORY_ALLOC_FAILED                        0x56
#define BTSTACK_ACL_BUFFERS_FULL                           0x57
#define BTSTACK_LE_CHANNEL_NOT_EXIST                       0x59

// l2cap errors - enumeration by the command that created them
#define L2CAP_SERVICE_NOT_REGISTERED                       0x6B
#define L2CAP_CONNECTION_INSUFFICIENT_SECURITY             0x6C

#define ATT_HANDLE_VALUE_INDICATION_IN_PROGRESS            0x90
#define ATT_HANDLE_VALUE_INDICATION_TIMEOUT                0x91

// compatibility for typo
#define ATT_HANDLE_VALUE_INDICATION_IN_PORGRESS            ATT_HANDLE_VALUE_INDICATION_IN_PROGRESS

#define GATT_CLIENT_NOT_CONNECTED                          0x93
#define GATT_CLIENT_BUSY                                   0x94
#define GATT_CLIENT_IN_WRONG_STATE                         0x95
#define GATT_CLIENT_VALUE_TOO_LONG                         0x97
#define GATT_CLIENT_CHARACTERISTIC_NOTIFICATION_NOT_SUPPORTED 0x98
#define GATT_CLIENT_CHARACTERISTIC_INDICATION_NOT_SUPPORTED   0x99

#define EATT_BEARER_NOT_FOUND                              0xA0
#define EATT_BEARER_TYPE_NOT_SUPPORTED                     0xA1

// hci con handles (12 bit): 0x0000..0x0fff
#define HCI_CON_HANDLE_INVALID 0xffff

// EVENTS

// data: event(8), len(8), nr hci connections
#define BTSTACK_EVENT_NR_CONNECTIONS_CHANGED               0x61

/**
 * @format
 */
#define BTSTACK_EVENT_POWERON_FAILED                       0x62

// additional HCI events

/**
 * @brief Outgoing packet
 */
#define HCI_EVENT_TRANSPORT_PACKET_SENT                    0x6E

// L2CAP EVENTS

/**
 * @brief an L2CAP channel is opened
 * @ref `l2cap_event_channel_opened_t`
 */
#define L2CAP_EVENT_CHANNEL_OPENED                         0x70

/*
 * @brief an L2CAP channel is closed
 * @ref `l2cap_event_channel_closed_t`
 */
#define L2CAP_EVENT_CHANNEL_CLOSED                         0x71

/**
 * @format BH222
 * @param address
 * @param handle
 * @param psm
 * @param local_cid
 * @param remote_cid
 */
#define L2CAP_EVENT_INCOMING_CONNECTION                    0x72

// ??
// data: event(8), len(8), handle(16)
#define L2CAP_EVENT_TIMEOUT_CHECK                          0x73

/**
 * @format H2222
 * @param handle
 * @param interval_min
 * @param interval_max
 * @param latencey
 * @param timeout_multiplier
 */
#define L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST    0x76

// data: event(8), len(8), handle(16), result (16) (0 == ok, 1 == fail)
 /**
  * @format H2
  * @param handle
  * @result
  */
#define L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_RESPONSE   0x77

/**
 * @format 2
 * @param local_cid
 */
#define L2CAP_EVENT_CAN_SEND_NOW                           0x78

 /**
  * @format H12
  * @param handle
  * @param identifier
  * @result
  */
#define L2CAP_EVENT_COMMAND_REJECT_RESPONSE                0x79

/**
 * @brief LE credit channel : an SDU (complete package) event
 *
 * Use `decode_l2cap_event(l2cap_event_complete_sdu_t, package)` to decode this event.
 *
 * @ref `l2cap_event_complete_sdu_t`
*/
#define L2CAP_EVENT_COMPLETED_SDU_PACKET                   0x7A

/**
 * @brief LE credit channel : a fragment (K-frame) of an SDU event
 *
 * Use `decode_l2cap_event(l2cap_event_fragment_sdu_t, package)` to decode this event.
 *
 * @ref `l2cap_event_fragment_sdu_t`
*/
#define L2CAP_EVENT_FRAGMENT_SDU_PACKET                    0x7B

/*
 * @format 2
 * @param local_cid
 */
// #define L2CAP_EVENT_ERTM_BUFFER_RELEASED                   0x7bu

// L2CAP Channel in LE Credit-based Flow-Control Mode (CBM)

/**
 * @format 1BH2222
 * @param address_type
 * @param address
 * @param handle
 * @param psm
 * @param local_cid
 * @param remote_cid
 * @param remote_mtu
 */
#define L2CAP_EVENT_CBM_INCOMING_CONNECTION                 0x7cu

/**
 * @format 11BH122222
 * @param status
 * @param address_type
 * @param address
 * @param handle
 * @param incoming
 * @param psm
 * @param local_cid
 * @param remote_cid
 * @param local_mtu
 * @param remote_mtu
 */
#define L2CAP_EVENT_CBM_CHANNEL_OPENED                      0x7du

/*
 * @format
 */
#define L2CAP_EVENT_TRIGGER_RUN                             0x7eu

/**
 * @format 1BH212
 * @param address_type
 * @param address
 * @param handle
 * @param psm
 * @param num_channels
 * @param local_cid first new cid
 */
#define L2CAP_EVENT_ECBM_INCOMING_CONNECTION               0x7fu

/**
 * @format 11BH122222
 * @param status
 * @param address_type
 * @param address
 * @param handle
 * @param incoming
 * @param psm
 * @param local_cid
 * @param remote_cid
 * @param local_mtu
 * @param remote_mtu
 */
#define L2CAP_EVENT_ECBM_CHANNEL_OPENED              0x8au

/*
 * @format 222
 * @param remote_cid
 * @param mtu
 * @param mps
 */
#define L2CAP_EVENT_ECBM_RECONFIGURED                0x8bu

/*
 * @format 22
 * @param local_cid
 * @param reconfigure_result
 */
#define L2CAP_EVENT_ECBM_RECONFIGURATION_COMPLETE    0x8cu

/*
 * @format 2
 * @param local_cid
 */
#define L2CAP_EVENT_PACKET_SENT                            0x8du//0x7au btstack value

/**
 * @format H1
 * @param handle
 * @param status
 */
#define GATT_EVENT_QUERY_COMPLETE                                0xA0

/**
 * @format HX
 * @param handle
 * @param service
 */
#define GATT_EVENT_SERVICE_QUERY_RESULT                          0xA1

/**
 * @format HY
 * @param handle
 * @param characteristic
 */
#define GATT_EVENT_CHARACTERISTIC_QUERY_RESULT                   0xA2

/**
 * @format H2X
 * @param handle
 * @param include_handle
 * @param service
 */
#define GATT_EVENT_INCLUDED_SERVICE_QUERY_RESULT                 0xA3

/**
 * @format HZ
 * @param handle
 * @param characteristic_descriptor
 */
#define GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT   0xA4

/**
 * @format H2LV
 * @param handle
 * @param value_handle
 * @param value_length
 * @param value
 */
#define GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT             0xA5

/**
 * @format H22LV
 * @param handle
 * @param value_handle
 * @param value_offset
 * @param value_length
 * @param value
 */
#define GATT_EVENT_LONG_CHARACTERISTIC_VALUE_QUERY_RESULT        0xA6

/**
 * @format H2LV
 * @param handle
 * @param value_handle
 * @param value_length
 * @param value
 */
#define GATT_EVENT_NOTIFICATION                                  0xA7

/**
 * @format H2LV
 * @param handle
 * @param value_handle
 * @param value_length
 * @param value
 */
#define GATT_EVENT_INDICATION                                    0xA8

/**
 * @format H2LV
 * @param descriptor_handle
 * @param descriptor_length
 * @param descriptor
 */
#define GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT        0xA9

/**
 * @format H2LV
 * @param handle
 * @param descriptor_offset
 * @param descriptor_length
 * @param descriptor
 */
#define GATT_EVENT_LONG_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT   0xAA

/**
 * @format H2
 * @param handle
 * @param MTU
 */
#define GATT_EVENT_MTU                                           0xAB

/**
 * @format 1HH4
 * @param type            GATT_EVENT_NOTIFICATION or GATT_EVENT_INDICATION
 * @param value_handle
 * @param size
 * @param data
 */
#define GATT_EVENT_UNHANDLED_SERVER_VALUE                         0xAC

/**
 * @format 11BH
 * @param status
 * @param address_type
 * @param address
 * @param handle
 */
 #define GATT_EVENT_CONNECTED                                     0xAD

 /**
 * @format H
 * @param handle
 */
 #define GATT_EVENT_DISCONNECTED                                  0xAE

 /**
 * @format H2
 * @param handle
 * @param MTU
 */
#define ATT_EVENT_MTU_EXCHANGE_COMPLETE                          0xB5

 /**
  * @format 1H2
  * @param status
  * @param conn_handle
  * @param attribute_handle
  */
#define ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE               0xB6

 /**
  * @format H1B
  * @param handle
  * @param addr_type
  * @param address
  */
#define SM_EVENT_JUST_WORKS_REQUEST                              0xD0

 /**
  * @format H1B
  * @param handle
  * @param addr_type
  * @param address
  */
#define SM_EVENT_JUST_WORKS_CANCEL                               0xD1

 /**
  * @format H1B4
  * @param handle
  * @param addr_type
  * @param address
  * @param passkey
  */
#define SM_EVENT_PASSKEY_DISPLAY_NUMBER                          0xD2

 /**
  * @format H1B
  * @param handle
  * @param addr_type
  * @param address
  */
#define SM_EVENT_PASSKEY_DISPLAY_CANCEL                          0xD3

 /**
  * @format H1B421
  * @param handle
  * @param addr_type
  * @param address
  */
#define SM_EVENT_PASSKEY_INPUT_NUMBER                            0xD4

 /**
  * @format H1B
  * @param handle
  * @param addr_type
  * @param address
  */
#define SM_EVENT_PASSKEY_INPUT_CANCEL                            0xD5

 /**
  * @format H1B
  * @param handle
  * @param addr_type
  * @param address
  */
#define SM_EVENT_IDENTITY_RESOLVING_STARTED                      0xD6

 /**
  * @format H1B
  * @param handle
  * @param addr_type
  * @param address
  */
#define SM_EVENT_IDENTITY_RESOLVING_FAILED                       0xD7

 /**
  * @format H1B2
  * @param handle
  * @param addr_type
  * @param address
  * @param le_device_db_index
  */
#define SM_EVENT_IDENTITY_RESOLVING_SUCCEEDED                    0xD8

 /**
  * @format H1B
  * @param handle
  * @param addr_type
  * @param address
  */
#define SM_EVENT_AUTHORIZATION_REQUEST                           0xD9

 /**
  * @format H1B1
  * @param handle
  * @param addr_type
  * @param address
  * @param authorization_result
  */
#define SM_EVENT_AUTHORIZATION_RESULT                            0xDA

 /**
  * @format H1B1
  * @param handle
  * @param new private resolvable/non-resolvable address
  */
#define SM_EVENT_PRIVATE_RANDOM_ADDR_UPDATE                      0xDB

 /**
  * @format H1B1
  * @param handle
  * @param state state of the security procedure (`sm_state_t`)
  */
#define SM_EVENT_STATE_CHANGED                                  0xDC

 /**
  * @format H1B1
  * @param handle
  * @param show calculated numeric value that needs to be compared
  */
#define SM_EVENT_NUMERIC_COMPARISON_REQUEST                     0xDD

/**
 * @format PP
 * @param irk
 * @param dhk
 */
#define SM_EVENT_IRK_DHK_RESULT                                    0xDE

// GAP

/**
 * @format H1
 * @param handle
 * @param security_level
 */
#define GAP_EVENT_SECURITY_LEVEL                                 0xE0

/**
 * @format 1B
 * @param status
 * @param address
 */
#define GAP_EVENT_DEDICATED_BONDING_COMPLETED                    0xE1

#endif
