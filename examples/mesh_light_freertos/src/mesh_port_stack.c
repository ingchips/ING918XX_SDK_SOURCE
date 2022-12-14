#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "platform_api.h"
#include "gap.h"
#include "bt_types.h"
#include "btstack_event.h"
#include "att_db.h"
#include "sig_uuid.h"
#include "mesh_port_stack.h"
#include "adv_bearer.h"
#include "mesh_btstack_defines.h"
#include "mesh_memory.h"
#include "mesh.h"
#include "mesh_proxy.h"
#include "mesh_port_pb.h"

// profile data.
#include "../data/gatt_pb.const"
#include "../data/gatt_proxy.const"

#define log_info(...)          printf(__VA_ARGS__);printf("\n")
#define log_debug(...)          printf(__VA_ARGS__);printf("\n")


// variable.
//static uint16_t SERVICE_MESH_PROXY_HDL = ATT_HANDLE_INVALID;
//static uint16_t SERVICE_MESH_PROV_HDL = ATT_HANDLE_INVALID;
static uint16_t PROV_DATA_IN_HDL = ATT_HANDLE_INVALID;
static uint16_t PROXY_DATA_IN_HDL = ATT_HANDLE_INVALID;
static uint16_t PROV_DATA_OUT_HDL = ATT_HANDLE_INVALID;
static uint16_t PROXY_DATA_OUT_HDL = ATT_HANDLE_INVALID;
static uint16_t PROV_DATA_OUT_CCCD_HDL = ATT_HANDLE_INVALID;
static uint16_t PROXY_DATA_OUT_CCCD_HDL = ATT_HANDLE_INVALID;

static bool notify_enable = false;

// extern function.
extern void mesh_setup_adv(void);
extern uint32_t mesh_msg_send_to_queue(MeshMsg_t * mesh_msg);
// local function.
void mesh_scan_20_20_start(void);
void mesh_gatt_notify_enable_callback(void);

/******************************************************************************************
 * @brief Receive attribute read requestion.
 * @note This func run at host task.
 */
uint16_t mesh_att_read_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, uint8_t *buffer, uint16_t buffer_size)
{
    if(attribute_handle == PROV_DATA_OUT_CCCD_HDL || attribute_handle == PROXY_DATA_OUT_CCCD_HDL){
        memcpy(buffer, &notify_enable, 2);
        return 2;
    }
    return 0;
}

/******************************************************************************************
 * @brief Receive attribute write requestion.
 * @note This func run at host task.
 */
int mesh_att_write_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, const uint8_t *buffer, uint16_t buffer_size)
{
    mesh_att_write_prov_proxy_callback(con_handle, attribute_handle, buffer, buffer_size);
    
    if(PROV_DATA_OUT_CCCD_HDL == attribute_handle || PROXY_DATA_OUT_CCCD_HDL == attribute_handle ){
        uint16_t notify_en = little_endian_read_16(buffer, 0);
        if(notify_en){
            mesh_gatt_notify_enable_callback();
        }
    }
    
    return 0;
}

/******************************************************************************************
 * @brief Btstack init ok callback.
 * @note This func run at host task.
 */
#include "mesh_setup_profile.h"
void mesh_stack_ready(void)
{
    printf("%s\n", __func__);
    
    mesh_setup_adv();
    // mesh_node_reset();
#if defined(ENABLE_MESH_ADV_BEARER)
    // setup scanning when supporting ADV Bearer
    mesh_setup_scan();
    // mesh_scan_start();
    mesh_scan_20_20_start();
#endif
}

/******************************************************************************************
 * @brief Central connect success
 * @note This func run at host task.
 */
void mesh_connected(uint16_t conn_handle)
{
    printf("%s\n", __func__);
    mesh_ble_connect_callback(conn_handle);
}

/******************************************************************************************
 * @brief Central diconnect success
 * @note This func run at host task.
 */
void mesh_disconnected(uint16_t conn_handle, uint8_t reason)
{
    printf("%s\n", __func__);
    
    // mesh_adv_start();
    mesh_port_proxy_disconnect(conn_handle);
    mesh_ble_disconnect_callback(conn_handle);
}




// -------------------------------------------------------------------------------------------------------
#if 1 
// These is used for managing connection parameters and scanning parameters and non-conn-adv parameters.
// some function that may be used:
// gap_update_connection_parameters
// mesh_scan_param_set
// mesh_scan_start
// mesh_scan_stop
// ble_get_curr_conn_interval_ms
// ble_set_conn_interval_ms
// Is_ble_curr_conn_valid
extern void mesh_scan_param_set(uint16_t interval_ms, uint8_t window_ms);
extern void mesh_scan_start(void);
extern void mesh_scan_stop(void);
extern uint16_t ble_get_curr_conn_interval_ms(void);
extern void ble_set_conn_interval_ms(uint16_t interval_ms);
extern bool Is_ble_curr_conn_valid(void);

