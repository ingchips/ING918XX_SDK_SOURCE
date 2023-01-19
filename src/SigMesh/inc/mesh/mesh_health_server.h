#ifndef __MESH_HEALTH_SERVER_H
#define __MESH_HEALTH_SERVER_H

#include <stdint.h>

#include "mesh/mesh_access.h"

#ifdef __cplusplus
extern "C"
{
#endif

const mesh_operation_t * mesh_health_server_get_operations(void);

/**
 * @brief Register packet handler
 * @param mesh_model for health server
 * @param events_packet_handler
 */
void mesh_health_server_register_packet_handler(mesh_model_t *mesh_model, btstack_packet_handler_t events_packet_handler);

/**
 * @brief Setup model publication for health state
 * @param mesh_model for health server
 * @param publication_model
 */
void mesh_health_server_set_publication_model(mesh_model_t * mesh_model, mesh_publication_model_t * publication_model);

/**
 * @brief Notify health server that test was perfomed, params are from MESH_SUBEVENT_HEALTH_PERFORM_TEST
 * @param dest
 * @param netkey_index
 * @param appkey_index
 * @param test_id
 * @param company_id
 * @param acknowledged
 */
void mesh_health_server_report_test_done(uint16_t dest, uint16_t netkey_index, uint16_t appkey_index, uint8_t test_id, uint16_t company_id, bool acknowledged);

/**
 * @brief Provide fault state memory
 * @param mesh_model for health server
 * @param company_id
 * @param fault_state
 */
void mesh_health_server_add_fault_state(mesh_model_t *mesh_model, uint16_t company_id, mesh_health_fault_t * fault_state);

/*
 * @brief Set fault
 * @param mesh_model for health server
 * @param company_id
 * @param fault_code
 */
void mesh_health_server_set_fault(mesh_model_t *mesh_model, uint16_t company_id, uint8_t fault_code);

/*
 * @brief Clear fault
 * @param mesh_model for health server
 * @param company_id
 * @param fault_code
 */
void mesh_health_server_clear_fault(mesh_model_t *mesh_model, uint16_t company_id, uint8_t fault_code);


#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif
