#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "platform_api.h"
#include "gap.h"
#include "bt_types.h"
#include "btstack_event.h"
#include "att_db.h"
#include "sig_uuid.h"
#include "mesh_port_stack.h"
#include "adv_bearer.h"
#include "mesh.h"
#include "mesh_port_pb.h"
#include "mesh_profile.h"
#include "app_config.h"
#include "mesh_profile.h"
#include "app_debug.h"

// profile data.
#include "../data/gatt_pb.const"
#include "../data/gatt_proxy.const"

#define ATT_HANDLE_INVALID 0xFFFF

// variable.
static uint16_t PROV_DATA_IN_HDL = ATT_HANDLE_INVALID;
static uint16_t PROXY_DATA_IN_HDL = ATT_HANDLE_INVALID;
static uint16_t PROV_DATA_OUT_HDL = ATT_HANDLE_INVALID;
static uint16_t PROXY_DATA_OUT_HDL = ATT_HANDLE_INVALID;
static uint16_t PROV_DATA_OUT_CCCD_HDL = ATT_HANDLE_INVALID;
static uint16_t PROXY_DATA_OUT_CCCD_HDL = ATT_HANDLE_INVALID;

static bool notify_enable = false;


/******************************************************************************************
 * @brief Receive attribute read requestion.
 * @note This func run at host task.
 */
uint16_t mesh_att_read_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, uint8_t *buffer, uint16_t buffer_size)
{
    if(attribute_handle == PROV_DATA_OUT_CCCD_HDL || attribute_handle == PROXY_DATA_OUT_CCCD_HDL){
        if (buffer) {            
            memcpy(buffer, &notify_enable, 2);
        }
        return 2;
    }
    return 0;
}

/******************************************************************************************
 * @brief Receive attribute write requestion.
 * @note This func run at host task.
 */
int mesh_att_write_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, const uint8_t *buffer, uint16_t buffer_size)
{
    mesh_att_write_prov_proxy_callback(con_handle, attribute_handle, buffer, buffer_size);
    
    if(PROV_DATA_OUT_CCCD_HDL == attribute_handle || PROXY_DATA_OUT_CCCD_HDL == attribute_handle ){
        notify_enable = little_endian_read_16(buffer, 0) != 0;
    }
    
    return 0;
}

/******************************************************************************************
 * @brief Btstack init ok callback.
 * @note This func run at host task.
 */
void mesh_stack_ready(void)
{
    app_log_debug("%s\n", __func__);
    mesh_setup_adv();
    mesh_setup_scan();
}

/******************************************************************************************
 * @brief Central connect success
 * @note This func run at host task.
 */
void mesh_connected(uint16_t conn_handle)
{
    app_log_debug("%s\n", __func__);
}

/******************************************************************************************
 * @brief Central diconnect success
 * @note This func run at host task.
 */
void mesh_disconnected(uint16_t conn_handle, uint8_t reason)
{
    app_log_debug("%s\n", __func__);
    
    // mesh_adv_start();
    mesh_port_proxy_disconnect(conn_handle);
}

/******************************************************************************************
 * @brief gatt can send now callback.
 * @note This func run at host task.
 */
void mesh_gatt_can_send_now(uint16_t conn_handle)
{
    mesh_att_service_trigger_can_send_now();
}

/******************************************************************************************
 * @brief Mesh stack initialization function.
 */
void mesh_port_init(void)
{
    PROV_DATA_IN_HDL = HANDLE_MESH_PROVISIONING_DATA_IN;
    PROV_DATA_OUT_HDL = HANDLE_MESH_PROVISIONING_DATA_OUT;
    PROV_DATA_OUT_CCCD_HDL = HANDLE_MESH_PROVISIONING_DATA_OUT_CLIENT_CHAR_CONFIG;
    
    PROXY_DATA_IN_HDL = HANDLE_MESH_PROXY_DATA_IN;
    PROXY_DATA_OUT_HDL = HANDLE_MESH_PROXY_DATA_OUT;
    PROXY_DATA_OUT_CCCD_HDL = HANDLE_MESH_PROXY_DATA_OUT_CLIENT_CHAR_CONFIG;

    mesh_att_handle_prov_proxy_init(PROV_DATA_IN_HDL, PROV_DATA_OUT_HDL, PROV_DATA_OUT_CCCD_HDL,
                                    PROXY_DATA_IN_HDL, PROXY_DATA_OUT_HDL, PROXY_DATA_OUT_CCCD_HDL);
}

