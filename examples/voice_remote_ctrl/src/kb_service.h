#ifndef _KB_SERVICE_H
#define _KB_SERVICE_H

#include <stdint.h>

typedef struct my_kb_report
{
    uint8_t flags;
    uint8_t keycode;
    uint8_t mouse_key;
     int8_t mouse_move[2];
    uint8_t reserved;
} my_kb_report_t;

int kb_att_write_callback(uint16_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, 
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size);

uint16_t kb_att_read_callback(uint16_t connection_handle, uint16_t att_handle, uint16_t offset, 
                                  uint8_t * buffer, uint16_t buffer_size);

void init_kb_service(void);

void kb_send_report(void);

#endif
