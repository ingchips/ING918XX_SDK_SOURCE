#include <stdio.h>
#include <string.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include "uart_driver.h"

#include "ll_api.h"

static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset,
                                  uint8_t * buffer, uint16_t buffer_size)
{
    switch (att_handle)
    {

    default:
        return 0;
    }
}

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode,
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    switch (att_handle)
    {

    default:
        return 0;
    }
}

#define USER_MSG_HCI            1

#pragma pack (push, 1)

typedef struct hci_cmd_header
{
    uint8_t tag;
    uint16_t op_code;
    uint8_t param_len;
} hci_cmd_header_t;

struct
{
    int rx_cnt;
    hci_cmd_header_t header;
    uint8_t data[256];
} hci_cmd_fsm = {0};

typedef struct hci_cmd
{
    hci_cmd_header_t header;
    uint8_t data[0];
} hci_cmd_t;

#pragma pack (pop)

void rx_hci_byte(void *user_data, uint8_t c)
{
    uint8_t *p = (uint8_t *)&hci_cmd_fsm.header;
    p[hci_cmd_fsm.rx_cnt++] = c;
    if (hci_cmd_fsm.rx_cnt < sizeof(hci_cmd_header_t)) return;
    if (hci_cmd_fsm.rx_cnt != sizeof(hci_cmd_header_t) + hci_cmd_fsm.header.param_len) return;
    uint8_t *msg = ll_malloc(hci_cmd_fsm.rx_cnt);
    memcpy(msg, p, hci_cmd_fsm.rx_cnt);
    btstack_push_user_msg(USER_MSG_HCI, msg, hci_cmd_fsm.rx_cnt);
    hci_cmd_fsm.rx_cnt = 0;
}

#define HCI_Reset                   0x0c03
#define HCI_LE_Receiver_Test_v1     0x201d
#define HCI_LE_Receiver_Test_v2     0x2033
#define HCI_LE_Transmitter_Test_v1  0x201E
#define HCI_LE_Transmitter_Test_v2  0x2034
#define HCI_LE_Transmitter_Test_v4  0x207b
#define HCI_LE_Test_End             0x201f

#define HCI_LE_Vendor_CW_Test       0xfc02

#define DEF_NUM_OF_HCI              0x0b

static uint16_t test_op_code = 0;

void send_event(uint8_t event_code,
    const uint8_t *param,
    int param_len)
{
    static uint8_t packet[200] = {0x4, 0};
    packet[1] = event_code;
    packet[2] = param_len;
    memcpy(packet + 3, param, param_len);
    driver_append_tx_data(packet, param_len + 3);
}

