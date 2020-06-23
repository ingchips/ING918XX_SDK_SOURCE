#include "ancs.h"
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include "gatt_client.h"
#include <string.h>
#include <stdio.h>

/*******************************************************************************
* Macros
*******************************************************************************/
#define ANCS_NAME_MAX_LENGTH_MSB            (0)
#define ANCS_NAME_MAX_LENGTH_LSB            (20)

/*******************************************************************************
* Enums for ANCS Service
*******************************************************************************/

typedef enum
{
    ANCS_CATEGORY_ID_OTHER                  = 0,
    ANCS_CATEGORY_ID_INCOMING_CALL,
    ANCS_CATEGORY_ID_MISSED_CALL,
    ANCS_CATEGORY_ID_VOICEMAIL,
    ANCS_CATEGORY_ID_SOCIAL,
    ANCS_CATEGORY_ID_SCHEDULE               = 5,
    ANCS_CATEGORY_ID_EMAIL,
    ANCS_CATEGORY_ID_NEWS,
    ANCS_CATEGORY_ID_HEALTH_AND_FITNESS,
    ANCS_CATEGORY_ID_BUSINESS_AND_FINANCE,
    ANCS_CATEGORY_ID_LOCATION,
    ANCS_CATEGORY_ID_ENTERTAINMENT          = 11
} ANCS_CATEGORY_ID_VALUES;

typedef enum
{
    ANCS_EVENT_ID_NOTIFICATION_ADDED        = 0,
    ANCS_EVENT_ID_NOTIFICATION_MODIFIED,
    ANCS_EVENT_ID_NOTIFICATION_REMOVED
} ANCS_EVENT_ID_VALUES;

typedef enum
{
    ANCS_EVENT_FLAG_SILENT          = (1 << 0),
    ANCS_EVENT_FLAG_IMPORTANT       = (1 << 1),
    ANCS_EVENT_FLAG_PRE_EXISTING    = (1 << 2),
    ANCS_EVENT_FLAG_POSITIVE_ACTION = (1 << 3),
    ANCS_EVENT_FLAG_NEGATIVE_ACTION = (1 << 4)
} ANCS_EVENT_FLAGS;

typedef enum
{
    ANCS_COMMAND_ID_GET_NOTIFICATION_ATTRIBUTES = 0,
    ANCS_COMMAND_ID_GET_APP_ATTRIBUTES,
    ANCS_COMMAND_ID_PERFORM_NOTIFICATION_ACTION
} ANCS_COMMAND_ID_VALUES;

typedef enum
{
    ANCS_NOTIFICATION_ATTRIBUTE_ID_APP_IDENTIFIER        = 0,
    ANCS_NOTIFICATION_ATTRIBUTE_ID_TITLE,
    ANCS_NOTIFICATION_ATTRIBUTE_ID_SUBTITLE,
    ANCS_NOTIFICATION_ATTRIBUTE_ID_MESSAGE,
    ANCS_NOTIFICATION_ATTRIBUTE_ID_MESSAGE_SIZE,
    ANCS_NOTIFICATION_ATTRIBUTE_ID_DATE                  = 5,
    ANCS_NOTIFICATION_ATTRIBUTE_ID_POSITIVE_ACTION_LABEL,
    ANCS_NOTIFICATION_ATTRIBUTE_ID_NEGATIVE_ACTION_LABEL
} ANCS_NOTIFICATION_ATTRIBUTE_ID_VALUES;

typedef enum
{
    ANCS_APP_ATTRIBUTE_ID_DISPLAY_NAME = 0
} ANCS_APP_ATTRIBUTE_ID_VALUES;


/*******************************************************************************
* ANCS Service, Characteristic and Descriptor UUIDs
*******************************************************************************/
const uint8_t UUID_ANCS_SERVICE[] = {0x79,0x05,0xF4,0x31,0xB5,0xCE,0x4E,0x99,
                                     0xA4,0x0F,0x4B,0x1E,0x12,0x2D,0x00,0xD0};

const uint8_t UUID_ANCS_NOTIF_SOURCE_CHAR[] = {0x9F,0xBF,0x12,0x0D,0x63,0x01,0x42,0xD9,
                                               0x8C,0x58,0x25,0xE6,0x99,0xA2,0x1D,0xBD};

