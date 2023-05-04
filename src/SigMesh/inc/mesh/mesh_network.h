#ifndef __MESH_NETWORK
#define __MESH_NETWORK

#include "mesh/provisioning.h"
#include "mesh/mesh_keys.h"
#include "mesh_port_run_loop.h"

#if defined __cplusplus
extern "C" {
#endif

#define MESH_DEVICE_KEY_INDEX     0xffff

#define MESH_NETWORK_PAYLOAD_MAX      29
#define MESH_ACCESS_PAYLOAD_MAX      384
#define MESH_CONTROL_PAYLOAD_MAX     256

#define MESH_ADDRESS_UNSASSIGNED     0x0000u
#define MESH_ADDRESS_ALL_PROXIES     0xFFFCu
#define MESH_ADDRESS_ALL_FRIENDS     0xFFFDu
#define MESH_ADDRESS_ALL_RELAYS      0xFFFEu
#define MESH_ADDRESS_ALL_NODES       0xFFFFu

typedef enum {
    MESH_NETWORK_PROXY_CONNECTED,
    MESH_NETWORK_PROXY_DISCONNECTED,
    MESH_NETWORK_PDU_RECEIVED,
    MESH_NETWORK_PDU_SENT,
    MESH_NETWORK_PDU_ENCRYPTED,
    MESH_NETWORK_CAN_SEND_NOW,
} mesh_network_callback_type_t;

typedef enum {
    MESH_PDU_TYPE_INVALID,
    MESH_PDU_TYPE_NETWORK,
    MESH_PDU_TYPE_SEGMENT_ACKNOWLEDGMENT,
    MESH_PDU_TYPE_SEGMENTED,
    MESH_PDU_TYPE_UNSEGMENTED,
    MESH_PDU_TYPE_ACCESS,
    MESH_PDU_TYPE_CONTROL,
    MESH_PDU_TYPE_UPPER_SEGMENTED_ACCESS,
    MESH_PDU_TYPE_UPPER_UNSEGMENTED_ACCESS,
    MESH_PDU_TYPE_UPPER_SEGMENTED_CONTROL,
    MESH_PDU_TYPE_UPPER_UNSEGMENTED_CONTROL,
} mesh_pdu_type_t;

typedef struct mesh_pdu {
    // allow for linked lists
    mesh_linked_item_t item;
    // type
    mesh_pdu_type_t pdu_type;

} mesh_pdu_t;

// 
#define MESH_NETWORK_PDU_FLAGS_PROXY_CONFIGURATION 1  // is proxy config pdu ?
#define MESH_NETWORK_PDU_FLAGS_GATT_BEARER         2  // recv from adv?    bit1=0:from adv,   bit1=1:from gatt.
#define MESH_NETWORK_PDU_FLAGS_RELAY               4  // recv from relay?  bit2=0:from local, bit2=1:from relay.

#define MESH_NETWORK_PDU_FLAGS_SEND_VIA_GATT       8  // send via gatt? (proxy config not care this bit.)
#define MESH_NETWORK_PDU_FLAGS_SEND_VIA_ADV        16 // send via adv? (proxy config not care this bit.)

typedef struct mesh_network_pdu {
    mesh_pdu_t pdu_header;

    // meta data network layer
    uint16_t              netkey_index;
    // MESH_NETWORK_PDU_FLAGS
    uint16_t              flags;
    // dst (added by lizhk, for send dst record.)
    uint16_t              dst;

    // pdu
    uint16_t              len;
    uint8_t               data[MESH_NETWORK_PAYLOAD_MAX];
} mesh_network_pdu_t;

#define MESH_TRANSPORT_FLAG_SEQ_RESERVED      1
#define MESH_TRANSPORT_FLAG_CONTROL           2
#define MESH_TRANSPORT_FLAG_TRANSMIC_64       4
#define MESH_TRANSPORT_FLAG_ACK_TIMER         8
#define MESH_TRANSPORT_FLAG_INCOMPLETE_TIMER 16

typedef struct {
    mesh_pdu_t pdu_header;
    // network header
    uint8_t               ivi_nid;
    uint8_t               ctl_ttl;
    uint16_t              src;
    uint16_t              dst;
    uint32_t              seq;

    // incoming: acknowledgement timer / outgoing: segment transmission timer
    mesh_timer_source_t acknowledgement_timer;
    // incoming: incomplete timer / outgoing: not used
    mesh_timer_source_t incomplete_timer;
    // block access
    uint32_t              block_ack;
    // meta data network layer
    uint16_t              netkey_index;
    // akf - aid for access, opcode for control
    uint8_t               akf_aid_control;
    // MESH_TRANSPORT_FLAG
    uint16_t              flags;
    // retry count
    uint8_t               retry_count;
    // pdu segments
    uint16_t              len;
    mesh_linked_list_t segments;
} mesh_segmented_pdu_t;

typedef struct {
    // generic pdu header
    mesh_pdu_t            pdu_header;
    // network header
    uint8_t               ivi_nid;
    uint8_t               ctl_ttl;
    uint16_t              src;
    uint16_t              dst;
    uint32_t              seq;
    // meta data network layer
    uint16_t              netkey_index;
    // meta data transport layer
    uint16_t              appkey_index;
    // akf - aid for access, opcode for control
    uint8_t               akf_aid_control;
    // MESH_TRANSPORT_FLAG
    uint16_t              flags;
    // payload
    uint16_t              len;
    uint8_t               data[MESH_ACCESS_PAYLOAD_MAX];

} mesh_access_pdu_t;

// for unsegmented + segmented access + segmented control pdus
typedef struct {
    // generic pdu header
    mesh_pdu_t            pdu_header;
    // network header
    uint8_t               ivi_nid;
    uint8_t               ctl_ttl;
    uint16_t              src;
    uint16_t              dst;
    uint32_t              seq;
    // meta data network layer
    uint16_t              netkey_index;
    // meta data transport layer
    uint16_t              appkey_index;
    // akf - aid for access, opcode for control
    uint8_t               akf_aid_control;
    // MESH_TRANSPORT_FLAG
    uint16_t              flags;
    // payload, single segmented or list of them
    uint16_t              len;
    mesh_linked_list_t segments;

    // access acknowledged message
    uint16_t retransmit_count;
    uint32_t retransmit_timeout_ms;
    uint32_t ack_opcode;

    // associated lower transport pdu
    mesh_pdu_t *          lower_pdu;
} mesh_upper_transport_pdu_t;

typedef struct {
    // generic pdu header
    mesh_pdu_t            pdu_header;
    // network header
    uint8_t               ivi_nid;
    uint8_t               ctl_ttl;
    uint16_t              src;
    uint16_t              dst;
    uint32_t              seq;
    // meta data network layer
    uint16_t              netkey_index;
    // akf - aid for access, opcode for control
    uint8_t               akf_aid_control;
    // MESH_TRANSPORT_FLAG
    uint16_t              flags;
    // payload
    uint16_t              len;
    uint8_t               data[MESH_CONTROL_PAYLOAD_MAX];
} mesh_control_pdu_t;

typedef enum {
    MESH_KEY_REFRESH_NOT_ACTIVE = 0,
    MESH_KEY_REFRESH_FIRST_PHASE,
    MESH_KEY_REFRESH_SECOND_PHASE
} mesh_key_refresh_state_t;

typedef enum {
    MESH_SECURE_NETWORK_BEACON_W2_AUTH_VALUE,
    MESH_SECURE_NETWORK_BEACON_W4_AUTH_VALUE,
    MESH_SECURE_NETWORK_BEACON_AUTH_VALUE,
    MESH_SECURE_NETWORK_BEACON_W2_SEND_ADV,
    MESH_SECURE_NETWORK_BEACON_ADV_SENT,
    MESH_SECURE_NETWORK_BEACON_W2_SEND_GATT,
    MESH_SECURE_NETWORK_BEACON_GATT_SENT,
    MESH_SECURE_NETWORK_BEACON_W4_INTERVAL
} mesh_secure_network_beacon_state_t;

typedef struct {
    mesh_linked_item_t item;

    // netkey index
    uint16_t              netkey_index;

    // current / old key
    mesh_network_key_t * old_key;

    // new key (only set during key refresh)
    mesh_network_key_t * new_key;

    // key refresh state
    mesh_key_refresh_state_t key_refresh;

    // advertisement using node id active
    uint8_t node_id_advertisement_running;


    // advertisement using network id (used by proxy)
    adv_bearer_connectable_advertisement_data_item_t advertisement_with_network_id;

    // advertising using node id (used by proxy)
    adv_bearer_connectable_advertisement_data_item_t advertisement_with_node_id;

    // secure network beacons
    mesh_secure_network_beacon_state_t beacon_state;
    uint32_t                           beacon_interval_ms;
    uint32_t                           beacon_observation_start_ms;
    uint16_t                           beacon_observation_counter;

} mesh_subnet_t;

typedef struct {
    mesh_linked_list_iterator_t it;
} mesh_subnet_iterator_t;


/**
 * @brief Validate network addresses
 * @param ctl
 * @param src
 * @param dst
 * @return 1 if valid, 
 */
int mesh_network_addresses_valid(uint8_t ctl, uint16_t src, uint16_t dst);

/**
 * @brief Check if Unicast address
 * @param addr
 * @return 1 if unicast
 */
int mesh_network_address_unicast(uint16_t addr);

/**
 * @brief Check if Group address
 * @param addr
 * @return 1 if unicast
 */
int mesh_network_address_group(uint16_t addr);

/**
 * @brief Check if All Proxies address
 * @param addr
 * @return 1 if all proxies
 */
int mesh_network_address_all_proxies(uint16_t addr);

/**
 * @brief Check if All Nodes address
 * @param addr
 * @return 1 if all nodes
 */
int mesh_network_address_all_nodes(uint16_t addr);

/**
 * @brief Check if All Friends address
 * @param addr
 * @return 1 if all friends
 */
int mesh_network_address_all_friends(uint16_t addr);

/**
 * @brief Check if All Relays address
 * @param addr
 * @return 1 if all relays
 */
int mesh_network_address_all_relays(uint16_t addr);

/**
 * @brief Check if Virtual address
 * @param addr
 * @return 1 if virtual
 */
int mesh_network_address_virtual(uint16_t addr);

/**
 * @brief Get subnet for netkey_index
 * @param netkey_index
 * @return mesh_subnet_t or NULL
 */
mesh_subnet_t * mesh_subnet_get_by_netkey_index(uint16_t netkey_index);

/**
 * @brief Get number of stored subnets
 * @return count
 */
int mesh_subnet_list_count(void);

/**
 * @brief remove all subnet.
 */
void mesh_subnet_delete_all_subnet(void); 


#if defined __cplusplus
}
#endif

#endif
