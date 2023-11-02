#include "brpc.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "platform_api.h"
#include "ll_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include "gatt_client.h"
#include "sm.h"
#include "kv_storage.h"
#include "profile.h"

#include "port_gen_os_driver.h"

#include "brpc_defs.h"

#include "log.h"

extern void sleep_ms(uint32_t ms);
platform_task_id_t platform_get_current_task();

#define GEN_OS()      ((struct gen_os_driver *)platform_get_gen_os_driver())

#define RPC_FRAME_INTERNAL_RUNNABLE     0xff

#pragma pack(push, 1)
struct node
{
    struct node *next;
    uint8_t body[0];
};

struct runnable
{
    f_btstack_user_runnable f;
    void *data;
    uint16_t value;
};

#pragma pack(pop)

struct rpc_event_ready_frame ready_info = {0};

void post_msg_to_host(struct node *node);

void brpc_rx_byte(void *user_data, uint8_t c)
{
    uint8_t *p = (uint8_t *)&frame_split.full_frame;
    p[frame_split.rx_cnt++] = c;
    if (frame_split.rx_cnt < sizeof(uint16_t)) return;

    int total = 2 + frame_split.full_frame.frame.len;

    if (frame_split.rx_cnt < total) return;

    struct node *pp = (struct node *)GEN_OS()->malloc(total + sizeof(struct node));

    memcpy(pp->body, &frame_split.full_frame, total);
    post_msg_to_host(pp);

    frame_split.rx_cnt = 0;
}

static btstack_packet_callback_registration_t * hci_callback_chain = NULL;
static btstack_packet_callback_registration_t * sm_callback_chain = NULL;
static btstack_packet_handler_t gatt_callback = NULL;
static att_read_callback_t read_callback = {0};
static att_write_callback_t write_callback = {0};

void att_server_init(att_read_callback_t read_cb, att_write_callback_t write_cb)
{
    read_callback = read_cb;
    write_callback = write_cb;
}

void att_server_register_packet_handler(btstack_packet_handler_t handler)
{

}

void hci_add_event_handler(btstack_packet_callback_registration_t * callback_handler)
{
    callback_handler->item.next = NULL;
    if (hci_callback_chain == NULL)
    {
        hci_callback_chain = callback_handler;
        return;
    }
    struct btstack_linked_item *prev = (struct btstack_linked_item *)hci_callback_chain;
    while (prev->next) prev = prev->next;
    prev->next = (struct btstack_linked_item *)callback_handler;
}

void sm_add_event_handler(btstack_packet_callback_registration_t * callback_handler)
{
    callback_handler->item.next = NULL;
    if (sm_callback_chain == NULL)
    {
        sm_callback_chain = callback_handler;
        return;
    }
    struct btstack_linked_item *prev = (struct btstack_linked_item *)sm_callback_chain;
    while (prev->next) prev = prev->next;
    prev->next = (struct btstack_linked_item *)callback_handler;
}

static void broadcast_stack_event(struct btstack_linked_item *cur, uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    while (cur)
    {
        btstack_packet_callback_registration_t *cb = (btstack_packet_callback_registration_t *)cur;
        cb->callback(packet_type, channel, packet, size);
        cur = cur->next;
    }
}

static void broadcast_sm_event(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    struct btstack_linked_item *cur = (struct btstack_linked_item *)sm_callback_chain;
    broadcast_stack_event(cur, packet_type, channel, packet, size);
}

static void broadcast_hci_event(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    struct btstack_linked_item *cur = (struct btstack_linked_item *)hci_callback_chain;
    broadcast_stack_event(cur, packet_type, channel, packet, size);
}

const char *show_chip_family(int i)
{
    switch (i)
    {
    case 0:
        return "ING918XX";
    case 1:
        return "ING916XX";
    default:
        return "???";
    }
}

const char *get_fun_name(uint16_t fun_id);
static void call_irq(int n);

static void handle_evt(struct rpc_event_frame *evt, int len)
{
    len--;
    switch (evt->packet_type)
    {
    case RPC_EVT_READY:
        {
            struct rpc_event_ready_frame *ready = &evt->ready;
            ready_info = evt->ready;
            if (ready->brpc_ver == BRPC_VERSION)
                LOG_OK("Ready. %s (%d.%d.%d)",
                    show_chip_family(ready->chip_family),
                    ready->ver.major, ready->ver.minor, ready->ver.patch);
            else
            {
                LOG_E("BRPC Version not match: %d vs %d", BRPC_VERSION, ready->brpc_ver);
            }
        }
        break;
    case RPC_EVT_HCI:
        {
            struct rpc_event_hci_frame *hci = &evt->hci;
            broadcast_hci_event(HCI_EVENT_PACKET, 0, hci->packet, len);
        }
        break;
    case RPC_EVT_ATT_READ:
        if (read_callback == 0)
        {
            LOG_E("read_callback is missing!");
            break;
        }
        {
            struct rpc_event_att_read_frame *read =
                (struct rpc_event_att_read_frame *)&evt->att_read;
            uint16_t len = read_callback(read->conn_handle, read->att_handle, 0, NULL, 0);
            if (len == ATT_DEFERRED_READ)
                return;
            uint8_t buffer[len];
            len = read_callback(read->conn_handle, read->att_handle, 0, buffer, len);
            att_server_deferred_read_response(read->conn_handle, read->att_handle, buffer, len);
        }
    case RPC_EVT_ATT_WRITE:
        if (write_callback == 0)
        {
            LOG_E("write_callback is missing!");
            break;
        }
        {
            struct rpc_event_att_write_frame *write =
                (struct rpc_event_att_write_frame *)&evt->att_read;

            int size = len - sizeof(*write);
            write_callback(write->conn_handle, write->att_handle, write->transaction_mode,
                       write->offset, write->buffer, size);
        }
        break;
    case RPC_EVT_GATT:
        {
            if (gatt_callback == NULL)
                break;

            struct rpc_event_gatt_frame *gatt =
                (struct rpc_event_gatt_frame *)&evt->att_read;

            int size = len - sizeof(*gatt);
            gatt_callback(gatt->packet_type, gatt->conn_handle, gatt->packet, size);
        }
        break;
    case RPC_EVT_SM:
        {
            struct rpc_event_hci_frame *hci = &evt->hci;
            broadcast_sm_event(HCI_EVENT_PACKET, 0, hci->packet, len);
        }
        break;
    case RPC_EVT_CALLBACK:
        {
            struct rpc_event_callback_frame *callback =
                (struct rpc_event_callback_frame *)&evt->callback;

            switch (callback->type)
            {
            case RPC_CB_HCI_CMD_COMPLETE:
                {
                    #pragma pack (push, 1)
                    struct _param
                    {
                        void *user_data;
                        uint8_t return_params[0];
                    } *_param = (struct _param *)callback->packet;
                    #pragma pack (pop)
                    gap_hci_cmd_complete_cb_t cb = (gap_hci_cmd_complete_cb_t)(callback->cb);
                    cb(_param->return_params, _param->user_data);
                }
                break;

            case RPC_CB_HCI_PACKET_HANDLER:
                {
                    #pragma pack (push, 1)
                    struct _param
                    {
                        uint8_t packet_type;
                        uint16_t channel;
                        uint8_t packet[0];
                    } * _param = (struct _param *)callback->packet;
                    #pragma pack (pop)
                    int size = len - sizeof(*_param);
                    btstack_packet_handler_t cb = (btstack_packet_handler_t)(callback->cb);
                    cb(_param->packet_type, _param->channel, _param->packet, size);
                }
                break;

            case RPC_CB_PLATFORM_IRQ:
                call_irq((int)callback->cb);
                break;

            case RPC_CB_VOID_VOID:
                {
                    f_void_void f = (f_void_void)callback->cb;
                    if (f) f();
                }
                break;

            default:
                LOG_E("unknown callback type: %d", callback->type);
                break;
            }
        }
        break;
    case RPC_EVT_MEMCPY:
        {
            struct rpc_event_memcpy_frame *copy =
                (struct rpc_event_memcpy_frame *)&evt->memcpy;
            int size = len - sizeof(*copy);
            memcpy(copy->dest, copy->data, size);
        }
        break;

    case RPC_EVT_UNDEF:
        LOG_E("undef API: %s", get_fun_name(evt->undef.fun));
        break;

    case RPC_EVT_PLATFORM:
        {
            struct rpc_event_platform_frame *callback =
                (struct rpc_event_platform_frame *)&evt->platform;
            switch (callback->type)
            {
            case PLATFORM_CB_EVT_HARD_FAULT:
                {
                    const hard_fault_info_t *info = (const hard_fault_info_t *)callback->packet;
                    LOG_E("HARDFAULT:\nPC : 0x%08X\nLR : 0x%08X\nPSR: 0x%08X\n"
                                    "R0 : 0x%08X\nR1 : 0x%08X\nR2 : 0x%08X\nP3 : 0x%08X\n"
                                    "R12: 0x%08X",
                                    info->pc, info->lr, info->psr,
                                    info->r0, info->r1, info->r2, info->r3, info->r12);
                }
                break;
            case PLATFORM_CB_EVT_ASSERTION:
                {
                    #pragma pack (push, 1)
                    struct ret
                    {
                        int line_no;
                        char file_name[1];
                    } *_r;
                    #pragma pack (pop)
                    const struct ret *info = (struct ret *)callback->packet;
                    LOG_E("[ASSERTION] @ %s:%d",
                                    info->file_name,
                                    info->line_no);
                }
                break;
            case PLATFORM_CB_EVT_HEAP_OOM:
                {
                    const uint32_t *tag = (uint32_t *)callback->packet;
                    LOG_E("[OOM] @ %d", *tag);
                }
                break;
            case PLATFORM_CB_EVT_EXCEPTION:
                {
                    const uint32_t *tag = (uint32_t *)callback->packet;
                    LOG_E("[EXCEPTION] @ %d", *tag);
                }
                break;
            default:
                LOG_E("unknown platform evt: %d", callback->type);
                break;
            }
        }
        break;
    default:
        LOG_E("unknown event: %d", evt->packet_type);
        break;
    }
}

