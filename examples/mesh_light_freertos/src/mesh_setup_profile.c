
/*  ING CHIPS MESH */

/*
 * Copyright (c) 2018 INGCHIPS MESH
 */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "gap.h"
#include "bt_types.h"
#include "btstack_event.h"
#include "errno.h"
#include "stdbool.h"
#include "platform_api.h"
#include "att_db.h"

#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "mesh_port_pb.h"
#include "btstack_port_mesh_init.h"
#include "mesh.h"
#include "mesh_proxy.h"
#include "btstack_debug.h"
#include "mesh_port_stack.h"
#include "mesh_setup_profile.h"




const uint8_t gatt_data_pb[] =
{
    #include "../data/gatt_pb.profile"
};

const uint8_t gatt_data_proxy[] =
{
    #include "../data/gatt_proxy.profile"
};

#include "../data/gatt_pb.const"
#include "../data/gatt_proxy.const"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define dbg_printf(...)         printf(__VA_ARGS__);printf("\n")
#define dbg_printf_hex(...)     printf(__VA_ARGS__)
//#define log_debug(...)          printf(__VA_ARGS__);printf("\n")


const static uint8_t prov_adv_data[] = {
    // 0x01 - «Flags»
    2, 0x01,
    0x06,

    // 0x03 - «Complete List of 16-bit Service Class UUIDs»
    3, 0x03,
    0x27, 0x18,

    // 0x16 - «Service Data - 16-bit UUID»
    21, 0x16,
    0x27, 0x18, 0x00, 0x01, 0x61, 0x00, 0x04, 0x20,
    0x30, 0x75, 0x79, 0xBC, 0xDE, 0xEF, 0xB6, 0xDB,
    0x6D, 0xB6, 0x00, 0x00
};

const static uint8_t proxy_adv_data[] = {
    // 0x01 - «Flags»
    2, 0x01,
    0x06,

    // 0x03 - «Complete List of 16-bit Service Class UUIDs»
    3, 0x03,
    0x27, 0x18,

    // 0x16 - «Service Data - 16-bit UUID»
    21, 0x16,
    0x27, 0x18, 0x00, 0x01, 0x61, 0x00, 0x04, 0x20,
    0x30, 0x75, 0x79, 0xBC, 0xDE, 0xEF, 0xB6, 0xDB,
    0x6D, 0xB6, 0x00, 0x00
};

// hello-mesh
const static uint8_t scan_data[] = {
    // 0x09 - «Complete Local Name»
    0x0B, 0x09,
    0x68, 0x65, 0x6C, 0x6C, 0x6F, 0x2D, 0x6D, 0x65, 0x73, 0x68
};

static hci_con_handle_t my_conn_handle = CON_HANDLE_INVALID;
static uint16_t conn_interval_ms = 0;



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

#define MAX_ADV_HANDLE  2
typedef struct adv_en
{
    uint8_t en;
    uint8_t setsnum;
    ext_adv_set_en_t adv_set_conf[MAX_ADV_HANDLE];
}adv_en_t;

typedef struct adv_data
{
    uint8_t adv_handle;
    uint8_t len;
    uint8_t *data;
}adv_data_t;

typedef struct scan_rsp_data
{
    uint8_t adv_handle;
    uint8_t len;
    uint8_t *data;
}scan_rsp_data_t;

/////////////////////////////////////////////////////////////////
/******************************************************************************************
 * @brief App set adv random address, push struct to host msg queue.
 * @example 
    rand_addr_t ranaddr ={.adv_handle = advhandle};
    memcpy(ranaddr.addr, randaddr, sizeof(bd_addr_t));
    ble_gap_set_adv_set_random_addr(&ranaddr);
 */
uint32_t ble_gap_set_adv_set_random_addr(rand_addr_t * rand_addr)
{
    //if(is_curr_host_task()){
        return (uint32_t)gap_set_adv_set_random_addr(rand_addr->adv_handle, rand_addr->addr);
    //}
}

/******************************************************************************************
 * @brief App set adv data req, push struct to host msg queue.
 * @note Data area are not alloc, so input adv data should use a galobal buffer.
 * @example 
    adv_data_t adv_data ={.adv_handle = handle, .len = len, .data = data};
    ble_gap_set_ext_adv_data(&adv_data);
 */
