#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "bluetooth.h"
#include "btstack_util.h"

#include "aes_cmac.c"

#define ATT_DB_PRINT_EN     0
#define ATT_RC_PRINT_EN     0

static uint8_t * att_db;
static uint16_t  att_db_size;
static uint16_t  att_db_max_size;
static uint16_t  att_db_next_handle;

static uint8_t * att_rc = NULL;
static uint16_t  att_rc_size = 0;
static uint16_t  att_rc_max_size = 0;
static uint16_t  att_rc_util_add_service_uuid16(uint16_t att_handle, uint16_t att_type, const uint8_t * data, uint16_t data_len);

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
#if ATT_DB_PRINT_EN
    printf("att_db: out of memory !!!\n");
#endif
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
    if(att_rc != NULL){
        att_rc_util_add_service_uuid16(att_db_next_handle - 1, uuid16, data, data_len);
    }
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

void att_db_util_add_primary_service_uuid16(uint16_t uuid16){
    att_db_util_add_service_uuid16(uuid16);
}

void att_db_util_add_secondary_service_uuid16(uint16_t uuid16){
    uint8_t buffer[2];
    little_endian_store_16(buffer, 0, uuid16);
    att_db_util_add_attribute_uuid16(GATT_SECONDARY_SERVICE_UUID, ATT_PROPERTY_READ, buffer, sizeof(buffer));
}

void att_db_util_add_include_service_uuid16(uint16_t included_service_att_handle, uint16_t end_group_handle, uint16_t uuid16){
    uint8_t buffer[6];
    little_endian_store_16(buffer, 0, included_service_att_handle);
    little_endian_store_16(buffer, 2, end_group_handle);
    little_endian_store_16(buffer, 4, uuid16);
    att_db_util_add_attribute_uuid16(GATT_INCLUDE_SERVICE_UUID, ATT_PROPERTY_READ, buffer, sizeof(buffer));
}

void att_db_util_set_next_handle(uint16_t handle){
    att_db_next_handle = handle;
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

/////////////////////////////////////////////////////


void att_rc_util_init(uint8_t *att_rc_storage, const uint16_t rc_max_size)
{
    att_rc = att_rc_storage;
    att_rc_max_size = rc_max_size;
    att_rc_size = 0;
}

static int att_rc_util_assert_space(uint16_t size){

    if (att_rc_size + size <= att_rc_max_size) return 1;

    // att_rc: out of memory
#if ATT_RC_PRINT_EN
    printf("att_rc: out of memory !!!\n");
#endif
    while (1);
}

static int att_rc_util_assert_att_type(uint16_t att_type){
    switch(att_type){
        case GATT_PRIMARY_SERVICE_UUID:
        case GATT_SECONDARY_SERVICE_UUID:
        case GATT_INCLUDE_SERVICE_UUID:
        case GATT_CHARACTERISTICS_UUID:
        case GATT_CHARACTERISTIC_EXTENDED_PROPERTIES:
            return 1; // att_handle + att_type + att_data
        case GATT_CHARACTERISTIC_USER_DESCRIPTION:
        case GATT_CLIENT_CHARACTERISTICS_CONFIGURATION:
        case GATT_SERVER_CHARACTERISTICS_CONFIGURATION:
        case GATT_CHARACTERISTIC_PRESENTATION_FORMAT:
        case GATT_CHARACTERISTIC_AGGREGATE_FORMAT:
            return 2; // att_handle + att_type
        default:
            break;
    }
    return 0;
}

static uint16_t att_rc_util_add_service_uuid16(uint16_t att_handle, uint16_t att_type, const uint8_t * data, uint16_t data_len){
    int size;
    if (!att_rc_util_assert_att_type(att_type)) return 0xffff;
    if(att_rc_util_assert_att_type(att_type) == 2){
        data_len = 0;
    }
    size = 2 + 2 + data_len;
    if (!att_rc_util_assert_space(size)) return 0xffff;
    little_endian_store_16(att_rc, att_rc_size, att_handle);
    att_rc_size += 2;
    little_endian_store_16(att_rc, att_rc_size, att_type);
    att_rc_size += 2;
    if(data_len){
        memcpy(&att_rc[att_rc_size], data, data_len);
        att_rc_size += data_len;
    }
#if ATT_RC_PRINT_EN
    printf("Handle %04X Data ", att_handle);
    uint16_t i=0;
    for(i=0; i<size; i++){
        if(i!=0 && i%2==0){
            printf("-");
        }
        printf("%02X", att_rc[att_rc_size-size+i]);
    }
    printf("\n");
#endif
    return 0;
}

uint8_t * att_rc_util_get_address(void){
    return att_rc;
}

uint16_t att_rc_util_get_size(void){
    return att_rc_size;
}

int att_rc_util_calc_database_hash(uint8_t *db_hash)
{
    // calc aes-cmac    
    unsigned char all_Zero[16] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    unsigned char database_hash_cmac[16];
    
    if(db_hash == NULL) return -1; // input param error.
    if(att_rc == NULL)  return -2; // uninit.
    
    AES_CMAC(all_Zero, att_rc, att_rc_size, database_hash_cmac);
    reverse_bytes(database_hash_cmac, db_hash, 16);
#if ATT_RC_PRINT_EN
    printf("AES_CMAC       "); print128(database_hash_cmac); printf("\n");    
    printf("DB_HASH        "); print128(db_hash); printf("\n");
#endif
    return 0;
}


