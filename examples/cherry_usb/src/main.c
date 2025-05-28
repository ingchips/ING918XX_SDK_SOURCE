#include <stdio.h>
#include <string.h>
#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include "port_gen_os_driver.h"
#include "trace.h"
#include "../data/setup_soc.cgen"
#include "SEGGER_RTT.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usbh_core.h"

static uint32_t cb_hard_fault(hard_fault_info_t *info, void *_)
{
    platform_printf("HARDFAULT:\nPC : 0x%08X\nLR : 0x%08X\nPSR: 0x%08X\n"
                    "R0 : 0x%08X\nR1 : 0x%08X\nR2 : 0x%08X\nR3 : 0x%08X\n"
                    "R12: 0x%08X\n",
                    info->pc, info->lr, info->psr,
                    info->r0, info->r1, info->r2, info->r3, info->r12);
    for (;;);
}

static uint32_t cb_assertion(assertion_info_t *info, void *_)
{
    platform_printf("[ASSERTION] @ %s:%d\n",
                    info->file_name,
                    info->line_no);
    for (;;);
}

static uint32_t cb_heap_out_of_mem(uint32_t tag, void *_)
{
    platform_printf("[OOM] @ %d\n", tag);
    for (;;);
}


uint32_t cb_putc(char *c, void *dummy)
{
    while (apUART_Check_TXFIFO_FULL(APB_UART0) == 1);
    UART_SendData(APB_UART0, (uint8_t)*c);
//    SEGGER_RTT_Write(0, c, 1);
    return 0;
}

int fputc(int ch, FILE *f)
{
    cb_putc((char *)&ch, NULL);
    return ch;
}

void setup_peripherals(void)
{
    cube_setup_peripherals();
}

uint32_t on_lle_init(void *dummy, void *user_data)
{
    (void)(dummy);
    (void)(user_data);
    cube_on_lle_init();
    return 0;
}


static const platform_evt_cb_table_t evt_cb_table =
{
    .callbacks = {
        [PLATFORM_CB_EVT_HARD_FAULT] = {
            .f = (f_platform_evt_cb)cb_hard_fault,
        },
        [PLATFORM_CB_EVT_ASSERTION] = {
            .f = (f_platform_evt_cb)cb_assertion,
        },
        [PLATFORM_CB_EVT_HEAP_OOM] = {
            .f = (f_platform_evt_cb)cb_heap_out_of_mem,
        },
        [PLATFORM_CB_EVT_PROFILE_INIT] = {
            .f = setup_profile,
        },
        [PLATFORM_CB_EVT_LLE_INIT] = {
            .f = on_lle_init,
        },
        [PLATFORM_CB_EVT_PUTC] = {
            .f = (f_platform_evt_cb)cb_putc,
        },
    }
};




static void test_usb_task(void *pdata)
{
    #if CONFIG_USE_USB_DEVICE
    extern void hid_mouse_init(uint8_t busid, uint32_t reg_base);
    hid_mouse_init(0, AHB_USB_BASE);
    #elif CONFIG_USE_USB_HOST
    usbh_initialize(0, AHB_USB_BASE);
    #endif
    while(1)
    {
        #if CONFIG_USE_USB_DEVICE
        extern void hid_mouse_test(uint8_t busid);
        hid_mouse_test(0);
        #endif
        vTaskDelay(1000);
    
    }
}
extern uint32_t SystemCoreClock;
// TODO: add RTOS source code to the project.
uintptr_t app_main()
{
    SYSCTRL_Init();
    cube_soc_init();

    // setup event handlers
    platform_set_evt_callback_table(&evt_cb_table);

    setup_peripherals();
    
    SEGGER_RTT_Init();
    printf("init\n");
    SystemCoreClock = SYSCTRL_GetPLLClk();
    
    xTaskCreate(test_usb_task,
           "usb task",
           configMINIMAL_STACK_SIZE,
           NULL,
           (configMAX_PRIORITIES - 1),
           NULL);

    return (uintptr_t)os_impl_get_driver();
}

