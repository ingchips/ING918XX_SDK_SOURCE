#include "mesh_includes.h"
//#include "btstack_debug.h"
#include "att_db.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "string.h"
#include "kv_storage.h"
#include "mesh_storage.h"

#define MESH_STORAGE_KEY_START      KV_MESH_KEY_START 
#define MESH_STORAGE_KEY_END        KV_MESH_KEY_END

#define MESH_STORAGE_APP_KEY        MESH_STORAGE_KEY_START

#define MESH_FLAG_BIT_NAME          ((uint32_t)( 1 << 0 ))
#define MESH_FLAG_BIT_GATT_ADDR     ((uint32_t)( 1 << 1 ))
#define MESH_FLAG_BIT_BEACON_ADDR   ((uint32_t)( 1 << 2 ))
#define MESH_FLAG_BIT_UUID          ((uint32_t)( 1 << 3 ))

// Mesh application storage struct.
typedef struct mesh_app_storage_db {

    uint32_t flag;

    // Identification
    uint8_t name[30];
    uint16_t name_len;

    bd_addr_t gatt_addr;
    bd_addr_t beacon_addr;
    uint8_t device_uuid[16];
    uint16_t uuid_len;

} mesh_app_storage_db_t;

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
        printf("error: uuid len err=%d(rightLen=%d)\n", len, sizeof(store->device_uuid));
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
    erase_flash_page(HAL_FLASH_BANK_2_ADDR);
}

// write to flash.
static int my_kv_write_to_flash(const void *db, const int size){
    printf("===Write size s = %d\n", size);
    int ret = program_flash(HAL_FLASH_BANK_2_ADDR, (uint8_t *)db, size);//Auto erase flash before each writing flash operation.
    printf("===Write size e = %d\n", size);
    return ret;
}

// read from flash.
static int my_kv_read_from_flash(void *db, const int max_size){
    // read data from flash: just need copy it!
    memcpy((uint8_t *)db, ((uint8_t *) HAL_FLASH_BANK_2_ADDR), max_size);
    printf("===Read max size = %d\n", max_size);
    return 0;
}

// static uint8_t wrData[] = "\x11\x22\x33\x44\x55\x66\x77\x88\x55\x22\x33\x44\x55\x66\x77\x99";
// static uint8_t tmpData[20];
// static int tmpLen;
// int flash_test(void){


//     // read 16
//     memcpy((uint8_t *)tmpData, ((uint8_t *) HAL_FLASH_BANK_0_ADDR), 16);
//     printf("read[0x%x]: ", HAL_FLASH_BANK_0_ADDR);
//     printf_hexdump(tmpData, 16);

//     // read 8
//     memcpy((uint8_t *)tmpData, ((uint8_t *) HAL_FLASH_BANK_0_ADDR)+8, 8);
//     printf("read[0x%x]: ", HAL_FLASH_BANK_0_ADDR+8);
//     printf_hexdump(tmpData, 8);

//     // erase
//     erase_flash_page(HAL_FLASH_BANK_0_ADDR);

//     // read 16
//     memcpy((uint8_t *)tmpData, ((uint8_t *) HAL_FLASH_BANK_0_ADDR), 16);
//     printf("read[0x%x]: ", HAL_FLASH_BANK_0_ADDR);
//     printf_hexdump(tmpData, 16);

//     // read 8
//     memcpy((uint8_t *)tmpData, ((uint8_t *) HAL_FLASH_BANK_0_ADDR)+8, 8);
//     printf("read[0x%x]: ", HAL_FLASH_BANK_0_ADDR+8);
//     printf_hexdump(tmpData, 8);

//     // write
//     int ret = write_flash(HAL_FLASH_BANK_0_ADDR+8, (uint8_t *)wrData, 8);
//     printf("===Write 8\n");

//     // read 16
//     memcpy((uint8_t *)tmpData, ((uint8_t *) HAL_FLASH_BANK_0_ADDR), 16);
//     printf("read[0x%x]: ", HAL_FLASH_BANK_0_ADDR);
//     printf_hexdump(tmpData, 16);

//     // read 8
//     memcpy((uint8_t *)tmpData, ((uint8_t *) HAL_FLASH_BANK_0_ADDR)+8, 8);
//     printf("read[0x%x]: ", HAL_FLASH_BANK_0_ADDR+8);
//     printf_hexdump(tmpData, 8);



//     return 0;
// }

// init.
int mesh_storage_init(void){
    // return flash_test();
    uint32_t flag = 0;

    kv_init(&my_kv_write_to_flash, &my_kv_read_from_flash);
    // if not init, init it. or print flag.
    if(mesh_storage_flag_get(&flag) < 0){
        printf("===init\n");
        kv_put(MESH_STORAGE_APP_KEY, NULL, sizeof(mesh_app_storage_db_t));
        mesh_app_storage_db_t *store = (mesh_app_storage_db_t *)kv_get(MESH_STORAGE_APP_KEY, NULL);
        memset(store, 0, sizeof(mesh_app_storage_db_t));
        store->flag = 0;
        return 1;
    } else {
        printf("===flag = %02X\n", flag);
    }
    return 0;
}

