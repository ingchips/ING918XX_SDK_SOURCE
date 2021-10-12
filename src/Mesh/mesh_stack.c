#include <stdarg.h>
#include <stdio.h>
#include "gap.h"
#include "bt_types.h"
#include "btstack_event.h"
#include "errno.h"
#include "stdbool.h"
#include "mesh_def.h"
#include "access.h"
#include "platform_api.h"
#include "mesh_queue.h"
#include "mesh_priv.h"
#include "att_db.h"
#include "mesh_api.h"
#include "sig_uuid.h"

#define PANIC(fmt, args...)            \
    do                                 \
    {                                  \
        printf("PANIC: " fmt, ##args); \
        for (;;)                       \
            ;                          \
    } while (false);

static uint16_t SERVICE_MESH_PROXY_HDL = 0;
static uint16_t SERVICE_MESH_PROV_HDL = 0;
static uint16_t PROV_DATA_IN_HDL = 0;
static uint16_t PROXY_DATA_IN_HDL = 0;
static uint16_t PROV_DATA_OUT_CCC = 0;
static uint16_t PROXY_DATA_OUT_CCC = 0;
static uint16_t notify_write = 0;

static uint8_t pre_adv[255] = {0};

static mesh_gap_event mesh_event;

int mesh_att_write_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    if (attribute_handle == PROV_DATA_IN_HDL || attribute_handle == PROXY_DATA_IN_HDL)
    {
        //sent to the queue of mesh
        memset(&mesh_event, 0, sizeof(mesh_event));
        mesh_event.type = BLE_GAP_EVENT_CONNECT; // TODO: ext adv or legal adv.
        mesh_event.connect.conn_handle = con_handle;
        mesh_event.connect.attr_handle = attribute_handle;
        uint8_t *ptr = NULL;
        DTBT_msg_t *proxy_data = btstack_memory_dtbt_msg_get();
        if (proxy_data == NULL)
        {
            printf("warning buffer full\n");
            return -1;
        }
        ASSERT_ERR(HCI_ACL_PAYLOAD_SIZE + 4 > (buffer_size + sizeof(mesh_event)));
        mesh_event.connect.data = proxy_data->data + sizeof(mesh_event);
        mesh_event.connect.length = buffer_size;
        ptr = proxy_data->data + sizeof(mesh_event);
        memcpy(proxy_data->data, &mesh_event, sizeof(mesh_event));
        memcpy(ptr, buffer, buffer_size);
        Host2Mesh_msg_send((uint8_t *)(proxy_data->data), sizeof(mesh_event) + buffer_size);
    }
    else
    {
        if (!bt_mesh_is_provisioned())
        {
            if (attribute_handle == PROV_DATA_OUT_CCC)
                notify_write = little_endian_read_16(buffer, 0);
        }
        else
        {
            if (attribute_handle == PROXY_DATA_OUT_CCC)
                notify_write = little_endian_read_16(buffer, 0);
        }
    }
    
    return 0;
}

/**
* @brief to dispose the read operation befor returning data
*        in mesh service ,this function only used to dippose the character client configure attribute
*
* @param con_handle          handle of a connection
*
*
* @param attribute_handle    from which database handle to read data
*
* @param offset              offset from the reading data
*
* @param buffer              to cache the read data
*
* @param buffer_size         how much data to be read
*
* @return    the size the data to be read
*/
uint16_t mesh_att_read_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    if (!bt_mesh_is_provisioned())
    {
        if (attribute_handle == PROV_DATA_OUT_CCC)
            memcpy(buffer, &notify_write, 2);
        return 2;
    }
    else
    {
        if (attribute_handle == PROXY_DATA_OUT_CCC)
            memcpy(buffer, &notify_write, 2);
        return 2;
    }
}

/**
* Retrieves the attribute handle associated with a local GATT service.
*
* @param uuid                  The UUID of the service to look up.
* @param out_handle            On success, populated with the handle of the
*                                  service attribute.  Pass null if you don't
*                                  need this value.
*
* @return                      0 on success;
*                              BLE_HS_ENOENT if the specified service could
*                                  not be found.
*/
int ble_gatts_find_svc(uint8_t *db, const ble_uuid_t *uuid, uint16_t *out_handle)
{

    if ((((ble_uuid16_t *)uuid)->value) == SIG_UUID_SERVICE_MESH_PROXY)
    {
        *out_handle = SERVICE_MESH_PROXY_HDL;
        return 0;
    }
    if ((((ble_uuid16_t *)uuid)->value) == SIG_UUID_SERVICE_MESH_PROVISIONING)
    {
        *out_handle = SERVICE_MESH_PROV_HDL;
        return 0;
    }
    return -1;
}

void mesh_handle_adv_report(const uint8_t *packet, uint16_t size)
{
    const le_ext_adv_report_t *report;

    //need to filter the message at first, only for the mesh message
    if (filtermeshADV(HCI_EVENT_PACKET, (uint8_t *)packet, size))
        return;

    if (memcmp(pre_adv, packet, size) == 0)
        return;
    
    report = decode_hci_le_meta_event(packet, le_meta_event_ext_adv_report_t)->reports;

    #define CACHE_CURRENT_ADV_PACKET(to, from, size) \
            {                                        \
            memset(to, 0, sizeof(to));               \
            memcpy(to, from, size);                  \
            }
    //store current packet as last received pre_adv.
    CACHE_CURRENT_ADV_PACKET(pre_adv, packet, size);
            
    memset(&mesh_event, 0, sizeof(mesh_event));
    mesh_event.type = BLE_GAP_EVENT_EXT_DISC; // TODO: ext adv or legal adv.
    mesh_event.ext_disc.addr.type = report->addr_type;
    mesh_event.ext_disc.props = report->evt_type;
    memcpy(mesh_event.ext_disc.addr.val, report->address, 6);
    mesh_event.ext_disc.rssi = report->rssi;
    mesh_event.ext_disc.legacy_event_type = report->evt_type | 0x10;
    mesh_event.ext_disc.length_data = report->data_len;
    //printf("add 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",report->address[0],report->address[1],report->address[2],report->address[3],report->address[4],report->address[5]);
    if (report->data_len == 0)
        return;
    DTBT_msg_t *padv = btstack_memory_dtbt_msg_get();
    if (padv == NULL)
    {
        printf("warning:full\n");
        return;
    }
    ASSERT_ERR(HCI_ACL_PAYLOAD_SIZE + 4 > (report->data_len + sizeof(mesh_event)));

    mesh_event.ext_disc.data = padv->data + sizeof(mesh_event);
    reverse_bd_addr(report->address, mesh_event.ext_disc.addr.val); //must reverse the addr
    mesh_event.ext_disc.addr.type = (bd_addr_type_t)report->addr_type;
    memcpy(padv->data, &mesh_event, sizeof(mesh_event));
    memcpy(padv->data + sizeof(mesh_event), (uint8_t *)report->data, report->data_len);
    //printf("addr 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",report->address[0],report->address[1],report->address[2],report->address[3],report->address[4],report->address[5]);
    Host2Mesh_msg_send((uint8_t *)padv->data, sizeof(mesh_event) + report->data_len);
}



void mesh_stack_ready()
{
    mesh_event.type = STACK_INIT_DONE;
    Host2Mesh_msg_send((uint8_t *)&mesh_event, sizeof(mesh_event));
}

void mesh_connected(uint16_t conn_handle)
{
    mesh_event.type = ADV_TERM_CON;
    mesh_event.connect.conn_handle = conn_handle;
    Host2Mesh_msg_send((uint8_t *)&mesh_event, sizeof(mesh_event));
}

void mesh_disconnected(uint8_t reason, uint16_t conn_handle)
{
    mesh_event.type = BLE_GAP_EVENT_DISCONNECT;
    mesh_event.disconnect.reason = reason;
    mesh_event.disconnect.conn.conn_handle = conn_handle;
    Host2Mesh_msg_send((uint8_t *)&mesh_event, sizeof(mesh_event));
}

static const struct bt_mesh_prov *p_prov = NULL;
static const struct bt_mesh_comp *p_comp = NULL;

/**
* @brief  setup the SIG model and vendor model ,as well as setup the provision configuration
*
* @param a_prov     a struct to a provision configuration
*
* @param a_comp     pointer to a device composition that consist of SIG model and vendor model
*
* @return    the size the data to be read
*/
void mesh_setup(const struct bt_mesh_prov *a_prov, const struct bt_mesh_comp *a_comp)
{
    struct bt_mesh_elem *root = a_comp->elem;
    struct bt_mesh_model *model = root->models;
    u8_t model_count;

    p_prov = a_prov;
    p_comp = a_comp;
    // setup configure client & server op
    for (model_count = 0; model_count < root->model_count; model_count++)
    {
        if (BT_MESH_MODEL_ID_CFG_SRV == model[model_count].id)
        {
            *((const struct bt_mesh_model_op **)&model[model_count].op) = bt_mesh_cfg_srv_op;
        }
        if (BT_MESH_MODEL_ID_HEALTH_SRV == model[model_count].id)
            *((const struct bt_mesh_model_op **)&model[model_count].op) = bt_mesh_health_srv_op;
#if MYNEWT_VAL(BLE_MESH_CFG_CLI)
        if (BT_MESH_MODEL_ID_CFG_CLI == model[model_count].id)
            *((const struct bt_mesh_model_op **)&model[model_count].op) = bt_mesh_cfg_cli_op;
#endif
    }
}

