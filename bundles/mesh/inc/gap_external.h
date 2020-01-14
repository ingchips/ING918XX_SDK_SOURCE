// ----------------------------------------------------------------------------
// Copyright Message                                                           
// ----------------------------------------------------------------------------
//                                                                             
// INGCHIPS confidential and proprietary.                                      
// COPYRIGHT (c) 2018 by INGCHIPS                                              
//                                                                             
// All rights are reserved. Reproduction in whole or in part is                
// prohibited without the written consent of the copyright owner.              
//                                                                             
// ----------------------------------------------------------------------------
// Design Information                                                          
// ----------------------------------------------------------------------------
//                                                                             
// File         : gap_external.h
//                                                                             
// Author       : YiyunDeng
//                                                                             
// Organisation : INGCHIPS                                                     
//                                                                             
// Date         : 2019-03-01
//                                                                             
// Revision     : v0.1                                                         
//                                                                             
// Project      : ING91800                                                     
//                                                                             
// Description  :                                                              
//                                                                             
// ----------------------------------------------------------------------------
// Revision History                                                            
// ----------------------------------------------------------------------------
//                                                                             
// Date       Author          Revision  Change Description                     
// ========== =============== ========= =======================================
// 2019/03/01 Dengyiyun       v0.1       Creat
//                                                                             
// ----------------------------------------------------------------------------
 
#ifndef __GAP_EXTERNAL_H
#define __GAP_EXTERNAL_H

#include <stdint.h>
#include "bt_types.h"
#include "btstack_config.h"
#if defined __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t type;
    uint8_t val[6];
} ble_addr_t2;

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


/*bit configure for */
typedef enum PHY_BIT_TYPE
{
    PHY_CODED_BIT = BIT(2),
    PHY_1M_BIT    = BIT(0),
    PHY_2M_BIT    = BIT(1),
}phy_bittype_t;

typedef enum PHY_TYPE
{
     PHY_1M = 0x01,
     PHY_2M = 0x02,
     PHY_CODED = 0x03,
}phy_type_t;

typedef enum scan_type
{
    PASSIVE_SCAN = 0x00,
    ACTIVE_SCAN  = 0x01, 
} scan_type_t;

typedef enum Advertising_Event_Properties
{
    CONNECTABLE_ADV_BIT       = BIT(0),
    SCANNABLE_ADV_BIT         = BIT(1),
    DIRECT_ADV_BIT            = BIT(2),
    HIGH_DUTY_CIR_DIR_ADV_BIT = BIT(3),
    LEGACY_PDU_BIT            = BIT(4),         
    ANONY_ADV_BIT             = BIT(5),
    INC_TX_ADV_BIT            = BIT(6)
}Advertising_Event_Properties_t;

typedef enum Periodic_Advertising_Properties
{
    INC_TX_ADV_PERIOD_BIT     = BIT(6),
}Periodic_Advertising_Properties_t;

typedef  struct ble_gap_event  mesh_gap_event;
#if BLE50_ENABLED
/* Ext Adv Set enable parameters, not in HCI order */

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
#endif


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

    /** Local over-the-air address */
    bd_addr_t our_ota_addr;

    /** Peer over-the-air address */
    bd_addr_t peer_ota_addr;

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

#if (BLE50_ENABLED)
        /**
         * Represents an extended advertising report received during a discovery
         * procedure.  Valid for the following event types:
         *     o BLE_GAP_EVENT_EXT_DISC
         */
        struct ble_gap_ext_disc_desc ext_disc;
#endif

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

#if 0 //(BLE50_ENABLED)
            /** Advertising instance */
            uint8_t instance;
            /** The handle of the relevant connection - valid if reason=0 */
            uint16_t conn_handle;
            /**
             * Number of completed extended advertising events
             *
             * This field is only valid if non-zero max_events was passed to
             * ble_gap_ext_adv_start() and advertising completed due to duration
             * timeout or max events transmitted.
             * */
            uint8_t num_ext_adv_events;
#endif
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

typedef struct le_connection_parameter_range{
    uint16_t le_conn_interval_min;
    uint16_t le_conn_interval_max;
    uint16_t le_conn_latency_min;
    uint16_t le_conn_latency_max;
    uint16_t le_supervision_timeout_min;
    uint16_t le_supervision_timeout_max;
} le_connection_parameter_range_t;

