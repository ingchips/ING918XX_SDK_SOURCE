/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef _MESH_GLUE_
#define _MESH_GLUE_

#include <assert.h>
#include "gap.h"
#include "errno.h"

#include "syscfg.h"

#include "nimble_npl.h"

#include "os_mbuf.h"
#include "queue.h"

#include "ble.h"
#include "ble_hs_log.h"
// #include "ble_uuid.h"

//#include "ble_sm_priv.h"
//#include "ble_hs_hci_priv.h"


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
#define STACK_INIT_DONE                     0xfe
#define ADV_TERM_CON                        0xfc
#define u8_t    uint8_t
#define s8_t    int8_t
#define u16_t   uint16_t
#define s16_t   int16_t
#define u32_t   uint32_t
#define u64_t   uint64_t
#define s64_t   int64_t
#define s32_t   int32_t

typedef struct {
    uint8_t type;
    uint8_t val[6];
} ble_addr_t2;

typedef struct advertising_report {
   uint8_t   type;
   uint8_t   event_type;
   uint8_t   address_type;
   bd_addr_t address;
   int8_t   rssi;
   uint8_t   length;
   const uint8_t * data;
} advertising_report_t;

typedef struct ext_advertising_report {
   uint8_t          type;
   uint16_t         event_type;
   bd_addr_type_t   address_type;
   bd_addr_t        address;
   uint8_t          primary_phy;
   uint8_t          sec_phy;
   uint8_t          adv_sid;
   int8_t           tx_pwr;
   int8_t           rssi;
   uint16_t         PRD_ADV_INTER;
   bd_addr_type_t   direct_addr_type;
   bd_addr_t        Direct_Address;
   uint8_t          length;
   const uint8_t *  data;
} ext_advertising_report_t;

typedef struct prd_advertising_report {
   uint8_t   type;
   uint16_t  Sync_Handle;
   int8_t   tx_pwr;
   int8_t   rssi;
   uint8_t   unused;
   uint8_t   status;
   uint8_t   length;
   const uint8_t * data;
} prd_advertising_report_t;

struct ble_gap_ext_adv_params {
    unsigned int connectable:1;
    unsigned int scannable:1;
    unsigned int directed:1;
    unsigned int high_duty_directed:1;
    unsigned int legacy_pdu:1;
    unsigned int anonymous:1;
    unsigned int include_tx_power:1;
    unsigned int scan_req_notif:1;

    uint32_t itvl_min;
    uint32_t itvl_max;
    uint8_t channel_map;
    uint8_t own_addr_type;
    ble_addr_t2 peer;
    uint8_t filter_policy;
    uint8_t primary_phy;
    uint8_t secondary_phy;
    int8_t tx_power;
    uint8_t sid;
};

struct ble_gap_ext_disc_params {
    uint16_t itvl;
    uint16_t window;
    uint8_t passive:1;
};

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

typedef enum {
	GAP_CONNECTION_INVALID,
	GAP_CONNECTION_ACL,
	GAP_CONNECTION_LE
} gap_connection_type_t;

typedef struct le_event_struct_head {
    uint8_t event_code;
    uint8_t total_len;
}le_event_struct_head_t;

struct le_event_advset_Terminate{
    le_event_struct_head_t head;
    uint8_t sub_event_code;
    uint8_t status;
    uint8_t adv_handle;
    uint16_t conn_handle;
    uint8_t num_events;
};

struct ble_gap_disc_params {
    uint16_t itvl;
    uint16_t window;
    uint8_t filter_policy;
    uint8_t limited:1;
    uint8_t passive:1;
    uint8_t filter_duplicates:1;
};

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

struct ble_gap_disc_desc {
    /*** Common fields. */
    uint8_t event_type;
    uint8_t length_data;
    bd_addr_t addr;
    int8_t rssi;
    uint8_t *data;

    /***
     * LE direct advertising report fields; direct_addr is BLE_ADDR_ANY if
     * direct address fields are not present.
     */
    bd_addr_t direct_addr;
};

/** @brief Connection descriptor */
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

    };
};
struct ble_gap_adv_params {
    /** Advertising mode. Can be one of following constants:
     *  - BLE_GAP_CONN_MODE_NON (non-connectable; 3.C.9.3.2).
     *  - BLE_GAP_CONN_MODE_DIR (directed-connectable; 3.C.9.3.3).
     *  - BLE_GAP_CONN_MODE_UND (undirected-connectable; 3.C.9.3.4).
     */
    uint8_t conn_mode;
    /** Discoverable mode. Can be one of following constants:
     *  - BLE_GAP_DISC_MODE_NON  (non-discoverable; 3.C.9.2.2).
     *  - BLE_GAP_DISC_MODE_LTD (limited-discoverable; 3.C.9.2.3).
     *  - BLE_GAP_DISC_MODE_GEN (general-discoverable; 3.C.9.2.4).
     */
    uint8_t disc_mode;

    /** Minimum advertising interval, if 0 stack use sane defaults */
    uint16_t itvl_min;
    /** Maximum advertising interval, if 0 stack use sane defaults */
    uint16_t itvl_max;
    /** Advertising channel map , if 0 stack use sane defaults */
    uint8_t channel_map;

    /** Advertising  Filter policy */
    uint8_t filter_policy;

    /** If do High Duty cycle for Directed Advertising */
    uint8_t high_duty_cycle:1;
};

typedef int ble_gap_event_fn(struct ble_gap_event *event, void *arg);
struct ble_gap_mesh_state {
    ble_gap_event_fn *cb;
    void *cb_arg;
};

extern struct ble_gap_mesh_state ble_gap_mesh;

#if MYNEWT_VAL(BLE_EXT_ADV)
#define BT_MESH_ADV_INST     (MYNEWT_VAL(BLE_MULTI_ADV_INSTANCES))


/* Note that BLE_MULTI_ADV_INSTANCES contains number of additional instances.
 * Instance 0 is always there
 */
#if MYNEWT_VAL(BLE_MULTI_ADV_INSTANCES) < 1
#error "Mesh needs at least BLE_MULTI_ADV_INSTANCES set to 1"
#endif
#define BT_MESH_ADV_GATT_INST     (MYNEWT_VAL(BLE_MULTI_ADV_INSTANCES) - 1)

#endif /* BLE_EXT_ADV */

#define MESH_PROXY_ADV_HANDLE        0x07

#define PB_ADV_HANDLE                0x06


extern uint8_t MESH_PROXY_RANDOM_ADDR[16];
extern uint8_t PB_ADV_RANDOM_ADDR[16];
/** @brief Helper to declare elements of bt_data arrays
 *
 *  This macro is mainly for creating an array of struct bt_data
 *  elements which is then passed to bt_le_adv_start().
 *
 *  @param _type Type of advertising data field
 *  @param _data Pointer to the data field payload
 *  @param _data_len Number of bytes behind the _data pointer
 */
#define BT_DATA(_type, _data, _data_len) \
    { \
        .type = (_type), \
        .data_len = (_data_len), \
        .data = (const u8_t *)(_data), \
    }

/** @brief Helper to declare elements of bt_data arrays
 *
 *  This macro is mainly for creating an array of struct bt_data
 *  elements which is then passed to bt_le_adv_start().
 *
 *  @param _type Type of advertising data field
 *  @param _bytes Variable number of single-byte parameters
 */
#define BT_DATA_BYTES(_type, _bytes...) \
    BT_DATA(_type, ((u8_t []) { _bytes }), \
        sizeof((u8_t []) { _bytes }))

