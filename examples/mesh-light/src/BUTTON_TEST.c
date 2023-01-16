
/*  ING CHIPS MESH */

/*
 * Copyright (c) 2022 INGCHIPS MESH
 */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "mesh_btstack_defines.h"
#include "BUTTON_TEST.h"
#include "stdbool.h"
#include "mesh_profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include "mesh_storage_app.h"
#include "adv_bearer.h"
#include "mesh_port_run_loop.h"
#include "app_debug.h"


// button gpio select.
#define KB_KEY_1            GIO_GPIO_1 //key 1
#define KB_KEY_2            GIO_GPIO_5 //key 2
#define KB_KEY_3            GIO_GPIO_7 //key 3
#define KB_KEY_4            GIO_GPIO_4 //key 4



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* connection params update test */
#if 1 
#define CPI_VAL_TO_MS(x)    ((uint16_t)(x * 5 / 4))
#define CPI_MS_TO_VAL(x)    ((uint16_t)(x * 4 / 5))
#define CPSTT_VAL_TO_MS(x)  ((uint16_t)(x * 10))
#define CPSTT_MS_TO_VAL(x)  ((uint16_t)(x / 10))

//static void conn_params_print(const le_meta_event_enh_create_conn_complete_t *conn){
//    app_log_debug("my_conn_handle:%d\n", conn->handle);
//    app_log_debug("conn_interval:%dms\n", CPI_VAL_TO_MS(conn->interval));
//    app_log_debug("conn_timeout:%dms\n", CPSTT_VAL_TO_MS(conn->sup_timeout));
//}

// Switch different connection interval params.
static void app_update_conn_params_req(void){
    if(!Is_ble_curr_conn_valid()){
        app_log_error("conn handle err.\n");
        return ;
    }

    static uint8_t flag = 0;
    if(flag == 0){
        app_log_debug("conn interval: %d\n", CPI_VAL_TO_MS(CPI_MS_TO_VAL(200)));
        gap_update_connection_parameters(   ble_get_curr_conn_handle(), CPI_MS_TO_VAL(200), CPI_MS_TO_VAL(200), 
                                            0, CPSTT_MS_TO_VAL(5000), 0x0000, 8);//ce_len = unit 0.625ms.
        flag ++;
    } else if (flag == 1) {
        app_log_debug("conn interval: %d\n", CPI_VAL_TO_MS(CPI_MS_TO_VAL(48)));
        gap_update_connection_parameters(   ble_get_curr_conn_handle(), CPI_MS_TO_VAL(48), CPI_MS_TO_VAL(48), 
                                            0, CPSTT_MS_TO_VAL(5000), 0x0000, 8);//ce_len = unit 0.625ms.
        flag ++;
    } else if (flag == 2) {
        app_log_debug("conn interval: %d\n", CPI_VAL_TO_MS(CPI_MS_TO_VAL(30)));
        gap_update_connection_parameters(   ble_get_curr_conn_handle(), CPI_MS_TO_VAL(30), CPI_MS_TO_VAL(30), 
                                            0, CPSTT_MS_TO_VAL(5000), 0x0000, 8);//ce_len = unit 0.625ms.
        flag = 0;
    }
}
#endif 




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* clear flash mesh information test */
#if 1
// var
static mesh_timer_source_t       mesh_ble_params_reset_delay_timer;

// ext func.
extern void mesh_node_reset(void);
extern void mesh_platform_init(void);

// local func.
static void mesh_ble_params_reset_delay_timer_timeout_handler(mesh_timer_source_t * ts){
    UNUSED(ts);
    app_log_debug("[V] timeout , mesh params reset now !!!!\n");
    
    // mesh network reset.
    mesh_node_reset();    
    // mesh storage clear and reload.
    mesh_storage_app_reinit();
    // mesh platform reinit.
    mesh_platform_init();
}

static void mesh_ble_params_reset_delay_timer_start(uint32_t timeout_in_ms){
    // set timer
    mesh_run_loop_set_timer_handler(&mesh_ble_params_reset_delay_timer, (mesh_func_timer_process)mesh_ble_params_reset_delay_timer_timeout_handler);
    mesh_run_loop_set_timer(&mesh_ble_params_reset_delay_timer, timeout_in_ms);
    mesh_run_loop_add_timer(&mesh_ble_params_reset_delay_timer);
    app_log_debug("[V] mesh ble params reset delay timer start: %d ms\n", timeout_in_ms);
}
#endif



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* scan params test */
#if 1 
typedef struct {
    uint8_t seq;
    uint16_t interval;
    uint16_t window;
} my_scan_stu_t;

const my_scan_stu_t my_scan_param[] = {
    {0, 48, 40}, //seq, interval, window (0.625ms)
    {1, 100, 80},
    {2, 200, 160},
    {3, 10, 8},
    {4, 10, 10},
    {5, 20, 20},
    {6, 20, 18},
};