/***add by haoyanbiao 2016.11.23****/
typedef enum{

  //DEVDISC_MODE_NONDISCOVERABLE, 
  //No discoverable setting. 
  GENERAL_DISCOV_PROC=0X01,
  //General Discoverable devices. 
  LIMITED_DISCOV_PROC,
  //Limited Discoverable devices. 
  GENERAL_CONNECTION_PROC,
 //DEVDISC_MODE_ALL
}gap_pro_state_t;

typedef struct pro_ongoing{
gap_pro_state_t discover_type;
}pro_ongoing_t;

// void gap_dispose_set_scan_para_complete(uint8_t status);
typedef void  (*f_gap_dispose_set_scan_para_complete)(uint8_t status);
#define gap_dispose_set_scan_para_complete ((f_gap_dispose_set_scan_para_complete)(0x000129f9))

// void gap_dispose_set_adv_para_complete(uint8_t status);
typedef void  (*f_gap_dispose_set_adv_para_complete)(uint8_t status);
#define gap_dispose_set_adv_para_complete ((f_gap_dispose_set_adv_para_complete)(0x00012979))

// uint8_t gap_discovery_proc_init(gap_pro_state_t discover_type,uint8_t app_id);
typedef uint8_t  (*f_gap_discovery_proc_init)(gap_pro_state_t discover_type,uint8_t app_id);
#define gap_discovery_proc_init ((f_gap_discovery_proc_init)(0x0001292d))

// uint8_t gap_general_connection_proc_init(uint8_t app_id);
typedef uint8_t  (*f_gap_general_connection_proc_init)(uint8_t app_id);
#define gap_general_connection_proc_init ((f_gap_general_connection_proc_init)(0x00012a35))

// uint8_t gap_auto_connection_proc_init(uint8_t app_id,bd_addr_type_t address_type, bd_addr_t address);
typedef uint8_t  (*f_gap_auto_connection_proc_init)(uint8_t app_id,bd_addr_type_t address_type, bd_addr_t address);
#define gap_auto_connection_proc_init ((f_gap_auto_connection_proc_init)(0x000125f5))

// void fill_advertising_report_from_packet(advertising_report_t * report, const uint8_t *packet);
typedef void  (*f_fill_advertising_report_from_packet)(advertising_report_t * report, const uint8_t *packet);
#define fill_advertising_report_from_packet ((f_fill_advertising_report_from_packet)(0x000115f5))

// void fill_ext_advertising_report_from_packet(ext_advertising_report_t * report, const uint8_t *packet);
typedef void  (*f_fill_ext_advertising_report_from_packet)(ext_advertising_report_t * report, const uint8_t *packet);
#define fill_ext_advertising_report_from_packet ((f_fill_ext_advertising_report_from_packet)(0x00011631))

// void fill_period_report_from_packet(prd_advertising_report_t * report, const uint8_t *packet);
typedef void  (*f_fill_period_report_from_packet)(prd_advertising_report_t * report, const uint8_t *packet);
#define fill_period_report_from_packet ((f_fill_period_report_from_packet)(0x0001169d))

// void gap_get_advertisingReport(advertising_report_t * report, const uint8_t *packet);
typedef void  (*f_gap_get_advertisingReport)(advertising_report_t * report, const uint8_t *packet);
#define gap_get_advertisingReport ((f_gap_get_advertisingReport)(0x00012a61))

// uint8_t gap_observe_proc_init(uint8_t app_id);
typedef uint8_t  (*f_gap_observe_proc_init)(uint8_t app_id);
#define gap_observe_proc_init ((f_gap_observe_proc_init)(0x00012c69))


/**
 * @brief Set parameters for LE Scan
 */
// void gap_set_scan_parameters(uint8_t scan_type, uint16_t scan_interval, uint16_t scan_window);
typedef void  (*f_gap_set_scan_parameters)(uint8_t scan_type, uint16_t scan_interval, uint16_t scan_window);
#define gap_set_scan_parameters ((f_gap_set_scan_parameters)(0x00013069))


/**
 * @brief Start LE Scan 
 */
// void gap_start_scan(void);
typedef void  (*f_gap_start_scan)(void);
#define gap_start_scan ((f_gap_start_scan)(0x00013125))


/**
 * @brief Stop LE Scan
 */
