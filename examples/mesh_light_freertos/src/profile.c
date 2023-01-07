#include "profile.h"
#include "platform_api.h"
#include "mesh_storage.h"
#include "mesh.h"
#include "mesh_port_stack.h"
#include "mesh_port_low_level_init.h"
#include "board.h"

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
    u8_t val;
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

int light_model_gen_onoff_get(mesh_model_t *model, u8_t *state)
{
    light_state_t *a_light = get_light_state(model, bt_mesh_gen_onoff_srv_cb);
    *state = a_light->onoff[0];
    printf("gen get state: %d\n", *state);
    return 0;
}

int light_model_gen_onoff_set(mesh_model_t *model, u8_t state)
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

int light_model_gen_level_get(mesh_model_t *model, s16_t *level)
{
    light_state_t *a_light = get_light_state(model, bt_mesh_gen_level_srv_cb);
    *level = a_light->level[0];
    printf("gen get level: %d\n", *level);
    return 0;
}

int light_model_gen_level_set(mesh_model_t *model, s16_t  level)
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


static void mesh_elems_model_init(void){
    mesh_elems_and_models_ll_init(&comp);
}




/*--------------------------------------------------------------------
 *------------------------> PROVISIONING <----------------------------
 *------------------------------------------------------------------*/
#if 0


#define USE_OOB

#ifdef USE_OOB
static int output_number(bt_mesh_output_action_t action, u32_t number)
{
    printf("OOB Number %u", number);
    return 0;
}

static int output_string(const char *str)
{
    printf("OOB String %s", str);
    return 0;
}
#endif

static void prov_complete(u16_t net_idx, u16_t addr)
{
    printf("provisioning complete for net_idx 0x%04x addr 0x%04x",net_idx, addr);
    
}

static void prov_reset(void)
{
    
}

#define BLE_MESH_DEV_UUID ((uint8_t[16]){0xA8, 0x01, 0x61,0x00,0x04,0x20,0x30,0x75,0x9a,0x00,0x09,0xda,0x78,0x00,0x00,0x00})

static u8_t dev_uuid[16] = BLE_MESH_DEV_UUID;
const unsigned char addr[6] = {1,0,0,0,0,0};

static const struct bt_mesh_prov prov = {
    .uuid = dev_uuid,
#if USE_OOB
    .output_size = 6,
    .output_actions = (BT_MESH_DISPLAY_NUMBER | BT_MESH_DISPLAY_STRING),
    .output_number = output_number,
    .output_string = output_string,
#else
    .output_size = 0,
    .output_actions = 0,
    .output_number = 0,
    .output_string = 0,
#endif
    .complete = prov_complete,
    .reset = prov_reset,
};

#endif

/*--------------------------------------------------------------------
 *--------------------------> PLATFORM <------------------------------
 *------------------------------------------------------------------*/



/*--------------------------------------------------------------------
 *-----------------------------> API <--------------------------------
 *------------------------------------------------------------------*/
static void mesh_flash_init(void){
    mesh_storage_init();
    mesh_stack_storage_init();
}

void mesh_elements_init(void){
    mesh_elems_model_init();
}

extern void mesh_platform_config(void);
static void mesh_platform_init(void){
    // name, addr.
    mesh_platform_config();
    
}

extern void mesh_prov_config(void);
static void mesh_provising_init(void){
    mesh_port_init();
    mesh_prov_config();
}

void mesh_init(void){
    platform_printf("mesh start.\n");
    mesh_flash_init();
    mesh_stack_init(&mesh_elements_init);
    mesh_platform_init();
    mesh_provising_init();
}

