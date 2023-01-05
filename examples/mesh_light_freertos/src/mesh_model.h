#ifndef _MESH_MODEL_H_
#define _MESH_MODEL_H_

#include <stdint.h>
#include <stdio.h>
#include "stdbool.h"

#include "mesh_node.h"
#include "mesh_foundation.h"

// Ingchips Vendor Model Id
#define INGCHIPS_VND_ID_1  0x0000u
#define INGCHIPS_VND_ID_2  0x0001u


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

/** Mesh Configuration Server Model Context */
typedef struct bt_mesh_cfg_srv {
	u8_t relay;                /* Relay Mode state */
	u8_t gatt_proxy;           /* GATT Proxy state */
	u8_t frnd;                 /* Friend state */
	u8_t low_pwr;              /* Low Power state */
	u8_t beacon;               /* Secure Network Beacon state */
	u8_t default_ttl;          /* Default TTL */    
	u8_t net_transmit;         /* Network Transmit count */
	u8_t relay_retransmit;     /* Relay Retransmit count */
}bt_mesh_cfg_srv_t;

/*
* @def  MESH feature switch
* @brief define value to enable or disable  mesh feature.
*/
#define BT_MESH_RELAY_DISABLED              0x00
#define BT_MESH_RELAY_ENABLED               0x01
#define BT_MESH_RELAY_NOT_SUPPORTED         0x02

#define BT_MESH_GATT_PROXY_DISABLED         0x00
#define BT_MESH_GATT_PROXY_ENABLED          0x01
#define BT_MESH_GATT_PROXY_NOT_SUPPORTED    0x02

#define BT_MESH_FRIEND_DISABLED             0x00
#define BT_MESH_FRIEND_ENABLED              0x01
#define BT_MESH_FRIEND_NOT_SUPPORTED        0x02

#define BT_MESH_LOW_POWER_DISABLED          0x00
#define BT_MESH_LOW_POWER_ENABLED           0x01
#define BT_MESH_LOW_POWER_NOT_SUPPORTED     0x02

#define BT_MESH_BEACON_DISABLED             0x00
#define BT_MESH_BEACON_ENABLED              0x01

/* Features map*/
#define BT_MESH_FEAT_RELAY                  BIT(0)
#define BT_MESH_FEAT_PROXY                  BIT(1)
#define BT_MESH_FEAT_FRIEND                 BIT(2)
#define BT_MESH_FEAT_LOW_POWER              BIT(3)



/////////////////////////////////////////////////////////////////////////
// access.h

typedef struct node_info {
    u16_t cid;
    u16_t pid;
    u16_t vid;
}node_info_t;

/** Node Composition */
typedef struct bt_mesh_comp {
    node_info_t info;
    size_t elem_count;
    mesh_element_t *elem;
}bt_mesh_comp_t;

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
    .models_count_sig     = ARRAY_SIZE(_mods),      \
    .models_count_vendor  = ARRAY_SIZE(_vnd_mods),  \
}



#define BT_MESH_MODEL_VND(_cid, _id, _srv) \
{ \
    .model_identifier = ((uint32_t) _cid << 16) | _id, \
    .appkey_indices = { [0 ... (MAX_NR_MESH_APPKEYS_PER_MODEL - 1)] = MESH_APPKEY_INVALID }, \
    .subscriptions  = { [0 ... (MAX_NR_MESH_SUBSCRIPTION_PER_MODEL - 1)] = 0x0000 }, \
    .user_data = _srv, \
}

// typedef struct mesh_model
#define BT_MESH_MODEL(_id, _srv) \
    BT_MESH_MODEL_VND(BLUETOOTH_COMPANY_ID_BLUETOOTH_SIG_INC, _id, _srv)







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

typedef struct HSL_VAL
{
    uint16_t lightness;
    uint16_t hue;
    uint16_t sa;
    uint8_t  TID;
    uint8_t  transtime;
    uint8_t  delay;
}hsl_val_t;

struct bt_mesh_gen_onoff_srv_cb {
    int (*get)(mesh_model_t *model, u8_t *state);
    int (*set)(mesh_model_t *model, u8_t state);
    void* light_state;
};

struct bt_mesh_gen_level_srv_cb {
    int (*get)(mesh_model_t *model, s16_t *level);
    int (*set)(mesh_model_t *model, s16_t level);
    void* light_state;
};

struct bt_mesh_light_lightness_srv_cb {
    int (*get)(mesh_model_t *model, u16_t *p_light ,u16_t *light,u8_t * remain);
    int (*set)(mesh_model_t *model, u16_t light);
    void* light_state;
};

struct bt_mesh_light_hsl_srv_cb{
    int (*get)(mesh_model_t *model,uint16_t *hue,uint16_t* sa,uint16_t* lightness,uint8_t* remain );
    int (*set)(mesh_model_t *model,hsl_val_t *val);
    void* light_state;
};




/////////////////////////////////////////////////////////////////////////
// mesh_model.h

void mesh_elems_model_init(void);

#endif

