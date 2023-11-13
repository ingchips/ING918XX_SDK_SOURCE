#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "btstack_sync.h"
#include "platform_api.h"
#include "att_db.h"

#include "btstack_event.h"
#include "btstack_defines.h"
#include "gatt_client_util.h"

#include "sig_uuid.h"

#include "port_gen_os_driver.h"

#ifndef GATT_UTIL_MAX_DISCOVERER_NUM
#define GATT_UTIL_MAX_DISCOVERER_NUM 8
#endif

#define INVALID_HANDLE 0xffff

struct btstack_enqueue_runnable
{
    f_btstack_synced_runnable runnable;
    void *user_data;
};

struct gatt_msg_discover_all
{
    hci_con_handle_t con_handle;
    int *err_code;
    struct gatt_client_discoverer *r;
};

struct gatt_msg_read_value
{
    hci_con_handle_t con_handle;
    uint16_t handle;
    uint8_t *data;
    uint16_t *length;
    int err_code;
};

struct gatt_msg_write_value
{
    hci_con_handle_t con_handle;
    uint16_t handle;
    const uint8_t *data;
    uint16_t length;
    int err_code;
};

struct btstack_synced_runner
{
    gen_handle_t msg_queue;
    gen_handle_t done_evt;
    int gap_err_code;
    struct gatt_msg_discover_all    discover_all;
    struct gatt_msg_read_value      read_value;
    struct gatt_msg_write_value     write_value;
};

static struct btstack_synced_runner *runners[GATT_UTIL_MAX_DISCOVERER_NUM] = {NULL};

typedef struct gatt_synced_msg
{
    struct btstack_enqueue_runnable  runnable;
} gatt_synced_msg_t;

struct gap_synced_create_conn
{
    gen_handle_t timer;
    initiating_filter_policy_t filter_policy;
    bd_addr_type_t own_addr_type;
    bd_addr_type_t peer_addr_type;
    const uint8_t *peer_addr;
    uint8_t initiating_phy_num;
    const initiating_phy_config_t *phy_configs;
    le_meta_event_enh_create_conn_complete_t *complete;
};

struct gap_synced_ctx
{
    uint8_t buffer[100];
    uint16_t conn_handle;
    uint16_t wait_opcode;
    uint16_t wait_subevent;
    uint8_t wait_event;
    gen_handle_t done_evt;
    struct gap_synced_create_conn create_conn;
} * gap_synced_ctx = NULL;

#define GEN_OS          ((const gen_os_driver_t *)platform_get_gen_os_driver())

