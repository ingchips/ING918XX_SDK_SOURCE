#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gatt_client_util.h"
#include "platform_api.h"
#include "att_db.h"

#include "btstack_event.h"
#include "btstack_defines.h"
#include "sig_uuid.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#ifndef iprintf
#define iprintf(...)
#endif

typedef struct gatt_client_discoverer
{
    hci_con_handle_t con_handle;
    service_node_t first;
    service_node_t *cur_disc_service;
    char_node_t *cur_disc_char;
    f_on_fully_discovered on_fully_discovered;
    void *user_data;
} gatt_client_discoverer_t;

#ifndef GATT_UTIL_MAX_DISCOVERER_NUM
#define GATT_UTIL_MAX_DISCOVERER_NUM 8
#endif

static gatt_client_discoverer_t *discoverers[GATT_UTIL_MAX_DISCOVERER_NUM] = {NULL};

#define first_service       (discoverers[con_handle]->first)
#define cur_disc_service    (discoverers[con_handle]->cur_disc_service)
#define cur_disc_char       (discoverers[con_handle]->cur_disc_char)

struct list_node
{
    struct list_node *next;
};

typedef void (*f_list_node)(struct list_node *node);

static void list_for_each(struct list_node *head, f_list_node f)
{
    while (head)
    {
        struct list_node *t = head;
        head = head->next;
        f(t);
    }
}

static void free_list(struct list_node *head)
{
    list_for_each(head, (f_list_node)free);
}

static void free_char(char_node_t *a_char)
{
    if (a_char == NULL) return;
    free_list((struct list_node *)a_char->descs);
    if (a_char->notification) free(a_char->notification);
    free(a_char);
}

static void free_service(service_node_t *s)
{
    if (s == NULL) return;
    list_for_each((struct list_node *)s->chars, (f_list_node)free_char);
    free(s);
}

void free_services(gatt_client_discoverer_t *discoverer)
{
    if (NULL == discoverer->first.next) return;
    list_for_each((struct list_node *)discoverer->first.next, (f_list_node)free_service);
    discoverer->first.next = NULL;
}

static service_node_t *get_last_service(uint16_t con_handle)
{
    service_node_t *r = &first_service;
    while (r->next)
        r = r->next;
    return r;
}

char_node_t *gatt_client_util_find_char(gatt_client_discoverer_t *discoverer, uint16_t handle)
{
    service_node_t *s = discoverer->first.next;
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

static int is_sig_uuid(const uint8_t *uuid, uint16_t sig_id)
{
    if (uuid_has_bluetooth_prefix(uuid))
    {
        uint16_t v = (uuid[2] << 8) | uuid[3];
        return v == sig_id;
    }
    else
        return 0;
}

desc_node_t *gatt_client_util_find_config_desc(char_node_t *c)
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

void gatt_client_util_print_uuid(const uint8_t *uuid)
{
    if (uuid_has_bluetooth_prefix(uuid))
    {
        iprintf("0x%04x", (uuid[2] << 8) | uuid[3]);
    }
    else
        iprintf("{%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
                        uuid[0], uuid[1], uuid[2], uuid[3],
                        uuid[4], uuid[5], uuid[6], uuid[7], uuid[8], uuid[9],
                        uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);
}

void gatt_client_util_print_properties(uint16_t v)
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

void gatt_client_util_dump_profile(service_node_t *first, void *user_data, int err_code)
{
    service_node_t *s = first;

    iprintf("========== DUMPING GATT PROFILE ==========\n\n");

    while (s)
    {
        char_node_t *c = s->chars;
        iprintf("SERVICE: ");
        gatt_client_util_print_uuid(s->service.uuid128);
        iprintf("\nHANDLE RANGE: %d - %d\n\n", s->service.start_group_handle, s->service.end_group_handle);

        while (c)
        {
            iprintf("    CHARACTERISTIC: ");
            gatt_client_util_print_uuid(c->chara.uuid128);
            iprintf("\n        HANDLE RANGE: %d - %d\n", c->chara.start_handle, c->chara.end_handle);
            iprintf(  "        VALUE HANDLE: %d\n", c->chara.value_handle);
            iprintf(  "        PROPERTIES  : "); gatt_client_util_print_properties(c->chara.properties); iprintf("\n\n");

            desc_node_t *d = c->descs;
            while (d)
            {
                iprintf("        DESCRIPTOR: ");
                gatt_client_util_print_uuid(d->desc.uuid128);
                iprintf("\n        HANDLE: %d\n\n", d->desc.handle);

                d = d->next;
            }
            c = c->next;
        }
        s = s->next;
    }
}

static void characteristic_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size);
static void descriptor_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size);

static void discover_char_of_service(uint16_t con_handle)
{
    if (cur_disc_service)
    {
        gatt_client_discover_characteristics_for_service(characteristic_discovery_callback,
                                                         con_handle,
                                                         cur_disc_service->service.start_group_handle,
                                                         cur_disc_service->service.end_group_handle);
    }
    else
    {
        iprintf("all discovered\n");
        discoverers[con_handle]->on_fully_discovered(discoverers[con_handle]->first.next, discoverers[con_handle]->user_data, 0);
        discoverers[con_handle] = NULL;
    }
}

static void discover_desc_of_char(uint16_t con_handle)
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
                                                        con_handle,
                                                        &cur_disc_char->chara);
    }
    else
    {
        cur_disc_service = cur_disc_service->next;
        discover_char_of_service(con_handle);
    }
}

#define CHECK_STATUS()                                                                              \
        if (gatt_event_query_complete_parse(packet)->status != 0)                                   \
        {                                                                                           \
            iprintf("failed: %08x\n", discoverers[con_handle]);                                     \
            discoverers[con_handle]->on_fully_discovered(discoverers[con_handle]->first.next,       \
                                                         discoverers[con_handle]->user_data,        \
                                                         gatt_event_query_complete_parse(packet)->status); \
            discoverers[con_handle] = NULL;                                                         \
            break;                                                                                  \
        }

static void descriptor_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    uint16_t con_handle;
    switch (packet[0])
    {
    case GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT:
        {
            const gatt_event_all_characteristic_descriptors_query_result_t *result =
                gatt_event_all_characteristic_descriptors_query_result_parse(packet);
            con_handle = result->handle;
            desc_node_t *node = (desc_node_t *)malloc(sizeof(desc_node_t));
            node->next = cur_disc_char->descs;
            cur_disc_char->descs = node;

            node->desc = result->descriptor;
        }
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        {
            con_handle = gatt_event_query_complete_parse(packet)->handle;
            iprintf("descriptor_discovery COMPLETE: %d\n", gatt_event_query_complete_parse(packet)->status);
            CHECK_STATUS();

            cur_disc_char = cur_disc_char->next;
            if (cur_disc_char)
            {
                discover_desc_of_char(con_handle);
            }
            else
            {
                cur_disc_service = cur_disc_service->next;
                discover_char_of_service(con_handle);
            }
        }
        break;
    }
}

static void characteristic_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    uint16_t con_handle;
    switch (packet[0])
    {
    case GATT_EVENT_CHARACTERISTIC_QUERY_RESULT:
        {
            const gatt_event_characteristic_query_result_t *result = gatt_event_characteristic_query_result_parse(packet);
            uint16_t con_handle = result->handle;
            char_node_t *node = (char_node_t *)malloc(sizeof(char_node_t));
            node->next = cur_disc_service->chars;
            cur_disc_service->chars = node;

            node->descs = NULL;
            node->notification = NULL;
            node->chara = result->characteristic;
        }
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        con_handle = gatt_event_query_complete_parse(packet)->handle;
        iprintf("characteristic_discovery COMPLETE: %d\n", gatt_event_query_complete_parse(packet)->status);
        CHECK_STATUS();

        cur_disc_char = cur_disc_service->chars;
        discover_desc_of_char(con_handle);
        break;
    }
}

static void service_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    uint16_t con_handle;
    switch (packet[0])
    {
    case GATT_EVENT_SERVICE_QUERY_RESULT:
        {
            const gatt_event_service_query_result_t *result = gatt_event_service_query_result_parse(packet);
            con_handle = result->handle;
            service_node_t *last = get_last_service(con_handle);
            last->next = (service_node_t *)malloc(sizeof(service_node_t));
            last->next->chars = NULL;
            last->next->next = NULL;
            last->next->service = result->service;
        }
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        con_handle = gatt_event_query_complete_parse(packet)->handle;
        iprintf("service_discovery COMPLETE: %d\n", gatt_event_query_complete_parse(packet)->status);
        CHECK_STATUS();

        cur_disc_service = first_service.next;
        if (cur_disc_service)
            gatt_client_discover_characteristics_for_service(characteristic_discovery_callback,
                                                             con_handle,
                                                             cur_disc_service->service.start_group_handle,
                                                             cur_disc_service->service.end_group_handle);
        break;
    }
}

struct gatt_client_discoverer *gatt_client_util_discover_all(hci_con_handle_t con_handle, f_on_fully_discovered on_fully_discovered, void *user_data)
{
    if ((con_handle >= GATT_UTIL_MAX_DISCOVERER_NUM) || discoverers[con_handle]) platform_raise_assertion("gatt_client_util.c", __LINE__);

    struct gatt_client_discoverer *r = malloc(sizeof(gatt_client_discoverer_t));
    memset(r, 0, sizeof(*r));
    r->con_handle = con_handle;
    r->on_fully_discovered = on_fully_discovered;
    r->user_data = user_data;
    discoverers[con_handle] = r;
    gatt_client_discover_primary_services(service_discovery_callback, con_handle);
    return r;
}

void gatt_client_util_free(struct gatt_client_discoverer *ctx)
{
    if (ctx == NULL) return;
    free_services(ctx);
    free(ctx);
}

service_node_t *gatt_client_util_get_first_service(struct gatt_client_discoverer *ctx)
{
    return ctx->first.next;
}

char_node_t *gatt_client_util_find_char_uuid128(struct gatt_client_discoverer *discoverer,
                                                const uint8_t *uuid)
{
    service_node_t *s = discoverer->first.next;
    while (s)
    {
        char_node_t *c = s->chars;
        while (c)
        {
            if (memcmp(c->chara.uuid128, uuid, sizeof(c->chara.uuid128)) == 0)
                return c;
            c = c->next;
        }
        s = s->next;
    }
    return NULL;
}

char_node_t *gatt_client_util_find_char_uuid16(struct gatt_client_discoverer *discoverer,
                                                const uint16_t uuid)
{
    service_node_t *s = discoverer->first.next;
    while (s)
    {
        char_node_t *c = s->chars;
        while (c)
        {
            if (is_sig_uuid(c->chara.uuid128, uuid))
                return c;
            c = c->next;
        }
        s = s->next;
    }
    return NULL;

}

struct gatt_msg_enque_runnable
{
    f_gatt_client_synced_runnable runnable;
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

enum gatt_msg_id
{
    MSG_ENQUE_RUNNABLE,
    MSG_DISCOVER_ALL,
    MSG_READ_CHAR_VALUE,
    MSG_READ_DISCRIPTOR,
    MSG_WRITE_CHAR_VALUE,
    MSG_WRITE_CHAR_VALUE_WO_RSP,
    MSG_WRITE_DISCRIPTOR,
};

struct gatt_client_synced_runner
{
    f_gatt_client_synced_push_user_msg push_user_msg;
    QueueHandle_t msg_queue;
    SemaphoreHandle_t done_sem;

    struct gatt_msg_discover_all    discover_all;
    struct gatt_msg_read_value      read_value;
    struct gatt_msg_write_value     write_value;
};

static struct gatt_client_synced_runner *runners[GATT_UTIL_MAX_DISCOVERER_NUM] = {NULL};

typedef struct gatt_synced_msg
{
    struct gatt_msg_enque_runnable  runnable;
} gatt_synced_msg_t;

static void synced_runner_task(struct gatt_client_synced_runner *runner)
{
    for (;;)
    {
        gatt_synced_msg_t msg;
        if (xQueueReceive(runner->msg_queue, &msg, portMAX_DELAY) != pdPASS) continue;
        msg.runnable.runnable(msg.runnable.user_data);
    }
}

struct gatt_client_synced_runner *gatt_client_create_sync_runner(f_gatt_client_synced_push_user_msg push_user_msg)
{
    struct gatt_client_synced_runner *runner =
        (struct gatt_client_synced_runner *)malloc(sizeof(struct gatt_client_synced_runner));
    memset(runner, 0, sizeof(*runner));
    runner->push_user_msg = push_user_msg;
    runner->msg_queue = xQueueCreate(1, sizeof(gatt_synced_msg_t));
    runner->done_sem = xSemaphoreCreateBinary();
    xTaskCreate((TaskFunction_t)synced_runner_task,
               "b",
               GATT_CLIENT_SYNC_RUNNER_STACK_SIZE,
               runner,
               5,
               NULL);
    return runner;
}

int gatt_client_sync_run(struct gatt_client_synced_runner *runner, f_gatt_client_synced_runnable runnable, void *user_data)
{
    gatt_synced_msg_t msg =
    {
        .runnable =
        {
            .runnable = runnable,
            .user_data = user_data
        }
    };
    if (IS_IN_INTERRUPT())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        return xQueueSendToBackFromISR(runner->msg_queue, &msg, &xHigherPriorityTaskWoken) == pdTRUE ? 0 : 1;
    }
    else
        return xQueueSendToBack(runner->msg_queue, &msg, portMAX_DELAY) == pdTRUE ? 0 : 1;
}

static int gatt_client_sync_wait_done(struct gatt_client_synced_runner *runner)
{
    int8_t err;
    if (IS_IN_INTERRUPT())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        err = xSemaphoreTakeFromISR(runner->done_sem, &xHigherPriorityTaskWoken);

    }
    else
        err = xSemaphoreTake(runner->done_sem, portMAX_DELAY);
    return err == pdTRUE ? 0 : 1;
}

static void gatt_client_sync_mark_done(struct gatt_client_synced_runner *runner)
{
    xSemaphoreGive(runner->done_sem);
}

static void gatt_client_sync_disovered(service_node_t *first, void *user_data, int err_code)
{
    struct gatt_client_synced_runner *runner = (struct gatt_client_synced_runner *)user_data;
    *runner->discover_all.err_code = err_code;
    gatt_client_sync_mark_done(runner);
}

static void read_value_callback(uint8_t packet_type, uint16_t con_handle, const uint8_t *packet, uint16_t size)
{
    struct gatt_client_synced_runner *runner = runners[con_handle];
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
                runner->read_value.err_code = GATT_CLIENT_SYNCED_ERROR_BUFFER_TOO_SMALL;
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
    struct gatt_client_synced_runner *runner = runners[con_handle];
    switch (packet[0])
    {
    case GATT_EVENT_QUERY_COMPLETE:
        gatt_client_sync_mark_done(runner);
        break;
    }
}

void gatt_client_sync_handle_msg(struct gatt_client_synced_runner *runner, uint8_t msg_id)
{
    switch (msg_id)
    {
    case MSG_DISCOVER_ALL:
        runner->discover_all.r = gatt_client_util_discover_all(runner->discover_all.con_handle,
                                    gatt_client_sync_disovered, runner);
        break;
    case MSG_READ_CHAR_VALUE:
        runners[runner->discover_all.con_handle] = runner;
        runner->read_value.err_code = gatt_client_read_value_of_characteristic_using_value_handle(
                read_value_callback,
                runner->discover_all.con_handle,
                runner->read_value.handle);
        if (runner->read_value.err_code)
            gatt_client_sync_mark_done(runner);
        break;
    case MSG_READ_DISCRIPTOR:
        runners[runner->discover_all.con_handle] = runner;
        runner->read_value.err_code = gatt_client_read_characteristic_descriptor_using_descriptor_handle(
                read_value_callback,
                runner->discover_all.con_handle,
                runner->read_value.handle);
        if (runner->read_value.err_code)
            gatt_client_sync_mark_done(runner);
        break;
    case MSG_WRITE_CHAR_VALUE:
        runners[runner->discover_all.con_handle] = runner;
        runner->write_value.err_code = gatt_client_write_value_of_characteristic(
                write_value_callback,
                runner->discover_all.con_handle,
                runner->write_value.handle,
                runner->write_value.length,
                runner->write_value.data);
        if (runner->read_value.err_code)
            gatt_client_sync_mark_done(runner);
        break;
    case MSG_WRITE_CHAR_VALUE_WO_RSP:
        runner->write_value.err_code = gatt_client_write_value_of_characteristic_without_response(
                runner->discover_all.con_handle,
                runner->write_value.handle,
                runner->write_value.length,
                runner->write_value.data);
        gatt_client_sync_mark_done(runner);
        break;

    case MSG_WRITE_DISCRIPTOR:
        runners[runner->discover_all.con_handle] = runner;
        runner->write_value.err_code = gatt_client_write_characteristic_descriptor_using_descriptor_handle(
                write_value_callback,
                runner->discover_all.con_handle,
                runner->write_value.handle,
                runner->write_value.length,
                (uint8_t *)runner->write_value.data);
        if (runner->read_value.err_code)
            gatt_client_sync_mark_done(runner);
        break;
    }
}

