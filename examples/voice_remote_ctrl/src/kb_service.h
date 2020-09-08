#ifndef _KB_SERVICE_H
#define _KB_SERVICE_H

#include <stdint.h>

#include "app_cfg.h"

#if (APP_TYPE == APP_ANDROID)
#define INPUT_REPORT_KEYS_MAX   6

#pragma pack (push, 1)
typedef struct
{
    uint8_t k_map;
} my_kb_report_t;
#pragma pack (pop)

#elif (APP_TYPE == APP_CUSTOMER)
typedef struct my_kb_report
{
    uint8_t flags;
    uint8_t keycode;
    uint8_t mouse_key;
     int8_t mouse_move[2];
    uint8_t reserved;
} my_kb_report_t;
#elif (APP_TYPE == APP_MIBOXS)
typedef struct my_kb_report
{
    uint8_t flags;
    uint8_t keycode;
    uint8_t k_map;
} my_kb_report_t;
#else
typedef struct my_kb_report
{
    uint8_t k_map;
} my_kb_report_t;
#endif

int kb_att_write_callback(uint16_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, 
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size);

uint16_t kb_att_read_callback(uint16_t connection_handle, uint16_t att_handle, uint16_t offset, 
                                  uint8_t * buffer, uint16_t buffer_size);

void init_kb_service(void);

void kb_send_report(void);

#endif