// void gap_stop_scan(void);
typedef void  (*f_gap_stop_scan)(void);
#define gap_stop_scan ((f_gap_stop_scan)(0x000131c1))

// Classic | LE

// void gap_set_random_device_address(bd_addr_t address);
typedef void  (*f_gap_set_random_device_address)(bd_addr_t address);
#define gap_set_random_device_address ((f_gap_set_random_device_address)(0x0001305d))



/**
 * @brief Enable privacy by using random addresses
 * @param random_address_type to use (incl. OFF)
 */
// void gap_random_address_set_mode(gap_random_address_type_t random_address_type);
typedef void  (*f_gap_random_address_set_mode)(gap_random_address_type_t random_address_type);
#define gap_random_address_set_mode ((f_gap_random_address_set_mode)(0x00012da9))


/** 
 * @brief Sets a fixed random address for advertising
 * @param addr
 * @note Sets random address mode to type off
 */
// void gap_random_address_set(bd_addr_t addr);
typedef void  (*f_gap_random_address_set)(bd_addr_t addr);
#define gap_random_address_set ((f_gap_random_address_set)(0x00012d81))

/**
 * @brief Get privacy mode
 */
// gap_random_address_type_t gap_random_address_get_mode(void);
typedef gap_random_address_type_t  (*f_gap_random_address_get_mode)(void);
#define gap_random_address_get_mode ((f_gap_random_address_get_mode)(0x00012d75))



/**
 * @brief Auto Connection Establishment - Start Connecting to device
 * @param address_typ
 * @param address
 * @returns 0 if ok
 */
// int gap_auto_connection_start(bd_addr_type_t address_typ, bd_addr_t address);
typedef int  (*f_gap_auto_connection_start)(bd_addr_type_t address_typ, bd_addr_t address);
#define gap_auto_connection_start ((f_gap_auto_connection_start)(0x00012621))

// uint8_t gap_auto_connect(void);
typedef uint8_t  (*f_gap_auto_connect)(void);
#define gap_auto_connect ((f_gap_auto_connect)(0x000125ad))


/**
 * @brief Auto Connection Establishment - Stop Connecting to device
 * @param address_typ
 * @param address
 * @returns 0 if ok
 */
// int gap_auto_connection_stop(bd_addr_type_t address_typ, bd_addr_t address);
typedef int  (*f_gap_auto_connection_stop)(bd_addr_type_t address_typ, bd_addr_t address);
#define gap_auto_connection_stop ((f_gap_auto_connection_stop)(0x00012671))


/**
 * @brief Auto Connection Establishment - Stop everything
 * @note  Convenience function to stop all active auto connection attempts
 */
// void gap_auto_connection_stop_all(void);
typedef void  (*f_gap_auto_connection_stop_all)(void);
#define gap_auto_connection_stop_all ((f_gap_auto_connection_stop_all)(0x00012681))



/**
 * @brief Connect to remote LE device
 */
// uint8_t gap_connect(bd_addr_t addr, bd_addr_type_t addr_type);
typedef uint8_t  (*f_gap_connect)(bd_addr_t addr, bd_addr_type_t addr_type);
#define gap_connect ((f_gap_connect)(0x0001272d))



/**
 * @brief Disconnect connection with handle
 * @param handle
 */
// uint8_t gap_disconnect(hci_con_handle_t handle);
typedef uint8_t  (*f_gap_disconnect)(hci_con_handle_t handle);
#define gap_disconnect ((f_gap_disconnect)(0x000128a5))


/**
 * @brief Get connection type
 * @param con_handle
 * @result connection_type
 */
// gap_connection_type_t gap_get_connection_type(hci_con_handle_t connection_handle);
typedef gap_connection_type_t  (*f_gap_get_connection_type)(hci_con_handle_t connection_handle);
#define gap_get_connection_type ((f_gap_get_connection_type)(0x00012ae9))



/**
 * @brief Set Advertisement Data
 * @param advertising_data_length
 * @param advertising_data (max 31 octets)
 * @note data is not copied, pointer has to stay valid
 */
// void gap_advertisements_set_data(uint8_t advertising_data_length, uint8_t * advertising_data);
typedef void  (*f_gap_advertisements_set_data)(uint8_t advertising_data_length, uint8_t * advertising_data);
#define gap_advertisements_set_data ((f_gap_advertisements_set_data)(0x00012555))