static void synced_runner_task(void *data)
{
    struct btstack_synced_runner *runner = (struct btstack_synced_runner *)data;
    for (;;)
    {
        gatt_synced_msg_t msg;
        if (GEN_OS->queue_recv_msg(runner->msg_queue, &msg) != 0) continue;
        msg.runnable.runnable(runner, msg.runnable.user_data);
    }
}

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    uint8_t event = hci_event_packet_get_type(packet);
    if (packet_type != HCI_EVENT_PACKET) return;

    if (event == gap_synced_ctx->wait_event)
    {
        if (gap_synced_ctx->conn_handle != INVALID_HANDLE)
        {
            uint16_t handle = little_endian_read_16(packet, 3);
            if (gap_synced_ctx->conn_handle != handle)
                return;
        }

        gap_synced_ctx->wait_event = 0xff;
        gap_synced_ctx->conn_handle = INVALID_HANDLE;
        memcpy(gap_synced_ctx->buffer, packet + 2,  size - 2);
        GEN_OS->event_set(gap_synced_ctx->done_evt);
        return;
    }

    switch (event)
    {
    case HCI_EVENT_LE_META:
        if (hci_event_le_meta_get_subevent_code(packet) == gap_synced_ctx->wait_subevent)
        {
            if (gap_synced_ctx->conn_handle != INVALID_HANDLE)
            {
                uint16_t handle = little_endian_read_16(packet, 4);
                if (gap_synced_ctx->conn_handle != handle)
                    break;
            }

            memcpy(gap_synced_ctx->buffer, packet + 3, size - 3);
            gap_synced_ctx->wait_subevent = 0xff;
            gap_synced_ctx->conn_handle = INVALID_HANDLE;
            GEN_OS->event_set(gap_synced_ctx->done_evt);
        }

        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
            if (gap_synced_ctx->create_conn.complete)
            {
                const le_meta_event_enh_create_conn_complete_t * complete =
                    decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t);
                GEN_OS->timer_stop(gap_synced_ctx->create_conn.timer);
                memcpy(gap_synced_ctx->create_conn.complete, complete, sizeof(*complete));
                GEN_OS->event_set(gap_synced_ctx->done_evt);
            }
            break;
        default:
            break;
        }
        break;
    case HCI_EVENT_COMMAND_COMPLETE:
        if (gap_synced_ctx->wait_opcode == hci_event_command_complete_get_command_opcode(packet))
        {
            const uint8_t *returns = hci_event_command_complete_get_return_parameters(packet);
            if (gap_synced_ctx->conn_handle != INVALID_HANDLE)
            {
                uint16_t handle = little_endian_read_16(returns, 1);
                if (gap_synced_ctx->conn_handle != handle)
                    break;
            }

            memcpy(gap_synced_ctx->buffer, returns, size - (returns - packet));
            gap_synced_ctx->wait_opcode = 0xffff;
            gap_synced_ctx->conn_handle = INVALID_HANDLE;
            GEN_OS->event_set(gap_synced_ctx->done_evt);
        }
        break;

    default:
        break;
    }
}

static btstack_packet_callback_registration_t hci_event_callback_registration =
{
    .callback = &user_packet_handler,
};

struct btstack_synced_runner *btstack_create_sync_runner(int enable_gap_api)
{
    struct btstack_synced_runner *runner =
        (struct btstack_synced_runner *)GEN_OS->malloc(sizeof(struct btstack_synced_runner));
    memset(runner, 0, sizeof(*runner));
    runner->msg_queue = GEN_OS->queue_create(1, sizeof(gatt_synced_msg_t));
    runner->done_evt = GEN_OS->event_create();
    GEN_OS->task_create("b",
               synced_runner_task, runner,
               BTSTACK_SYNC_RUNNER_STACK_SIZE,
               GEN_TASK_PRIORITY_LOW);
    if (enable_gap_api)
    {
        gap_synced_ctx = (struct gap_synced_ctx *)GEN_OS->malloc(sizeof(struct gap_synced_ctx));
        memset(gap_synced_ctx, 0, sizeof(*gap_synced_ctx));
        gap_synced_ctx->done_evt = runner->done_evt;
        hci_add_event_handler(&hci_event_callback_registration);
    }

    return runner;
}

int btstack_sync_run(struct btstack_synced_runner *runner, f_btstack_synced_runnable runnable, void *user_data)
{
    gatt_synced_msg_t msg =
    {
        .runnable =
        {
            .runnable = runnable,
            .user_data = user_data
        }
    };
    return GEN_OS->queue_send_msg(runner->msg_queue, &msg);
}

static int gatt_client_sync_wait_done(struct btstack_synced_runner *runner)
{
    return GEN_OS->event_wait(runner->done_evt);
}

static void gatt_client_sync_mark_done(struct btstack_synced_runner *runner)
{
    GEN_OS->event_set(runner->done_evt);
}

static void gatt_client_sync_discovered(service_node_t *first, void *user_data, int err_code)
{
    struct btstack_synced_runner *runner = (struct btstack_synced_runner *)user_data;
    *runner->discover_all.err_code = err_code;
    gatt_client_sync_mark_done(runner);
}

