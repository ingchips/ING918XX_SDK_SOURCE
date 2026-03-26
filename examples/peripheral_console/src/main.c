#define OPTIONAL_RF_CLK

#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include <stdio.h>
#include "power_ctrl.h"
#include "key_detector.h"
#include "trace.h"

#include "FreeRTOS.h"
#include "task.h"

#include "main_shared.c"

int app_main()
{
    _app_main();

    return 0;
}

void show_stack_mem(char *buffer)
{
    sprintf(buffer, "stack min free\n"
        "ctrl: %uB\n"
        "host: %uB\n",
        (uint32_t)uxTaskGetStackHighWaterMark((TaskHandle_t)platform_get_task_handle(PLATFORM_TASK_CONTROLLER)) * 4,
        (uint32_t)uxTaskGetStackHighWaterMark((TaskHandle_t)platform_get_task_handle(PLATFORM_TASK_HOST)) * 4);
}

const char welcome_msg[] = "Using Built-in FreeRTOS";
