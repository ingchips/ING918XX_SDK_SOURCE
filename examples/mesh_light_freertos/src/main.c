#include "ingsoc.h"
#include "platform_api.h"
#include "peripheral_pwm.h"
#include <stdio.h>
#include "profile.h"
#include "trace.h"
#include "btstack_event.h"
#include "mesh_setup_profile.h"
#include "rgb_led.h"

// #define ENABLE_RF_TX_RX //used to enable rf tx and rx.

#ifdef ENABLE_RF_TX_RX
#define PRINT_UART_BAUD     921600
#else
#define PRINT_UART_BAUD     115200
#endif
#define PRINT_UART          APB_UART0

// GPIO SELECT ----------------------------------------
#ifdef ENABLE_RF_TX_RX

#define RF_IO_TX_IND        GIO_GPIO_2 //RF tx
#define RF_IO_RX_IND        GIO_GPIO_6 //RF rx

#define USER_UART0_IO_TX    GIO_GPIO_0 //uart0 tx
#define USER_UART0_IO_RX    GIO_GPIO_9 //uart0 rx

#else

#define USER_UART0_IO_TX    GIO_GPIO_2 //uart0 tx
#define USER_UART0_IO_RX    GIO_GPIO_3 //uart0 rx

#endif

#define KB_KEY_1            GIO_GPIO_1 //key 1
#define KB_KEY_2            GIO_GPIO_5 //key 2
#define KB_KEY_3            GIO_GPIO_7 //key 3
#define KB_KEY_4            GIO_GPIO_4 //key 4
//-----------------------------------------------------


uint32_t cb_hard_fault(hard_fault_info_t *info, void *_)
{
    platform_printf("HARDFAULT:\nPC : 0x%08X\nLR : 0x%08X\nPSR: 0x%08X\n"
                    "R0 : 0x%08X\nR1 : 0x%08X\nR2 : 0x%08X\nP3 : 0x%08X\n"
                    "R12: 0x%08X\n",
                    info->pc, info->lr, info->psr,
                    info->r0, info->r1, info->r2, info->r3, info->r12);
    for (;;);
}

uint32_t cb_assertion(assertion_info_t *info, void *_)
{
    platform_printf("[ASSERTION] @ %s:%d\n",
                    info->file_name,
                    info->line_no);
    for (;;);
}

uint32_t cb_putc(char *c, void *dummy)
{
    while (apUART_Check_TXFIFO_FULL(PRINT_UART) == 1);
    UART_SendData(PRINT_UART, (uint8_t)*c);
    return 0;
}

int fputc(int ch, FILE *f)
{
   cb_putc((char *)&ch, NULL);
   return ch;
}

void __aeabi_assert(const char *a ,const char* b, int c)
{
    platform_printf("assert:%s,%s,%d\n", a, b, c);
    for (;;);
}


#ifdef ENABLE_RF_TX_RX