/**
 * @brief Set Advertisement Paramters
 * @param adv_int_min
 * @param adv_int_max
 * @param adv_type
 * @param direct_address_type
 * @param direct_address
 * @param channel_map
 * @param filter_policy
 * @note own_address_type is used from gap_random_address_set_mode
 */
// void gap_advertisements_set_params(uint16_t adv_int_min, uint16_t adv_int_max, uint8_t adv_type,
// 	uint8_t direct_address_typ, bd_addr_t direct_address, uint8_t channel_map, uint8_t filter_policy);
typedef void  (*f_gap_advertisements_set_params)(uint16_t adv_int_min, uint16_t adv_int_max, uint8_t adv_type,
	uint8_t direct_address_typ, bd_addr_t direct_address, uint8_t channel_map, uint8_t filter_policy);
#define gap_advertisements_set_params ((f_gap_advertisements_set_params)(0x00012571))


/** 
 * @brief Enable/Disable Advertisements
 * @param enabled
 */
// void gap_advertisements_enable(int enabled);
typedef void  (*f_gap_advertisements_enable)(int enabled);
#define gap_advertisements_enable ((f_gap_advertisements_enable)(0x000124f9))


/** 
 * @brief Set Scan Response Data
 *
 * @note For scan response data, scannable undirected advertising (ADV_SCAN_IND) need to be used
 *
 * @param advertising_data_length
 * @param advertising_data (max 31 octets)
 * @note data is not copied, pointer has to stay valid
 */
// void gap_scan_response_set_data(uint8_t scan_response_data_length, uint8_t * scan_response_data);
typedef void  (*f_gap_scan_response_set_data)(uint8_t scan_response_data_length, uint8_t * scan_response_data);
#define gap_scan_response_set_data ((f_gap_scan_response_set_data)(0x00012f95))

// void ble_gap_set_whitelist(bd_addr_t address,bd_addr_type_t  addtype);
typedef void  (*f_ble_gap_set_whitelist)(bd_addr_t address,bd_addr_type_t  addtype);
#define ble_gap_set_whitelist ((f_ble_gap_set_whitelist)(0x00008939))

// void ble_gap_remove_whitelist(bd_addr_t address,bd_addr_type_t addtype);
typedef void  (*f_ble_gap_remove_whitelist)(bd_addr_t address,bd_addr_type_t addtype);
#define ble_gap_remove_whitelist ((f_ble_gap_remove_whitelist)(0x00008925))


// app_timer_source_t* btstack_memory_dtbt_app_timer_get(void);
typedef app_timer_source_t*  (*f_btstack_memory_dtbt_app_timer_get)(void);
// WARNING: this API is not available in this release
#define btstack_memory_dtbt_app_timer_get ((f_btstack_memory_dtbt_app_timer_get)(btstack_memory_dtbt_app_timer_get))

// void btstack_memory_dtbt_app_timer_free(app_timer_source_t *dtbt_app_timer);
typedef void  (*f_btstack_memory_dtbt_app_timer_free)(app_timer_source_t *dtbt_app_timer);
#define btstack_memory_dtbt_app_timer_free ((f_btstack_memory_dtbt_app_timer_free)(0x0000e67d))

// uint8_t app_funregister(APP_TimerExpFun funaddr);
typedef uint8_t  (*f_app_funregister)(APP_TimerExpFun funaddr);
#define app_funregister ((f_app_funregister)(0x00006995))


//for customer timer API.
// void app_OS_Timer(TimerHandle_t xTimer);
typedef void  (*f_app_OS_Timer)(TimerHandle_t xTimer);
#define app_OS_Timer ((f_app_OS_Timer)(0x000068b5))

// void app_set_timer(app_timer_source_t *a,uint32_t timerid, uint32_t timeout_in_ms,uint8_t opt);
typedef void  (*f_app_set_timer)(app_timer_source_t *a,uint32_t timerid, uint32_t timeout_in_ms,uint8_t opt);
#define app_set_timer ((f_app_set_timer)(0x00007281))

// void app_timer_delete(app_timer_source_t *a);
typedef void  (*f_app_timer_delete)(app_timer_source_t *a);
#define app_timer_delete ((f_app_timer_delete)(0x000072dd))

