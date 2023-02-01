#ifndef _MESH_STORAGE_APP_H_
#define _MESH_STORAGE_APP_H_

#include "bt_types.h"
#include "gap.h"

void mesh_gatt_addr_generate_and_get(bd_addr_t addr);
void mesh_beacon_addr_generate_and_get(bd_addr_t addr);
void mesh_generate_random_uuid(uint8_t * dev_uuid, uint16_t *len);
void mesh_generate_random_name(uint8_t * name, uint16_t *len);



// mesh application storage init.
void mesh_storage_app_init(void);
void mesh_storage_app_reinit(void);

// mesh stack storage init.
void mesh_storage_stack_init(void);

#endif
