#ifndef __MESH_GENERIC_LEVEL_SERVER_H
#define __MESH_GENERIC_LEVEL_SERVER_H

#include <stdint.h>

#include "mesh/mesh_access.h"

#ifdef __cplusplus
extern "C"
{
#endif

const mesh_operation_t * mesh_generic_level_server_get_operations(void);
/**
 * @brief Register packet handler
 * @param generic_level_server_model
 * @param transition_events_packet_handler
 */
void mesh_generic_level_server_register_packet_handler(mesh_model_t *generic_level_server_model, btstack_packet_handler_t transition_events_packet_handler);

/**
 * @brief Set publication model
 * @param generic_level_server_model
 * @param publication_model
 */
void mesh_generic_level_server_set_publication_model(mesh_model_t *generic_level_server_model, mesh_publication_model_t * publication_model);


#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif
