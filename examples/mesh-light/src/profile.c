#include <stdio.h>

#include "syscfg.h"
//#include "glue.h"
//#include "main.h"

#include "mesh_api.h"

#include "cfg_srv.h"
#include "cfg_cli.h"

#include "model_srv.h"
#include "model_cli.h"

#define BT_INFO_ENABLED 1

#define INGCHIPS_COMP_ID                           0x06AC

/*
 * Server Configuration Declaration
 */

static struct bt_mesh_cfg_srv cfg_srv = {
    .relay = BT_MESH_RELAY_DISABLED,
    .beacon = BT_MESH_BEACON_ENABLED,
#if defined(CONFIG_BT_MESH_FRIEND)
    .frnd = BT_MESH_FRIEND_ENABLED,
#else
    .frnd = BT_MESH_FRIEND_NOT_SUPPORTED,
#endif
#if defined(CONFIG_BT_MESH_GATT_PROXY)
    .gatt_proxy = BT_MESH_GATT_PROXY_ENABLED,
#else
    .gatt_proxy = BT_MESH_GATT_PROXY_NOT_SUPPORTED,
#endif
    .default_ttl = 7,

    /* 3 transmissions with 20ms interval */
    .net_transmit = BT_MESH_TRANSMIT(2, 20),
    .relay_retransmit = BT_MESH_TRANSMIT(2, 20),
};

/*
 * Client Configuration Declaration
 */

static struct bt_mesh_cfg_cli cfg_cli = {
};


/*
 * Publication Declarations
 *
 * The publication messages are initialized to the
 * the size of the opcode + content
 *
 * For publication, the message must be in static or global as
 * it is re-transmitted several times. This occurs
 * after the function that called bt_mesh_model_publish() has
 * exited and the stack is no longer valid.
 *
 * Note that the additional 4 bytes for the AppMIC is not needed
 * because it is added to a stack variable at the time a
 * transmission occurs.
 *
 */

static struct bt_mesh_model_pub gen_onoff_pub;
static struct bt_mesh_model_pub gen_level_pub;
static struct bt_mesh_model_pub light_lightness_pub;
static struct bt_mesh_model_pub light_hsl_pub;

void init_pub(void)
{
    gen_onoff_pub.msg = NET_BUF_SIMPLE(2 + 2);
    light_lightness_pub.msg = NET_BUF_SIMPLE(2 + 3);
    gen_level_pub.msg = NET_BUF_SIMPLE(2 + 3);
    light_hsl_pub.msg = NET_BUF_SIMPLE(2 + 1 + 2 * 3);
}

#define LED_1 1

struct light_state {
    u8_t onoff[2];
    s16_t level[2];
    u16_t lightness[2];
    u16_t hue[2];
    u16_t saturation[2];
    u8_t led_gpio_pin;
} a_light_state = 
{
    .led_gpio_pin = LED_1
};

int light_model_gen_onoff_get(struct bt_mesh_model *model, u8_t *state, void *user_data);
int light_model_gen_onoff_set(struct bt_mesh_model *model, u8_t  state, void *user_data);
int light_model_gen_level_get(struct bt_mesh_model *model, s16_t *level, void *user_data);
int light_model_gen_level_set(struct bt_mesh_model *model, s16_t  level, void *user_data);
int light_model_light_lightness_get(struct bt_mesh_model *model, u16_t *lightness, void *user_data);
int light_model_light_lightness_set(struct bt_mesh_model *model, u16_t  lightness, void *user_data);
int light_model_light_hsl_get(struct bt_mesh_model *model, u16_t *lightness, u16_t *hue, u16_t *saturation, void *user_data);
int light_model_light_hsl_set(struct bt_mesh_model *model, u16_t  lightness, u16_t  hue, u16_t  saturation, void *user_data);

static struct bt_mesh_gen_onoff_srv_cb gen_onoff_srv_cb = {
	.get = light_model_gen_onoff_get,
	.set = light_model_gen_onoff_set,
    .user_data = &a_light_state
};

static struct bt_mesh_gen_level_srv_cb gen_level_srv_cb = {
	.get = light_model_gen_level_get,
	.set = light_model_gen_level_set,
    .user_data = &a_light_state
};

static struct bt_mesh_light_lightness_srv_cb light_lightness_srv_cb = {
	.get = light_model_light_lightness_get,
	.set = light_model_light_lightness_set,
    .user_data = &a_light_state
};

static struct bt_mesh_light_hsl_srv_cb light_hsl_srv_cb = {
	.get = light_model_light_hsl_get,
	.set = light_model_light_hsl_set,
    .user_data = &a_light_state
};

