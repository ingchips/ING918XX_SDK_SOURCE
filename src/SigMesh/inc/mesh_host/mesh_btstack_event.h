
#ifndef MESH_BTSTACK_EVENT_H
#define MESH_BTSTACK_EVENT_H
#include <stdint.h>
#include "btstack_util.h"




/*** 175
 * @brief Get subevent code for mesh event
 * @param event packet
 * @return subevent_code
 */
static inline uint8_t hci_event_mesh_meta_get_subevent_code(const uint8_t * event){
    return event[2];
}



/** 348
 * @brief Get field connection_handle from event HCI_EVENT_DISCONNECTION_COMPLETE
 * @param event packet
 * @return connection_handle
 * @note: btstack_type 2
 */
static inline uint16_t hci_event_disconnection_complete_get_connection_handle(const uint8_t * event){
    return little_endian_read_16(event, 3);
}


/** 3796
 * @brief Get field advertising_event_type from event HCI_SUBEVENT_LE_EXTENDED_ADVERTISING_REPORT
 * @param event packet
 * @return advertising_event_type
 * @note: btstack_type 1
 */
static inline uint8_t gap_event_advertising_report_get_advertising_event_type(const uint8_t * event){
    return event[2];
}
/**
 * @brief Get field address_type from event HCI_SUBEVENT_LE_EXTENDED_ADVERTISING_REPORT
 * @param event packet
 * @return address_type
 * @note: btstack_type 1
 */
static inline uint8_t gap_event_advertising_report_get_address_type(const uint8_t * event){
    return event[3];
}
/**
 * @brief Get field address from event HCI_SUBEVENT_LE_EXTENDED_ADVERTISING_REPORT
 * @param event packet
 * @param Pointer to storage for address
 * @note: btstack_type B
 */
static inline void gap_event_advertising_report_get_address(const uint8_t * event, bd_addr_t address){
    reverse_bytes(&event[4], address, 6);
}
/**
 * @brief Get field rssi from event HCI_SUBEVENT_LE_EXTENDED_ADVERTISING_REPORT
 * @param event packet
 * @return rssi
 * @note: btstack_type 1
 */
static inline uint8_t gap_event_advertising_report_get_rssi(const uint8_t * event){
    return event[10];
}
/**
 * @brief Get field data_length from event HCI_SUBEVENT_LE_EXTENDED_ADVERTISING_REPORT
 * @param event packet
 * @return data_length
 * @note: btstack_type J
 */
static inline uint8_t gap_event_advertising_report_get_data_length(const uint8_t * event){
    return event[11];
}
/**
 * @brief Get field data from event HCI_SUBEVENT_LE_EXTENDED_ADVERTISING_REPORT
 * @param event packet
 * @return data
 * @note: btstack_type V
 */
static inline const uint8_t * gap_event_advertising_report_get_data(const uint8_t * event){
    return &event[12];
}




/** 13025
 * @brief Get field status from event MESH_SUBEVENT_PB_TRANSPORT_PDU_SENT
 * @param event packet
 * @return status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_pb_transport_pdu_sent_get_status(const uint8_t * event){
    return event[3];
}

/**
 * @brief Get field status from event MESH_SUBEVENT_PB_TRANSPORT_LINK_OPEN
 * @param event packet
 * @return status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_pb_transport_link_open_get_status(const uint8_t * event){
    return event[3];
}
/**
 * @brief Get field pb_transport_cid from event MESH_SUBEVENT_PB_TRANSPORT_LINK_OPEN
 * @param event packet
 * @return pb_transport_cid
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_pb_transport_link_open_get_pb_transport_cid(const uint8_t * event){
    return little_endian_read_16(event, 4);
}
/**
 * @brief Get field pb_type from event MESH_SUBEVENT_PB_TRANSPORT_LINK_OPEN
 * @param event packet
 * @return pb_type
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_pb_transport_link_open_get_pb_type(const uint8_t * event){
    return event[6];
}

/**
 * @brief Get field pb_transport_cid from event MESH_SUBEVENT_PB_TRANSPORT_LINK_CLOSED
 * @param event packet
 * @return pb_transport_cid
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_pb_transport_link_closed_get_pb_transport_cid(const uint8_t * event){
    return event[3];
}
/**
 * @brief Get field reason from event MESH_SUBEVENT_PB_TRANSPORT_LINK_CLOSED
 * @param event packet
 * @return reason
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_pb_transport_link_closed_get_reason(const uint8_t * event){
    return little_endian_read_16(event, 4);
}

/**
 * @brief Get field pb_transport_cid from event MESH_SUBEVENT_PB_PROV_ATTENTION_TIMER
 * @param event packet
 * @return pb_transport_cid
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_pb_prov_attention_timer_get_pb_transport_cid(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field attention_time from event MESH_SUBEVENT_PB_PROV_ATTENTION_TIMER
 * @param event packet
 * @return attention_time
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_pb_prov_attention_timer_get_attention_time(const uint8_t * event){
    return event[5];
}

/**
 * @brief Get field pb_transport_cid from event MESH_SUBEVENT_PB_PROV_START_EMIT_PUBLIC_KEY_OOB
 * @param event packet
 * @return pb_transport_cid
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_pb_prov_start_emit_public_key_oob_get_pb_transport_cid(const uint8_t * event){
    return little_endian_read_16(event, 3);
}

/**
 * @brief Get field pb_transport_cid from event MESH_SUBEVENT_PB_PROV_STOP_EMIT_PUBLIC_KEY_OOB
 * @param event packet
 * @return pb_transport_cid
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_pb_prov_stop_emit_public_key_oob_get_pb_transport_cid(const uint8_t * event){
    return little_endian_read_16(event, 3);
}

/**
 * @brief Get field pb_transport_cid from event MESH_SUBEVENT_PB_PROV_INPUT_OOB_REQUEST
 * @param event packet
 * @return pb_transport_cid
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_pb_prov_input_oob_request_get_pb_transport_cid(const uint8_t * event){
    return little_endian_read_16(event, 3);
}

/**
 * @brief Get field pb_transport_cid from event MESH_SUBEVENT_PB_PROV_START_EMIT_OUTPUT_OOB
 * @param event packet
 * @return pb_transport_cid
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_pb_prov_start_emit_output_oob_get_pb_transport_cid(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field output_oob from event MESH_SUBEVENT_PB_PROV_START_EMIT_OUTPUT_OOB
 * @param event packet
 * @return output_oob
 * @note: btstack_type 4
 */
static inline uint32_t mesh_subevent_pb_prov_start_emit_output_oob_get_output_oob(const uint8_t * event){
    return little_endian_read_32(event, 5);
}

/**
 * @brief Get field pb_transport_cid from event MESH_SUBEVENT_PB_PROV_STOP_EMIT_OUTPUT_OOB
 * @param event packet
 * @return pb_transport_cid
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_pb_prov_stop_emit_output_oob_get_pb_transport_cid(const uint8_t * event){
    return little_endian_read_16(event, 3);
}

/**
 * @brief Get field pb_transport_cid from event MESH_SUBEVENT_PB_PROV_START_RECEIVE_PUBLIC_KEY_OOB
 * @param event packet
 * @return pb_transport_cid
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_pb_prov_start_receive_public_key_oob_get_pb_transport_cid(const uint8_t * event){
    return little_endian_read_16(event, 3);
}

/**
 * @brief Get field pb_transport_cid from event MESH_SUBEVENT_PB_PROV_STOP_RECEIVE_PUBLIC_KEY_OOB
 * @param event packet
 * @return pb_transport_cid
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_pb_prov_stop_receive_public_key_oob_get_pb_transport_cid(const uint8_t * event){
    return little_endian_read_16(event, 3);
}

/**
 * @brief Get field pb_transport_cid from event MESH_SUBEVENT_PB_PROV_OUTPUT_OOB_REQUEST
 * @param event packet
 * @return pb_transport_cid
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_pb_prov_output_oob_request_get_pb_transport_cid(const uint8_t * event){
    return little_endian_read_16(event, 3);
}

/**
 * @brief Get field pb_transport_cid from event MESH_SUBEVENT_PB_PROV_START_EMIT_INPUT_OOB
 * @param event packet
 * @return pb_transport_cid
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_pb_prov_start_emit_input_oob_get_pb_transport_cid(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field output_oob from event MESH_SUBEVENT_PB_PROV_START_EMIT_INPUT_OOB
 * @param event packet
 * @return output_oob
 * @note: btstack_type 4
 */
static inline uint32_t mesh_subevent_pb_prov_start_emit_input_oob_get_output_oob(const uint8_t * event){
    return little_endian_read_32(event, 5);
}

/**
 * @brief Get field pb_transport_cid from event MESH_SUBEVENT_PB_PROV_STOP_EMIT_INPUT_OOB
 * @param event packet
 * @return pb_transport_cid
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_pb_prov_stop_emit_input_oob_get_pb_transport_cid(const uint8_t * event){
    return little_endian_read_16(event, 3);
}

/**
 * @brief Get field pb_transport_cid from event MESH_SUBEVENT_PB_PROV_CAPABILITIES
 * @param event packet
 * @return pb_transport_cid
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_pb_prov_capabilities_get_pb_transport_cid(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field num_elements from event MESH_SUBEVENT_PB_PROV_CAPABILITIES
 * @param event packet
 * @return num_elements
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_pb_prov_capabilities_get_num_elements(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field algorithms from event MESH_SUBEVENT_PB_PROV_CAPABILITIES
 * @param event packet
 * @return algorithms
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_pb_prov_capabilities_get_algorithms(const uint8_t * event){
    return little_endian_read_16(event, 6);
}
/**
 * @brief Get field public_key from event MESH_SUBEVENT_PB_PROV_CAPABILITIES
 * @param event packet
 * @return public_key
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_pb_prov_capabilities_get_public_key(const uint8_t * event){
    return event[8];
}
/**
 * @brief Get field static_oob_type from event MESH_SUBEVENT_PB_PROV_CAPABILITIES
 * @param event packet
 * @return static_oob_type
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_pb_prov_capabilities_get_static_oob_type(const uint8_t * event){
    return event[9];
}
/**
 * @brief Get field output_oob_size from event MESH_SUBEVENT_PB_PROV_CAPABILITIES
 * @param event packet
 * @return output_oob_size
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_pb_prov_capabilities_get_output_oob_size(const uint8_t * event){
    return event[10];
}
/**
 * @brief Get field output_oob_action from event MESH_SUBEVENT_PB_PROV_CAPABILITIES
 * @param event packet
 * @return output_oob_action
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_pb_prov_capabilities_get_output_oob_action(const uint8_t * event){
    return little_endian_read_16(event, 11);
}
/**
 * @brief Get field input_oob_size from event MESH_SUBEVENT_PB_PROV_CAPABILITIES
 * @param event packet
 * @return input_oob_size
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_pb_prov_capabilities_get_input_oob_size(const uint8_t * event){
    return event[13];
}
/**
 * @brief Get field input_oob_action from event MESH_SUBEVENT_PB_PROV_CAPABILITIES
 * @param event packet
 * @return input_oob_action
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_pb_prov_capabilities_get_input_oob_action(const uint8_t * event){
    return little_endian_read_16(event, 14);
}

/**
 * @brief Get field pb_transport_cid from event MESH_SUBEVENT_PB_PROV_COMPLETE
 * @param event packet
 * @return pb_transport_cid
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_pb_prov_complete_get_pb_transport_cid(const uint8_t * event){
    return little_endian_read_16(event, 3);
}

/**
 * @brief Get field attention_time from event MESH_SUBEVENT_ATTENTION_TIMER
 * @param event packet
 * @return attention_time
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_attention_timer_get_attention_time(const uint8_t * event){
    return event[3];
}

/**
 * @brief Get field con_handle from event MESH_SUBEVENT_PROXY_CONNECTED
 * @param event packet
 * @return con_handle
 * @note: btstack_type H
 */
static inline hci_con_handle_t mesh_subevent_proxy_connected_get_con_handle(const uint8_t * event){
    return little_endian_read_16(event, 3);
}

/**
 * @brief Get field con_handle from event MESH_SUBEVENT_PROXY_PDU_SENT
 * @param event packet
 * @return con_handle
 * @note: btstack_type H
 */
static inline hci_con_handle_t mesh_subevent_proxy_pdu_sent_get_con_handle(const uint8_t * event){
    return little_endian_read_16(event, 3);
}

/**
 * @brief Get field con_handle from event MESH_SUBEVENT_PROXY_DISCONNECTED
 * @param event packet
 * @return con_handle
 * @note: btstack_type H
 */
static inline hci_con_handle_t mesh_subevent_proxy_disconnected_get_con_handle(const uint8_t * event){
    return little_endian_read_16(event, 3);
}

/**
 * @brief Get field con_handle from event MESH_SUBEVENT_MESSAGE_SENT
 * @param event packet
 * @return con_handle
 * @note: btstack_type H
 */
static inline hci_con_handle_t mesh_subevent_message_sent_get_con_handle(const uint8_t * event){
    return little_endian_read_16(event, 3);
}

/**
 * @brief Get field element_index from event MESH_SUBEVENT_STATE_UPDATE_BOOL
 * @param event packet
 * @return element_index
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_state_update_bool_get_element_index(const uint8_t * event){
    return event[3];
}
/**
 * @brief Get field model_identifier from event MESH_SUBEVENT_STATE_UPDATE_BOOL
 * @param event packet
 * @return model_identifier
 * @note: btstack_type 4
 */
static inline uint32_t mesh_subevent_state_update_bool_get_model_identifier(const uint8_t * event){
    return little_endian_read_32(event, 4);
}
/**
 * @brief Get field state_identifier from event MESH_SUBEVENT_STATE_UPDATE_BOOL
 * @param event packet
 * @return state_identifier
 * @note: btstack_type 4
 */
static inline uint32_t mesh_subevent_state_update_bool_get_state_identifier(const uint8_t * event){
    return little_endian_read_32(event, 8);
}
/**
 * @brief Get field reason from event MESH_SUBEVENT_STATE_UPDATE_BOOL
 * @param event packet
 * @return reason
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_state_update_bool_get_reason(const uint8_t * event){
    return event[12];
}
/**
 * @brief Get field value from event MESH_SUBEVENT_STATE_UPDATE_BOOL
 * @param event packet
 * @return value
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_state_update_bool_get_value(const uint8_t * event){
    return event[13];
}

/**
 * @brief Get field element_index from event MESH_SUBEVENT_STATE_UPDATE_INT16
 * @param event packet
 * @return element_index
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_state_update_int16_get_element_index(const uint8_t * event){
    return event[3];
}
/**
 * @brief Get field model_identifier from event MESH_SUBEVENT_STATE_UPDATE_INT16
 * @param event packet
 * @return model_identifier
 * @note: btstack_type 4
 */
static inline uint32_t mesh_subevent_state_update_int16_get_model_identifier(const uint8_t * event){
    return little_endian_read_32(event, 4);
}
/**
 * @brief Get field state_identifier from event MESH_SUBEVENT_STATE_UPDATE_INT16
 * @param event packet
 * @return state_identifier
 * @note: btstack_type 4
 */
static inline uint32_t mesh_subevent_state_update_int16_get_state_identifier(const uint8_t * event){
    return little_endian_read_32(event, 8);
}
/**
 * @brief Get field reason from event MESH_SUBEVENT_STATE_UPDATE_INT16
 * @param event packet
 * @return reason
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_state_update_int16_get_reason(const uint8_t * event){
    return event[12];
}
/**
 * @brief Get field value from event MESH_SUBEVENT_STATE_UPDATE_INT16
 * @param event packet
 * @return value
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_state_update_int16_get_value(const uint8_t * event){
    return little_endian_read_16(event, 13);
}

/**
 * @brief Get field element_index from event MESH_SUBEVENT_MESSAGE_NOT_ACKNOWLEDGED
 * @param event packet
 * @return element_index
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_message_not_acknowledged_get_element_index(const uint8_t * event){
    return event[3];
}
/**
 * @brief Get field model_identifier from event MESH_SUBEVENT_MESSAGE_NOT_ACKNOWLEDGED
 * @param event packet
 * @return model_identifier
 * @note: btstack_type 4
 */
static inline uint32_t mesh_subevent_message_not_acknowledged_get_model_identifier(const uint8_t * event){
    return little_endian_read_32(event, 4);
}
/**
 * @brief Get field opcode from event MESH_SUBEVENT_MESSAGE_NOT_ACKNOWLEDGED
 * @param event packet
 * @return opcode
 * @note: btstack_type 4
 */
static inline uint32_t mesh_subevent_message_not_acknowledged_get_opcode(const uint8_t * event){
    return little_endian_read_32(event, 8);
}
/**
 * @brief Get field dest from event MESH_SUBEVENT_MESSAGE_NOT_ACKNOWLEDGED
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_message_not_acknowledged_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 12);
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_GENERIC_ON_OFF
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_generic_on_off_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field status from event MESH_SUBEVENT_GENERIC_ON_OFF
 * @param event packet
 * @return status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_generic_on_off_get_status(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field present_value from event MESH_SUBEVENT_GENERIC_ON_OFF
 * @param event packet
 * @return present_value
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_generic_on_off_get_present_value(const uint8_t * event){
    return event[6];
}
/**
 * @brief Get field target_value from event MESH_SUBEVENT_GENERIC_ON_OFF
 * @param event packet
 * @return target_value
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_generic_on_off_get_target_value(const uint8_t * event){
    return event[7];
}
/**
 * @brief Get field remaining_time_ms from event MESH_SUBEVENT_GENERIC_ON_OFF
 * @param event packet
 * @return remaining_time_ms
 * @note: btstack_type 4
 */
