/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/* Bluetooth: Mesh Generic OnOff, Generic Level, Lighting & Vendor Models
 *
 * Copyright (c) 2018 Vikrant More
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mesh_api.h"
#include "access.h"
#include "model_srv.h"
#include "mesh_priv.h"
#include "mesh_common.h"
#include "device_composition.h"
#include "state_binding.h"
#include "transition.h"
#include "profile.h"
#include "os_mbuf.h"
#include "mesh_non_volatile_memory.h"
#include "cfg_cli.h"
#include "platform_api.h"

#define NO_TRANSTION_STATUS    (1)   //dengyiyunb 1: not report the transition information in status ,and set the target value in status msg.  0: report the transition in status msg.

/* Model Operation Codes */

#define save_on_flash(b)
#define hal_gpio_write(a,b)

#define DEFAULT_MAX_TEMP 0x4E20
#define DEFAULT_MIN_TEMP 0x0320

uint8_t g_gen_onoff_state = 0;
int16_t g_gen_level_state = 0;
uint32_t GWW, GCW;

static struct bt_mesh_cfg_srv cfg_srv = {
    .relay = BT_MESH_RELAY_DISABLED,
    .beacon = BT_MESH_BEACON_ENABLED,

#if defined(CONFIG_BT_MESH_FRIEND)
    .frnd = BT_MESH_FRIEND_ENABLED,
#else
    .frnd = BT_MESH_FRIEND_NOT_SUPPORTED,
#endif
    .gatt_proxy = BT_MESH_GATT_PROXY_ENABLED,
    .default_ttl = 7,
    /* 2 transmissions with 20ms interval */
    .net_transmit = BT_MESH_TRANSMIT(2, 20),
    /* 3 transmissions with 20ms interval */
    .relay_retransmit = BT_MESH_TRANSMIT(3, 20),
};

static struct bt_mesh_health_srv health_srv = {
};

static struct bt_mesh_model_pub health_pub;

static struct bt_mesh_model_pub gen_onoff_srv_pub_root;
static struct bt_mesh_model_pub gen_onoff_cli_pub_root;
static struct bt_mesh_model_pub gen_level_srv_pub_root;
static struct bt_mesh_model_pub gen_level_cli_pub_root;
static struct bt_mesh_model_pub gen_def_trans_time_srv_pub;
static struct bt_mesh_model_pub gen_def_trans_time_cli_pub;
static struct bt_mesh_model_pub gen_power_onoff_srv_pub;
static struct bt_mesh_model_pub gen_power_onoff_cli_pub;
static struct bt_mesh_model_pub light_lightness_srv_pub;
static struct bt_mesh_model_pub light_lightness_cli_pub;
static struct bt_mesh_model_pub light_ctl_srv_pub;
static struct bt_mesh_model_pub vnd_pub;

static struct os_mbuf *bt_mesh_pub_msg_health_pub;
static struct os_mbuf *bt_mesh_pub_msg_gen_onoff_srv_pub_root;
static struct os_mbuf *bt_mesh_pub_msg_gen_level_srv_pub_root;
static struct os_mbuf *bt_mesh_pub_msg_gen_def_trans_time_srv_pub;
static struct os_mbuf *bt_mesh_pub_msg_gen_power_onoff_srv_pub;
static struct os_mbuf *bt_mesh_pub_msg_light_lightness_srv_pub;
static struct os_mbuf *bt_mesh_pub_msg_light_ctl_srv_pub;
static struct os_mbuf *bt_mesh_pub_msg_vnd_pub;

/* Definitions of models user data (Start) */
struct generic_onoff_state gen_onoff_srv_root_user_data = {.transition = &lightness_transition};
struct generic_level_state gen_level_srv_root_user_data = {.transition = &lightness_transition};
static struct gen_def_trans_time_state gen_def_trans_time_srv_user_data;
static struct generic_onpowerup_state gen_power_onoff_srv_user_data;
struct light_lightness_state light_lightness_srv_user_data = {.transition = &lightness_transition};
struct light_ctl_state light_ctl_srv_user_data = {.transition = &lightness_transition};
struct generic_level_state gen_level_srv_s0_user_data = {.transition = &temp_transition};
static struct vendor_state vnd_user_data;
static struct bt_mesh_elem elements[];

#define M_BT_DBG dbg_printf("%s\r\n", __func__)

/* message handlers (Start) */

/* Generic OnOff Server message handlers */
static void gen_onoff_get(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct os_mbuf *buf)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(2 + 3 + 4);
    struct generic_onoff_state *state = model->user_data;

    M_BT_DBG;

    bt_mesh_model_msg_init(msg, OP_GEN_ONOFF_STATUS);
#if (NO_TRANSTION_STATUS==0)
    net_buf_simple_add_u8(msg, state->onoff);
    if (state->transition->counter) {
        calculate_rt(state->transition);
        net_buf_simple_add_u8(msg, state->target_onoff);
        net_buf_simple_add_u8(msg, state->transition->rt);
    }
#else
    net_buf_simple_add_u8(msg,state->target_onoff);
#endif

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        printk("Unable to send GEN_ONOFF_SRV Status response\n");
    }

    os_mbuf_free_chain(msg);
}

void gen_onoff_publish(struct bt_mesh_model *model)
{
    int err;
    struct os_mbuf *msg = model->pub->msg;
    struct generic_onoff_state *state = model->user_data;

    M_BT_DBG;

    if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }

    bt_mesh_model_msg_init(msg, OP_GEN_ONOFF_STATUS);
    net_buf_simple_add_u8(msg, state->onoff);
    if (state->transition->counter) {
        calculate_rt(state->transition);
        net_buf_simple_add_u8(msg, state->target_onoff);
        net_buf_simple_add_u8(msg, state->transition->rt);
    }

    err = bt_mesh_model_publish(model);
    if (err) {
        printk("bt_mesh_model_publish err %d\n", err);
    }
}

static void gen_onoff_set_unack(struct bt_mesh_model *model,
                struct bt_mesh_msg_ctx *ctx,
                struct os_mbuf *buf)
{
    u8_t tid, onoff, tt, delay;
    s64_t now;
    struct generic_onoff_state *state = model->user_data;

    M_BT_DBG;

    onoff = net_buf_simple_pull_u8(buf);
    tid = net_buf_simple_pull_u8(buf);

    if (onoff > STATE_ON) {
        return;
    }

    now = k_uptime_get();
    if (state->last_tid == tid &&
        state->last_src_addr == ctx->addr &&
        state->last_dst_addr == ctx->recv_dst &&
        (now - state->last_msg_timestamp <= K_SECONDS(6))) {
        return;
    }

    switch (buf->om_len) {
    case 0x00:    /* No optional fields are available */
        tt = default_tt;
        delay = 0;
        break;
    case 0x02:    /* Optional fields are available */
        tt = net_buf_simple_pull_u8(buf);
        if ((tt & 0x3F) == 0x3F) {
            return;
        }

        delay = net_buf_simple_pull_u8(buf);
        break;
    default:
        return;
    }

    *ptr_counter = 0;
    ble_npl_callout_stop(ptr_timer);

    state->last_tid = tid;
    state->last_src_addr = ctx->addr;
    state->last_dst_addr = ctx->recv_dst;
    state->last_msg_timestamp = now;
    state->target_onoff = onoff;

    if (state->target_onoff != state->onoff) {
        onoff_tt_values(state, tt, delay);
        dbg_printf("tt 3 0x%p \n",state->transition);
    } else {
        gen_onoff_publish(model);
        return;
    }

    /* For Instantaneous Transition */
    if (state->transition->counter == 0) {
        state->onoff = state->target_onoff;
    }

    state->transition->just_started = true;
    gen_onoff_publish(model);
    onoff_handler(state);
}

static void gen_onoff_set(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct os_mbuf *buf)
{
    u8_t tid, onoff, tt, delay;
    s64_t now;
    struct generic_onoff_state *state = model->user_data;

    M_BT_DBG;

    onoff = net_buf_simple_pull_u8(buf);
    tid = net_buf_simple_pull_u8(buf);
    dbg_printf("onoff 0x%x\n",onoff);
    if (onoff > STATE_ON) {
        return;
    }

    now = k_uptime_get();
    if (state->last_tid == tid &&
        state->last_src_addr == ctx->addr &&
        state->last_dst_addr == ctx->recv_dst &&
        (now - state->last_msg_timestamp <= K_SECONDS(6))) {
        dbg_printf("less < 6s\n");
        gen_onoff_get(model, ctx, buf);
        return;
    }

    switch (buf->om_len) {
    case 0x00:    /* No optional fields are available */
        tt = default_tt;
        delay = 0;
        break;
    case 0x02:    /* Optional fields are available */
        tt = net_buf_simple_pull_u8(buf);
        if ((tt & 0x3F) == 0x3F) {
            return;
        }

        delay = net_buf_simple_pull_u8(buf);
        break;
    default:
        return;
    }

    *ptr_counter = 0;
    ble_npl_callout_stop(ptr_timer);

    state->last_tid = tid;
    state->last_src_addr = ctx->addr;
    state->last_dst_addr = ctx->recv_dst;
    state->last_msg_timestamp = now;
    state->target_onoff = onoff;
    dbg_printf("tt value 0x%x 0x%x state 0x%x target stat 0x%x\n",tt,state->transition,state->onoff,state->target_onoff);
    if (state->target_onoff != state->onoff) {
        onoff_tt_values(state, tt, delay);
    } else {
        gen_onoff_get(model, ctx, buf);
        gen_onoff_publish(model);
        return;
    }

    /* For Instantaneous Transition */
    if (state->transition->counter == 0) {
        state->onoff = state->target_onoff;
    }

    state->transition->just_started = true;
    gen_onoff_get(model, ctx, buf);
    gen_onoff_publish(model);
    dbg_printf("stat 0x%x \n",state);
    onoff_handler(state);
}

/* Generic OnOff Client message handlers */
static void gen_onoff_status(struct bt_mesh_model *model,
                 struct bt_mesh_msg_ctx *ctx,
                 struct os_mbuf *buf)
{
    M_BT_DBG;

    printk("Acknownledgement from GEN_ONOFF_SRV\n");
    printk("Present OnOff = %02x\n", net_buf_simple_pull_u8(buf));

    if (buf->om_len == 2) {
        printk("Target OnOff = %02x\n", net_buf_simple_pull_u8(buf));
        printk("Remaining Time = %02x\n", net_buf_simple_pull_u8(buf));
    }
}

/* Generic Level Server message handlers */
static void gen_level_get(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct os_mbuf *buf)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(2 + 5 + 4);
    struct generic_level_state *state = model->user_data;

    M_BT_DBG;

    bt_mesh_model_msg_init(msg, OP_GEN_LEVEL_STATUS);
#if (NO_TRANSTION_STATUS==0)
    net_buf_simple_add_le16(msg, state->level);

    if (state->transition->counter) {
        calculate_rt(state->transition);
        net_buf_simple_add_le16(msg, state->target_level);
        net_buf_simple_add_u8(msg, state->transition->rt);
    }
#else
    net_buf_simple_add_le16(msg, state->target_level);
#endif
    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        printk("Unable to send GEN_LEVEL_SRV Status response\n");
    }

    os_mbuf_free_chain(msg);
}

void gen_level_publish(struct bt_mesh_model *model)
{
    int err;
    struct os_mbuf *msg = model->pub->msg;
    struct generic_level_state *state = model->user_data;

    M_BT_DBG;

    if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }

    bt_mesh_model_msg_init(msg, OP_GEN_LEVEL_STATUS);
    net_buf_simple_add_le16(msg, state->level);

    if (state->transition->counter) {
        calculate_rt(state->transition);
        net_buf_simple_add_le16(msg, state->target_level);
        net_buf_simple_add_u8(msg, state->transition->rt);
    }

    err = bt_mesh_model_publish(model);
    if (err) {
        printk("bt_mesh_model_publish err %d\n", err);
    }
}

static void gen_level_set_unack(struct bt_mesh_model *model,
                struct bt_mesh_msg_ctx *ctx,
                struct os_mbuf *buf)
{
    u8_t tid, tt, delay;
    s16_t level;
    s64_t now;
    struct generic_level_state *state = model->user_data;

    M_BT_DBG;

    level = (s16_t) net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    now = k_uptime_get();
    if (state->last_tid == tid &&
        state->last_src_addr == ctx->addr &&
        state->last_dst_addr == ctx->recv_dst &&
        (now - state->last_msg_timestamp <= K_SECONDS(6))) {
        return;
    }

    switch (buf->om_len) {
    case 0x00:    /* No optional fields are available */
        tt = default_tt;
        delay = 0;
        break;
    case 0x02:    /* Optional fields are available */
        tt = net_buf_simple_pull_u8(buf);
        if ((tt & 0x3F) == 0x3F) {
            return;
        }

        delay = net_buf_simple_pull_u8(buf);
        break;
    default:
        return;
    }

    *ptr_counter = 0;
    ble_npl_callout_stop(ptr_timer);

    state->last_tid = tid;
    state->last_src_addr = ctx->addr;
    state->last_dst_addr = ctx->recv_dst;
    state->last_msg_timestamp = now;
    state->target_level = level;

    if (state->target_level != state->level) {
        level_tt_values(state, tt, delay);
    } else {
        gen_level_publish(model);
        return;
    }

    /* For Instantaneous Transition */
    if (state->transition->counter == 0) {
        state->level = state->target_level;
    }

    state->transition->just_started = true;
    gen_level_publish(model);

    if (bt_mesh_model_elem(model)->addr == elements[0].addr) {
        /* Root element */
        transition_type = LEVEL_TT;
        level_lightness_handler(state);
    } else if (bt_mesh_model_elem(model)->addr == elements[1].addr) {
        /* Secondary element */
        transition_type = LEVEL_TEMP_TT;
        level_temp_handler(state);
    }
}

static void gen_level_set(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct os_mbuf *buf)
{
    u8_t tid, tt, delay;
    s16_t level;
    s64_t now;
    struct generic_level_state *state = model->user_data;

    M_BT_DBG;

    level = (s16_t) net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);
    now = k_uptime_get();
    if (state->last_tid == tid &&
        state->last_src_addr == ctx->addr &&
        state->last_dst_addr == ctx->recv_dst &&
        (now - state->last_msg_timestamp <= K_SECONDS(6))) {
        gen_level_get(model, ctx, buf);
        return;
    }
    switch (buf->om_len) {
    case 0x00:    /* No optional fields are available */
        tt = default_tt;
        delay = 0;
        break;
    case 0x02:    /* Optional fields are available */
        tt = net_buf_simple_pull_u8(buf);
        if ((tt & 0x3F) == 0x3F) {
            return;
        }

        delay = net_buf_simple_pull_u8(buf);
        break;
    default:
        return;
    }
    *ptr_counter = 0;
    ble_npl_callout_stop(ptr_timer);

    state->last_tid = tid;
    state->last_src_addr = ctx->addr;
    state->last_dst_addr = ctx->recv_dst;
    state->last_msg_timestamp = now;
    state->target_level = level;

    if (state->target_level != state->level) {
        level_tt_values(state, tt, delay);
    } else {
        gen_level_get(model, ctx, buf);
        gen_level_publish(model);
        return;
    }

    /* For Instantaneous Transition */
    if (state->transition->counter == 0) {
        state->level = state->target_level;
    }

    state->transition->just_started = true;
    gen_level_get(model, ctx, buf);
    gen_level_publish(model);

    if (bt_mesh_model_elem(model)->addr == elements[0].addr) {
        /* Root element */
        transition_type = LEVEL_TT;
        level_lightness_handler(state);
    } else if (bt_mesh_model_elem(model)->addr == elements[1].addr) {
        /* Secondary element */
        transition_type = LEVEL_TEMP_TT;
        level_temp_handler(state);
    }
}

static void gen_delta_set_unack(struct bt_mesh_model *model,
                struct bt_mesh_msg_ctx *ctx,
                struct os_mbuf *buf)
{
    u8_t tid, tt, delay;
    s32_t tmp32, delta;
    s64_t now;
    struct generic_level_state *state = model->user_data;

    M_BT_DBG;

    delta = (s32_t) net_buf_simple_pull_le32(buf);
    tid = net_buf_simple_pull_u8(buf);

    now = k_uptime_get();
    if (state->last_tid == tid &&
        state->last_src_addr == ctx->addr &&
        state->last_dst_addr == ctx->recv_dst &&
        (now - state->last_msg_timestamp <= K_SECONDS(6))) {

        if (state->last_delta == delta) {
            return;
        }
        tmp32 = state->last_level + delta;

    } else {
        state->last_level = state->level;
        tmp32 = state->level + delta;
    }

    switch (buf->om_len) {
    case 0x00:    /* No optional fields are available */
        tt = default_tt;
        delay = 0;
        break;
    case 0x02:    /* Optional fields are available */
        tt = net_buf_simple_pull_u8(buf);
        if ((tt & 0x3F) == 0x3F) {
            return;
        }

        delay = net_buf_simple_pull_u8(buf);
        break;
    default:
        return;
    }

    *ptr_counter = 0;
    ble_npl_callout_stop(ptr_timer);

    state->last_tid = tid;
    state->last_src_addr = ctx->addr;
    state->last_dst_addr = ctx->recv_dst;
    state->last_msg_timestamp = now;
    state->last_delta = delta;

    if (tmp32 < INT16_MIN) {
        tmp32 = INT16_MIN;
    } else if (tmp32 > INT16_MAX) {
        tmp32 = INT16_MAX;
    }

    state->target_level = tmp32;

    if (state->target_level != state->level) {
        level_tt_values(state, tt, delay);
    } else {
        gen_level_publish(model);
        return;
    }

    /* For Instantaneous Transition */
    if (state->transition->counter == 0) {
        state->level = state->target_level;
    }

    state->transition->just_started = true;
    gen_level_publish(model);

    if (bt_mesh_model_elem(model)->addr == elements[0].addr) {
        /* Root element */
        transition_type = LEVEL_TT_DELTA;
        level_lightness_handler(state);
    } else if (bt_mesh_model_elem(model)->addr ==
           elements[1].addr) {
        /* Secondary element */
        transition_type = LEVEL_TEMP_TT_DELTA;
        level_temp_handler(state);
    }
}

static void gen_delta_set(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct os_mbuf *buf)
{
    u8_t tid, tt, delay;
    s32_t tmp32, delta;
    s64_t now;
    struct generic_level_state *state = model->user_data;

    M_BT_DBG;

    delta = (s32_t) net_buf_simple_pull_le32(buf);
    tid = net_buf_simple_pull_u8(buf);

    now = k_uptime_get();
    if (state->last_tid == tid &&
        state->last_src_addr == ctx->addr &&
        state->last_dst_addr == ctx->recv_dst &&
        (now - state->last_msg_timestamp <= K_SECONDS(6))) {

        if (state->last_delta == delta) {
            gen_level_get(model, ctx, buf);
            return;
        }
        tmp32 = state->last_level + delta;

    } else {
        state->last_level = state->level;
        tmp32 = state->level + delta;
    }

    switch (buf->om_len) {
    case 0x00:    /* No optional fields are available */
        tt = default_tt;
        delay = 0;
        break;
    case 0x02:    /* Optional fields are available */
        tt = net_buf_simple_pull_u8(buf);
        if ((tt & 0x3F) == 0x3F) {
            return;
        }

        delay = net_buf_simple_pull_u8(buf);
        break;
    default:
        return;
    }

    *ptr_counter = 0;
    ble_npl_callout_stop(ptr_timer);

    state->last_tid = tid;
    state->last_src_addr = ctx->addr;
    state->last_dst_addr = ctx->recv_dst;
    state->last_msg_timestamp = now;
    state->last_delta = delta;

    if (tmp32 < INT16_MIN) {
        tmp32 = INT16_MIN;
    } else if (tmp32 > INT16_MAX) {
        tmp32 = INT16_MAX;
    }

    state->target_level = tmp32;

    if (state->target_level != state->level) {
        level_tt_values(state, tt, delay);
    } else {
        gen_level_get(model, ctx, buf);
        gen_level_publish(model);
        return;
    }

    /* For Instantaneous Transition */
    if (state->transition->counter == 0) {
        state->level = state->target_level;
    }

    state->transition->just_started = true;
    gen_level_get(model, ctx, buf);
    gen_level_publish(model);

    if (bt_mesh_model_elem(model)->addr == elements[0].addr) {
        /* Root element */
        transition_type = LEVEL_TT_DELTA;
        level_lightness_handler(state);
    } else if (bt_mesh_model_elem(model)->addr == elements[1].addr) {
        /* Secondary element */
        transition_type = LEVEL_TEMP_TT_DELTA;
        level_temp_handler(state);
    }
}

static void gen_level_move_get(struct bt_mesh_model *model,
                   struct bt_mesh_msg_ctx *ctx,
                   struct os_mbuf *buf)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(2 + 5 + 4);
    struct generic_level_state *state = model->user_data;

    M_BT_DBG;

    bt_mesh_model_msg_init(msg, OP_GEN_LEVEL_STATUS);
    net_buf_simple_add_le16(msg, state->level);

    if (state->transition->counter) {
        if (state->last_delta < 0) {
            net_buf_simple_add_le16(msg, (uint16_t)INT16_MIN);
        } else { /* 0 should not be possible */
            net_buf_simple_add_le16(msg, (uint16_t)INT16_MAX);
        }

        net_buf_simple_add_u8(msg, UNKNOWN_VALUE);
    }

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        printk("Unable to send GEN_LEVEL_SRV Status response\n");
    }

    os_mbuf_free_chain(msg);
}

static void gen_level_move_publish(struct bt_mesh_model *model)
{
    int err;
    struct os_mbuf *msg = model->pub->msg;
    struct generic_level_state *state = model->user_data;

    M_BT_DBG;

    if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }

    bt_mesh_model_msg_init(msg, OP_GEN_LEVEL_STATUS);
    net_buf_simple_add_le16(msg, state->level);

    if (state->transition->counter) {
        if (state->last_delta < 0) {
            net_buf_simple_add_le16(msg, (uint16_t)INT16_MIN);
        } else { /* 0 should not be possible */
            net_buf_simple_add_le16(msg, (uint16_t)INT16_MAX);
        }

        net_buf_simple_add_u8(msg, UNKNOWN_VALUE);
    }

    err = bt_mesh_model_publish(model);
    if (err) {
        printk("bt_mesh_model_publish err %d\n", err);
    }
}

static void gen_move_set_unack(struct bt_mesh_model *model,
                   struct bt_mesh_msg_ctx *ctx,
                   struct os_mbuf *buf)
{
    u8_t tid, tt, delay;
    s16_t delta;
    s32_t tmp32;
    s64_t now;
    struct generic_level_state *state = model->user_data;

    M_BT_DBG;

    delta = (s16_t) net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    now = k_uptime_get();
    if (state->last_tid == tid &&
        state->last_src_addr == ctx->addr &&
        state->last_dst_addr == ctx->recv_dst &&
        (now - state->last_msg_timestamp <= K_SECONDS(6))) {
        return;
    }

    switch (buf->om_len) {
    case 0x00:    /* No optional fields are available */
        tt = default_tt;
        delay = 0;
        break;
    case 0x02:    /* Optional fields are available */
        tt = net_buf_simple_pull_u8(buf);
        if ((tt & 0x3F) == 0x3F) {
            return;
        }

        delay = net_buf_simple_pull_u8(buf);
        break;
    default:
        return;
    }

    *ptr_counter = 0;
    ble_npl_callout_stop(ptr_timer);

    state->last_tid = tid;
    state->last_src_addr = ctx->addr;
    state->last_dst_addr = ctx->recv_dst;
    state->last_msg_timestamp = now;
    state->last_delta = delta;

    tmp32 = state->level + delta;
    if (tmp32 < INT16_MIN) {
        tmp32 = INT16_MIN;
    } else if (tmp32 > INT16_MAX) {
        tmp32 = INT16_MAX;
    }

    state->target_level = tmp32;

    if (state->target_level != state->level) {
        level_tt_values(state, tt, delay);
    } else {
        gen_level_move_publish(model);
        return;
    }

    if (state->transition->counter == 0) {
        return;
    }

    state->transition->just_started = true;
    gen_level_move_publish(model);

    if (bt_mesh_model_elem(model)->addr == elements[0].addr) {
        /* Root element */
        transition_type = LEVEL_TT_MOVE;
        level_lightness_handler(state);
    } else if (bt_mesh_model_elem(model)->addr == elements[1].addr) {
        /* Secondary element */
        transition_type = LEVEL_TEMP_TT_MOVE;
        level_temp_handler(state);
    }
}

static void gen_move_set(struct bt_mesh_model *model,
             struct bt_mesh_msg_ctx *ctx,
             struct os_mbuf *buf)
{
    u8_t tid, tt, delay;
    s16_t delta;
    s32_t tmp32;
    s64_t now;
    struct generic_level_state *state = model->user_data;

    M_BT_DBG;

    delta = (s16_t) net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    now = k_uptime_get();
    if (state->last_tid == tid &&
        state->last_src_addr == ctx->addr &&
        state->last_dst_addr == ctx->recv_dst &&
        (now - state->last_msg_timestamp <= K_SECONDS(6))) {
        gen_level_move_get(model, ctx, buf);
        return;
    }

    switch (buf->om_len) {
    case 0x00:    /* No optional fields are available */
        tt = default_tt;
        delay = 0;
        break;
    case 0x02:    /* Optional fields are available */
        tt = net_buf_simple_pull_u8(buf);
        if ((tt & 0x3F) == 0x3F) {
            return;
        }

        delay = net_buf_simple_pull_u8(buf);
        break;
    default:
        return;
    }

    *ptr_counter = 0;
    ble_npl_callout_stop(ptr_timer);

    state->last_tid = tid;
    state->last_src_addr = ctx->addr;
    state->last_dst_addr = ctx->recv_dst;
    state->last_msg_timestamp = now;
    state->last_delta = delta;

    tmp32 = state->level + delta;
    if (tmp32 < INT16_MIN) {
        tmp32 = INT16_MIN;
    } else if (tmp32 > INT16_MAX) {
        tmp32 = INT16_MAX;
    }

    state->target_level = tmp32;

    if (state->target_level != state->level) {
        level_tt_values(state, tt, delay);
    } else {
        gen_level_move_get(model, ctx, buf);
        gen_level_move_publish(model);
        return;
    }

    if (state->transition->counter == 0) {
        return;
    }

    state->transition->just_started = true;
    gen_level_move_get(model, ctx, buf);
    gen_level_move_publish(model);

    if (bt_mesh_model_elem(model)->addr == elements[0].addr) {
        /* Root element */
        transition_type = LEVEL_TT_MOVE;
        level_lightness_handler(state);
    } else if (bt_mesh_model_elem(model)->addr == elements[1].addr) {
        /* Secondary element */
        transition_type = LEVEL_TEMP_TT_MOVE;
        level_temp_handler(state);
    }
}

/* Generic Level Client message handlers */
static void gen_level_status(struct bt_mesh_model *model,
                 struct bt_mesh_msg_ctx *ctx,
                 struct os_mbuf *buf)
{
    M_BT_DBG;

    printk("Acknownledgement from GEN_LEVEL_SRV\n");
    printk("Present Level = %04x\n", net_buf_simple_pull_le16(buf));

    if (buf->om_len == 3) {
        printk("Target Level = %04x\n", net_buf_simple_pull_le16(buf));
        printk("Remaining Time = %02x\n", net_buf_simple_pull_u8(buf));
    }
}

/* Generic Default Transition Time Server message handlers */
static void gen_def_trans_time_get(struct bt_mesh_model *model,
                   struct bt_mesh_msg_ctx *ctx,
                   struct os_mbuf *buf)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(2 + 1 + 4);
    struct gen_def_trans_time_state *state = model->user_data;

    M_BT_DBG;

    bt_mesh_model_msg_init(msg, OP_GEN_DEF_TRANS_TIME_STATUS);
    net_buf_simple_add_u8(msg, state->tt);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        printk("Unable to send GEN_DEF_TT_SRV Status response\n");
    }

    os_mbuf_free_chain(msg);
}

static void gen_def_trans_time_publish(struct bt_mesh_model *model)
{
    int err;
    struct os_mbuf *msg = model->pub->msg;
    struct gen_def_trans_time_state *state = model->user_data;

    M_BT_DBG;

    if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }

    bt_mesh_model_msg_init(msg, OP_GEN_DEF_TRANS_TIME_STATUS);
    net_buf_simple_add_u8(msg, state->tt);

    err = bt_mesh_model_publish(model);
    if (err) {
        printk("bt_mesh_model_publish err %d\n", err);
    }
}

static bool gen_def_trans_time_setunack(struct bt_mesh_model *model,
                    struct bt_mesh_msg_ctx *ctx,
                    struct os_mbuf *buf)
{
    u8_t tt;
    struct gen_def_trans_time_state *state = model->user_data;

    M_BT_DBG;

    tt = net_buf_simple_pull_u8(buf);

    /* Here, Model specification is silent about tid implementation */

    if ((tt & 0x3F) == 0x3F) {
        return false;
    }

    if (state->tt != tt) {
        state->tt = tt;
        default_tt = tt;

        save_on_flash(GEN_DEF_TRANS_TIME_STATE);
    }

    return true;
}

static void gen_def_trans_time_set_unack(struct bt_mesh_model *model,
                     struct bt_mesh_msg_ctx *ctx,
                     struct os_mbuf *buf)
{
    M_BT_DBG;

    if (gen_def_trans_time_setunack(model, ctx, buf) == true) {
        gen_def_trans_time_publish(model);
    }
}

static void gen_def_trans_time_set(struct bt_mesh_model *model,
                   struct bt_mesh_msg_ctx *ctx,
                   struct os_mbuf *buf)
{
    M_BT_DBG;

    if (gen_def_trans_time_setunack(model, ctx, buf) == true) {
        gen_def_trans_time_get(model, ctx, buf);
        gen_def_trans_time_publish(model);
    }
}

/* Generic Default Transition Time Client message handlers */
static void gen_def_trans_time_status(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct os_mbuf *buf)
{
    M_BT_DBG;

    printk("Acknownledgement from GEN_DEF_TT_SRV\n");
    printk("Transition Time = %02x\n", net_buf_simple_pull_u8(buf));
}

/* Generic Power OnOff Server message handlers */
static void gen_onpowerup_get(struct bt_mesh_model *model,
                  struct bt_mesh_msg_ctx *ctx,
                  struct os_mbuf *buf)
{
    M_BT_DBG;

    struct os_mbuf *msg = NET_BUF_SIMPLE(2 + 1 + 4);
    struct generic_onpowerup_state *state = model->user_data;

    bt_mesh_model_msg_init(msg, OP_GEN_ONPOWERUP_STATUS);
    net_buf_simple_add_u8(msg, state->onpowerup);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        printk("Unable to send GEN_POWER_ONOFF_SRV Status response\n");
    }

    os_mbuf_free_chain(msg);
}

/* Generic Power OnOff Client message handlers */
static void gen_onpowerup_status(struct bt_mesh_model *model,
                 struct bt_mesh_msg_ctx *ctx,
                 struct os_mbuf *buf)
{
    M_BT_DBG;

    printk("Acknownledgement from GEN_POWER_ONOFF_SRV\n");
    printk("OnPowerUp = %02x\n", net_buf_simple_pull_u8(buf));
}

/* Generic Power OnOff Setup Server message handlers */

