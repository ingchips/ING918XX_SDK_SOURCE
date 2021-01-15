#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_util.h"
#include "btstack_defines.h"
#include "gatt_client.h"
#include "sig_uuid.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <ctype.h>
#include "sm.h"

#include "uart_console.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

sm_persistent_t sm_persistent =
{
    .er = {1, 2, 3},
    .ir = {4, 5, 6},
    .identity_addr_type     = BD_ADDR_TYPE_LE_RANDOM,
    .identity_addr          = {0xC6, 0xFA, 0x5C, 0x20, 0x87, 0xA7}
};

// GATT characteristic handles
#define HANDLE_DEVICE_NAME                                   3

static uint8_t adv_data[31] = {
    #include "../data/advertising.adv"
};

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
};

const static uint8_t profile_data[] = {
    #include "../data/gatt.profile"
};

#define INVALID_HANDLE  0xffff
uint16_t conn_handle = INVALID_HANDLE;
static int bonding_flag = 0;

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

uint8_t slave_addr[] = {0,0,0,0,0,0};
bd_addr_type_t slave_addr_type = BD_ADDR_TYPE_LE_RANDOM;

extern int8_t adv_tx_power;

void do_set_data()
{
    gap_set_ext_adv_data(0, adv_data[0] + 1, (uint8_t *)(adv_data));
}

#define iprintf platform_printf

typedef struct desc_node
{
    struct desc_node *next;
    gatt_client_characteristic_descriptor_t desc;
} desc_node_t;

typedef struct char_node
{
    struct char_node *next;
    gatt_client_characteristic_t chara;
    gatt_client_notification_t *notification;
    desc_node_t *descs;
} char_node_t;

typedef struct service_node
{
    struct service_node *next;
    gatt_client_service_t service;
    char_node_t *chars;
} service_node_t;

struct list_node
{
    struct list_node *next;
};

service_node_t first_service = {0};
service_node_t *cur_disc_service = NULL;
char_node_t *cur_disc_char = NULL;

typedef void (*f_list_node)(struct list_node *node);

void list_for_each(struct list_node *head, f_list_node f)
{
    while (head)
    {
        struct list_node *t = head;
        head = head->next;
        f(t);
    }
}

void free_list(struct list_node *head)
{
    list_for_each(head, (f_list_node)free);
}

void free_char(char_node_t *a_char)
{
    if (a_char == NULL) return;
    free_list((struct list_node *)a_char->descs);
    if (a_char->notification) free(a_char->notification);
    free(a_char);
}

void free_service(service_node_t *s)
{
    if (s == NULL) return;
    list_for_each((struct list_node *)s->chars, (f_list_node)free_char);
    free(s);
}

void free_services(void)
{
    if (NULL == first_service.next) return;
    list_for_each((struct list_node *)first_service.next, (f_list_node)free_service);
    first_service.next = NULL;
}

service_node_t *get_last_service(void)
{
    service_node_t *r = &first_service;
    while (r->next)
        r = r->next;
    return r;
}

char_node_t *find_char(uint16_t handle)
{
    service_node_t *s = first_service.next;
    while (s)
    {
        char_node_t *c = s->chars;
        while (c)
        {
            if (c->chara.value_handle == handle)
                return c;
            c = c->next;
        }
        s = s->next;
    }
    return NULL;
}

int is_sig_uuid(const uint8_t *uuid, uint16_t sig_id)
{
    if (uuid_has_bluetooth_prefix(uuid))
    {
        uint16_t v = (uuid[2] << 8) | uuid[3];
        return v == sig_id;
    }
    else
        return 0;
}

desc_node_t *find_config_desc(char_node_t *c)
{
    if (NULL == c) return NULL;
    desc_node_t *d = c->descs;
    while (d)
    {
        if (is_sig_uuid(d->desc.uuid128, SIG_UUID_DESCRIP_GATT_CLIENT_CHARACTERISTIC_CONFIGURATION))
            return d;
        d = d->next;
    }
    return NULL;
}

void print_uuid(const uint8_t *uuid)
{
    if (uuid_has_bluetooth_prefix(uuid))
    {
        uint16_t v = (uuid[2] << 8) | uuid[3];
        iprintf("0x%04x", v);
    }
    else
        platform_printf("{%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
                        uuid[0], uuid[1], uuid[2], uuid[3],
                        uuid[4], uuid[5], uuid[6], uuid[7], uuid[8], uuid[9],
                        uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);
}

void print_properties(uint16_t v)
{
    iprintf("0x%04x (", v);
    if (v & 0x1)
        iprintf("broadcast, ");
    if (v & 0x2)
        iprintf("read, ");
    if (v & 0x4)
        iprintf("writeWithoutResponse, ");
    if (v & 0x8)
        iprintf("write, ");
    if (v & 0x10)
        iprintf("notify, ");
    if (v & 0x20)
        iprintf("indicate, ");
    if (v & 0x40)
        iprintf("authWrite, ");
    if (v & 0x80)
        iprintf("extendedProperties, ");
    iprintf(")");
}

void print_gatt_profile(void)
{
    service_node_t *s = first_service.next;

    iprintf("========== GATT PROFILE DUMP ==========\n\n");

    while (s)
    {
        char_node_t *c = s->chars;
        iprintf("SERVICE: ");
        print_uuid(s->service.uuid128);
        iprintf("\nHANDLE RANGE: %d - %d\n\n", s->service.start_group_handle, s->service.end_group_handle);

        while (c)
        {
            desc_node_t *d = c->descs;

            iprintf("    CHARACTERISTIC: ");
            print_uuid(c->chara.uuid128);
            iprintf("\n        HANDLE RANGE: %d - %d\n", c->chara.start_handle, c->chara.end_handle);
            iprintf(  "        VALUE HANDLE: %d\n", c->chara.value_handle);
            iprintf(  "        PROPERTIES  : "); print_properties(c->chara.properties); iprintf("\n\n");

            while (d)
            {
                iprintf("        DESCRIPTOR: ");
                print_uuid(d->desc.uuid128);
                iprintf("\n        HANDLE: %d\n\n", d->desc.handle);

                d = d->next;
            }
            c = c->next;
        }
        s = s->next;
    }
}

void characteristic_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size);
void descriptor_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size);

void discover_char_of_service(void)
{
    if (cur_disc_service)
    {
        gatt_client_discover_characteristics_for_service(characteristic_discovery_callback,
                                                         0,
                                                         cur_disc_service->service.start_group_handle,
                                                         cur_disc_service->service.end_group_handle);
    }
    else
    {
        iprintf("all discovered\n");
        print_gatt_profile();
    }
}

void discover_desc_of_char(void)
{
    while (cur_disc_char)
    {
        if (cur_disc_char->chara.end_handle > cur_disc_char->chara.value_handle)
            break;
        cur_disc_char = cur_disc_char->next;
    }

    if (cur_disc_char)
    {
        gatt_client_discover_characteristic_descriptors(descriptor_discovery_callback,
                                                        0,
                                                        &cur_disc_char->chara);
    }
    else
    {
        cur_disc_service = cur_disc_service->next;
        discover_char_of_service();
    }
}

void descriptor_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT:
        {
            const gatt_event_all_characteristic_descriptors_query_result_t *result =
                gatt_event_all_characteristic_descriptors_query_result_parse(packet);
            desc_node_t *node = (desc_node_t *)malloc(sizeof(desc_node_t));
            node->next = cur_disc_char->descs;
            cur_disc_char->descs = node;

            node->desc = result->descriptor;
        }
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        iprintf("descriptor_discovery COMPLETE: %d\n", gatt_event_query_complete_parse(packet)->status);
        cur_disc_char = cur_disc_char->next;
        if (cur_disc_char)
        {
            discover_desc_of_char();
        }
        else
        {
            cur_disc_service = cur_disc_service->next;
            discover_char_of_service();
        }
        break;
    }
}

void characteristic_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_CHARACTERISTIC_QUERY_RESULT:
        {
            const gatt_event_characteristic_query_result_t *result = gatt_event_characteristic_query_result_parse(packet);
            char_node_t *node = (char_node_t *)malloc(sizeof(char_node_t));
            node->next = cur_disc_service->chars;
            cur_disc_service->chars = node;

            node->descs = NULL;
            node->notification = NULL;
            node->chara = result->characteristic;
        }
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        iprintf("characteristic_discovery COMPLETE: %d\n", gatt_event_query_complete_parse(packet)->status);
        if (gatt_event_query_complete_parse(packet)->status != 0)
            break;

        iprintf("characteristic_discovery COMPLETE\n");
        cur_disc_char = cur_disc_service->chars;
        discover_desc_of_char();
        break;
    }
}

void service_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_SERVICE_QUERY_RESULT:
        {
            const gatt_event_service_query_result_t *result = gatt_event_service_query_result_parse(packet);
            service_node_t *last = get_last_service();
            last->next = (service_node_t *)malloc(sizeof(service_node_t));
            last->next->chars = NULL;
            last->next->next = NULL;
            last->next->service = result->service;
        }
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        iprintf("service_discovery COMPLETE: %d\n", gatt_event_query_complete_parse(packet)->status);
        if (gatt_event_query_complete_parse(packet)->status != 0)
        {
            gap_disconnect(0);
            break;
        }

        cur_disc_service = first_service.next;
        if (cur_disc_service)
            gatt_client_discover_characteristics_for_service(characteristic_discovery_callback,
                                                             0,
                                                             cur_disc_service->service.start_group_handle,
                                                             cur_disc_service->service.end_group_handle);
        break;
    }
}

char nibble_to_char(int v)
{
    return v <= 9 ? v - 0 + '0' : v - 10 + 'A';
}

void print_hex_table(const uint8_t *value, int len)
{
    int i;
    static char line[16 * 3 + 16 + 5] = {0};

    line[sizeof(line) - 1] = '\0';

    iprintf(" 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15\n");
    iprintf("===============================================\n");

    for (i = 0; i < (len + 15) / 16; i++)
    {
        int s = i * 16;
        int n = s + 16 > len ? len - s : 16;
        int j;
        memset(line, ' ', sizeof(line) - 1);

        line[16 * 3 + 1] = '|';

        for (j = 0; j < n; j++)
        {
            uint8_t v = value[s + j];
            line[3 * j + 0] = nibble_to_char(v >> 4);
            line[3 * j + 1] = nibble_to_char(v & 0xf);
            line[16 * 3 + 3 + j] = (v <= 127) && isprint(v) ? v : '.';
        }
        iprintf("%s\n", line);
    }
}

void read_characteristic_value_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT:
        {
            uint16_t value_size;
            const gatt_event_value_packet_t *value =
                gatt_event_characteristic_value_query_result_parse(packet, size, &value_size);
            if (value_size)
            {
                iprintf("VALUE of %d:\n", value->handle);
                print_hex_table(value->value, value_size);
            }
        }
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        iprintf("read_characteristic_value COMPLETE: %d\n", gatt_event_query_complete_parse(packet)->status);
        if (gatt_event_query_complete_parse(packet)->status != 0)
        {
            gap_disconnect(0);
            break;
        }
        break;
    }
}

void write_characteristic_value_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_QUERY_COMPLETE:
        iprintf("write_characteristic_value COMPLETE: %d\n", gatt_event_query_complete_parse(packet)->status);
        if (gatt_event_query_complete_parse(packet)->status != 0)
        {
            gap_disconnect(0);
            break;
        }
        break;
    }
}

void write_characteristic_descriptor_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_QUERY_COMPLETE:
        iprintf("write_characteristic_descriptor COMPLETE: %d\n", gatt_event_query_complete_parse(packet)->status);
        if (gatt_event_query_complete_parse(packet)->status != 0)
        {
            gap_disconnect(0);
            break;
        }
        break;
    }
}

static void output_notification_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    const gatt_event_value_packet_t *value;
    uint16_t value_size;
    switch (packet[0])
    {
    case GATT_EVENT_NOTIFICATION:
        value = gatt_event_notification_parse(packet, size, &value_size);
        if (value_size)
        {
            iprintf("NOTIFACATION of %d:\n", value->handle);
            print_hex_table(value->value, value_size);
        }
        break;
    case GATT_EVENT_INDICATION:
        value = gatt_event_indication_parse(packet, size, &value_size);
        if (value_size)
        {
            iprintf("INDICATION of %d:\n", value->handle);
            print_hex_table(value->value, value_size);
        }
        break;
    }
}

#define USER_MSG_START_ADV          0
#define USER_MSG_STOP_ADV           1
#define USER_MSG_UPDATE_ADV_DATA    2
#define USER_MSG_UPDATE_ADDR        3
#define USER_MSG_CONN_TO_SLAVE      4
#define USER_MSG_CONN_CANCEL        5
#define USER_MSG_READ_CHAR          6
#define USER_MSG_WRITE_CHAR         7
#define USER_MSG_WOR_CHAR           8
#define USER_MSG_SUB_TO_CHAR        9
#define USER_MSG_UNSUB_TO_CHAR      10
#define USER_MSG_SET_BONDING        11

const static ext_adv_set_en_t adv_sets_en[] = {{.handle = 0, .duration = 0, .max_events = 0}};

#define CONN_PARAM  {                   \
            .scan_int = 200,            \
            .scan_win = 100,            \
            .interval_min = 50,         \
            .interval_max = 50,         \
            .latency = 0,               \
            .supervision_timeout = 600, \
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

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
    case USER_MSG_START_ADV:
        gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
        printf("adv started\n");
        break;
    case USER_MSG_STOP_ADV:
        gap_set_ext_adv_enable(0, 0, NULL);
        printf("adv stopped\n");
        break;
    case USER_MSG_UPDATE_ADV_DATA:
        do_set_data();
        printf("adv updated\n");
        break;
    case USER_MSG_UPDATE_ADDR:
        gap_set_adv_set_random_addr(0, sm_persistent.identity_addr);
        printf("addr changed: %02X:%02X:%02X:%02X:%02X:%02X\n",
                sm_persistent.identity_addr[0], sm_persistent.identity_addr[1],
                sm_persistent.identity_addr[2], sm_persistent.identity_addr[3],
                sm_persistent.identity_addr[4], sm_persistent.identity_addr[5]);
        break;
    case USER_MSG_CONN_TO_SLAVE:
        printf("create connection...\n");
        gap_ext_create_connection(INITIATING_ADVERTISER_FROM_PARAM,
                                                  BD_ADDR_TYPE_LE_RANDOM,           // Own_Address_Type,
                                                  slave_addr_type,                  // Peer_Address_Type,
                                                  slave_addr,                       // Peer_Address,
                                                  sizeof(phy_configs) / sizeof(phy_configs[0]),
                                                  phy_configs);
        break;
    case USER_MSG_CONN_CANCEL:
        printf("create connection cancelled.\n");
        gap_create_connection_cancel();
        break;
    case USER_MSG_READ_CHAR:
        {
            char_node_t *c = find_char(size);
            if (NULL == c)
            {
                iprintf("CHAR not found: %d\n", size);
                break;
            }

            gatt_client_read_value_of_characteristic_using_value_handle(
                read_characteristic_value_callback,
                conn_handle,
                c->chara.value_handle);
        }
        break;
    case USER_MSG_WRITE_CHAR:
        {
            char_node_t *c = find_char(size);
            block_value_t *v = (block_value_t *)(data);
            if (NULL == c)
            {
                iprintf("CHAR not found: %d\n", size);
                break;
            }

            gatt_client_write_value_of_characteristic(
                write_characteristic_value_callback,
                conn_handle,
                c->chara.value_handle,
                v->len,
                v->value);
        }
        break;
    case USER_MSG_WOR_CHAR:
        {
            char_node_t *c = find_char(size);
            block_value_t *v = (block_value_t *)(data);
            if (NULL == c)
            {
                iprintf("CHAR not found: %d\n", size);
                break;
            }

            gatt_client_write_value_of_characteristic_without_response(
                conn_handle,
                c->chara.value_handle,
                v->len,
                v->value);
        }
        break;
    case USER_MSG_SUB_TO_CHAR:
        {
            uint16_t config = GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION;
            char_node_t *c = find_char(size);
            desc_node_t *d = find_config_desc(c);
            if (NULL == d)
            {
                iprintf("CHAR/DESC not found: %d\n", size);
                break;
            }

            if (c->notification == NULL)
            {
                c->notification = (gatt_client_notification_t *)malloc(sizeof(gatt_client_notification_t));
                gatt_client_listen_for_characteristic_value_updates(
                    c->notification, output_notification_handler,
                    conn_handle, c->chara.value_handle);
            }

            gatt_client_write_characteristic_descriptor_using_descriptor_handle(
                write_characteristic_descriptor_callback,
                conn_handle,
                d->desc.handle,
                sizeof(config),
                (uint8_t *)&config);
        }
        break;
    case USER_MSG_UNSUB_TO_CHAR:
        {
            uint16_t config = 0;
            char_node_t *c = find_char(size);
            desc_node_t *d = find_config_desc(c);
            if (NULL == d)
            {
                iprintf("CHAR/DESC not found: %d\n", size);
                break;
            }

            gatt_client_write_characteristic_descriptor_using_descriptor_handle(
                write_characteristic_descriptor_callback,
                conn_handle,
                d->desc.handle,
                sizeof(config),
                (uint8_t *)&config);
        }
        break;
    case USER_MSG_SET_BONDING:
        bonding_flag = size;
        if (bonding_flag)
        {
            //sm_set_authentication_requirements(SM_AUTHREQ_BONDING);

        }
        else
        {
            //sm_set_authentication_requirements(SM_AUTHREQ_NO_BONDING);
        }
        break;
    default:
        ;
    }
}

