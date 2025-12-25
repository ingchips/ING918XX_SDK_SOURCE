#include "peripheral_sysctrl.h"
#include "platform_lib.h"
#include "platform_api.h"

const platform_ver_t *platform_get_version(void)
{
    extern uint32_t __PLATFORM_VER;
    return (const platform_ver_t *)&__PLATFORM_VER;
}

static void set_reg_bit(volatile uint32_t *reg, uint8_t v, uint8_t bit_offset)
{
    if (v)
        *reg |= 1 << bit_offset;
    else
        *reg &= ~(1 << bit_offset);
}

static void SYSCTRL_DisableSlowRC(void)
{
    #define AON1_PMU3       ((volatile uint32_t *)(AON1_CTRL_BASE + 0x3c))

    set_reg_bit(AON1_PMU3, 0, 19);
}

/* Note: If 24M OSC is used as source of slow clock, then
 * this simplified `SYSCTRL_GetSlowClk` can be used:
 *
 * uint32_t SYSCTRL_GetSlowClk(void)
 * {
 *     return OSC_CLK_FREQ;
 * }
 */

int main(void)
{
    SYSCTRL_ConfigPLLClk(5, 80, 1);
    SYSCTRL_EnablePLL(1);
    SYSCTRL_SelectFlashClk(SYSCTRL_CLK_PLL_DIV_5);
    SYSCTRL_SelectHClk(SYSCTRL_CLK_PLL_DIV_3);

    SYSCTRL_DisableSlowRC();

    platform_config(PLATFORM_CFG_RT_CLK, PLATFORM_RT_RC);
    platform_config(PLATFORM_CFG_RT_OSC_EN, 0);

    launch_platform(3);

    return 0;
}