static void gen_onpowerup_publish(struct bt_mesh_model *model)
{
    int err;
    struct os_mbuf *msg = model->pub->msg;
    struct generic_onpowerup_state *state = model->user_data;

    M_BT_DBG;

    if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }

    bt_mesh_model_msg_init(msg, OP_GEN_ONPOWERUP_STATUS);
    net_buf_simple_add_u8(msg, state->onpowerup);

    err = bt_mesh_model_publish(model);
    if (err) {
        printk("bt_mesh_model_publish err %d\n", err);
    }
}

static bool gen_onpowerup_setunack(struct bt_mesh_model *model,
                struct bt_mesh_msg_ctx *ctx,
                struct os_mbuf *buf)
{
    u8_t onpowerup;
    struct generic_onpowerup_state *state = model->user_data;

    M_BT_DBG;

    onpowerup = net_buf_simple_pull_u8(buf);

    /* Here, Model specification is silent about tid implementation */

    if (onpowerup > STATE_RESTORE) {
        return false;
    }

    if (state->onpowerup != onpowerup) {
        state->onpowerup = onpowerup;

        save_on_flash(GEN_ONPOWERUP_STATE);
    }

    return true;
}

static void gen_onpowerup_set_unack(struct bt_mesh_model *model,
                    struct bt_mesh_msg_ctx *ctx,
                    struct os_mbuf *buf)
{
    M_BT_DBG;

    if (gen_onpowerup_setunack(model, ctx, buf) == true) {
        gen_onpowerup_publish(model);
    }
}

static void gen_onpowerup_set(struct bt_mesh_model *model,
                  struct bt_mesh_msg_ctx *ctx,
                  struct os_mbuf *buf)
{
    M_BT_DBG;

    if (gen_onpowerup_setunack(model, ctx, buf) == true) {
        gen_onpowerup_get(model, ctx, buf);
        gen_onpowerup_publish(model);
    }
}

/* Vendor Model message handlers*/
static void vnd_get(struct bt_mesh_model *model,
            struct bt_mesh_msg_ctx *ctx,
            struct os_mbuf *buf)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(3 + 6 + 4);
    struct vendor_state *state = model->user_data;

    M_BT_DBG;

    /* This is dummy response for demo purpose */
    state->response = 0xA578FEB3;

    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_3(0x04, INGCHIPS_COMP_ID));
    net_buf_simple_add_le16(msg, state->current);
    net_buf_simple_add_le32(msg, state->response);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        printk("Unable to send VENDOR Status response\n");
    }

    os_mbuf_free_chain(msg);
}

static void vnd_set_unack(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct os_mbuf *buf)
{
    u8_t tid;
    int current;
    s64_t now;
    struct vendor_state *state = model->user_data;

    M_BT_DBG;

    current = net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    now = k_uptime_get();
    if (state->last_tid == tid &&
        state->last_src_addr == ctx->addr &&
        state->last_dst_addr == ctx->recv_dst &&
        (now - state->last_msg_timestamp <= K_SECONDS(6))) {
        return;
    }

    state->last_tid = tid;
    state->last_src_addr = ctx->addr;
    state->last_dst_addr = ctx->recv_dst;
    state->last_msg_timestamp = now;
    state->current = current;

    printk("Vendor model message = %04x\n", state->current);

    if (state->current == STATE_ON) {
        /* LED2 On */
        hal_gpio_write(led_device[1], 0);
    } else {
        /* LED2 Off */
        hal_gpio_write(led_device[1], 1);
    }
}

static void vnd_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    static uint16_t vnd_counter = 0;
    static bool beacon_disabled = 0;

    M_BT_DBG;

    vnd_counter++;
    if(beacon_disabled ==0)
    {
        gap_beacon_disable();
        beacon_disabled = 1;
    }
    dbg_printf("VND set 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
                    buf->om_data[0], buf->om_data[1], buf->om_data[2],
                    buf->om_data[3], buf->om_data[4], buf->om_data[5],
                    buf->om_data[6], buf->om_data[7]);
    update_led_command(*(remote_control_adv_t *)buf->om_data, 8);
}

static void vnd_status(struct bt_mesh_model *model,
               struct bt_mesh_msg_ctx *ctx,
               struct os_mbuf *buf)
{
    M_BT_DBG;

    printk("Acknownledgement from Vendor\n");
    printk("cmd = %04x\n", net_buf_simple_pull_le16(buf));
    printk("response = %08lx\n", net_buf_simple_pull_le32(buf));
}

/* Light Lightness Server message handlers */
static void light_lightness_get(struct bt_mesh_model *model,
                struct bt_mesh_msg_ctx *ctx,
                struct os_mbuf *buf)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(2 + 5 + 4);
    struct light_lightness_state *state = model->user_data;

    M_BT_DBG;

    bt_mesh_model_msg_init(msg, OP_LIGHT_LIGHTNESS_STATUS);
#if (NO_TRANSITON_STATUS==0)
    net_buf_simple_add_le16(msg, state->actual);

    if (state->transition->counter) {
        calculate_rt(state->transition);
        net_buf_simple_add_le16(msg, state->target_actual);
        net_buf_simple_add_u8(msg, state->transition->rt);
    }
#else
    net_buf_simple_add_le16(msg, state->target_actual);
#endif
    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        printk("Unable to send LightLightnessAct Status response\n");
    }

    os_mbuf_free_chain(msg);
}

static void light_lightness_publish(struct bt_mesh_model *model)
{
    int err;
    struct os_mbuf *msg = model->pub->msg;
    struct light_lightness_state *state = model->user_data;

    M_BT_DBG;

    if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }

    bt_mesh_model_msg_init(msg, OP_LIGHT_LIGHTNESS_STATUS);
    net_buf_simple_add_le16(msg, state->actual);

    if (state->transition->counter) {
        calculate_rt(state->transition);
        net_buf_simple_add_le16(msg, state->target_actual);
        net_buf_simple_add_u8(msg, state->transition->rt);
    }

    err = bt_mesh_model_publish(model);
    if (err) {
        printk("bt_mesh_model_publish err %d\n", err);
    }
}

static void light_lightness_set_unack(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct os_mbuf *buf)
{
    u8_t tid, tt, delay;
    u16_t actual;
    s64_t now;
    struct light_lightness_state *state = model->user_data;

    M_BT_DBG;

    actual = net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    now = k_uptime_get();
    if (state->last_tid == tid &&
        state->last_src_addr == ctx->addr &&
        state->last_dst_addr == ctx->recv_dst &&
        (now - state->last_msg_timestamp <= K_SECONDS(6))) {
        return;
    }

    switch (buf->om_len) {
    case 0x00:    /* No optional fields are available */
        tt = default_tt;
        delay = 0;
        break;
    case 0x02:    /* Optional fields are available */
        tt = net_buf_simple_pull_u8(buf);
        if ((tt & 0x3F) == 0x3F) {
            return;
        }

        delay = net_buf_simple_pull_u8(buf);
        break;
    default:
        return;
    }

    *ptr_counter = 0;
    ble_npl_callout_stop(ptr_timer);

    state->last_tid = tid;
    state->last_src_addr = ctx->addr;
    state->last_dst_addr = ctx->recv_dst;
    state->last_msg_timestamp = now;

    if (actual > 0 && actual < state->light_range_min) {
        actual = state->light_range_min;
    } else if (actual > state->light_range_max) {
        actual = state->light_range_max;
    }

    state->target_actual = actual;

    if (state->target_actual != state->actual) {
        light_lightness_actual_tt_values(state, tt, delay);
    } else {
        light_lightness_publish(model);
        return;
    }

    /* For Instantaneous Transition */
    if (state->transition->counter == 0) {
        state->actual = state->target_actual;
    }

    state->transition->just_started = true;
    light_lightness_publish(model);
    light_lightness_actual_handler(state);
}

static void light_lightness_set(struct bt_mesh_model *model,
                struct bt_mesh_msg_ctx *ctx,
                struct os_mbuf *buf)
{
    u8_t tid, tt, delay;
    u16_t actual;
    s64_t now;
    struct light_lightness_state *state = model->user_data;

    M_BT_DBG;

    actual = net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    now = k_uptime_get();
    if (state->last_tid == tid &&
        state->last_src_addr == ctx->addr &&
        state->last_dst_addr == ctx->recv_dst &&
        (now - state->last_msg_timestamp <= K_SECONDS(6))) {
        light_lightness_get(model, ctx, buf);
        return;
    }

    switch (buf->om_len) {
    case 0x00:    /* No optional fields are available */
        tt = default_tt;
        delay = 0;
        break;
    case 0x02:    /* Optional fields are available */
        tt = net_buf_simple_pull_u8(buf);
        if ((tt & 0x3F) == 0x3F) {
            return;
        }

        delay = net_buf_simple_pull_u8(buf);
        break;
    default:
        return;
    }

    *ptr_counter = 0;
    ble_npl_callout_stop(ptr_timer);

    state->last_tid = tid;
    state->last_src_addr = ctx->addr;
    state->last_dst_addr = ctx->recv_dst;
    state->last_msg_timestamp = now;

    if (actual > 0 && actual < state->light_range_min) {
        actual = state->light_range_min;
    } else if (actual > state->light_range_max) {
        actual = state->light_range_max;
    }

    state->target_actual = actual;

    if (state->target_actual != state->actual) {
        light_lightness_actual_tt_values(state, tt, delay);
    } else {
        light_lightness_get(model, ctx, buf);
        light_lightness_publish(model);
        return;
    }

    /* For Instantaneous Transition */
    if (state->transition->counter == 0) {
        state->actual = state->target_actual;
    }

    state->transition->just_started = true;
    light_lightness_get(model, ctx, buf);
    light_lightness_publish(model);
    light_lightness_actual_handler(state);
}

static void light_lightness_linear_get(struct bt_mesh_model *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct os_mbuf *buf)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(2 + 5 + 4);
    struct light_lightness_state *state = model->user_data;

    M_BT_DBG;

    bt_mesh_model_msg_init(msg,
                   OP_LIGHT_LIGHTNESS_LINEAR_STATUS);
#if (NO_TRANSITION_STATUS==0)
    net_buf_simple_add_le16(msg, state->linear);

    if (state->transition->counter) {
        calculate_rt(state->transition);
        net_buf_simple_add_le16(msg, state->target_linear);
        net_buf_simple_add_u8(msg, state->transition->rt);
    }
#else
    net_buf_simple_add_le16(msg, state->target_linear);
#endif
    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        printk("Unable to send LightLightnessLin Status response\n");
    }

    os_mbuf_free_chain(msg);
}

