#ifndef _MESH_STORAGE_H_
#define _MESH_STORAGE_H_

#include <stdint.h>
#include "bt_types.h"
#include "gap.h"
#include "btstack_event.h"
#include "eflash.h" //ingchips eflash controller

#define HAL_FLASH_BANK_SIZE     EFLASH_PAGE_SIZE  //For ingchips eflash, the true unit of bank is page, and one page = 8KB(0x2000). 
#define HAL_FLASH_BANK_0_ADDR   ((uint32_t)0x00070000)
#define HAL_FLASH_BANK_1_ADDR   ((uint32_t)(HAL_FLASH_BANK_0_ADDR+0x2000))
#define HAL_FLASH_BANK_2_ADDR   ((uint32_t)(HAL_FLASH_BANK_1_ADDR+0x2000))  //we use this bank for app storage.


// name
void mesh_storage_name_get(uint8_t *name, uint16_t *len);
int mesh_storage_is_name_set(void);
void mesh_storage_name_set(uint8_t *name, uint16_t len);

// gatt addr
void mesh_storage_gatt_addr_get(bd_addr_t addr);
int mesh_storage_is_gatt_addr_set(void);
void mesh_storage_gatt_addr_set(bd_addr_t addr);

// beacon addr
void mesh_storage_beacon_addr_get(bd_addr_t addr);
int mesh_storage_is_beacon_addr_set(void);
void mesh_storage_beacon_addr_set(bd_addr_t addr);

// uuid
void mesh_storage_device_uuid_get(uint8_t *uuid, uint16_t *len);
int mesh_storage_is_device_uuid_set(void);
void mesh_storage_device_uuid_set(uint8_t *uuid, uint16_t len);

//init
int mesh_storage_init(void);



#endif
