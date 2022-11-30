#include "profile.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include "gatt_client.h"
#include "gatt_client_util.h"
#include "btstack_sync.h"

#include "btstack_mt.h"
#include "btstack_defines.h"

#include "port_gen_os_driver.h"

#include "log.h"
#include "tools.h"

static Timing Timing;

#define GEN_OS()      ((struct gen_os_driver *)platform_get_gen_os_driver())

const uint8_t addr[6] = {0xFE, 0x11, 0x11, 0x11, 0x11, 0x11};

uint8_t target[6] = {0xCD, 0xA3, 0x28, 0x11, 0x89, 0x3e};

#define ADDR_FMT    "%02X:%02X:%02X:%02X:%02X:%02X"
#define ADDR_LE(X)  X[5], X[4], X[3], X[2], X[1], X[0]
#define ADDR_BE(X)  X[0], X[1], X[2], X[3], X[4], X[5]

int error_codes_stat[256] = {0};
bool connected = false;
gen_handle_t disc_event = NULL;

#define CONN_PARAM  {                   \
            .scan_int = 200,            \
            .scan_win = 100,            \
            .interval_min = 30,         \
            .interval_max = 30,         \
            .latency = 0,               \
            .supervision_timeout = 100, \
            .min_ce_len = 90,           \
            .max_ce_len = 90            \
    }

static initiating_phy_config_t phy_configs[] =
{
    {
        .phy = PHY_1M,
        .conn_param = CONN_PARAM
    },
    {
        .phy = PHY_2M,
        .conn_param = CONN_PARAM
    },
    {
        .phy = PHY_CODED,
        .conn_param = CONN_PARAM
    }
};

bool wait_until(bool (*f)(void *), void *data, int timeout_ms)
{
    uint64_t t = get_sys_ms_time();
    while (get_sys_ms_time() - t < timeout_ms)
    {
        if (f(data)) return true;
        sleep_ms(1);
    }
    return false;
}

bool is_disconnected(void *)
{
    return !connected;
}

int do_run_test(struct btstack_synced_runner *runner,
              const uint8_t *device,
              int initiating_timeout,
              int connection_duration,
              int disconnect_timeout,
              int &init_pass,
              int &keep_pass)
{
    le_meta_event_enh_create_conn_complete_t complete = {0};
    Timing.Diff();
    int r = gap_sync_ext_create_connection(runner,
                                INITIATING_ADVERTISER_FROM_PARAM,
                                BD_ADDR_TYPE_LE_RANDOM,           // Own_Address_Type,
                                BD_ADDR_TYPE_LE_RANDOM,                  // Peer_Address_Type,
                                device,                       // Peer_Address,
                                sizeof(phy_configs) / sizeof(phy_configs[0]),
                                phy_configs,
                                initiating_timeout,
                                &complete);
    if (r)
    {
        LOG_E("connection timeout");
        return - __LINE__;
    }

    LOG_OK("connected in %d ms", Timing.Diff());
    init_pass++;

    LOG_PROG("keep connection (%d ms)", connection_duration);
    wait_until(is_disconnected, NULL, connection_duration);

    if (!connected)
    {
        LOG_E("connection lost after %d ms", Timing.Diff());
        return - __LINE__;
    }

    LOG_OK("connection maintained");
    keep_pass++;

    LOG_PROG("disconnecting...");
    Timing.Diff();
    mt_gap_disconnect(complete.handle);
    GEN_OS()->event_wait(disc_event);
    uint32_t t = Timing.Diff();
    if (t <= disconnect_timeout)
        LOG_OK("disconnected in %d ms", t);
    else
    {
        LOG_W("disconnected in %d ms", t);
        return - __LINE__;
    }

    return 0;
}

void run_test(struct btstack_synced_runner *runner,
              const uint8_t *device,
              int initiating_timeout,
              int connection_duration,
              int disconnect_timeout,
              int idle_duration,
              int n)
{
    int c = 0;
    int init_pass = 0;
    int keep_pass = 0;
    for (int i = 0; i < n; i++)
    {
        LOG_PROG("TEST ROUND (%d / %d) started", i + 1, n);
        if (0 == do_run_test(runner, device, initiating_timeout, connection_duration, disconnect_timeout,
                             init_pass, keep_pass))
        {
            LOG_PASS("test round ended");
            c++;
        }
        else
            LOG_FAIL("test round ended");
        sleep_ms(idle_duration);
    }

    LOG_PLAIN("SUMMARY:");
    LOG_PLAIN("TOTAL: %d", n);
    LOG_PLAIN("PASS : %d (%.1f%%)", c, c * 100. / n);
    LOG_PLAIN("INIT : %d (%.1f%%)", init_pass, init_pass * 100. / n);
    LOG_PLAIN("KEEP : %d (%.1f%%)", keep_pass, keep_pass * 100. / n);


    //exit(0);

    LOG_PLAIN("Disconnect status code statistics (code, number)");
    for (int i = 0; i < sizeof(error_codes_stat) / sizeof(error_codes_stat[0]); i++)
    {
        if (error_codes_stat[i] > 0)
            LOG_PLAIN("%d,%d", i, error_codes_stat[i]);
    }

    LOG_PLAIN("----- done ----");
    exit(0);
}

extern "C" void synced_run_test(struct btstack_synced_runner *runner, void *user_data)
{
    run_test(runner,
             target,
             2000,
             10000,
             1000,
             1000,
             5);
}

static struct btstack_synced_runner *runner = NULL;

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    const static ext_adv_set_en_t adv_sets_en[] = {{.handle = 0, .duration = 0, .max_events = 0}};
    const btstack_user_msg_t *p_user_msg;
    uint8_t event = hci_event_packet_get_type(packet);
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;

        if (runner == NULL)
        {
            runner = btstack_create_sync_runner(1);
            disc_event = GEN_OS()->event_create();
        }
        gap_set_random_device_address(addr);
        btstack_sync_run(runner, synced_run_test, NULL);
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
            {
                const le_meta_event_enh_create_conn_complete_t * complete =
                    decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t);

                if (complete->status == 0)
                    connected = true;
            }
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        {
            const event_disconn_complete_t *complete = decode_hci_event_disconn_complete(packet);
            connected = false;
            error_codes_stat[complete->reason]++;
            switch (complete->reason)
            {
            case 0x13:
                LOG_OK("Disconnected by remote");
                break;
            case 0x16:
                LOG_OK("Disconnected by local");
                break;
            default:
                LOG_E("Disconnect reason: %d", complete->reason);
                break;
            }
            GEN_OS()->event_set(disc_event);
        }
        break;

    case GATT_EVENT_MTU:
        LOG_OK("GATT client MTU updated: %d", gatt_event_mtu_get_mtu(packet));
        break;

    case ATT_EVENT_CAN_SEND_NOW:
        // add your code
        break;

    default:
        break;
    }
}

static btstack_packet_callback_registration_t hci_event_callback_registration;

uint32_t setup_profile(void *data, void *user_data)
{
    LOG_PLAIN("=== BRPC Client ===");
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}
