#include "mesh_storage_low_level.h"
#include "platform_api.h"
#include "att_db.h"
#include "FreeRTOS.h"
#include "task.h"
#include "kv_storage.h"
#include "eflash.h"
#include "app_debug.h"


// Application flash related.
#define MESH_STORAGE_KEY_START      KV_MESH_KEY_START 
#define MESH_STORAGE_KEY_END        KV_MESH_KEY_END

#define MESH_STORAGE_APP_KEY        MESH_STORAGE_KEY_START

#define MESH_FLAG_BIT_NAME          ((uint32_t)( 1 << 0 ))
#define MESH_FLAG_BIT_GATT_ADDR     ((uint32_t)( 1 << 1 ))
#define MESH_FLAG_BIT_BEACON_ADDR   ((uint32_t)( 1 << 2 ))
#define MESH_FLAG_BIT_UUID          ((uint32_t)( 1 << 3 ))

// Mesh application storage struct.
typedef struct mesh_app_storage_db {
    // control flag.
    uint32_t flag;
    
    // name
    uint8_t  name[30];
    uint16_t name_len;
    // connect addr
    bd_addr_t gatt_addr;
    // non-connect addr
    bd_addr_t beacon_addr;
    // mesh uuid
    uint8_t  device_uuid[16];
    uint16_t uuid_len;

} mesh_app_storage_db_t;

static uint32_t app_flash_start_address = 0;

/* ------------------------------- flag --------------------------------*/

// flag get.
int mesh_storage_flag_get(uint32_t *flag){
    mesh_app_storage_db_t *store = (mesh_app_storage_db_t *)kv_get(MESH_STORAGE_APP_KEY, NULL);
    if (NULL == store)
        return -1;
    
    *flag = store->flag;
    return 0;
}

// single flag get.
int mesh_storage_single_flag_check(uint32_t bit){
    mesh_app_storage_db_t *store = (mesh_app_storage_db_t *)kv_get(MESH_STORAGE_APP_KEY, NULL);
    if (NULL == store)
        return -1;
    
    if(store->flag & bit){
        return 1;
    }
    return 0;
}

/* ------------------------------- name --------------------------------*/

// name get.
void mesh_storage_name_get(uint8_t *name, uint16_t *len){
    mesh_app_storage_db_t *store = (mesh_app_storage_db_t *)kv_get(MESH_STORAGE_APP_KEY, NULL);
    if (NULL == store)
        return;
    
    if(store->flag & MESH_FLAG_BIT_NAME){
        memcpy(name, store->name, store->name_len);
        *len = store->name_len;
    } else {
        *len = 0;
    }
}

// name already exist check.
int mesh_storage_is_name_set(void){
    if(1 == mesh_storage_single_flag_check(MESH_FLAG_BIT_NAME)){
        return 1;
    } else {
        return 0;
    }
}

// name set.
void mesh_storage_name_set(uint8_t *name, uint16_t len){
    mesh_app_storage_db_t *store = (mesh_app_storage_db_t *)kv_get(MESH_STORAGE_APP_KEY, NULL);
    if (NULL == store)
        return;

    memcpy(store->name, name, len);
    store->name_len = len;
    store->flag |= MESH_FLAG_BIT_NAME;
    kv_value_modified();
}

/* ------------------------------- gatt addr --------------------------------*/

// gatt addr get.
void mesh_storage_gatt_addr_get(bd_addr_t addr){
    mesh_app_storage_db_t *store = (mesh_app_storage_db_t *)kv_get(MESH_STORAGE_APP_KEY, NULL);
    if (NULL == store)
        return;
    
    if(store->flag & MESH_FLAG_BIT_GATT_ADDR){
        memcpy(addr, store->gatt_addr, sizeof(bd_addr_t));
    } else {
        memset(addr, 0, sizeof(bd_addr_t));
    }
}

// gatt address already exist check.
int mesh_storage_is_gatt_addr_set(void){
    if(1 == mesh_storage_single_flag_check(MESH_FLAG_BIT_GATT_ADDR)){
        return 1;
    } else {
        return 0;
    }
}

// gatt addr set.
void mesh_storage_gatt_addr_set(bd_addr_t addr){
    mesh_app_storage_db_t *store = (mesh_app_storage_db_t *)kv_get(MESH_STORAGE_APP_KEY, NULL);
    if (NULL == store)
        return;

    memcpy(store->gatt_addr, addr, sizeof(bd_addr_t));
    store->flag |= MESH_FLAG_BIT_GATT_ADDR;
    kv_value_modified();
}

/* ------------------------------- beacon addr --------------------------------*/

// beacon addr get.
void mesh_storage_beacon_addr_get(bd_addr_t addr){
    mesh_app_storage_db_t *store = (mesh_app_storage_db_t *)kv_get(MESH_STORAGE_APP_KEY, NULL);
    if (NULL == store)
        return;
    
    if(store->flag & MESH_FLAG_BIT_BEACON_ADDR){
        memcpy(addr, store->beacon_addr, sizeof(bd_addr_t));
    } else {
        memset(addr, 0, sizeof(bd_addr_t));
    }
}

// beacon address already exist check.
int mesh_storage_is_beacon_addr_set(void){
    if(1 == mesh_storage_single_flag_check(MESH_FLAG_BIT_BEACON_ADDR)){
        return 1;
    } else {
        return 0;
    }
}

// beacon addr set.
void mesh_storage_beacon_addr_set(bd_addr_t addr){
    mesh_app_storage_db_t *store = (mesh_app_storage_db_t *)kv_get(MESH_STORAGE_APP_KEY, NULL);
    if (NULL == store)
        return;

    memcpy(store->beacon_addr, addr, sizeof(bd_addr_t));
    store->flag |= MESH_FLAG_BIT_BEACON_ADDR;
    kv_value_modified();
}

/* ------------------------------- mesh uuid --------------------------------*/

// mesh device uuid get.
void mesh_storage_device_uuid_get(uint8_t *uuid, uint16_t *len){
    mesh_app_storage_db_t *store = (mesh_app_storage_db_t *)kv_get(MESH_STORAGE_APP_KEY, NULL);
    if (NULL == store)
        return;
    
    if(store->flag & MESH_FLAG_BIT_UUID){
        memcpy(uuid, store->device_uuid, sizeof(store->device_uuid));
        *len = sizeof(store->device_uuid);
    } else {
        *len = 0;
    }
}

// mesh uuid already exist check.
int mesh_storage_is_device_uuid_set(void){
    if(1 == mesh_storage_single_flag_check(MESH_FLAG_BIT_UUID)){
        return 1;
    } else {
        return 0;
    }
}

// mesh device uuid set.
void mesh_storage_device_uuid_set(uint8_t *uuid, uint16_t len){
    mesh_app_storage_db_t *store = (mesh_app_storage_db_t *)kv_get(MESH_STORAGE_APP_KEY, NULL);
    if (NULL == store)
        return;

    if (len != sizeof(store->device_uuid)){
        app_log_error("error: uuid len err=%d(right Len =%d)\n", len, sizeof(store->device_uuid));
        return;
    }
    
    memcpy(store->device_uuid, uuid, sizeof(store->device_uuid));
    store->uuid_len = len;
    store->flag |= MESH_FLAG_BIT_UUID;

    kv_value_modified();
}

// flash operation --------------------------------------------------------------

// erase flash
static void my_kv_erase_flash(void){
    app_assert(app_flash_start_address != 0);
    erase_flash_page(app_flash_start_address);
}

// write to flash.
static int my_kv_write_to_flash(const void *db, const int size){
    app_log_debug("my_kv_write_to_flash start, size = %d\n", size);
    app_assert(app_flash_start_address != 0);
    int ret = program_flash(app_flash_start_address, (uint8_t *)db, size);//Auto erase flash before each writing flash operation.
    app_log_debug("my_kv_write_to_flash end, size = %d\n", size);
    return ret;
}

// read from flash.
static int my_kv_read_from_flash(void *db, const int max_size){
    // read data from flash: just need copy it!
    app_assert(app_flash_start_address != 0);
    memcpy((uint8_t *)db, ((uint8_t *) app_flash_start_address), max_size);
    app_log_debug("===Read max size = %d\n", max_size);
    return 0;
}

// init.
int mesh_storage_low_level_init(uint32_t start_address){
    uint32_t flag = 0;
    
    app_flash_start_address = start_address;
    
    kv_init(&my_kv_write_to_flash, &my_kv_read_from_flash);
    // if not init, init it. 
    if(mesh_storage_flag_get(&flag) < 0){
        app_log_info("kv storage init\n");
        kv_put(MESH_STORAGE_APP_KEY, NULL, sizeof(mesh_app_storage_db_t));
        mesh_app_storage_db_t *store = (mesh_app_storage_db_t *)kv_get(MESH_STORAGE_APP_KEY, NULL);
        memset(store, 0, sizeof(mesh_app_storage_db_t));
        store->flag = 0;
        return 1;
    }
    // if already init, just print the flag.
    else {
        app_log_debug("===flag = %02X\n", flag);
    }
    return 0;
}


void mesh_storage_low_level_reinit(void){
    // clear all information in flash.
    my_kv_erase_flash();
    app_flash_start_address = 0;
}


