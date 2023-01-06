#include "mesh_model.h"
#include "mesh_generic_model.h"
#include "btstack_port_mesh_init.h"

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
    

#define get_light_state(model, srv_cb) (light_state_t *)((struct srv_cb *)model->user_data)->light_state

int light_model_gen_onoff_get(mesh_model_t *model, u8_t *state)
{
    light_state_t *a_light = get_light_state(model, bt_mesh_gen_onoff_srv_cb);
    *state = a_light->onoff[0];
    printf("gen get state: %d\n", *state);
    return 0;
}
#include "rgb_led.h"
int light_model_gen_onoff_set(mesh_model_t *model, u8_t state)
{
    light_state_t *a_light = get_light_state(model, bt_mesh_gen_onoff_srv_cb);
    a_light->onoff[1] = a_light->onoff[0];
    a_light->onoff[0] = state;
    a_light->lightness[1] = a_light->lightness[0];
    a_light->lightness[0] = state ? 65535 : 0;
    
    printf("gen set state: %d\n", state);
    
    if(state){
        set_rgb_led_color(50, 50, 50);
    } else {
        set_rgb_led_color(0, 0, 0);
    }
    // light_update(a_light);
    return 0;
}

int light_model_gen_level_get(mesh_model_t *model, s16_t *level)
{
    light_state_t *a_light = get_light_state(model, bt_mesh_gen_level_srv_cb);
    *level = a_light->level[0];
    printf("gen get lightness: %d\n", *level);
    return 0;
}

int light_model_gen_level_set(mesh_model_t *model, s16_t  level)
{
    light_state_t *a_light = get_light_state(model, bt_mesh_gen_level_srv_cb);
    a_light->level[1] = a_light->level[0];
    a_light->level[0] = level;
    a_light->lightness[1] = a_light->lightness[0];
    a_light->lightness[0] = level + 32768;
    printf("gen set lightness -> %d, %04x\n", level, a_light->lightness[0]);
    // light_update(a_light);
    if(level > 0){
        set_rgb_led_color(50, 50, 50);
    } else {
        set_rgb_led_color(0, 0, 0);
    }
    return 0;
}

int light_model_light_lightness_get(mesh_model_t *model, u16_t *lightness, u16_t *light, u8_t *remain)
{
    light_state_t *a_light = get_light_state(model, bt_mesh_light_lightness_srv_cb);
    *lightness = a_light->lightness[0];
    return 0;
}

int light_model_light_lightness_set(mesh_model_t *model, u16_t lightness)
{
    light_state_t *a_light = get_light_state(model, bt_mesh_light_lightness_srv_cb);
    a_light->lightness[1] = a_light->lightness[0];
    a_light->lightness[0] = lightness;

    // light_update(a_light);
    return 0;
}

int light_model_light_hsl_get(mesh_model_t *model, u16_t *hue, u16_t *saturation, u16_t *lightness, uint8_t* remain)
{
    light_state_t *a_light = get_light_state(model, bt_mesh_light_hsl_srv_cb);
    *lightness  = a_light->lightness[0];
    *hue        = a_light->hue[0];
    *saturation = a_light->saturation[0];
    return 0;
}

int light_model_light_hsl_set(mesh_model_t *model, hsl_val_t *val)
{
    light_state_t *a_light = get_light_state(model, bt_mesh_light_hsl_srv_cb);

    a_light->lightness[1]     = a_light->lightness[0];
    a_light->hue[1]           = a_light->hue[0];
    a_light->saturation[1]    = a_light->saturation[0];
    a_light->lightness[0] = val->lightness;
    a_light->hue[0]       = val->hue;
    a_light->saturation[0]= val->sa;

    // light_update(a_light);
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

static struct bt_mesh_light_lightness_srv_cb light_lightness_srv_cb = {
    .get = light_model_light_lightness_get,
    .set = light_model_light_lightness_set,
    .light_state = &a_light_state
};

static struct bt_mesh_light_hsl_srv_cb light_hsl_srv_cb = {
    .get = light_model_light_hsl_get,
    .set = light_model_light_hsl_set,
    .light_state = &a_light_state
};





static mesh_model_t sig_models[] = {
    // mandatory models: Config Server and Health Server.
    BT_MESH_MODEL(MESH_SIG_MODEL_ID_CONFIGURATION_SERVER, &cfg_srv),
    BT_MESH_MODEL(MESH_SIG_MODEL_ID_HEALTH_SERVER, NULL),
    
    // other models.
    BT_MESH_MODEL(MESH_SIG_MODEL_ID_CONFIGURATION_CLIENT, NULL),
    BT_MESH_MODEL(MESH_SIG_MODEL_ID_GENERIC_ON_OFF_SERVER, &gen_onoff_srv_cb),
    BT_MESH_MODEL(MESH_SIG_MODEL_ID_GENERIC_ON_OFF_CLIENT, NULL),
    BT_MESH_MODEL(MESH_SIG_MODEL_ID_GENERIC_LEVEL_SERVER, &gen_level_srv_cb),
    BT_MESH_MODEL(MESH_SIG_MODEL_ID_GENERIC_LEVEL_CLIENT, NULL),
    BT_MESH_MODEL(MESH_SIG_MODEL_ID_LIGHT_LIGHTNESS_SERVER, &light_lightness_srv_cb),
    BT_MESH_MODEL(MESH_SIG_MODEL_ID_LIGHT_HSL_SERVER, &light_hsl_srv_cb),
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


void mesh_elems_model_init(void){
    mesh_elements_and_models_init(&comp);
}