static void light_lightness_linear_publish(struct bt_mesh_model *model)
{
    int err;
    struct os_mbuf *msg = model->pub->msg;
    struct light_lightness_state *state = model->user_data;

    M_BT_DBG;

    if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }

    bt_mesh_model_msg_init(msg,
                   OP_LIGHT_LIGHTNESS_LINEAR_STATUS);
    net_buf_simple_add_le16(msg, state->linear);

    if (state->transition->counter) {
        calculate_rt(state->transition);
        net_buf_simple_add_le16(msg, state->target_linear);
        net_buf_simple_add_u8(msg, state->transition->rt);
    }

    err = bt_mesh_model_publish(model);
    if (err) {
        printk("bt_mesh_model_publish err %d\n", err);
    }
}

static void light_lightness_linear_set_unack(struct bt_mesh_model *model,
                         struct bt_mesh_msg_ctx *ctx,
                         struct os_mbuf *buf)
{
    u8_t tid, tt, delay;
    u16_t linear;
    s64_t now;
    struct light_lightness_state *state = model->user_data;

    M_BT_DBG;

    linear = net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    now = k_uptime_get();
    if (state->last_tid == tid &&
        state->last_src_addr == ctx->addr &&
        state->last_dst_addr == ctx->recv_dst &&
        (now - state->last_msg_timestamp <= K_SECONDS(6))) {
        return;
    }

    switch (buf->om_len) {
    case 0x00:    /* No optional fields are available */
        tt = default_tt;
        delay = 0;
        break;
    case 0x02:    /* Optional fields are available */
        tt = net_buf_simple_pull_u8(buf);
        if ((tt & 0x3F) == 0x3F) {
            return;
        }

        delay = net_buf_simple_pull_u8(buf);
        break;
    default:
        return;
    }

    *ptr_counter = 0;
    ble_npl_callout_stop(ptr_timer);

    state->last_tid = tid;
    state->last_src_addr = ctx->addr;
    state->last_dst_addr = ctx->recv_dst;
    state->last_msg_timestamp = now;
    state->target_linear = linear;

    if (state->target_linear != state->linear) {
        light_lightness_linear_tt_values(state, tt, delay);
    } else {
        light_lightness_linear_publish(model);
        return;
    }

    /* For Instantaneous Transition */
    if (state->transition->counter == 0) {
        state->linear = state->target_linear;
    }

    state->transition->just_started = true;
    light_lightness_linear_publish(model);
    light_lightness_linear_handler(state);
}

static void light_lightness_linear_set(struct bt_mesh_model *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct os_mbuf *buf)
{
    u8_t tid, tt, delay;
    u16_t linear;
    s64_t now;
    struct light_lightness_state *state = model->user_data;

    M_BT_DBG;

    linear = net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    now = k_uptime_get();
    if (state->last_tid == tid &&
        state->last_src_addr == ctx->addr &&
        state->last_dst_addr == ctx->recv_dst &&
        (now - state->last_msg_timestamp <= K_SECONDS(6))) {
        light_lightness_linear_get(model, ctx, buf);
        return;
    }

    switch (buf->om_len) {
    case 0x00:    /* No optional fields are available */
        tt = default_tt;
        delay = 0;
        break;
    case 0x02:    /* Optional fields are available */
        tt = net_buf_simple_pull_u8(buf);
        if ((tt & 0x3F) == 0x3F) {
            return;
        }

        delay = net_buf_simple_pull_u8(buf);
        break;
    default:
        return;
    }

    *ptr_counter = 0;
    ble_npl_callout_stop(ptr_timer);

    state->last_tid = tid;
    state->last_src_addr = ctx->addr;
    state->last_dst_addr = ctx->recv_dst;
    state->last_msg_timestamp = now;
    state->target_linear = linear;

    if (state->target_linear != state->linear) {
        light_lightness_linear_tt_values(state, tt, delay);
    } else {
        light_lightness_linear_get(model, ctx, buf);
        light_lightness_linear_publish(model);
        return;
    }

    /* For Instantaneous Transition */
    if (state->transition->counter == 0) {
        state->linear = state->target_linear;
    }

    state->transition->just_started = true;
    light_lightness_linear_get(model, ctx, buf);
    light_lightness_linear_publish(model);
    light_lightness_linear_handler(state);
}

static void light_lightness_last_get(struct bt_mesh_model *model,
                     struct bt_mesh_msg_ctx *ctx,
                     struct os_mbuf *buf)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(2 + 2 + 4);
    struct light_lightness_state *state = model->user_data;

    M_BT_DBG;

    bt_mesh_model_msg_init(msg, OP_LIGHT_LIGHTNESS_LAST_STATUS);
    net_buf_simple_add_le16(msg, state->last);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        printk("Unable to send LightLightnessLast Status response\n");
    }

    os_mbuf_free_chain(msg);
}

static void light_lightness_default_get(struct bt_mesh_model *model,
                    struct bt_mesh_msg_ctx *ctx,
                    struct os_mbuf *buf)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(2 + 2 + 4);
    struct light_lightness_state *state = model->user_data;

    M_BT_DBG;

    bt_mesh_model_msg_init(msg,
                   OP_LIGHT_LIGHTNESS_DEFAULT_STATUS);
    net_buf_simple_add_le16(msg, state->def);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        printk("Unable to send LightLightnessDef Status response\n");
    }

    os_mbuf_free_chain(msg);
}

static void light_lightness_range_get(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct os_mbuf *buf)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(2 + 5 + 4);
    struct light_lightness_state *state = model->user_data;

    M_BT_DBG;

    state->status_code = RANGE_SUCCESSFULLY_UPDATED;

    bt_mesh_model_msg_init(msg, OP_LIGHT_LIGHTNESS_RANGE_STATUS);
    net_buf_simple_add_u8(msg, state->status_code);
    net_buf_simple_add_le16(msg, state->light_range_min);
    net_buf_simple_add_le16(msg, state->light_range_max);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        printk("Unable to send LightLightnessRange Status response\n");
    }

    os_mbuf_free_chain(msg);
}

/* Light Lightness Setup Server message handlers */

static void light_lightness_default_publish(struct bt_mesh_model *model)
{
    int err;
    struct os_mbuf *msg = model->pub->msg;
    struct light_lightness_state *state = model->user_data;

    M_BT_DBG;

    if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }

    bt_mesh_model_msg_init(msg,
                   OP_LIGHT_LIGHTNESS_DEFAULT_STATUS);
    net_buf_simple_add_le16(msg, state->def);

    err = bt_mesh_model_publish(model);
    if (err) {
        printk("bt_mesh_model_publish err %d\n", err);
    }
}

static void light_lightness_default_set_unack(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct os_mbuf *buf)
{
    u16_t lightness;
    struct light_lightness_state *state = model->user_data;

    M_BT_DBG;

    lightness = net_buf_simple_pull_le16(buf);

    /* Here, Model specification is silent about tid implementation */

    if (state->def != lightness) {
        state->def = lightness;
        light_ctl_srv_user_data.lightness_def = state->def;

        save_on_flash(LIGHTNESS_TEMP_DEF_STATE);
    }

    light_lightness_default_publish(model);
}

static void light_lightness_default_set(struct bt_mesh_model *model,
                    struct bt_mesh_msg_ctx *ctx,
                    struct os_mbuf *buf)
{
    M_BT_DBG;
    light_lightness_default_set_unack(model, ctx, buf);
    light_lightness_default_get(model, ctx, buf);
    light_lightness_default_publish(model);
}

static void light_lightness_range_publish(struct bt_mesh_model *model)
{
    int err;
    struct os_mbuf *msg = model->pub->msg;
    struct light_lightness_state *state = model->user_data;

    M_BT_DBG;

    if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }

    bt_mesh_model_msg_init(msg, OP_LIGHT_LIGHTNESS_RANGE_STATUS);
    net_buf_simple_add_u8(msg, state->status_code);
    net_buf_simple_add_le16(msg, state->light_range_min);
    net_buf_simple_add_le16(msg, state->light_range_max);

    err = bt_mesh_model_publish(model);
    if (err) {
        printk("bt_mesh_model_publish err %d\n", err);
    }
}

static bool light_lightness_range_setunack(struct bt_mesh_model *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct os_mbuf *buf)
{
    u16_t min, max;
    struct light_lightness_state *state = model->user_data;

    M_BT_DBG;

    min = net_buf_simple_pull_le16(buf);
    max = net_buf_simple_pull_le16(buf);

    /* Here, Model specification is silent about tid implementation */

    if (min == 0 || max == 0) {
        return false;
    } else {
        if (min <= max) {
            state->status_code = RANGE_SUCCESSFULLY_UPDATED;

            if (state->light_range_min != min ||
                state->light_range_max != max) {

                state->light_range_min = min;
                state->light_range_max = max;

                save_on_flash(LIGHTNESS_RANGE);
            }
        } else {
            /* The provided value for Range Max cannot be set */
            state->status_code = CANNOT_SET_RANGE_MAX;
            return false;
        }
    }

    return true;
}

static void light_lightness_range_set_unack(struct bt_mesh_model *model,
                        struct bt_mesh_msg_ctx *ctx,
                        struct os_mbuf *buf)
{
    M_BT_DBG;
    if (light_lightness_range_setunack(model, ctx, buf) == true) {
        light_lightness_range_publish(model);
    }
}

static void light_lightness_range_set(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct os_mbuf *buf)
{
    M_BT_DBG;
    if (light_lightness_range_setunack(model, ctx, buf) == true) {
        light_lightness_range_get(model, ctx, buf);
        light_lightness_range_publish(model);
    }
}

/* Light Lightness Client message handlers */
static void light_lightness_status(struct bt_mesh_model *model,
                   struct bt_mesh_msg_ctx *ctx,
                   struct os_mbuf *buf)
{
    M_BT_DBG;
    printk("Acknownledgement from LIGHT_LIGHTNESS_SRV (Actual)\n");
    printk("Present Lightness = %04x\n", net_buf_simple_pull_le16(buf));

    if (buf->om_len == 3) {
        printk("Target Lightness = %04x\n",
               net_buf_simple_pull_le16(buf));
        printk("Remaining Time = %02x\n", net_buf_simple_pull_u8(buf));
    }
}

static void light_lightness_linear_status(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct os_mbuf *buf)
{
    M_BT_DBG;
    printk("Acknownledgement from LIGHT_LIGHTNESS_SRV (Linear)\n");
    printk("Present Lightness = %04x\n", net_buf_simple_pull_le16(buf));

    if (buf->om_len == 3) {
        printk("Target Lightness = %04x\n",
               net_buf_simple_pull_le16(buf));
        printk("Remaining Time = %02x\n", net_buf_simple_pull_u8(buf));
    }
}