// run state.
enum {
    MESH_RUNNING_STATE_IDLE,
    MESH_RUNNING_STATE_TIMER_START_48,
    MESH_RUNNING_STATE_UPDATE_CONN_PARAM_REQ_48,
    MESH_RUNNING_STATE_UPDATE_CONN_PARAM_REQ_200,
    MESH_RUNNING_STATE_UPDATE_CONN_PARAM_OK_48,
    MESH_RUNNING_STATE_UPDATE_CONN_PARAM_OK_200,

};

// scan state.
enum {
    MESH_SCAN_STATE_IDLE,
    MESH_SCAN_STATE_INTERVAL_20_WINDOW_20,

};

// control struct.
typedef struct {
    uint8_t run_state;
    uint8_t scan_state;

} MeshConnScanTypdef_t;

// control value.
static MeshConnScanTypdef_t MeshConnScan =  {   .run_state = MESH_RUNNING_STATE_IDLE,
                                                .scan_state = MESH_SCAN_STATE_IDLE,
                                            };

// timer define.
static mesh_timer_source_t       mesh_conn_param_update_timer;

// timer timeout handler.
static void mesh_conn_param_update_timer_timeout_handler(mesh_timer_source_t * ts){
    UNUSED(ts);
    printf("[V] %s .\n", __func__);
    if(MeshConnScan.run_state == MESH_RUNNING_STATE_TIMER_START_48){
        printf("update 48ms\n");
        mesh_scan_stop();
        ble_set_conn_interval_ms(ble_get_curr_conn_interval_ms()); //just update max_ce_len = 5ms, do not change conn interval.
        MeshConnScan.run_state = MESH_RUNNING_STATE_UPDATE_CONN_PARAM_REQ_48;
    }
    return;
}

// timer start.
void mesh_conn_param_update_timer_start(uint32_t time_ms){
    // set timer
    mesh_run_loop_set_timer_handler(&mesh_conn_param_update_timer, (mesh_func_timer_process)mesh_conn_param_update_timer_timeout_handler);
    mesh_run_loop_set_timer(&mesh_conn_param_update_timer, MESH_CONN_PARAM_UPDATE_TIMER_ID, time_ms);
    mesh_run_loop_add_timer(&mesh_conn_param_update_timer);
    printf("[V] mesh conn param update timer start: %d ms\n", time_ms);
    return;
}

// timer stop.
void mesh_conn_param_update_timer_stop(void){
    mesh_run_loop_remove_timer(&mesh_conn_param_update_timer);
    return;
}

// notify enable callback.
void mesh_gatt_notify_enable_callback(void){
    printf("-- %s .\n", __func__);

    if(!Is_ble_curr_conn_valid())
        return;
    
    if(MeshConnScan.run_state != MESH_RUNNING_STATE_IDLE){
        printf("Run busy: %d\n", MeshConnScan.run_state);
        return;
    }

    // start timer , delay the conn param update procedure, which waiting for ble stable.
    // Such as huawei phone, when it just connect, it will auto change conn params some times, 
    // So we need wait for the auto-procedure completing, and after that, we can start our's conn param update request.
    printf("timer start 48ms\n");
    mesh_conn_param_update_timer_start(200);
    MeshConnScan.run_state = MESH_RUNNING_STATE_TIMER_START_48;

    return;
}

// mesh on-off server control callback.
void mesh_on_off_server_control_callback(void){
    printf("-- %s .\n", __func__);

    if(!Is_ble_curr_conn_valid())
        return;
    
    if(MeshConnScan.run_state != MESH_RUNNING_STATE_IDLE){
        printf("Run busy: %d\n", MeshConnScan.run_state);
        return;
    }
    
    if(ble_get_curr_conn_interval_ms() < 180){
        printf("update 200ms\n");
        mesh_scan_stop();
        ble_set_conn_interval_ms(200); //update interval to 200ms , and update max_ce_len = 5ms .
        MeshConnScan.run_state = MESH_RUNNING_STATE_UPDATE_CONN_PARAM_REQ_200;
    }

    return;
}

void mesh_scan_20_20_start(void){

    // if(!ble_mesh_is_provisioned()) // if enable pb_adv, we must remove this.
    //     return;
    
    mesh_scan_stop();
    mesh_scan_param_set(20, 20);
    mesh_scan_start();
    MeshConnScan.scan_state = MESH_SCAN_STATE_INTERVAL_20_WINDOW_20;
}

// conn params update complete callback.
void mesh_conn_params_update_complete_callback(uint8_t status, uint16_t handle, uint16_t interval, uint16_t sup_timeout){
    printf("-- %s .\n", __func__);

    if(!status){
        //update success.
        if(MeshConnScan.run_state == MESH_RUNNING_STATE_UPDATE_CONN_PARAM_REQ_48){
            printf("update 48ms ok (max_ce_len also updated.).\n");
            MeshConnScan.run_state = MESH_RUNNING_STATE_IDLE;

            #if 1
            if(ble_get_curr_conn_interval_ms() >= 30){
                mesh_scan_20_20_start();
                printf("scan_window_1: 20ms\n");
            }
            #endif

        } else if (MeshConnScan.run_state == MESH_RUNNING_STATE_UPDATE_CONN_PARAM_REQ_200){
            printf("update 200ms ok (max_ce_len also updated.).\n");
            MeshConnScan.run_state = MESH_RUNNING_STATE_IDLE;

            #if 1
            if(ble_get_curr_conn_interval_ms() >= 30){
                mesh_scan_20_20_start();
                printf("scan_window_2: 20ms\n");
            }
            #endif
        }
    } else {
        // update fail.
        printf("update conn param fail.\n");
        MeshConnScan.run_state = MESH_RUNNING_STATE_IDLE;
    }

    return;
}


// ble connect callback.
void mesh_ble_connect_callback(uint16_t handle){
    printf("-- %s .\n", __func__);

    mesh_conn_param_update_timer_stop();
    MeshConnScan.run_state = MESH_RUNNING_STATE_IDLE;

    mesh_scan_stop();
    MeshConnScan.scan_state = MESH_SCAN_STATE_IDLE;

    return;
}

// ble disconnect callback.
void mesh_ble_disconnect_callback(uint16_t handle){
    printf("-- %s .\n", __func__);

    mesh_conn_param_update_timer_stop();
    MeshConnScan.run_state = MESH_RUNNING_STATE_IDLE;

    // mesh_scan_stop();
    // MeshConnScan.scan_state = MESH_SCAN_STATE_IDLE;

    return;
}

#endif
// -------------------------------------------------------------------------------------------------------




/******************************************************************************************
 * @brief gatt can send now callback.
 * @note This func run at host task.
 */
void mesh_gatt_can_send_now(uint16_t conn_handle)
{
    UNUSED(conn_handle);
    // Ble_AttSendBuffer_can_send_now_handler();
    extern void mock_att_service_trigger_can_send_now(void);
    mock_att_service_trigger_can_send_now();
}

static void mesh_node_reset_handler(void){
    mesh_stack_init();//added by lizhk.
    mesh_server_restart(); //added by lizhk.
}

/******************************************************************************************
 * @brief Mesh stack initialization function.
 */
void mesh_stack_init(void)
{
    printf("%s\n", __func__);

//    SERVICE_MESH_PROV_HDL = HANDLE_MESH_PROVISIONING_DATA_IN-2;
    PROV_DATA_IN_HDL = HANDLE_MESH_PROVISIONING_DATA_IN;
    PROV_DATA_OUT_HDL = HANDLE_MESH_PROVISIONING_DATA_OUT;
    PROV_DATA_OUT_CCCD_HDL = HANDLE_MESH_PROVISIONING_DATA_OUT_CLIENT_CHAR_CONFIG;
    
//    SERVICE_MESH_PROXY_HDL = HANDLE_MESH_PROXY_DATA_IN-2;
    PROXY_DATA_IN_HDL = HANDLE_MESH_PROXY_DATA_IN;
    PROXY_DATA_OUT_HDL = HANDLE_MESH_PROXY_DATA_OUT;
    PROXY_DATA_OUT_CCCD_HDL = HANDLE_MESH_PROXY_DATA_OUT_CLIENT_CHAR_CONFIG;

    mesh_att_handle_prov_proxy_init(PROV_DATA_IN_HDL, PROV_DATA_OUT_HDL, PROV_DATA_OUT_CCCD_HDL,
                                    PROXY_DATA_IN_HDL, PROXY_DATA_OUT_HDL, PROXY_DATA_OUT_CCCD_HDL);
    
    mesh_node_reset_handler_register(&mesh_node_reset_handler);
}



