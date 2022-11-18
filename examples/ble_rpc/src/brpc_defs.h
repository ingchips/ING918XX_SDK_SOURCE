#pragma once

#define BRPC_VERSION        0x1

#define STATUS_OK           0
#define STATUS_ERR          1

#define RPC_EVT_READY       0
#define RPC_EVT_HCI         1
#define RPC_EVT_ATT_READ    2
#define RPC_EVT_ATT_WRITE   3
#define RPC_EVT_GATT        4
#define RPC_EVT_SM          5
#define RPC_EVT_SM_OOB_REQ  6
#define RPC_EVT_CALLBACK    7
#define RPC_EVT_MEMCPY      8
#define RPC_EVT_UNDEF       9
#define RPC_EVT_PLATFORM    10

#define RPC_FRAME_CALL      0
#define RPC_FRAME_RET       1
#define RPC_FRAME_EVT       2
#define RPC_FRAME_OOB       4

#define RPC_CB_HCI_CMD_COMPLETE     0
#define RPC_CB_HCI_PACKET_HANDLER   1
#define RPC_CB_PLATFORM_IRQ         2
#define RPC_CB_VOID_VOID            3

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)

typedef void (*f_void_void)(void);

struct rpc_event_ready_frame
{
    uint8_t brpc_ver;
    uint8_t chip_family;
    platform_ver_t ver;
};

struct rpc_event_hci_frame
{

    uint8_t packet[0];
};

struct rpc_event_att_read_frame
{
    uint16_t conn_handle;
    uint16_t att_handle;
};

struct rpc_event_att_write_frame
{
    uint16_t conn_handle;
    uint16_t att_handle;
    uint16_t transaction_mode;
    uint16_t offset;
    uint8_t buffer[0];
};

struct rpc_event_gatt_frame
{
    uint16_t conn_handle;
    uint8_t packet_type;
    uint8_t packet[0];
};

struct rpc_event_callback_frame
{
    uint8_t type;
    void *cb;
    uint8_t packet[0];
};

struct rpc_event_memcpy_frame
{
    void *dest;
    uint8_t data[0];
};

struct rpc_event_undef_fun_frame
{
    uint16_t fun;
};

struct rpc_event_platform_frame
{
    uint8_t type;
    uint8_t packet[0];
};

struct rpc_call_frame
{
    uint16_t fun;
    uint8_t body[0];
};

struct rpc_ret_frame
{
    uint16_t fun;
    uint8_t body[0];
};

struct rpc_event_frame
{
    uint8_t packet_type;
    union
    {
        struct rpc_event_hci_frame hci;
        struct rpc_event_att_read_frame att_read;
        struct rpc_event_att_write_frame att_write;
        struct rpc_event_gatt_frame gatt;
        struct rpc_event_ready_frame ready;
        struct rpc_event_callback_frame callback;
        struct rpc_event_memcpy_frame memcpy;
        struct rpc_event_undef_fun_frame undef;
        struct rpc_event_platform_frame platform;
        uint8_t body[0];
    };
};

struct uart_frame
{
    uint16_t len; // type included
    uint8_t type;

    union
    {
        struct rpc_call_frame call;
        struct rpc_ret_frame ret;
        struct rpc_event_frame evt;
        uint8_t body[0];
    };
};

struct frame_long
{
    struct uart_frame frame;
    uint8_t body[255];
};

struct frame_split
{
    int rx_cnt;
    struct frame_long full_frame;
} frame_split = {0};

#define sizeof_item_in_frame(frame) (frame->len - 1)
#define sizeof_event_item_in_frame(frame) (frame->len - 2)

#pragma pack(pop)

#define ID_sys_mem_map_add              1
#define ID_sys_persist_mem_new          2
#define ID_sys_persist_mem_append       3
#define ID_sys_persist_mem_free         4
#define ID_sys_alloc_heap_for_conn      5
#define ID_sys_enable_irq               6
#define ID_sys_get_heap_status          7
#define ID_sys_shutdown                 8
#define ID_sys_hrng                     9
#define ID_sys_set_timer                10

#define ID_att_set_db                   0x30
#define ID_gatt_client_get_mtu          0x31
#define ID_gap_aes_encrypt              0x32
#define ID_sm_private_random_address_generation_get              0x33

#ifdef __cplusplus
}
#endif