uint32_t ble_gap_set_ext_adv_data(adv_data_t * adv_data)
{
    //if(is_curr_host_task()){
        return (uint32_t)gap_set_ext_adv_data(adv_data->adv_handle, adv_data->len, adv_data->data);
    //}
}


/******************************************************************************************
 * @brief App set adv enable/disable, push struct to host msg queue.
 * @example 
    ext_adv_set_en_t set_conf;
    set_conf.duration = duration;
    set_conf.max_events = max_events;
    set_conf.handle = PB_ADV_HANDLE;
    adv_en_t adven ={.en=1,.setsnum =1};
    memcpy(&adven.adv_set_conf,&set_conf,sizeof(set_conf));
    ble_gap_set_ext_adv_enable(&adven);
 */
uint32_t ble_gap_set_ext_adv_enable(adv_en_t * adv_en)
{
    //if(is_curr_host_task()){
        return (uint32_t)gap_set_ext_adv_enable(adv_en->en, adv_en->setsnum, adv_en->adv_set_conf);
    //}
}

/******************************************************************************************
 * @brief Set adv params request, you can invoke it anywhere.
 * @example 
    adv_param_t advParam = {.adv_handle = advhandle,
                            .prop = (adv_event_properties_t)properties,
                            .inter_min = params->itvl_min,
                            .inter_max = params->itvl_max,
                            .map = params->channel_map,
                            .addr_type = (bd_addr_type_t)params->own_addr_type,
                            .peer_addr_type = (bd_addr_type_t)params->peer.type,
                            .filter = (adv_filter_policy_t) params->filter_policy,
                            .tx_power = params->tx_power,
                            .adv_phy = (phy_type_t)params->primary_phy,
                            .max_skip = 0,
                            .sec_phy = (phy_type_t)params->secondary_phy,
                            .sid = params->sid,
                            .notify = params->scan_req_notif};
    memcpy(advParam.peer_addr,params->peer.val,sizeof(bd_addr_t));
    ble_gap_set_ext_adv_para(&advParam);
 */
uint32_t ble_gap_set_ext_adv_para(adv_param_t * adv_param)
{
    //if(is_curr_host_task()){
        return (uint32_t)gap_set_ext_adv_para(  adv_param->adv_handle,
                                                adv_param->prop,
                                                adv_param->inter_min,
                                                adv_param->inter_max,
                                                adv_param->map, 
                                                adv_param->addr_type,
                                                adv_param->peer_addr_type,
                                                adv_param->peer_addr,
                                                adv_param->filter,
                                                adv_param->tx_power,
                                                adv_param->adv_phy,
                                                adv_param->max_skip,
                                                adv_param->sec_phy,
                                                adv_param->sid,
                                                adv_param->notify);
    //}
}

/******************************************************************************************
 * @brief App set adv rsp data req, push struct to host msg queue.
 * @note Data area are not alloc, so input adv data should use a galobal buffer.
 * @example 
    scan_rsp_data_t rsp_data ={.adv_handle = handle, .len =len, .data = data};
    ble_gap_set_ext_scan_response_data(&rsp_data);
 */
uint32_t ble_gap_set_ext_scan_response_data(scan_rsp_data_t * rsp)
{
    //if(is_curr_host_task()){
        return (uint32_t)gap_set_ext_scan_response_data(rsp->adv_handle, rsp->len, rsp->data);
    //}
}

/////////////////////////////////////////////////////////////////

static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset,
                                  uint8_t * buffer, uint16_t buffer_size)
{
    UNUSED(offset);
    return mesh_att_read_callback(connection_handle, att_handle, buffer, buffer_size);
}

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode,
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    UNUSED(transaction_mode);
    UNUSED(offset);
    return mesh_att_write_callback(connection_handle, att_handle, buffer, buffer_size);
}

#if 1 // -----------------------------------------------------------------------------------------------------

#define CPI_VAL_TO_MS(x)    ((uint16_t)(x * 5 / 4))
#define CPI_MS_TO_VAL(x)    ((uint16_t)(x * 4 / 5))

#define CPSTT_VAL_TO_MS(x)  ((uint16_t)(x * 10))
#define CPSTT_MS_TO_VAL(x)  ((uint16_t)(x / 10))