const uint8_t UUID_ANCS_CTRL_POINT_CHAR[] = {0x69,0xD1,0xD8,0xF3,0x45,0xE1,0x49,0xA8,
                                             0x98,0x21,0x9B,0xBD,0xFD,0xAA,0xD9,0xD9};

const uint8_t UUID_ANCS_DATA_SOURCE_CHAR[] = {0x22,0xEA,0xC6,0xE9,0x24,0xD6,0x4B,0xB5,
                                              0xBE,0x44,0xB3,0x6A,0xCE,0x7C,0x7B,0xFB};

#define CCCD_UUID_16BIT   (0x2902)

/*******************************************************************************
* ANCS Notification Source characteristic notification data structure
*******************************************************************************/
typedef struct 
{
    uint8_t eventId;
    uint8_t eventFlags;
    uint8_t categoryId;
    uint8_t categoryCount;
    uint8_t notificationUid[4];
} ancs_notif_source_packet;

typedef struct
{
    gatt_client_service_t                   service;
    gatt_client_characteristic_t            ctrl_point_char;
    gatt_client_characteristic_t            notif_source_char;    
    gatt_client_characteristic_t            data_source_char;
    gatt_client_notification_t              notif_source_notify;
    gatt_client_notification_t              data_source_notify;
    uint16_t    conn_handle;
} ancs_info_t;

typedef struct
{
    int missed_call_count;
    int voice_mail_count;
    int email_count;    
    ancs_notif_source_packet notification;
    uint8_t wait_action;
    uint8_t action_notif_uid[4];
} ancs_status_t;

ancs_status_t ancs_status;

ancs_info_t ancs_info;

static void dummy_handler(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
}

// send a "Get Notification Attributes" command to the iOS
static void ancs_get_notification_attribute_title(const uint8_t * notificationUid)
{
    uint8_t writeData[8];

    writeData[0] = ANCS_COMMAND_ID_GET_NOTIFICATION_ATTRIBUTES;
    memcpy(writeData + 1, notificationUid, 4);
    writeData[5] = ANCS_NOTIFICATION_ATTRIBUTE_ID_TITLE;
    writeData[6] = ANCS_NAME_MAX_LENGTH_LSB;
    writeData[7] = ANCS_NAME_MAX_LENGTH_MSB;
    
    gatt_client_write_value_of_characteristic(dummy_handler, ancs_info.conn_handle, 
                                              ancs_info.ctrl_point_char.value_handle, sizeof(writeData), writeData);
}

static void ancs_get_notification_attribute_title_msg(const uint8_t * notificationUid)
{
    uint8_t writeData[11];

    writeData[0] = ANCS_COMMAND_ID_GET_NOTIFICATION_ATTRIBUTES;
    memcpy(writeData + 1, notificationUid, 4);
    writeData[5] = ANCS_NOTIFICATION_ATTRIBUTE_ID_TITLE;
    writeData[6] = ANCS_NAME_MAX_LENGTH_LSB;
    writeData[7] = ANCS_NAME_MAX_LENGTH_MSB;
    writeData[8] = ANCS_NOTIFICATION_ATTRIBUTE_ID_MESSAGE;
    writeData[9] = ANCS_NAME_MAX_LENGTH_LSB;
    writeData[10] = ANCS_NAME_MAX_LENGTH_MSB;
    
    gatt_client_write_value_of_characteristic(dummy_handler, ancs_info.conn_handle, 
                                              ancs_info.ctrl_point_char.value_handle, sizeof(writeData), writeData);
}

static void ancs_perform_notification_action(const uint8_t * notificationUid, ANCS_ACTION_ID_VALUES actionId)
{
    uint8_t writeData[6];

    writeData[0] = ANCS_COMMAND_ID_PERFORM_NOTIFICATION_ACTION;
    memcpy(writeData + 1, notificationUid, 4);
    writeData[5] = actionId;
    
    gatt_client_write_value_of_characteristic(dummy_handler, ancs_info.conn_handle, 
                                              ancs_info.ctrl_point_char.value_handle, sizeof(writeData), writeData);
}

int ancs_user_action(ANCS_ACTION_ID_VALUES actionId)
{
    if (ancs_status.wait_action == 0) return 1;
    ancs_status.wait_action = 0;
    ancs_perform_notification_action(ancs_status.action_notif_uid, actionId);
    return 0;
}

static void ancs_update_info(void)
{
    printf("Missed calls: %d\n"
           "Voicemails  : %d\n"
           "Emails      : %d",
           ancs_status.missed_call_count, ancs_status.voice_mail_count, ancs_status.email_count);
}

void ancs_handle_notifications(const ancs_notif_source_packet *source)
{
    memcpy(&ancs_status.notification, source, sizeof(ancs_status.notification));    
    switch(source->categoryId)
    {
    case ANCS_CATEGORY_ID_INCOMING_CALL:
        switch (source->eventId)
        {
        case ANCS_EVENT_ID_NOTIFICATION_ADDED:
        case ANCS_EVENT_ID_NOTIFICATION_MODIFIED:
            {
                printf("\n\n\rIncoming call ");
                
                // ask for the caller information
                ancs_get_notification_attribute_title(source->notificationUid);
            }
            break;    
        case ANCS_EVENT_ID_NOTIFICATION_REMOVED:
            printf("Call Removed\n");
            ancs_status.wait_action = 0;
            break;
        }
        break;

    case ANCS_CATEGORY_ID_MISSED_CALL:
        ancs_status.missed_call_count = source->categoryCount;
        ancs_update_info();
        break;
    
    case ANCS_CATEGORY_ID_VOICEMAIL:
        ancs_status.voice_mail_count = source->categoryCount;
        ancs_update_info();
        break;
    
    case ANCS_CATEGORY_ID_EMAIL:
        ancs_status.email_count = source->categoryCount;
        ancs_update_info();
        break;

    default:
        ancs_get_notification_attribute_title_msg(source->notificationUid);
        break;
    }
}

static void printsn(const uint8_t *data, int len)
{
    static char s[ANCS_NAME_MAX_LENGTH_LSB + 1];
    memcpy(s, data, len);
    s[len] = '\0';
    printf(s);
}

typedef struct
{
    const uint8_t *msg;
    int len;
} attr_value;

void ancs_handle_data(const uint8_t *data, int len)
{   
    uint8_t attr_id;
    uint16_t attr_len;
    attr_value values[2] = {0};
    
    if ((len < 5)
        || (data[0] != ANCS_COMMAND_ID_GET_NOTIFICATION_ATTRIBUTES)
        || (memcmp(data + 1, ancs_status.notification.notificationUid, sizeof(ancs_status.notification.notificationUid)) != 0))
        return;
    len -= 5;
    data += 5;

    while (len >= 3)
    {
        attr_id = data[0];
        attr_len = data[1] + (data[2] << 8);
        if (len < attr_len + 3) return;
        data += 3;
        
        switch (attr_id)
        {
        case ANCS_NOTIFICATION_ATTRIBUTE_ID_TITLE:
            values[0].msg = data;
            values[0].len = attr_len;
            break;
        case ANCS_NOTIFICATION_ATTRIBUTE_ID_MESSAGE:
            values[1].msg = data;
            values[1].len = attr_len;
            break;        
        default:
            printf("Data of Attribute ID: %d\n", attr_id);
            break;
        }
        
        data += attr_len;
        len -= attr_len + 3;
    }
    
    switch (ancs_status.notification.categoryId)    
    {
    case ANCS_CATEGORY_ID_INCOMING_CALL:
        printf("from "); printsn(values[0].msg, values[0].len);
        if((ancs_status.notification.eventFlags & (ANCS_EVENT_FLAG_NEGATIVE_ACTION | ANCS_EVENT_FLAG_POSITIVE_ACTION)) ==
            (ANCS_EVENT_FLAG_NEGATIVE_ACTION | ANCS_EVENT_FLAG_POSITIVE_ACTION))
        {
            printf(". Accept or Decline?");
            memcpy(ancs_status.action_notif_uid, ancs_status.notification.notificationUid, sizeof(ancs_status.notification.notificationUid));
            ancs_status.wait_action = 1;
        }
        break;
    case ANCS_CATEGORY_ID_SOCIAL:
        printf("SOCIAL from ");
    default:
        if (values[0].len > 0)
        {
            printsn(values[0].msg, values[0].len);
            printf(": ");
        }
        if (values[1].len > 0)
            printsn(values[1].msg, values[1].len);
        printf("\n");
    }
}

typedef struct
{
    gatt_client_characteristic_t *c;
    const uint8_t *uuid;
} char_disc_info_t;