void mesh_storage_clear_and_reinit(void){
    // clear all information in flash.
    my_kv_erase_flash(); 
    mesh_storage_init();
}

// ble params reset delay timer.
#include "btstack_run_loop.h"
//#include "msg_timer_def.h"
#include "mesh_btstack_defines.h"
//#include "mesh_timer.h"
static btstack_timer_source_t       mesh_ble_params_reset_delay_timer;

extern void ble_port_generate_name_and_load_name(void);
extern void ble_port_generate_uuid_and_load_uuid(void);
extern void ble_port_generate_addr_and_load_addr(void);
static void mesh_ble_params_reload_init(void){
    //name reload.
    ble_port_generate_name_and_load_name();
    //addr reload.
    ble_port_generate_addr_and_load_addr();
    //uuid reload.
    ble_port_generate_uuid_and_load_uuid();
}

extern void mesh_node_reset(void);
static void mesh_ble_params_reset_delay_timer_timeout_handler(btstack_timer_source_t * ts){
    UNUSED(ts);
    printf("[V] timeout , ble params reset now !!!!\n");
    mesh_node_reset();
    mesh_storage_clear_and_reinit();
    mesh_ble_params_reload_init();
}

void mesh_ble_params_reset_delay_timer_start(uint32_t timeout_in_ms){
    // set timer
    mesh_btstack_run_loop_set_timer_handler(&mesh_ble_params_reset_delay_timer, (f_timer_process)mesh_ble_params_reset_delay_timer_timeout_handler);
    mesh_btstack_run_loop_set_timer(&mesh_ble_params_reset_delay_timer, MESH_BLE_PARAMS_RESET_DELAY_TIMER_ID, timeout_in_ms);
    mesh_btstack_run_loop_add_timer(&mesh_ble_params_reset_delay_timer);
    printf("[V] mesh ble params reset delay timer start: %d ms\n", timeout_in_ms);
}



// test ================================================================================================
#if 0
static char my_name[] = "this_is_my_name";
static bd_addr_t gatt_addr = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
static bd_addr_t beacon_addr = {0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc};
static char my_uuid[] = "123456789abcdefg";

static uint8_t tmpBuf[100];
static uint16_t tmpLen=0;
static bd_addr_t tmpAddr;



static void read_name(void){
    // read name.
    memset(tmpBuf, 0, sizeof(tmpBuf)); tmpLen = 0;
    mesh_storage_name_get(tmpBuf, &tmpLen);
    printf("read name[%d]: %s\n",tmpLen,  (char*)tmpBuf);
}

static void read_gatt_addr(void){
    // read gatt addr.
    memset(tmpAddr, 0, sizeof(tmpAddr));
    mesh_storage_gatt_addr_get(tmpAddr);
    printf("gatt_addr[%d]: ",sizeof(tmpAddr));
    printf_hexdump(tmpAddr, sizeof(tmpAddr));
}

static void read_beacon_addr(void){
    // read beacon addr.
    memset(tmpAddr, 0, sizeof(tmpAddr));
    mesh_storage_beacon_addr_get(tmpAddr);
    printf("beacon_addr[%d]: ",sizeof(tmpAddr));
    printf_hexdump(tmpAddr, sizeof(tmpAddr));
}

static void read_uuid(void){
    // read gatt addr.
    memset(tmpBuf, 0, sizeof(tmpBuf)); tmpLen = 0;
    mesh_storage_device_uuid_get(tmpBuf, &tmpLen);
    printf("my_uuid[%d]: ", tmpLen);
    printf_hexdump(tmpBuf, tmpLen);
}

static void read_all(void){
    read_name();
    read_gatt_addr();
    read_beacon_addr();
    read_uuid();
}

static void set_all(void){
    // set name.
    mesh_storage_name_set((uint8_t *)my_name, strlen(my_name));//不包含'\0',所以用strlen，不能用sizeof

    // set gatt addr.
    mesh_storage_gatt_addr_set(gatt_addr);

    // set beacon addr.
    mesh_storage_beacon_addr_set(beacon_addr);

    // set uuid.
    mesh_storage_device_uuid_set((uint8_t *)my_uuid, strlen(my_uuid));
}


void mesh_storage_test(void){
    printf("\n---------------------------\n");

    // erase test.
    // my_kv_erase_flash();

    // init.
    if(mesh_storage_init()){
        set_all();
    }
    
    // read all.
    read_all();

    printf("\n---------------------------\n");
}
#endif










