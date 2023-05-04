#include "brpc.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "platform_api.h"
#include "ll_api.h"
#include "uart_driver.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include "gatt_client.h"
#include "sm.h"

#include "port_gen_os_driver.h"

#include "str_util.h"
#include "bcp.h"
#include "brpc_defs.h"

#define GEN_OS()      ((struct gen_os_driver *)platform_get_gen_os_driver())

#ifndef EVT_FILTER_SIZE
#define EVT_FILTER_SIZE 10
#endif

struct
{
    int cnt;
    uint32_t codes[EVT_FILTER_SIZE];
} event_filter = {0};


static struct sys_data_buffer
{
    int len;
    uint8_t buffer[2000];
} sys_data_buffer = {0};

struct sys_mem_map
{
    void *addr;
    int size;
    uint8_t buf[255];
};

struct sys_persist_mem_map
{
    void *remote_addr;
    void *local_addr;
    int local_written;
};

#define MAX_MEM_MAPS_NUM 5
#define MAX_PERSIST_MEM_MAPS_NUM 5

static struct sys_mem_map mem_maps[MAX_MEM_MAPS_NUM] = {0};
static struct sys_persist_mem_map persist_mem_maps[MAX_PERSIST_MEM_MAPS_NUM] = {0};

struct conn_mem_block
{
    struct conn_mem_block *next;
    const void *remote;
    uint16_t conn_handle;
    uint8_t buf[0];
};

struct conn_mem_block first_mem_block = {0};

void sys_mem_map_add(void *addr, uint16_t size, const void *buf)
{
    struct sys_mem_map *map = NULL;
    int i;
    for (i = 0; i < MAX_MEM_MAPS_NUM; i++)
    {
        if (NULL == mem_maps[i].addr)
        {
            map = &mem_maps[i];
            break;
        }
    }
    if ((map == NULL) || (size > sizeof(map->buf)))
        platform_raise_assertion(__FILE__, __LINE__);
    map->addr = addr;
    map->size = size;
    memcpy(map->buf, buf, size);
}

void sys_mem_map_clear()
{
    int i;
    for (i = 0; i < MAX_MEM_MAPS_NUM; i++) mem_maps[i].addr = NULL;
}

void sys_mem_new_persistent(void *addr, uint16_t total_size, uint16_t size, const void *buf)
{
    int i;
    struct sys_persist_mem_map *map = NULL;
    for (i = 0; i < MAX_MEM_MAPS_NUM; i++)
    {
        if (NULL == persist_mem_maps[i].remote_addr)
        {
            map = &persist_mem_maps[i];
            break;
        }
    }
    if (map == NULL)
        platform_raise_assertion(__FILE__, __LINE__);
    persist_mem_maps[i].local_addr = GEN_OS()->malloc(total_size);
    if (persist_mem_maps[i].local_addr == NULL)
        platform_raise_assertion(__FILE__, __LINE__);
    persist_mem_maps[i].remote_addr = addr;
    memcpy(persist_mem_maps[i].local_addr, buf, size);
    persist_mem_maps[i].local_written = size;
}

void sys_mem_append_persistent(void *addr, uint16_t size, const void *buf)
{
    int i;
    struct sys_persist_mem_map *map = NULL;
    for (i = 0; i < MAX_MEM_MAPS_NUM; i++)
    {
        if (addr == persist_mem_maps[i].remote_addr)
        {
            map = &persist_mem_maps[i];
            break;
        }
    }
    if (map == NULL)
        platform_raise_assertion(__FILE__, __LINE__);

    memcpy((uint8_t *)(persist_mem_maps[i].local_addr) + persist_mem_maps[i].local_written,
        buf, size);
    persist_mem_maps[i].local_written += size;
}

void sys_mem_free_persistent(void *addr)
{
    int i;
    struct sys_persist_mem_map *map = NULL;
    for (i = 0; i < MAX_MEM_MAPS_NUM; i++)
    {
        if (addr == persist_mem_maps[i].remote_addr)
        {
            map = &persist_mem_maps[i];
            break;
        }
    }
    if (map == NULL)
        platform_raise_assertion(__FILE__, __LINE__);

    GEN_OS()->free(map->local_addr);
    persist_mem_maps[i].remote_addr = NULL;
}