/* EIR/AD data type definitions */
#define BT_DATA_FLAGS                   0x01 /* AD flags */
#define BT_DATA_UUID16_SOME             0x02 /* 16-bit UUID, more available */
#define BT_DATA_UUID16_ALL              0x03 /* 16-bit UUID, all listed */
#define BT_DATA_UUID32_SOME             0x04 /* 32-bit UUID, more available */
#define BT_DATA_UUID32_ALL              0x05 /* 32-bit UUID, all listed */
#define BT_DATA_UUID128_SOME            0x06 /* 128-bit UUID, more available */
#define BT_DATA_UUID128_ALL             0x07 /* 128-bit UUID, all listed */
#define BT_DATA_NAME_SHORTENED          0x08 /* Shortened name */
#define BT_DATA_NAME_COMPLETE           0x09 /* Complete name */
#define BT_DATA_TX_POWER                0x0a /* Tx Power */
#define BT_DATA_SOLICIT16               0x14 /* Solicit UUIDs, 16-bit */
#define BT_DATA_SOLICIT128              0x15 /* Solicit UUIDs, 128-bit */
#define BT_DATA_SVC_DATA16              0x16 /* Service data, 16-bit UUID */
#define BT_DATA_GAP_APPEARANCE          0x19 /* GAP appearance */
#define BT_DATA_SOLICIT32               0x1f /* Solicit UUIDs, 32-bit */
#define BT_DATA_SVC_DATA32              0x20 /* Service data, 32-bit UUID */
#define BT_DATA_SVC_DATA128             0x21 /* Service data, 128-bit UUID */
#define BT_DATA_URI                     0x24 /* URI */
#define BT_DATA_MESH_PROV               0x29 /* Mesh Provisioning PDU */
#define BT_DATA_MESH_MESSAGE            0x2a /* Mesh Networking PDU */
#define BT_DATA_MESH_BEACON             0x2b /* Mesh Beacon */

#define BT_DATA_MANUFACTURER_DATA       0xff /* Manufacturer Specific Data */

#define BT_LE_AD_LIMITED                0x01 /* Limited Discoverable */
#define BT_LE_AD_GENERAL                0x02 /* General Discoverable */
#define BT_LE_AD_NO_BREDR               0x04 /* BR/EDR not supported */

#define sys_put_be16(a,b) put_be16(b, a)
#define sys_put_le16(a,b) put_le16(b, a)
#define sys_put_be32(a,b) put_be32(b, a)
#define sys_get_be16(a) get_be16(a)
#define sys_get_le16(a) get_le16(a)
#define sys_get_be32(a) get_be32(a)
#define sys_cpu_to_be16(a) htobe16(a)
#define sys_cpu_to_be32(a) htobe32(a)
#define sys_be32_to_cpu(a) be32toh(a)
#define sys_be16_to_cpu(a) be16toh(a)
#define sys_le16_to_cpu(a) le16toh(a)

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define CODE_UNREACHABLE __builtin_unreachable()
#define __ASSERT(code, str) \
    do {                          \
        if (!(code)) BT_ERR(str); \
        assert(code);             \
    } while (0);

#define __ASSERT_NO_MSG(test) __ASSERT(test, "")

/* Mesh is designed to not use mbuf chains */
#if BT_DBG_ENABLED
#define ASSERT_NOT_CHAIN(om) assert(SLIST_NEXT(om, om_next) == NULL)
#else
#define ASSERT_NOT_CHAIN(om) (void)(om)
#endif

#define __packed    __attribute__((__packed__))

#define MSEC_PER_SEC   (1000)
#define K_MSEC(ms)     (ms)
#define K_SECONDS(s)   K_MSEC((s) * MSEC_PER_SEC)
#define K_MINUTES(m)   K_SECONDS((m) * 60)
#define K_HOURS(h)     K_MINUTES((h) * 60)

#ifndef BIT
#define BIT(n)  (1UL << (n))
#endif

#define BIT_MASK(n) (BIT(n) - 1)

#define BT_GAP_ADV_FAST_INT_MIN_1               0x0030  /* 30 ms    */
#define BT_GAP_ADV_FAST_INT_MAX_1               0x0060  /* 60 ms    */
#define BT_GAP_ADV_FAST_INT_MIN_2               0x00a0  /* 100 ms   */
#define BT_GAP_ADV_FAST_INT_MAX_2               0x00f0  /* 150 ms   */
#define BT_GAP_ADV_SLOW_INT_MIN                 0x0640  /* 1 s      */
#define BT_GAP_ADV_SLOW_INT_MAX                 0x0780  /* 1.2 s    */

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
#define BT_GATT_ERR(_att_err)   (-(_att_err))

typedef ble_addr_t bt_addr_le_t;

#define k_fifo_init(queue) ble_npl_eventq_init(queue)
#define net_buf_simple_tailroom(buf) OS_MBUF_TRAILINGSPACE(buf)
#define net_buf_tailroom(buf) net_buf_simple_tailroom(buf)
#define net_buf_headroom(buf) ((buf)->om_data - &(buf)->om_databuf[buf->om_pkthdr_len])
#define net_buf_simple_headroom(buf) net_buf_headroom(buf)
#define net_buf_simple_tail(buf) ((buf)->om_data + (buf)->om_len)

struct net_buf_simple_state {
    /** Offset of the data pointer from the beginning of the storage */
    u16_t offset;
    /** Length of data */
    u16_t len;
};

static inline struct os_mbuf * NET_BUF_SIMPLE(uint16_t size)
{
    struct os_mbuf *buf;

    buf = os_msys_get(size, 0);
    assert(buf);

    return buf;
}

#define K_NO_WAIT (0)
#define K_FOREVER (-1)

/* This is by purpose */
static inline void net_buf_simple_init(struct os_mbuf *buf,
                                       size_t reserve_head)
{
    /* This is called in Zephyr after init.
     * Note in Mynewt case we don't care abour reserved head*/
    buf->om_data = &buf->om_databuf[buf->om_pkthdr_len] + reserve_head;
    buf->om_len = 0;
}

void net_buf_put(struct ble_npl_eventq *fifo, struct os_mbuf *buf);
void * net_buf_ref(struct os_mbuf *om);
void net_buf_unref(struct os_mbuf *om);
uint16_t net_buf_simple_pull_le16(struct os_mbuf *om);
uint16_t net_buf_simple_pull_be16(struct os_mbuf *om);
uint32_t net_buf_simple_pull_be32(struct os_mbuf *om);
uint32_t net_buf_simple_pull_le32(struct os_mbuf *om);
uint8_t net_buf_simple_pull_u8(struct os_mbuf *om);
void net_buf_simple_add_le16(struct os_mbuf *om, uint16_t val);
void net_buf_simple_add_be16(struct os_mbuf *om, uint16_t val);
void net_buf_simple_add_u8(struct os_mbuf *om, uint8_t val);
void net_buf_simple_add_be32(struct os_mbuf *om, uint32_t val);
void net_buf_simple_add_le32(struct os_mbuf *om, uint32_t val);
void net_buf_add_zeros(struct os_mbuf *om, uint8_t len);
void net_buf_simple_push_le16(struct os_mbuf *om, uint16_t val);
void net_buf_simple_push_be16(struct os_mbuf *om, uint16_t val);
void net_buf_simple_push_u8(struct os_mbuf *om, uint8_t val);
void *net_buf_simple_pull(struct os_mbuf *om, uint8_t len);
void *net_buf_simple_add(struct os_mbuf *om, uint8_t len);
bool k_fifo_is_empty(struct ble_npl_eventq *q);
void *net_buf_get(struct ble_npl_eventq *fifo,s32_t t);
uint8_t *net_buf_simple_push(struct os_mbuf *om, uint8_t len);
void net_buf_reserve(struct os_mbuf *om, size_t reserve);

#define net_buf_add_mem(a,b,c) os_mbuf_append(a,b,c)
#define net_buf_simple_add_mem(a,b,c) os_mbuf_append(a,b,c)
#define net_buf_add_u8(a,b) net_buf_simple_add_u8(a,b)
#define net_buf_add(a,b) net_buf_simple_add(a,b)

#define net_buf_clone(a, b) os_mbuf_dup(a)
#define net_buf_add_be32(a, b) net_buf_simple_add_be32(a, b)
#define net_buf_add_be16(a, b) net_buf_simple_add_be16(a, b)

#define BT_GATT_CCC_NOTIFY BLE_GATT_CHR_PROP_NOTIFY
#define bt_gatt_attr ble_gatt_attr

/** Description of different data types that can be encoded into
  * advertising data. Used to form arrays that are passed to the
  * bt_le_adv_start() function.
  */
struct bt_data {
    u8_t type;
    u8_t data_len;
    const u8_t *data;
};

struct bt_pub_key_cb {
    /** @brief Callback type for Public Key generation.
     *
     *  Used to notify of the local public key or that the local key is not
     *  available (either because of a failure to read it or because it is
     *  being regenerated).
     *
     *  @param key The local public key, or NULL in case of no key.
     */
    void (*func)(const u8_t key[64]);

    struct bt_pub_key_cb *_next;
};

typedef void (*bt_dh_key_cb_t)(const u8_t key[32]);
int bt_dh_key_gen(const u8_t remote_pk[64], bt_dh_key_cb_t cb);
int bt_pub_key_gen(struct bt_pub_key_cb *new_cb);
uint8_t *bt_pub_key_get(void);
int bt_rand(void *buf, size_t len);
const char * bt_hex(const void *buf, size_t len);
int bt_encrypt_be(const uint8_t *key, const uint8_t *plaintext, uint8_t *enc_data);
// void bt_mesh_register_gatt(void);
// WARNING: ^^^ this API is not available in this release

int bt_le_adv_start(const struct ble_gap_adv_params *param,
                    const struct bt_data *ad, size_t ad_len,
                    const struct bt_data *sd, size_t sd_len);
int bt_le_adv_stop(bool proxy);

struct k_delayed_work {
    struct ble_npl_callout work;
};

void k_work_init(struct ble_npl_callout *work, ble_npl_event_fn handler);
void k_delayed_work_init(struct k_delayed_work *w, ble_npl_event_fn *f);
void k_delayed_work_cancel(struct k_delayed_work *w);
void k_delayed_work_submit(struct k_delayed_work *w, uint32_t ms);
int64_t k_uptime_get(void);
u32_t k_uptime_get_32(void);
void k_sleep(int32_t duration);
void k_work_submit(struct ble_npl_callout *w);
void k_work_add_arg(struct ble_npl_callout *w, void *arg);
void k_delayed_work_add_arg(struct k_delayed_work *w, void *arg);
uint32_t k_delayed_work_remaining_get(struct k_delayed_work *w);

static inline void net_buf_simple_save(struct os_mbuf *buf,
                       struct net_buf_simple_state *state)
{
    state->offset = net_buf_simple_headroom(buf);
    state->len = buf->om_len;
}

static inline void net_buf_simple_restore(struct os_mbuf *buf,
                                          struct net_buf_simple_state *state)
{
      buf->om_data = &buf->om_databuf[buf->om_pkthdr_len] + state->offset;
      buf->om_len = state->len;
}

static inline void sys_memcpy_swap(u8_t *dst, const u8_t *src, size_t length)
{
    __ASSERT(((src < dst && (src + length) <= dst) ||
          (src > dst && (dst + length) <= src)),
         "Source and destination buffers must not overlap");

    src += length - 1;

    for (; length > 0; length--) {
        *((u8_t *)dst++) = *((u8_t *)src--);
    }
}

#define popcount(x) __builtin_popcount(x)

static inline unsigned int find_lsb_set(u32_t op)
{
    return __builtin_ffs(op);
}

static inline unsigned int find_msb_set(u32_t op)
{
    if (!op)
        return 0;

    return 32 - __builtin_clz(op);
}

extern uint32_t MYNEWT_VAL_BLE_MESH_GATT_PROXY;
#define CONFIG_BT_MESH_FRIEND               BLE_MESH_FRIEND

#define CONFIG_BT_MESH_IV_UPDATE_TEST       BLE_MESH_IV_UPDATE_TEST
#define CONFIG_BT_MESH_LOW_POWER            BLE_MESH_LOW_POWER
#define CONFIG_BT_MESH_LPN_AUTO             BLE_MESH_LPN_AUTO
#define CONFIG_BT_MESH_LPN_ESTABLISHMENT    BLE_MESH_LPN_ESTABLISHMENT
#define CONFIG_BT_MESH_PB_ADV               BLE_MESH_PB_ADV
#define CONFIG_BT_MESH_PB_GATT              BLE_MESH_PB_GATT
#define CONFIG_BT_MESH_PROV                 BLE_MESH_PROV
#define CONFIG_BT_TESTING                   BLE_MESH_TESTING
#define CONFIG_BT_SETTINGS                  BLE_MESH_SETTINGS
#define CONFIG_SETTINGS                     BLE_MESH_SETTINGS
#define BT_SETTINGS                         BLE_MESH_SETTINGS

/* Above flags are used with IS_ENABLED macro */
#define IS_ENABLED(config) MYNEWT_VAL(config)

#define CONFIG_BT_MESH_LPN_GROUPS           MYNEWT_VAL(BLE_MESH_LPN_GROUPS)
#define CONFIG_BT_MESH_ADV_BUF_COUNT        MYNEWT_VAL(BLE_MESH_ADV_BUF_COUNT)
#define CONFIG_BT_MESH_FRIEND_QUEUE_SIZE    MYNEWT_VAL(BLE_MESH_FRIEND_QUEUE_SIZE)
#define CONFIG_BT_MESH_FRIEND_RECV_WIN      MYNEWT_VAL(BLE_MESH_FRIEND_RECV_WIN)
#define CONFIG_BT_MESH_LPN_POLL_TIMEOUT     MYNEWT_VAL(BLE_MESH_LPN_POLL_TIMEOUT)
#define CONFIG_BT_MESH_MODEL_GROUP_COUNT    MYNEWT_VAL(BLE_MESH_MODEL_GROUP_COUNT)
#define CONFIG_BT_MESH_MODEL_KEY_COUNT      MYNEWT_VAL(BLE_MESH_MODEL_KEY_COUNT)
#define CONFIG_BT_MESH_NODE_ID_TIMEOUT      MYNEWT_VAL(BLE_MESH_NODE_ID_TIMEOUT)
#define CONFIG_BT_MAX_CONN                  MYNEWT_VAL(BLE_MAX_CONNECTIONS)
#define CONFIG_BT_MESH_SEQ_STORE_RATE       MYNEWT_VAL(BLE_MESH_SEQ_STORE_RATE)
#define CONFIG_BT_MESH_RPL_STORE_TIMEOUT    MYNEWT_VAL(BLE_MESH_RPL_STORE_TIMEOUT)
#define CONFIG_BT_MESH_APP_KEY_COUNT        MYNEWT_VAL(BLE_MESH_APP_KEY_COUNT)
#define CONFIG_BT_MESH_SUBNET_COUNT         MYNEWT_VAL(BLE_MESH_SUBNET_COUNT)
#define CONFIG_BT_MESH_STORE_TIMEOUT        MYNEWT_VAL(BLE_MESH_STORE_TIMEOUT)
#define CONFIG_BT_MESH_IVU_DIVIDER          MYNEWT_VAL(BLE_MESH_IVU_DIVIDER)
#define CONFIG_BT_DEVICE_NAME               MYNEWT_VAL(BLE_MESH_DEVICE_NAME)
#define CONFIG_BT_MESH_TX_SEG_MAX           MYNEWT_VAL(BLE_MESH_TX_SEG_MAX)

#define printk printf

#define CONTAINER_OF(ptr, type, field) \
	((type *)(((char *)(ptr)) - offsetof(type, field)))


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

/* Helpers to access the storage array, since we don't have access to its
 * type at this point anymore.
 */

#define BUF_SIZE(pool) (pool->omp_pool->mp_block_size)

static inline int net_buf_id(struct os_mbuf *buf)
{
	struct os_mbuf_pool *pool = buf->om_omp;
	u8_t *pool_start = (u8_t *)pool->omp_pool->mp_membuf_addr;
	u8_t *buf_ptr = (u8_t *)buf;

	return (buf_ptr - pool_start) / BUF_SIZE(pool);
}

/* XXX: We should not use os_mbuf_pkthdr chains to represent a list of
 * packets, this is a hack. For now this is not an issue, because mesh
 * does not use os_mbuf chains. We should change this in the future.
 */
STAILQ_HEAD(net_buf_slist_t, os_mbuf_pkthdr);

void net_buf_slist_init(struct net_buf_slist_t *list);
bool net_buf_slist_is_empty(struct net_buf_slist_t *list);
struct os_mbuf *net_buf_slist_peek_head(struct net_buf_slist_t *list);
struct os_mbuf *net_buf_slist_peek_next(struct os_mbuf *buf);
struct os_mbuf *net_buf_slist_get(struct net_buf_slist_t *list);
void net_buf_slist_put(struct net_buf_slist_t *list, struct os_mbuf *buf);
void net_buf_slist_remove(struct net_buf_slist_t *list, struct os_mbuf *prev,
			  struct os_mbuf *cur);
