#ifndef __MESH_ACCESS_H
#define __MESH_ACCESS_H

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include "mesh_linked_list.h"
#include "mesh/mesh_lower_transport.h"
#include "mesh/mesh_keys.h"
#include "mesh/mesh_node.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define BLUETOOTH_COMPANY_ID_BLUETOOTH_SIG_INC                                           0x003F

#define MESH_SEQUENCE_NUMBER_STORAGE_INTERVAL 1000

typedef enum {
    MESH_DEFAULT_TRANSITION_STEP_RESOLUTION_100ms = 0x00u,
    MESH_DEFAULT_TRANSITION_STEP_RESOLUTION_1s,
    MESH_DEFAULT_TRANSITION_STEP_RESOLUTION_10s,
    MESH_DEFAULT_TRANSITION_STEP_RESOLUTION_10min
} mesh_default_transition_step_resolution_t;

typedef enum {
    MODEL_STATE_UPDATE_REASON_SET = 0x00u, 
    MODEL_STATE_UPDATE_REASON_TRANSITION_START, 
    MODEL_STATE_UPDATE_REASON_TRANSITION_ACTIVE,
    MODEL_STATE_UPDATE_REASON_TRANSITION_END, 
    MODEL_STATE_UPDATE_REASON_TRANSITION_ABORT, 
    // MODEL_STATE_UPDATE_REASON_BOUND_STATE, 
    MODEL_STATE_UPDATE_REASON_APPLICATION_CHANGE
} model_state_update_reason_t;

typedef enum {
    TRANSITION_START,
    TRANSITION_UPDATE
} transition_event_t;

typedef enum {
    MESH_TRANSITION_STATE_IDLE,
    MESH_TRANSITION_STATE_DELAYED,
    MESH_TRANSITION_STATE_ACTIVE
} mesh_transition_state_t;

typedef enum {
    MODEL_STATE_ID_GENERIC_ON_OFF = (BLUETOOTH_COMPANY_ID_BLUETOOTH_SIG_INC << 16) | 0u,
    MODEL_STATE_ID_GENERIC_LEVEL  = (BLUETOOTH_COMPANY_ID_BLUETOOTH_SIG_INC << 16) | 1u,
} model_state_id_t;

#define MESH_MAX_NUM_FAULTS 5

#define MESH_TRANSITION_NUM_STEPS_INFINITE 0x3f

typedef struct {
    // linked list item
    mesh_linked_item_t item;
    uint8_t  test_id;
    uint16_t company_id;
    uint16_t num_current_faults;
    uint16_t num_registered_faults;
    uint8_t  current_faults[MESH_MAX_NUM_FAULTS];
    uint8_t  registered_faults[MESH_MAX_NUM_FAULTS];
} mesh_health_fault_t;

typedef struct {
    // linked list of mesh_health_fault items
    mesh_linked_list_t faults;
    uint8_t fast_period_divisor;
} mesh_health_state_t;

typedef struct {
    uint32_t opcode;
    uint8_t * data;
    uint16_t len;
} mesh_access_parser_state_t;

typedef struct {
    uint32_t     opcode;
    const char * format;
} mesh_access_message_t;

typedef enum {
    MESH_TRANSACTION_STATUS_NEW = 0,
    MESH_TRANSACTION_STATUS_RETRANSMISSION,
    MESH_TRANSACTION_STATUS_DIFFERENT_DST_OR_SRC
} mesh_transaction_status_t;

typedef struct mesh_transition {
    mesh_timer_source_t timer;

    mesh_transition_state_t state;

    uint8_t  transaction_identifier; 
    uint32_t transaction_timestamp_ms;
    uint16_t src_address;
    uint16_t dst_address; 

    uint8_t num_steps;
    mesh_default_transition_step_resolution_t step_resolution;
    uint32_t step_duration_ms;

    // to send events and/or publish changes
    mesh_model_t * mesh_model;
        
    // to execute transition
    void (* transition_callback)(struct mesh_transition * transition, model_state_update_reason_t event);
} mesh_transition_t;


/**
 * @brief Get number of retransmissions used by default
 */
uint8_t mesh_access_acknowledged_message_retransmissions(void);

/**
 * @brief Get retransmission timeout
 */
uint32_t mesh_access_acknowledged_message_timeout_ms(void);


#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif
