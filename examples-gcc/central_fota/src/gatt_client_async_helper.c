#include <stdint.h>
#include <stddef.h>
#include "btstack_event.h"
#include "gatt_client.h"

const uint8_t *gatt_helper_value_query_result_parse(const uint8_t *packet, uint16_t size, uint16_t *value_size) {
    const gatt_event_value_packet_t *value =
        gatt_event_characteristic_value_query_result_parse(packet, size, value_size);
    return value_size ? value->value : NULL;
}
