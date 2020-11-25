#ifndef _hal_h
#define _hal_h

#include <stdint.h>

#include "FreeRTOS.h"
#include "timers.h"

#include "ble_qiot_import.h"

typedef struct
{
    uint16_t handle;
    uint16_t config;
    ble_on_write_cb on_write;
} char_def_t;

#define Q_CHAR_DEVICE_INFO  0
#define Q_CHAR_DATA         1
#define Q_CHAR_EVENT        2

extern char_def_t qcloud_char_defs[Q_CHAR_EVENT + 1];

typedef struct
{
    TimerHandle_t handle;
    ble_timer_cb  on_timer;
    void         *param;
} stack_timer_t;

#define MAX_TIMER_NUMBER                10
#define USER_MESSAGE_STACK_TIMER        100

void invoke_stack_timer_cb(int index);

#endif
