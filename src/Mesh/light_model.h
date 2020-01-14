/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BT_MESH_LIGHT_MODEL_H
#define __BT_MESH_LIGHT_MODEL_H

#include "syscfg.h"
#include "mesh.h"

typedef struct HSL_VAL
{
    uint16_t lightness;
    uint16_t hue;
    uint16_t sa;
    uint8_t  TID;
    uint8_t  transtime;
    uint8_t  delay;
}hsl_val_t;

int light_model_gen_onoff_get(struct bt_mesh_model *model, u8_t *state);
int light_model_gen_onoff_set(struct bt_mesh_model *model, u8_t state);
int light_model_gen_level_get(struct bt_mesh_model *model, s16_t *level);
int light_model_gen_level_set(struct bt_mesh_model *model, s16_t level);
int light_model_light_lightness_get(struct bt_mesh_model *model, u16_t *p_lightness,u16_t *lightness,u8_t* remain);
int light_model_light_lightness_set(struct bt_mesh_model *model, u16_t lightness);
void light_gpio_init(void);

int light_model_light_HSL_get(struct bt_mesh_model *model, uint16_t *hue, uint16_t *sa, uint16_t *lightness,uint8_t *trans);
int light_model_light_HSL_set(struct bt_mesh_model *model,hsl_val_t* val);

//light ctl temperature
int light_model_ctl_temp_get(struct bt_mesh_model *model,uint16_t *temp , s16_t* deluv, uint16_t* tar_temp, s16_t* tar_deluv, uint8_t* time);
int light_model_ctl_temp_set(struct bt_mesh_model *model,uint16_t temp ,  s16_t deluv, uint8_t TID, uint8_t transit, uint8_t delay);

//light ctl
int light_model_ctl_get(struct bt_mesh_model *model,uint16_t *lightness,uint16_t *temp , uint16_t* tar_lightness, uint16_t* tar_temp, uint8_t* time);
int light_model_ctl_set(struct bt_mesh_model *model,uint16_t lightness , uint16_t temp, s16_t deluv,uint8_t TID, uint8_t transit, uint8_t delay);

int unbind_light_mode_run(uint32_t duration);
int light_provsioned_complete();
#endif