void *sys_mem_map(const void *addr)
{
    if (addr == NULL) return NULL;
    int i;
    for (i = 0; i < MAX_MEM_MAPS_NUM; i++)
    {
        if (mem_maps[i].addr == addr)
            return mem_maps[i].buf;
    }
    for (i = 0; i < MAX_MEM_MAPS_NUM; i++)
    {
        if (persist_mem_maps[i].remote_addr == addr)
            return persist_mem_maps[i].local_addr;
    }

    struct conn_mem_block *prev = &first_mem_block;
    while (prev->next)
    {
        struct conn_mem_block *t = prev->next;
        if (t->remote == addr)
            return t->buf;
        prev = prev->next;
    }
    platform_raise_assertion(__FILE__, __LINE__);
    return NULL;
}

uint8_t sys_append_buffer(const uint8_t *body)
{
    if (sys_data_buffer.len + body[0] <= sizeof(sys_data_buffer.buffer))
    {
        memcpy(sys_data_buffer.buffer + sys_data_buffer.len, body + 1, body[0]);
        sys_data_buffer.len += body[0];
        return STATUS_OK;
    }
    else
        return STATUS_ERR;
}

void sys_clear_buffer()
{
    sys_data_buffer.len = 0;
}

uint8_t sys_evt_filter_add(uint32_t code)
{
    if (event_filter.cnt < EVT_FILTER_SIZE)
    {
        event_filter.codes[event_filter.cnt++] = code;
        return STATUS_OK;
    }
    else
        return STATUS_ERR;
}

void sys_evt_filter_remove(uint32_t code)
{
    int i;
    for (i = 0; i < event_filter.cnt; i++)
    {
        if (event_filter.codes[i] == code)
        {
            memmove(&event_filter.codes[i],
                    &event_filter.codes[i + 1],
                    (event_filter.cnt - i - 1) * sizeof(event_filter.codes[0]));
            event_filter.cnt--;
            break;
        }
    }
}

void sys_evt_filter_clear()
{
    event_filter.cnt = 0;
}

static void dump_msg(struct uart_frame *msg)
{
    platform_printf("MSG: %02X\n", msg->type);
    print_hex_table(msg->body, msg->len - 1, (f_print_string)puts);
}

void post_msg_to_host(void *msg);

void brpc_rx_byte(void *user_data, uint8_t c)
{
    uint8_t *p = (uint8_t *)&frame_split.full_frame;
    p[frame_split.rx_cnt++] = c;
    if (frame_split.rx_cnt < sizeof(uint16_t)) return;

    int total = 2 + frame_split.full_frame.frame.len;

    if (frame_split.rx_cnt < total) return;

    p = malloc(total);
    if (NULL == p) platform_raise_assertion(__FILE__, __LINE__);

    memcpy(p, &frame_split.full_frame, total);
    post_msg_to_host(p);

    frame_split.rx_cnt = 0;
}

void send_generic_response(uint8_t type, int len1, const void *d1,
    int len2, const void *d2)
{
    struct frame_long full = {0};

    uint16_t total_size = len1 + len2 + 1 + 2;
    if (total_size >= sizeof(full.body)) platform_raise_assertion(__FILE__, __LINE__);

    full.frame.len = len1 + len2 + 1;
    full.frame.type = type;
    if (len1) memcpy(full.frame.body, d1, len1);
    if (len2) memcpy(full.frame.body + len1, d2, len2);

    driver_append_tx_data(&full, total_size);
}

void send_ret_response(uint16_t fun, int len, void *ret)
{
    struct rpc_ret_frame h =
    {
        .fun = fun
    };
    send_generic_response(RPC_FRAME_RET, sizeof(h), &h, len, ret);
}

void send_ret_u8_response(uint16_t fun, uint8_t ret)
{
    send_ret_response(fun, 1, &ret);
}

