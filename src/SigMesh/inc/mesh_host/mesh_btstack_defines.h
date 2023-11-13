/**
 *
 * BTstack definitions, events, and error codes 
 *
 */

#ifndef MESH_BTSTACK_DEFINES_H
#define MESH_BTSTACK_DEFINES_H
#include <stdint.h>

#include "mesh_linked_list.h" 

// UNUSED macro
#ifndef UNUSED
#define UNUSED(x) (void)(sizeof(x))
#endif

// context callback supporting multiple registrations
typedef struct {
  mesh_linked_item_t * item;
  void (*callback)(void * context);
  void * context;
} btstack_context_callback_registration_t;


// Mesh Provisioning PDU
#define PROVISIONING_DATA_PACKET 0x11u

// Mesh Proxy PDU
#define MESH_PROXY_DATA_PACKET   0x11u

// Mesh Network PDU
#define MESH_NETWORK_PACKET      0x12u

// Mesh Network PDU
#define MESH_BEACON_PACKET       0x13u

#define HCI_EVENT_MESH_META                                      0xF5u

/**
 * @format 1
 * @param subevent_code
 */
#define MESH_SUBEVENT_CAN_SEND_NOW                                                   0x01u

/**
 * @format 11
 * @param subevent_code
 * @param status
 */
#define MESH_SUBEVENT_PB_TRANSPORT_PDU_SENT                                          0x02u

/**
 * @format 1121
 * @param subevent_code
 * @param status
 * @param pb_transport_cid
 * @param pb_type
 */
#define MESH_SUBEVENT_PB_TRANSPORT_LINK_OPEN                                         0x03u

/**
 * @format 112
 * @param subevent_code
 * @param pb_transport_cid
 * @param reason
 */
#define MESH_SUBEVENT_PB_TRANSPORT_LINK_CLOSED                                       0x04u

/**
 * @format 121
 * @param subevent_code
 * @param pb_transport_cid
 * @param attention_time in seconds
 */
#define MESH_SUBEVENT_PB_PROV_ATTENTION_TIMER                                        0x10u

/**
 * Device Role
 * @format 12
 * @param subevent_code
 * @param pb_transport_cid
 */
#define MESH_SUBEVENT_PB_PROV_START_EMIT_PUBLIC_KEY_OOB                              0x11u

/**
 * Device Role
 * @format 12
 * @param subevent_code
 * @param pb_transport_cid
 */
#define MESH_SUBEVENT_PB_PROV_STOP_EMIT_PUBLIC_KEY_OOB                               0x12u

/**
 * Device Role
 * @format 12
 * @param subevent_code
 * @param pb_transport_cid
 */
#define MESH_SUBEVENT_PB_PROV_INPUT_OOB_REQUEST                                      0x13u

/**
 * Device Role
 * @format 124
 * @param subevent_code
 * @param pb_transport_cid
 * @param output_oob number
 */
#define MESH_SUBEVENT_PB_PROV_START_EMIT_OUTPUT_OOB                                  0x15u

/**
 * Device Role
 * @format 12
 * @param subevent_code
 * @param pb_transport_cid
 */
#define MESH_SUBEVENT_PB_PROV_STOP_EMIT_OUTPUT_OOB                                   0x16u

/**
 * Provisioner Role
 * @format 12
 * @param subevent_code
 * @param pb_transport_cid
 */
#define MESH_SUBEVENT_PB_PROV_START_RECEIVE_PUBLIC_KEY_OOB                           0x17u

/**
 * Provisioner Role
 * @format 12
 * @param subevent_code
 * @param pb_transport_cid
 */
#define MESH_SUBEVENT_PB_PROV_STOP_RECEIVE_PUBLIC_KEY_OOB                            0x18u

/**
 * Provisioner Role
 * @format 12
 * @param subevent_code
 * @param pb_transport_cid
 */
#define MESH_SUBEVENT_PB_PROV_OUTPUT_OOB_REQUEST                                     0x19u

/**
 * Provisioner Role
 * @format 124
 * @param subevent_code
 * @param pb_transport_cid
 * @param output_oob number
 */
#define MESH_SUBEVENT_PB_PROV_START_EMIT_INPUT_OOB                                   0x1au

/**
 * Provisioner Role
 * @format 12
 * @param subevent_code
 * @param pb_transport_cid
 */
#define MESH_SUBEVENT_PB_PROV_STOP_EMIT_INPUT_OOB                                    0x1bu

/**
 * Provisioner Role
 * @format 1212111212
 * @param subevent_code
 * @param pb_transport_cid
 * @param num_elements
 * @param algorithms
 * @param public_key
 * @param static_oob_type
 * @param output_oob_size
 * @param output_oob_action
 * @param input_oob_size
 * @param input_oob_action
 */
#define MESH_SUBEVENT_PB_PROV_CAPABILITIES                                           0x1cu