void handle_msg(struct uart_frame *frame)
{
    switch (frame->type)
    {
    case RPC_FRAME_RET:
        LOG_E("unhandled RET of %s", get_fun_name(frame->ret.fun));
        break;
    case RPC_FRAME_EVT:
        handle_evt(&frame->evt, frame->len - 1);
        break;
    case RPC_FRAME_INTERNAL_RUNNABLE:
        {
            struct runnable *runnable = (struct runnable *)frame->body;
            runnable->f(runnable->data, runnable->value);
        }
        break;
    default:
        break;
    }
}

static gen_handle_t msg_queue;
static struct node *cached_msg = NULL;

static void cache_msg_node(struct node *node)
{
    GEN_OS()->enter_critical();

    node->next = NULL;
    if (cached_msg == NULL)
    {
        cached_msg = node;
        goto exit;
    }

    struct node *prev = cached_msg;
    while (prev->next) prev = prev->next;
    prev->next = node;

exit:
    GEN_OS()->leave_critical();
}

static int mt_handle_msg_node(struct node *node);

static void brpc_entry(void *data)
{
    struct node *node;
    setup_profile(NULL, NULL);

    LOG_PROG("try to reset remote device");
    platform_reset();

    while (1)
    {
        while (cached_msg)
        {
            GEN_OS()->enter_critical();
            struct node *node = cached_msg;
            cached_msg = node->next;
            GEN_OS()->leave_critical();

            handle_msg((struct uart_frame *)node->body);
            GEN_OS()->free(node);
        }

        if (GEN_OS()->queue_recv_msg(msg_queue, &node) != 0)
        {
            perror("recv_msg:");
            abort();
            continue;
        }

        if (mt_handle_msg_node(node) == 0)
        {
            handle_msg((struct uart_frame *)node->body);
            GEN_OS()->free(node);
        }
    }
}

void post_msg_to_host(struct node *node)
{
    GEN_OS()->queue_send_msg(msg_queue, &node);
}

static gen_handle_t host_task = NULL;

void brpc_init(void)
{
    msg_queue = GEN_OS()->queue_create(20, sizeof(void *));
    host_task = GEN_OS()->task_create("brpc", brpc_entry, NULL, 10240, 0);
}

extern void comm_append_block(int len, const void *data);

void send_generic_response(uint8_t type, int len1, const void *d1,
    int len2, const void *d2)
{
    struct frame_long full = {0};

    uint16_t total_size = len1 + len2 + 1 + 2;
    if (total_size >= sizeof(full.body))
    {
        LOG_E("send_generic_response: buffer too large");
        exit(-1);
    }

    full.frame.len = len1 + len2 + 1;
    full.frame.type = type;
    if (len1) memcpy(full.frame.body, d1, len1);
    if (len2) memcpy(full.frame.body + len1, d2, len2);

    comm_append_block(total_size, &full);
}

static void call_void_fun(int id, void *param, int len)
{
    struct rpc_call_frame call =
    {
        .fun = id
    };
    send_generic_response(RPC_FRAME_CALL, sizeof(call), &call, len, param);
}

