#ifndef __MESH_CONFIGURATION_SERVER_H
#define __MESH_CONFIGURATION_SERVER_H

#include <stdint.h>

#include "mesh/mesh_access.h"

#ifdef __cplusplus
extern "C"
{
#endif

// typedefs

typedef struct  {
    mesh_timer_source_t timer;
    uint8_t timer_active;
    uint16_t active_features;
    uint32_t period_ms;
    uint16_t count;
    //
    uint16_t destination;
    // uint16_t count_log;
    uint8_t  period_log;
    uint8_t  ttl;
    uint16_t features;
    uint16_t netkey_index;
} mesh_heartbeat_publication_t;

typedef struct  {
    // config
    uint16_t source;
    uint16_t destination;
    uint8_t  period_log;
    // data
    uint32_t period_start_ms;
    uint8_t  min_hops;
    uint8_t  max_hops;
    uint16_t count;
} mesh_heartbeat_subscription_t;

typedef struct {
    mesh_heartbeat_publication_t   heartbeat_publication;
    mesh_heartbeat_subscription_t  heartbeat_subscription;

} mesh_configuration_server_model_context_t;

// API
const mesh_operation_t * mesh_configuration_server_get_operations(void);


#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif
