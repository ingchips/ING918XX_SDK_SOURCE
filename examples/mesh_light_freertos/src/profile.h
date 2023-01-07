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



/*--------------------------------------------------------------------
 *----------------------------> MODEL <-------------------------------
 *------------------------------------------------------------------*/
// Ingchips Vendor Model Id
#define INGCHIPS_VND_ID_1  0x0000u
#define INGCHIPS_VND_ID_2  0x0001u

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


// ACCESS
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
    u8_t onoff[2];
    s16_t level[2];
    u16_t lightness[2];
    u16_t hue[2];
    u16_t saturation[2];
    u8_t led_gpio_pin;
} light_state_t;



/*--------------------------------------------------------------------
 *------------------------> PROVISIONING <----------------------------
 *------------------------------------------------------------------*/

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

/**
* @enum prov_oob
*/
typedef enum {
	BT_MESH_PROV_OOB_OTHER     = BIT(0),
	BT_MESH_PROV_OOB_URI       = BIT(1),
	BT_MESH_PROV_OOB_2D_CODE   = BIT(2),
	BT_MESH_PROV_OOB_BAR_CODE  = BIT(3),
	BT_MESH_PROV_OOB_NFC       = BIT(4),
	BT_MESH_PROV_OOB_NUMBER    = BIT(5),
	BT_MESH_PROV_OOB_STRING    = BIT(6),
	/* 7 - 10 are reserved */
	BT_MESH_PROV_OOB_ON_BOX    = BIT(11),
	BT_MESH_PROV_OOB_IN_BOX    = BIT(12),
	BT_MESH_PROV_OOB_ON_PAPER  = BIT(13),
	BT_MESH_PROV_OOB_IN_MANUAL = BIT(14),
	BT_MESH_PROV_OOB_ON_DEV    = BIT(15),
} bt_mesh_prov_oob_info_t;

/** Provisioning properties & capabilities. */
/**
* @struct bt_mesh_prov
*/
struct bt_mesh_prov {
	/** The UUID that's used when advertising as unprovisioned */
	const u8_t *uuid;

	/** Optional URI. This will be advertised separately from the
	 *  unprovisioned beacon, however the unprovisioned beacon will
	 *  contain a hash of it so the two can be associated by the
	 *  provisioner.
	 */
	const char *uri;

	/** Out of Band information field. */
	bt_mesh_prov_oob_info_t oob_info;

	/** Static OOB value */
	const u8_t *static_val;
	/** Static OOB value length */
	u8_t        static_val_len;

	/** Maximum size of Output OOB supported */
	u8_t        output_size;
	/** Supported Output OOB Actions */
	u16_t       output_actions;

	/* Maximum size of Input OOB supported */
	u8_t        input_size;
	/** Supported Input OOB Actions */
	u16_t       input_actions;

	/** @brief Output of a number is requested.
	 *
	 *  This callback notifies the application that it should
	 *  output the given number using the given action.
	 *
	 *  @param act Action for outputting the number.
	 *  @param num Number to be outputted.
	 *
	 *  @return Zero on success or negative error code otherwise
	 */
	int         (*output_number)(bt_mesh_output_action_t act, u32_t num);

	/** @brief Output of a string is requested.
	 *
	 *  This callback notifies the application that it should
	 *  display the given string to the user.
	 *
	 *  @param str String to be displayed.
	 *
	 *  @return Zero on success or negative error code otherwise
	 */
	int         (*output_string)(const char *str);

	/** @brief Input is requested.
	 *
	 *  This callback notifies the application that it should
	 *  request input from the user using the given action. The
	 *  requested input will either be a string or a number, and
	 *  the application needs to consequently call the
	 *  bt_mesh_input_string() or bt_mesh_input_number() functions
	 *  once the data has been acquired from the user.
	 *
	 *  @param act Action for inputting data.
	 *  @param num Maximum size of the inputted data.
	 *
	 *  @return Zero on success or negative error code otherwise
	 */
	int         (*input)(bt_mesh_input_action_t act, u8_t size);

	/** @brief Provisioning link has been opened.
	 *
	 *  This callback notifies the application that a provisioning
	 *  link has been opened on the given provisioning bearer.
	 *
	 *  @param bearer Provisioning bearer.
	 */
	void        (*link_open)(bt_mesh_prov_bearer_t bearer);

	/** @brief Provisioning link has been closed.
	 *
	 *  This callback notifies the application that a provisioning
	 *  link has been closed on the given provisioning bearer.
	 *
	 *  @param bearer Provisioning bearer.
	 */
	void        (*link_close)(bt_mesh_prov_bearer_t bearer);

	/** @brief Provisioning is complete.
	 *
	 *  This callback notifies the application that provisioning has
	 *  been successfully completed, and that the local node has been
	 *  assigned the specified NetKeyIndex and primary element address.
	 *
	 *  @param net_idx NetKeyIndex given during provisioning.
	 *  @param addr Primary element address.
	 */
	void        (*complete)(u16_t net_idx, u16_t addr);

	/** @brief A new node has been added to the provisioning database.
	 *
	 *  This callback notifies the application that provisioning has
	 *  been successfully completed, and that a node has been assigned
	 *  the specified NetKeyIndex and primary element address.
	 *
	 *  @param net_idx NetKeyIndex given during provisioning.
	 *  @param addr Primary element address.
	 *  @param num_elem Number of elements that this node has.
	 */
	void        (*node_added)(u16_t net_idx, u16_t addr, u8_t num_elem);

	/** @brief Node has been reset.
	 *
	 *  This callback notifies the application that the local node
	 *  has been reset and needs to be reprovisioned. The node will
	 *  not automatically advertise as unprovisioned, rather the
	 *  bt_mesh_prov_enable() API needs to be called to enable
	 *  unprovisioned advertising on one or more provisioning bearers.
	 */
	void        (*reset)(void);

    /**@brief provisioner has provisioned a device.
	 *
	 *  This callback notifies the application that the provisioner
	 *  has provisioned a device.
     */
    void        (*provisioner_complete)(u16_t addr);

    /**@brief report to APP the middle status in the provisioner or device.
	 *
	 *  This callback notifies the application
	 *
     */
    void       (*status_report)(u8_t  status_code,u8_t* param);

    /**@brief add for proxy_service.
	 *
	 *  proxy_uuid for pb_gatt or gatt_proxy service.
	 *
     */
    const u8_t *proxy_uuid;
};

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



/*--------------------------------------------------------------------
 *--------------------------> PLATFORM <------------------------------
 *------------------------------------------------------------------*/



/*--------------------------------------------------------------------
 *-----------------------------> API <--------------------------------
 *------------------------------------------------------------------*/

void mesh_init(void);


#endif

