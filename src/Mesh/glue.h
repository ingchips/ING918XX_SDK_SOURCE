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
#include "mesh_def.h"
#include "syscfg.h"
#include "access.h"
#include "nimble_npl.h"

#include "os_mbuf.h"
#include "queue.h"

#include "ble.h"
#include "ble_hs_log.h"

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

#define BT_MESH_SCAN_INTERVAL_MS 20
#define BT_MESH_SCAN_WINDOW_MS   20
#define BT_MESH_SCAN_INTERVAL    ADV_SCAN_UNIT(BT_MESH_SCAN_INTERVAL_MS)
#define BT_MESH_SCAN_WINDOW      ADV_SCAN_UNIT(BT_MESH_SCAN_WINDOW_MS)

#define BT_GAP_ADV_FAST_INT_MIN_1               0x0030  /* 30 ms    */
#define BT_GAP_ADV_FAST_INT_MAX_1               0x0060  /* 60 ms    */
#define BT_GAP_ADV_FAST_INT_MIN_2               0x00a0  /* 100 ms   */
#define BT_GAP_ADV_FAST_INT_MAX_2               0x00f0  /* 150 ms   */
#define BT_GAP_ADV_SLOW_INT_MIN                 (480)
#define BT_GAP_ADV_SLOW_INT_MAX                 (480)
//#define BT_GAP_ADV_SLOW_INT_MIN                 0x0640  /* 1 s      */
//#define BT_GAP_ADV_SLOW_INT_MAX                 0x0780  /* 1.2 s    */

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


#define CODE_UNREACHABLE __builtin_unreachable()


/* Mesh is designed to not use mbuf chains */
#if BT_DBG_ENABLED
#define ASSERT_NOT_CHAIN(om) assert(SLIST_NEXT(om, om_next) == NULL)
#else
#define ASSERT_NOT_CHAIN(om) (void)(om)
#endif



#define BIT_MASK(n) (BIT(n) - 1)

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


/* This is by purpose */
static inline void net_buf_simple_init(struct os_mbuf *buf,
                                       size_t reserve_head)
{
    /* This is called in Zephyr after init.
     * Note in Mynewt case we don't care abour reserved head*/
    buf->om_data = &buf->om_databuf[buf->om_pkthdr_len] + reserve_head;
    buf->om_len = 0;
}



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
int bt_rand(u8_t *buf, size_t len);
const char * bt_hex(const void *buf, size_t len);
int bt_encrypt_be(const uint8_t *key, const uint8_t *plaintext, uint8_t *enc_data);
void bt_mesh_register_gatt(void);
int bt_le_adv_start(const struct ble_gap_adv_params *param,
                    const struct bt_data *ad, size_t ad_len,
                    const struct bt_data *sd, size_t sd_len);
int bt_le_adv_stop(bool proxy);



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
#define CONFIG_BT_MESH_PROVISIONER          BLE_MESH_PROVISIONER

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
#define CONFIG_BT_MESH_NODE_COUNT           MYNEWT_VAL(BLE_MESH_NODE_COUNT)

#define CONTAINER_OF(ptr, type, field) \
	((type *)(((char *)(ptr)) - offsetof(type, field)))


#define k_sem ble_npl_sem

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

typedef struct ext_scan
{
    uint8_t enable;
    uint8_t filter_dup;
    uint16_t dur;
    uint16_t peri;
}ext_scan_t;

typedef struct scan_param
{
    bd_addr_type_t addr_type;
    scan_filter_policy_t pol;
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
    adv_event_properties_t  prop;
    uint32_t inter_min;
    uint32_t inter_max;
    uint8_t  map;
    bd_addr_type_t  addr_type;
    bd_addr_type_t  peer_addr_type;
    bd_addr_t peer_addr;
    adv_filter_policy_t  filter;
    uint8_t  tx_power;
    phy_type_t  adv_phy;
    uint8_t  max_skip;
    phy_type_t  sec_phy;
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

/** @brief Initialize Mesh support
 *
 *  After calling this API, the node will not automatically advertise as
 *  unprovisioned, rather the bt_mesh_prov_enable() API needs to be called
 *  to enable unprovisioned advertising on one or more provisioning bearers.
 *
 *  @param own_addr_type Node address type
 *  @param prov Node provisioning information.
 *  @param comp Node Composition.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_init(u8_t own_addr_type,
		 const struct bt_mesh_prov *prov,
		 const struct bt_mesh_comp *comp);

/** @brief Reset the state of the local Mesh node.
 *
 *  Resets the state of the node, which means that it needs to be
 *  reprovisioned to become an active node in a Mesh network again.
 *
 *  After calling this API, the node will not automatically advertise as
 *  unprovisioned, rather the bt_mesh_prov_enable() API needs to be called
 *  to enable unprovisioned advertising on one or more provisioning bearers.
 *
 */
void bt_mesh_reset(bool);

/** @brief Provision the local Mesh Node.
 *
 *  This API should normally not be used directly by the application. The
 *  only exception is for testing purposes where manual provisioning is
 *  desired without an actual external provisioner.
 *
 *  @param net_key  Network Key
 *  @param net_idx  Network Key Index
 *  @param flags    Provisioning Flags
 *  @param iv_index IV Index
 *  @param addr     Primary element address
 *  @param dev_key  Device Key
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_provision(const u8_t net_key[16], u16_t net_idx,
		      u8_t flags, u32_t iv_index, u16_t addr,
		      const u8_t dev_key[16]);

/** @brief Check if the local node has been provisioned.
 *
 *  This API can be used to check if the local node has been provisioned
 *  or not. It can e.g. be helpful to determine if there was a stored
 *  network in flash, i.e. if the network was restored after calling
 *  settings_load().
 *
 *  @return True if the node is provisioned. False otherwise.
 */
bool bt_mesh_is_provisioned(void);

/** @brief Toggle the IV Update test mode
 *
 *  This API is only available if the IV Update test mode has been enabled
 *  in Kconfig. It is needed for passing most of the IV Update qualification
 *  test cases.
 *
 *  @param enable true to enable IV Update test mode, false to disable it.
 */
void bt_mesh_iv_update_test(bool enable);

/** @brief Toggle the IV Update state
 *
 *  This API is only available if the IV Update test mode has been enabled
 *  in Kconfig. It is needed for passing most of the IV Update qualification
 *  test cases.
 *
 *  @return true if IV Update In Progress state was entered, false otherwise.
 */
bool bt_mesh_iv_update(void);

/** @brief Toggle the Low Power feature of the local device
 *
 *  Enables or disables the Low Power feature of the local device. This is
 *  exposed as a run-time feature, since the device might want to change
 *  this e.g. based on being plugged into a stable power source or running
 *  from a battery power source.
 *
 *  @param enable  true to enable LPN functionality, false to disable it.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_lpn_set(bool enable);

/** @brief Send out a Friend Poll message.
 *
 *  Send a Friend Poll message to the Friend of this node. If there is no
 *  established Friendship the function will return an error.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_lpn_poll(void);

/** @brief Register a callback for Friendship changes.
 *
 *  Registers a callback that will be called whenever Friendship gets
 *  established or is lost.
 *
 *  @param cb Function to call when the Friendship status changes.
 */
void bt_mesh_lpn_set_cb(void (*cb)(u16_t friend_addr, bool established));

#define ASSERT_ERR(cond)  \
  { \
    if((cond)==0) \
    { \
        platform_raise_assertion(__MODULE__, __LINE__);\
    } \
  }
/**
 * @}
 */

extern uint32_t sleep_duration;
extern uint16_t fea_layer_sel;
extern uint8_t cla_flag;

#endif