static inline uint32_t mesh_subevent_generic_on_off_get_remaining_time_ms(const uint8_t * event){
    return little_endian_read_32(event, 8);
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_GENERIC_LEVEL
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_generic_level_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field status from event MESH_SUBEVENT_GENERIC_LEVEL
 * @param event packet
 * @return status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_generic_level_get_status(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field present_value from event MESH_SUBEVENT_GENERIC_LEVEL
 * @param event packet
 * @return present_value
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_generic_level_get_present_value(const uint8_t * event){
    return little_endian_read_16(event, 6);
}
/**
 * @brief Get field target_value from event MESH_SUBEVENT_GENERIC_LEVEL
 * @param event packet
 * @return target_value
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_generic_level_get_target_value(const uint8_t * event){
    return little_endian_read_16(event, 8);
}
/**
 * @brief Get field remaining_time_ms from event MESH_SUBEVENT_GENERIC_LEVEL
 * @param event packet
 * @return remaining_time_ms
 * @note: btstack_type 4
 */
static inline uint32_t mesh_subevent_generic_level_get_remaining_time_ms(const uint8_t * event){
    return little_endian_read_32(event, 10);
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_HEALTH_PERFORM_TEST
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_health_perform_test_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field netkey_index from event MESH_SUBEVENT_HEALTH_PERFORM_TEST
 * @param event packet
 * @return netkey_index
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_health_perform_test_get_netkey_index(const uint8_t * event){
    return little_endian_read_16(event, 5);
}
/**
 * @brief Get field appkey_index from event MESH_SUBEVENT_HEALTH_PERFORM_TEST
 * @param event packet
 * @return appkey_index
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_health_perform_test_get_appkey_index(const uint8_t * event){
    return little_endian_read_16(event, 7);
}
/**
 * @brief Get field company_id from event MESH_SUBEVENT_HEALTH_PERFORM_TEST
 * @param event packet
 * @return company_id
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_health_perform_test_get_company_id(const uint8_t * event){
    return little_endian_read_16(event, 9);
}
/**
 * @brief Get field test_id from event MESH_SUBEVENT_HEALTH_PERFORM_TEST
 * @param event packet
 * @return test_id
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_health_perform_test_get_test_id(const uint8_t * event){
    return event[11];
}
/**
 * @brief Get field acknowledged from event MESH_SUBEVENT_HEALTH_PERFORM_TEST
 * @param event packet
 * @return acknowledged
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_health_perform_test_get_acknowledged(const uint8_t * event){
    return event[12];
}

/**
 * @brief Get field element_index from event MESH_SUBEVENT_HEALTH_ATTENTION_TIMER_CHANGED
 * @param event packet
 * @return element_index
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_health_attention_timer_changed_get_element_index(const uint8_t * event){
    return event[3];
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_GENERIC_DEFAULT_TRANSITION_TIME
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_generic_default_transition_time_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field status from event MESH_SUBEVENT_GENERIC_DEFAULT_TRANSITION_TIME
 * @param event packet
 * @return status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_generic_default_transition_time_get_status(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field transition_time_gdtt from event MESH_SUBEVENT_GENERIC_DEFAULT_TRANSITION_TIME
 * @param event packet
 * @return transition_time_gdtt
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_generic_default_transition_time_get_transition_time_gdtt(const uint8_t * event){
    return event[6];
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_CONFIGURATION_BEACON
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_beacon_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field foundation_status from event MESH_SUBEVENT_CONFIGURATION_BEACON
 * @param event packet
 * @return foundation_status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_beacon_get_foundation_status(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field secure_network_beacon_state from event MESH_SUBEVENT_CONFIGURATION_BEACON
 * @param event packet
 * @return secure_network_beacon_state
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_beacon_get_secure_network_beacon_state(const uint8_t * event){
    return event[6];
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_CONFIGURATION_DEFAULT_TTL
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_default_ttl_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field foundation_status from event MESH_SUBEVENT_CONFIGURATION_DEFAULT_TTL
 * @param event packet
 * @return foundation_status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_default_ttl_get_foundation_status(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field default_ttl from event MESH_SUBEVENT_CONFIGURATION_DEFAULT_TTL
 * @param event packet
 * @return default_ttl
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_default_ttl_get_default_ttl(const uint8_t * event){
    return event[6];
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_CONFIGURATION_GATT_PROXY
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_gatt_proxy_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field foundation_status from event MESH_SUBEVENT_CONFIGURATION_GATT_PROXY
 * @param event packet
 * @return foundation_status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_gatt_proxy_get_foundation_status(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field gatt_proxy_state from event MESH_SUBEVENT_CONFIGURATION_GATT_PROXY
 * @param event packet
 * @return gatt_proxy_state
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_gatt_proxy_get_gatt_proxy_state(const uint8_t * event){
    return event[6];
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_CONFIGURATION_RELAY
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_relay_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field foundation_status from event MESH_SUBEVENT_CONFIGURATION_RELAY
 * @param event packet
 * @return foundation_status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_relay_get_foundation_status(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field relay from event MESH_SUBEVENT_CONFIGURATION_RELAY
 * @param event packet
 * @return relay
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_relay_get_relay(const uint8_t * event){
    return event[6];
}
/**
 * @brief Get field retransmit_count from event MESH_SUBEVENT_CONFIGURATION_RELAY
 * @param event packet
 * @return retransmit_count
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_relay_get_retransmit_count(const uint8_t * event){
    return event[7];
}
/**
 * @brief Get field retransmit_interval_ms from event MESH_SUBEVENT_CONFIGURATION_RELAY
 * @param event packet
 * @return retransmit_interval_ms
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_relay_get_retransmit_interval_ms(const uint8_t * event){
    return event[8];
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_CONFIGURATION_MODEL_PUBLICATION
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_model_publication_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field foundation_status from event MESH_SUBEVENT_CONFIGURATION_MODEL_PUBLICATION
 * @param event packet
 * @return foundation_status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_model_publication_get_foundation_status(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field publish_address from event MESH_SUBEVENT_CONFIGURATION_MODEL_PUBLICATION
 * @param event packet
 * @return publish_address
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_model_publication_get_publish_address(const uint8_t * event){
    return little_endian_read_16(event, 6);
}
/**
 * @brief Get field appkey_index from event MESH_SUBEVENT_CONFIGURATION_MODEL_PUBLICATION
 * @param event packet
 * @return appkey_index
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_model_publication_get_appkey_index(const uint8_t * event){
    return little_endian_read_16(event, 8);
}
/**
 * @brief Get field credential_flag from event MESH_SUBEVENT_CONFIGURATION_MODEL_PUBLICATION
 * @param event packet
 * @return credential_flag
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_model_publication_get_credential_flag(const uint8_t * event){
    return event[10];
}
/**
 * @brief Get field publish_ttl from event MESH_SUBEVENT_CONFIGURATION_MODEL_PUBLICATION
 * @param event packet
 * @return publish_ttl
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_model_publication_get_publish_ttl(const uint8_t * event){
    return event[11];
}
/**
 * @brief Get field publish_period from event MESH_SUBEVENT_CONFIGURATION_MODEL_PUBLICATION
 * @param event packet
 * @return publish_period
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_model_publication_get_publish_period(const uint8_t * event){
    return event[12];
}
/**
 * @brief Get field publish_retransmit_count from event MESH_SUBEVENT_CONFIGURATION_MODEL_PUBLICATION
 * @param event packet
 * @return publish_retransmit_count
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_model_publication_get_publish_retransmit_count(const uint8_t * event){
    return event[13];
}
/**
 * @brief Get field publish_retransmit_interval_steps from event MESH_SUBEVENT_CONFIGURATION_MODEL_PUBLICATION
 * @param event packet
 * @return publish_retransmit_interval_steps
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_model_publication_get_publish_retransmit_interval_steps(const uint8_t * event){
    return event[14];
}
/**
 * @brief Get field model_identifier from event MESH_SUBEVENT_CONFIGURATION_MODEL_PUBLICATION
 * @param event packet
 * @return model_identifier
 * @note: btstack_type 4
 */
static inline uint32_t mesh_subevent_configuration_model_publication_get_model_identifier(const uint8_t * event){
    return little_endian_read_32(event, 15);
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_CONFIGURATION_MODEL_SUBSCRIPTION
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_model_subscription_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field foundation_status from event MESH_SUBEVENT_CONFIGURATION_MODEL_SUBSCRIPTION
 * @param event packet
 * @return foundation_status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_model_subscription_get_foundation_status(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field address from event MESH_SUBEVENT_CONFIGURATION_MODEL_SUBSCRIPTION
 * @param event packet
 * @return address
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_model_subscription_get_address(const uint8_t * event){
    return little_endian_read_16(event, 6);
}
/**
 * @brief Get field model_identifier from event MESH_SUBEVENT_CONFIGURATION_MODEL_SUBSCRIPTION
 * @param event packet
 * @return model_identifier
 * @note: btstack_type 4
 */
static inline uint32_t mesh_subevent_configuration_model_subscription_get_model_identifier(const uint8_t * event){
    return little_endian_read_32(event, 8);
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_CONFIGURATION_MODEL_SUBSCRIPTION_LIST_ITEM
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_model_subscription_list_item_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field foundation_status from event MESH_SUBEVENT_CONFIGURATION_MODEL_SUBSCRIPTION_LIST_ITEM
 * @param event packet
 * @return foundation_status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_model_subscription_list_item_get_foundation_status(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field model_identifier from event MESH_SUBEVENT_CONFIGURATION_MODEL_SUBSCRIPTION_LIST_ITEM
 * @param event packet
 * @return model_identifier
 * @note: btstack_type 4
 */
static inline uint32_t mesh_subevent_configuration_model_subscription_list_item_get_model_identifier(const uint8_t * event){
    return little_endian_read_32(event, 6);
}
/**
 * @brief Get field num_subscription_addresses from event MESH_SUBEVENT_CONFIGURATION_MODEL_SUBSCRIPTION_LIST_ITEM
 * @param event packet
 * @return num_subscription_addresses
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_model_subscription_list_item_get_num_subscription_addresses(const uint8_t * event){
    return event[10];
}
/**
 * @brief Get field subscription_address_pos from event MESH_SUBEVENT_CONFIGURATION_MODEL_SUBSCRIPTION_LIST_ITEM
 * @param event packet
 * @return subscription_address_pos
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_model_subscription_list_item_get_subscription_address_pos(const uint8_t * event){
    return event[11];
}
/**
 * @brief Get field subscription_address_item from event MESH_SUBEVENT_CONFIGURATION_MODEL_SUBSCRIPTION_LIST_ITEM
 * @param event packet
 * @return subscription_address_item
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_model_subscription_list_item_get_subscription_address_item(const uint8_t * event){
    return little_endian_read_16(event, 12);
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_CONFIGURATION_NETKEY_INDEX
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_netkey_index_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field foundation_status from event MESH_SUBEVENT_CONFIGURATION_NETKEY_INDEX
 * @param event packet
 * @return foundation_status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_netkey_index_get_foundation_status(const uint8_t * event){
    return event[5];
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_CONFIGURATION_NETKEY_INDEX_LIST_ITEM
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_netkey_index_list_item_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field foundation_status from event MESH_SUBEVENT_CONFIGURATION_NETKEY_INDEX_LIST_ITEM
 * @param event packet
 * @return foundation_status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_netkey_index_list_item_get_foundation_status(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field num_netkey_indexes from event MESH_SUBEVENT_CONFIGURATION_NETKEY_INDEX_LIST_ITEM
 * @param event packet
 * @return num_netkey_indexes
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_netkey_index_list_item_get_num_netkey_indexes(const uint8_t * event){
    return event[6];
}
/**
 * @brief Get field netkey_index_pos from event MESH_SUBEVENT_CONFIGURATION_NETKEY_INDEX_LIST_ITEM
 * @param event packet
 * @return netkey_index_pos
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_netkey_index_list_item_get_netkey_index_pos(const uint8_t * event){
    return event[7];
}
/**
 * @brief Get field netkey_index_item from event MESH_SUBEVENT_CONFIGURATION_NETKEY_INDEX_LIST_ITEM
 * @param event packet
 * @return netkey_index_item
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_netkey_index_list_item_get_netkey_index_item(const uint8_t * event){
    return little_endian_read_16(event, 8);
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_CONFIGURATION_APPKEY_INDEX
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_appkey_index_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field foundation_status from event MESH_SUBEVENT_CONFIGURATION_APPKEY_INDEX
 * @param event packet
 * @return foundation_status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_appkey_index_get_foundation_status(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field netkey_index_item from event MESH_SUBEVENT_CONFIGURATION_APPKEY_INDEX
 * @param event packet
 * @return netkey_index_item
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_appkey_index_get_netkey_index_item(const uint8_t * event){
    return little_endian_read_16(event, 6);
}
/**
 * @brief Get field appkey_index_item from event MESH_SUBEVENT_CONFIGURATION_APPKEY_INDEX
 * @param event packet
 * @return appkey_index_item
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_appkey_index_get_appkey_index_item(const uint8_t * event){
    return little_endian_read_16(event, 8);
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_CONFIGURATION_APPKEY_INDEX_LIST_ITEM
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_appkey_index_list_item_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field foundation_status from event MESH_SUBEVENT_CONFIGURATION_APPKEY_INDEX_LIST_ITEM
 * @param event packet
 * @return foundation_status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_appkey_index_list_item_get_foundation_status(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field netkey_index from event MESH_SUBEVENT_CONFIGURATION_APPKEY_INDEX_LIST_ITEM
 * @param event packet
 * @return netkey_index
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_appkey_index_list_item_get_netkey_index(const uint8_t * event){
    return little_endian_read_16(event, 6);
}
/**
 * @brief Get field num_appkey_indexes from event MESH_SUBEVENT_CONFIGURATION_APPKEY_INDEX_LIST_ITEM
 * @param event packet
 * @return num_appkey_indexes
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_appkey_index_list_item_get_num_appkey_indexes(const uint8_t * event){
    return event[8];
}
/**
 * @brief Get field appkey_index_pos from event MESH_SUBEVENT_CONFIGURATION_APPKEY_INDEX_LIST_ITEM
 * @param event packet
 * @return appkey_index_pos
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_appkey_index_list_item_get_appkey_index_pos(const uint8_t * event){
    return event[9];
}
/**
 * @brief Get field netkey_index_item from event MESH_SUBEVENT_CONFIGURATION_APPKEY_INDEX_LIST_ITEM
 * @param event packet
 * @return netkey_index_item
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_appkey_index_list_item_get_netkey_index_item(const uint8_t * event){
    return little_endian_read_16(event, 10);
}
/**
 * @brief Get field appkey_index_item from event MESH_SUBEVENT_CONFIGURATION_APPKEY_INDEX_LIST_ITEM
 * @param event packet
 * @return appkey_index_item
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_appkey_index_list_item_get_appkey_index_item(const uint8_t * event){
    return little_endian_read_16(event, 12);
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_CONFIGURATION_NODE_IDENTITY
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_node_identity_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field foundation_status from event MESH_SUBEVENT_CONFIGURATION_NODE_IDENTITY
 * @param event packet
 * @return foundation_status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_node_identity_get_foundation_status(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field netkey_index_item from event MESH_SUBEVENT_CONFIGURATION_NODE_IDENTITY
 * @param event packet
 * @return netkey_index_item
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_node_identity_get_netkey_index_item(const uint8_t * event){
    return little_endian_read_16(event, 6);
}
/**
 * @brief Get field identity_status from event MESH_SUBEVENT_CONFIGURATION_NODE_IDENTITY
 * @param event packet
 * @return identity_status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_node_identity_get_identity_status(const uint8_t * event){
    return event[8];
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_CONFIGURATION_MODEL_APP
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_model_app_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field foundation_status from event MESH_SUBEVENT_CONFIGURATION_MODEL_APP
 * @param event packet
 * @return foundation_status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_model_app_get_foundation_status(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field appkey_index from event MESH_SUBEVENT_CONFIGURATION_MODEL_APP
 * @param event packet
 * @return appkey_index
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_model_app_get_appkey_index(const uint8_t * event){
    return little_endian_read_16(event, 6);
}
/**
 * @brief Get field model_identifier from event MESH_SUBEVENT_CONFIGURATION_MODEL_APP
 * @param event packet
 * @return model_identifier
 * @note: btstack_type 4
 */
static inline uint32_t mesh_subevent_configuration_model_app_get_model_identifier(const uint8_t * event){
    return little_endian_read_32(event, 8);
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_CONFIGURATION_MODEL_APP_LIST_ITEM
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_model_app_list_item_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field foundation_status from event MESH_SUBEVENT_CONFIGURATION_MODEL_APP_LIST_ITEM
 * @param event packet
 * @return foundation_status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_model_app_list_item_get_foundation_status(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field model_id from event MESH_SUBEVENT_CONFIGURATION_MODEL_APP_LIST_ITEM
 * @param event packet
 * @return model_id
 * @note: btstack_type 4
 */
static inline uint32_t mesh_subevent_configuration_model_app_list_item_get_model_id(const uint8_t * event){
    return little_endian_read_32(event, 6);
}
/**
 * @brief Get field num_appkey_indexes from event MESH_SUBEVENT_CONFIGURATION_MODEL_APP_LIST_ITEM
 * @param event packet
 * @return num_appkey_indexes
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_model_app_list_item_get_num_appkey_indexes(const uint8_t * event){
    return event[10];
}
/**
 * @brief Get field appkey_index_pos from event MESH_SUBEVENT_CONFIGURATION_MODEL_APP_LIST_ITEM
 * @param event packet
 * @return appkey_index_pos
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_model_app_list_item_get_appkey_index_pos(const uint8_t * event){
    return event[11];
}
/**
 * @brief Get field appkey_index_item from event MESH_SUBEVENT_CONFIGURATION_MODEL_APP_LIST_ITEM
 * @param event packet
 * @return appkey_index_item
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_model_app_list_item_get_appkey_index_item(const uint8_t * event){
    return little_endian_read_16(event, 12);
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_CONFIGURATION_NODE_RESET
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_node_reset_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field foundation_status from event MESH_SUBEVENT_CONFIGURATION_NODE_RESET
 * @param event packet
 * @return foundation_status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_node_reset_get_foundation_status(const uint8_t * event){
    return event[5];
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_CONFIGURATION_FRIEND
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_friend_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field foundation_status from event MESH_SUBEVENT_CONFIGURATION_FRIEND
 * @param event packet
 * @return foundation_status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_friend_get_foundation_status(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field friend_state from event MESH_SUBEVENT_CONFIGURATION_FRIEND
 * @param event packet
 * @return friend_state
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_friend_get_friend_state(const uint8_t * event){
    return event[6];
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_CONFIGURATION_KEY_REFRESH_PHASE
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_key_refresh_phase_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field foundation_status from event MESH_SUBEVENT_CONFIGURATION_KEY_REFRESH_PHASE
 * @param event packet
 * @return foundation_status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_key_refresh_phase_get_foundation_status(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field netkey_index from event MESH_SUBEVENT_CONFIGURATION_KEY_REFRESH_PHASE
 * @param event packet
 * @return netkey_index
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_key_refresh_phase_get_netkey_index(const uint8_t * event){
    return little_endian_read_16(event, 6);
}
/**
 * @brief Get field phase from event MESH_SUBEVENT_CONFIGURATION_KEY_REFRESH_PHASE
 * @param event packet
 * @return phase
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_key_refresh_phase_get_phase(const uint8_t * event){
    return event[8];
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_CONFIGURATION_HEARTBEAT_PUBLICATION
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_heartbeat_publication_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field foundation_status from event MESH_SUBEVENT_CONFIGURATION_HEARTBEAT_PUBLICATION
 * @param event packet
 * @return foundation_status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_heartbeat_publication_get_foundation_status(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field heartbeat_destination from event MESH_SUBEVENT_CONFIGURATION_HEARTBEAT_PUBLICATION
 * @param event packet
 * @return heartbeat_destination
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_heartbeat_publication_get_heartbeat_destination(const uint8_t * event){
    return little_endian_read_16(event, 6);
}
/**
 * @brief Get field count_S from event MESH_SUBEVENT_CONFIGURATION_HEARTBEAT_PUBLICATION
 * @param event packet
 * @return count_S
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_heartbeat_publication_get_count_S(const uint8_t * event){
    return little_endian_read_16(event, 8);
}
/**
 * @brief Get field period_S from event MESH_SUBEVENT_CONFIGURATION_HEARTBEAT_PUBLICATION
 * @param event packet
 * @return period_S
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_heartbeat_publication_get_period_S(const uint8_t * event){
    return little_endian_read_16(event, 10);
}
/**
 * @brief Get field ttl from event MESH_SUBEVENT_CONFIGURATION_HEARTBEAT_PUBLICATION
 * @param event packet
 * @return ttl
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_heartbeat_publication_get_ttl(const uint8_t * event){
    return event[12];
}
/**
 * @brief Get field features from event MESH_SUBEVENT_CONFIGURATION_HEARTBEAT_PUBLICATION
 * @param event packet
 * @return features
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_heartbeat_publication_get_features(const uint8_t * event){
    return little_endian_read_16(event, 13);
}
/**
 * @brief Get field netkey_index from event MESH_SUBEVENT_CONFIGURATION_HEARTBEAT_PUBLICATION
 * @param event packet
 * @return netkey_index
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_heartbeat_publication_get_netkey_index(const uint8_t * event){
    return little_endian_read_16(event, 15);
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_CONFIGURATION_HEARTBEAT_SUBSCRIPTION
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_heartbeat_subscription_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field foundation_status from event MESH_SUBEVENT_CONFIGURATION_HEARTBEAT_SUBSCRIPTION
 * @param event packet
 * @return foundation_status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_heartbeat_subscription_get_foundation_status(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field heartbeat_destination from event MESH_SUBEVENT_CONFIGURATION_HEARTBEAT_SUBSCRIPTION
 * @param event packet
 * @return heartbeat_destination
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_heartbeat_subscription_get_heartbeat_destination(const uint8_t * event){
    return little_endian_read_16(event, 6);
}
/**
 * @brief Get field heartbeat_source from event MESH_SUBEVENT_CONFIGURATION_HEARTBEAT_SUBSCRIPTION
 * @param event packet
 * @return heartbeat_source
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_heartbeat_subscription_get_heartbeat_source(const uint8_t * event){
    return little_endian_read_16(event, 8);
}
/**
 * @brief Get field count_S from event MESH_SUBEVENT_CONFIGURATION_HEARTBEAT_SUBSCRIPTION
 * @param event packet
 * @return count_S
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_heartbeat_subscription_get_count_S(const uint8_t * event){
    return little_endian_read_16(event, 10);
}
/**
 * @brief Get field period_S from event MESH_SUBEVENT_CONFIGURATION_HEARTBEAT_SUBSCRIPTION
 * @param event packet
 * @return period_S
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_heartbeat_subscription_get_period_S(const uint8_t * event){
    return little_endian_read_16(event, 12);
}
/**
 * @brief Get field min_hops from event MESH_SUBEVENT_CONFIGURATION_HEARTBEAT_SUBSCRIPTION
 * @param event packet
 * @return min_hops
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_heartbeat_subscription_get_min_hops(const uint8_t * event){
    return event[14];
}
/**
 * @brief Get field max_hops from event MESH_SUBEVENT_CONFIGURATION_HEARTBEAT_SUBSCRIPTION
 * @param event packet
 * @return max_hops
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_heartbeat_subscription_get_max_hops(const uint8_t * event){
    return event[15];
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_CONFIGURATION_LOW_POWER_NODE_POLL_TIMEOUT
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_low_power_node_poll_timeout_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field foundation_status from event MESH_SUBEVENT_CONFIGURATION_LOW_POWER_NODE_POLL_TIMEOUT
 * @param event packet
 * @return foundation_status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_low_power_node_poll_timeout_get_foundation_status(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field lpn_address from event MESH_SUBEVENT_CONFIGURATION_LOW_POWER_NODE_POLL_TIMEOUT
 * @param event packet
 * @return lpn_address
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_low_power_node_poll_timeout_get_lpn_address(const uint8_t * event){
    return little_endian_read_16(event, 6);
}
/**
 * @brief Get field poll_timeout from event MESH_SUBEVENT_CONFIGURATION_LOW_POWER_NODE_POLL_TIMEOUT
 * @param event packet
 * @return poll_timeout
 * @note: btstack_type 3
 */
static inline uint32_t mesh_subevent_configuration_low_power_node_poll_timeout_get_poll_timeout(const uint8_t * event){
    return little_endian_read_24(event, 8);
}

/**
 * @brief Get field dest from event MESH_SUBEVENT_CONFIGURATION_NETWORK_TRANSMIT
 * @param event packet
 * @return dest
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_network_transmit_get_dest(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field foundation_status from event MESH_SUBEVENT_CONFIGURATION_NETWORK_TRANSMIT
 * @param event packet
 * @return foundation_status
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_network_transmit_get_foundation_status(const uint8_t * event){
    return event[5];
}
/**
 * @brief Get field transmit_count from event MESH_SUBEVENT_CONFIGURATION_NETWORK_TRANSMIT
 * @param event packet
 * @return transmit_count
 * @note: btstack_type 1
 */
static inline uint8_t mesh_subevent_configuration_network_transmit_get_transmit_count(const uint8_t * event){
    return event[6];
}
/**
 * @brief Get field transmit_interval_steps_ms from event MESH_SUBEVENT_CONFIGURATION_NETWORK_TRANSMIT
 * @param event packet
 * @return transmit_interval_steps_ms
 * @note: btstack_type 2
 */
static inline uint16_t mesh_subevent_configuration_network_transmit_get_transmit_interval_steps_ms(const uint8_t * event){
    return little_endian_read_16(event, 7);
}




#endif