static void conn_params_print(const le_meta_event_enh_create_conn_complete_t *conn){
    printf("my_conn_handle:%d\n", conn->handle);
    printf("conn_interval:%dms\n", CPI_VAL_TO_MS(conn->interval));
    printf("conn_timeout:%dms\n", CPSTT_VAL_TO_MS(conn->sup_timeout));
}

void app_update_conn_params_req(void){
    if(my_conn_handle == CON_HANDLE_INVALID){
        printf("conn handle err.\n");
        return ;
    }

    static uint8_t flag = 0;
    if(flag == 0){
        printf("conn interval: %d\n", CPI_VAL_TO_MS(CPI_MS_TO_VAL(200)));
        gap_update_connection_parameters(   my_conn_handle, CPI_MS_TO_VAL(200), CPI_MS_TO_VAL(200), 
                                            0, CPSTT_MS_TO_VAL(5000), 0x0000, 8);//ce_len = unit 0.625ms.
        flag ++;
    } else if (flag == 1) {
        printf("conn interval: %d\n", CPI_VAL_TO_MS(CPI_MS_TO_VAL(48)));
        gap_update_connection_parameters(   my_conn_handle, CPI_MS_TO_VAL(48), CPI_MS_TO_VAL(48), 
                                            0, CPSTT_MS_TO_VAL(5000), 0x0000, 8);//ce_len = unit 0.625ms.
        flag ++;
    } else if (flag == 2) {
        printf("conn interval: %d\n", CPI_VAL_TO_MS(CPI_MS_TO_VAL(30)));
        gap_update_connection_parameters(   my_conn_handle, CPI_MS_TO_VAL(30), CPI_MS_TO_VAL(30), 
                                            0, CPSTT_MS_TO_VAL(5000), 0x0000, 8);//ce_len = unit 0.625ms.
        flag = 0;
    }
}

uint16_t ble_get_curr_conn_interval_ms(void){
    return conn_interval_ms;
}

void ble_set_conn_interval_ms(uint16_t interval_ms){
    gap_update_connection_parameters(   my_conn_handle, CPI_MS_TO_VAL(interval_ms), CPI_MS_TO_VAL(interval_ms), 
                                        0, CPSTT_MS_TO_VAL(5000), 0, 8);//ce_len = unit 0.625ms. 8*0.625=5ms

}

bool Is_ble_curr_conn_valid(void){
    return (my_conn_handle == CON_HANDLE_INVALID)? false:true;
}

#endif // -----------------------------------------------------------------------------------------------------

