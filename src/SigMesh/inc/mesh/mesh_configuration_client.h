#ifndef __MESH_CONFIGURATION_CLIENT_H
#define __MESH_CONFIGURATION_CLIENT_H

#include <stdint.h>

#include "mesh/mesh_access.h"

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct {
    const uint8_t * models;
    uint16_t size;
    uint16_t offset;

    uint32_t id;
} mesh_model_id_iterator_t;

typedef struct {
    const uint8_t * elements;
    uint16_t size;
    uint16_t offset;

    uint16_t loc;

    mesh_model_id_iterator_t sig_model_iterator;
    mesh_model_id_iterator_t vendor_model_iterator;
} mesh_composite_data_iterator_t;

typedef struct {
    uint16_t publish_address_unicast;       
    uint8_t  publish_address_virtual[16];
    uint16_t appkey_index;
    uint8_t  credential_flag;
    uint8_t  publish_ttl;
    uint8_t  publish_period;
    uint8_t  publish_retransmit_count;
    uint8_t  publish_retransmit_interval_steps;
} mesh_publication_model_config_t;

const mesh_operation_t * mesh_configuration_client_get_operations(void);

/**
 * @brief Register packet handler
 * @param configuration_client_model
 * @param events_packet_handler
 */
void mesh_configuration_client_register_packet_handler(mesh_model_t *configuration_client_model, btstack_packet_handler_t events_packet_handler);


#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif
