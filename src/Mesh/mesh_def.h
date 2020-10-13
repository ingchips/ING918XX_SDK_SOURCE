/** @file     mesh_def.h                                                 *                                                                            
*  @brief    function API and data structure in MESH stack              *                                       
*  @version  1.0.0.                                                     *
*  @date     2019/10/10                                                 *
*                                                                       *
*  Copyright (c) 2019 IngChips corp.                                    *
*                                                                       *
*                                                                       *
 *************************************************************************************************************/

#ifndef _MESH_DEF_
#define _MESH_DEF_

#include <assert.h>
#include "gap.h"
#include "errno.h"
#include "nimble_npl.h"
#define MYNEWT_VAL(x)                           MYNEWT_VAL_ ## x
#include "os_mbuf.h"
#include "queue.h"

#define ASSERT_ERR(cond)  \
  { \
    if((cond)==0) \
    { \
        platform_raise_assertion(__MODULE__, __LINE__);\
    } \
  }

/*below for MESH->HOST message invoke*/
#define USER_MSG_ID_CCM_REQ    (0x0f)
#define USER_MSG_ID_AES_REQ    (0x10)
#define USER_MSG_ID_MESH_INIT_DONE (0x11)
#define SCAN_RSP_DATA_SET  0x21
#define EXT_SCAN           0x22
#define EXT_ADV            0x23
#define WHITELST_SET       0x24
#define ADV_PARAM_SET      0x25
#define SCAN_PARAM_SET     0x26
#define RAND_ADDR_SET      0x27
#define ADV_DATA_SET       0x28
#define DISCONNECT         0x29
#define WHITELST_RMV       0x2A
#define NODE_RESET         0x2B

/**
 * @brief CCM and AES structure declaration
 * @defgroup encryption
 * @ingroup bt_mesh
 * @{
 */
 
/** structure definition for CCM, AES for HARDWARE implement */
/**
* @struct ccm_data
*/
typedef struct ccm_data
{
    uint8_t  type;          // 0: encrypt  1: decrypt
    uint8_t  mic_size;
    uint16_t msg_len;
    uint16_t aad_len;
    uint32_t tag;          // same value will be reported in event
    uint8_t *key;
    uint8_t *nonce;
    uint8_t *msg;
    uint8_t *aad;
    uint8_t *out_msg;
}ccm_data_t;

/**
* @struct aes_data
*/
typedef struct aes_data
{
    const uint8_t * key;
    uint8_t * plaintext;
    uint8_t * enc_data;
}aes_data_t; 

/**
 * @}
 */

#define BLE_GAP_EVENT_CONNECT               0
#define BLE_GAP_EVENT_DISCONNECT            1
/* Reserved                                 2 */
#define BLE_GAP_EVENT_CONN_UPDATE           3
#define BLE_GAP_EVENT_CONN_UPDATE_REQ       4
#define BLE_GAP_EVENT_L2CAP_UPDATE_REQ      5
#define BLE_GAP_EVENT_TERM_FAILURE          6
#define BLE_GAP_EVENT_DISC                  7
#define BLE_GAP_EVENT_DISC_COMPLETE         8
#define BLE_GAP_EVENT_ADV_COMPLETE          9
#define BLE_GAP_EVENT_ENC_CHANGE            10
#define BLE_GAP_EVENT_PASSKEY_ACTION        11
#define BLE_GAP_EVENT_NOTIFY_RX             12
#define BLE_GAP_EVENT_NOTIFY_TX             13
#define BLE_GAP_EVENT_SUBSCRIBE             14
#define BLE_GAP_EVENT_MTU                   15
#define BLE_GAP_EVENT_IDENTITY_RESOLVED     16
#define BLE_GAP_EVENT_REPEAT_PAIRING        17
#define BLE_GAP_EVENT_PHY_UPDATE_COMPLETE   18
#define BLE_GAP_EVENT_EXT_DISC              19
#define BLE_GAP_EVENT_PROXY                 20
#define BLE_MESH_APP_EVENT                  21
#define BLE_MESH_BEACON_STATE               22

#define DEVICE_ROLE                         0xfd
#define STACK_INIT_DONE                     0xfe
#define ADV_TERM_CON                        0xfc

typedef enum {
	NODE_ONLY,
	PROVISIONER_ROLE
}mesh_device_role;

/**
 * @brief gap_event interface with host
 * @defgroup gap event structure
 * @ingroup bt_mesh
 * @{
 */
/**
* @struct ble_addr_t2
* 
* @note host stack has defined ble_addr_t type,but is not same structure. so rename ble_addr_t2.
*/
typedef struct {
    uint8_t type;
    uint8_t val[6];
} ble_addr_t2;

/**
* @struct ble_gap_ext_disc_desc
* 
* @note for extended advertising data.
*/
struct ble_gap_ext_disc_desc {
    /*** Common fields. */
    uint8_t props;
    uint8_t data_status;
    uint8_t legacy_event_type;
    ble_addr_t2 addr;
    int8_t rssi;
    uint8_t tx_power;
    uint8_t sid;
    uint8_t prim_phy;
    uint8_t sec_phy;
    uint8_t length_data;
    uint8_t *data;
    /***
     * LE direct advertising report fields; direct_addr is BLE_ADDR_ANY if
     * direct address fields are not present.
     */
    bd_addr_t direct_addr;
};

/**
* @struct ble_gap_disc_desc
* 
* @note for legacy advertising data.
*/
struct ble_gap_disc_desc {
    /*** Common fields. */
    uint8_t event_type;
    uint8_t length_data;
    ble_addr_t2 addr;
    int8_t rssi;
    uint8_t *data;

    /***
     * LE direct advertising report fields; direct_addr is BLE_ADDR_ANY if
     * direct address fields are not present.
     */
    ble_addr_t2 direct_addr;
};

/**
* @struct ble_gap_sec_state
*
* @note   unused currently   private structur.
*/
struct ble_gap_sec_state {
    /** If connection is encrypted */
    unsigned encrypted:1;

    /** If connection is authenticated */
    unsigned authenticated:1;

    /** If connection is bonded (security information is stored)  */
    unsigned bonded:1;

    /** Size of a key used for encryption */
    unsigned key_size:5;
};

/** @brief Connection descriptor 
 *
 *  @note not used the structure in mesh.
 **/
struct ble_gap_conn_desc {
    /** Connection security state */
    struct ble_gap_sec_state sec_state;

    /** Local identity address */
    bd_addr_t our_id_addr;

    /** Peer identity address */
    bd_addr_t peer_id_addr;

    /** Connection handle */
    uint16_t conn_handle;

    /** Connection interval */
    uint16_t conn_itvl;

    /** Connection latency */
    uint16_t conn_latency;

    /** Connection supervision timeout */
    uint16_t supervision_timeout;

    /** Connection Role
     * Possible values BLE_GAP_ROLE_SLAVE or BLE_GAP_ROLE_MASTER
     */
    uint8_t role;

    /** Master clock accuracy */
    uint8_t master_clock_accuracy;
};

/** @brief gap event deliveried to mesh stack. 
 *
 *  @note once the mesh advertising data is received, it should compose the data into such format and deliver to mesh stack
 **/
struct ble_gap_event
{
    /**
     * Indicates the type of GAP event that occurred.  This is one of the
     * BLE_GAP_EVENT codes.
     */
    uint8_t type;

    /**
     * A discriminated union containing additional details concerning the GAP
     * event.  The 'type' field indicates which member of the union is valid.
     */
    union {
        /**
         * Represents a connection attempt.  Valid for the following event
         * types:
         *     o BLE_GAP_EVENT_CONNECT
         */
        struct {
            /**
             * The status of the connection attempt;
             *     o 0: the connection was successfully established.
             *     o BLE host error code: the connection attempt failed for
             *       the specified reason.
             */
            int status;

            /** The handle of the relevant connection. */
            uint16_t conn_handle;

            /** The handle in the server database*/
            uint16_t attr_handle;
            
            /*pointer to the data of the upper data*/
            uint8_t* data;
            
            /*lenth of data*/
            uint8_t  length;            
        } connect;

        /**
         * Represents a terminated connection.  Valid for the following event
         * types:
         *     o BLE_GAP_EVENT_DISCONNECT
         */
        struct {
            /**
             * A BLE host return code indicating the reason for the
             * disconnect.
             */
            int reason;

            /** Information about the connection prior to termination. */
			struct ble_gap_conn_desc conn;
        } disconnect;

        /**
         * Represents an advertising report received during a discovery
         * procedure.  Valid for the following event types:
         *     o BLE_GAP_EVENT_DISC
         */
        struct ble_gap_disc_desc disc;


        /**
         * Represents an extended advertising report received during a discovery
         * procedure.  Valid for the following event types:
         *     o BLE_GAP_EVENT_EXT_DISC
         */
        struct ble_gap_ext_disc_desc ext_disc;

        /**
         * Represents a completed discovery procedure.  Valid for the following
         * event types:
         *     o BLE_GAP_EVENT_DISC_COMPLETE
         */
        struct {
            /**
             * The reason the discovery procedure stopped.  Typical reason
             * codes are:
             *     o 0: Duration expired.
             *     o BLE_HS_EPREEMPTED: Host aborted procedure to configure a
             *       peer's identity.
             */
            int reason;
        } disc_complete;

        /**
         * Represents a completed advertise procedure.  Valid for the following
         * event types:
         *     o BLE_GAP_EVENT_ADV_COMPLETE
         */
        struct {
            /**
             * The reason the advertise procedure stopped.  Typical reason
             * codes are:
             *     o 0: Terminated due to connection.
             *     o BLE_HS_ETIMEOUT: Duration expired.
             *     o BLE_HS_EPREEMPTED: Host aborted procedure to configure a
             *       peer's identity.
             */
            int reason;

        } adv_complete;

        /**
         * Represents a failed attempt to terminate an established connection.
         * Valid for the following event types:
         *     o BLE_GAP_EVENT_TERM_FAILURE
         */
        struct {
            /**
             * A BLE host return code indicating the reason for the failure.
             */
            int status;

            /** The handle of the relevant connection. */
            uint16_t conn_handle;
        } term_failure;
        /**
         * Represents a state change in a peer's subscription status.  In this
         * comment, the term "update" is used to refer to either a notification
         * or an indication.  This event is triggered by any of the following
         * occurrences:
         *     o Peer enables or disables updates via a CCCD write.
         *     o Connection is about to be terminated and the peer is
         *       subscribed to updates.
         *     o Peer is now subscribed to updates after its state was restored
         *       from persistence.  This happens when bonding is restored.
         *
         * Valid for the following event types:
         *     o BLE_GAP_EVENT_SUBSCRIBE
         */
        struct {
            /** The handle of the relevant connection. */
            uint16_t conn_handle;

            /** The value handle of the relevant characteristic. */
            uint16_t attr_handle;

            /** One of the BLE_GAP_SUBSCRIBE_REASON codes. */
            uint8_t reason;

            /** Whether the peer was previously subscribed to notifications. */
            uint8_t prev_notify:1;

            /** Whether the peer is currently subscribed to notifications. */
            uint8_t cur_notify:1;

            /** Whether the peer was previously subscribed to indications. */
            uint8_t prev_indicate:1;

            /** Whether the peer is currently subscribed to indications. */
            uint8_t cur_indicate:1;
        } subscribe;   
        struct{
            uint8_t * data;
            uint8_t length;
        }mesh_app;  

        struct{
            uint8_t enable;
        }beacon_state;
        
        struct{
            uint8_t role;
        }device_role;            

    };
};

/**
 *
 * @}
 */
typedef  struct ble_gap_event  mesh_gap_event;
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

/**
* @brief  the callback function type for acknowledge of mesh config client request.
 * 
 * @param cmd_code  the configuration cliet command code
 *
 * @param status    the result of execution
 */
typedef void (*config_status)(uint16_t cmd_code, uint8_t status);

/**
 * Allocate a mbuf from msys.  Based upon the data size requested,
 * os_msys_get() will choose the mbuf pool that has the best fit.
 *
 * @param dsize The estimated size of the data being stored in the mbuf
 * @param leadingspace The amount of leadingspace to allocate in the mbuf
 *
 * @return A freshly allocated mbuf on success, NULL on failure.
 */
struct os_mbuf *os_msys_get(uint16_t dsize, uint16_t leadingspace);

/**
 * Free a chain of mbufs
 *
 * @param om  The starting mbuf of the chain to free back into the pool
 *
 * @return 0 on success, -1 on failure
 */
int os_mbuf_free_chain(struct os_mbuf *om);


#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define __ASSERT(code, str) \
    do {                          \
        if (!(code)) BT_ERR(str); \
        assert(code);             \
    } while (0);

#define __ASSERT_NO_MSG(test) __ASSERT(test, "")

#define __packed    __attribute__((__packed__))

#define MSEC_PER_SEC   (1000)
#define K_MSEC(ms)     (ms)
#define K_SECONDS(s)   K_MSEC((s) * MSEC_PER_SEC)
#define K_MINUTES(m)   K_SECONDS((m) * 60)
#define K_HOURS(h)     K_MINUTES((h) * 60)

#ifndef BIT
#define BIT(n)  (1UL << (n))
#endif

#define BT_DBG(fmt, ...)    \
    if (BT_DBG_ENABLED) { \
        BLE_HS_LOG(DEBUG, "%s: " fmt "\n", __func__, ## __VA_ARGS__); \
    }
#define BT_INFO(fmt, ...)    \
    if (BT_INFO_ENABLED) { \
        BLE_HS_LOG(INFO, "%s: " fmt "\n", __func__, ## __VA_ARGS__); \
    }
#define BT_WARN(fmt, ...)   \
    if (BT_WARN_ENABLED) {   \
        BLE_HS_LOG(WARN, "%s: " fmt "\n", __func__, ## __VA_ARGS__);  \
    }
    
#define BT_ERR(fmt, ...)  BLE_HS_LOG(ERROR, "%s: " fmt "\n", __func__, ## __VA_ARGS__);


/**
 * @brief  obtain the buffer typed by os_mbuf
 *
 * @param size   the size of the buffer
 *
 * @return  pointer to the buffer
 */

static inline struct os_mbuf * NET_BUF_SIMPLE(uint16_t size)
{
    struct os_mbuf *buf;

    buf = os_msys_get(size, 0);
    assert(buf);

    return buf;
}

#define K_NO_WAIT (0)
#define K_FOREVER (-1)

/*
* @note when mesh beacon has been initialized ,it will signal this message to app.
*/
#define USER_MSG_MESH_INIT_DONE (0x11)

/*
* @note  must not modify the vlue  once defined
*/
#define MYNEWT_VAL_BLE_MESH_MODEL_GROUP_COUNT (2)
#define MYNEWT_VAL_BLE_MESH_MODEL_KEY_COUNT   (2)
#define CONFIG_BT_MESH_MODEL_GROUP_COUNT    MYNEWT_VAL(BLE_MESH_MODEL_GROUP_COUNT)
#define CONFIG_BT_MESH_MODEL_KEY_COUNT      MYNEWT_VAL(BLE_MESH_MODEL_KEY_COUNT)

/*
* @def  MESH feature switch
* @brief define value to enable or disable  mesh feature.
*/
#define BT_MESH_NET_PRIMARY                 0x000

#define BT_MESH_RELAY_DISABLED              0x00
#define BT_MESH_RELAY_ENABLED               0x01
#define BT_MESH_RELAY_NOT_SUPPORTED         0x02

#define BT_MESH_BEACON_DISABLED             0x00
#define BT_MESH_BEACON_ENABLED              0x01

#define BT_MESH_GATT_PROXY_DISABLED         0x00
#define BT_MESH_GATT_PROXY_ENABLED          0x01
#define BT_MESH_GATT_PROXY_NOT_SUPPORTED    0x02

#define BT_MESH_FRIEND_DISABLED             0x00
#define BT_MESH_FRIEND_ENABLED              0x01
#define BT_MESH_FRIEND_NOT_SUPPORTED        0x02

#define BT_MESH_NODE_IDENTITY_STOPPED       0x00
#define BT_MESH_NODE_IDENTITY_RUNNING       0x01
#define BT_MESH_NODE_IDENTITY_NOT_SUPPORTED 0x02

/* Features */
#define BT_MESH_FEAT_RELAY                  BIT(0)
#define BT_MESH_FEAT_PROXY                  BIT(1)
#define BT_MESH_FEAT_FRIEND                 BIT(2)
#define BT_MESH_FEAT_LOW_POWER              BIT(3)

#define BT_MESH_FEAT_SUPPORTED              (BT_MESH_FEAT_RELAY |   \
					                         BT_MESH_FEAT_PROXY |   \
					                         BT_MESH_FEAT_FRIEND |  \
                                             BT_MESH_FEAT_LOW_POWER)

/**
 * @brief Bluetooth Mesh Provisioning
 * @defgroup bt_mesh_prov Bluetooth Mesh Provisioning
 * @ingroup bt_mesh
 * @{
 */

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
    void       (*status_report)(u8_t  status_code);
};
						 
/** @brief Enable specific provisioning bearers
 *
 *  Enable one or more provisioning bearers.
 *
 *  @param bearers Bit-wise or of provisioning bearers.
 *
 *  @return Zero on success or (negative) error code otherwise.
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

/**
 * @}
 */		

/**
* @brief operation on os_mbuf
* @defgroup mesh memory buffer
* @ingroup bluetooth_mesh
* @{
 */

/** @brief put an os_mbuf into a queue and chained.
 *
 *  put an os_mbuf into a queue and chained
 *
 *  @param fifo a queue which chained the data typed by os_mbuf
 *
 *  @param buf pointer to  struct memory  os_mbuf .
 *
 *  @return  void
 */
void net_buf_put(struct ble_npl_eventq *fifo, struct os_mbuf *buf);

/** @brief a reference of the os_mbuf
 *
 *  add a referece couter of this os_mbuf
 *
 *  @param om  a pointer to an struct memory of os_mbuf
 *
 *  @return  the poiniter of om
 */
void * net_buf_ref(struct os_mbuf *om);

/** @brief a unreference of  an os_mbuf
 *
 *  decrease a referece couter of this os_mbuf
 *
 *  @param om  a pointer to an struct memory of os_mbuf
 *
 *  @return  void
 */
void net_buf_unref(struct os_mbuf *om);

/** @brief  get the uint16 value from os_mbuf
 *
 *  to obstain the uint16_t value in litte endium
 *
 *  @param om  a pointer to an struct memory of os_mbuf
 *
 *  @return   uint16_t value
 */
uint16_t net_buf_simple_pull_le16(struct os_mbuf *om);

/** @brief  get the uint16 value from os_mbuf
 *
 *  to obstain the uint16_t value in big endium
 *
 *  @param om  a pointer to an struct memory of os_mbuf
 *
 *  @return   uint16_t value
 */
uint16_t net_buf_simple_pull_be16(struct os_mbuf *om);

/** @brief  get the uint32_t value from os_mbuf
 *
 *  to obstain the uint32_t value in big endium
 *
 *  @param om  a pointer to an struct memory of os_mbuf
 *
 *  @return   uint32_t value
 */
uint32_t net_buf_simple_pull_be32(struct os_mbuf *om);

/** @brief  get the uint32_t value from os_mbuf
 *
 *  to obstain the uint32_t value in little endium
 *
 *  @param om  a pointer to an struct memory of os_mbuf
 *
 *  @return   uint32_t value
 */
uint32_t net_buf_simple_pull_le32(struct os_mbuf *om);

/** @brief  get the uint8_t  value from os_mbuf
 *
 *  to obstain the uint_t value 
 *
 *  @param om  a pointer to an struct memory of os_mbuf
 *
 *  @return   uint8_t value
 */
uint8_t net_buf_simple_pull_u8(struct os_mbuf *om);

/** @brief  add an uint16_t value to os_mbuf
 *
 *  to store an uint16_t  value in little endium ,and it increase the os_mbuf buffer size 
 *
 *  @param om  a pointer to an struct memory of os_mbuf
 *
 *  @return   void
 */
void net_buf_simple_add_le16(struct os_mbuf *om, uint16_t val);

/** @brief  add an uint16_t value to os_mbuf
 *
 *  to store an uint16_t  value in lbig endium ,and it increase the os_mbuf buffer size 
 *
 *  @param om  a pointer to an struct memory of os_mbuf
 *
 *  @param val   value of uint16_t
 *
 *  @return   void
 */

void net_buf_simple_add_be16(struct os_mbuf *om, uint16_t val);

/** @brief  add an uint8_t value to os_mbuf
 *
 *  to store an uint8_t  value ,and it increase the os_mbuf buffer size 
 *
 *  @param om  a pointer to an struct memory of os_mbuf
 * 
 *  @param val   value of uint16_t
 *
 *  @return   void
 */
void net_buf_simple_add_u8(struct os_mbuf *om, uint8_t val);

/** @brief  add an uint32_t value to os_mbuf
 *
 *  to store an uint32_t  value in big endium ,and it increase the os_mbuf buffer size 
 *
 *  @param om  a pointer to an struct memory of os_mbuf
 *
 *  @param val   value of uint8_t
 *
 *  @return   void
 */
void net_buf_simple_add_be32(struct os_mbuf *om, uint32_t val);

/** @brief  add an uint32_t value to os_mbuf
 *
 *  to store an uint32_t  value in little endium ,and it increase the os_mbuf buffer size 
 *
 *  @param om  a pointer to an struct memory of os_mbuf
 *
 *  @param val   value of uint32_t
 *
 *  @return   void
 */
void net_buf_simple_add_le32(struct os_mbuf *om, uint32_t val);

/** @brief  add a series zero value specified by len  to os_mbuf
 *
 *  to store a seriese zero value,and it increase the os_mbuf buffer size 
 *
 *  @param om  a pointer to an struct memory of os_mbuf
 *
 *  @param len   the length of the zero.
 *
 *  @return   void
 *
 *  @note  similarly like function meset(buf,0,len)
 */
void net_buf_add_zeros(struct os_mbuf *om, uint8_t len);


/** @brief  add an uint16_t value to os_mbuf
 *
 *  to store an uint16_t  value in little endium ,the space is reserved in advance.
 *
 *  @param om  a pointer to an struct memory of os_mbuf
 *
 *  @param val   value of uint16_t
 *
 *  @return   void
 */
void net_buf_simple_push_le16(struct os_mbuf *om, uint16_t val);

/** @brief  add an uint16_t value to os_mbuf
 *
 *  to store an uint16_t  value in big endium ,the space is reserved in advance.
 *
 *  @param om  a pointer to an struct memory of os_mbuf
 *
 *  @param val   value of uint16_t
 *
 *  @return   void
 */
void net_buf_simple_push_be16(struct os_mbuf *om, uint16_t val);


/** @brief  add an uint8_t value to os_mbuf
 *
 *  to store an uint8_t  value in big endium ,the space is reserved in advance.
 *
 *  @param om  a pointer to an struct memory of os_mbuf
 *
 *  @param val   value of uint8_t
 *
 *  @return   void
 */
void net_buf_simple_push_u8(struct os_mbuf *om, uint8_t val);

/** @brief  get a series of bytes from os_mbuf
 *
 *  to get a series bytes from os_mbuf
 *
 *  @param om  a pointer to an struct memory of os_mbuf
 *
 *  @param len   lenth of the bytes
 *
 *  @return   void
 *
 *  @note     similarily like function: memcpy
 */
void *net_buf_simple_pull(struct os_mbuf *om, uint8_t len);

/** @brief  store a series of bytes into os_mbuf
 *
 *  to store a series bytes  into os_mbuf ,it will increase the os_mbuf  buffer size
 *
 *  @param om  a pointer to an struct memory of os_mbuf
 *
 *  @param len   lenth of the bytes
 *
 *  @return   pointer to the data block of om_mbuf
 *
 *  @note     similarily like function: memcpy
 */
void *net_buf_simple_add(struct os_mbuf *om, uint8_t len);

/** @brief  store a series of bytes into os_mbuf
 *
 *  to store a series bytes  into os_mbuf ,the space is reserved in advance.
 *
 *  @param om  a pointer to an struct memory of os_mbuf
 *
 *  @param len   lenth of the bytes
 *
 *  @return   pointer to the data block of om_mbuf
 *
 *  @note     similarily like function: memcpy
 */
uint8_t *net_buf_simple_push(struct os_mbuf *om, uint8_t len);

bool k_fifo_is_empty(struct ble_npl_eventq *q);

void *net_buf_get(struct ble_npl_eventq *fifo,s32_t t);

/** @brief  reserver a number of bytes for the future use
 *
 *  to reserve a number of bytes in os_mbuf, it will be taken by function net_buf_simple_push_x
 *
 *  @param om  a pointer to an struct memory of os_mbuf
 *
 *  @param reserve  lenth of space in bytes
 *
 *  @return   void
 *
 */
void net_buf_reserve(struct os_mbuf *om, size_t reserve);

/**
* @}
*/


/**
 * @brief timers or events monitor queue
 * @defgroup k_delayed_work events operation
 * @ingroup bt_mesh
 * @{
 */

/** a struct definition of k_delyed_work . */
/**
* @struct k_delayed_work
*/

struct k_delayed_work {
    struct ble_npl_callout work;
};


/** @brief   timer initialization
 *
 *  to initialize a timer. it will create  timer resouce ,and define the queue where  the timer callback events push to
 *
 *  @param work  a structure to hold the timer resource and event queue that timer routine events would be pushed to
 *
 *  @param handler    a callback function once the timer expires
 *
 *  @return   void
 *
 */
void k_work_init(struct ble_npl_callout *work, ble_npl_event_fn handler);

/** @brief   timer initialization
 *
 *  to initialize a timer. it will create  timer resouce ,and define the queue where  the timer callback events push to
 *
 *  @param w  a structure to hold the timer resource and event queue that timer routine events would be pushed to
 *
 *  @param f    a callback function once the timer expires
 *
 *  @return   void
 *
 */

void k_delayed_work_init(struct k_delayed_work *w, ble_npl_event_fn *f);

/** @brief   timer cancel
 *
 *  to cancel a timer routine.
 *
 *  @param w  a structure to hold the timer resource and event queue that timer routine events would be pushed to
 *
 *  @return   void
 *
 */
void k_delayed_work_cancel(struct k_delayed_work *w);

/** @brief   start a timer 
 *
 *  to start a timer routine
 *
 *  @param w  a structure to hold the timer resource and event queue that timer routine events would be pushed to
 *
 *  @param ms  how long of this timer duraton in unit of millisecond.
 *
 *  @return   void
 *
 */
void k_delayed_work_submit(struct k_delayed_work *w, uint32_t ms);


/** @brief   get current time stamp
 *
 *  to get current time stamp in unit of milisecond.
 *
 *  @return    the time stamp in ms.
 *
 */
int64_t k_uptime_get(void);

/** @brief   get current time stamp
 *
 *  to get current time stamp in unit of milisecond.
 *
 *  @return    the time stamp in ms.
 *
 */
u32_t k_uptime_get_32(void);

/** @brief   to make a task sleep in some time
 *
 *  to make the current task sleep for some time 
 *
 *  @param duration   how long in milliseconds current task would sleep
 *
 *  @return    void
 *
 */
void k_sleep(int32_t duration);

/** @brief   make a event or timer callback routine starts immediately.
 *
 *  start the timer callback routiner without waiting.
 *
 *  @param w   a structure to hold the timer resource and event queue that timer routine events would be pushed to
 *
 *  @return    void
 *
 */
void k_work_submit(struct ble_npl_callout *w);

/** @brief   add arguments associated to  a timer.
 *
 *  add arguments with the specified timer ,and this arguments would be transferred to the timer callback routine.
 *
 *  @param w   a structure to hold the timer resource and event queue that timer routine events would be pushed to
 *
 *  @param arg  the pointer to the arument 
 *
 *  @return    void
 *
 */
void k_work_add_arg(struct ble_npl_callout *w, void *arg);

/** @brief   add arguments associated to  a timer.
 *
 *  add arguments with the specified timer ,and this arguments would be transferred to the timer callback routine.
 *
 *  @param w   a structure k_delayed_work  to hold the timer resource and event queue that timer routine events would be pushed to
 *
 *  @param arg  the pointer to the arument 
 *
 *  @return    void
 *
 */
void k_delayed_work_add_arg(struct k_delayed_work *w, void *arg);

/** @brief   get the remaining time in ms of a timer
 *
 *  get the remainning time of a timer before it start to implement its timer callback routine.
 *
 *  @param w   a structure k_delayed_work  to hold the timer resource and event queue that timer routine events would be pushed to
 *
 *  @return    the remainning time in milliseconds.
 *
 */
uint32_t k_delayed_work_remaining_get(struct k_delayed_work *w);

/**
* @}
*/

/**
 * @brief Bluetooth Mesh
 * @defgroup bt_mesh_cfg_srv Bluetooth Mesh Configuration Server Model
 * @ingroup bt_mesh
 * @{
 */

/** Mesh Configuration Server Model Context */
struct bt_mesh_cfg_srv {
	struct bt_mesh_model *model;

	u8_t net_transmit;         /* Network Transmit state */
	u8_t relay;                /* Relay Mode state */
	u8_t relay_retransmit;     /* Relay Retransmit state */
	u8_t beacon;               /* Secure Network Beacon state */
	u8_t gatt_proxy;           /* GATT Proxy state */
	u8_t frnd;                 /* Friend state */
	u8_t default_ttl;          /* Default TTL */
	/* Heartbeat Publication */
	struct bt_mesh_hb_pub {
		struct k_delayed_work timer;

		u16_t dst;
		u16_t count;
		u8_t  period;
		u8_t  ttl;
		u16_t feat;
		u16_t net_idx;
	} hb_pub;

	/* Heartbeat Subscription */
	struct bt_mesh_hb_sub {
		s64_t  expiry;

		u16_t src;
		u16_t dst;
		u16_t count;
		u8_t  min_hops;
		u8_t  max_hops;

		/* Optional subscription tracking function */
		void (*func)(u8_t hops, u16_t feat);
	} hb_sub;
};

extern const struct bt_mesh_model_op bt_mesh_cfg_srv_op[];

/**
* @def  BT_MESH_MODEL_CFG_SRV
*
*  A helper to define a server model  message.
*
*  @param srv_data Pointer to struct bt_mesh_cfg_srv 
*
*  @return New mesh model instance.

*/
#define BT_MESH_MODEL_CFG_SRV(srv_data)                                      \
		BT_MESH_MODEL(BT_MESH_MODEL_ID_CFG_SRV,                      \
			      bt_mesh_cfg_srv_op, NULL, srv_data)
/**
* @}
*/


/**
 * @brief Mesh Bluetooth Mesh Health Server Model
 * @defgroup bt_mesh_health_srv
 * @ingroup bt_mesh
 * @{
 */

struct bt_mesh_health_srv_cb {
	/* Fetch current faults */
	int (*fault_get_cur)(struct bt_mesh_model *model, u8_t *test_id,
			     u16_t *company_id, u8_t *faults,
			     u8_t *fault_count);

	/* Fetch registered faults */
	int (*fault_get_reg)(struct bt_mesh_model *model, u16_t company_id,
			     u8_t *test_id, u8_t *faults,
			     u8_t *fault_count);

	/* Clear registered faults */
	int (*fault_clear)(struct bt_mesh_model *model, u16_t company_id);

	/* Run a specific test */
	int (*fault_test)(struct bt_mesh_model *model, u8_t test_id,
			  u16_t company_id);

	/* Attention on */
	void (*attn_on)(struct bt_mesh_model *model);

	/* Attention off */
	void (*attn_off)(struct bt_mesh_model *model);
};

/** @def BT_MESH_HEALTH_FAULT_MSG
 *
 *  A helper to define a health fault message.
 *
 *  @param max_faults Maximum number of faults the element can have.
 *
 *  @return a New net_buf_simple of the needed size.
 */
#define BT_MESH_HEALTH_FAULT_MSG(max_faults) \
	NET_BUF_SIMPLE(1 + 3 + (max_faults))

/** Mesh Health Server Model Context */
struct bt_mesh_health_srv {
	struct bt_mesh_model *model;

	/* Optional callback struct */
	const struct bt_mesh_health_srv_cb *cb;

	/* Attention Timer state */
	struct k_delayed_work attn_timer;
};


extern const struct bt_mesh_model_op bt_mesh_health_srv_op[];

/** @def BT_MESH_MODEL_HEALTH_SRV
 *
 *  Define a new health server model. Note that this API needs to be
 *  repeated for each element that the application wants to have a
 *  health server model on. Each instance also needs a unique
 *  bt_mesh_health_srv and bt_mesh_model_pub context.
 *
 *  @param srv Pointer to a unique struct bt_mesh_health_srv.
 *  @param pub Pointer to a unique struct bt_mesh_model_pub.
 *
 *  @return New mesh model instance.
 */
#define BT_MESH_MODEL_HEALTH_SRV(srv, pub)                                   \
		BT_MESH_MODEL(BT_MESH_MODEL_ID_HEALTH_SRV,                   \
			      bt_mesh_health_srv_op, pub, srv)

/**
 * @}
 */

typedef struct app_request
{
    struct bt_mesh_model *model;
    uint16_t app_idx;
    uint16_t dst;
    u32_t opcode;
    uint8_t msg[20];
    uint8_t len; 
    uint8_t bear;
}app_request_t;

/**
* @ref printk
*/
#define printk printf

 /**
* @ enum trace_info
*
* @brief bit sets of mesh trace layer or functional trace
*
*/
typedef enum trace_info{   
    ADV_LAYER   = BIT(0),    ///< bit set of advertise layer for trace
    NET_LAYER   = BIT(1),    ///< bit set of network layer for trace
    TRANS_LAYER = BIT(2),    ///< bit set of transport layer for trace
    ACC_LAYER   = BIT(3),    ///< bit set of access layer for trace
    BEA_FEA     = BIT(4),    ///< bit set of beacon message for trace
    FRND_FEA    = BIT(5),    ///< bit set of friend message for trace 
    LPN_FEA     = BIT(6),    ///< bit set of low power exchange message for trace
    MESH_FEA    = BIT(7),    ///< bit set of mesh related message for trace
    CRYPTO_FEA  = BIT(8),    ///< bit set of encryption or decryption message for trace
    PROV_FEA    = BIT(9),    ///< bit set of proving message for trace 
    PROXY_FEA   = BIT(10),   ///< bit set of proxy message for trace
    SET_FEA     = BIT(11),   ///< bit set of NVM operation message for trace
    CFG_FEA     = BIT(12),   ///< bit set of configure server mssage for trace
    HEALTH_FEA  = BIT(13),   ///< bit set of health model message for trace
    NODE_FEA    = BIT(14)    ///< bit set of node message for trace 
}mesh_trace_info_t;

/**
* @ enum TRACE_CLA
*
* @brief set the trace class
*
*  in mesh trace design there are 3 classes log.  debug, normal and warning message.
*/
enum TRACE_CLA{
    DEBUG_CLA = BIT(0),
    INFO_CLA  = BIT(1),
    WARN_CLA  = BIT(2),
};

#define k_sem ble_npl_sem

static inline void k_sem_init(struct k_sem *sem, unsigned int initial_count,
			      unsigned int limit)
{
	ble_npl_sem_init(sem, initial_count);
}

static inline int k_sem_take(struct k_sem *sem, s32_t timeout)
{
	uint32_t ticks;

	ble_npl_time_ms_to_ticks(timeout, &ticks);
	return - ble_npl_sem_pend(sem,  ticks);

}

static inline void k_sem_give(struct k_sem *sem)
{
	ble_npl_sem_release(sem);  //not used the sem mechanism
}

#endif
