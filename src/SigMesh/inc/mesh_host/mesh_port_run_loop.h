
#ifndef MESH_PORT_RUN_LOOP_H
#define MESH_PORT_RUN_LOOP_H

#include "mesh_linked_list.h"
#include "Bluetooth.h"
#include "bt_types.h"
#include <stdint.h>

/**
 * Callback types for run loop data sources
 */
typedef enum {
    MESH_DATA_SOURCE_CALLBACK_POLL  = 1 << 0,
    MESH_DATA_SOURCE_CALLBACK_READ  = 1 << 1,
    MESH_DATA_SOURCE_CALLBACK_WRITE = 1 << 2,
} mesh_data_source_callback_type_t;

typedef struct mesh_data_source {
    //
    mesh_linked_item_t item;
    // file descriptor to watch for run loops that support file descriptors
    int  fd;
    // callback to call for enabled callback types
    void  (*process)(struct mesh_data_source *ds, mesh_data_source_callback_type_t callback_type);
    // flags storing enabled callback types
    uint16_t flags;
} mesh_data_source_t;

typedef void (*mesh_func_timer_process)(void *context);

typedef struct mesh_timer_source {
    mesh_linked_item_t item;
    uint32_t timeout;
    // will be called when timer fired
    mesh_func_timer_process process;
    void * context;
    void * pTimer;
} mesh_timer_source_t;

/***********************************************************************/

void mesh_run_loop_set_timer(mesh_timer_source_t *ts, uint32_t timeout_in_ms);
void mesh_run_loop_add_timer(mesh_timer_source_t *ts);
int mesh_run_loop_remove_timer(mesh_timer_source_t *ts);
void mesh_run_loop_set_timer_handler(mesh_timer_source_t *ts, mesh_func_timer_process process);
void mesh_run_loop_set_timer_context(mesh_timer_source_t *ts, void *context);
void * mesh_run_loop_get_timer_context(mesh_timer_source_t *ts);
uint32_t mesh_run_loop_get_time_ms(void);


#endif