#undef  BT_MESH_MODEL_CFG_SRV
#undef  BT_MESH_MODEL_CFG_CLI
#define BT_MESH_MODEL_CFG_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_CFG_SRV, NULL, NULL, _user_data)
#define BT_MESH_MODEL_CFG_CLI(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_CFG_CLI, NULL, NULL, _user_data)

static struct bt_mesh_model root_models[] = {
    BT_MESH_MODEL_CFG_SRV(&cfg_srv),
    BT_MESH_MODEL_CFG_CLI(&cfg_cli),
    BT_MESH_MODEL_GEN_ONOFF_SRV(&gen_onoff_srv_cb, &gen_onoff_pub),
    BT_MESH_MODEL_GEN_ONOFF_CLI(),
	BT_MESH_MODEL_GEN_LEVEL_SRV(&gen_level_srv_cb, &gen_level_pub),
    BT_MESH_MODEL_GEN_LEVEL_CLI(),
	BT_MESH_MODEL_LIGHT_LIGHTNESS_SRV(&light_lightness_srv_cb, &light_lightness_pub),
    BT_MESH_MODEL_LIGHT_HSL_SRV(&light_hsl_srv_cb, &light_hsl_pub),
};

/*
 * Root and Secondary Element Declarations
 */

static struct bt_mesh_elem elements[] = {
    BT_MESH_ELEM(0, root_models, BT_MESH_MODEL_NONE),
};

static const struct bt_mesh_comp comp = {
    .cid = INGCHIPS_COMP_ID,
    .elem = elements,
    .elem_count = ARRAY_SIZE(elements),
};

void light_update(struct light_state *a_light);

volatile static u16_t primary_addr;
volatile static u16_t primary_net_idx;

int light_model_gen_onoff_get(struct bt_mesh_model *model, u8_t *state, void *user_data)
{
    struct light_state *a_light = (struct light_state *)user_data;
    *state = a_light->onoff[0];
    return 0;
}

int light_model_gen_onoff_set(struct bt_mesh_model *model, u8_t state, void *user_data)
{
    struct light_state *a_light = (struct light_state *)user_data;
    a_light->onoff[1] = a_light->onoff[0];
    a_light->onoff[0] = state;
    a_light->lightness[1] = a_light->lightness[0];
    a_light->lightness[0] = state ? 65535 : 0;
    light_update(a_light);
    return 0;
}

int light_model_gen_level_get(struct bt_mesh_model *model, s16_t *level, void *user_data)
{
    struct light_state *a_light = (struct light_state *)user_data;
    *level = a_light->level[0];
    return 0;
}

int light_model_gen_level_set(struct bt_mesh_model *model, s16_t  level, void *user_data)
{
    struct light_state *a_light = (struct light_state *)user_data;
    uint32_t temp = level + 32768;
    a_light->level[1] = a_light->level[0];
    a_light->level[0] = level;
    a_light->lightness[1] = a_light->lightness[0];
    a_light->lightness[0] = (255 * temp) >> 16;
    light_update(a_light);
    return 0;
}

int light_model_light_lightness_get(struct bt_mesh_model *model, u16_t *lightness, void *user_data)
{
    struct light_state *a_light = (struct light_state *)user_data;
    *lightness = a_light->lightness[0];
    return 0;
}

int light_model_light_lightness_set(struct bt_mesh_model *model, u16_t lightness, void *user_data)
{
    struct light_state *a_light = (struct light_state *)user_data;
    a_light->lightness[1] = a_light->lightness[0];
    a_light->lightness[0] = lightness;
    light_update(a_light);
    return 0;
}

int light_model_light_hsl_get(struct bt_mesh_model *model, u16_t *lightness, u16_t *hue, u16_t *saturation, void *user_data)
{
    struct light_state *a_light = (struct light_state *)user_data;
    *lightness  = a_light->lightness[0];
    *hue        = a_light->hue[0];
    *saturation = a_light->saturation[0];
    return 0;
}

static uint8_t Q_MULT(uint8_t a, uint8_t b)
{
    uint16_t t = (uint16_t)a * b;
    uint8_t r = t >> 8;
    if (t & 0x80) r++;
    return r;
}
#define CLIP(v) ((v) <= 0 ? 0 : ((v) >= 255 ? 255 : (v)))

static uint8_t hue2rgb(int16_t v1, int16_t v2, int16_t vH)
{
    if (vH < 0)     vH += 255;
    if (vH > 255)   vH -= 255;
    if (6 * vH < 255) return v1 + Q_MULT(v2 - v1, 6 * vH);
    if (2 * vH < 255) return v2;
    if (3 * vH < 255 * 2) return v1 + Q_MULT(v2 - v1, 255 * 4 - 6 * vH);
    return v1;
}