void send_undef_fun_event(uint16_t fun_id)
{
    struct rpc_event_frame h =
    {
        .packet_type = RPC_EVT_UNDEF,
        .undef =
        {
            .fun = fun_id,
        }
    };

    send_generic_response(RPC_FRAME_EVT, 1 + sizeof(struct rpc_event_undef_fun_frame), &h, 0, NULL);
}

void send_memcpy_event(void *dest, int len, void *data)
{
    struct rpc_event_frame h =
    {
        .packet_type = RPC_EVT_MEMCPY,
        .memcpy =
        {
            .dest = dest,
        }
    };

    send_generic_response(RPC_FRAME_EVT, 1 + sizeof(struct rpc_event_memcpy_frame), &h, len, data);
}

void send_callback_event(uint8_t type, void *cb, void *param, int len)
{
    struct rpc_event_frame h =
    {
        .packet_type = RPC_EVT_CALLBACK,
        .callback =
        {
            .type = type,
            .cb = cb
        }
    };
    send_generic_response(RPC_FRAME_EVT, 1 + sizeof(struct rpc_event_callback_frame), &h, len, param);
}

void send_platform_event(uint8_t type, void *param, int len)
{
    struct rpc_event_frame h =
    {
        .packet_type = RPC_EVT_PLATFORM,
        .platform =
        {
            .type = type,
        }
    };
    send_generic_response(RPC_FRAME_EVT, 1 + sizeof(struct rpc_event_platform_frame), &h, len, param);
}

void send_hci_event(const void *packet, int len)
{
    struct rpc_event_frame h =
    {
        .packet_type = RPC_EVT_HCI
    };
    send_generic_response(RPC_FRAME_EVT, 1, &h, len, packet);
}

void send_sm_event(const void *packet, int len)
{
    struct rpc_event_frame h =
    {
        .packet_type = RPC_EVT_SM,
    };
    send_generic_response(RPC_FRAME_EVT, 1, &h, len, packet);
}

void send_att_read_event(uint16_t conn_handle, uint16_t att_handle)
{
    struct rpc_event_frame h =
    {
        .packet_type = RPC_EVT_ATT_READ,
        .att_read =
        {
            .conn_handle = conn_handle,
            .att_handle = att_handle
        }
    };
    send_generic_response(RPC_FRAME_EVT, 1 + sizeof(struct rpc_event_att_read_frame), &h, 0, NULL);
}

void send_att_write_event(uint16_t conn_handle, uint16_t att_handle,
    uint16_t transaction_mode,
    uint16_t offset,
    const void *buffer, int len)
{
    struct rpc_event_frame h =
    {
        .packet_type = RPC_EVT_ATT_WRITE,
        .att_write =
        {
            .conn_handle = conn_handle,
            .att_handle = att_handle,
            .transaction_mode= transaction_mode,
            .offset = offset,
        }
    };
    send_generic_response(RPC_FRAME_EVT, 1 + sizeof(struct rpc_event_att_write_frame), &h, len, buffer);
}

void send_gatt_event(uint8_t packet_type, uint16_t conn_handle, const void *packet, int len)
{
    struct rpc_event_frame h =
    {
        .packet_type = RPC_EVT_GATT,
        .gatt =
        {
            .conn_handle = conn_handle,
            .packet_type= packet_type,
        }
    };
    send_generic_response(RPC_FRAME_EVT, 1 + sizeof(struct rpc_event_gatt_frame), &h, len, packet);
}

void send_ready(void)
{
    struct rpc_event_frame h =
    {
        .packet_type = RPC_EVT_READY,
        .ready =
        {
            .brpc_ver = BRPC_VERSION,
            .ver = *platform_get_version(),
            .chip_family = INGCHIPS_FAMILY
        }
    };
    send_generic_response(RPC_FRAME_EVT, 1 + sizeof(struct rpc_event_ready_frame), &h, 0, NULL);
}

void brpc_init(void)
{
    send_ready();
}

#define MAX_HCI_COMPLETE_CB 10
#define MAX_STACK_PACKET_HANDLE 20

