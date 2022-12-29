#ifndef _MESH_STORAGE_H_
#define _MESH_STORAGE_H_

#include <stdint.h>
#include "bt_types.h"
#include "gap.h"
#include "btstack_event.h"
#include "eflash.h"

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
int mesh_storage_init(void);
void mesh_storage_clear_and_reinit(void);

// mesh stack
void mesh_stack_storage_init(void);

#endif