void user_msg_handler(btstack_user_msg_t * usrmsg)
{
    bool done = false;
    uint32_t cmd_id = usrmsg->msg_id;
    void *MsgData = usrmsg->data;
    // printf("ble_msg_process: cmd=%x, p=(%p)\n", cmd_id, MsgData);
    
    switch(cmd_id){
        case USER_MSG_ID_KEY_EVENT:
            {
                uint8_t key_num = (uint8_t)usrmsg->len;
                extern void key_proc_in_host_task(uint8_t num);
                key_proc_in_host_task(key_num);
            }
            break;
        default:
            {
                ;
            }
            break;
    }
    
    if(done == true && MsgData != NULL){
        //ble_memory_free(MsgData);
        // printf("ble_memory_free:(%p)\n", MsgData);
    }
}

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    uint8_t event = hci_event_packet_get_type(packet);
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;
        dbg_printf("bt init ok.\n");
        mesh_stack_ready();
        {
            extern void set_gpio_2_6_for_rf_tx_rx(void);
            set_gpio_2_6_for_rf_tx_rx();
        }
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
            case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:{
                    const le_meta_event_enh_create_conn_complete_t *create_conn =
                                        decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t);
                    my_conn_handle = create_conn->handle;
                    conn_interval_ms = CPI_VAL_TO_MS(create_conn->interval);
                    dbg_printf("connect.\n");
                    if(conn_interval_ms){
                        conn_params_print(create_conn);
                    }
                    att_set_db(my_conn_handle, ( ble_mesh_is_provisioned()?  gatt_data_proxy : gatt_data_pb ));
                    mesh_connected(my_conn_handle);
                    platform_calibrate_32k();
                    // update connection interval.
                    // if(CPI_VAL_TO_MS(create_conn->interval) < 125){ //here will fail, must reuest later when connection stable.
                    //     printf("req conn update interval:%dms\n", CPI_VAL_TO_MS(CPI_MS_TO_VAL(125)));
                    //     gap_update_connection_parameters(my_conn_handle, CPI_MS_TO_VAL(100), CPI_MS_TO_VAL(125), 
                    //                                     0, CPSTT_MS_TO_VAL(5000), 0x0000, 0xffff);
                    // }
                }
                break;
            case HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE:{
                    const le_meta_event_conn_update_complete_t *conn_update = 
                                        decode_hci_le_meta_event(packet, le_meta_event_conn_update_complete_t);
                    printf("\nconn update complete:%d\n", conn_update->status);
                    if(conn_update->status == ERROR_CODE_SUCCESS){
                        conn_interval_ms = CPI_VAL_TO_MS(conn_update->interval);
                        
                        printf("status:%d\n", conn_update->status);
                        printf("handle:%d\n", conn_update->handle);
                        printf("interval:%dms\n", CPI_VAL_TO_MS(conn_update->interval));
                        printf("sup_timeout:%dms\n", CPSTT_VAL_TO_MS(conn_update->sup_timeout));
                    }
                    mesh_conn_params_update_complete_callback(conn_update->status, conn_update->handle, conn_update->interval, conn_update->sup_timeout);
                }
			    break;
            default:
                break;
        }
        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        dbg_printf("disconnect.\n");
        dbg_printf("------------------------------------------------------------------------------------------------\r\n");
        mesh_disconnected(my_conn_handle, NULL);
        my_conn_handle = CON_HANDLE_INVALID;
        conn_interval_ms = 0;
        break;

    case ATT_EVENT_CAN_SEND_NOW:
        mesh_gatt_can_send_now(my_conn_handle);
        break;

    case BTSTACK_EVENT_USER_MSG:
        {
            btstack_user_msg_t *p_user_msg;
            p_user_msg = (btstack_user_msg_t *)hci_event_packet_get_user_msg(packet);
            user_msg_handler(p_user_msg);
        }
        break;

    default:
        break;
    }
}

// name, addr, uuid
#include "mesh_storage.h"
static void mesh_gatt_addr_generate_and_get(bd_addr_t addr){
    if(!mesh_storage_is_gatt_addr_set()){
        // generate random gatt address.
        int rand = platform_rand();
        addr[5] = rand & 0xFF;
        addr[4] = (rand >> 8) & 0xFF;
        addr[3] = (rand >> 16) & 0xFF;
        addr[2] = (rand >> 24) & 0xFF;
        // write addr to database and flash.
        mesh_storage_gatt_addr_set(addr);
    } else {
        // read addr from database.
        mesh_storage_gatt_addr_get(addr);
    }

    printf("gatt_addr: ");
    printf_hexdump(addr, sizeof(bd_addr_t));
}

static void mesh_beacon_addr_generate_and_get(bd_addr_t addr){
    if(!mesh_storage_is_beacon_addr_set()){
        // generate random beacon address.
        int rand = platform_rand();
        addr[5] = rand & 0xFF;
        addr[4] = (rand >> 8) & 0xFF;
        addr[3] = (rand >> 16) & 0xFF;
        addr[2] = (rand >> 24) & 0xFF;
        // write addr to database and flash.
        mesh_storage_beacon_addr_set(addr);
    } else {
        // read addr from database.
        mesh_storage_beacon_addr_get(addr);
    }

    printf("beacon_addr: ");
    printf_hexdump(addr, sizeof(bd_addr_t));
}

static bd_addr_t m_rand_addr   = {0xFC, 0x01, 0x02, 0x03, 0x23, 0x01};

const static ext_adv_set_en_t adv_sets_en[2] = {{.handle = MESH_PROXY_ADV_HANDLE, .duration = 0, .max_events = 0},
                                                {.handle = MESH_PB_ADV_HANDLE, .duration = 0, .max_events = 0}};
