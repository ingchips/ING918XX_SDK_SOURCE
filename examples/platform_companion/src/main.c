#include "peripheral_sysctrl.h"
#include "platform_lib.h"
#include "platform_api.h"

const platform_ver_t *platform_get_version(void)
{
    extern uint32_t __PLATFORM_VER;
    return (const platform_ver_t *)&__PLATFORM_VER;
}

int main(void)
{
    SYSCTRL_ConfigPLLClk(5, 70, 1);
    SYSCTRL_EnablePLL(1);
    SYSCTRL_SelectFlashClk(SYSCTRL_CLK_PLL_DIV_2);
    SYSCTRL_SelectHClk(SYSCTRL_CLK_PLL_DIV_3);

    SYSCTRL_EnableSlowRC(0, (SYSCTRL_SlowRCClkMode)0);

    platform_config(PLATFORM_CFG_RT_CLK, PLATFORM_RT_RC);
    platform_config(PLATFORM_CFG_RT_OSC_EN, 0);

    launch_platform();

    return 0;
}
