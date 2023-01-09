#include "profile.h"
#include "platform_api.h"
#include "mesh_storage_app.h"
#include "mesh.h"
#include "mesh_port_stack.h"
#include "mesh_port_low_level_init.h"
#include "board.h"
#include "mesh_version.h"

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
    .net_transmit = 3,
    .relay_retransmit = 3,
    
};

static void light_update(struct light_state *a_light)
{
    uint8_t val;
    if (!a_light->onoff[0]) { // Set led off.
        val = 0;
    } else { // Set led on, belongs to level value.
        uint32_t lvl = a_light->level[0] + 32768; // 0~65535
        lvl = ((lvl + 1) >> 8);     //0~256
        if(lvl == 256) lvl = 255;   //0~255
        val = (uint8_t)lvl;
    }
    printf("gen set rgb val: %d\n",val);
    set_rgb_led_color(val, val, val);
}

#define get_light_state(model, srv_cb) (light_state_t *)((struct srv_cb *)model->user_data)->light_state

int light_model_gen_onoff_get(mesh_model_t *model, uint8_t *state)
{
    light_state_t *a_light = get_light_state(model, bt_mesh_gen_onoff_srv_cb);
    *state = a_light->onoff[0];
    printf("gen get state: %d\n", *state);
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
    
    printf("gen set state: %d\n", state);
    light_update(a_light);
    return 0;
}

int light_model_gen_level_get(mesh_model_t *model, int16_t *level)
{
    light_state_t *a_light = get_light_state(model, bt_mesh_gen_level_srv_cb);
    *level = a_light->level[0];
    printf("gen get level: %d\n", *level);
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
    printf("gen set level -> %d\n", level);
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
    printf("OOB Number %u", number);
    return 0;
}
#endif

#if (USE_OOB_TYPE == MESH_OOB_TYPE_INTPUT)
static int input_request(void)
{
    printf("Please view displaying string or number at client. And then call :\n");
    printf("1. bt_mesh_input_string(); to send string. Or,\n");
    printf("2. bt_mesh_input_number(); to send number.\n");
    return 0;
}
#endif

static void prov_complete(uint16_t net_idx, uint16_t addr)
{
    printf("provisioning complete for net_idx 0x%04x addr 0x%04x\n",net_idx, addr);
    
}

static void prov_reset(void)
{
    printf("====>node reset.\n");
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



/*--------------------------------------------------------------------
 *-----------------------------> API <--------------------------------
 *------------------------------------------------------------------*/
static void mesh_get_ver_info(void){    
    char version[8];
    int ver_len = mesh_get_version_info_str(version, sizeof(version));
    if(ver_len > 0 && ver_len < sizeof(version)-1){
        version[ver_len] = '\0';
        printf("mesh ver: v%s\n", version); 
    }
    
    char date_time[30];
    int str_len = mesh_get_lib_compile_date_time(date_time, sizeof(date_time));
    if(str_len > 0 ){
        printf("mesh date: %s\n", date_time); 
    }
}

static void mesh_flash_init(void){
    mesh_storage_app_init();
    mesh_storage_stack_init();
}

void mesh_elements_init(void){
    mesh_elems_and_models_ll_init(&comp);
}

extern void mesh_platform_config(void);
static void mesh_platform_init(void){
    // name, addr.
    mesh_platform_config();
    
}

static void mesh_provising_init(void){
    mesh_port_init();
    mesh_prov_ll_init(&prov);
}



void mesh_init(void){
    platform_printf("mesh start.\n");
    mesh_get_ver_info();
    mesh_flash_init();
    mesh_platform_init();
    mesh_stack_init(&mesh_elements_init);
    mesh_provising_init();
}