void net_buf_slist_merge_slist(struct net_buf_slist_t *list,
			       struct net_buf_slist_t *list_to_append);
#define NET_BUF_SLIST_FOR_EACH_NODE(head, var) STAILQ_FOREACH(var, head, omp_next)

#if MYNEWT_VAL(BLE_MESH_SETTINGS)

#define settings_load conf_load
int settings_bytes_from_str(char *val_str, void *vp, int *len);
char *settings_str_from_bytes(void *vp, int vp_len, char *buf, int buf_len);

#define snprintk snprintf
#define BT_SETTINGS_SIZE(in_size)   (in_size+1)  
#define settings_save_one save_binary_value

#else

static inline int
settings_load(void)
{
	return 0;
}

#endif /* MYNEWT_VAL(MYNEWT_VAL_BLE_MESH_SETTINGS) */

int PB_ADV_config(void);
int le_adv_status_reset(bool proxy);
#define BUILD_ASSERT(cond)  \                 //modified by dengyiyun

typedef  struct ble_gap_event  mesh_gap_event;

/*below for MESH->HOST message invoke*/
#define USER_MSG_ID_CCM_REQ    (0x0f)
#define SCAN_RSP_DATA_SET  0x21
#define EXT_SCAN           0x22
#define EXT_ADV            0x23
#define WHITELST_SET       0x24
#define ADV_PARAM_SET      0x25
#define SCAN_PARAM_SET     0x26
#define RAND_ADDR_SET      0x27
#define ADV_DATA_SET       0x28

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


typedef struct ext_scan
{
    uint8_t enable;
    uint8_t filter_dup;
    uint16_t dur;
    uint16_t peri;
}ext_scan_t;

typedef struct scan_param
{
    uint8_t addr_type;
    uint8_t pol;
    uint8_t num;
    scan_phy_config_t config[2];    
}scan_param_t;

typedef struct scan_rsp_data
{
    uint8_t  adv_handle;
    uint8_t  length;
    uint8_t* data;
}scan_rsp_data_t;

typedef struct adv_param
{
    uint8_t  adv_handle;
    uint8_t  prop;
    uint32_t inter_min;
    uint32_t inter_max;
    uint8_t  map;
    uint8_t  addr_type;
    uint8_t  peer_addr_type;
    bd_addr_t peer_addr;
    uint8_t  filter;
    uint8_t  tx_power;
    uint8_t  adv_phy;
    uint8_t  max_skip;
    uint8_t  sec_phy;
    uint8_t  sid;
    uint8_t  notify;    
}adv_param_t;

typedef struct rand_addr
{
    uint8_t adv_handle;
    bd_addr_t addr;
}rand_addr_t;


typedef struct adv_en
{
    uint8_t en;
    uint8_t setsnum;
    ext_adv_set_en_t adv_set_conf;
}adv_en_t;

typedef struct adv_data
{
    uint8_t adv_handle;
    uint8_t len;
    uint8_t * data;
}adv_data_t;

typedef struct whitelst
{
    uint8_t type;
    bd_addr_t whitaddr;
}whitelst_t;

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

typedef enum trace_info{   
    ADV_LAYER   = BIT(0),
    NET_LAYER   = BIT(1),
    TRANS_LAYER = BIT(2),
    ACC_LAYER   = BIT(3),
    BEA_FEA     = BIT(4),
    FRND_FEA    = BIT(5),
    LPN_FEA     = BIT(6),
    MESH_FEA    = BIT(7),
    CRYPTO_FEA  = BIT(8),
    PROV_FEA    = BIT(9),
    PROXY_FEA   = BIT(10),
    SET_FEA     = BIT(11),
    CFG_FEA     = BIT(12),
    HEALTH_FEA  = BIT(13),
}trace_info_t;

enum TRACE_CLA{
    DEBUG_CLA = BIT(0),
    INFO_CLA  = BIT(1),
    WARN_CLA  = BIT(2),
};

//below used to setup the uart data output function from APP
typedef void (*mesh_at_out)(const char* buf, int len);
#endif
