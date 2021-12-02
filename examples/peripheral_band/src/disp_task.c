#include "disp_task.h"

#include "Display_LCD.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include "peripheral_rtc.h"
#include <stdio.h>

#include "platform_api.h"

#define QUEUE_LENGTH    5
#define ITEM_SIZE       sizeof(char) 
static StaticQueue_t xStaticDispQueue;
static uint8_t ucQueueStorageArea[ITEM_SIZE * QUEUE_LENGTH];
QueueHandle_t xDispQueue;

static const char LOGO[]            = "INGCHIPS BAND";
static const char STATUS_CONN[]     = "CONNECTED";
static const char STATUS_DISCONN[]  = "DISCONNECTED";

static char str_buf[50] = {0};

uint32_t clock_sec_offset = 8 * 3600;

static void format_time()
{
    uint32_t t = RTC_Current();
    uint32_t sec = t >> 15; // t / 32768
    uint32_t m;
    sec += clock_sec_offset;
    m = sec / 60;
    sec -= m * 60;
    uint32_t h = m / 60;
    m -= h * 60;
    h %= 24;
    sprintf(str_buf, "%02d:%02d:%02d", h, m, sec);
}

static void display_task(void *pdata)
{
#ifndef SIMULATION
    OLED_Init();
    OLED_Display(LOGO);
    for (;;)
    {
        char display;
        vTaskDelay(pdMS_TO_TICKS(2000));
        OLED_DispOff();
        LCD_Off();
        if (xQueueReceive(xDispQueue, &display, portMAX_DELAY ) != pdPASS)
            continue;
        OLED_Init();
        switch (display)
        {
        case DISP_LOGO:
            OLED_Display(LOGO);
            break;
        case DISP_TIME:
            format_time();
            OLED_Display(str_buf);
            break;
        case DISP_CONNECTED:
            OLED_Display(STATUS_CONN);
            break;
        case DISP_DISCONNECTED:
            OLED_Display(STATUS_DISCONN);
            break;
        }
    }
#else
    #define Display(s) platform_printf(s); platform_printf("\n");
    
    Display(LOGO);
    for (;;)
    {
        char display;

        if (xQueueReceive(xDispQueue, &display, portMAX_DELAY ) != pdPASS)
            continue;

        switch (display)
        {
        case DISP_LOGO:
            Display(LOGO);
            break;
        case DISP_TIME:
            format_time();
            Display(str_buf);
            break;
        case DISP_CONNECTED:
            Display(STATUS_CONN);
            break;
        case DISP_DISCONNECTED:
            Display(STATUS_DISCONN);
            break;
        }
    }
#endif
}

void disp_init(void)
{
    xDispQueue = xQueueCreateStatic(QUEUE_LENGTH,
                                 ITEM_SIZE,
                                 ucQueueStorageArea,
                                 &xStaticDispQueue);
    xTaskCreate(display_task,
               "d",
               150,
               NULL,
               (configMAX_PRIORITIES - 1),
               NULL);
}

void disp_item(uint8_t item)
{
    BaseType_t xHigherPriorityTaskWoke = pdFALSE;
    if (IS_IN_INTERRUPT())
        xQueueSendFromISR(xDispQueue, &item, &xHigherPriorityTaskWoke);
    else
        xQueueSend(xDispQueue, &item, 0);
}