/**
 * @format 12
 * @param subevent_code
 * @param pb_transport_cid
 */
#define MESH_SUBEVENT_PB_PROV_COMPLETE                                               0x1du

/**
 * @format 11
 * @param subevent_code
 * @param attention_time in seconds
 */
#define MESH_SUBEVENT_ATTENTION_TIMER                                                0x1eu

/**
 * @format 
 * @param subevent_code
 * @param attention_time in seconds
 */
#define MESH_SUBEVENT_NODE_RESET                                                     0x1fu

/**
 * @format 1H
 * @param subevent_code
 * @param con_handle
 */
#define MESH_SUBEVENT_PROXY_CONNECTED                                                0x20u

/**
 * @format 1H
 * @param subevent_code
 * @param con_handle
 */
#define MESH_SUBEVENT_PROXY_PDU_SENT                                                 0x21u

/**
 * @format 1H
 * @param subevent_code
 * @param con_handle
 */
#define MESH_SUBEVENT_PROXY_DISCONNECTED                                             0x22u

/**
 * @format 1H
 * @param subevent_code
 * @param con_handle
 */
#define MESH_SUBEVENT_MESSAGE_SENT                                                   0x23u

/**
 * @format 114411
 * @param subevent_code
 * @param element_index
 * @param model_identifier
 * @param state_identifier
 * @param reason
 * @param value
 */
#define MESH_SUBEVENT_STATE_UPDATE_BOOL                                              0x24u

/**
 * @format 114412
 * @param subevent_code
 * @param element_index
 * @param model_identifier
 * @param state_identifier
 * @param reason
 * @param value
 */
#define MESH_SUBEVENT_STATE_UPDATE_INT16                                              0x25u

// Mesh Client Events
/**
 * @format 11442
 * @param subevent_code
 * @param element_index
 * @param model_identifier
 * @param opcode
 * @param dest
 */
#define MESH_SUBEVENT_MESSAGE_NOT_ACKNOWLEDGED                                        0x30u

/**
 * @format 121114
 * @param subevent_code
 * @param dest
 * @param status
 * @param present_value
 * @param target_value       optional, if value > 0, than remaining_time_ms must be read
 * @param remaining_time_ms  
 */
#define MESH_SUBEVENT_GENERIC_ON_OFF                                                  0x31u

/**
 * @format 121224
 * @param subevent_code
 * @param dest
 * @param status
 * @param present_value
 * @param target_value       optional, if value > 0, than remaining_time_ms must be read
 * @param remaining_time_ms  
 */
#define MESH_SUBEVENT_GENERIC_LEVEL                                                   0x32u

/**
 * @format 1222211
 * @param subevent_code
 * @param dest
 * @param netkey_index
 * @param appkey_index
 * @param company_id
 * @param test_id
 * @param acknowledged
 */
#define MESH_SUBEVENT_HEALTH_PERFORM_TEST                                            0x33u

/**
 * @format 11
 * @param subevent_code
 * @param element_index
 */
#define MESH_SUBEVENT_HEALTH_ATTENTION_TIMER_CHANGED                                 0x34u

/**
 * @format 1211
 * @param subevent_code
 * @param dest
 * @param status
 * @param transition_time_gdtt  
 */
#define MESH_SUBEVENT_GENERIC_DEFAULT_TRANSITION_TIME                                0x35u

/**
 * @format 1211
 * @param subevent_code
 * @param dest
 * @param foundation_status
 * @param secure_network_beacon_state  
 */
#define MESH_SUBEVENT_CONFIGURATION_BEACON                                           0x36u

// Composition Data has variable of element descriptions, with two lists of model lists
// Use .. getters to access data
#define MESH_SUBEVENT_CONFIGURATION_COMPOSITION_DATA                                 0x37u

/**
 * @format 1211
 * @param subevent_code
 * @param dest
 * @param foundation_status
 * @param default_ttl  
 */
#define MESH_SUBEVENT_CONFIGURATION_DEFAULT_TTL                                      0x38u

/**
 * @format 1211
 * @param subevent_code
 * @param dest
 * @param foundation_status
 * @param gatt_proxy_state  
 */
#define MESH_SUBEVENT_CONFIGURATION_GATT_PROXY                                       0x39u

/**
 * @format 121111
 * @param subevent_code
 * @param dest
 * @param foundation_status
 * @param relay
 * @param retransmit_count                          the number of times that packet is transmitted for each packet that is relayed.
 * @param retransmit_interval_ms                    retransmission interval in ms
 */
#define MESH_SUBEVENT_CONFIGURATION_RELAY                                            0x40u


/**
 * @format 12122111114
 * @param subevent_code
 * @param dest                                      element_address
 * @param foundation_status
 * @param publish_address
 * @param appkey_index
 * @param credential_flag
 * @param publish_ttl
 * @param publish_period
 * @param publish_retransmit_count
 * @param publish_retransmit_interval_steps
 * @param model_identifier
 */
