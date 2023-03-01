#include "mesh_manage_conn_and_scan.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "app_config.h"
#include "mesh.h"
#include "mesh_port_pb.h"
#include "mesh_profile.h"
#include "app_debug.h"
#include "ble_status.h"

/**
 * @brief This file is mainly for managing connection parameters and scanning parameters when they exist at the same time.
 *        In case of established connections, if scanning is turned on when the connection interval is too small, 
 *        the actual time ratio of valid scanning may be low. As a result, the packet loss rate of nodes scanning 
 *        broadcast packets is high, that is, the communication efficiency is low. 
 *        Therefore, in order to maintain a high scan duty ratio of nodes while keeping connections, we have 
 *        to adjust connection parameters and scan parameters, so that the controller has a higher utilization rate 
 *        of radio frequency. This document exists to address this problem.
 * @name mcas : The abbreviation means "manage connection and scan".
 * @details 
 *        1. mesh model control: reuest to update conn_interval to MESH_FINAL_CONN_INTERVAL_MS if current connect interval 
 *                               less then MESH_FINAL_CONN_INTERVAL_CMP_MS. Stop scanning and start a timer to keep scanning 
 *                               alive some time later, use MESH_MCAS_KEEP_ALIVE_WAIT_MS to set timeout time.
 *        2. timer timeout handler: if MeshConnScan.run_state == MESH_RUNNING_STATE_UPDATE_CONN_PARAM_REQ_200, recovery scanning.
 *        3. connection update ok: (1) Depending on the trigger, the treatment is different:
 *                                     * mesh model control :  start scanning.
 *                                     * other reason : don't care.
 *                                 (2) Set local max_ce_len to 5ms.
 *        4. ble connected:(1) mesh provisioned ?
 *                             yes: start scannig;
 *                             no: stop scanning;
 *                         (2) Set local max_ce_len to 5ms.
 *        5. ble disconnect: start scanning.
 *        6. ble ready: start scanning.
 *        7. mesh prov complete: start scanning.
 */

/**
 * @brief application definition.
 */

// scan
#define MESH_FINAL_SCAN_WINDOW_MS       20      // scan window.
#define MESH_FINAL_SCAN_INTERVAL_MS     20      // scan interval.
// conn_interval
#define MESH_FINAL_CONN_INTERVAL_MS     200     // conn interval.
#define MESH_FINAL_CONN_INTERVAL_CMP_MS ((MESH_FINAL_CONN_INTERVAL_MS>10)?(MESH_FINAL_CONN_INTERVAL_MS-10):10) // Compare to this value, and then decide if send connection update request.
// time
#define MESH_MCAS_KEEP_ALIVE_WAIT_MS    4000    //(ms) After starting connect interval request, if update complete event not come, this timer will start scanning when timer timeout.

/**
 * @var control value.
 */
static MeshConnScanTypdef_t MeshConnScan =  {   .run_state  = MESH_RUNNING_STATE_IDLE, \
                                            };
/**
 * @var timer id.
 */
static mesh_timer_source_t       mesh_keep_scan_alive_timer;

// func declaration.
static void mesh_mcas_init(void);

// ------------------------------------------------------------------------------------

static void mesh_mcas_set_slave_local_ce_len(uint16_t handle){
    ll_hint_on_ce_len(handle, 0, 8); // max_ce_len = 5ms
}

/**
 * @brief mesh mcas scan start.
 */
static void mesh_mcas_scan_start(void){
    // mesh_profile_scan_stop();
    mesh_profile_scan_param_set(MESH_FINAL_SCAN_INTERVAL_MS, MESH_FINAL_SCAN_WINDOW_MS);
    mesh_profile_scan_duty_start();
}

/**
 * @brief mesh mcas scan stop.
 * 
 */
static void mesh_mcas_scan_stop(void){
    mesh_profile_scan_stop();
}

/**
 * @brief mesh mcas timer start.
 * 
 * @param time_ms delay timer.
 */
static void mesh_keep_scan_alive_timer_start(uint32_t time_ms, mesh_func_timer_process tmo_handler){
    app_log_debug("[V] %s: tmo=%dms", __func__, time_ms);
    // set timer
    mesh_run_loop_set_timer_handler(&mesh_keep_scan_alive_timer, tmo_handler);
    mesh_run_loop_set_timer(&mesh_keep_scan_alive_timer, time_ms);
    mesh_run_loop_add_timer(&mesh_keep_scan_alive_timer);
    return;
}

/**
 * @brief mesh mcas timer stop.
 * 
 */
static void mesh_keep_scan_alive_timer_stop(void){
    app_log_debug("[V] %s", __func__);
    mesh_run_loop_remove_timer(&mesh_keep_scan_alive_timer);
    return;
}

// ------------------------------------------------------------------------------------

/**
 * @brief ble notify enable callback.
 * 
 */
void mesh_mcas_gatt_notify_enable_callback(void){
    app_log_debug("-- %s .", __func__);
}

/**
 * @brief mesh mcas timer timeout handler.
 * 
 * @param ts timer id.
 */
static void mesh_keep_scan_alive_timer_timeout_handler(mesh_timer_source_t * ts){
    app_log_debug("[V] %s", __func__);
    if (MeshConnScan.run_state == MESH_RUNNING_STATE_UPDATE_CONN_PARAM_REQ_200){
        mesh_mcas_scan_start();
        MeshConnScan.run_state = MESH_RUNNING_STATE_IDLE;
    }
    return;
}

/**
 * @brief mesh mcas conn params update complete callback.
 * 
 * @param status connection update status: 0:success, other:error.
 * @param handle connection handle.
 * @param interval connection interval.
 * @param sup_timeout connection timeout time.
 */
void mesh_mcas_conn_params_update_complete_callback(uint8_t status, uint16_t handle, uint16_t interval, uint16_t sup_timeout){
    app_log_debug("-- %s .", __func__);

    if(status == 0){
        //update success.
        if (MeshConnScan.run_state == MESH_RUNNING_STATE_UPDATE_CONN_PARAM_REQ_200){
            mesh_keep_scan_alive_timer_stop();
            mesh_mcas_scan_start();
            MeshConnScan.run_state = MESH_RUNNING_STATE_IDLE;
        }
    } else {
        // update fail.
        app_log_error("update conn param fail.\n");
        MeshConnScan.run_state = MESH_RUNNING_STATE_IDLE;
    }

    // set max_ce_len
    mesh_mcas_set_slave_local_ce_len(handle);
}

/**
 * @brief mesh on-off server control callback.
 * 
 */
void mesh_mcas_on_off_server_control_callback(void){
    app_log_debug("-- %s .", __func__);

    if(!ble_status_connection_state_get())
        return;
    
    if(MeshConnScan.run_state != MESH_RUNNING_STATE_IDLE){
        app_log_debug("Run busy: %d", MeshConnScan.run_state);
        return;
    }
    
    if(ble_status_connection_interval_get() < MESH_FINAL_CONN_INTERVAL_CMP_MS){
        app_log_debug("update 200ms");
        mesh_mcas_scan_stop();
        //update interval to 200ms
        ble_set_conn_interval_ms(MESH_FINAL_CONN_INTERVAL_MS);
        mesh_keep_scan_alive_timer_start(MESH_MCAS_KEEP_ALIVE_WAIT_MS, (mesh_func_timer_process)&mesh_keep_scan_alive_timer_timeout_handler);
        MeshConnScan.run_state = MESH_RUNNING_STATE_UPDATE_CONN_PARAM_REQ_200;
    }
}

/**
 * @brief mesh stack ready.
 */
void mesh_mcas_stack_ready_callback(void){
    app_log_debug("-- %s .", __func__);
    mesh_profile_scan_addr_set();
    mesh_mcas_scan_start();
    mesh_mcas_init();
}

/**
 * @brief mesh provision complete.
 * 
 */
void mesh_mcas_prov_complete(void){
    app_log_debug("-- %s .", __func__);
    mesh_mcas_scan_start();
}

/**
 * @brief mesh mcas connection established.
 * 
 * @param handle connection handle.
 */
void mesh_mcas_connect_callback(uint16_t handle){
    app_log_debug("-- %s .\n", __func__);
    // set max_ce_len.
    mesh_mcas_set_slave_local_ce_len(handle);
    // set scan.
    if (mesh_is_provisioned()){
        mesh_mcas_scan_start();
    } else {
        mesh_mcas_scan_stop();
    }
    // init.
    mesh_mcas_init();
}

/**
 * @brief mesh mcas disconnection.
 * 
 * @param handle connection handle.
 */
void mesh_mcas_disconnect_callback(uint16_t handle){
    app_log_debug("-- %s .\n", __func__);
    mesh_mcas_scan_start();
    mesh_mcas_init();
}

/**
 * @brief initialize.
 * 
 */
static void mesh_mcas_init(void){
    mesh_keep_scan_alive_timer_stop();
    MeshConnScan.run_state = MESH_RUNNING_STATE_IDLE;
}