// key press change scan params.
static void mesh_scan_param_update(void){
    static uint8_t scan_flag = 0;
    uint8_t i = 0;

    mesh_scan_run_set(0);

    for(i=0; i<sizeof(my_scan_param)/sizeof(my_scan_stu_t); i++){
        if(my_scan_param[i].seq == scan_flag){
            app_log_debug("scan param, interval, window: %d,%d . \n", my_scan_param[i].interval, my_scan_param[i].window);
            mesh_scan_param_set(my_scan_param[i].interval, my_scan_param[i].window);//interval, window, ms, 0.625ms
            mesh_scan_run_set(1);
            scan_flag ++;
            break;
        }
    }

    if(i == sizeof(my_scan_param)/sizeof(my_scan_stu_t)){
        app_log_debug("stop scan.\n");
        scan_flag = 0;
    }
}
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* send beacon test */
#if 1 //for test.
static void mesh_send_non_conn_data(void){
    uint8_t data[] = "0123456789abcdef";
    uint16_t dat_len = sizeof(data)-1;
    uint8_t  count   = 3;
    uint16_t intvl   = 100;
    adv_bearer_send_network_pdu(data, dat_len, count, intvl);
}
#endif











///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* button msg dispatch */
static void key_proc_in_host_task(uint8_t num){
    switch(num){
        case 1: // key1 : change connection interval.
            {
                app_log_debug("key1 : change connection interval. \n");
                app_update_conn_params_req();
            }
            break;
        case 2: // key2 : clear all mesh info in flash.
            {
                app_log_debug("key2 : clear all mesh info in flash. \n");
                mesh_ble_params_reset_delay_timer_start(100);
            }
            break;
        case 3: // key3 : change scan params.
            {
                app_log_debug("key3 : change scan params. \n");
                mesh_scan_param_update();
            }
            break;
        case 4: // key4 : send non-conn data.
            {
                app_log_debug("key4 : send non-conn data. \n");
                mesh_send_non_conn_data();
            }
            break;
    }
}

// button msg handler.
void button_msg_handler(btstack_user_msg_t * usrmsg)
{
    uint32_t cmd_id = usrmsg->msg_id;    
    switch(cmd_id){
        case USER_MSG_ID_KEY_PRESSED_EVENT:
            {
                uint8_t key_num = (uint8_t)usrmsg->len;
                key_proc_in_host_task(key_num);
            }
            break;
        case USER_MSG_ID_KEY_RELEASED_EVENT:
            {
                // do nothing.
            }
            break;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* button system init */

static void key_pressed_callback(uint16_t num){
    btstack_push_user_msg(USER_MSG_ID_KEY_PRESSED_EVENT, NULL, (uint16_t)num);
}

static void key_released_callback(uint16_t num){
    btstack_push_user_msg(USER_MSG_ID_KEY_RELEASED_EVENT, NULL, (uint16_t)num);
}

static void kb_state_changed(uint16_t key_state)
{
    static uint8_t key_pressed[4] = {1,2,4,8};
    static uint8_t key_old[4] = {0,0,0,0};
    
    uint16_t i=0;
    for(i=0; i<4; i++){
        if(key_state & key_pressed[i]){
            if(!key_old[i]){
                key_old[i] = 1;
                // key pressed callback.
                key_pressed_callback(i+1);
            }
        } else {
            if(key_old[i]){
                key_old[i] = 0;
                // key released callback.
                key_released_callback(i+1);
            }
        }
    }
}

static uint32_t gpio_isr(void *user_data)
{
    uint32_t current = ~GIO_ReadAll();
    uint16_t v = 0;
    // report which keys are pressed
    if (current & (1 << KB_KEY_1))
        v |= 1;
    if (current & (1 << KB_KEY_2))
        v |= 2;
    if (current & (1 << KB_KEY_3))
        v |= 4;
    if (current & (1 << KB_KEY_4))
        v |= 8;
    kb_state_changed(v);

    GIO_ClearAllIntStatus();
    return 0;
}

static void setup_key_gpio(void)
{    
    SYSCTRL_ClearClkGateMulti(  (1 << SYSCTRL_ClkGate_APB_GPIO0) |
                                (1 << SYSCTRL_ClkGate_APB_GPIO1) |
                                (1 << SYSCTRL_ClkGate_APB_PinCtrl));

    // setup GPIOs for keys
    PINCTRL_SetPadMux(KB_KEY_1, IO_SOURCE_GPIO);
    PINCTRL_SetPadMux(KB_KEY_2, IO_SOURCE_GPIO);
    PINCTRL_SetPadMux(KB_KEY_3, IO_SOURCE_GPIO);
    PINCTRL_SetPadMux(KB_KEY_4, IO_SOURCE_GPIO);
    GIO_SetDirection(KB_KEY_1, GIO_DIR_INPUT);
    GIO_SetDirection(KB_KEY_2, GIO_DIR_INPUT);
    GIO_SetDirection(KB_KEY_3, GIO_DIR_INPUT);
    GIO_SetDirection(KB_KEY_4, GIO_DIR_INPUT);
    GIO_ConfigIntSource(KB_KEY_1, GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE | GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE,
                        GIO_INT_EDGE);
    GIO_ConfigIntSource(KB_KEY_2, GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE | GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE,
                        GIO_INT_EDGE);
    GIO_ConfigIntSource(KB_KEY_3, GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE | GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE,
                        GIO_INT_EDGE);
    GIO_ConfigIntSource(KB_KEY_4, GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE | GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE,
                        GIO_INT_EDGE);
}

// init func. need to be called when system init.
/* API START */
void button_test_init(void){
    platform_set_irq_callback(PLATFORM_CB_IRQ_GPIO, gpio_isr, NULL);
    setup_key_gpio();
}