static void light_lightness_last_status(struct bt_mesh_model *model,
                    struct bt_mesh_msg_ctx *ctx,
                    struct os_mbuf *buf)
{
    M_BT_DBG;
    printk("Acknownledgement from LIGHT_LIGHTNESS_SRV (Last)\n");
    printk("Lightness = %04x\n", net_buf_simple_pull_le16(buf));
}

static void light_lightness_default_status(struct bt_mesh_model *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct os_mbuf *buf)
{
    M_BT_DBG;
    printk("Acknownledgement from LIGHT_LIGHTNESS_SRV (Default)\n");
    printk("Lightness = %04x\n", net_buf_simple_pull_le16(buf));
}

static void light_lightness_range_status(struct bt_mesh_model *model,
                     struct bt_mesh_msg_ctx *ctx,
                     struct os_mbuf *buf)
{
    M_BT_DBG;
    printk("Acknownledgement from LIGHT_LIGHTNESS_SRV (Lightness Range)\n");
    printk("Status Code = %02x\n", net_buf_simple_pull_u8(buf));
    printk("Range Min = %04x\n", net_buf_simple_pull_le16(buf));
    printk("Range Max = %04x\n", net_buf_simple_pull_le16(buf));
}

/* Light CTL Server message handlers */
static void light_ctl_get(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct os_mbuf *buf)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(2 + 9 + 4);
    struct light_ctl_state *state = model->user_data;

    M_BT_DBG;

    bt_mesh_model_msg_init(msg, OP_LIGHT_CTL_STATUS);
#if (NO_TRANSITION_STATUS==0)
    net_buf_simple_add_le16(msg, state->lightness);
    net_buf_simple_add_le16(msg, state->temp);

    if (state->transition->counter) {
        calculate_rt(state->transition);
        net_buf_simple_add_le16(msg, state->target_lightness);
        net_buf_simple_add_le16(msg, state->target_temp);
        net_buf_simple_add_u8(msg, state->transition->rt);
    }
#else
    net_buf_simple_add_le16(msg, state->target_lightness);
    net_buf_simple_add_le16(msg, state->target_temp);
#endif
    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        printk("Unable to send LightCTL Status response\n");
    }

    os_mbuf_free_chain(msg);
}

static void light_ctl_publish(struct bt_mesh_model *model)
{
    int err;
    struct os_mbuf *msg = model->pub->msg;
    struct light_ctl_state *state = model->user_data;

    M_BT_DBG;

    if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }

    bt_mesh_model_msg_init(msg, OP_LIGHT_CTL_STATUS);

    /* Here, as per Model specification, status should be
     * made up of lightness & temperature values only
     */
    net_buf_simple_add_le16(msg, state->lightness);
    net_buf_simple_add_le16(msg, state->temp);

    if (state->transition->counter) {
        calculate_rt(state->transition);
        net_buf_simple_add_le16(msg, state->target_lightness);
        net_buf_simple_add_le16(msg, state->target_temp);
        net_buf_simple_add_u8(msg, state->transition->rt);
    }

    err = bt_mesh_model_publish(model);
    if (err) {
        printk("bt_mesh_model_publish err %d\n", err);
    }
}

static void light_ctl_set_unack(struct bt_mesh_model *model,
                struct bt_mesh_msg_ctx *ctx,
                struct os_mbuf *buf)
{
    u8_t tid, tt, delay;
    s16_t delta_uv;
    u16_t lightness, temp;
    s64_t now;
    struct light_ctl_state *state = model->user_data;

    M_BT_DBG;

    lightness = net_buf_simple_pull_le16(buf);
    temp = net_buf_simple_pull_le16(buf);
    delta_uv = (s16_t) net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    if (temp < TEMP_MIN || temp > TEMP_MAX) {
        return;
    }

    now = k_uptime_get();
    if (state->last_tid == tid &&
        state->last_src_addr == ctx->addr &&
        state->last_dst_addr == ctx->recv_dst &&
        (now - state->last_msg_timestamp <= K_SECONDS(6))) {
        return;
    }

    switch (buf->om_len) {
    case 0x00:    /* No optional fields are available */
        tt = default_tt;
        delay = 0;
        break;
    case 0x02:    /* Optional fields are available */
        tt = net_buf_simple_pull_u8(buf);
        if ((tt & 0x3F) == 0x3F) {
            return;
        }

        delay = net_buf_simple_pull_u8(buf);
        break;
    default:
        return;
    }

    *ptr_counter = 0;
    ble_npl_callout_stop(ptr_timer);

    state->last_tid = tid;
    state->last_src_addr = ctx->addr;
    state->last_dst_addr = ctx->recv_dst;
    state->last_msg_timestamp = now;
    state->target_lightness = lightness;

    if (temp < state->temp_range_min) {
        temp = state->temp_range_min;
    } else if (temp > state->temp_range_max) {
        temp = state->temp_range_max;
    }

    state->target_temp = temp;
    state->target_delta_uv = delta_uv;

    if (state->target_lightness != state->lightness ||
        state->target_temp != state->temp ||
        state->target_delta_uv != state->delta_uv) {
        light_ctl_tt_values(state, tt, delay);
    } else {
        light_ctl_publish(model);
        return;
    }

    /* For Instantaneous Transition */
    if (state->transition->counter == 0) {
        state->lightness = state->target_lightness;
        state->temp = state->target_temp;
        state->delta_uv = state->target_delta_uv;
    }

    state->transition->just_started = true;
    light_ctl_publish(model);
    light_ctl_handler(state);
}

static void light_ctl_set(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct os_mbuf *buf)
{
    u8_t tid, tt, delay;
    s16_t delta_uv;
    u16_t lightness, temp;
    s64_t now;
    struct light_ctl_state *state = model->user_data;

    M_BT_DBG;

    lightness = net_buf_simple_pull_le16(buf);
    temp = net_buf_simple_pull_le16(buf);
    delta_uv = (s16_t) net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    if (temp < TEMP_MIN || temp > TEMP_MAX) {
        return;
    }

    now = k_uptime_get();
    if (state->last_tid == tid &&
        state->last_src_addr == ctx->addr &&
        state->last_dst_addr == ctx->recv_dst &&
        (now - state->last_msg_timestamp <= K_SECONDS(6))) {
        light_ctl_get(model, ctx, buf);
        return;
    }

    switch (buf->om_len) {
    case 0x00:    /* No optional fields are available */
        tt = default_tt;
        delay = 0;
        break;
    case 0x02:    /* Optional fields are available */
        tt = net_buf_simple_pull_u8(buf);
        if ((tt & 0x3F) == 0x3F) {
            return;
        }

        delay = net_buf_simple_pull_u8(buf);
        break;
    default:
        return;
    }

    *ptr_counter = 0;
    ble_npl_callout_stop(ptr_timer);

    state->last_tid = tid;
    state->last_src_addr = ctx->addr;
    state->last_dst_addr = ctx->recv_dst;
    state->last_msg_timestamp = now;
    state->target_lightness = lightness;

    if (temp < state->temp_range_min) {
        temp = state->temp_range_min;
    } else if (temp > state->temp_range_max) {
        temp = state->temp_range_max;
    }

    state->target_temp = temp;
    state->target_delta_uv = delta_uv;

    if (state->target_lightness != state->lightness ||
        state->target_temp != state->temp ||
        state->target_delta_uv != state->delta_uv) {
        light_ctl_tt_values(state, tt, delay);
    } else {
        light_ctl_get(model, ctx, buf);
        light_ctl_publish(model);
        return;
    }

    /* For Instantaneous Transition */
    if (state->transition->counter == 0) {
        state->lightness = state->target_lightness;
        state->temp = state->target_temp;
        state->delta_uv = state->target_delta_uv;
    }
    dbg_printf("target_temp444  0x%x\n",target_temperature);
    state->transition->just_started = true;
    light_ctl_get(model, ctx, buf);
    light_ctl_publish(model);
    light_ctl_handler(state);
}

static void light_ctl_temp_range_get(struct bt_mesh_model *model,
                     struct bt_mesh_msg_ctx *ctx,
                     struct os_mbuf *buf)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(2 + 5 + 4);
    struct light_ctl_state *state = model->user_data;

    M_BT_DBG;

    state->status_code = RANGE_SUCCESSFULLY_UPDATED;

    bt_mesh_model_msg_init(msg, OP_LIGHT_CTL_TEMP_RANGE_STATUS);
    net_buf_simple_add_u8(msg, state->status_code);
    net_buf_simple_add_le16(msg, state->temp_range_min);
    net_buf_simple_add_le16(msg, state->temp_range_max);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        printk("Unable to send LightCTL Temp Range Status response\n");
    }

    os_mbuf_free_chain(msg);
}

static void light_ctl_default_get(struct bt_mesh_model *model,
                  struct bt_mesh_msg_ctx *ctx,
                  struct os_mbuf *buf)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(2 + 6 + 4);
    struct light_ctl_state *state = model->user_data;

    M_BT_DBG;

    bt_mesh_model_msg_init(msg, OP_LIGHT_CTL_DEFAULT_STATUS);
    net_buf_simple_add_le16(msg, state->lightness_def);
    net_buf_simple_add_le16(msg, state->temp_def);
    net_buf_simple_add_le16(msg, state->delta_uv_def);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        printk("Unable to send LightCTL Default Status response\n");
    }

    os_mbuf_free_chain(msg);
}

/* Light CTL Setup Server message handlers */

static void light_ctl_default_publish(struct bt_mesh_model *model)
{
    int err;
    struct os_mbuf *msg = model->pub->msg;
    struct light_ctl_state *state = model->user_data;

    M_BT_DBG;

    if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }

    bt_mesh_model_msg_init(msg, OP_LIGHT_CTL_DEFAULT_STATUS);
    net_buf_simple_add_le16(msg, state->lightness_def);
    net_buf_simple_add_le16(msg, state->temp_def);
    net_buf_simple_add_le16(msg, state->delta_uv_def);

    err = bt_mesh_model_publish(model);
    if (err) {
        printk("bt_mesh_model_publish err %d\n", err);
    }
}

static bool light_ctl_default_setunack(struct bt_mesh_model *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct os_mbuf *buf)
{
    u16_t lightness, temp;
    s16_t delta_uv;
    struct light_ctl_state *state = model->user_data;

    M_BT_DBG;

    lightness = net_buf_simple_pull_le16(buf);
    temp = net_buf_simple_pull_le16(buf);
    delta_uv = (s16_t) net_buf_simple_pull_le16(buf);

    /* Here, Model specification is silent about tid implementation */

    if (temp < TEMP_MIN || temp > TEMP_MAX) {
        return false;
    }

    if (temp < state->temp_range_min) {
        temp = state->temp_range_min;
    } else if (temp > state->temp_range_max) {
        temp = state->temp_range_max;
    }

    if (state->lightness_def != lightness || state->temp_def != temp ||
        state->delta_uv_def != delta_uv) {
        state->lightness_def = lightness;
        state->temp_def = temp;
        state->delta_uv_def = delta_uv;

        save_on_flash(LIGHTNESS_TEMP_DEF_STATE);
    }

    return true;
}