static void rf_tx_rx_gpio_init(void){

    PINCTRL_SetPadMux(RF_IO_TX_IND, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(RF_IO_TX_IND, 0);
    GIO_SetDirection(RF_IO_TX_IND, GIO_DIR_OUTPUT);
    GIO_WriteValue(RF_IO_TX_IND, 0);

    PINCTRL_SetPadMux(RF_IO_RX_IND, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(RF_IO_RX_IND, 0);
    GIO_SetDirection(RF_IO_RX_IND, GIO_DIR_OUTPUT);
    GIO_WriteValue(RF_IO_RX_IND, 0);

}

#define w32(a,b) *(volatile uint32_t*)(a) = (uint32_t)(b);
#define r32(a)   *(volatile uint32_t*)(a)

void set_gpio_2_6_for_rf_tx_rx(void)
{
    w32(0x4007005c, 0x82);
    w32(0x40070044, ((r32(0x40070044)) | (0xf<<8) | (0xf<<24)));
    w32(0x40090064, 0x400);
}

#else 

void set_gpio_2_6_for_rf_tx_rx(void)
{

}

#endif


static void uart_gpio_init(void){
    SYSCTRL_ClearClkGateMulti(1 << SYSCTRL_ClkGate_APB_UART0);
    SYSCTRL_ClearClkGateMulti(  (1 << SYSCTRL_ClkGate_APB_GPIO) | 
                                (1 << SYSCTRL_ClkGate_APB_PWM)  | 
                                (1 << SYSCTRL_ClkGate_APB_PinCtrl));
    
    PINCTRL_SetPadMux(USER_UART0_IO_RX, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(USER_UART0_IO_RX, 0);
    PINCTRL_SetPadMux(USER_UART0_IO_TX, IO_SOURCE_UART0_TXD);
    PINCTRL_Pull(USER_UART0_IO_RX, PINCTRL_PULL_UP);
    PINCTRL_SelUartRxdIn(UART_PORT_0, USER_UART0_IO_RX);
}

void config_uart(uint32_t freq, uint32_t baud)
{
    UART_sStateStruct UART_0;

    UART_0.word_length       = UART_WLEN_8_BITS;
    UART_0.parity            = UART_PARITY_NOT_CHECK;
    UART_0.fifo_enable       = 1;
    UART_0.two_stop_bits     = 0;   //used 2 stop bit ,    0
    UART_0.receive_en        = 1;
    UART_0.transmit_en       = 1;
    UART_0.UART_en           = 1;
    UART_0.cts_en            = 0;
    UART_0.rts_en            = 0;
    UART_0.rxfifo_waterlevel = 1;   //UART_FIFO_ONE_SECOND;
    UART_0.txfifo_waterlevel = 1;   //UART_FIFO_ONE_SECOND;
    UART_0.ClockFrequency    = freq;
    UART_0.BaudRate          = baud;

    apUART_Initialize(PRINT_UART, &UART_0, 0);
}




#define KEY_MASK        ((1 << KB_KEY_1) | (1 << KB_KEY_2) | (1 << KB_KEY_3) | (1 << KB_KEY_4))

void setup_key_gpio(void)
{    
    SYSCTRL_ClearClkGateMulti(  (1 << SYSCTRL_ClkGate_APB_GPIO0) |
                                (1 << SYSCTRL_ClkGate_APB_GPIO1) |
                                (1 << SYSCTRL_ClkGate_APB_PinCtrl));

    // setup GPIOs for keys
    PINCTRL_SetPadMux(KB_KEY_1, IO_SOURCE_GPIO);
    PINCTRL_SetPadMux(KB_KEY_2, IO_SOURCE_GPIO);
    PINCTRL_SetPadMux(KB_KEY_3, IO_SOURCE_GPIO);
    PINCTRL_SetPadMux(KB_KEY_4, IO_SOURCE_GPIO);
    GIO_SetDirection(KB_KEY_1, GIO_DIR_INPUT);
    GIO_SetDirection(KB_KEY_2, GIO_DIR_INPUT);
    GIO_SetDirection(KB_KEY_3, GIO_DIR_INPUT);
    GIO_SetDirection(KB_KEY_4, GIO_DIR_INPUT);
    GIO_ConfigIntSource(KB_KEY_1, GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE | GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE,
                        GIO_INT_EDGE);
    GIO_ConfigIntSource(KB_KEY_2, GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE | GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE,
                        GIO_INT_EDGE);
    GIO_ConfigIntSource(KB_KEY_3, GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE | GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE,
                        GIO_INT_EDGE);
    GIO_ConfigIntSource(KB_KEY_4, GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE | GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE,
                        GIO_INT_EDGE);
}

#define KEY_1_PRESSED   1
#define KEY_2_PRESSED   2
#define KEY_3_PRESSED   4
#define KEY_4_PRESSED   8

static uint8_t key_pressed[4] = {1,2,4,8};
static uint8_t key_old[4] = {0,0,0,0};


void key_proc_in_host_task(uint8_t num){
    switch(num){
        case 1: // key1 : change connection interval.
            {
                printf("key1 : change connection interval. \n");
                extern void app_update_conn_params_req(void);
                app_update_conn_params_req();
            }
            break;
        case 2: // key2 : clear all mesh info in flash.
            {
                printf("key2 : clear all mesh info in flash. \n");
                extern void mesh_ble_params_reset_delay_timer_start(uint32_t timeout_in_ms);
                mesh_ble_params_reset_delay_timer_start(100);
            }
            break;
        case 3: // key3 : change scan params.
            {
                printf("key3 : change scan params. \n");
                extern void mesh_scan_param_update(void);
                mesh_scan_param_update();
            }
            break;
        case 4: // key4 : send non-conn data.
            {
                printf("key4 : send non-conn data. \n");
                extern void mesh_send_non_conn_data(void);
                mesh_send_non_conn_data();
            }
            break;
    }
}

static void key_process_in_uart_int(uint8_t num){
    switch(num){
        case 1: // key1 pressed event.
        case 2: // key2 pressed event.
        case 3:
        case 4:
            btstack_push_user_msg(USER_MSG_ID_KEY_EVENT, NULL, (uint16_t)num);
            break;
    }
}

void kb_state_changed(uint16_t key_state)
{
    uint8_t i=0;
    // platform_printf("key state changed:%x\n", key_state);
    for(i=0; i<4; i++){
        if(key_state & key_pressed[i]){
            if(!key_old[i]){
                key_old[i] = 1;
                // platform_printf("key %d pressed.\n", (i+1));
                key_process_in_uart_int(i+1);
            }
        } else {
            if(key_old[i]){
                key_old[i] = 0;
                // platform_printf("key %d released.\n", (i+1));
            }
        }
    }
}

uint32_t gpio_isr(void *user_data)
{
    uint32_t current = ~GIO_ReadAll();
    uint16_t v = 0;
    // report which keys are pressed
    if (current & (1 << KB_KEY_1))
        v |= 1;
    if (current & (1 << KB_KEY_2))
        v |= 2;
    if (current & (1 << KB_KEY_3))
        v |= 4;
    if (current & (1 << KB_KEY_4))
        v |= 8;
    kb_state_changed(v);

    GIO_ClearAllIntStatus();
    return 0;
}


void setup_peripherals(void)
{
    uart_gpio_init();
    config_uart(OSC_CLK_FREQ, PRINT_UART_BAUD);
    
    setup_rgb_led();

    setup_key_gpio();

#ifdef ENABLE_RF_TX_RX
    rf_tx_rx_gpio_init();
    set_gpio_2_6_for_rf_tx_rx(); // may fail.
#endif
}

trace_rtt_t trace_ctx = {0};
extern const gen_os_driver_t *os_impl_get_driver(void);
int app_main()
{
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    // platform_set_rf_clk_source(0);

    platform_set_irq_callback(PLATFORM_CB_IRQ_GPIO, gpio_isr, NULL);
    setup_peripherals();

    // setup putc handle
    platform_set_evt_callback(PLATFORM_CB_EVT_HARD_FAULT, (f_platform_evt_cb)cb_hard_fault, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_ASSERTION, (f_platform_evt_cb)cb_assertion, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);

    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);

    trace_rtt_init(&trace_ctx);
    platform_set_evt_callback(PLATFORM_CB_EVT_TRACE, (f_platform_evt_cb)cb_trace_rtt, &trace_ctx);
    platform_config(PLATFORM_CFG_TRACE_MASK, 0xfff);
    // platform_config(PLATFORM_CFG_LOG_HCI, PLATFORM_CFG_ENABLE);//enable hci log

    return (int)os_impl_get_driver();
}

