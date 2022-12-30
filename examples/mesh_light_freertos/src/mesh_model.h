#ifndef _MESH_MODEL_H_
#define _MESH_MODEL_H_

#include <stdint.h>
#include <stdio.h>
#include "stdbool.h"

#include "mesh_node.h"
#include "mesh_foundation.h"


/////////////////////////////////////////////////////////////////////////
// mesh_def.h

/**
* @def basic data type
* @brief define the basic data type name reference  */
#define u8_t    uint8_t
#define s8_t    int8_t
#define u16_t   uint16_t
#define s16_t   int16_t
#define u32_t   uint32_t
#define u64_t   uint64_t
#define s64_t   int64_t
#define s32_t   int32_t

// ARRAY
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))





/////////////////////////////////////////////////////////////////////////
// access.h

/** Node Composition */
struct bt_mesh_comp {
    u16_t cid;
    u16_t pid;
    u16_t vid;

    size_t elem_count;
    mesh_element_t *elem;
};

/** Helper to define a mesh element within an array.
 *
 *  In case the element has no SIG or Vendor models the helper
 *  macro BT_MESH_MODEL_NONE can be given instead.
 *
 *  @param _loc       Location Descriptor.
 *  @param _mods      Array of models.
 *  @param _vnd_mods  Array of vendor models.
 */
// typedef struct mesh_element
#define BT_MESH_ELEM(_loc, _mods, _vnd_mods)        \
{                                                   \
    .loc                  = (_loc),                 \
    .sig_models           = (_mods),                \
    .vnd_models           = (_vnd_mods),            \
    .models_count_sig     = 0,      \
    .models_count_vendor  = 0,  \
}

/** Helper to define an empty model array */
#define BT_MESH_MODEL_NONE ((mesh_model_t []){})

// typedef struct mesh_model
#define BT_MESH_MODEL(_id, _op, _pub, _srv)                            \
{                                                                            \
    .model_identifier = mesh_model_get_model_identifier_bluetooth_sig(_id),                                                         \
    .operations = _op,                                                           \
    .appkey_indices = { [0 ... (MAX_NR_MESH_APPKEYS_PER_MODEL - 1)] =             \
            MESH_APPKEY_INVALID },                                \
    .publication_model = _pub,                                                         \
    .subscriptions = { [0 ... (MAX_NR_MESH_SUBSCRIPTION_PER_MODEL - 1)] =         \
            0x0000 },                           \
    .model_data = _srv,                                             \
}

/////////////////////////////////////////////////////////////////////////
// model_srv.h

typedef struct light_state {
    u8_t onoff[2];
    s16_t level[2];
    u16_t lightness[2];
    u16_t hue[2];
    u16_t saturation[2];
    u8_t led_gpio_pin;
} light_state_t;

#include "mesh_generic_on_off_server.h"
#define BT_MESH_MODEL_GEN_ONOFF_SRV(srv, pub)		\
	BT_MESH_MODEL(MESH_SIG_MODEL_ID_GENERIC_ON_OFF_SERVER,	\
                mesh_generic_on_off_server_get_operations(), pub, srv)


struct bt_mesh_gen_onoff_srv_cb {
    int (*get)(mesh_model_t *model, u8_t *state);
    int (*set)(mesh_model_t *model, u8_t state);
    void* light_state;
};

/////////////////////////////////////////////////////////////////////////
// mesh_model.h

void mesh_model_init(void);

#endif

