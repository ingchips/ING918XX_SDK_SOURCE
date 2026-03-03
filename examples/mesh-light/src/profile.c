#include "profile.h"
#include "platform_api.h"
#include "mesh_storage_app.h"
#include "mesh.h"
#include "mesh_port_stack.h"
#include "mesh_port_low_level_init.h"
#include "board.h"
#include "mesh_version.h"
#include "adv_bearer.h"
#include "mesh_profile.h"
#include "app_debug.h"
#include "mesh_debug.h"

/*--------------------------------------------------------------------
 *----------------------------> MODEL <-------------------------------
 *------------------------------------------------------------------*/

#define INGCHIPS_COMP_ID                            0x06AC
#define MESH_PRODUCT_ID                             0x0000
#define MESH_VERSION_ID                             0x0000

/*
 * Server Configuration Declaration
 */
static bt_mesh_cfg_srv_t cfg_srv = {

    .relay = BT_MESH_RELAY_ENABLED,
    .gatt_proxy = BT_MESH_GATT_PROXY_ENABLED,
    .frnd = BT_MESH_FRIEND_NOT_SUPPORTED,
    .low_pwr = BT_MESH_LOW_POWER_NOT_SUPPORTED,
    .beacon = BT_MESH_BEACON_DISABLED,
    .default_ttl = 7,

    /* 3 transmissions with 20ms interval */
    .net_transmit = BT_MESH_TRANSMIT(5, 50),
    .relay_retransmit = BT_MESH_TRANSMIT(3, 50),

};

extern void led_set_state(uint8_t led_on);
    
static void light_update(struct light_state *a_light)
{
    // DEMO only: we only handle ONOFF here.
    app_log_info("set LED onoff: %d\n", a_light->onoff[0]);
    led_set_state(a_light->onoff[0]);
}

#define get_light_state(model, srv_cb) (light_state_t *)((struct srv_cb *)model->user_data)->light_state

int light_model_gen_onoff_get(mesh_model_t *model, uint8_t *state)
{
    light_state_t *a_light = get_light_state(model, bt_mesh_gen_onoff_srv_cb);
    *state = a_light->onoff[0];
    app_log_info("gen get state: %d\n", *state);
    return 0;
}

int light_model_gen_onoff_set(mesh_model_t *model, uint8_t state)
{
    light_state_t *a_light = get_light_state(model, bt_mesh_gen_onoff_srv_cb);
    a_light->onoff[0] = state;
    if(state){ //bind to level
        if(a_light->level[0] < 0){ //lightness can not less 50%.
            a_light->level[0] = 0;
        }
    }

    app_log_info("gen set state: %d\n", state);
    light_update(a_light);
    return 0;
}

int light_model_gen_level_get(mesh_model_t *model, int16_t *level)
{
    light_state_t *a_light = get_light_state(model, bt_mesh_gen_level_srv_cb);
    *level = a_light->level[0];
    app_log_info("gen get level: %d\n", *level);
    return 0;
}

int light_model_gen_level_set(mesh_model_t *model, int16_t  level)
{
    light_state_t *a_light = get_light_state(model, bt_mesh_gen_level_srv_cb);
    a_light->level[0] = level;
    if(level == -32768) //bind to state.
        a_light->onoff[0] = 0;
    else
        a_light->onoff[0] = 1;
    app_log_info("gen set level -> %d\n", level);
    light_update(a_light);
    return 0;
}

#define LED_1 1

light_state_t a_light_state =
{
    .led_gpio_pin = LED_1
};

static struct bt_mesh_gen_onoff_srv_cb gen_onoff_srv_cb = {
    .get = light_model_gen_onoff_get,
    .set = light_model_gen_onoff_set,
    .light_state = &a_light_state
};

static struct bt_mesh_gen_level_srv_cb gen_level_srv_cb = {
    .get = light_model_gen_level_get,
    .set = light_model_gen_level_set,
    .light_state = &a_light_state
};


static mesh_model_t sig_models[] = {
    // mandatory sig models: Config Server and Health Server.
    BT_MESH_MODEL(MESH_SIG_MODEL_ID_CONFIGURATION_SERVER, &cfg_srv),
    BT_MESH_MODEL(MESH_SIG_MODEL_ID_HEALTH_SERVER, NULL),

    // other sig models.
    BT_MESH_MODEL(MESH_SIG_MODEL_ID_CONFIGURATION_CLIENT, NULL),
    BT_MESH_MODEL(MESH_SIG_MODEL_ID_GENERIC_ON_OFF_SERVER, &gen_onoff_srv_cb),
    BT_MESH_MODEL(MESH_SIG_MODEL_ID_GENERIC_ON_OFF_CLIENT, NULL),
    BT_MESH_MODEL(MESH_SIG_MODEL_ID_GENERIC_LEVEL_SERVER, &gen_level_srv_cb),
    BT_MESH_MODEL(MESH_SIG_MODEL_ID_GENERIC_LEVEL_CLIENT, NULL),
};

// example:
// BT_MESH_MODEL_VND(INGCHIPS_COMP_ID, INGCHIPS_VND_ID_1, &vendor_srv_1);
// BT_MESH_MODEL_VND(INGCHIPS_COMP_ID, INGCHIPS_VND_ID_2, &vendor_srv_2);
static mesh_model_t vnd_models[] = {
    // add vendor models here.
};

/*
 * Primary and Secondary Element Declarations
 */
static mesh_element_t elements[] = {
    BT_MESH_ELEM(0x0000, sig_models, vnd_models), //primary element.
};

static const bt_mesh_comp_t comp = {
    .info.cid = INGCHIPS_COMP_ID,
    .info.pid = MESH_PRODUCT_ID,
    .info.vid = MESH_VERSION_ID,

    .elem = elements,
    .elem_count = ARRAY_SIZE(elements),
};




/*--------------------------------------------------------------------
 *------------------------> PROVISIONING <----------------------------
 *------------------------------------------------------------------*/

#define USE_OOB_TYPE    MESH_OOB_TYPE_NONE

#if (USE_OOB_TYPE == MESH_OOB_TYPE_OUTPUT)
static int output_number(uint32_t number)
{
    app_log_info("OOB Number %u", number);
    return 0;
}
#endif

#if (USE_OOB_TYPE == MESH_OOB_TYPE_INTPUT)
static int input_request(void)
{
    app_log_info("Please view displaying string or number at client. And then call :\n");
    app_log_info("1. bt_mesh_input_string(); to send string. Or,\n");
    app_log_info("2. bt_mesh_input_number(); to send number.\n");
    return 0;
}
#endif

static void prov_complete(uint16_t net_idx, uint16_t addr)
{
    app_log_info("provisioning complete for net_idx 0x%04x addr 0x%04x\n",net_idx, addr);
}

static void prov_reset(void)
{
    app_log_info("====>node reset.\n");
    platform_reset();
}

#define BLE_MESH_DEV_UUID ((uint8_t[16]){0xA8, 0x01, 0x61,0x00,0x04,0x20,0x30,0x75,0x9a,0x00,0x09,0xda,0x78,0x00,0x00,0x00})

static uint8_t dev_uuid[16] = BLE_MESH_DEV_UUID;

// Provisioning struct.
static const bt_mesh_prov_t prov = {
    .uuid = dev_uuid,
#if (USE_OOB_TYPE == MESH_OOB_TYPE_OUTPUT)
    .output_oob_action = BT_MESH_DISPLAY_NUMBER,
    .output_oob_max_size = 1,
    .output_number = output_number,
#elif (USE_OOB_TYPE == MESH_OOB_TYPE_INTPUT)
    .input_oob_action = BT_MESH_ENTER_STRING,
    .input_oob_max_size = 8,
    .input_req = input_request,
#endif
    .complete = prov_complete,
    .reset = prov_reset,
};


/*--------------------------------------------------------------------
 *--------------------------> PLATFORM <------------------------------
 *------------------------------------------------------------------*/

void mesh_platform_init(void){
    const char mesh_name[] = "ing-mesh";
    const bd_addr_t addr_gatt_adv    = {0xd5, 0x33, 0xa3, 0x17, 0x2f, 0xFC};
    const bd_addr_t addr_beacon_adv  = {0xd0, 0x2a, 0x4e, 0x19, 0x28, 0xFC};

    mesh_set_addr_static((uint8_t *)addr_gatt_adv);
    mesh_set_addr_static((uint8_t *)addr_beacon_adv);
    mesh_platform_config(MESH_CFG_NAME, (uint8_t *)mesh_name, strlen(mesh_name));
    mesh_platform_config(MESH_CFG_GATT_ADV_ADDR, (uint8_t *)addr_gatt_adv, sizeof(bd_addr_t));
    mesh_platform_config(MESH_CFG_BEACON_ADV_ADDR, (uint8_t *)addr_beacon_adv, sizeof(bd_addr_t));
    mesh_platform_adv_params_init();
}


/*--------------------------------------------------------------------
 *-----------------------------> API <--------------------------------
 *------------------------------------------------------------------*/
static void mesh_get_ver_info(void){
    char version[28];
    int ver_len = mesh_get_lib_version_info(version, sizeof(version));
    if(ver_len > 0){
        platform_printf("mesh version info: %s\n", version);
    }
}

static void mesh_flash_init(void){
    mesh_storage_app_init();
    mesh_storage_stack_init();
}

void mesh_elements_init(void){
    mesh_elems_and_models_ll_init(&comp);
}

static void mesh_provising_init(void){
    mesh_port_init();
    mesh_prov_ll_init(&prov);
}

static void print_addr(char *str, bd_addr_t addr)
{
    platform_printf("%s: %02X:%02X:%02X:%02X:%02X:%02X\n", str, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}

static void mesh_basic_info_print(void){
    
    // name
    uint8_t name[30];
    uint8_t name_len = sizeof(name);
    adv_bearer_adv_get_scan_rsp_data(name, &name_len);
    name[name_len] = '\0';
    platform_printf("dev name: %s\n", name);

    // uuid
    uint8_t uuid[16];
    const uint8_t * pUuid = mesh_node_get_device_uuid();
    memcpy(uuid, pUuid, 16);
    platform_printf("mesh uuid: ");
    printf_hexdump(uuid, sizeof(uuid));

    // addr
    bd_addr_t gatt_adv_addr;
    bd_addr_t beacon_adv_addr;
    mesh_gatt_adv_addr_get(gatt_adv_addr);
    mesh_beacon_adv_addr_get(beacon_adv_addr);
    print_addr((char*)"gatt adv addr", gatt_adv_addr);
    print_addr((char*)"beacon adv addr", beacon_adv_addr);
}



void mesh_init(void){
    app_log_info("mesh start.\n");
    // mesh_trace_config(FLASH_FEA, ALL_LVL);
    mesh_get_ver_info();
    mesh_flash_init();
    mesh_platform_init();
    mesh_stack_init(&mesh_elements_init);
    mesh_provising_init();
    mesh_basic_info_print();
}