struct hci_cmd_complate_info
{
    gap_hci_cmd_complete_cb_t cb;
    void * user_data;
} cmd_complate_cbs[MAX_HCI_COMPLETE_CB] = {0};

static void alloc_heap_for_conn(const void *remote, int size, uint16_t conn_handle)
{
    struct conn_mem_block * r = (struct conn_mem_block *)GEN_OS()->malloc(sizeof(struct conn_mem_block) + size);
    r->next = NULL;
    r->remote = remote;
    r->conn_handle = conn_handle;
    struct conn_mem_block *prev = &first_mem_block;
    while (prev->next) prev = prev->next;
    prev->next = r;
}

int cb_complete_add(gap_hci_cmd_complete_cb_t cb, void * user_data)
{
    int i;
    for (i = 0; i < MAX_HCI_COMPLETE_CB; i++)
    {
        if (cmd_complate_cbs[i].cb == NULL)
        {
            cmd_complate_cbs[i].cb = cb;
            cmd_complate_cbs[i].user_data = user_data;
            return 0;
        }
    }
    return -1;
}

static void btstack_packet_handler_n(int n, uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size);

static void btstack_packet_handler_0(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size){ btstack_packet_handler_n(0, packet_type, channel, packet, size);}
static void btstack_packet_handler_1(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size){ btstack_packet_handler_n(1, packet_type, channel, packet, size);}
static void btstack_packet_handler_2(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size){ btstack_packet_handler_n(2, packet_type, channel, packet, size);}
static void btstack_packet_handler_3(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size){ btstack_packet_handler_n(3, packet_type, channel, packet, size);}
static void btstack_packet_handler_4(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size){ btstack_packet_handler_n(4, packet_type, channel, packet, size);}
static void btstack_packet_handler_5(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size){ btstack_packet_handler_n(5, packet_type, channel, packet, size);}
static void btstack_packet_handler_6(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size){ btstack_packet_handler_n(6, packet_type, channel, packet, size);}
static void btstack_packet_handler_7(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size){ btstack_packet_handler_n(7, packet_type, channel, packet, size);}
static void btstack_packet_handler_8(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size){ btstack_packet_handler_n(8, packet_type, channel, packet, size);}
static void btstack_packet_handler_9(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size){ btstack_packet_handler_n(9, packet_type, channel, packet, size);}
static void btstack_packet_handler_10(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size){ btstack_packet_handler_n(10, packet_type, channel, packet, size);}
static void btstack_packet_handler_11(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size){ btstack_packet_handler_n(11, packet_type, channel, packet, size);}
static void btstack_packet_handler_12(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size){ btstack_packet_handler_n(12, packet_type, channel, packet, size);}
static void btstack_packet_handler_13(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size){ btstack_packet_handler_n(13, packet_type, channel, packet, size);}
static void btstack_packet_handler_14(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size){ btstack_packet_handler_n(14, packet_type, channel, packet, size);}
static void btstack_packet_handler_15(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size){ btstack_packet_handler_n(15, packet_type, channel, packet, size);}
static void btstack_packet_handler_16(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size){ btstack_packet_handler_n(16, packet_type, channel, packet, size);}
static void btstack_packet_handler_17(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size){ btstack_packet_handler_n(17, packet_type, channel, packet, size);}
static void btstack_packet_handler_18(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size){ btstack_packet_handler_n(18, packet_type, channel, packet, size);}
static void btstack_packet_handler_19(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size){ btstack_packet_handler_n(19, packet_type, channel, packet, size);}

struct hci_packet_handler_info
{
    btstack_packet_handler_t local;
    btstack_packet_handler_t handler;
    uint16_t conn_handle;
} packet_handlers[MAX_STACK_PACKET_HANDLE] = {
    {.local = btstack_packet_handler_0},
    {.local = btstack_packet_handler_1},
    {.local = btstack_packet_handler_2},
    {.local = btstack_packet_handler_3},
    {.local = btstack_packet_handler_4},
    {.local = btstack_packet_handler_5},
    {.local = btstack_packet_handler_6},
    {.local = btstack_packet_handler_7},
    {.local = btstack_packet_handler_8},
    {.local = btstack_packet_handler_9},
    {.local = btstack_packet_handler_10},
    {.local = btstack_packet_handler_11},
    {.local = btstack_packet_handler_12},
    {.local = btstack_packet_handler_13},
    {.local = btstack_packet_handler_14},
    {.local = btstack_packet_handler_15},
    {.local = btstack_packet_handler_16},
    {.local = btstack_packet_handler_17},
    {.local = btstack_packet_handler_18},
    {.local = btstack_packet_handler_19},
};

