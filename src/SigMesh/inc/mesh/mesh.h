#ifndef __MESH_H
#define __MESH_H

#include "btstack_defines.h"
#include "mesh/provisioning.h"
#include "mesh/mesh_keys.h"
#include "mesh/mesh_access.h"
#include "mesh/mesh_virtual_addresses.h"

#if defined __cplusplus
extern "C" {
#endif

/**
 * Init Mesh network stack
 */
typedef void (*f_mesh_elem_init_t)(void);
void mesh_stack_init(f_mesh_elem_init_t mesh_elems_init);

/**
 * Register for Mesh Provisioning Device events
 * @param packet_handler
 */
void mesh_register_provisioning_device_packet_handler(btstack_packet_handler_t packet_handler);

// Mesh Virtual Address Management
void mesh_load_virtual_addresses(void);
void mesh_delete_virtual_addresses(void);
void mesh_virtual_address_decrease_refcount(mesh_virtual_address_t * virtual_address);
void mesh_virtual_address_increase_refcount(mesh_virtual_address_t * virtual_address);

// Foundation state
void mesh_foundation_state_load(void);
void mesh_foundation_state_store(void);

// Mesh Model Subscriptions
void mesh_load_subscriptions(void);
void mesh_model_store_subscriptions(mesh_model_t * model);
void mesh_delete_subscriptions(void);

// Mesh Model Publication
void mesh_load_publications(void);
void mesh_delete_publications(void);
void mesh_model_store_publication(mesh_model_t * mesh_model);

// Mesh NetKey List
void mesh_store_network_key(mesh_network_key_t * network_key);
void mesh_delete_network_key(uint16_t internal_index);
void mesh_delete_network_keys(void);
void mesh_load_network_keys(void);

// Mesh Appkeys
void mesh_store_app_key(mesh_transport_key_t * app_key);
void mesh_delete_app_key(uint16_t internal_index);
void mesh_delete_app_keys(void);
void mesh_load_app_keys(void);

// Mesh Model to Appkey List
void mesh_load_appkey_lists(void);
void mesh_delete_appkey_lists(void);
uint8_t mesh_model_bind_appkey(mesh_model_t * mesh_model, uint16_t appkey_index);
void mesh_model_unbind_appkey(mesh_model_t * mesh_model, uint16_t appkey_index);
int mesh_model_contains_appkey(mesh_model_t * mesh_model, uint16_t appkey_index);

// Mesh Node Reset
void mesh_node_reset(void);

// Attention Timer
void    mesh_attention_timer_set(uint8_t timer_s);
uint8_t mesh_attention_timer_get(void);

// provision state
bool mesh_is_provisioned(void);

// advertising control.
void mesh_proxy_start_gatt_advertising(void);
void mesh_proxy_stop_gatt_advertising(void);


#if defined __cplusplus
}
#endif

#endif //__MESH_H