void hsl_to_rgb(u16_t H, u16_t S, u16_t L,
                u8_t *R, u8_t *G, u8_t *B)
{
    int16_t var_1, var_2;
    H >>= 8;
    S >>= 8;
    L >>= 8;
    if (0 == S)
    {
        *R = L;
        *G = L;
        *B = L;
        return;
    }
    
    if (L < 128) var_2 = Q_MULT(L, (255 + S));
    else         var_2 = (L + S) - Q_MULT(S, L);
    var_2 = CLIP(var_2);
    var_1 = 2 * L - var_2;    
    *R = hue2rgb(var_1, var_2, ((int16_t)H) + (255 / 3));
    *G = hue2rgb(var_1, var_2, H);
    *B = hue2rgb(var_1, var_2, ((int16_t)H) - (255 / 3));
}

void light_update(struct light_state *a_light)
{
    u8_t r, g, b;
    hsl_to_rgb(a_light->hue[0], a_light->saturation[0], a_light->lightness[0], &r, &g, &b);
#ifdef SIMULATION
    printf("=======\nLED %d => RGB: #%02X%02X%02X\n=======\n", 
               a_light->led_gpio_pin, r, g, b);
#else
    extern void set_led_color(uint8_t r, uint8_t g, uint8_t b);
    set_led_color(r, g, b);
#endif
}

int light_model_light_hsl_set(struct bt_mesh_model *model, u16_t  lightness, u16_t  hue, u16_t  saturation, void *user_data)
{
    struct light_state *a_light = (struct light_state *)user_data;

    a_light->lightness[1]     = a_light->lightness[0];
    a_light->hue[1]           = a_light->hue[0];
    a_light->saturation[1]    = a_light->saturation[0];
    a_light->lightness[0] = lightness;
    a_light->hue[0]       = hue;
    a_light->saturation[0]= saturation;    

    light_update(a_light);
    return 0;
}

#ifdef USE_OOB
static int output_number(bt_mesh_output_action_t action, u32_t number)
{
    BT_INFO("OOB Number %u", number);
    return 0;
}

static int output_string(const char *str)
{
    BT_INFO("OOB String %s", str);
    return 0;
}
#endif

static void prov_complete(u16_t net_idx, u16_t addr)
{
    BT_INFO("provisioning complete for net_idx 0x%04x addr 0x%04x",
                net_idx, addr);
    primary_addr = addr;
    primary_net_idx = net_idx;
}

static void prov_reset(void)
{
    bt_mesh_prov_enable((bt_mesh_prov_bearer_t)(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT));
}

#ifdef V2
static u8_t dev_uuid[16] = {0x22, 0};
const unsigned char addr[6] = {2,0,0,0,0,0};
#else
static u8_t dev_uuid[16] = MYNEWT_VAL(BLE_MESH_DEV_UUID);
const unsigned char addr[6] = {1,0,0,0,0,0};
#endif

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

#define TIANMAO

#ifdef TIANMAO
static uint8_t param[32] =
{
    0x92, 0x04, 0x00, 0x00, // product ID
    0x36, 0x00, 0x00, 0x00, // authentation length
    0xd7,0xf5,0x99,0x06,0xaa,0xf2,0x81,0xe6,0xe1,0x9e,0xb3,0x1f,0xa6,0xe3,0x7b,0x6d, // security (TEST DATA)
};
#else
static uint8_t param[32] = {0};
#endif


static void model_init()
{
    const static bd_addr_t addr_pb_adv  = {0x73, 0x2a, 0x4e, 0x19, 0x28, 0xC0};
    const static bd_addr_t addr_pb_gatt = {0x77, 0x33, 0xa3, 0x17, 0x2f, 0xC0};
#ifdef V2
    mesh_set_dev_name("hello-mesh-2");
#else
    mesh_set_dev_name("hello-mesh");
#endif
#ifdef TIANMAO    
    mesh_platform_config(MESH_PLT_PB_ADV, addr_pb_adv, param);
#else
    mesh_platform_config(MESH_PLT_PB_ADV, addr_pb_adv, NULL);
#endif
    mesh_platform_config(MESH_PLT_PB_GATT, addr_pb_gatt, NULL);

    init_pub();
    mesh_setup(0, &prov, &comp);
}

uint32_t setup_profile(void *data, void *user_data)
{
    model_init();
    return 0;
}
