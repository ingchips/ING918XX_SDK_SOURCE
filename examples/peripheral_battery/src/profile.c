#include <stdint.h>
#include "platform_api.h"
#include "gap.h"
#include "att_db.h"
#include "btstack_event.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "freertos_util.h"

#include "ingsoc.h"
#include "peripheral_adc.h"
#include <stdio.h>
#include <stdlib.h>

#include "adc_cali.h"
#include "profile.h"

// GATT characteristic handles
#include "../data/gatt.const"

const static uint8_t adv_data[] = {
    #include "../data/advertising.adv"
};

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
};

static uint8_t profile_data[] = {
    #include "../data/gatt.profile"
};

static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, 
                                  uint8_t * buffer, uint16_t buffer_size)
{
    switch (att_handle)
    {

    default:
        return 0;
    }
}

static btstack_packet_callback_registration_t hci_event_callback_registration;

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, 
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    switch (att_handle)
    {

    default:
        return 0;
    }
}

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
        // add your code
    //case MY_MESSAGE_ID:
    //    break;
    default:
            ;
    }
}

bd_addr_t null_addr = {0xAB, 0x89, 0x67, 0x45, 0x23, 0x01};

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    const static ext_adv_set_en_t adv_sets_en[1] = {{.handle = 0, .duration = 0, .max_events = 0}};
    bd_addr_t rand_addr = {1,2,3,4,5,6};    // TODO: random address generation
    uint8_t event = hci_event_packet_get_type(packet);
    const btstack_user_msg_t *p_user_msg;
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;

        gap_set_adv_set_random_addr(0, rand_addr);
        gap_set_ext_adv_para(0, 
                                CONNECTABLE_ADV_BIT | SCANNABLE_ADV_BIT | LEGACY_PDU_BIT,
                                0x00a1, 0x00a1,            // Primary_Advertising_Interval_Min, Primary_Advertising_Interval_Max
                                PRIMARY_ADV_ALL_CHANNELS,  // Primary_Advertising_Channel_Map
                                BD_ADDR_TYPE_LE_RANDOM,    // Own_Address_Type
                                BD_ADDR_TYPE_LE_PUBLIC,    // Peer_Address_Type (ignore)
                                NULL,                      // Peer_Address      (ignore)
                                ADV_FILTER_ALLOW_ALL,      // Advertising_Filter_Policy
                                0x00,                      // Advertising_Tx_Power
                                PHY_1M,                    // Primary_Advertising_PHY
                                0,                         // Secondary_Advertising_Max_Skip
                                PHY_1M,                    // Secondary_Advertising_PHY
                                0x00,                      // Advertising_SID
                                0x00);                     // Scan_Request_Notification_Enable
        gap_set_ext_adv_data(0, sizeof(adv_data), (uint8_t*)adv_data);
        gap_set_ext_scan_response_data(0, sizeof(scan_data), (uint8_t*)scan_data);
        gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
            att_set_db(decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t)->handle,
                       profile_data);
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
        break;

    case ATT_EVENT_CAN_SEND_NOW:
        // add your code
        break;

    case BTSTACK_EVENT_USER_MSG:
        p_user_msg = hci_event_packet_get_user_msg(packet);
        user_msg_handler(p_user_msg->msg_id, p_user_msg->data, p_user_msg->len);
        break;

    default:
        break;
    }
}

#define SAMPLERATE  100
#define ADC_CLK_MHZ 6
#define ADC_CH_NUM  1
#define LOOP_DELAY(c, s, ch)      ((((c) * (1000000)) / (s)) - (((16) * (ch)) + (5)))
void ADC_AveInit(void);
uint16_t read_adc(uint8_t channel)
{
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    SYSCTRL_WaitForLDO();

    ADC_Reset();
    ADC_PowerCtrl(1);    

    ADC_SetClkSel(ADC_CLK_EN | ADC_CLK_128);
    ADC_SetMode(ADC_MODE_LOOP);
    ADC_EnableChannel(channel == 0 ? 1 : 0, 1);
    ADC_EnableChannel(channel, 1);
    ADC_Enable(1);

    while (ADC_IsChannelDataValid(channel) == 0) ;
    uint16_t voltage = ADC_ReadChannelData(channel);

    ADC_ClearChannelDataValid(channel);
    while (ADC_IsChannelDataValid(channel) == 0) ;
    voltage = ADC_ReadChannelData(channel);

    ADC_Enable(0);
    ADC_PowerCtrl(0);

    return adc_calibrate(ADC_SAMPLE_MODE_SLOW, channel, voltage);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    ADC_ConvCfg(CONTINUES_MODE, PGA_PARA_1, 1, (SADC_channelId)channel, AVE_NUM, 0, 
        SINGLE_END_MODE, LOOP_DELAY(ADC_CLK_MHZ, SAMPLERATE, ADC_CH_NUM));
    ADC_AveInit();
    ADC_Start(1);
    while (!ADC_GetIntStatus()) ;
    ADC_Start(0);
    ADC_EnableChannel((SADC_channelId)channel, 0);
    ADC_IntEnable(0);
    uint32_t data;
    uint16_t sample;
    while (!ADC_GetFifoEmpty()) {
        data = ADC_PopFifoData();
        sample = ADC_GetAveData(data);
    }
    ADC_AveDisable();
    return sample;
#endif
}

uint8_t *battery_level = NULL;

#define ADC_CHANNEL    4

#ifdef DEMO_TASK_DELAY_UNTIL

static void battery_task(void *pdata)
{
    uint16_t voltage;
    TickType_t xPreviousWakeTime = xTaskGetTickCount();
    for (;;)
    {
        vTaskDelayUntil(&xPreviousWakeTime, AccurateMS_TO_TICKS(900));

#ifndef SIMULATION        
        printf("U = %d", read_adc(ADC_CHANNEL));
#else
        voltage = 800 + rand() % 200;
#endif
        // level is reported by comparing max & min voltage
        // for DEMO only
#define MAX_VOLT       1023
#define MIN_VOLT       800
        if (voltage > MIN_VOLT)
            *battery_level = (uint32_t)(voltage - MIN_VOLT) * 100 / (MAX_VOLT - MIN_VOLT);
        else
            *battery_level = 0;
        
        // add a random delay. see how ADC is sampled at fixed intervals.
        vTaskDelay(pdMS_TO_TICKS(100 + (platform_rand() & 0xf) * 10));
    }
}

uint32_t timer_isr(void *user_data)
{
    TMR_IntClr(APB_TMR1);
    return 0;
}

#else

static SemaphoreHandle_t sem_battery = NULL;

static void battery_task(void *pdata)
{
    uint16_t voltage;
    for (;;)
    {
        BaseType_t r = xSemaphoreTake(sem_battery,  portMAX_DELAY);
        if (r != pdTRUE)
            continue;
            
#ifndef SIMULATION        
        printf("U = %d\n", read_adc(ADC_CHANNEL));
#else
        voltage = 800 + rand() % 200;
#endif
        // level is reported by comparing max & min voltage
        // for DEMO only
#define MAX_VOLT       1023
#define MIN_VOLT       800
        if (voltage > MIN_VOLT)
            *battery_level = (uint32_t)(voltage - MIN_VOLT) * 100 / (MAX_VOLT - MIN_VOLT);
        else
            *battery_level = 0;
    }
}

uint32_t timer_isr(void *user_data)
{
    BaseType_t xHigherPriorityTaskWoke = pdFALSE;
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    TMR_IntClr(APB_TMR1);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    TMR_IntClr(APB_TMR1, 0, 0xf);
#else
    #error unknown or unsupported chip family
#endif
    xSemaphoreGiveFromISR(sem_battery, &xHigherPriorityTaskWoke);
    return 0;
}

#endif

uint32_t setup_profile(void *data, void *user_data)
{
    platform_printf("setup_profile\n");
    battery_level = profile_data + HANDLE_BATTERY_LEVEL_OFFSET;

#ifdef DEMO_TASK_DELAY_UNTIL
    platform_32k_rc_auto_tune();
    vUpdateTicksClockFrequency();
#else
    sem_battery = xSemaphoreCreateBinary();
#endif

    xTaskCreate(battery_task,
               "b",
               configMINIMAL_STACK_SIZE,
               NULL,
               (configMAX_PRIORITIES - 1),
               NULL);
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}