void set_adv_local_name(const char *name, int16_t len)
{
    adv_data[0] = len + 1;
    adv_data[1] = 0x9;              // Complete Local Name
    memcpy(adv_data + 2, name, len);
    btstack_push_user_msg(USER_MSG_UPDATE_ADV_DATA, NULL, 0);
}

void set_bonding(int flag)
{
    btstack_push_user_msg(USER_MSG_SET_BONDING, NULL, flag);
}

void start_adv()
{
    btstack_push_user_msg(USER_MSG_START_ADV, NULL, 0);
}

void stop_adv()
{
    btstack_push_user_msg(USER_MSG_STOP_ADV, NULL, 0);
}

void update_addr()
{
    btstack_push_user_msg(USER_MSG_UPDATE_ADDR, NULL, 0);
}

void conn_to_slave()
{
    btstack_push_user_msg(USER_MSG_CONN_TO_SLAVE, NULL, 0);
}

void cancel_create_conn()
{
    btstack_push_user_msg(USER_MSG_CONN_CANCEL, NULL, 0);
}

void read_value_of_char(int handle)
{
    btstack_push_user_msg(USER_MSG_READ_CHAR, NULL, (uint16_t)handle);
}

void write_value_of_char(int handle, block_value_t *value)
{
    btstack_push_user_msg(USER_MSG_WRITE_CHAR, value, (uint16_t)handle);
}

void wor_value_of_char(int handle, block_value_t *value)
{
    btstack_push_user_msg(USER_MSG_WOR_CHAR, value, (uint16_t)handle);
}

void sub_to_char(int handle)
{
    btstack_push_user_msg(USER_MSG_SUB_TO_CHAR, NULL, (uint16_t)handle);
}

void unsub_to_char(int handle)
{
    btstack_push_user_msg(USER_MSG_UNSUB_TO_CHAR, NULL, (uint16_t)handle);
}

#pragma pack (push, 1)
typedef struct read_remote_version
{
    uint8_t Status;
    uint16_t Connection_Handle;
    uint8_t Version;
    uint16_t Manufacturer_Name;
    uint16_t Subversion;
} read_remote_version_t;
#pragma pack (pop)

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    uint8_t event = hci_event_packet_get_type(packet);
    const btstack_user_msg_t *p_user_msg;
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;
        gap_set_random_device_address(sm_persistent.identity_addr);
        gap_set_adv_set_random_addr(0, sm_persistent.identity_addr);
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
        do_set_data();
        gap_set_ext_scan_response_data(0, sizeof(scan_data), (uint8_t*)scan_data);
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
            {
                const le_meta_event_enh_create_conn_complete_t * complete =
                    decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t);
                conn_handle = complete->handle;
                if (HCI_ROLE_SLAVE == complete->role)
                    att_set_db(conn_handle, profile_data);
                else
                    gap_read_remote_info(conn_handle);
            }
            break;
        case HCI_SUBEVENT_LE_READ_REMOTE_USED_FEATURES_COMPLETE:
            if (0 == bonding_flag)
            {
                iprintf("discovering...\n");
                gatt_client_discover_primary_services(service_discovery_callback, conn_handle);
            }
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_ENCRYPTION_CHANGE:
        {
            iprintf("discovering...\n");
            gatt_client_discover_primary_services(service_discovery_callback, conn_handle);
        }
        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        iprintf("disconnected\n");
        conn_handle = INVALID_HANDLE;
        free_services();
        break;

    case HCI_EVENT_COMMAND_COMPLETE:
        {
            const uint8_t *returns = hci_event_command_complete_get_return_parameters(packet);
            if (*returns != 0)
                platform_printf("COMMAND_COMPLETE: 0x%02x for OPCODE %04X\n",
                    *returns, hci_event_command_complete_get_command_opcode(packet));
        }
        break;
    case HCI_EVENT_COMMAND_STATUS:
        {
            const uint8_t status = hci_event_command_status_get_status(packet);
            if (status != 0)
                platform_printf("COMMAND_STATUS: 0x%02x for OPCODE %04X\n",
                    status, hci_event_command_status_get_command_opcode(packet));
        }
        break;
    case HCI_EVENT_READ_REMOTE_VERSION_INFORMATION_COMPLETE:
        {
            const read_remote_version_t *version = decode_event_offset(packet, read_remote_version_t, 2);
            if (version->Status == 0)
            {
                platform_printf("Remote version\n"
                "Version          : %d\n"
                "Manufacturer Name: 0x%04X\n"
                "Subversion       : 0x%04X\n", version->Version, version->Manufacturer_Name, version->Subversion);
            }
            gap_read_remote_used_features(conn_handle);
        }
        break;

    case GATT_EVENT_MTU:
        iprintf("GATT client MTU updated: %d\n", gatt_event_mtu_get_mtu(packet));
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

static void sm_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    uint8_t event = hci_event_packet_get_type(packet);

    if (packet_type != HCI_EVENT_PACKET) return;
    if (0 == bonding_flag) return;

    platform_printf("SM: %d\n", event);
    switch (event)
    {
    case SM_EVENT_JUST_WORKS_REQUEST:
        iprintf("JUST_WORKS confirmed\n");
        sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
        break;
    case SM_EVENT_PASSKEY_DISPLAY_NUMBER:
        platform_printf("===================\npasskey: %06d\n===================\n",
            sm_event_passkey_display_number_get_passkey(packet));
        break;
    case SM_EVENT_PASSKEY_DISPLAY_CANCEL:
        platform_printf("TODO: DISPLAY_CANCEL\n");
        break;
    case SM_EVENT_PASSKEY_INPUT_NUMBER:
        // TODO: intput number
        platform_printf("===================\ninput number:\n");
        break;
    case SM_EVENT_PASSKEY_INPUT_CANCEL:
        platform_printf("TODO: INPUT_CANCEL\n");
        break;
    case SM_EVENT_IDENTITY_RESOLVING_FAILED:
        platform_printf("not authourized\n");
        if (bonding_flag)
        {
            iprintf("paring...\n");
            sm_request_pairing(conn_handle == INVALID_HANDLE ? 0 : conn_handle);
        }
        break;
    case SM_EVENT_IDENTITY_RESOLVING_SUCCEEDED:
        gatt_client_discover_primary_services(service_discovery_callback, conn_handle);
        break;
    default:
        break;
    }
}

static btstack_packet_callback_registration_t sm_event_callback_registration  = {.callback = &sm_packet_handler};

uint32_t setup_profile(void *data, void *user_data)
{
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    gatt_client_register_handler(&user_packet_handler);
    sm_add_event_handler(&sm_event_callback_registration);
    sm_config(IO_CAPABILITY_NO_INPUT_NO_OUTPUT,
              0,
              &sm_persistent);
    sm_add_event_handler(&sm_event_callback_registration);
    return 0;
}

