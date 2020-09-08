/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __MODEL_SRV_H__
#define __MODEL_SRV_H__
#include <stdint.h>
//#include "light_model.h"
typedef struct HSL_VAL
{
    uint16_t lightness;
    uint16_t hue;
    uint16_t sa;
    uint8_t  TID;
    uint8_t  transtime;
    uint8_t  delay;
}hsl_val_t;

struct bt_mesh_gen_onoff_srv_cb {
    int (*get)(struct bt_mesh_model *model, u8_t *state);
    int (*set)(struct bt_mesh_model *model, u8_t state);
    void* user_data;
};

extern const struct bt_mesh_model_op gen_onoff_srv_op[];

#define BT_MESH_MODEL_GEN_ONOFF_SRV(srv, pub)		\
	BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_SRV,	\
		      gen_onoff_srv_op, pub, srv)

struct bt_mesh_gen_level_srv_cb {
    int (*get)(struct bt_mesh_model *model, s16_t *level);
    int (*set)(struct bt_mesh_model *model, s16_t level);
    void* user_data;
};

extern const struct bt_mesh_model_op gen_level_srv_op[];

#define BT_MESH_MODEL_GEN_LEVEL_SRV(srv, pub)		\
	BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_LEVEL_SRV,	\
		      gen_level_srv_op, pub, srv)

struct bt_mesh_light_lightness_srv_cb {
    int (*get)(struct bt_mesh_model *model, u16_t *p_light ,u16_t *light,u8_t * remain);
    int (*set)(struct bt_mesh_model *model, u16_t light);
    void* user_data;
};

struct bt_mesh_light_hsl_srv_cb{
    int (*get)(struct bt_mesh_model *model,uint16_t *hue,uint16_t* sa,uint16_t* lightness,uint8_t* remain );
    int (*set)(struct bt_mesh_model *model,hsl_val_t *val);
    void* user_data;    
};

struct bt_mesh_light_ctl_temp_srv_cb{
    int (*get)(struct bt_mesh_model *model,uint16_t *temp , s16_t* deluv, uint16_t* tar_temp, uint16_t* tar_deluv, uint8_t* time);
    int (*set)(struct bt_mesh_model *model,uint16_t temp , uint16_t deluv, uint8_t TID, uint8_t transit, uint8_t delay);      
};

struct bt_mesh_light_ctl_srv_cb{
    int (*get)(struct bt_mesh_model *model,uint16_t *lightness,uint16_t *temp , uint16_t* tar_lightness, uint16_t* tar_temp, uint8_t* time);
    int (*set)(struct bt_mesh_model *model,uint16_t lightness , uint16_t temp, s16_t deluv,uint8_t TID, uint8_t transit, uint8_t delay);      
};

extern const struct bt_mesh_model_op light_lightness_srv_op[];

#define BT_MESH_MODEL_LIGHT_LIGHTNESS_SRV(srv, pub)		\
	BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV,	\
		      light_lightness_srv_op, pub, srv)


void bt_mesh_set_gen_onoff_srv_cb(struct bt_mesh_gen_onoff_srv_cb *gen_onoff_cb);
void bt_mesh_set_gen_level_srv_cb(struct bt_mesh_gen_level_srv_cb *gen_level_cb);
void bt_mesh_set_light_lightness_srv_cb(struct bt_mesh_light_lightness_srv_cb *light_lightness_cb);

extern const struct bt_mesh_model_op light_hsl_srv_op[];
#define BT_MESH_MODEL_LIGHT_HSL_SRV(srv,pub)    \
	BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_HSL_SRV,	\
		      light_hsl_srv_op, pub, srv)


extern const struct bt_mesh_model_op light_ctl_temp_srv_op[];
#define BT_MESH_MODEL_LIGHT_CTL_TEMP_SRV(srv,pub)   \
    BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV,    \
              light_ctl_temp_srv_op, pub, srv)
              
extern const struct bt_mesh_model_op light_ctl_srv_op[];
#define BT_MESH_MODEL_LIGHT_CTL_SRV(srv,pub)   \
    BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_CTL_SRV,    \
              light_ctl_srv_op, pub, srv)             
#endif