static void read_value_callback(uint8_t packet_type, uint16_t con_handle, const uint8_t *packet, uint16_t size)
{
    struct btstack_synced_runner *runner = runners[con_handle];
    switch (packet[0])
    {
    case GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT:
    case GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT:
        {
            uint16_t value_size;
            const gatt_event_value_packet_t *value =
                gatt_event_characteristic_value_query_result_parse(packet, size, &value_size);
            if (value_size <= *runner->read_value.length)
            {
                *runner->read_value.length = value_size;
                memcpy(runner->read_value.data, value->value, value_size);
            }
            else
                runner->read_value.err_code = BTSTACK_SYNCED_ERROR_BUFFER_TOO_SMALL;
        }
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        if (runner->read_value.err_code == 0)
            runner->read_value.err_code = gatt_event_query_complete_parse(packet)->status;
        gatt_client_sync_mark_done(runner);
        break;
    }
}

void write_value_callback(uint8_t packet_type, uint16_t con_handle, const uint8_t *packet, uint16_t size)
{
    struct btstack_synced_runner *runner = runners[con_handle];
    switch (packet[0])
    {
    case GATT_EVENT_QUERY_COMPLETE:
        gatt_client_sync_mark_done(runner);
        break;
    }
}

static void discover_all(struct btstack_synced_runner *runner, uint16_t _)
{
    runner->discover_all.r = gatt_client_util_discover_all(runner->discover_all.con_handle,
            gatt_client_sync_discovered, runner);
}

static void read_value(struct btstack_synced_runner *runner, uint16_t _)
{
    runners[runner->discover_all.con_handle] = runner;
    runner->read_value.err_code = gatt_client_read_value_of_characteristic_using_value_handle(
            read_value_callback,
            runner->discover_all.con_handle,
            runner->read_value.handle);
    if (runner->read_value.err_code)
        gatt_client_sync_mark_done(runner);
}

static void read_discriptor(struct btstack_synced_runner *runner, uint16_t _)
{
    runners[runner->discover_all.con_handle] = runner;
    runner->read_value.err_code = gatt_client_read_characteristic_descriptor_using_descriptor_handle(
            read_value_callback,
            runner->discover_all.con_handle,
            runner->read_value.handle);
    if (runner->read_value.err_code)
        gatt_client_sync_mark_done(runner);
}

static void write_value(struct btstack_synced_runner *runner, uint16_t _)
{
    runners[runner->discover_all.con_handle] = runner;
    runner->write_value.err_code = gatt_client_write_value_of_characteristic(
            write_value_callback,
            runner->discover_all.con_handle,
            runner->write_value.handle,
            runner->write_value.length,
            runner->write_value.data);
    if (runner->read_value.err_code)
        gatt_client_sync_mark_done(runner);
}

static void write_value_wo_response(struct btstack_synced_runner *runner, uint16_t _)
{
    runner->write_value.err_code = gatt_client_write_value_of_characteristic_without_response(
            runner->discover_all.con_handle,
            runner->write_value.handle,
            runner->write_value.length,
            runner->write_value.data);
    gatt_client_sync_mark_done(runner);
}

static void write_descriptor(struct btstack_synced_runner *runner, uint16_t _)
{
    runners[runner->discover_all.con_handle] = runner;
    runner->write_value.err_code = gatt_client_write_characteristic_descriptor_using_descriptor_handle(
            write_value_callback,
            runner->discover_all.con_handle,
            runner->write_value.handle,
            runner->write_value.length,
            (uint8_t *)runner->write_value.data);
    if (runner->read_value.err_code)
        gatt_client_sync_mark_done(runner);
}

struct gatt_client_discoverer * gatt_client_sync_discover_all(struct btstack_synced_runner *runner,
    hci_con_handle_t con_handle, int *err_code)
{
    runner->discover_all.con_handle = con_handle;
    runner->discover_all.err_code = err_code;
    runner->discover_all.r = NULL;

    btstack_push_user_runnable((f_btstack_user_runnable)discover_all, runner, 0);

    if (gatt_client_sync_wait_done(runner) != 0)
    {
        if (runner->discover_all.r)
        {
            gatt_client_util_free(runner->discover_all.r);
            runner->discover_all.r = NULL;
        }
    }
    return runner->discover_all.r;
}


