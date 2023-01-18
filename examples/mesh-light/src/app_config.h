#ifndef __APP_CONFIG_H
#define __APP_CONFIG_H

/* debug */
// assert.
#define APP_ASSERT_EN
// log
//#define APP_LOG_INFO_EN
//#define APP_LOG_DEBUG_EN
//#define APP_LOG_ERROR_EN

/* BUTTON TEST */
#define ENABLE_BUTTON_TEST

/* Mesh App Config */
#define ENABLE_MESH_ADV_BEARER
#define ENABLE_MESH_GATT_BEARER


/* storage */
// app info storage.
#define MESH_UUID_USE_FLASH
#define MESH_NAME_USE_FLASH
#define MESH_GATT_ADV_ADDR_USE_FLASH
#define MESH_BEACON_ADV_ADDR_USE_FLASH

// mesh stack tlv storage.
#define MESH_STACK_TLV_USE_FLASH

// eflash aligment.
#define EFLASH_WRITE_SUPPORT_OFFSET_ADDR_NOT_ALIGMENT

#endif
