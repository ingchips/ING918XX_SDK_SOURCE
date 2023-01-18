#ifndef _MESH_STORAGE_LOW_LEVEL_H_
#define _MESH_STORAGE_LOW_LEVEL_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "bluetooth.h"


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

// init
int mesh_storage_low_level_init(uint32_t start_address);
void mesh_storage_low_level_reinit(void);


#endif