void *extract_ret_param(struct node *node, int *param_len)
{
    struct uart_frame *f = (struct uart_frame *)node->body;
    if (param_len) *param_len = f->len - 1 - 2;
    return f->ret.body;
}

static int dirty_mt_call_fun = 0;
struct node *dirty_mt_ret_node = NULL;
static int dirty_mt_fun_id = 0;

static int mt_handle_msg_node(struct node *node)
{
    if (dirty_mt_call_fun == 0) return 0;

    struct uart_frame *f = (struct uart_frame *)node->body;

    if (f->type == RPC_FRAME_EVT)
    {
        int flag = 0;
        switch (f->evt.packet_type)
        {
        case RPC_EVT_MEMCPY:
            flag = 1;
            break;
        case RPC_EVT_PLATFORM:
            flag = 1;
            break;
        }
        if (flag)
        {
            handle_msg(f);
            GEN_OS()->free(node);
            return 1;
        }
    }

    if ((f->type != RPC_FRAME_RET) || (f->ret.fun != dirty_mt_fun_id))
    {
        LOG_D("cache %d, %d", f->type, f->ret.fun);
        cache_msg_node(node);
        return 1;
    }

    dirty_mt_ret_node = node;
    dirty_mt_call_fun = 0;
    return 1;
}

struct node * call_fun_mt(int id, void *param, int len)
{
    struct rpc_call_frame call =
    {
        .fun = id
    };
    dirty_mt_fun_id = id;
    dirty_mt_call_fun = 1;
    send_generic_response(RPC_FRAME_CALL, sizeof(call), &call, len, param);

    while (dirty_mt_call_fun)
    {
        sleep_ms(1);
    }
    return dirty_mt_ret_node;
}

struct node * call_fun(int id, void *param, int len)
{
    if (platform_get_current_task() != PLATFORM_TASK_HOST)
        return call_fun_mt(id, param, len);

    struct rpc_call_frame call =
    {
        .fun = id
    };

    send_generic_response(RPC_FRAME_CALL, sizeof(call), &call, len, param);

    while (1)
    {
        struct node *node = NULL;
        if (GEN_OS()->queue_recv_msg(msg_queue, &node) != 0)
        {
            perror("recv_msg:");
            continue;
        }
        struct uart_frame *f = (struct uart_frame *)node->body;

        if (f->type == RPC_FRAME_EVT)
        {
            int flag = 0;
            switch (f->evt.packet_type)
            {
            case RPC_EVT_MEMCPY:
                flag = 1;
                break;
            case RPC_EVT_PLATFORM:
                flag = 1;
                break;
            }
            if (flag)
            {
                handle_msg(f);
                GEN_OS()->free(node);
                continue;
            }
        }

        if ((f->type != RPC_FRAME_RET) || (f->ret.fun != id))
        {
            cache_msg_node(node);
            continue;
        }

        return node;
    }
}

static void remote_mem_map(const void *local, int len)
{
    if (local == NULL) return;
    #pragma pack (push, 1)
    struct param
    {
        const void *addr;
        uint8_t buf[len];
    } param;
    #pragma pack (pop)
    param.addr = local;
    memcpy(param.buf, local, len);
    call_void_fun(ID_sys_mem_map_add, &param, sizeof(param));
}

static void remote_mem_alloc(const void *local, int len)
{
    int first_len = len <= 255 ? len : 255;
    #pragma pack (push, 1)
    struct param
    {
        const void *addr;
        uint16_t total_len;
        uint16_t len;
        uint8_t buf[first_len];
    } param;
    #pragma pack (pop)
    param.addr = local;
    param.total_len = len;
    param.len = first_len;
    memcpy(param.buf, local, first_len);
    call_void_fun(ID_sys_persist_mem_new, &param, sizeof(param));
    void *p = ((uint8_t *)local) + first_len;
    len -= first_len;
    while (len > 0)
    {
        int block_len = len <= 255 ? len : 255;
        #pragma pack (push, 1)
        struct param
        {
            const void *addr;
            uint16_t len;
            uint8_t buf[block_len];
        } param;
        #pragma pack (pop)
        param.addr = local;
        param.len = block_len;
        memcpy(param.buf, p, first_len);
        call_void_fun(ID_sys_persist_mem_append, &param, sizeof(param));
        len -= block_len;
        p += block_len;
    }
}

