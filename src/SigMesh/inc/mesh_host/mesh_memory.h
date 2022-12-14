#ifndef MESH_MEMORY_H
#define MESH_MEMORY_H

#include "mesh_network.h"
#include "mesh_keys.h"
#include "mesh_virtual_addresses.h"


void *mesh_memory_alloc(uint16_t size);
void mesh_memory_free(void *data);


/**
 * @brief Initializes BTstack memory pools.
 */
void mesh_memory_init(void);

// #ifdef ENABLE_MESH
mesh_network_pdu_t * mesh_memory_mesh_network_pdu_get(void);
void   mesh_memory_mesh_network_pdu_free(mesh_network_pdu_t *mesh_network_pdu);
mesh_segmented_pdu_t * mesh_memory_mesh_segmented_pdu_get(void);
void   mesh_memory_mesh_segmented_pdu_free(mesh_segmented_pdu_t *mesh_segmented_pdu);
mesh_upper_transport_pdu_t * mesh_memory_mesh_upper_transport_pdu_get(void);
void   mesh_memory_mesh_upper_transport_pdu_free(mesh_upper_transport_pdu_t *mesh_upper_transport_pdu);
mesh_network_key_t * mesh_memory_mesh_network_key_get(void);
void   mesh_memory_mesh_network_key_free(mesh_network_key_t *mesh_network_key);
mesh_transport_key_t * mesh_memory_mesh_transport_key_get(void);
void   mesh_memory_mesh_transport_key_free(mesh_transport_key_t *mesh_transport_key);
mesh_virtual_address_t * mesh_memory_mesh_virtual_address_get(void);
void   mesh_memory_mesh_virtual_address_free(mesh_virtual_address_t *mesh_virtual_address);
mesh_subnet_t * mesh_memory_mesh_subnet_get(void);
void   mesh_memory_mesh_subnet_free(mesh_subnet_t *mesh_subnet);
// #endif

#endif