static void light_ctl_default_set_unack(struct bt_mesh_model *model,
                    struct bt_mesh_msg_ctx *ctx,
                    struct os_mbuf *buf)
{
    M_BT_DBG;
    if (light_ctl_default_setunack(model, ctx, buf) == true) {
        light_ctl_default_publish(model);
    }
}

static void light_ctl_default_set(struct bt_mesh_model *model,
                  struct bt_mesh_msg_ctx *ctx,
                  struct os_mbuf *buf)
{
    M_BT_DBG;
    if (light_ctl_default_setunack(model, ctx, buf) == true) {
        light_ctl_default_get(model, ctx, buf);
        light_ctl_default_publish(model);
    }
}

static void light_ctl_temp_range_publish(struct bt_mesh_model *model)
{
    int err;
    struct os_mbuf *msg = model->pub->msg;
    struct light_ctl_state *state = model->user_data;

    M_BT_DBG;

    if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }

    bt_mesh_model_msg_init(msg, OP_LIGHT_CTL_TEMP_RANGE_STATUS);
    net_buf_simple_add_u8(msg, state->status_code);
    net_buf_simple_add_le16(msg, state->temp_range_min);
    net_buf_simple_add_le16(msg, state->temp_range_max);

    err = bt_mesh_model_publish(model);
    if (err) {
        printk("bt_mesh_model_publish err %d\n", err);
    }
}

static bool light_ctl_temp_range_setunack(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct os_mbuf *buf)
{
    u16_t min, max;
    struct light_ctl_state *state = model->user_data;

    M_BT_DBG;

    min = net_buf_simple_pull_le16(buf);
    max = net_buf_simple_pull_le16(buf);

    /* Here, Model specification is silent about tid implementation */

    /* This is as per 6.1.3.1 in Mesh Model Specification */
    if (min < TEMP_MIN || min > TEMP_MAX ||
        max < TEMP_MIN || max > TEMP_MAX) {
        return false;
    }

    if (min <= max) {
        state->status_code = RANGE_SUCCESSFULLY_UPDATED;

            if (state->temp_range_min != min ||
                state->temp_range_max != max) {

                state->temp_range_min = min;
                state->temp_range_max = max;

                save_on_flash(TEMPERATURE_RANGE);
            }
    } else {
        /* The provided value for Range Max cannot be set */
        state->status_code = CANNOT_SET_RANGE_MAX;
        return false;
    }

    return true;
}

static void light_ctl_temp_range_set_unack(struct bt_mesh_model *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct os_mbuf *buf)
{
    M_BT_DBG;
    if (light_ctl_temp_range_setunack(model, ctx, buf) == true) {
        light_ctl_temp_range_publish(model);
    }
}

static void light_ctl_temp_range_set(struct bt_mesh_model *model,
                     struct bt_mesh_msg_ctx *ctx,
                     struct os_mbuf *buf)
{
    M_BT_DBG;
    if (light_ctl_temp_range_setunack(model, ctx, buf) == true) {
        light_ctl_temp_range_get(model, ctx, buf);
        light_ctl_temp_range_publish(model);
    }
}

/* message handlers (End) */

/* Mapping of message handlers for Generic OnOff Server (0x1000) */
static const struct bt_mesh_model_op gen_onoff_srv_op[] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x01), 0, gen_onoff_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x02), 2, gen_onoff_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x03), 2, gen_onoff_set_unack },
    { BT_MESH_MODEL_OP_2(0x82, 0x04), 1, gen_onoff_status },
    BT_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Generic OnOff Client (0x1001) */
static const struct bt_mesh_model_op gen_onoff_cli_op[] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x04), 1, gen_onoff_status },
    BT_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Generic Levl Server (0x1002) */
static const struct bt_mesh_model_op gen_level_srv_op[] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x05), 0, gen_level_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x06), 3, gen_level_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x07), 3, gen_level_set_unack },
    { BT_MESH_MODEL_OP_2(0x82, 0x09), 5, gen_delta_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x0A), 5, gen_delta_set_unack },
    { BT_MESH_MODEL_OP_2(0x82, 0x0B), 3, gen_move_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x0C), 3, gen_move_set_unack },
    BT_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Generic Level Client (0x1003) */
static const struct bt_mesh_model_op gen_level_cli_op[] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x08), 2, gen_level_status },
    BT_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Generic Default TT Server (0x1004) */
static const struct bt_mesh_model_op gen_def_trans_time_srv_op[] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x0D), 0, gen_def_trans_time_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x0E), 1, gen_def_trans_time_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x0F), 1, gen_def_trans_time_set_unack },
    BT_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Generic Default TT Client (0x1005) */
static const struct bt_mesh_model_op gen_def_trans_time_cli_op[] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x10), 1, gen_def_trans_time_status },
    BT_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Generic Power OnOff Server (0x1006) */
static const struct bt_mesh_model_op gen_power_onoff_srv_op[] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x11), 0, gen_onpowerup_get },
    BT_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Generic Power OnOff Setup Server (0x1007) */
static const struct bt_mesh_model_op gen_power_onoff_setup_srv_op[] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x13), 1, gen_onpowerup_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x14), 1, gen_onpowerup_set_unack },
    BT_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Generic Power OnOff Client (0x1008) */
static const struct bt_mesh_model_op gen_power_onoff_cli_op[] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x12), 1, gen_onpowerup_status },
    BT_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Light Lightness Server (0x1300) */
static const struct bt_mesh_model_op light_lightness_srv_op[] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x4B), 0, light_lightness_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x4C), 3, light_lightness_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x4D), 3, light_lightness_set_unack },
    { BT_MESH_MODEL_OP_2(0x82, 0x4F), 0, light_lightness_linear_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x50), 3, light_lightness_linear_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x51), 3, light_lightness_linear_set_unack },
    { BT_MESH_MODEL_OP_2(0x82, 0x53), 0, light_lightness_last_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x55), 0, light_lightness_default_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x57), 0, light_lightness_range_get },
    BT_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Light Lightness Setup Server (0x1301) */
static const struct bt_mesh_model_op light_lightness_setup_srv_op[] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x59), 2, light_lightness_default_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x5A), 2, light_lightness_default_set_unack },
    { BT_MESH_MODEL_OP_2(0x82, 0x5B), 4, light_lightness_range_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x5C), 4, light_lightness_range_set_unack },
    BT_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Light Lightness Client (0x1302) */
static const struct bt_mesh_model_op light_lightness_cli_op[] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x4E), 2, light_lightness_status },
    { BT_MESH_MODEL_OP_2(0x82, 0x52), 2, light_lightness_linear_status },
    { BT_MESH_MODEL_OP_2(0x82, 0x54), 2, light_lightness_last_status },
    { BT_MESH_MODEL_OP_2(0x82, 0x56), 2, light_lightness_default_status },
    { BT_MESH_MODEL_OP_2(0x82, 0x58), 5, light_lightness_range_status },
    BT_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Light CTL Server (0x1303) */
static const struct bt_mesh_model_op light_ctl_srv_op[] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x5D), 0, light_ctl_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x5E), 7, light_ctl_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x5F), 7, light_ctl_set_unack },
    { BT_MESH_MODEL_OP_2(0x82, 0x62), 0, light_ctl_temp_range_get },
    { BT_MESH_MODEL_OP_2(0x82, 0x67), 0, light_ctl_default_get },
    BT_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Light CTL Setup Server (0x1304) */
static const struct bt_mesh_model_op light_ctl_setup_srv_op[] = {
    { BT_MESH_MODEL_OP_2(0x82, 0x69), 6, light_ctl_default_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x6A), 6, light_ctl_default_set_unack },
    { BT_MESH_MODEL_OP_2(0x82, 0x6B), 4, light_ctl_temp_range_set },
    { BT_MESH_MODEL_OP_2(0x82, 0x6C), 4, light_ctl_temp_range_set_unack },
    BT_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Vendor  */
static const struct bt_mesh_model_op vnd_ops[] = {
    { BT_MESH_MODEL_OP_3(0x01, INGCHIPS_COMP_ID), 0, vnd_get },
    { BT_MESH_MODEL_OP_3(0x02, INGCHIPS_COMP_ID), 3, vnd_set },
    { BT_MESH_MODEL_OP_3(0x03, INGCHIPS_COMP_ID), 3, vnd_set_unack },
    { BT_MESH_MODEL_OP_3(0x04, INGCHIPS_COMP_ID), 6, vnd_status },
    BT_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Vendor TIANMAO  */
static const struct bt_mesh_model_op vnd_tm_ops[] = {
    { BT_MESH_MODEL_OP_3(0xd0, ALIbaba_COMP_ID), 0, vnd_get },
    { BT_MESH_MODEL_OP_3(0xd1, ALIbaba_COMP_ID), 3, vnd_set },
    { BT_MESH_MODEL_OP_3(0xd2, ALIbaba_COMP_ID), 3, vnd_set_unack },
    { BT_MESH_MODEL_OP_3(0xd3, ALIbaba_COMP_ID), 6, vnd_status },
    { BT_MESH_MODEL_OP_3(0xd4, ALIbaba_COMP_ID), 6, vnd_status },
    { BT_MESH_MODEL_OP_3(0xd5, ALIbaba_COMP_ID), 6, vnd_status },
    { BT_MESH_MODEL_OP_3(0xcf, ALIbaba_COMP_ID), 6, vnd_status },
    BT_MESH_MODEL_OP_END,
};

#undef  BT_MESH_MODEL_CFG_SRV
#undef  BT_MESH_MODEL_CFG_CLI
#define BT_MESH_MODEL_CFG_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_CFG_SRV, NULL, NULL, _user_data)
#define BT_MESH_MODEL_CFG_CLI(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_CFG_CLI, NULL, NULL, _user_data)
#define BT_MESH_MODEL_HEALTH_SRV1(_user_data,_pub) BT_MESH_MODEL(BT_MESH_MODEL_ID_HEALTH_SRV, NULL, _pub, _user_data)
static struct bt_mesh_cfg_cli cfg_cli = {
};

static struct bt_mesh_model root_models[] = {
    BT_MESH_MODEL_CFG_SRV(&cfg_srv),
    BT_MESH_MODEL_CFG_CLI(&cfg_cli),
    BT_MESH_MODEL_HEALTH_SRV1(&health_srv, &health_pub),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_SRV, gen_onoff_srv_op, &gen_onoff_srv_pub_root, &gen_onoff_srv_root_user_data),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_CLI, gen_onoff_cli_op, &gen_onoff_cli_pub_root, NULL),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_LEVEL_SRV, gen_level_srv_op, &gen_level_srv_pub_root, &gen_level_srv_root_user_data),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_LEVEL_CLI, gen_level_cli_op, &gen_level_cli_pub_root, NULL),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV, gen_def_trans_time_srv_op, &gen_def_trans_time_srv_pub, &gen_def_trans_time_srv_user_data),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI, gen_def_trans_time_cli_op, &gen_def_trans_time_cli_pub, NULL),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV, gen_power_onoff_srv_op, &gen_power_onoff_srv_pub, &gen_power_onoff_srv_user_data),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV, gen_power_onoff_setup_srv_op, &gen_power_onoff_srv_pub, &gen_power_onoff_srv_user_data),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_POWER_ONOFF_CLI, gen_power_onoff_cli_op, &gen_power_onoff_cli_pub, NULL),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV, light_lightness_srv_op, &light_lightness_srv_pub, &light_lightness_srv_user_data),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV, light_lightness_setup_srv_op, &light_lightness_srv_pub, &light_lightness_srv_user_data),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI, light_lightness_cli_op, &light_lightness_cli_pub, NULL),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_CTL_SRV, light_ctl_srv_op, &light_ctl_srv_pub, &light_ctl_srv_user_data),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV, light_ctl_setup_srv_op, &light_ctl_srv_pub, &light_ctl_srv_user_data),
};

