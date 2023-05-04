#ifndef __MESH_GENERIC_ON_OFF_SERVER_H
#define __MESH_GENERIC_ON_OFF_SERVER_H

#include <stdint.h>

#include "mesh/mesh_access.h"

#ifdef __cplusplus
extern "C"
{
#endif

const mesh_operation_t * mesh_generic_on_off_server_get_operations(void);
/**
 * @brief Register packet handler
 * @param generic_on_off_server_model
 * @param transition_events_packet_handler
 */
void mesh_generic_on_off_server_register_packet_handler(mesh_model_t *generic_on_off_server_model, btstack_packet_handler_t transition_events_packet_handler);

/**
 * @brief Set publication model
 * @param generic_on_off_server_model
 * @param publication_model
 */
void mesh_generic_on_off_server_set_publication_model(mesh_model_t *generic_on_off_server_model, mesh_publication_model_t * publication_model);

/**
 * @brief Set ON/OFF value
 * @param generic_on_off_server_model
 * @param on_off_value
 * @param transition_time_gdtt
 * @param delay_time_gdtt
 */
void mesh_generic_on_off_server_set(mesh_model_t *generic_on_off_server_model, uint8_t on_off_value, uint8_t transition_time_gdtt, uint8_t delay_time_gdtt);

/**
 * @brief  Get present ON/OFF value
 * @param  generic_on_off_server_model
 * @return on_off_value
 */
uint8_t mesh_generic_on_off_server_get(mesh_model_t *generic_on_off_server_model);


#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif
