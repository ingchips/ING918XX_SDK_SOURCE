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
} gatt_client_discoverer_t;

static gatt_client_discoverer_t *discoverer = NULL;

#define first_service       (discoverer->first)
#define cur_disc_service    (discoverer->cur_disc_service)
#define cur_disc_char       (discoverer->cur_disc_char)

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

void free_services(void)
{
    if (NULL == first_service.next) return;
    list_for_each((struct list_node *)first_service.next, (f_list_node)free_service);
    first_service.next = NULL;
}

static service_node_t *get_last_service(void)
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

void gatt_client_util_dump_profile(service_node_t *first)
{
    service_node_t *s = first;

    iprintf("========== CLONING GATT PROFILE ==========\n\n");

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

static void discover_char_of_service(void)
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
        discoverer->on_fully_discovered(discoverer->first.next);
        discoverer = NULL;
    }
}

static void discover_desc_of_char(void)
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

static void descriptor_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
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

static void characteristic_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
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

static void service_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
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

struct gatt_client_discoverer *gatt_client_util_discover_all(hci_con_handle_t con_handle, f_on_fully_discovered on_fully_discovered)
{
    struct gatt_client_discoverer *r = malloc(sizeof(gatt_client_discoverer_t));
    memset(r, 0, sizeof(*r));
    r->con_handle = con_handle;
    r->on_fully_discovered = on_fully_discovered;
    discoverer = r;
    gatt_client_discover_primary_services(service_discovery_callback, con_handle);    
    return r;
}

void gatt_client_util_free(struct gatt_client_discoverer *ctx)
{
    discoverer = ctx;
    free_services();
    free(ctx);
    discoverer = NULL;
}

service_node_t *gatt_client_util_get_first_service(struct gatt_client_discoverer *ctx)
{
    return ctx->first.next;
}