// void app_add_timer(app_timer_source_t *ts);
typedef void  (*f_app_add_timer)(app_timer_source_t *ts);
#define app_add_timer ((f_app_add_timer)(0x00006979))

// void reverse_bd_addr(const bd_addr_t src, bd_addr_t dest);
typedef void  (*f_reverse_bd_addr)(const bd_addr_t src, bd_addr_t dest);
#define reverse_bd_addr ((f_reverse_bd_addr)(0x0003387b))

#if (BLE50_ENABLED)
// uint8_t gap_LE_read_phy(uint16_t connecton_handler);
typedef uint8_t  (*f_gap_LE_read_phy)(uint16_t connecton_handler);
// WARNING: this API is not available in this release
#define gap_LE_read_phy ((f_gap_LE_read_phy)(gap_LE_read_phy))

// uint8_t gap_LE_set_def_phy(uint8_t ALL_PHYS,uint8_t TX_PHYS ,uint8_t RX_PHYS);
typedef uint8_t  (*f_gap_LE_set_def_phy)(uint8_t ALL_PHYS,uint8_t TX_PHYS ,uint8_t RX_PHYS);
// WARNING: this API is not available in this release
#define gap_LE_set_def_phy ((f_gap_LE_set_def_phy)(gap_LE_set_def_phy))

// uint8_t gap_LE_set_phy(uint16_t con_handle,uint8_t all_phys,uint8_t tx_phys,uint8_t rx_phys,uint8_t phy_opt);
typedef uint8_t  (*f_gap_LE_set_phy)(uint16_t con_handle,uint8_t all_phys,uint8_t tx_phys,uint8_t rx_phys,uint8_t phy_opt);
// WARNING: this API is not available in this release
#define gap_LE_set_phy ((f_gap_LE_set_phy)(gap_LE_set_phy))

// uint8_t gap_LE_Advertising_Set_Random_Address(uint8_t con_handle,bd_addr_t random_addr);
typedef uint8_t  (*f_gap_LE_Advertising_Set_Random_Address)(uint8_t con_handle,bd_addr_t random_addr);
// WARNING: this API is not available in this release
#define gap_LE_Advertising_Set_Random_Address ((f_gap_LE_Advertising_Set_Random_Address)(gap_LE_Advertising_Set_Random_Address))

// uint8_t gap_LE_set_ext_scan_para(bd_addr_type_t,uint8_t filter,phy_bittype_t phys,scan_type_t scan_type, uint16_t scan_interval, uint16_t scan_window,...);
typedef uint8_t  (*f_gap_LE_set_ext_scan_para)(bd_addr_type_t,uint8_t filter,phy_bittype_t phys,scan_type_t scan_type, uint16_t scan_interval, uint16_t scan_window,...);
// WARNING: this API is not available in this release
#define gap_LE_set_ext_scan_para ((f_gap_LE_set_ext_scan_para)(gap_LE_set_ext_scan_para))

// uint8_t gap_LE_ext_scan_response_set_data(uint8_t ad_handle,uint16_t scan_response_data_length, uint8_t * scan_response_data);
typedef uint8_t  (*f_gap_LE_ext_scan_response_set_data)(uint8_t ad_handle,uint16_t scan_response_data_length, uint8_t * scan_response_data);
// WARNING: this API is not available in this release
#define gap_LE_ext_scan_response_set_data ((f_gap_LE_ext_scan_response_set_data)(gap_LE_ext_scan_response_set_data))

// uint8_t gap_LE_set_ext_scan_enable(uint8_t enable,uint8_t filter,uint16_t duration, uint16_t period );
typedef uint8_t  (*f_gap_LE_set_ext_scan_enable)(uint8_t enable,uint8_t filter,uint16_t duration, uint16_t period );
// WARNING: this API is not available in this release
#define gap_LE_set_ext_scan_enable ((f_gap_LE_set_ext_scan_enable)(gap_LE_set_ext_scan_enable))

// uint8_t gap_LE_set_ext_adv_enable(uint8_t enable,uint8_t numberofset,...);
typedef uint8_t  (*f_gap_LE_set_ext_adv_enable)(uint8_t enable,uint8_t numberofset,...);
// WARNING: this API is not available in this release
#define gap_LE_set_ext_adv_enable ((f_gap_LE_set_ext_adv_enable)(gap_LE_set_ext_adv_enable))

// uint8_t gap_LE_set_ext_adv_para(uint8_t,Advertising_Event_Properties_t Advertising_Event_Properties,uint32_t Primary_Advertising_Interval_Min,uint32_t Primary_Advertising_Interval_Max,uint8_t Primary_Advertising_Channel_Map,
// 	bd_addr_type_t Own_Address_Type,bd_addr_type_t Peer_Address_Type, bd_addr_t Peer_Address,uint8_t Advertising_Filter_Policy, int8_t Advertising_Tx_Power,phy_type_t Primary_Advertising_PHY,
// 	uint8_t Secondary_Advertising_Max_Skip, phy_type_t Secondary_Advertising_PHY,uint8_t Advertising_SID,uint8_t Scan_Request_Notification_Enable);
typedef uint8_t  (*f_gap_LE_set_ext_adv_para)(uint8_t,Advertising_Event_Properties_t Advertising_Event_Properties,uint32_t Primary_Advertising_Interval_Min,uint32_t Primary_Advertising_Interval_Max,uint8_t Primary_Advertising_Channel_Map,
	bd_addr_type_t Own_Address_Type,bd_addr_type_t Peer_Address_Type, bd_addr_t Peer_Address,uint8_t Advertising_Filter_Policy, int8_t Advertising_Tx_Power,phy_type_t Primary_Advertising_PHY,
	uint8_t Secondary_Advertising_Max_Skip, phy_type_t Secondary_Advertising_PHY,uint8_t Advertising_SID,uint8_t Scan_Request_Notification_Enable);
// WARNING: this API is not available in this release
#define gap_LE_set_ext_adv_para ((f_gap_LE_set_ext_adv_para)(gap_LE_set_ext_adv_para))

// uint8_t gap_LE_set_ext_adv_data(uint8_t Advertising_Handle,uint16_t Data_Length,uint8_t* data);
typedef uint8_t  (*f_gap_LE_set_ext_adv_data)(uint8_t Advertising_Handle,uint16_t Data_Length,uint8_t* data);
// WARNING: this API is not available in this release
#define gap_LE_set_ext_adv_data ((f_gap_LE_set_ext_adv_data)(gap_LE_set_ext_adv_data))

// uint8_t gap_LE_set_period_adv_data(uint8_t Advertising_Handle,uint16_t data_len,uint8_t* data);
typedef uint8_t  (*f_gap_LE_set_period_adv_data)(uint8_t Advertising_Handle,uint16_t data_len,uint8_t* data);
// WARNING: this API is not available in this release
#define gap_LE_set_period_adv_data ((f_gap_LE_set_period_adv_data)(gap_LE_set_period_adv_data))

// uint8_t gap_LE_set_period_adv_enable(uint8_t enable,uint8_t adv_handle);   //return handler
typedef uint8_t  (*f_gap_LE_set_period_adv_enable)(uint8_t enable,uint8_t adv_handle);   //return handler
// WARNING: this API is not available in this release
#define gap_LE_set_period_adv_enable ((f_gap_LE_set_period_adv_enable)(gap_LE_set_period_adv_enable))

// uint8_t gap_LE_set_period_adv_para(uint8_t adv_handle,uint8_t Periodic_Advertising_Interval_Min,uint8_t Periodic_Advertising_Interval_Max,Periodic_Advertising_Properties_t Periodic_Advertising_Properties);
typedef uint8_t  (*f_gap_LE_set_period_adv_para)(uint8_t adv_handle,uint8_t Periodic_Advertising_Interval_Min,uint8_t Periodic_Advertising_Interval_Max,Periodic_Advertising_Properties_t Periodic_Advertising_Properties);
// WARNING: this API is not available in this release
#define gap_LE_set_period_adv_para ((f_gap_LE_set_period_adv_para)(gap_LE_set_period_adv_para))

// uint8_t gap_LE_clr_advset(void);
typedef uint8_t  (*f_gap_LE_clr_advset)(void);
// WARNING: this API is not available in this release
#define gap_LE_clr_advset ((f_gap_LE_clr_advset)(gap_LE_clr_advset))

// uint8_t gap_LE_remove_advset(uint8_t adv_handle);
typedef uint8_t  (*f_gap_LE_remove_advset)(uint8_t adv_handle);
// WARNING: this API is not available in this release
#define gap_LE_remove_advset ((f_gap_LE_remove_advset)(gap_LE_remove_advset))

// uint8_t gap_LE_read_max_adv_data_len(void);
typedef uint8_t  (*f_gap_LE_read_max_adv_data_len)(void);
// WARNING: this API is not available in this release
#define gap_LE_read_max_adv_data_len ((f_gap_LE_read_max_adv_data_len)(gap_LE_read_max_adv_data_len))

// uint8_t gap_LE_period_adv_create_sync(uint8_t Filter_Policy,uint8_t Advertising_SID,uint8_t Advertiser_Address_Type,bd_addr_t Advertiser_Address,uint16_t Skip,uint16_t Sync_Timeout,uint8_t reseved);
typedef uint8_t  (*f_gap_LE_period_adv_create_sync)(uint8_t Filter_Policy,uint8_t Advertising_SID,uint8_t Advertiser_Address_Type,bd_addr_t Advertiser_Address,uint16_t Skip,uint16_t Sync_Timeout,uint8_t reseved);
// WARNING: this API is not available in this release
#define gap_LE_period_adv_create_sync ((f_gap_LE_period_adv_create_sync)(gap_LE_period_adv_create_sync))

// uint8_t gap_LE_period_adv_create_sync_cancel(void);
typedef uint8_t  (*f_gap_LE_period_adv_create_sync_cancel)(void);
// WARNING: this API is not available in this release
#define gap_LE_period_adv_create_sync_cancel ((f_gap_LE_period_adv_create_sync_cancel)(gap_LE_period_adv_create_sync_cancel))

// uint8_t gap_LE_period_adv_term_sync(uint16_t sync_handle);
typedef uint8_t  (*f_gap_LE_period_adv_term_sync)(uint16_t sync_handle);
// WARNING: this API is not available in this release
#define gap_LE_period_adv_term_sync ((f_gap_LE_period_adv_term_sync)(gap_LE_period_adv_term_sync))

// uint8_t gap_LE_ADD_DEV_TO_PERIOD_list(uint8_t address_type,bd_addr_t address,uint8_t SID);
typedef uint8_t  (*f_gap_LE_ADD_DEV_TO_PERIOD_list)(uint8_t address_type,bd_addr_t address,uint8_t SID);
// WARNING: this API is not available in this release
#define gap_LE_ADD_DEV_TO_PERIOD_list ((f_gap_LE_ADD_DEV_TO_PERIOD_list)(gap_LE_ADD_DEV_TO_PERIOD_list))

// uint8_t gap_LE_clr_period_adv_list(void);
typedef uint8_t  (*f_gap_LE_clr_period_adv_list)(void);
// WARNING: this API is not available in this release
#define gap_LE_clr_period_adv_list ((f_gap_LE_clr_period_adv_list)(gap_LE_clr_period_adv_list))

// uint8_t gap_LE_read_period_adv_list_size(void);
typedef uint8_t  (*f_gap_LE_read_period_adv_list_size)(void);
// WARNING: this API is not available in this release
#define gap_LE_read_period_adv_list_size ((f_gap_LE_read_period_adv_list_size)(gap_LE_read_period_adv_list_size))

// uint8_t gap_LE_ext_create_connection(uint8_t Initiator_Filter_Policy,bd_addr_type_t Own_Address_Type,bd_addr_type_t Peer_Address_Type,bd_addr_t Peer_Address,phy_bittype_t Initiating_PHYs,
//         /*uint16_t scan_int,uint16_t scan_win,uint16_t conn_int_min,uint16_t conn_int_max,uint16_t conn_latency, uint16_t supp_tmo, uint16_t min_ce_len, uint16_t max_ce_len,*/...);
typedef uint8_t  (*f_gap_LE_ext_create_connection)(uint8_t Initiator_Filter_Policy,bd_addr_type_t Own_Address_Type,bd_addr_type_t Peer_Address_Type,bd_addr_t Peer_Address,phy_bittype_t Initiating_PHYs,
        /*uint16_t scan_int,uint16_t scan_win,uint16_t conn_int_min,uint16_t conn_int_max,uint16_t conn_latency, uint16_t supp_tmo, uint16_t min_ce_len, uint16_t max_ce_len,*/...);
// WARNING: this API is not available in this release
#define gap_LE_ext_create_connection ((f_gap_LE_ext_create_connection)(gap_LE_ext_create_connection))

// uint8_t gap_LE_set_host_channel_classification(uint32_t channel_low, uint8_t channel_high);
typedef uint8_t  (*f_gap_LE_set_host_channel_classification)(uint32_t channel_low, uint8_t channel_high);
// WARNING: this API is not available in this release
#define gap_LE_set_host_channel_classification ((f_gap_LE_set_host_channel_classification)(gap_LE_set_host_channel_classification))


#endif


/**
 * @brief Sets update period for random address
 * @param period_ms in ms
 */
//  void gap_random_address_set_update_period(int period_ms);
typedef  void  (*f_gap_random_address_set_update_period)(int period_ms);
#define gap_random_address_set_update_period ((f_gap_random_address_set_update_period)(0x00012dd1))



/**
 * @brief Request an update of the connection parameter for a given LE connection
 * @param handle
 * @param conn_interval_min (unit: 1.25ms)
 * @param conn_interval_max (unit: 1.25ms)
 * @param conn_latency
 * @param supervision_timeout (unit: 10ms)
 * @returns 0 if ok
 */
// int gap_request_connection_parameter_update(hci_con_handle_t con_handle, uint16_t conn_interval_min,
// 	uint16_t conn_interval_max, uint16_t conn_latency, uint16_t supervision_timeout);
typedef int  (*f_gap_request_connection_parameter_update)(hci_con_handle_t con_handle, uint16_t conn_interval_min,
	uint16_t conn_interval_max, uint16_t conn_latency, uint16_t supervision_timeout);
// WARNING: this API is not available in this release
#define gap_request_connection_parameter_update ((f_gap_request_connection_parameter_update)(gap_request_connection_parameter_update))


/**
 * @brief Updates the connection parameters for a given LE connection
 * @param handle
 * @param conn_interval_min (unit: 1.25ms)
 * @param conn_interval_max (unit: 1.25ms)
 * @param conn_latency
 * @param supervision_timeout (unit: 10ms)
 * @returns 0 if ok
 */
// int gap_update_connection_parameters(hci_con_handle_t con_handle, uint16_t conn_interval_min,
// 	uint16_t conn_interval_max, uint16_t conn_latency, uint16_t supervision_timeout);
typedef int  (*f_gap_update_connection_parameters)(hci_con_handle_t con_handle, uint16_t conn_interval_min,
	uint16_t conn_interval_max, uint16_t conn_latency, uint16_t supervision_timeout);
#define gap_update_connection_parameters ((f_gap_update_connection_parameters)(0x000131dd))


/**
 * @brief Set accepted connection parameter range
 * @param range
 */
// void gap_get_connection_parameter_range(le_connection_parameter_range_t * range);
typedef void  (*f_gap_get_connection_parameter_range)(le_connection_parameter_range_t * range);
#define gap_get_connection_parameter_range ((f_gap_get_connection_parameter_range)(0x00012acd))


/**
 * @brief Get accepted connection parameter range
 * @param range
 */
// void gap_set_connection_parameter_range(le_connection_parameter_range_t * range);
typedef void  (*f_gap_set_connection_parameter_range)(le_connection_parameter_range_t * range);
#define gap_set_connection_parameter_range ((f_gap_set_connection_parameter_range)(0x00013025))



//HCI func interface

/**
 * @brief Add event packet handler. 
 */
// void hci_add_event_handler(btstack_packet_callback_registration_t * callback_handler);
typedef void  (*f_hci_add_event_handler)(btstack_packet_callback_registration_t * callback_handler);
#define hci_add_event_handler ((f_hci_add_event_handler)(0x00017b5d))


/**
 * @brief Registers a packet handler for ACL data. Used by L2CAP
 */
// void hci_register_acl_packet_handler(btstack_packet_handler_t handler);
typedef void  (*f_hci_register_acl_packet_handler)(btstack_packet_handler_t handler);
#define hci_register_acl_packet_handler ((f_hci_register_acl_packet_handler)(0x00018c75))


/**
 * @brief Requests the change of BTstack power mode.
 */
// int  hci_power_control(HCI_POWER_MODE mode);
typedef int   (*f_hci_power_control)(HCI_POWER_MODE mode);
#define hci_power_control ((f_hci_power_control)(0x00018acd))


#if defined __cplusplus
}
#endif

#endif