static void platform_timer_handler_n(int n);
static void platform_timer_handler_0(void){ platform_timer_handler_n(0);}
static void platform_timer_handler_1(void){ platform_timer_handler_n(1);}
static void platform_timer_handler_2(void){ platform_timer_handler_n(2);}
static void platform_timer_handler_3(void){ platform_timer_handler_n(3);}
static void platform_timer_handler_4(void){ platform_timer_handler_n(4);}
static void platform_timer_handler_5(void){ platform_timer_handler_n(5);}
static void platform_timer_handler_6(void){ platform_timer_handler_n(6);}
static void platform_timer_handler_7(void){ platform_timer_handler_n(7);}
static void platform_timer_handler_8(void){ platform_timer_handler_n(8);}
static void platform_timer_handler_9(void){ platform_timer_handler_n(9);}
static void platform_timer_handler_10(void){ platform_timer_handler_n(10);}
static void platform_timer_handler_11(void){ platform_timer_handler_n(11);}
static void platform_timer_handler_12(void){ platform_timer_handler_n(12);}
static void platform_timer_handler_13(void){ platform_timer_handler_n(13);}
static void platform_timer_handler_14(void){ platform_timer_handler_n(14);}
static void platform_timer_handler_15(void){ platform_timer_handler_n(15);}
static void platform_timer_handler_16(void){ platform_timer_handler_n(16);}
static void platform_timer_handler_17(void){ platform_timer_handler_n(17);}
static void platform_timer_handler_18(void){ platform_timer_handler_n(18);}
static void platform_timer_handler_19(void){ platform_timer_handler_n(19);}

struct platform_timer_cb_info
{
    void (*local)(void);
    void *handler;
} platform_timer_handlers[MAX_STACK_PACKET_HANDLE] = {
    {.local = platform_timer_handler_0},
    {.local = platform_timer_handler_1},
    {.local = platform_timer_handler_2},
    {.local = platform_timer_handler_3},
    {.local = platform_timer_handler_4},
    {.local = platform_timer_handler_5},
    {.local = platform_timer_handler_6},
    {.local = platform_timer_handler_7},
    {.local = platform_timer_handler_8},
    {.local = platform_timer_handler_9},
    {.local = platform_timer_handler_10},
    {.local = platform_timer_handler_11},
    {.local = platform_timer_handler_12},
    {.local = platform_timer_handler_13},
    {.local = platform_timer_handler_14},
    {.local = platform_timer_handler_15},
    {.local = platform_timer_handler_16},
    {.local = platform_timer_handler_17},
    {.local = platform_timer_handler_18},
    {.local = platform_timer_handler_19},
};

static void platform_timer_handler_n(int n)
{
    struct platform_timer_cb_info *handler = &platform_timer_handlers[n];
    send_callback_event(RPC_CB_VOID_VOID, handler->handler, NULL, 0);
    handler->handler = NULL;
}

static f_void_void sys_fun_map_void_void(void *remote)
{
    int i;
    for (i = 0; i < MAX_STACK_PACKET_HANDLE; i++)
    {
        if (platform_timer_handlers[i].handler == NULL)
        {
            platform_timer_handlers[i].handler = remote;
            return platform_timer_handlers[i].local;
        }
    }
    platform_raise_assertion(__FILE__, __LINE__);
    return NULL;
}

void sys_clear_fun_map_void_void(void *remote)
{
    int i;
    for (i = 0; i < MAX_STACK_PACKET_HANDLE; i++)
    {
        if (platform_timer_handlers[i].handler == remote)
            platform_timer_handlers[i].handler = NULL;
    }
}