int mesh_main()
{
    int err;
    printf("Bluetooth initialized\n");

#if (MYNEWT_VAL(BLE_MESH_SHELL))
    ble_mesh_shell_init(); //note: this shell init should follow PLATFORM_CB_EVT_PROFILE_INIT.
#endif

    if ((NULL == p_prov) || (NULL == p_comp))
        PANIC("prov/comp not initialized");

    err = bt_mesh_init(1, p_prov, p_comp);
    if (err)
        PANIC("Initializing mesh failed (err %d)\n", err);

    /* Make sure we're scanning for provisioning inviations */
    /* Enable unprovisioned beacon sending */
    bt_mesh_beacon_init();

    return 0;
}

void mesh_stack_init(uint16_t service_proxy, uint16_t handle_proxy_in, uint16_t handle_proxy_out,
                     uint16_t service_prov, uint16_t handle_prov_in, uint16_t handle_prov_out,
                     uint16_t feature_role)
{
    mesh_platform_setup();
    mesh_memory_init();
    mesh_feature_set(BT_MESH_FEAT_RELAY | BT_MESH_FEAT_PROXY, BT_MESH_FEAT_PROXY); //setup the feature_role
    model_init();
    mesh_main();
    PROV_DATA_IN_HDL = handle_prov_in;
    PROXY_DATA_IN_HDL = handle_proxy_in;
    SERVICE_MESH_PROV_HDL = service_prov;
    SERVICE_MESH_PROXY_HDL = service_proxy;
    PROV_DATA_OUT_CCC = handle_prov_out + 1;
    PROXY_DATA_OUT_CCC = handle_proxy_out + 1;
    bt_mesh_handle_set(service_prov, handle_prov_in, handle_prov_out,
                       service_proxy, handle_proxy_in, handle_proxy_out);
}    

void gap_beacon_disable(void)
{
    if (!is_mesh_task())
    {
        memset(&mesh_event, 0, sizeof(mesh_event));
        mesh_event.type = BLE_MESH_BEACON_STATE;
        mesh_event.beacon_state.enable = 0;
        Host2Mesh_msg_send((uint8_t *)&mesh_event, sizeof(mesh_event));
    }
    else
        bt_mesh_beacon_disable();
}

void gap_beacon_enable(void)
{
    if (!is_mesh_task())
    {
        memset(&mesh_event, 0, sizeof(mesh_event));
        mesh_event.type = BLE_MESH_BEACON_STATE;
        mesh_event.beacon_state.enable = 1;
        Host2Mesh_msg_send((uint8_t *)&mesh_event, sizeof(mesh_event));
    }
    else
        bt_mesh_beacon_enable();
}

uint16_t fea_layer_sel = 0;
uint8_t cla_flag = 0;

void mesh_trace_config(uint16_t sel_bits, uint16_t cla_bit)
{
    fea_layer_sel = sel_bits;
    cla_flag = cla_bit;
    return;
}

//below used to setup the uart data output function from APP

typedef void (*mesh_at_out)(const char *str, int cnt);

mesh_at_out uart_printf = NULL;

void set_mesh_uart_output_func(mesh_at_out ptrfun)
{
    uart_printf = ptrfun;
}

uint8_t mesh_at[100] = {0};
uint8_t size = 0;
bool recv_from_uart = false;

void mesh_at_entry(uint8_t *mesh_msg, uint8_t len)
{
    memcpy(mesh_at, mesh_msg, len);
    recv_from_uart = true;
}

//this function used to print the response of  AT command.
static char buf[200];
int console_printf(const char *fmt, ...)
{
    if (uart_printf)
    {
        va_list args;
        int num_chars;
        int len;

        num_chars = 0;
        va_start(args, fmt);
        len = vsnprintf(buf, sizeof(buf), fmt, args);
        num_chars += len;
        if (len >= sizeof(buf))
        {
            len = sizeof(buf) - 1;
        }
        uart_printf(buf, len);
        va_end(args);

        return num_chars;
    }
    else
        return 0;
}

extern uint32_t sleep_duration;
/**
* @note 20ms is a minimal advertision interval in mesh task.
*
*/
int8_t set_mesh_sleep_duration(uint32_t ms)
{
    if (ms >= 20)
    {
        sleep_duration = ms;
        return EOK;
    }
    else
    {
        return -EPERM;
    }
}