static void remote_mem_free(const void *local)
{
    #pragma pack (push, 1)
    struct param
    {
        const void *addr;
    } param;
    #pragma pack (pop)
    call_void_fun(ID_sys_persist_mem_free, &param, sizeof(param));
}

static void alloc_heap_for_conn(void *local, int size, uint16_t conn_handle)
{
    #pragma pack (push, 1)
    struct param
    {
        const void *addr;
        uint16_t size;
        uint16_t conn_handle;
    } param =
    {
        .addr = local,
        .size = (uint16_t)size,
        .conn_handle = conn_handle,
    };
    #pragma pack (pop)
    call_void_fun(ID_sys_alloc_heap_for_conn, &param, sizeof(param));
}

void att_set_db(hci_con_handle_t con_handle, const uint8_t *db)
{
    int total_len = 2;
    const uint8_t *p = db;
    uint16_t size;
    while ((size = *(uint16_t *)p) > 0)
    {
        p += size;
        total_len += size;
    }

    remote_mem_alloc(db, total_len);

    #pragma pack (push, 1)
    struct _param
    {
        hci_con_handle_t con_handle;
        const uint8_t *db;
    } _param =
    {
        .con_handle = con_handle,
        .db = db,
    };
    #pragma pack (pop)
    call_void_fun(ID_att_set_db, &_param, sizeof(_param));
}

void platform_set_evt_callback(platform_evt_callback_type_t type, f_platform_evt_cb f, void *user_data)
{}

struct irq_info
{
    f_platform_irq_cb f;
    void *user_data;
} irq_infos[PLATFORM_CB_IRQ_MAX] = {0};

static void call_irq(int n)
{
    if ((n < 0) || (n >= PLATFORM_CB_IRQ_MAX))
    {
        LOG_W("IRQ (%d) out of range", n);
        return;
    }

    if (irq_infos[n].f)
        irq_infos[n].f(irq_infos[n].user_data);
}

void platform_set_irq_callback(platform_irq_callback_type_t type, f_platform_irq_cb f, void *user_data)
{
    #pragma pack (push, 1)
    struct _param
    {
        int irq_n;
    } _param =
    {
        .irq_n = type,
    };
    #pragma pack (pop)
    irq_infos[(int)type].f = f;
    irq_infos[(int)type].user_data = user_data;
    call_void_fun(ID_sys_enable_irq, &_param, sizeof(_param));
}

const platform_ver_t *platform_get_version(void)
{
    return &ready_info.ver;
}

void platform_get_heap_status(platform_heap_status_t *status)
{
    struct node * _node = call_fun(ID_sys_get_heap_status, NULL, 0);
    int _len;
    void *_r = extract_ret_param(_node, &_len);
    if (_len == sizeof(*status))
        memcpy(status, _r, _len);
    else
        memset(status, 0, sizeof(*status));
    GEN_OS()->free(_node);
}

void platform_shutdown(const uint32_t duration_cycles, const void *p_retention_data, const uint32_t data_size)
{
    #pragma pack (push, 1)
    struct _param
    {
        uint32_t duration_cycles;
        const void *p_retention_data;
        uint32_t data_size;
    } _param =
    {
        .duration_cycles = duration_cycles,
        .p_retention_data = p_retention_data,
        .data_size = data_size,
    };
    #pragma pack (pop)
    call_void_fun(ID_sys_shutdown, &_param, sizeof(_param));
}

void platform_hrng(uint8_t *bytes, const uint32_t len)
{
    #pragma pack (push, 1)
    struct _param
    {
        uint32_t len;
    } _param =
    {
        .len = len
    };
    #pragma pack (pop)
    struct node * _node = call_fun(ID_sys_hrng, &_param, sizeof(_param));
    int _len;
    void *_r = extract_ret_param(_node, &_len);
    if (_len != len)
        LOG_W("platform_hrng: len mismatch");
    memcpy(bytes, _r, _len);
    GEN_OS()->free(_node);
}

void platform_set_timer(void (* callback)(void), uint32_t delay)
{
    #pragma pack (push, 1)
    struct _param
    {
        void (* callback)(void);
        uint32_t delay;
    } _param =
    {
        .callback = callback,
        .delay = delay,
    };
    #pragma pack (pop)
    call_void_fun(ID_sys_set_timer, &_param, sizeof(_param));
}

void sys_aligned_read_mem(uint32_t addr, int len, void *p)
{
    #pragma pack (push, 1)
    struct _param
    {
        uint32_t addr;
        int len;
    } _param =
    {
        .addr = addr,
        .len = len
    };
    #pragma pack (pop)
    struct node * _node = call_fun(ID_sys_aligned_read_mem, &_param, sizeof(_param));
    int _len;
    void *_r = extract_ret_param(_node, &_len);
    if (_len != len)
        LOG_W("sys_aligned_read_mem: len mismatch");
    memcpy(p, _r, _len);
    GEN_OS()->free(_node);
}

uint32_t sys_aligned_read(uint32_t addr)
{
    uint32_t r = 0;
    sys_aligned_read_mem(addr, sizeof(uint32_t), &r);
    return r;
}

void sys_aligned_write_mem(uint32_t addr, void *p, int len)
{
    #pragma pack (push, 1)
    struct _param
    {
        uint32_t addr;
        uint32_t data[len];
    } _param;
    #pragma pack (pop)
    _param.addr = addr;
    memcpy(_param.data, p, len);
    call_void_fun(ID_sys_aligned_write_mem, &_param, sizeof(_param));
}

void sys_aligned_write(uint32_t addr, uint32_t value)
{
    sys_aligned_write_mem(addr, &value, sizeof(value));
}

uint8_t gatt_client_get_mtu(hci_con_handle_t con_handle, uint16_t * mtu)
{
    #pragma pack (push, 1)
    struct _param
    {
        hci_con_handle_t con_handle;
    } _param =
    {
        .con_handle = con_handle,
    };
    struct _ret
    {
        uint8_t r;
        uint16_t mtu;
    };
    #pragma pack (pop)
    struct node * _node = call_fun(ID_gatt_client_get_mtu, &_param, sizeof(_param));
    struct _ret *_r = (struct _ret *)extract_ret_param(_node, NULL);
    *mtu = _r->mtu;
    GEN_OS()->free(_node);
    return _r->r;
}

void gatt_client_register_handler(btstack_packet_handler_t handler)
{
    gatt_callback = handler;
}

uint32_t btstack_push_user_runnable(f_btstack_user_runnable fun, void *data, const uint16_t user_value)
{
    struct node *pp = (struct node *)GEN_OS()->malloc(sizeof(struct runnable) + 3 + sizeof(struct node));
    struct uart_frame *frame = (struct uart_frame *)pp->body;
    struct runnable *runnable = (struct runnable *)frame->body;

    frame->len = 1 + sizeof(struct runnable);
    frame->type = RPC_FRAME_INTERNAL_RUNNABLE;

    runnable->f = fun;
    runnable->data = data;
    runnable->value = user_value;

    post_msg_to_host(pp);
    return 0;
}

platform_task_id_t platform_get_current_task()
{
    extern gen_handle_t port_get_current_tid(void);
    return port_get_current_tid() == host_task ? PLATFORM_TASK_HOST : (platform_task_id_t)(PLATFORM_TASK_HOST + 1);
}

const uint8_t bluetooth_base_uuid[] = { 0x00, 0x00, 0x00, 0x00, /* - */ 0x00, 0x00, /* - */ 0x10, 0x00, /* - */
    0x80, 0x00, /* - */ 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB };

int uuid_has_bluetooth_prefix(const uint8_t * uuid128)
{
    return memcmp(&uuid128[4], &bluetooth_base_uuid[4], 12) == 0;
}

void sm_register_oob_data_callback( int (*get_oob_data_callback)(uint8_t addres_type, bd_addr_t addr, uint8_t * oob_data))
{

}

const uint8_t *sm_private_random_address_generation_get(void)
{
    static bd_addr_t random_address = {0};
    #pragma pack (push, 1)
    struct _ret
    {
        bd_addr_t addr;
    };
    #pragma pack (pop)
    struct node * _node = call_fun(ID_sm_private_random_address_generation_get, NULL, 0);
    memcpy(random_address, extract_ret_param(_node, NULL), BD_ADDR_LEN);
    GEN_OS()->free(_node);
    return random_address;
}

void printf_hexdump(const void *data, int size)
{
    int i;
    if (size <= 0) return;
    for (i = 0; i< size; i++){
        printf("%02X ", ((uint8_t *)data)[i]);
    }
    printf("\n");
}

#include "../inc/brpc_calls.inc"

const char *get_fun_name(uint16_t fun_id)
{
    static char name[20];
    switch (fun_id)
    {
#include "../inc/brpc_fun_names.inc"
    default:
        sprintf(name, "??[%d]", fun_id);
        return name;
    }
}