typedef struct
{
    gatt_client_characteristic_t *c;
    gatt_client_notification_t   *n;
} char_notif_info_t;

char_disc_info_t char_disc_infos[] =
{
    {.c = &ancs_info.notif_source_char, .uuid = UUID_ANCS_NOTIF_SOURCE_CHAR},
    {.c = &ancs_info.ctrl_point_char,   .uuid = UUID_ANCS_CTRL_POINT_CHAR},
    {.c = &ancs_info.data_source_char,  .uuid = UUID_ANCS_DATA_SOURCE_CHAR}
};

char_notif_info_t char_notif_infos[] =
{
    {.c = &ancs_info.notif_source_char, .n = &ancs_info.notif_source_notify},
    {.c = &ancs_info.data_source_char,  .n = &ancs_info.data_source_notify}
};

int disc_id = -1;

static void notification_handler(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_NOTIFICATION:
    case GATT_EVENT_INDICATION:
        {
            uint16_t value_size;
            const gatt_event_value_packet_t *value_packet = gatt_event_notification_parse(packet, size, &value_size);
            const uint8_t *value = value_packet->value;
            if (value_packet->handle == ancs_info.notif_source_char.value_handle)
            {
                ancs_handle_notifications((const ancs_notif_source_packet *)value);
            }
            else if (value_packet->handle == ancs_info.data_source_char.value_handle)
            {
                ancs_handle_data(value, value_size);
            }
            else
                printf("ANCS Unknown Handle: %d\n", value_packet->handle);
        }
        break;
    }
}

void config_notification(void);

void descriptor_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_QUERY_COMPLETE:
        if (gatt_event_query_complete_parse(packet)->status != 0)
            break;
        
        disc_id++;
        if (disc_id < sizeof(char_notif_infos) / sizeof(char_notif_infos[0]))
        {
            config_notification();
        }
        else
            printf("ANCS Initialized\n");
        break;
    }
}

void config_notification(void)
{
    gatt_client_listen_for_characteristic_value_updates(char_notif_infos[disc_id].n,
                        notification_handler, ancs_info.conn_handle, char_notif_infos[disc_id].c->value_handle);
    gatt_client_write_client_characteristic_configuration(descriptor_discovery_callback, ancs_info.conn_handle,
                char_notif_infos[disc_id].c, GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION);
}

void characteristic_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_CHARACTERISTIC_QUERY_RESULT:
        *char_disc_infos[disc_id].c = gatt_event_characteristic_query_result_parse(packet)->characteristic;
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        if ((gatt_event_query_complete_parse(packet)->status != 0)
            || (char_disc_infos[disc_id].c->start_handle == 0xffff))
        {
            gap_disconnect(ancs_info.conn_handle);
            break;
        }

        disc_id++;
        if (disc_id < sizeof(char_disc_infos) / sizeof(char_disc_infos[0]))
            gatt_client_discover_characteristics_for_handle_range_by_uuid128(characteristic_discovery_callback, ancs_info.conn_handle, 
                ancs_info.service.start_group_handle, ancs_info.service.end_group_handle, char_disc_infos[disc_id].uuid);
        else
        {
            disc_id = 0;
            config_notification();
        }
        break;
    }
}

void service_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_SERVICE_QUERY_RESULT:
        ancs_info.service = gatt_event_service_query_result_parse(packet)->service;        
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        if ((gatt_event_query_complete_parse(packet)->status != 0)
            || (ancs_info.service.start_group_handle == 0xffff))
        {
            printf("service not found, disc\n");
            gap_disconnect(ancs_info.conn_handle);
            break;
        }
        disc_id = 0;
        gatt_client_discover_characteristics_for_handle_range_by_uuid128(characteristic_discovery_callback, ancs_info.conn_handle, 
            ancs_info.service.start_group_handle, ancs_info.service.end_group_handle, char_disc_infos[0].uuid);
        break;
    }
}

void ancs_discover(uint16_t conn_handle)
{
    ancs_reset();
    ancs_info.conn_handle = conn_handle;
    gatt_client_discover_primary_services_by_uuid128(service_discovery_callback, conn_handle, UUID_ANCS_SERVICE);
}

void ancs_reset(void)
{
    memset(&ancs_info, -1, sizeof(ancs_info));
    memset(&ancs_status, 0, sizeof(ancs_status));
}