#define MESH_SUBEVENT_CONFIGURATION_MODEL_PUBLICATION                                0x41u

/**
 * @format 12124
 * @param subevent_code
 * @param dest
 * @param foundation_status
 * @param address  
 * @param model_identifier
 */
#define MESH_SUBEVENT_CONFIGURATION_MODEL_SUBSCRIPTION                               0x42u

/**
 * @format 1214112
 * @param subevent_code
 * @param dest
 * @param foundation_status
 * @param model_identifier
 * @param num_subscription_addresses
 * @param subscription_address_pos
 * @param subscription_address_item
 */
#define MESH_SUBEVENT_CONFIGURATION_MODEL_SUBSCRIPTION_LIST_ITEM                     0x43u   


/**
 * @format 121
 * @param subevent_code
 * @param dest
 * @param foundation_status
 */
#define MESH_SUBEVENT_CONFIGURATION_NETKEY_INDEX                                      0x44u

/**
 * @format 121112
 * @param subevent_code
 * @param dest
 * @param foundation_status
 * @param num_netkey_indexes
 * @param netkey_index_pos
 * @param netkey_index_item
 */
#define MESH_SUBEVENT_CONFIGURATION_NETKEY_INDEX_LIST_ITEM                             0x45u

/**
 * @format 12122
 * @param subevent_code
 * @param dest
 * @param foundation_status
 * @param netkey_index_item
 * @param appkey_index_item
 */
#define MESH_SUBEVENT_CONFIGURATION_APPKEY_INDEX                                       0x46u

/**
 * @format 12121122
 * @param subevent_code
 * @param dest
 * @param foundation_status
 * @param netkey_index
 * @param num_appkey_indexes
 * @param appkey_index_pos
 * @param netkey_index_item
 * @param appkey_index_item
 */
#define MESH_SUBEVENT_CONFIGURATION_APPKEY_INDEX_LIST_ITEM                            0x47u

/**
 * @format 12121
 * @param subevent_code
 * @param dest
 * @param foundation_status
 * @param netkey_index_item
 * @param identity_status
 */
#define MESH_SUBEVENT_CONFIGURATION_NODE_IDENTITY                                      0x48u

/**
 * @format 12124
 * @param subevent_code
 * @param dest
 * @param foundation_status
 * @param appkey_index
 * @param model_identifier
 */
#define MESH_SUBEVENT_CONFIGURATION_MODEL_APP                                           0x49u

/**
 * @format 1214112
 * @param subevent_code
 * @param dest
 * @param foundation_status
 * @param model_id
 * @param num_appkey_indexes
 * @param appkey_index_pos
 * @param appkey_index_item
 */
#define MESH_SUBEVENT_CONFIGURATION_MODEL_APP_LIST_ITEM                                 0x50u

/**
 * @format 121
 * @param subevent_code
 * @param dest
 * @param foundation_status
 */
#define MESH_SUBEVENT_CONFIGURATION_NODE_RESET                                          0x51u

/**
 * @format 1211
 * @param subevent_code
 * @param dest
 * @param foundation_status
 * @param friend_state
 */
#define MESH_SUBEVENT_CONFIGURATION_FRIEND                                              0x52u

/**
 * @format 12121
 * @param subevent_code
 * @param dest
 * @param foundation_status
 * @param netkey_index
 * @param phase
 */
#define MESH_SUBEVENT_CONFIGURATION_KEY_REFRESH_PHASE                                   0x53u

/**
 * @format 121222122
 * @param subevent_code
 * @param dest
 * @param foundation_status
 * @param heartbeat_destination
 * @param count_S
 * @param period_S
 * @param ttl
 * @param features
 * @param netkey_index
 */
#define MESH_SUBEVENT_CONFIGURATION_HEARTBEAT_PUBLICATION                               0x54u

/**
 * @format 121222211
 * @param subevent_code
 * @param dest
 * @param foundation_status
 * @param heartbeat_destination
 * @param heartbeat_source
 * @param count_S
 * @param period_S
 * @param min_hops
 * @param max_hops
 */
#define MESH_SUBEVENT_CONFIGURATION_HEARTBEAT_SUBSCRIPTION                              0x55u

/**
 * @format 12123
 * @param subevent_code
 * @param dest
 * @param foundation_status
 * @param lpn_address
 * @param poll_timeout
 */
#define MESH_SUBEVENT_CONFIGURATION_LOW_POWER_NODE_POLL_TIMEOUT                         0x56u

/**
 * @format 12112
 * @param subevent_code
 * @param dest
 * @param foundation_status
 * @param transmit_count
 * @param transmit_interval_steps_ms
 */
#define MESH_SUBEVENT_CONFIGURATION_NETWORK_TRANSMIT                                    0x57u



#endif