static void btstack_packet_handler_n(int n, uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    struct hci_packet_handler_info *handler = &packet_handlers[n];
    #pragma pack (push, 1)
    struct _param
    {
        uint8_t packet_type;
        uint16_t channel;
        uint8_t packet[0];
    } * _param = (struct _param *)GEN_OS()->malloc(sizeof(*_param) + size);
    #pragma pack (pop)
    _param->packet_type = packet_type;
    _param->channel = channel;
    memcpy(_param->packet, packet, size);
    send_callback_event(RPC_CB_HCI_PACKET_HANDLER, handler->handler, _param, sizeof(*_param) + size);
    GEN_OS()->free(_param);
}

static btstack_packet_handler_t sys_fun_map(btstack_packet_handler_t remote, uint16_t conn_handle)
{
    int i;
    for (i = 0; i < MAX_STACK_PACKET_HANDLE; i++)
    {
        if (packet_handlers[i].handler == NULL)
        {
            packet_handlers[i].handler = remote;
            packet_handlers[i].conn_handle = conn_handle;
            return packet_handlers[i].local;
        }
    }
    platform_raise_assertion(__FILE__, __LINE__);
    return NULL;
}

void cleanup_packet_handlers(uint16_t conn_handle)
{
    int i;
    for (i = 0; i < MAX_STACK_PACKET_HANDLE; i++)
    {
        if (packet_handlers[i].conn_handle == conn_handle)
        {
            packet_handlers[i].handler = NULL;
            packet_handlers[i].conn_handle = 0xffff;
        }
    }

    struct conn_mem_block *prev = &first_mem_block;
    while (prev->next)
    {
        struct conn_mem_block *t = prev->next;
        if (t->conn_handle == conn_handle)
        {
            prev->next = t->next;
            GEN_OS()->free(t);
            continue;
        }
        prev = prev->next;
    }
}

struct hci_cmd_complate_info *find_cb(void * user_data)
{
    int i;
    for (i = 0; i < MAX_HCI_COMPLETE_CB; i++)
    {
        if (cmd_complate_cbs[i].cb == NULL) continue;
        if (cmd_complate_cbs[i].user_data == user_data)
            return &cmd_complate_cbs[i];
    }
    return NULL;
}

static uint32_t irq_cb(void *user_data)
{
    int id = (int)(intptr_t)user_data;
    send_callback_event(RPC_CB_PLATFORM_IRQ, (void *)id, NULL, 0);
    return 0;
}

void hci_aes_cmd_complete_cb(const uint8_t *return_params, void *user_data)
{
    int index = (int)user_data;
    if ((0 > index) || (index >= MAX_HCI_COMPLETE_CB))
        platform_raise_assertion(__FILE__, __LINE__);

    struct hci_cmd_complate_info *cb = cmd_complate_cbs + index;

    #pragma pack (push, 1)
    struct _ret
    {
        void *user_data;
        uint8_t return_params[17];
    } _ret;
    #pragma pack (pop)
    _ret.user_data = user_data;
    memcpy(_ret.return_params, return_params, sizeof(_ret.return_params));
    send_callback_event(RPC_CB_HCI_CMD_COMPLETE, cb->cb, &_ret, sizeof(_ret));

    cb->cb = NULL;
}

#define UNDEF_FUN(id)       do { send_undef_fun_event(id); } while (0)

#include "../data/brpc_handlers_weak.inc"