struct gatt_client_discoverer * gatt_client_sync_discover_all(struct gatt_client_synced_runner *runner,
    hci_con_handle_t con_handle, int *err_code)
{
    runner->discover_all.con_handle = con_handle;
    runner->discover_all.err_code = err_code;
    runner->discover_all.r = NULL;
    runner->push_user_msg(runner, MSG_DISCOVER_ALL);

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

int gatt_client_sync_read_value_of_characteristic(struct gatt_client_synced_runner *runner,
    hci_con_handle_t con_handle,
    uint16_t characteristic_value_handle, uint8_t *data, uint16_t *length)
{
    runner->read_value.con_handle = con_handle;
    runner->read_value.handle = characteristic_value_handle;
    runner->read_value.data = data;
    runner->read_value.length = length;
    runner->push_user_msg(runner, MSG_READ_CHAR_VALUE);

    return gatt_client_sync_wait_done(runner) ? GATT_CLIENT_SYNCED_ERROR_RTOS : runner->read_value.err_code;
}

int gatt_client_sync_read_characteristic_descriptor(struct gatt_client_synced_runner *runner,
    hci_con_handle_t con_handle,
    uint16_t descriptor_handle, uint8_t *data, uint16_t *length)
{
    runner->read_value.con_handle = con_handle;
    runner->read_value.handle = descriptor_handle;
    runner->read_value.data = data;
    runner->read_value.length = length;
    runner->push_user_msg(runner, MSG_READ_DISCRIPTOR);

    return gatt_client_sync_wait_done(runner) ? GATT_CLIENT_SYNCED_ERROR_RTOS : runner->read_value.err_code;
}

int gatt_client_sync_write_value_of_characteristic(struct gatt_client_synced_runner *runner,
    hci_con_handle_t con_handle,
    uint16_t characteristic_value_handle, const uint8_t *data, uint16_t length)
{
    runner->write_value.con_handle = con_handle;
    runner->write_value.handle = characteristic_value_handle;
    runner->write_value.data = data;
    runner->write_value.length = length;
    runner->push_user_msg(runner, MSG_WRITE_CHAR_VALUE);

    return gatt_client_sync_wait_done(runner) ? GATT_CLIENT_SYNCED_ERROR_RTOS : runner->write_value.err_code;
}

int gatt_client_sync_write_value_of_characteristic_without_response(struct gatt_client_synced_runner *runner,
    hci_con_handle_t con_handle,
    uint16_t characteristic_value_handle, const uint8_t *data, uint16_t length)
{
    runner->write_value.con_handle = con_handle;
    runner->write_value.handle = characteristic_value_handle;
    runner->write_value.data = data;
    runner->write_value.length = length;
    runner->push_user_msg(runner, MSG_WRITE_CHAR_VALUE_WO_RSP);

    return gatt_client_sync_wait_done(runner) ? GATT_CLIENT_SYNCED_ERROR_RTOS : runner->write_value.err_code;
}

int gatt_client_sync_write_characteristic_descriptor(struct gatt_client_synced_runner *runner,
    hci_con_handle_t con_handle,
    uint16_t descriptor_handle, const uint8_t *data, uint16_t length)
{
    runner->write_value.con_handle = con_handle;
    runner->write_value.handle = descriptor_handle;
    runner->write_value.data = data;
    runner->write_value.length = length;
    runner->push_user_msg(runner, MSG_WRITE_DISCRIPTOR);

    return gatt_client_sync_wait_done(runner) ? GATT_CLIENT_SYNCED_ERROR_RTOS : runner->write_value.err_code;
}
