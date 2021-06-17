#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "bluetooth.h"
#include "btstack_util.h"

static uint8_t * att_db;
static uint16_t  att_db_size;
static uint16_t  att_db_max_size;
static uint16_t  att_db_next_handle;

static void att_db_util_set_end_tag(void){
    // end tag
    att_db[att_db_size] = 0;
    att_db[att_db_size+1] = 0;
}

void att_db_util_init(uint8_t *att_db_storage, const int16_t db_max_size){
    att_db = att_db_storage;
    att_db_max_size = db_max_size;

    att_db_size = 0;
    att_db_next_handle = 1;
    att_db_util_set_end_tag();
}

/**
 * asserts that the requested amount of bytes can be stored in the att_db
 * @returns TRUE if space is available
 */
static int att_db_util_assert_space(uint16_t size){

    size += 2; // for end tag
    if (att_db_size + size <= att_db_max_size) return 1;

    // att_db: out of memory
    while (1);
}

// attribute size in bytes (16), flags(16), handle (16), uuid (16/128), value(...)

// db endds with 0x00 0x00

uint16_t att_db_util_add_attribute_uuid16(uint16_t uuid16, uint16_t flags, const uint8_t * data, uint16_t data_len){
    int size = 2 + 2 + 2 + 2 + data_len;
    if (!att_db_util_assert_space(size)) return 0xffff;
    little_endian_store_16(att_db, att_db_size, size);
    att_db_size += 2;
    little_endian_store_16(att_db, att_db_size, flags);
    att_db_size += 2;
    little_endian_store_16(att_db, att_db_size, att_db_next_handle);
    att_db_size += 2;
    att_db_next_handle++;
    little_endian_store_16(att_db, att_db_size, uuid16);
    att_db_size += 2;
    memcpy(&att_db[att_db_size], data, data_len);
    att_db_size += data_len;
    att_db_util_set_end_tag();
    return att_db_next_handle - 1;
}

uint16_t att_db_util_add_descriptor_uuid16(uint16_t uuid16, uint16_t flags, const uint8_t * data, uint16_t data_len) {
    return att_db_util_add_attribute_uuid16(uuid16, flags, data, data_len);
}

static void att_db_util_add_attribute_uuid128(const uint8_t * uuid128, uint16_t flags, const uint8_t * data, uint16_t data_len){
    int size = 2 + 2 + 2 + 16 + data_len;
    if (!att_db_util_assert_space(size)) return;
    flags |= ATT_PROPERTY_UUID128;
    little_endian_store_16(att_db, att_db_size, size);
    att_db_size += 2;
    little_endian_store_16(att_db, att_db_size, flags);
    att_db_size += 2;
    little_endian_store_16(att_db, att_db_size, att_db_next_handle);
    att_db_size += 2;
    att_db_next_handle++;
    reverse_128(uuid128, &att_db[att_db_size]);
    att_db_size += 16;
    memcpy(&att_db[att_db_size], data, data_len);
    att_db_size += data_len;
    att_db_util_set_end_tag();
}

void att_db_util_add_service_uuid16(uint16_t uuid16){
    uint8_t buffer[2];
    little_endian_store_16(buffer, 0, uuid16);
    att_db_util_add_attribute_uuid16(GATT_PRIMARY_SERVICE_UUID, ATT_PROPERTY_READ, buffer, 2);
}

void att_db_util_add_service_uuid128(const uint8_t * uuid128){
    uint8_t buffer[16];
    reverse_128(uuid128, buffer);
    att_db_util_add_attribute_uuid16(GATT_PRIMARY_SERVICE_UUID, ATT_PROPERTY_READ, buffer, 16);
}

uint16_t att_db_util_add_characteristic_uuid16(uint16_t uuid16, uint16_t properties, const uint8_t * data, uint16_t data_len){
    uint8_t buffer[5];
    uint16_t value_handle = 0;
    uint16_t flags = 0;
    buffer[0] = properties;
    little_endian_store_16(buffer, 1, att_db_next_handle + 1);
    little_endian_store_16(buffer, 3, uuid16);
    att_db_util_add_attribute_uuid16(GATT_CHARACTERISTICS_UUID, ATT_PROPERTY_READ, buffer, sizeof(buffer));
    value_handle = att_db_next_handle;
    att_db_util_add_attribute_uuid16(uuid16, properties, data, data_len);
    if (properties & (ATT_PROPERTY_NOTIFY | ATT_PROPERTY_INDICATE)){
        flags = ATT_PROPERTY_READ | ATT_PROPERTY_WRITE | ATT_PROPERTY_DYNAMIC;
        if (properties & ATT_PROPERTY_AUTHENTICATION_REQUIRED) {
            flags = flags | ATT_PROPERTY_AUTHENTICATION_REQUIRED;
        }
        little_endian_store_16(buffer, 0, 0);
        att_db_util_add_attribute_uuid16(GATT_CLIENT_CHARACTERISTICS_CONFIGURATION, flags, buffer, 2);
    }
    return value_handle;
}

uint16_t att_db_util_add_characteristic_uuid128(const uint8_t * uuid128, uint16_t properties, const uint8_t * data, uint16_t data_len){
    uint8_t buffer[19];
  uint16_t value_handle = 0;
    uint16_t flags = 0;
    buffer[0] = properties;
    little_endian_store_16(buffer, 1, att_db_next_handle + 1);
    reverse_128(uuid128, &buffer[3]);
    att_db_util_add_attribute_uuid16(GATT_CHARACTERISTICS_UUID, ATT_PROPERTY_READ, buffer, sizeof(buffer));
    value_handle = att_db_next_handle;
    att_db_util_add_attribute_uuid128(uuid128, properties, data, data_len);
    if (properties & (ATT_PROPERTY_NOTIFY | ATT_PROPERTY_INDICATE)){
        flags = ATT_PROPERTY_READ | ATT_PROPERTY_WRITE | ATT_PROPERTY_DYNAMIC;
        if (properties & ATT_PROPERTY_AUTHENTICATION_REQUIRED) {
            flags = flags | ATT_PROPERTY_AUTHENTICATION_REQUIRED;
        }
        little_endian_store_16(buffer, 0, 0);
        att_db_util_add_attribute_uuid16(GATT_CLIENT_CHARACTERISTICS_CONFIGURATION, flags, buffer, 2);
    }
    return value_handle;
}

uint8_t * att_db_util_get_address(void){
    return att_db;
}

uint16_t att_db_util_get_size(void){
    return att_db_size + 2;    // end tag
}

static void att_db_util_print_item(const uint8_t *p, const int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        if ((i > 0) && ((i & 0xf) == 0))
            printf("\n");
        printf("0x%02x, ", p[i]);
    }
}

void att_db_util_print(void)
{
    uint8_t * p = att_db;
    uint16_t i = 0;
    while (i < att_db_size)
    {
        uint16_t len = *(uint16_t *)(p);
        if (len == 0)
            break;
        printf("\n// len = %d, flags = 0x%04x, handle = %d\n", len, *(uint16_t *)(p + 2), *(uint16_t *)(p + 4));
        att_db_util_print_item(p, len);
        printf("\n");
        i += len;
        p += len;
    }
    printf("0x00, 0x00 // total size = %d\n", att_db_size + 2);
}

