
#include <stdio.h>
#include "platform_api.h"
#include "key_detector.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

static SemaphoreHandle_t sem_wakeup = NULL;

static f_key_event_cb key_event_cb;
static int last_value = 0;
static int pressed_evt_cnt = 0;
static int durtion = 0;

static int sampling_timer_cb(void)
{
    int value = GIO_ReadValue(KEY_PIN);
    if (value == last_value)
    {
        durtion++;
        if ((durtion > 20) && (value != VALUE_PRESSED))
        {
            if (pressed_evt_cnt > 0)
                key_event_cb((key_press_event_t)pressed_evt_cnt);
            return 0;
        }
        return 1;
    }
    
    if (value != VALUE_PRESSED)
    {
        if (durtion > 40)
        {
            key_event_cb(KEY_LONG_PRESSED);
            pressed_evt_cnt = 0;
            durtion = 0;
        }
        else
            pressed_evt_cnt++;
    }
    else
        durtion = 0;

    last_value = value;
    return 1;
}

static void timer_wakeup_task(void *pdata)
{
    for (;;)
    {
        BaseType_t r = xSemaphoreTake(sem_wakeup,  portMAX_DELAY);
        if (pdTRUE != r) continue;

        if (GIO_ReadValue(KEY_PIN) != VALUE_PRESSED) continue;

        last_value = VALUE_PRESSED;
        pressed_evt_cnt = 0;
        durtion = 0;

        while (sampling_timer_cb())
        {
            vTaskDelay(pdMS_TO_TICKS(30));
        }
    }
}

void key_detect_init(f_key_event_cb cb)
{
    key_event_cb = cb;
    sem_wakeup = xSemaphoreCreateBinary();

    xTaskCreate(timer_wakeup_task,
               "b",
               150,
               NULL,
               (configMAX_PRIORITIES - 1),
               NULL);
}

void key_detector_start_on_demand(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (GIO_ReadValue(KEY_PIN) != VALUE_PRESSED) return;
    xSemaphoreGiveFromISR(sem_wakeup, &xHigherPriorityTaskWoken);
}

void test_key_detector(void)
{
    platform_printf("wait\n");
    while (GIO_ReadValue(KEY_PIN) != VALUE_PRESSED) ; 
    key_detector_start_on_demand();
}