static void handle_call(struct rpc_call_frame *call, int len)
{
    len -= 2;

    switch (call->fun)
    {
    case ID_sys_mem_map_add:
        {
            #pragma pack (push, 1)
            struct param
            {
                void *addr;
                uint8_t buf[0];
            } *param = (struct param *)call->body;
            #pragma pack (pop)
            int size = len - sizeof(*param);
            sys_mem_map_add(param->addr, size, param->buf);
        }
        return;

    case ID_sys_persist_mem_new:
        {
            #pragma pack (push, 1)
            struct param
            {
                void *addr;
                uint16_t total_len;
                uint16_t len;
                uint8_t buf[0];
            } *param = (struct param *)call->body;
            #pragma pack (pop)
            sys_mem_new_persistent(param->addr, param->total_len, param->len, param->buf);
        }
        return;

    case ID_sys_persist_mem_append:
        {
            #pragma pack (push, 1)
            struct param
            {
                void *addr;
                uint16_t len;
                uint8_t buf[0];
            } *param = (struct param *)call->body;
            #pragma pack (pop)
            sys_mem_append_persistent(param->addr, param->len, param->buf);
        }
        return;

    case ID_sys_persist_mem_free:
        {
            #pragma pack (push, 1)
            struct param
            {
                void *addr;
            } *param = (struct param *)call->body;
            #pragma pack (pop)
            sys_mem_free_persistent(param->addr);
        }
        return;

    case ID_sys_alloc_heap_for_conn:
        {
            #pragma pack (push, 1)
            struct param
            {
                const void *addr;
                uint16_t size;
                uint16_t conn_handle;
            } *param = (struct param *)call->body;
            #pragma pack (pop)
            alloc_heap_for_conn(param->addr, param->size, param->conn_handle);
        }
        return;

    case ID_sys_enable_irq:
        {
            #pragma pack (push, 1)
            struct _param
            {
                int irq_n;
            } *_param = (struct _param *)call->body;
            #pragma pack (pop)
            platform_set_irq_callback((platform_irq_callback_type_t)_param->irq_n, irq_cb, (void*)(intptr_t)_param->irq_n);
        }
        return;

    case ID_sys_get_heap_status:
        {
            #pragma pack (push, 1)
            struct _ret
            {
                platform_heap_status_t status;
            } _ret;
            #pragma pack (pop)
            platform_get_heap_status(&_ret.status);
            send_ret_response(ID_sys_get_heap_status, sizeof(_ret), &_ret);
        }
        return;

    case ID_sys_shutdown:
        {
            #pragma pack (push, 1)
            struct _param
            {
                uint32_t duration_cycles;
                const void *p_retention_data;
                uint32_t data_size;
            } *_param = (struct _param *)call->body;
            #pragma pack (pop)
            platform_shutdown(_param->duration_cycles, _param->p_retention_data, _param->data_size);
        }
        return;

    case ID_sys_hrng:
        {
            #pragma pack (push, 1)
            struct _param
            {
                uint32_t len;
            } *_param = (struct _param *)call->body;
            #pragma pack (pop)
            uint8_t *b = GEN_OS()->malloc(_param->len);
            platform_hrng(b, _param->len);
            send_ret_response(ID_sys_hrng, _param->len, b);
            GEN_OS()->free(b);
        }
        return;

    case ID_sys_set_timer:
        {
            #pragma pack (push, 1)
            struct _param
            {
                void (* callback)(void);
                uint32_t delay;
            } *_param = (struct _param *)call->body;
            #pragma pack (pop)
            if (_param->delay > 0)
                platform_set_timer(sys_fun_map_void_void(_param->callback), _param->delay);
            else
                sys_clear_fun_map_void_void(_param->callback);
        }
        return;

    case ID_sys_aligned_read_mem:
        {
            #pragma pack (push, 1)
            struct _param
            {
                uint32_t addr;
                int      len;
            } *_param = (struct _param *)call->body;
            #pragma pack (pop)
            uint32_t *b = (uint32_t *)GEN_OS()->malloc(_param->len);
            uint32_t *p = (uint32_t *)(uintptr_t)_param->addr;
            int i;
            for (i = 0; i < _param->len / sizeof(uint32_t); i++)
                b[i] = p[i];
            send_ret_response(ID_sys_aligned_read_mem, _param->len, b);
            GEN_OS()->free(b);
        }
        return;

    case ID_sys_aligned_write_mem:
        {
            #pragma pack (push, 1)
            struct _param
            {
                uint32_t addr;
                uint32_t data[0];
            } *_param = (struct _param *)call->body;
            #pragma pack (pop)
            len -= sizeof(_param->addr);
            uint32_t *p = (uint32_t *)(uintptr_t)_param->addr;
            int i;
            for (i = 0; i < len / sizeof(uint32_t); i++)
                p[i] = _param->data[i];
        }
        return;

    case ID_att_set_db:
        {
            #pragma pack (push, 1)
            struct _param
            {
                hci_con_handle_t con_handle;
                const uint8_t *db;
            } *_param = (struct _param *)call->body;
            #pragma pack (pop)
            att_set_db(_param->con_handle, (const uint8_t *)sys_mem_map(_param->db));
        }

        return;

    case ID_gatt_client_get_mtu:
        {
            #pragma pack (push, 1)
            struct _param
            {
                hci_con_handle_t con_handle;
            } *_param = (struct _param *)call->body;
            struct _ret
            {
                uint8_t r;
                uint16_t mtu;
            } _ret;
            #pragma pack (pop)
            _ret.r = gatt_client_get_mtu(_param->con_handle, &_ret.mtu);
            send_ret_response(ID_gatt_client_get_mtu, sizeof(_ret), &_ret);
        }
        return;

    case ID_gap_aes_encrypt:
        {
            #pragma pack (push, 1)
            struct _param
            {
                uint8_t key[16];
                uint8_t plain[16];
                void *cb;
                void *user_data;
            } *_param = (struct _param *)call->body;
            #pragma pack (pop)
            int index = cb_complete_add(_param->cb, _param->user_data);
            gap_aes_encrypt(_param->key, _param->plain, hci_aes_cmd_complete_cb, (void *)index);
        }
        return;

    case ID_sm_private_random_address_generation_get:
        {
            #pragma pack (push, 1)
            struct _ret
            {
                bd_addr_t addr;
            } _ret;
            #pragma pack (pop)
            memcpy(_ret.addr, sm_private_random_address_generation_get(), BD_ADDR_LEN);
            send_ret_response(ID_sm_private_random_address_generation_get, sizeof(_ret), &_ret);
        }
        return;

    #include "../data/brpc_handlers.inc"

    default:
        send_undef_fun_event(call->fun);
        break;
    }

    sys_mem_map_clear();
}