int gatt_client_sync_read_value_of_characteristic(struct btstack_synced_runner *runner,
    hci_con_handle_t con_handle,
    uint16_t characteristic_value_handle, uint8_t *data, uint16_t *length)
{
    runner->read_value.con_handle = con_handle;
    runner->read_value.handle = characteristic_value_handle;
    runner->read_value.data = data;
    runner->read_value.length = length;
    btstack_push_user_runnable((f_btstack_user_runnable)read_value, runner, 0);

    return gatt_client_sync_wait_done(runner) ? BTSTACK_SYNCED_ERROR_RTOS : runner->read_value.err_code;
}

int gatt_client_sync_read_characteristic_descriptor(struct btstack_synced_runner *runner,
    hci_con_handle_t con_handle,
    uint16_t descriptor_handle, uint8_t *data, uint16_t *length)
{
    runner->read_value.con_handle = con_handle;
    runner->read_value.handle = descriptor_handle;
    runner->read_value.data = data;
    runner->read_value.length = length;
    btstack_push_user_runnable((f_btstack_user_runnable)read_discriptor, runner, 0);

    return gatt_client_sync_wait_done(runner) ? BTSTACK_SYNCED_ERROR_RTOS : runner->read_value.err_code;
}

int gatt_client_sync_write_value_of_characteristic(struct btstack_synced_runner *runner,
    hci_con_handle_t con_handle,
    uint16_t characteristic_value_handle, const uint8_t *data, uint16_t length)
{
    runner->write_value.con_handle = con_handle;
    runner->write_value.handle = characteristic_value_handle;
    runner->write_value.data = data;
    runner->write_value.length = length;
    btstack_push_user_runnable((f_btstack_user_runnable)write_value, runner, 0);

    return gatt_client_sync_wait_done(runner) ? BTSTACK_SYNCED_ERROR_RTOS : runner->write_value.err_code;
}

int gatt_client_sync_write_value_of_characteristic_without_response(struct btstack_synced_runner *runner,
    hci_con_handle_t con_handle,
    uint16_t characteristic_value_handle, const uint8_t *data, uint16_t length)
{
    runner->write_value.con_handle = con_handle;
    runner->write_value.handle = characteristic_value_handle;
    runner->write_value.data = data;
    runner->write_value.length = length;
    btstack_push_user_runnable((f_btstack_user_runnable)write_value_wo_response, runner, 0);

    return gatt_client_sync_wait_done(runner) ? BTSTACK_SYNCED_ERROR_RTOS : runner->write_value.err_code;
}

int gatt_client_sync_write_characteristic_descriptor(struct btstack_synced_runner *runner,
    hci_con_handle_t con_handle,
    uint16_t descriptor_handle, const uint8_t *data, uint16_t length)
{
    runner->write_value.con_handle = con_handle;
    runner->write_value.handle = descriptor_handle;
    runner->write_value.data = data;
    runner->write_value.length = length;
    btstack_push_user_runnable((f_btstack_user_runnable)write_descriptor, runner, 0);

    return gatt_client_sync_wait_done(runner) ? BTSTACK_SYNCED_ERROR_RTOS : runner->write_value.err_code;
}

#define OPCODE(ogf, ocf)            (ocf | ogf << 10)

static void create_connection(struct btstack_synced_runner *runner, uint16_t _)
{
    struct gap_synced_create_conn *create = &gap_synced_ctx->create_conn;
    runner->gap_err_code = gap_ext_create_connection(
        create->filter_policy,
        create->own_addr_type,
        create->peer_addr_type,
        create->peer_addr,
        create->initiating_phy_num,
        create->phy_configs);
    if (runner->gap_err_code)
        gatt_client_sync_mark_done(runner);
}