static struct bt_mesh_model vnd_models[] = {
    BT_MESH_MODEL_VND(INGCHIPS_COMP_ID, BT_MESH_VND_MODEL_ID_INGCHIPS_COMP, vnd_ops, &vnd_pub, &vnd_user_data),
    BT_MESH_MODEL_VND(ALIbaba_COMP_ID, 0x0000, vnd_tm_ops, &vnd_pub, &vnd_user_data),
};

static struct bt_mesh_elem elements[] = {
    BT_MESH_ELEM(0, root_models, vnd_models),
};

static const struct bt_mesh_comp comp = {
    .cid = INGCHIPS_COMP_ID,
    .elem = elements,
    .elem_count = ARRAY_SIZE(elements),
};

struct bt_mesh_elem * get_element_of_node()
{
    return elements;
}

struct bt_mesh_comp * get_comp_of_node()
{
    return (struct bt_mesh_comp*)&comp;
}

uint8_t model_info_pub()
{
    uint8_t count = 0;
    struct bt_mesh_elem * elem = &elements[0];
    struct bt_mesh_model *model = (struct bt_mesh_model *)(elem->models);
    for (count=0; count<7;count++)
    {
        model++;
    }
    return 0;
}

uint8_t get_element_count(void)
{
    return ARRAY_SIZE(elements);
}

struct bt_mesh_model * get_model_by_id(uint16_t id)
{
    uint8_t r_models =  sizeof(root_models) / sizeof(root_models[0]);
    uint8_t v_models =  sizeof(vnd_models) / sizeof(vnd_models[0]);
    uint8_t i = 0;
    for (i=0; i < r_models; i++) {
        if (root_models[i].id == id) {
            return &root_models[i];
        }
    }

    for (i=0; i < v_models; i++) {
        if (vnd_models[i].vnd.company == id) {
            return &vnd_models[i];
        }
    }
    return NULL;
}

bool get_group_addr_by_id(uint16_t id, uint16_t* group)
{
    struct bt_mesh_model *pmod = get_model_by_id(id);

    if (pmod == NULL) {
        return false;
    }

    uint8_t i = CONFIG_BT_MESH_MODEL_GROUP_COUNT;
    for (i = 0; i < CONFIG_BT_MESH_MODEL_GROUP_COUNT; i++) {
        if(pmod->groups[i] != BT_MESH_ADDR_UNASSIGNED) {
            *group = pmod->groups[i];
            return true;
        }
    }
    return false;
}

static void device_composition_light_default_var_init(void)
{
    gen_def_trans_time_srv_user_data.tt = 0x00;

    gen_power_onoff_srv_user_data.onpowerup = STATE_RESTORE;

    light_lightness_srv_user_data.light_range_min = LIGHTNESS_MIN;
    light_lightness_srv_user_data.light_range_max = LIGHTNESS_MAX;
    light_lightness_srv_user_data.last = LIGHTNESS_MAX;
    light_lightness_srv_user_data.def = LIGHTNESS_MAX;

    /* Following 2 values are as per specification */
    light_ctl_srv_user_data.temp_range_min = TEMP_MIN;
    light_ctl_srv_user_data.temp_range_max = TEMP_MAX;

    light_ctl_srv_user_data.temp_def = TEMP_MIN;
    light_ctl_srv_user_data.lightness_temp_last =(uint32_t)LIGHTNESS_MAX <<16 | TEMP_MIN;
}

static void device_composition_light_default_status_init(void)
{
    u16_t lightness;
    static const uint16_t MIN_TEMP =  DEFAULT_MIN_TEMP;
    static const uint16_t DIS_TEMP =  DEFAULT_MAX_TEMP - DEFAULT_MIN_TEMP;
    //read from the flash,the last value for lightness and light temperature
    uint32_t t1 = ((g_gen_level_state+32768));
    uint32_t t2 = (MIN_TEMP +  DIS_TEMP *(100-GCW)/100);
    last_lightness = t1;
    light_ctl_srv_user_data.lightness_temp_def = light_ctl_srv_user_data.lightness_temp_last =  (t1<<16) | t2;
    dbg_printf("t1 0x%x t2 0x%x 3 0x%x\n",t1,t2,light_ctl_srv_user_data.lightness_temp_last);
        lightness = (u16_t) (light_ctl_srv_user_data.lightness_temp_last >> 16);

    if (lightness) {
        gen_onoff_srv_root_user_data.onoff = STATE_ON;
        gen_onoff_srv_root_user_data.target_onoff = STATE_ON;  //added by dengyiyn, for first status check
    } else {
        gen_onoff_srv_root_user_data.onoff = STATE_OFF;
        gen_onoff_srv_root_user_data.target_onoff = STATE_OFF; //added by dengyiyun, for first status check
    }

    /* Retrieve Default Lightness & Temperature Values */

    if (light_ctl_srv_user_data.lightness_temp_def) {
        light_ctl_srv_user_data.lightness_def = (u16_t)
            (light_ctl_srv_user_data.lightness_temp_def >> 16);

        light_ctl_srv_user_data.temp_def = (u16_t)
            (light_ctl_srv_user_data.lightness_temp_def);
    }

    light_lightness_srv_user_data.def =
        light_ctl_srv_user_data.lightness_def;

    light_ctl_srv_user_data.temp = light_ctl_srv_user_data.temp_def;

    /* Retrieve Range of Lightness & Temperature */

    if (light_lightness_srv_user_data.lightness_range) {
        light_lightness_srv_user_data.light_range_max = (u16_t)
            (light_lightness_srv_user_data.lightness_range >> 16);

        light_lightness_srv_user_data.light_range_min = (u16_t)
            (light_lightness_srv_user_data.lightness_range);
    }

    if (light_ctl_srv_user_data.temperature_range) {
        light_ctl_srv_user_data.temp_range_max = (u16_t)
            (light_ctl_srv_user_data.temperature_range >> 16);

        light_ctl_srv_user_data.temp_range_min = (u16_t)
            (light_ctl_srv_user_data.temperature_range);
    }

    switch (gen_power_onoff_srv_user_data.onpowerup) {
        case STATE_OFF:
            gen_onoff_srv_root_user_data.onoff = STATE_OFF;
            state_binding(ONOFF, ONOFF_TEMP);
            break;
        case STATE_DEFAULT:
            gen_onoff_srv_root_user_data.onoff = STATE_ON;
            state_binding(ONOFF, ONOFF_TEMP);
            break;
        case STATE_RESTORE:
            light_lightness_srv_user_data.last = (u16_t)
                (light_ctl_srv_user_data.lightness_temp_last >> 16);
            gen_level_srv_root_user_data.level = light_lightness_srv_user_data.last - 32768;
            light_ctl_srv_user_data.temp =
                (u16_t) (light_ctl_srv_user_data.lightness_temp_last);
            //add by dengyiyun ,to setup the actual of lightness
            light_lightness_srv_user_data.actual =  light_lightness_srv_user_data.last;
            state_binding(ONPOWERUP, ONOFF_TEMP);
            break;
    }

    default_tt = gen_def_trans_time_srv_user_data.tt;
}

void device_composition_data_init(void)
{
    device_composition_light_default_var_init();
    device_composition_light_default_status_init();
    return;
}

void device_composition_init_pub(void)
{
    bt_mesh_pub_msg_health_pub                    = NET_BUF_SIMPLE(1 + 3 + 0);
    bt_mesh_pub_msg_gen_onoff_srv_pub_root        = NET_BUF_SIMPLE(2 + 3);
    bt_mesh_pub_msg_gen_level_srv_pub_root        = NET_BUF_SIMPLE(2 + 5);
    bt_mesh_pub_msg_gen_power_onoff_srv_pub       = NET_BUF_SIMPLE(2 + 1);
    bt_mesh_pub_msg_gen_def_trans_time_srv_pub    = NET_BUF_SIMPLE(2 + 1);
    bt_mesh_pub_msg_light_lightness_srv_pub       = NET_BUF_SIMPLE(2 + 5);
    bt_mesh_pub_msg_light_ctl_srv_pub             = NET_BUF_SIMPLE(2 + 9);


    health_pub.msg                  = bt_mesh_pub_msg_health_pub;
    gen_onoff_srv_pub_root.msg      = bt_mesh_pub_msg_gen_onoff_srv_pub_root;
    gen_level_srv_pub_root.msg      = bt_mesh_pub_msg_gen_level_srv_pub_root;
    gen_power_onoff_srv_pub.msg     = bt_mesh_pub_msg_gen_power_onoff_srv_pub;
    gen_def_trans_time_srv_pub.msg  = bt_mesh_pub_msg_gen_def_trans_time_srv_pub;
    light_lightness_srv_pub.msg     = bt_mesh_pub_msg_light_lightness_srv_pub;
    light_ctl_srv_pub.msg           = bt_mesh_pub_msg_light_ctl_srv_pub;
    vnd_pub.msg                     = bt_mesh_pub_msg_vnd_pub;
}



