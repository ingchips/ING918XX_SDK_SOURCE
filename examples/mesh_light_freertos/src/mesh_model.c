#include "mesh_model.h"


#define INGCHIPS_COMP_ID                            0x06AC
#define MESH_PRODUCT_ID                             0x0000
#define MESH_VERSION_ID                             0x0000

static mesh_publication_model_t     gen_onoff_publish;


#define get_light_state(model, srv_cb) (light_state_t *)((struct srv_cb *)model->model_data)->light_state

int light_model_gen_onoff_get(mesh_model_t *model, u8_t *state)
{
    light_state_t *a_light = get_light_state(model, bt_mesh_gen_onoff_srv_cb);
    *state = a_light->onoff[0];
    return 0;
}

int light_model_gen_onoff_set(mesh_model_t *model, u8_t state)
{
    struct light_state *a_light = get_light_state(model, bt_mesh_gen_onoff_srv_cb);
    a_light->onoff[1] = a_light->onoff[0];
    a_light->onoff[0] = state;
    a_light->lightness[1] = a_light->lightness[0];
    a_light->lightness[0] = state ? 65535 : 0;
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





static mesh_model_t sig_models[] = {
    BT_MESH_MODEL_GEN_ONOFF_SRV(&gen_onoff_srv_cb, &gen_onoff_publish),
};

/*
 * Root and Secondary Element Declarations
 */
static mesh_element_t elements[] = {
    BT_MESH_ELEM(0x0000, sig_models, BT_MESH_MODEL_NONE), //primary element.
};

static const struct bt_mesh_comp comp = {
    .cid = INGCHIPS_COMP_ID,
    .pid = MESH_PRODUCT_ID,
    .vid = MESH_VERSION_ID,

    .elem = elements,
    .elem_count = ARRAY_SIZE(elements),
};


static void mesh_setup(const struct bt_mesh_comp *a_comp){
    size_t i;
    for(i=0; i<a_comp->elem_count; i++){
        mesh_node_add_element(&a_comp->elem[i]);
    }
}

void mesh_model_init(void){
    mesh_setup(&comp);
}

