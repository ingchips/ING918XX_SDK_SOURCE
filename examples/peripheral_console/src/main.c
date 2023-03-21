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

#include "main_shared.inc"

// To calibration Tx power preciously, we can use a "fake" power mapping table,
// then measure Tx power using testers.
const int16_t power_mapping[] = {
    0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200,
    1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2100, 2200, 2300,
    2400, 2500, 2600, 2700, 2800, 2900, 3000, 3100, 3200, 3300, 3400,
    3500, 3600, 3700, 3800, 3900, 4000, 4100, 4200, 4300, 4400, 4500,
    4600, 4700, 4800, 4900, 5000, 5100, 5200, 5300, 5400, 5500, 5600,
    5700, 5800, 5900, 6000, 6100, 6200, 6300};

int app_main()
{
    _app_main();

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    // make sure that RAM does not exceed 0x20004000
    // then, we can power off the unused block 1
    SYSCTRL_SelectMemoryBlocks(SYSCTRL_MEM_BLOCK_0 | SYSCTRL_MEM_BLOCK_1);
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
    // then, we can power off the unused block 1
    SYSCTRL_SelectMemoryBlocks(SYSCTRL_MEM_BLOCK_0);
#endif

    platform_set_rf_power_mapping(power_mapping);

    platform_config(PLATFORM_CFG_USE_MINIMUM_RAM, PLATFORM_CFG_ENABLE);

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
