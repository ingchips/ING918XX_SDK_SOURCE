#ifndef _MESH_PORT_LOW_LEVEL_INIT_H_
#define _MESH_PORT_LOW_LEVEL_INIT_H_

#include <stdint.h>
#include "bt_types.h"
#include "profile.h"

/**
* @enum platform config
*/
typedef enum {
    MESH_CFG_NAME,
    MESH_CFG_GATT_ADV_ADDR,
    MESH_CFG_BEACON_ADV_ADDR,
} bt_mesh_config_t;


void mesh_elems_and_models_ll_init(const bt_mesh_comp_t *a_comp);
void mesh_prov_ll_init(const bt_mesh_prov_t *prov);
void mesh_platform_config(bt_mesh_config_t type, void *data, uint32_t data_len);
void mesh_platform_adv_params_init(void);
void mesh_set_addr_static(uint8_t * addr);

#endif

