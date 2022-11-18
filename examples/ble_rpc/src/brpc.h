#pragma once

#include <stdint.h>
#include "bluetooth.h"

void brpc_rx_byte(void *user_data, uint8_t c);

void brpc_handle_msg(void *msg_data);

void brpc_init(void);

void send_hci_event(const void *packet, int len);
void send_sm_event(const void *packet, int len);

void send_att_read_event(uint16_t conn_handle, uint16_t att_handle);

void send_att_write_event(uint16_t conn_handle, uint16_t att_handle,
    uint16_t transaction_mode,
    uint16_t offset,
    const void *buffer, int len);

void send_gatt_event(uint8_t packet_type, uint16_t conn_handle, const void *packet, int len);

void cleanup_packet_handlers(uint16_t conn_handle);