static void mesh_proxy_adv_setup(void){

    // generate and get random gatt address.
    mesh_gatt_addr_generate_and_get(m_rand_addr);

    // set addr.
    rand_addr_t ranaddr_proxy ={.adv_handle = MESH_PROXY_ADV_HANDLE};
    memcpy(ranaddr_proxy.addr, m_rand_addr, sizeof(bd_addr_t));
    ble_gap_set_adv_set_random_addr(&ranaddr_proxy);

    // set adv params.
    adv_param_t advParam_proxy = {  .inter_min = 80,
                                    .inter_max = 80,
                                    .map = PRIMARY_ADV_ALL_CHANNELS,
                                    .addr_type = (bd_addr_type_t)BD_ADDR_TYPE_LE_RANDOM,
                                    .peer_addr_type = (bd_addr_type_t)BD_ADDR_TYPE_LE_PUBLIC,                            
                                    .filter = (adv_filter_policy_t) ADV_FILTER_ALLOW_ALL,
                                    .tx_power = 100,
                                    .adv_phy = (phy_type_t)PHY_1M,
                                    .max_skip = 0,
                                    .sec_phy = (phy_type_t)PHY_1M,
                                    .sid = 0x00,
                                    .notify = 0x00};
    memset(advParam_proxy.peer_addr, 0, sizeof(bd_addr_t));
    advParam_proxy.adv_handle = MESH_PROXY_ADV_HANDLE;
    advParam_proxy.prop = CONNECTABLE_ADV_BIT | SCANNABLE_ADV_BIT | LEGACY_PDU_BIT;
    ble_gap_set_ext_adv_para(&advParam_proxy);

    // set adv data.
    adv_data_t adv_data_proxy ={.adv_handle = MESH_PROXY_ADV_HANDLE, .len = sizeof(proxy_adv_data), .data = (uint8_t *)proxy_adv_data};
    ble_gap_set_ext_adv_data(&adv_data_proxy);

    // set scan rsp data.
    scan_rsp_data_t rsp_data_proxy ={.adv_handle = MESH_PROXY_ADV_HANDLE, .len =sizeof(scan_data), .data = (uint8_t *)scan_data};
    ble_gap_set_ext_scan_response_data(&rsp_data_proxy);
}

static void mesh_pb_adv_setup(void){
    
    // generate and get random beacon address.
    mesh_beacon_addr_generate_and_get(m_rand_addr);

    // set addr.
    rand_addr_t ranaddr_pb ={.adv_handle = MESH_PB_ADV_HANDLE};
    memcpy(ranaddr_pb.addr, m_rand_addr, sizeof(bd_addr_t));
    ble_gap_set_adv_set_random_addr(&ranaddr_pb);
        
    // set adv params.
    adv_param_t advParam_pb = {.inter_min = 80,
                            .inter_max = 80,
                            .map = PRIMARY_ADV_ALL_CHANNELS,
                            .addr_type = (bd_addr_type_t)BD_ADDR_TYPE_LE_RANDOM,
                            .peer_addr_type = (bd_addr_type_t)BD_ADDR_TYPE_LE_PUBLIC,                            
                            .filter = (adv_filter_policy_t) ADV_FILTER_ALLOW_ALL,
                            .tx_power = 100,
                            .adv_phy = (phy_type_t)PHY_1M,
                            .max_skip = 0,
                            .sec_phy = (phy_type_t)PHY_1M,
                            .sid = 0x00,
                            .notify = 0x00};
    memset(advParam_pb.peer_addr, 0, sizeof(bd_addr_t));
    advParam_pb.adv_handle = MESH_PB_ADV_HANDLE;
    advParam_pb.prop = LEGACY_PDU_BIT;
    ble_gap_set_ext_adv_para(&advParam_pb);

    // set adv data.
    adv_data_t adv_data_pb ={.adv_handle = MESH_PB_ADV_HANDLE, .len = sizeof(prov_adv_data), .data = (uint8_t *)prov_adv_data};
    ble_gap_set_ext_adv_data(&adv_data_pb);
}

// reload gatt addr and beacon addr.
void ble_port_generate_addr_and_load_addr(void){
    bd_addr_t rnd_addr;
    // generate and get random gatt address.
    mesh_gatt_addr_generate_and_get(rnd_addr);

    // set addr.
    rand_addr_t ranaddr_proxy ={.adv_handle = MESH_PROXY_ADV_HANDLE};
    memcpy(ranaddr_proxy.addr, rnd_addr, sizeof(bd_addr_t));
    ble_gap_set_adv_set_random_addr(&ranaddr_proxy);

    // generate and get random beacon address.
    mesh_beacon_addr_generate_and_get(rnd_addr);

    // set addr.
    rand_addr_t ranaddr_pb ={.adv_handle = MESH_PB_ADV_HANDLE};
    memcpy(ranaddr_pb.addr, rnd_addr, sizeof(bd_addr_t));
    ble_gap_set_adv_set_random_addr(&ranaddr_pb);
}

void mesh_setup_adv(void)
{
    gap_advertisements_enable(MESH_PROXY_ADV_HANDLE, 0);
    gap_advertisements_enable(MESH_PB_ADV_HANDLE, 0);
    
    mesh_proxy_adv_setup();
    mesh_pb_adv_setup();
}

void gap_advertisements_set_params( uint8_t adv_handle, uint16_t adv_int_min, uint16_t adv_int_max, uint8_t adv_type,
                                    uint8_t direct_address_typ, bd_addr_t direct_address, uint8_t channel_map, uint8_t filter_policy){

    adv_param_t advParam = {.adv_handle = adv_handle,
                            .prop = (adv_event_properties_t)adv_type,
                            .inter_min = adv_int_min,
                            .inter_max = adv_int_max,
                            .map = channel_map,
                            .addr_type = (bd_addr_type_t)BD_ADDR_TYPE_LE_RANDOM,
                            .peer_addr_type = (bd_addr_type_t)direct_address_typ,                            
                            .filter = (adv_filter_policy_t) filter_policy,
                            .tx_power = 100,
                            .adv_phy = (phy_type_t)PHY_1M,
                            .max_skip = 0,
                            .sec_phy = (phy_type_t)PHY_1M,
                            .sid = 0x00,
                            .notify = 0x00};
    memcpy(advParam.peer_addr, direct_address, sizeof(bd_addr_t));
    ble_gap_set_ext_adv_para(&advParam);
}

void gap_advertisements_set_data(uint8_t adv_handle, uint8_t advertising_data_length, uint8_t * advertising_data){
    // gap_set_ext_adv_data(adv_handle, advertising_data_length, advertising_data);

    adv_data_t adv_data = {.adv_handle=adv_handle, .len=advertising_data_length, .data = advertising_data};
    ble_gap_set_ext_adv_data(&adv_data);
}

uint8_t gap_scan_response_set_data(const uint8_t adv_handle, const uint16_t length, const uint8_t *data)
{
    scan_rsp_data_t rsp_data ={.adv_handle = adv_handle, .len =length, .data = (uint8_t *)data};
    return ble_gap_set_ext_scan_response_data(&rsp_data);
}

void gap_advertisements_enable(uint8_t adv_handle, int enabled){

    uint8_t i;
    uint8_t set_number = sizeof(adv_sets_en) / sizeof(adv_sets_en[0]);
    for(i=0; i<set_number; i++){
        if(adv_handle == adv_sets_en[i].handle){
        //    gap_set_ext_adv_enable(enabled, 1, &adv_sets_en[i]);
            adv_en_t adv_en = {.en=enabled, .setsnum=1};
            memcpy((uint8_t *)&adv_en.adv_set_conf, (uint8_t *)&adv_sets_en[i], sizeof(ext_adv_set_en_t));
            ble_gap_set_ext_adv_enable(&adv_en);
        }
    }
}

#if 1
// If the interval and window are equal, the duty cycle is 100 percent.
// scan_duty_cycle = scan_window/scan_interval.
// The scan_interval can not be smaller than the scan_window.
#define SCAN_CONFIG_SCAN_INTERVAl_MS  30 //set scan interval.
#define SCAN_CONFIG_SCAN_WINDOW_MS    15 //set scan window.

#define SCAN_SET_TIME_MS(x)  (uint16_t)(x * 1000 / 625)

static bd_addr_t m_rnd_addr_scan = {0xFD, 0x51, 0x52, 0x53, 0x53, 0x02};

// set scan addr.
static void mesh_scan_addr_set(void){
    gap_set_random_device_address(m_rnd_addr_scan);
}

// set scan parameters.
void mesh_scan_param_set(uint16_t interval_ms, uint8_t window_ms){
    // Setup scan parameters.
    bd_addr_type_t          own_addr_type = BD_ADDR_TYPE_LE_RANDOM;
    scan_filter_policy_t    filter_policy = SCAN_ACCEPT_ALL_EXCEPT_NOT_DIRECTED;
    uint8_t                 config_num = 1;
    scan_phy_config_t       scan_config[1] = {{.phy = PHY_1M, .type = SCAN_PASSIVE, 
                                                .interval = SCAN_SET_TIME_MS(interval_ms), 
                                                .window = SCAN_SET_TIME_MS(window_ms)}};
    if (0 != gap_set_ext_scan_para(own_addr_type, filter_policy, config_num, scan_config)){
        printf("=============>ERR - %s\n", __func__);
    }
}

// scan enable or disable.
void mesh_scan_run_set(uint8_t en){

    printf("%s\n", __func__);

    // Scan diable.
    uint8_t     enable = en;    // 开始或者停止扫描  
    uint8_t     filter = 0;     // 是否对数据做去重处理：0 - 不去重；1 - 去重；2 - 去重，但是每个周期复位过滤器  
    uint16_t    duration = 0;   // 持续时间，单位为 10ms ：为0，则一直持续扫描。
    uint16_t    period = 0;     // 周期，单位为 1.28s :为0，则无周期，扫描duration后自动停止，如果duration也同时为0，则持续扫描，不停止。
    if (0 != gap_set_ext_scan_enable(enable, filter, duration, period)){
        printf("=============>ERR - %s, en = %d\n", __func__, en);
    }
}

void mesh_scan_start(void){
    printf("%s\n", __func__);
    mesh_scan_run_set(1);
}

void mesh_scan_stop(void){
    printf("%s\n", __func__);
    mesh_scan_run_set(0);
}


void mesh_setup_scan(void){

    printf("%s\n", __func__);

    // Setup address of scanning other devices and creating connection with other devices.
    mesh_scan_addr_set();

    // Setup scan parameters.
    mesh_scan_param_set(SCAN_CONFIG_SCAN_INTERVAl_MS, SCAN_CONFIG_SCAN_WINDOW_MS);
}

// -------------------------------------------------------------------------------------------------------
// for test scan params.
typedef struct {
    uint8_t seq;
    uint16_t interval;
    uint16_t window;
} my_scan_stu_t;

const my_scan_stu_t my_scan_param[] = {
    {0, 48, 40}, //seq, interval, window (0.625ms)
    {1, 100, 80},
    {2, 200, 160},
    {3, 10, 8},
    {4, 10, 10},
    {5, 20, 20},
    {6, 20, 18},
};

// key press change scan params.
void mesh_scan_param_update(void){
    static uint8_t scan_flag = 0;
    uint8_t i = 0;

    mesh_scan_run_set(0);

    for(i=0; i<sizeof(my_scan_param)/sizeof(my_scan_stu_t); i++){
        if(my_scan_param[i].seq == scan_flag){
            printf("scan param, interval, window: %d,%d . \n", my_scan_param[i].interval, my_scan_param[i].window);
            mesh_scan_param_set(my_scan_param[i].interval, my_scan_param[i].window);//interval, window, ms, 0.625ms
            mesh_scan_run_set(1);
            scan_flag ++;
            break;
        }
    }

    if(i == sizeof(my_scan_param)/sizeof(my_scan_stu_t)){
        printf("stop scan.\n");
        scan_flag = 0;
    }
}
// -------------------------------------------------------------------------------------------------------

#endif

/**
* @brief  after node reset, the mesh proxy service need to be restarted.
*
*/
void mesh_server_restart(void)
{
    ble_mesh_provisioned_status_set(0);
    printf("reset disconnect %d\n", my_conn_handle);
    if (my_conn_handle != CON_HANDLE_INVALID){
        gap_disconnect(my_conn_handle);
    } else {
        mesh_disconnected(my_conn_handle, NULL);
    }
}


static btstack_packet_callback_registration_t mesh_hci_event_callback_registration;
void mesh_setup_profile(void)
{
    platform_printf("mesh_setup_profile start\n");

    // mesh store init
    mesh_storage_init();
    
    // mesh init.
    mesh_stack_init();
    // Ble_AttSendBuffer_Init();
    bt_port_mesh_setup();

    // ble init.
    att_server_init(att_read_callback, att_write_callback);
    mesh_hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&mesh_hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);

    return;
}

