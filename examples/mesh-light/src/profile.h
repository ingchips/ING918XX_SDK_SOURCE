#ifndef __PROFILE_H_
#define __PROFILE_H_

#include <stdint.h>
#include <stdio.h>
#include "stdbool.h"

#include "mesh_node.h"
#include "mesh_foundation.h"
#include "mesh_generic_model.h"


/*--------------------------------------------------------------------
 *--------------------------> MESH DEFINE <---------------------------
 *------------------------------------------------------------------*/
// ARRAY
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))



/*--------------------------------------------------------------------
 *----------------------------> MODEL <-------------------------------
 *------------------------------------------------------------------*/
// Ingchips Vendor Model Id
#define INGCHIPS_VND_ID_1  0x0000u
#define INGCHIPS_VND_ID_2  0x0001u

/** Mesh Configuration Server Model Context */
typedef struct bt_mesh_cfg_srv {
    uint8_t relay;                /* Relay Mode state */
    uint8_t gatt_proxy;           /* GATT Proxy state */
    uint8_t frnd;                 /* Friend state */
    uint8_t low_pwr;              /* Low Power state */
    uint8_t beacon;               /* Secure Network Beacon state */
    uint8_t default_ttl;          /* Default TTL */    
    uint8_t net_transmit;         /* Network Transmit count */
    uint8_t relay_retransmit;     /* Relay Retransmit count */
}bt_mesh_cfg_srv_t;

/*
* @def  MESH feature switch
* @brief define value to enable or disable mesh feature.
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

/** @def BT_MESH_TRANSMIT
 *
 *  @brief Encode transmission count & interval steps.
 *
 *  @param count   Number of retransmissions (first transmission is excluded).
 *  @param int_ms  Interval steps in milliseconds. Must be greater than 0,
 *                 less than or equal to 320, and a multiple of 10.
 *
 *  @return Mesh transmit value that can be used e.g. for the default
 *          values of the configuration model data.
 */
#define BT_MESH_TRANSMIT(count, int_ms) ((count-1) | (((int_ms / 10) - 1) << 3))

// ACCESS
typedef struct node_info {
    uint16_t cid;
    uint16_t pid;
    uint16_t vid;
}node_info_t;

/** Node Composition */
typedef struct bt_mesh_comp {
    node_info_t info;
    size_t elem_count;
    mesh_element_t *elem;
}bt_mesh_comp_t;

/** Helper to define a mesh element within an array.
 *  @param _loc       Location Descriptor.
 *  @param _mods      Array of models.
 *  @param _vnd_mods  Array of vendor models.
 */
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

#define BT_MESH_MODEL(_id, _srv) \
    BT_MESH_MODEL_VND(BLUETOOTH_COMPANY_ID_BLUETOOTH_SIG_INC, _id, _srv)


// MODEL_SRV
typedef struct light_state {
    uint8_t onoff[2];
    int16_t level[2];
    uint16_t lightness[2];
    uint16_t hue[2];
    uint16_t saturation[2];
    uint8_t led_gpio_pin;
} light_state_t;



/*--------------------------------------------------------------------
 *------------------------> PROVISIONING <----------------------------
 *------------------------------------------------------------------*/
/*
* @def  MESH oob type
*/
#define MESH_OOB_TYPE_NONE          0
#define MESH_OOB_TYPE_STATIC        1
#define MESH_OOB_TYPE_OUTPUT        2
#define MESH_OOB_TYPE_INTPUT        3

/**
* @enum mesh_out_action
*/
typedef enum {
    BT_MESH_NO_OUTPUT       = 0,
    BT_MESH_BLINK           = BIT(0),
    BT_MESH_BEEP            = BIT(1),
    BT_MESH_VIBRATE         = BIT(2),
    BT_MESH_DISPLAY_NUMBER  = BIT(3),
    BT_MESH_DISPLAY_STRING  = BIT(4),
} bt_mesh_output_action_t;

/**
* @enum mesh_in_action
*/
typedef enum {
    BT_MESH_NO_INPUT      = 0,
    BT_MESH_PUSH          = BIT(0),
    BT_MESH_TWIST         = BIT(1),
    BT_MESH_ENTER_NUMBER  = BIT(2),
    BT_MESH_ENTER_STRING  = BIT(3),
} bt_mesh_input_action_t;

/**
* @enum prov_bearer
*/
typedef enum {
    BT_MESH_PROV_ADV   = BIT(0),
    BT_MESH_PROV_GATT  = BIT(1),
} bt_mesh_prov_bearer_t;

/** Provisioning properties & capabilities. */
/**
* @struct bt_mesh_prov
*/
typedef struct bt_mesh_prov {
    /** The UUID that's used when advertising as unprovisioned */
    const uint8_t *uuid;
    
    /* Public Key OOB */
    const uint8_t * public_key;
    const uint8_t * private_key;
    
    /* Static OOB */
    const uint8_t * static_oob_data;
    uint16_t static_oob_len;
    
    /* Output OOB */
    uint16_t output_oob_action;
    uint8_t  output_oob_max_size;
    
    /* Input OOB */
    uint16_t input_oob_action;
    uint8_t  input_oob_max_size;

    /** @brief Output of a number is displayed.
     *  This callback notifies the application that it should
     *  output the given number.
     *  @param num Number to be outputted.
     *  @return Zero on success or negative error code otherwise
     */
    int         (*output_number)(uint32_t num);

    /** @brief Input is requested.
     *
     *  This callback notifies the application that it should
     *  request input from the user. The
     *  requested input will either be a string or a number, and
     *  the application needs to consequently call the
     *  bt_mesh_input_string() or bt_mesh_input_number() functions
     *  once the data has been acquired from the user.
     *  @return Zero on success or negative error code otherwise
     */
    int         (*input_req)(void);

    /** @brief Provisioning is complete.
     *
     *  This callback notifies the application that provisioning has
     *  been successfully completed, and that the local node has been
     *  assigned the specified NetKeyIndex and primary element address.
     *
     *  @param net_idx NetKeyIndex given during provisioning.
     *  @param addr Primary element address.
     */
    void        (*complete)(uint16_t net_idx, uint16_t addr);
    
    /** @brief Node has been reset.
     *
     *  This callback notifies the application that the local node
     *  has been reset and needs to be reprovisioned. The node will
     *  not automatically advertise as unprovisioned, rather the
     *  bt_mesh_prov_enable() API needs to be called to enable
     *  unprovisioned advertising on one or more provisioning bearers.
     */
    void        (*reset)(void);
    
}bt_mesh_prov_t;

/** @brief Enable specific provisioning bearers
 *
 *  Enable one or more provisioning bearers.
 *
 *  @param bearers Bit-wise or of provisioning bearers.
 *
 *  @return Zero on success or (negative) error code otherwise.
 *
 */
int bt_mesh_prov_enable(bt_mesh_prov_bearer_t bearers);

/** @brief Disable specific provisioning bearers
 *
 *  Disable one or more provisioning bearers.
 *
 *  @param bearers Bit-wise or of provisioning bearers.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_prov_disable(bt_mesh_prov_bearer_t bearers);

int bt_mesh_input_string(const uint8_t * str, uint16_t len);
int bt_mesh_input_number(uint32_t num);



/*--------------------------------------------------------------------
 *--------------------------> PLATFORM <------------------------------
 *------------------------------------------------------------------*/
void mesh_platform_init(void);


/*--------------------------------------------------------------------
 *-----------------------------> API <--------------------------------
 *------------------------------------------------------------------*/

void mesh_init(void);


#endif