static void handle_msg(void *msg, uint16_t len)
{
    struct uart_frame *frame = (struct uart_frame *)msg;
    switch (frame->type)
    {
    case RPC_FRAME_CALL:
        handle_call(&frame->call, frame->len - 1);
        break;

    default:
        break;
    }
    free(msg);
}

void post_msg_to_host(void *msg)
{
    btstack_push_user_runnable(handle_msg, msg, 0);
}

uint32_t cb_hard_fault(hard_fault_info_t *info, void *_)
{
    send_platform_event(PLATFORM_CB_EVT_HARD_FAULT, info, sizeof(*info));
    driver_flush_tx();
    platform_printf("HARDFAULT:\nPC : 0x%08X\nLR : 0x%08X\nPSR: 0x%08X\n"
                    "R0 : 0x%08X\nR1 : 0x%08X\nR2 : 0x%08X\nP3 : 0x%08X\n"
                    "R12: 0x%08X\n",
                    info->pc, info->lr, info->psr,
                    info->r0, info->r1, info->r2, info->r3, info->r12);
    for (;;);
}

uint32_t cb_assertion(assertion_info_t *info, void *_)
{
    send_platform_event(PLATFORM_CB_EVT_ASSERTION, info, sizeof(*info));
    driver_flush_tx();
    platform_printf("[ASSERTION] @ %s:%d\n",
                    info->file_name,
                    info->line_no);
    for (;;);
}

uint32_t cb_heap_out_of_mem(uint32_t tag, void *_)
{
    send_platform_event(PLATFORM_CB_EVT_HEAP_OOM, &tag, sizeof(tag));
    driver_flush_tx();
    platform_printf("[OOM] @ %d\n", tag);
    for (;;);
}

uint32_t cb_exception(uint32_t tag, void *_)
{
    send_platform_event(PLATFORM_CB_EVT_EXCEPTION, &tag, sizeof(tag));
    driver_flush_tx();
    platform_printf("[EXCEPTION] @ %d\n", tag);
    for (;;);
}
