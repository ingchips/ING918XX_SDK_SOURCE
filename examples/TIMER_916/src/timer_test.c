#include <string.h>
#include "peripheral_timer.h"
#include "peripheral_sysctrl.h"
#include "peripheral_pinctrl.h"
#include "platform_api.h"

#if defined(TIMER_TEST)

uint32_t timer_isr(void *user_data)
{
    uint8_t state = TMR_GetIntState(APB_TMR1, 0);
    TMR_ClearIntState(APB_TMR1,0,state);
    printf("[timer tick]\n");
	return 0;
}

void timer_ability_test(void)
{
    SYSCTRL_ClearClkGateMulti( (1 << SYSCTRL_ITEM_APB_SysCtrl)
                                |(1 << SYSCTRL_ITEM_APB_PinCtrl)
                                |(1 << SYSCTRL_ITEM_APB_TMR1));
    // 使用计时器功能
    // 将TIMER1的通道0设置为`TMR_CTL_OP_MODE_32BIT_TIMER_x1`模式，并设定每 5000 APB 时钟周期产生一次中断：
    platform_set_irq_callback(PLATFORM_CB_IRQ_TIMER1, timer_isr, NULL);
    TMR_SetOpMode(APB_TMR1, 0, TMR_CTL_OP_MODE_32BIT_TIMER_x1, TMR_CLK_MODE_APB, 0);
    TMR_SetReload(APB_TMR1, 0, 0x00001387);  //4999
    printf("TMR_GetCMP = %x\n",TMR_GetCMP(APB_TMR1, 0));
    TMR_Enable(APB_TMR1, 0, 0xf);
    TMR_IntEnable(APB_TMR1, 0, 0xf);  
}

void timer_two_int_test(void)
{
    // 通道0产生2个周期性中断
    // 使用TIMER1通道0生成2个中断:一个用于每1000个APB时钟周期，另一个用于每3000个APB周期。
      SYSCTRL_ClearClkGateMulti( (1 << SYSCTRL_ITEM_APB_SysCtrl)
                                |(1 << SYSCTRL_ITEM_APB_PinCtrl)
                                |(1 << SYSCTRL_ITEM_APB_TMR1));

      SYSCTRL_SelectTimerClk(TMR_PORT_1, SYSCTRL_CLK_32k);
      TMR_SetOpMode(APB_TMR1, 0, TMR_CTL_OP_MODE_16BIT_TIMER_x2, TMR_CLK_MODE_APB, 0);
      TMR_SetReload(APB_TMR1, 0, 0x0BB703E7);  // 2999  999

      TMR_IntEnable(APB_TMR1, 0, 0x3);  //Ch0Int0 Ch0Int1
      TMR_Enable(APB_TMR1, 0, 0x3);     //Ch0TMR0En Ch0TMR1En

}

void timer_pwm_test(void)
{
    // 使用TIMER的PWM功能
    // 将TIMER1的通道0的工作模式设置为 `TMR_CTL_OP_MODE_16BIT_PWM` ，并使用13号引脚输出10HzPWM信号。
    #define PIN_TMR_PWM 13


     SYSCTRL_ClearClkGateMulti( (1 << SYSCTRL_ITEM_APB_SysCtrl)
                                |(1 << SYSCTRL_ITEM_APB_PinCtrl)
                                |(1 << SYSCTRL_ITEM_APB_TMR1));

     SYSCTRL_SelectTimerClk(TMR_PORT_1, SYSCTRL_CLK_32k);
     TMR_SetOpMode(APB_TMR1, 0, TMR_CTL_OP_MODE_16BIT_PWM, TMR_CLK_MODE_EXTERNAL, 0);
     TMR_SetReload(APB_TMR1, 0, 0x00090009); // 9 9
     TMR_Enable(APB_TMR1, 0, 0xf);

     PINCTRL_SetPadMux(PIN_TMR_PWM, IO_SOURCE_TIMER1_PWM0_B);
}

void timer_two_pwm_test(void)
{

    // 产生2路对齐的PWM信号
    // 使用TIMER1的通道0和通道1分别生成两路PWM信号PWM0和PWM1，对应参数设置如下所示：
    // PWM0:周期= 30个外部时钟周期，占空比= 1/3
    // PWM1:周期= 60个外部时钟周期，占空比= 1/3
    // 将两路PWM对齐，并分别由引脚13、14输出。
    #define PIN_TMR_PWM0 13
    #define PIN_TMR_PWM1 14

     SYSCTRL_ClearClkGateMulti( (1 << SYSCTRL_ITEM_APB_SysCtrl)
                                |(1 << SYSCTRL_ITEM_APB_PinCtrl)
                                |(1 << SYSCTRL_ITEM_APB_TMR1));

     SYSCTRL_SelectTimerClk(TMR_PORT_1, SYSCTRL_CLK_32k);

     TMR_SetOpMode(APB_TMR1, 0, TMR_CTL_OP_MODE_16BIT_PWM, TMR_CLK_MODE_EXTERNAL, 1);
     TMR_SetOpMode(APB_TMR1, 1, TMR_CTL_OP_MODE_16BIT_PWM, TMR_CLK_MODE_EXTERNAL, 1);
     TMR_SetReload(APB_TMR1, 0, 0x00090013);   //9   19
     TMR_SetReload(APB_TMR1, 1, 0x00130027);   //19  39
     TMR_Enable(APB_TMR1, 0, 0xf);
     TMR_Enable(APB_TMR1, 1, 0xf);

     PINCTRL_SetPadMux(PIN_TMR_PWM0, IO_SOURCE_TIMER1_PWM0_B);
     PINCTRL_SetPadMux(PIN_TMR_PWM1, IO_SOURCE_TIMER1_PWM1_A);
}

void timer_test(void)
{
    timer_ability_test();
    // timer_pwm_test();
    // timer_two_int_test();
    // timer_two_pwm_test();
}
#endif