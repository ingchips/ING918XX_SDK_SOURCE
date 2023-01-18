#ifndef __MESH_GENERIC_MODEL_H
#define __MESH_GENERIC_MODEL_H

#include "mesh/mesh_access.h"

#ifdef __cplusplus
extern "C"
{
#endif


#define MESH_GENERIC_ON_OFF_GET                                     0x8201u    
#define MESH_GENERIC_ON_OFF_SET                                     0x8202u
#define MESH_GENERIC_ON_OFF_SET_UNACKNOWLEDGED                      0x8203u
#define MESH_GENERIC_ON_OFF_STATUS                                  0x8204u

#define MESH_GENERIC_DEFAULT_TRANSITION_TIME_GET                    0x820Du    
#define MESH_GENERIC_DEFAULT_TRANSITION_TIME_SET                    0x820Eu
#define MESH_GENERIC_DEFAULT_TRANSITION_TIME_SET_UNACKNOWLEDGED     0x820Fu
#define MESH_GENERIC_DEFAULT_TRANSITION_TIME_STATUS                 0x8210u

#define MESH_GENERIC_LEVEL_GET                                      0x8205u    
#define MESH_GENERIC_LEVEL_SET                                      0x8206u
#define MESH_GENERIC_LEVEL_SET_UNACKNOWLEDGED                       0x8207u
#define MESH_GENERIC_LEVEL_STATUS                                   0x8208u
#define MESH_GENERIC_DELTA_SET                                      0x8209u
#define MESH_GENERIC_DELTA_SET_UNACKNOWLEDGED                       0x820Au
#define MESH_GENERIC_MOVE_SET                                       0x820Bu
#define MESH_GENERIC_MOVE_SET_UNACKNOWLEDGED                        0x820Cu

typedef struct {
    mesh_transition_t base_transition;

    uint8_t  current_value;
    uint8_t  target_value;
} mesh_transition_bool_t;

typedef struct {
    mesh_transition_t base_transition;

    int16_t current_value;
    int16_t initial_value;
    int16_t target_value;
    int16_t stepwise_value_increment;
} mesh_transition_int16_t;

typedef struct {
    mesh_transition_bool_t transition_data;          
} mesh_generic_on_off_state_t;

typedef struct {
    uint8_t  value;          
} mesh_generic_default_transition_time_state_t;

typedef struct {
    mesh_transition_int16_t transition_data;       
} mesh_generic_level_state_t;

////////////////////////////////////////////////////////////////////////////////
// for app api.

typedef struct HSL_VAL
{
    uint16_t lightness;
    uint16_t hue;
    uint16_t sa;
    uint8_t  TID;
    uint8_t  transtime;
    uint8_t  delay;
}hsl_val_t;

typedef struct bt_mesh_gen_onoff_srv_cb {
    int (*get)(mesh_model_t *model, uint8_t *state);
    int (*set)(mesh_model_t *model, uint8_t state);
    void* light_state;
}bt_mesh_gen_onoff_srv_cb_t;

typedef struct bt_mesh_gen_level_srv_cb {
    int (*get)(mesh_model_t *model, int16_t *level);
    int (*set)(mesh_model_t *model, int16_t level);
    void* light_state;
}bt_mesh_gen_level_srv_cb_t;

typedef struct bt_mesh_light_lightness_srv_cb {
    int (*get)(mesh_model_t *model, uint16_t *p_light ,uint16_t *light,uint8_t * remain);
    int (*set)(mesh_model_t *model, uint16_t light);
    void* light_state;
}bt_mesh_light_lightness_srv_cb_t;

typedef struct bt_mesh_light_hsl_srv_cb{
    int (*get)(mesh_model_t *model,uint16_t *hue,uint16_t* sa,uint16_t* lightness,uint8_t* remain );
    int (*set)(mesh_model_t *model,hsl_val_t *val);
    void* light_state;
}bt_mesh_light_hsl_srv_cb_t;


#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif
