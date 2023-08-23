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

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    // make sure that RAM does not exceed 0x20004000
    // then, we can power off the unused blocks
    SYSCTRL_SelectMemoryBlocks(SYSCTRL_RESERVED_MEM_BLOCKS);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
#ifdef DETECT_KEY
    // configure it only once
    GIO_EnableDeepSleepWakeupSource(PIN_WAKEUP, 1, 1, PINCTRL_PULL_DOWN);
#endif
    #ifdef USE_SLOW_CLK_RC
        #if (USE_SLOW_CLK_RC == 8)
            SYSCTRL_EnableSlowRC(1, SYSCTRL_SLOW_RC_8M);
            platform_config(PLATFORM_CFG_LL_DELAY_COMPENSATION, 9000);
        #elif (USE_SLOW_CLK_RC == 16)
            SYSCTRL_EnableSlowRC(1, SYSCTRL_SLOW_RC_16M);
            platform_config(PLATFORM_CFG_LL_DELAY_COMPENSATION, 5000);
        #elif (USE_SLOW_CLK_RC == 24)
            SYSCTRL_EnableSlowRC(1, SYSCTRL_SLOW_RC_24M);
            platform_config(PLATFORM_CFG_LL_DELAY_COMPENSATION, 2500);
        #elif (USE_SLOW_CLK_RC == 32)
            SYSCTRL_EnableSlowRC(1, SYSCTRL_SLOW_RC_32M);
            platform_config(PLATFORM_CFG_LL_DELAY_COMPENSATION, 2000);
        #elif (USE_SLOW_CLK_RC == 48)
            SYSCTRL_EnableSlowRC(1, SYSCTRL_SLOW_RC_48M);
            platform_config(PLATFORM_CFG_LL_DELAY_COMPENSATION, 1200);
        #else
            #error unsupported USE_SLOW_CLK_RC
        #endif
        SYSCTRL_AutoTuneSlowRC();
        SYSCTRL_SelectFlashClk(SYSCTRL_CLK_SLOW);
        SYSCTRL_SelectHClk(SYSCTRL_CLK_SLOW);
        SYSCTRL_EnablePLL(0);
        SYSCTRL_SelectSlowClk(SYSCTRL_SLOW_RC_CLK);
    #else
        #define HCLK_DIV 5

        platform_config(PLATFORM_CFG_DEEP_SLEEP_TIME_REDUCTION, 4000);
        platform_config(PLATFORM_CFG_LL_DELAY_COMPENSATION, 245);

        SYSCTRL_EnableConfigClocksAfterWakeup(1,
            PLL_BOOT_DEF_LOOP,
            HCLK_DIV,
            SYSCTRL_CLK_PLL_DIV_2,
            0);

        SYSCTRL_EnableSlowRC(0, SYSCTRL_SLOW_RC_24M);
        SYSCTRL_SelectHClk(HCLK_DIV);
        SYSCTRL_SelectFlashClk(SYSCTRL_CLK_PLL_DIV_2);
    #endif

    // make sure that RAM does not exceed 0x20004000
    // then, we can power off the unused blocks
    SYSCTRL_SelectMemoryBlocks(SYSCTRL_RESERVED_MEM_BLOCKS);
#endif

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
