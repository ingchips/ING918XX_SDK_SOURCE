#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gatt_client_util.h"
#include "platform_api.h"
#include "att_db.h"

#include "btstack_event.h"
#include "btstack_defines.h"
#include "sig_uuid.h"

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
            con_handle = result->handle;
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
