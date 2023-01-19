#ifndef __MESH_NODE_H
#define __MESH_NODE_H

#include <stdint.h>

#include "mesh_btstack_defines.h"
#include "mesh_linked_list.h"
#include "mesh/mesh_network.h"

#if defined __cplusplus
extern "C" {
#endif

#define MESH_APPKEY_INVALID                     0xffffu

#define MAX_NR_MESH_APPKEYS_PER_MODEL           3u
#define MAX_NR_MESH_SUBSCRIPTION_PER_MODEL      3u

#define MESH_HEARTBEAT_PUBLICATION_FEATURE_RELAY      1
#define MESH_HEARTBEAT_PUBLICATION_FEATURE_PROXY      2
#define MESH_HEARTBEAT_PUBLICATION_FEATURE_FRIEND     4
#define MESH_HEARTBEAT_PUBLICATION_FEATURE_LOW_POWER  8

struct mesh_model;
struct mesh_element;

// function to handle model operation message
typedef void (*mesh_operation_handler)(struct mesh_model * mesh_model, mesh_pdu_t * pdu);

// function to publish the current state of a model
// @param mesh_model to publish
// @return mesh_pdu with status message
typedef mesh_pdu_t * (*mesh_publish_state_t)(struct mesh_model * mesh_model);

typedef enum {
    MESH_NODE_IDENTITY_STATE_ADVERTISING_STOPPED = 0,
    MESH_NODE_IDENTITY_STATE_ADVERTISING_RUNNING,
    MESH_NODE_IDENTITY_STATE_ADVERTISING_NOT_SUPPORTED
} mesh_node_identity_state_t;

typedef enum {
    MESH_FRIEND_STATE_DISABLED = 0,
    MESH_FRIEND_STATE_ENABLED,
    MESH_FRIEND_STATE_NOT_SUPPORTED
} mesh_friend_state_t;

typedef enum {
    MESH_MODEL_PUBLICATION_STATE_IDLE,
    MESH_MODEL_PUBLICATION_STATE_W4_PUBLICATION_MS,
    MESH_MODEL_PUBLICATION_STATE_PUBLICATION_READY,
    MESH_MODEL_PUBLICATION_STATE_W4_RETRANSMIT_MS,
    MESH_MODEL_PUBLICATION_STATE_RETRANSMIT_READY,
} mesh_model_publication_state_t;

typedef struct {
    mesh_publish_state_t publish_state_fn;
    btstack_context_callback_registration_t send_request;
    mesh_model_publication_state_t state;
    uint32_t next_publication_ms;
    uint32_t next_retransmit_ms;
    uint8_t  retransmit_count;

    uint16_t address;
    uint16_t appkey_index;
    uint8_t  friendship_credential_flag;
    uint8_t  period;
    uint8_t  period_divisor;  // divide period by 2 ^ period_divisor, default = 2^0 = 1, added for Health Server
    uint8_t  ttl;
    uint8_t  retransmit;
} mesh_publication_model_t;

typedef struct {
    uint16_t destination;
    uint16_t count;      // Number of Heartbeat messages to be sent
    uint16_t period_s;   // Period for sending Heartbeat messages in seconds
    uint16_t features;   // Bit field indicating features that trigger Heartbeat messages when changed
    uint16_t netkey_index; 
    uint8_t  ttl;        // TTL to be used when sending Heartbeat messages
} mesh_heartbeat_publication_state_t;

typedef struct {
    uint32_t opcode;
    uint16_t minimum_length;
    mesh_operation_handler handler;
} mesh_operation_t;

typedef struct mesh_model {
    // linked list item
    mesh_linked_item_t item;

    // element
    struct mesh_element * element;

    // internal model enumeration
    uint16_t mid;

    // vendor_id << 16 | model id, use BLUETOOTH_COMPANY_ID_BLUETOOTH_SIG_INC for SIG models
    uint32_t model_identifier;

    // model operations
    const mesh_operation_t * operations;

    // publication model if supported
    mesh_publication_model_t * publication_model;

    // data
    void * model_data;

    // bound appkeys
    uint16_t appkey_indices[MAX_NR_MESH_APPKEYS_PER_MODEL];

    // subscription list
    uint16_t subscriptions[MAX_NR_MESH_SUBSCRIPTION_PER_MODEL];

    // packet handler for transition events in server, event callback handler in client
    btstack_packet_handler_t model_packet_handler;
    
    //lizhk
    void * user_data;
} mesh_model_t;

typedef struct {
    mesh_linked_list_iterator_t it;
} mesh_model_iterator_t;

typedef struct mesh_element {
    // linked list item
    mesh_linked_item_t item;
    
    // element index
    uint16_t element_index;

    // LOC
    uint16_t loc;
    
    // models
    mesh_linked_list_t models;
    uint16_t models_count_sig;
    uint16_t models_count_vendor;
    
    // models, added by lizhk
    mesh_model_t * const sig_models;
    mesh_model_t * const vnd_models;

} mesh_element_t;

typedef struct {
    mesh_linked_list_iterator_t it;
} mesh_element_iterator_t;

/**
 * @brief Primary element initialization.
 * @param elem
 */
void mesh_primary_elem_init(mesh_element_t * elem);

/**
 * @brief Set location of element
 * @param element
 * @param location
 */
void mesh_node_set_element_location(mesh_element_t * element, uint16_t location);

/**
 * @brief Get unicast address of primary element
 */
uint16_t mesh_node_get_primary_element_address(void);

/**
 * @brief Get Primary Element of this node
 */
mesh_element_t * mesh_node_get_primary_element(void);

/**
 * @brief Add secondary element
 * @param element
 */
void mesh_node_add_element(mesh_element_t * element);

/**
 * @brief Get number elements
 * @return number of elements on this node
 */
uint16_t mesh_node_element_count(void);

/**
 * @brief Get element for given unicast address
 * @param unicast_address
 */
mesh_element_t * mesh_node_element_for_unicast_address(uint16_t unicast_address);

/**
 * @brief Get element by index
 * @param element_index
 */
mesh_element_t * mesh_node_element_for_index(uint16_t element_index);

/**
 * @brief Get element index for give model
 * @param mesh_model
 */
uint8_t mesh_access_get_element_index(mesh_model_t * mesh_model);

/**
 * @brief Get unicast address for give model
 * @param mesh_model
 */
uint16_t mesh_access_get_element_address(mesh_model_t * mesh_model);

/**
 * @brief Add model to element
 * @param element
 * @param mesh_model
 */
void mesh_element_add_model(mesh_element_t * element, mesh_model_t * mesh_model);



// Mesh Model Utility

mesh_model_t * mesh_model_get_by_identifier(mesh_element_t * element, uint32_t model_identifier);

uint32_t mesh_model_get_model_identifier_bluetooth_sig(uint16_t model_id);

int mesh_model_is_bluetooth_sig(uint32_t model_identifier);

uint16_t mesh_model_get_model_id(uint32_t model_identifier);

uint32_t mesh_model_get_model_identifier(uint16_t vendor_id, uint16_t model_id);

uint16_t mesh_model_get_vendor_id(uint32_t model_identifier);

mesh_model_t * mesh_node_get_configuration_server(void);

mesh_model_t * mesh_node_get_health_server(void);

mesh_model_t * mesh_access_model_for_address_and_model_identifier(uint16_t element_address, uint32_t model_identifier, uint8_t * status);

void mesh_model_reset_appkeys(mesh_model_t * mesh_model);

// Mesh Model Subscriptions
int mesh_model_contains_subscription(mesh_model_t * mesh_model, uint16_t address);

/**
 * @brief Set Device UUID
 * @param device_uuid
 */
void mesh_node_set_device_uuid(const uint8_t * device_uuid);

/**
 * @brief Get Device UUID
 * @return device_uuid if set, NULL otherwise
 */
const uint8_t * mesh_node_get_device_uuid(void);

/**
 * @brief Set node info reported in Composition Data Page 0
 * @param company_id (cid)
 * @param product_id (pid)
 * @param product_version_id (vid)
 */
void mesh_node_set_info(uint16_t company_id, uint16_t product_id, uint16_t product_version_id);

/**
 * @brief Get node info: company_id
 * @return company_id
 */
uint16_t mesh_node_get_company_id(void);

/**
 * @brief Get node info: product_id
 * @return product_id
 */
uint16_t mesh_node_get_product_id(void);

/**
 * @brief Get node info: product_version_id
 * @return product_version_id
 */
uint16_t mesh_node_get_product_version_id(void);


#if defined __cplusplus
}
#endif

#endif //__MESH_NODE_H