static void cancel_create(void *_, uint16_t __)
{
    gap_create_connection_cancel();
}

static void create_connection_timeout(void *_)
{
    btstack_push_user_runnable(cancel_create, NULL, 0);
}

int gap_sync_ext_create_connection(struct btstack_synced_runner *runner,
                                  const initiating_filter_policy_t filter_policy,
                                  const bd_addr_type_t own_addr_type,
	                              const bd_addr_type_t peer_addr_type,
	                              const uint8_t *peer_addr,
                                  const uint8_t initiating_phy_num,
                                  const initiating_phy_config_t *phy_configs,
                                  uint32_t timeout_ms,
                                  le_meta_event_enh_create_conn_complete_t *complete)
{
    struct gap_synced_create_conn *create = &gap_synced_ctx->create_conn;
    create->timer = GEN_OS->timer_create(timeout_ms, NULL, create_connection_timeout);
    GEN_OS->timer_start(create->timer);

    create->filter_policy = filter_policy;
    create->own_addr_type = own_addr_type;
    create->peer_addr_type = peer_addr_type;
    create->peer_addr = peer_addr;
    create->initiating_phy_num = initiating_phy_num;
    create->phy_configs = phy_configs;
    create->complete = complete;
    btstack_push_user_runnable((f_btstack_user_runnable)create_connection, runner, 0);

    int t = gatt_client_sync_wait_done(runner);

    GEN_OS->timer_delete(create->timer);
    create->timer = NULL;
    create->complete = NULL;

    if (t) return BTSTACK_SYNCED_ERROR_RTOS;

    if (runner->gap_err_code == 0)
        runner->gap_err_code = complete->status;

    return runner->gap_err_code;
}

static void le_read_channel_map(struct btstack_synced_runner *runner, uint16_t con_handle)
{
    runner->gap_err_code = gap_le_read_channel_map(con_handle);
    if (runner->gap_err_code)
        gatt_client_sync_mark_done(runner);
}

int gap_sync_le_read_channel_map(struct btstack_synced_runner *runner,
    hci_con_handle_t con_handle, uint8_t channel_map[5])
{
    gap_synced_ctx->conn_handle = con_handle;
    gap_synced_ctx->wait_opcode = OPCODE(OGF_LE_CONTROLLER, 0x15);
    btstack_push_user_runnable((f_btstack_user_runnable)le_read_channel_map, runner, con_handle);

    if (gatt_client_sync_wait_done(runner))
        return BTSTACK_SYNCED_ERROR_RTOS;

    if (runner->gap_err_code == 0)
        runner->gap_err_code = gap_synced_ctx->buffer[0];

    if (runner->gap_err_code)
        return runner->gap_err_code;
    memcpy(channel_map, gap_synced_ctx->buffer + 3, 5);
    return 0;
}

static void read_rssi(struct btstack_synced_runner *runner, uint16_t con_handle)
{
    runner->gap_err_code = gap_read_rssi(con_handle);
    if (runner->gap_err_code)
        gatt_client_sync_mark_done(runner);
}

int gap_sync_read_rssi(struct btstack_synced_runner *runner,
    hci_con_handle_t con_handle, int8_t *rssi)
{
    gap_synced_ctx->conn_handle = con_handle;
    gap_synced_ctx->wait_opcode = OPCODE(OGF_STATUS_PARAMETERS, 0x05);
    btstack_push_user_runnable((f_btstack_user_runnable)read_rssi, runner, con_handle);

    if (gatt_client_sync_wait_done(runner))
        return BTSTACK_SYNCED_ERROR_RTOS;

    const event_command_complete_return_param_read_rssi_t *r =
        (const event_command_complete_return_param_read_rssi_t *)gap_synced_ctx->buffer;

    if (runner->gap_err_code == 0)
        runner->gap_err_code = r->status;

    if (runner->gap_err_code)
        return runner->gap_err_code;

    *rssi = r->rssi;
    return 0;
}

static void read_phy(struct btstack_synced_runner *runner, uint16_t con_handle)
{
    runner->gap_err_code = gap_read_phy(con_handle);
    if (runner->gap_err_code)
        gatt_client_sync_mark_done(runner);
}

int gap_sync_read_phy(struct btstack_synced_runner *runner,
    hci_con_handle_t con_handle, phy_type_t *tx_phy, phy_type_t *rx_phy)
{
    gap_synced_ctx->conn_handle = con_handle;
    gap_synced_ctx->wait_opcode = OPCODE(OGF_LE_CONTROLLER, 0x30);
    btstack_push_user_runnable((f_btstack_user_runnable)read_phy, runner, con_handle);
    if (gatt_client_sync_wait_done(runner))
        return BTSTACK_SYNCED_ERROR_RTOS;

    const event_command_complete_return_param_read_phy_t *r =
        (const event_command_complete_return_param_read_phy_t *)gap_synced_ctx->buffer;

    if (runner->gap_err_code == 0)
        runner->gap_err_code = r->status;

    if (runner->gap_err_code)
        return runner->gap_err_code;

    *tx_phy = r->tx_phy;
    *rx_phy = r->rx_phy;
    return 0;
}

static void read_remote_version(struct btstack_synced_runner *runner, uint16_t con_handle)
{
    runner->gap_err_code = gap_read_remote_version(con_handle);
    if (runner->gap_err_code)
        gatt_client_sync_mark_done(runner);
}

int gap_sync_read_remote_version(struct btstack_synced_runner *runner,
    hci_con_handle_t con_handle,
    uint8_t *version,
    uint16_t *manufacturer_name,
    uint16_t *subversion)
{
    gap_synced_ctx->conn_handle = con_handle;
    gap_synced_ctx->wait_event = HCI_EVENT_READ_REMOTE_VERSION_INFORMATION_COMPLETE;
    btstack_push_user_runnable((f_btstack_user_runnable)read_remote_version, runner, con_handle);
    if (gatt_client_sync_wait_done(runner))
        return BTSTACK_SYNCED_ERROR_RTOS;

    #pragma pack (push, 1)
    struct read_remote_version
    {
        uint8_t Status;
        uint16_t Connection_Handle;
        uint8_t Version;
        uint16_t Manufacturer_Name;
        uint16_t Subversion;
    } *r = (struct read_remote_version *)gap_synced_ctx->buffer;
    #pragma pack (pop)

    if (runner->gap_err_code == 0)
        runner->gap_err_code = r->Status;

    if (runner->gap_err_code)
        return runner->gap_err_code;

    *version = r->Version;
    *manufacturer_name = r->Manufacturer_Name;
    *subversion = r->Subversion;
    return 0;
}

static void read_remote_features(struct btstack_synced_runner *runner, uint16_t con_handle)
{
    runner->gap_err_code = gap_read_remote_used_features(con_handle);
    if (runner->gap_err_code)
        gatt_client_sync_mark_done(runner);
}

int gap_sync_read_remote_used_features(struct btstack_synced_runner *runner,
    hci_con_handle_t con_handle, uint8_t features[8])
{
    gap_synced_ctx->conn_handle = con_handle;
    gap_synced_ctx->wait_subevent = HCI_SUBEVENT_LE_READ_REMOTE_USED_FEATURES_COMPLETE;
    btstack_push_user_runnable((f_btstack_user_runnable)read_remote_features, runner, con_handle);
    if (gatt_client_sync_wait_done(runner))
        return BTSTACK_SYNCED_ERROR_RTOS;

    if (runner->gap_err_code == 0)
        runner->gap_err_code = gap_synced_ctx->buffer[0];

    if (runner->gap_err_code)
        return runner->gap_err_code;

    memcpy(features, gap_synced_ctx->buffer + 3, 8);
    return 0;
}
