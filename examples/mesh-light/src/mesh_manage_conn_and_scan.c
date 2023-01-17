#include "mesh_manage_conn_and_scan.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "app_config.h"
#include "mesh.h"
#include "mesh_port_pb.h"
#include "mesh_profile.h"
#include "app_debug.h"

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
 *        1. notify enable: start timer, delay 200ms to request connection_updating to update max_ce_len to 5ms, not changing connection interval.
 *        2. mesh model control: reuest to update conn_interval to 200ms.
 *        3. connection update ok: Depending on the trigger, the treatment is different.
 *                                  (1) notify enable : start scan.
 *                                  (2) mesh model control :  start scan.
 *                                  (3) other reason : don't care.
 *        4. ble connected: stop scan. stop delay timer.
 *        5. ble disconnect: stop delay timer. (can not stop scan, because we constantly need scanning to receive adv data.)
 */

/**
 * @brief application definition.
 */

#define MESH_FINAL_SCAN_INTERVAL_MS     20      // scan interval.
#define MESH_FINAL_CONN_INTERVAL_MS     200     // conn interval.
#define MESH_MCAS_TIMER_DELAY_MS        200     // delay time.

/**
 * @var control value.
 */
static MeshConnScanTypdef_t MeshConnScan =  {   .run_state  = MESH_RUNNING_STATE_IDLE,
                                                .scan_state = MESH_SCAN_STATE_IDLE,
                                            };
/**
 * @var timer id.
 */
static mesh_timer_source_t       mesh_conn_param_update_timer;

/**
 * @brief mesh mcas timer timeout handler.
 * 
 * @param ts timer id.
 */
static void mesh_conn_param_update_timer_timeout_handler(mesh_timer_source_t * ts){
    UNUSED(ts);
    app_log_debug("[V] %s .\n", __func__);
    if(MeshConnScan.run_state == MESH_RUNNING_STATE_TIMER_START_48){
        app_log_debug("update 48ms\n");
        mesh_scan_stop();
        ble_set_conn_interval_ms(ble_get_curr_conn_interval_ms()); //just update max_ce_len = 5ms, do not change conn interval.
        MeshConnScan.run_state = MESH_RUNNING_STATE_UPDATE_CONN_PARAM_REQ_48;
    }
    return;
}

/**
 * @brief mesh mcas timer start.
 * 
 * @param time_ms delay timer.
 */
static void mesh_conn_param_update_timer_start(uint32_t time_ms){
    // set timer
    mesh_run_loop_set_timer_handler(&mesh_conn_param_update_timer, (mesh_func_timer_process)mesh_conn_param_update_timer_timeout_handler);
    mesh_run_loop_set_timer(&mesh_conn_param_update_timer, time_ms);
    mesh_run_loop_add_timer(&mesh_conn_param_update_timer);
    app_log_debug("[V] mesh conn param update timer start: %d ms\n", time_ms);
    return;
}

/**
 * @brief mesh mcas timer stop.
 * 
 */
static void mesh_conn_param_update_timer_stop(void){
    mesh_run_loop_remove_timer(&mesh_conn_param_update_timer);
    return;
}

/**
 * @brief ble notify enable callback.
 * 
 */
void mesh_mcas_gatt_notify_enable_callback(void){
    app_log_debug("-- %s .\n", __func__);

    if(!Is_ble_curr_conn_valid())
        return;
    
    if(MeshConnScan.run_state != MESH_RUNNING_STATE_IDLE){
        app_log_debug("Run busy: %d\n", MeshConnScan.run_state);
        return;
    }

    // start timer , delay the conn param update procedure, which waiting for ble stable.
    // Such as huawei phone, when it just connect, it will auto change conn params some times, 
    // So we need wait for the auto-procedure completing, and after that, we can start our's conn param update request.
    app_log_debug("timer start 48ms\n");
    mesh_conn_param_update_timer_start(MESH_MCAS_TIMER_DELAY_MS);
    MeshConnScan.run_state = MESH_RUNNING_STATE_TIMER_START_48;

    return;
}

/**
 * @brief mesh on-off server control callback.
 * 
 */
void mesh_mcas_on_off_server_control_callback(void){
    app_log_debug("-- %s .\n", __func__);

    if(!Is_ble_curr_conn_valid())
        return;
    
    if(MeshConnScan.run_state != MESH_RUNNING_STATE_IDLE){
        app_log_debug("Run busy: %d\n", MeshConnScan.run_state);
        return;
    }
    
    app_assert(MESH_FINAL_CONN_INTERVAL_MS > 20);
    if(ble_get_curr_conn_interval_ms() < (MESH_FINAL_CONN_INTERVAL_MS - 20)){
        app_log_debug("update 200ms\n");
        mesh_scan_stop();

        //update interval to 200ms , and update max_ce_len = 5ms .
        ble_set_conn_interval_ms(MESH_FINAL_CONN_INTERVAL_MS); 
        MeshConnScan.run_state = MESH_RUNNING_STATE_UPDATE_CONN_PARAM_REQ_200;
    }

    return;
}

/**
 * @brief mesh mcas scan start.
 * @param connect_interval fixed = 20ms 
 */
void mesh_mcas_scan_start(void){
    mesh_scan_stop();
    // scan = 20ms.
    mesh_scan_param_set(MESH_FINAL_SCAN_INTERVAL_MS, MESH_FINAL_SCAN_INTERVAL_MS);
    mesh_scan_start();
    MeshConnScan.scan_state = MESH_SCAN_STATE_INTERVAL_20_WINDOW_20;
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
    app_log_debug("-- %s .\n", __func__);

    if(!status){
        //update success.
        if(MeshConnScan.run_state == MESH_RUNNING_STATE_UPDATE_CONN_PARAM_REQ_48){
            app_log_debug("update 48ms ok (max_ce_len also updated.).\n");
            MeshConnScan.run_state = MESH_RUNNING_STATE_IDLE;

            #if 1
            if(ble_get_curr_conn_interval_ms() >= (MESH_FINAL_SCAN_INTERVAL_MS + 10)){ // check conn intvl , can not too small.
                mesh_mcas_scan_start();
                app_log_debug("scan_window_1: 20ms\n");
            }
            #endif

        } else if (MeshConnScan.run_state == MESH_RUNNING_STATE_UPDATE_CONN_PARAM_REQ_200){
            app_log_debug("update 200ms ok (max_ce_len also updated.).\n");
            MeshConnScan.run_state = MESH_RUNNING_STATE_IDLE;

            #if 1
            if(ble_get_curr_conn_interval_ms() >= (MESH_FINAL_SCAN_INTERVAL_MS + 10)){ // check conn intvl , can not too small.
                mesh_mcas_scan_start();
                app_log_debug("scan_window_2: 20ms\n");
            }
            #endif
        }
    } else {
        // update fail.
        app_log_error("update conn param fail.\n");
        MeshConnScan.run_state = MESH_RUNNING_STATE_IDLE;
    }

    return;
}

/**
 * @brief mesh mcas connection established.
 * 
 * @param handle connection handle.
 */
void mesh_mcas_connect_callback(uint16_t handle){
    app_log_debug("-- %s .\n", __func__);

    mesh_conn_param_update_timer_stop();
    MeshConnScan.run_state = MESH_RUNNING_STATE_IDLE;

    mesh_scan_stop();
    MeshConnScan.scan_state = MESH_SCAN_STATE_IDLE;

    return;
}

/**
 * @brief mesh mcas disconnection.
 * 
 * @param handle connection handle.
 */
void mesh_mcas_disconnect_callback(uint16_t handle){
    app_log_debug("-- %s .\n", __func__);

    mesh_conn_param_update_timer_stop();
    MeshConnScan.run_state = MESH_RUNNING_STATE_IDLE;

    // mesh_scan_stop();
    // MeshConnScan.scan_state = MESH_SCAN_STATE_IDLE;

    return;
}


