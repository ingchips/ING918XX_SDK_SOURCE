#ifndef _MESH_API_H
#define _MESH_API_H

#include <stdint.h>

#include "glue.h"
#include "access.h"
#include "nimble_npl.h"
#include "nimble_npl_os.h"
#include "timers.h"
#include "btstack_event.h"
#define NODE_FEATURE_RELAY   1
#define NODE_FEATURE_PROXY   2
#define NODE_FEATURE_LP      4
#define NODE_FEATURE_FRIEND  8

#define NODE_USE_BLE_5_ADV   0x80

/** @brief Set up mesh features, provision, and composition
 *
 * @param features      (not implemented yet)
 * @param a_prov        provision configuration
 * @param a_comp        composition
 */
void mesh_setup(uint8_t features, const struct bt_mesh_prov *a_prov,
                const struct bt_mesh_comp *a_comp);

#define MESH_MODEL_OP_CLIENT  0
#define MESH_MODEL_OP_SERVER  1

/** @brief set device name for PB_GATT
 *
 * @param name          device name
 */
void mesh_set_dev_name(const char *name);

typedef enum
{
    MESH_PLT_PB_ADV  = 1,
    MESH_PLT_PB_GATT = 2
} mesh_platform_t;

/** @brief platform config
 *
 * @param platform      see mesh_platform_t
 * @param addr          random address for advertising
 * @param param         additional specific data for mesh provisioning vendors (Tianmao, etc)
 */
void mesh_platform_config(mesh_platform_t platform, uint8_t *addr, uint8_t *param);

#endif
