#ifndef _MESH_MANAGE_CONN_AND_SCAN_H_
#define _MESH_MANAGE_CONN_AND_SCAN_H_

#include <stdint.h>

/**
 * @struct mesh mcas run state.
 */
enum {
    MESH_RUNNING_STATE_IDLE,
    MESH_RUNNING_STATE_TIMER_START_48,
    MESH_RUNNING_STATE_UPDATE_CONN_PARAM_REQ_48,
    MESH_RUNNING_STATE_UPDATE_CONN_PARAM_REQ_200,
    MESH_RUNNING_STATE_UPDATE_CONN_PARAM_OK_48,
    MESH_RUNNING_STATE_UPDATE_CONN_PARAM_OK_200,
};

/**
 * @struct mesh mcas scan state.
 */
enum {
    MESH_SCAN_STATE_IDLE,
    MESH_SCAN_STATE_INTERVAL_20_WINDOW_20,
};

/**
 * @struct mesh mcas control.
 */
typedef struct {
    uint8_t run_state;
    uint8_t scan_state;

} MeshConnScanTypdef_t;


void mesh_mcas_scan_start(void);
void mesh_mcas_gatt_notify_enable_callback(void);
void mesh_mcas_on_off_server_control_callback(void);
void mesh_mcas_conn_params_update_complete_callback(uint8_t status, uint16_t handle, uint16_t interval, uint16_t sup_timeout);
void mesh_mcas_connect_callback(uint16_t handle);
void mesh_mcas_disconnect_callback(uint16_t handle);

#endif
