
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
#include "platform_api.h"
#include "mesh_storage_app.h"
#include "mesh_port_run_loop.h"
#include "app_debug.h"

// Test enable.
#define APP_MESH_FLASH_PARAM_CLEAR_TEST_EN


// Button gpio select.
#ifndef KB_KEY_RESET_PARAMS
#define KB_KEY_RESET_PARAMS GIO_GPIO_5
#endif
#ifndef KEY_PULL
#define KEY_PULL PINCTRL_PULL_DISABLE
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* clear flash mesh information test */
#ifdef APP_MESH_FLASH_PARAM_CLEAR_TEST_EN
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
//    // mesh storage clear and reload.
//    mesh_storage_app_reinit();
//    // mesh platform reinit.
//    mesh_platform_init();
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
/* Button msg dispatch */
static void key_proc_in_host_task(uint8_t num) {
    switch (num) {
        case 2: // key2 : clear all mesh info in flash.
            {
#ifdef APP_MESH_FLASH_PARAM_CLEAR_TEST_EN
                app_log_debug("key %d : clear all mesh info in flash.\n", num);
                mesh_ble_params_reset_delay_timer_start(100);
#endif
            }
            break;
        default:
            {
                app_log_debug("key %d : reserved.\n", num);
            }
            break;
    }
}

// Button msg handler.
void button_msg_handler(btstack_user_msg_t * usrmsg){
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
/* Button driver init */

static void key_pressed_callback(uint16_t num) {
    btstack_push_user_msg(USER_MSG_ID_KEY_PRESSED_EVENT, NULL, (uint16_t)num);
}

static void key_released_callback(uint16_t num) {
    btstack_push_user_msg(USER_MSG_ID_KEY_RELEASED_EVENT, NULL, (uint16_t)num);
}

static void kb_state_changed(uint16_t key_state) {
    static uint8_t key_pressed[4] = {1,2,4,8};
    static uint8_t key_old[4] = {0,0,0,0};

    uint16_t i=0;
    for(i = 0; i < 4; i++){
        if (key_state & key_pressed[i]) {
            if (!key_old[i]) {
                key_old[i] = 1;
                // key pressed callback.
                key_pressed_callback(i+1);
            }
        } else {
            if (key_old[i]) {
                key_old[i] = 0;
                // key released callback.
                key_released_callback(i+1);
            }
        }
    }
}

static uint32_t gpio_isr(void *user_data){
    uint32_t current = ~GIO_ReadAll();
    uint16_t v = 0;
    // report which keys are pressed
    if (current & (1 << KB_KEY_RESET_PARAMS))
        v |= 2;
    kb_state_changed(v);

    GIO_ClearAllIntStatus();
    return 0;
}

static void setup_key_gpio(void){
    SYSCTRL_ClearClkGateMulti(  (1 << SYSCTRL_ClkGate_APB_GPIO0) |
                                (1 << SYSCTRL_ClkGate_APB_GPIO1) |
                                (1 << SYSCTRL_ClkGate_APB_PinCtrl));

    // setup GPIOs for keys
    PINCTRL_SetPadMux(KB_KEY_RESET_PARAMS, IO_SOURCE_GPIO);
    GIO_SetDirection(KB_KEY_RESET_PARAMS, GIO_DIR_INPUT);
    PINCTRL_Pull(KB_KEY_RESET_PARAMS, KEY_PULL);
    GIO_ConfigIntSource(KB_KEY_RESET_PARAMS, GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE | GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE,
                        GIO_INT_EDGE);
}

// init func. need to be called when system init.
/* API START */
void button_test_init(void){
    platform_set_irq_callback(PLATFORM_CB_IRQ_GPIO, gpio_isr, NULL);
    setup_key_gpio();
}