void send_command_complete(uint8_t Num_HCI_Command_Packets,
    uint16_t Command_Opcode,
    const uint8_t *Return_Parameters,
    int param_len)
{
    static uint8_t param[200] = {0};
    param[0] = Num_HCI_Command_Packets;
    little_endian_store_16(param, 1, Command_Opcode);
    memcpy(param + 3, Return_Parameters, param_len);
    send_event(0x0e, param, param_len + 3);
}

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    uint8_t status = 0x12;
    switch (msg_id)
    {
    case USER_MSG_HCI:
        {
            hci_cmd_t *cmd = (hci_cmd_t *)data;
            test_op_code = cmd->header.op_code;
            switch (cmd->header.op_code)              
            {
            case HCI_Reset:
                platform_reset();
                break;
            case HCI_LE_Receiver_Test_v1:
                {
                    typedef struct
                    {
                        uint8_t RX_Channel;
                    } param_t;
                    param_t *param = (param_t *)cmd->data;
                    gap_rx_test_v2(param->RX_Channel, 1, 0);
                }
                break;
            case HCI_LE_Receiver_Test_v2:
                {
                    typedef struct
                    {
                        uint8_t RX_Channel;
                        uint8_t PHY;
                        uint8_t Modulation_Index;
                    } param_t;
                    param_t *param = (param_t *)cmd->data;
                    gap_rx_test_v2(param->RX_Channel, param->PHY, param->Modulation_Index);
                }
                break;
            case HCI_LE_Transmitter_Test_v1:
                {
                    typedef struct
                    {
                        uint8_t TX_Channel;
                        uint8_t Test_Data_Length;
                        uint8_t Packet_Payload;
                    } param_t;
                    param_t *param = (param_t *)cmd->data;
                    gap_tx_test_v2(param->TX_Channel, param->Test_Data_Length, 
                                   param->Packet_Payload, 1);
                }
                break;
            case HCI_LE_Transmitter_Test_v2:
                {
                    typedef struct
                    {
                        uint8_t TX_Channel;
                        uint8_t Test_Data_Length;
                        uint8_t Packet_Payload;
                        uint8_t PHY;
                    } param_t;
                    param_t *param = (param_t *)cmd->data;
                    gap_tx_test_v2(param->TX_Channel, param->Test_Data_Length, 
                                   param->Packet_Payload, param->PHY);
                }
                break;
            case HCI_LE_Transmitter_Test_v4:
                {
                    typedef struct
                    {
                        uint8_t TX_Channel;
                        uint8_t Test_Data_Length;
                        uint8_t Packet_Payload;
                        uint8_t PHY;
                        uint8_t CTE_Length;
                        uint8_t CTE_Type;
                        uint8_t Switching_Pattern_Length;
                        uint8_t TX_Power_Level;
                    } param_t;
                    if (cmd->header.param_len == sizeof(param_t))
                    {
                        param_t *param = (param_t *)cmd->data;
                        gap_tx_test_v4(param->TX_Channel, param->Test_Data_Length, 
                                       param->Packet_Payload, param->PHY,
                                       param->CTE_Length, param->CTE_Type,
                                       0, NULL, param->TX_Power_Level);
                    }
                    else
                    {
                         send_command_complete(0x0b,
                              cmd->header.op_code,
                              &status,
                              1);
                    }
                }
                break;
            case HCI_LE_Test_End:
                gap_test_end();
                break;
            case HCI_LE_Vendor_CW_Test:
                {
                    #pragma pack (push, 1)
                    typedef struct
                    {
                        uint8_t enable;
                        uint8_t reserved;
                        uint8_t power_index;
                        uint16_t freq;
                    } param_t;
                    #pragma pack (pop)
                    if (cmd->header.param_len == sizeof(param_t))
                    {
                        param_t *param = (param_t *)cmd->data;
                        gap_vendor_tx_continuous_wave(param->enable, param->power_index, (param->freq - 2) / 4);
                    }
                    else
                    {
                         send_command_complete(0x0b,
                              cmd->header.op_code,
                              &status,
                              1);
                    }
                }
                break;
            default:
                send_command_complete(DEF_NUM_OF_HCI,
                              cmd->header.op_code,
                              &status,
                              1);
                break;
            }
            ll_free(cmd);
        }
        break;
    default:
        ;
    }
}

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    uint8_t event = hci_event_packet_get_type(packet);
    const btstack_user_msg_t *p_user_msg;
    if (packet_type != HCI_EVENT_PACKET) return;
    uint8_t status = 0;
    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;
        send_command_complete(DEF_NUM_OF_HCI,
                              HCI_Reset,
                              &status,
                              1);
        break;

    case HCI_EVENT_COMMAND_COMPLETE:
        switch (hci_event_command_complete_get_command_opcode(packet))
        {
        case HCI_LE_Receiver_Test_v2:
        case HCI_LE_Transmitter_Test_v2:
        case HCI_LE_Transmitter_Test_v4:
        case HCI_LE_Vendor_CW_Test:
            send_command_complete(hci_event_command_complete_get_num_hci_command_packets(packet),
                                  test_op_code,
                                  hci_event_command_complete_get_return_parameters(packet),
                                  1);
            break;
        case HCI_LE_Test_End:
            send_command_complete(hci_event_command_complete_get_num_hci_command_packets(packet),
                                  HCI_LE_Test_End,
                                  hci_event_command_complete_get_return_parameters(packet),
                                  3);
            break;
        }
        break;

    case HCI_EVENT_LE_META:
        break;

    case BTSTACK_EVENT_USER_MSG:
        p_user_msg = hci_event_packet_get_user_msg(packet);
        user_msg_handler(p_user_msg->msg_id, p_user_msg->data, p_user_msg->len);
        break;

    default:
        break;
    }
}

static btstack_packet_callback_registration_t hci_event_callback_registration;

uint32_t setup_profile(void *data, void *user_data)
{
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}

