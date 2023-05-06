#ifndef _btstack_mt_h
#define _btstack_mt_h

#include "btstack_defines.h"
#include "gap.h"
#include "gatt_client.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 * @brief Thread-safe Bluetooth stack APIs
 *
 * `mt_xxx` are the thread-safe version of corresponding unsafe APIs which are
 *  free to be called in any threads/tasks including within the Host task itself.
 *
 * NOTE:
 * 1. Wrapping introduces overhead.
 * 2. These APIs are not recommended for high performance or timing critical applications.
 *
 * CAUTION:
 * 1. Do not use these APIs in ISR.
 * 2. A **true** RTOS is required.
 *
 ****************************************************************************************
 */

struct btstack_mt_event;


/**
 * @brief Allocate an event for synchronization
 *
 * @return  an event object
*/
struct btstack_mt_event *btstack_mt_event_alloc(void);

/**
 * @brief Free an event object
 *
 * The event to be freed must be in un-signaled state.
 *
 * @param[in] event     the event object
*/
void btstack_mt_event_free(struct btstack_mt_event * event);

/**
 * @brief Wait for an event to become signalled
 *
 * @param[in] event     the event object
*/
void btstack_mt_event_wait(struct btstack_mt_event * event);

/**
 * @brief Signal an event
 *
 * @param[in] event     the event object
*/
void btstack_mt_event_set(struct btstack_mt_event * event);


// -----------------------------------------------------------------------------
// Machine-generated code. DO NOT modify.
// Note: below declarations are one-to-one mapping of APIs GAP and ATT server.
// -----------------------------------------------------------------------------
uint8_t mt_gap_set_random_device_address(
    const uint8_t * address);

uint8_t mt_gap_disconnect(
    hci_con_handle_t handle);

void mt_gap_disconnect_all(
    void);

uint8_t mt_gap_add_whitelist(
    const uint8_t * address,
    bd_addr_type_t addtype);

uint8_t mt_gap_remove_whitelist(
    const uint8_t * address,
    bd_addr_type_t addtype);

uint8_t mt_gap_clear_white_lists(
    void);

uint8_t mt_gap_read_rssi(
    hci_con_handle_t handle);

uint8_t mt_gap_read_remote_used_features(
    hci_con_handle_t handle);

uint8_t mt_gap_read_remote_version(
    hci_con_handle_t handle);

uint8_t mt_gap_le_read_channel_map(
    hci_con_handle_t handle);

uint8_t mt_gap_read_phy(
    const uint16_t con_handle);

uint8_t mt_gap_set_def_phy(
    const uint8_t all_phys,
    const phy_bittypes_t tx_phys,
    const phy_bittypes_t rx_phys);

uint8_t mt_gap_set_phy(
    const uint16_t con_handle,
    const uint8_t all_phys,
    const phy_bittypes_t tx_phys,
    const phy_bittypes_t rx_phys,
    const phy_option_t phy_opt);

uint8_t mt_gap_set_adv_set_random_addr(
    const uint8_t adv_handle,
    const uint8_t * random_addr);

uint8_t mt_gap_set_ext_scan_para(
    const bd_addr_type_t own_addr_type,
    const scan_filter_policy_t filter,
    const uint8_t config_num,
    const scan_phy_config_t * configs);

uint8_t mt_gap_set_ext_scan_response_data(
    const uint8_t adv_handle,
    const uint16_t length,
    const uint8_t * data);

uint8_t mt_gap_set_ext_scan_enable(
    const uint8_t enable,
    const uint8_t filter,
    const uint16_t duration,
    const uint16_t period);

uint8_t mt_gap_set_ext_adv_enable(
    const uint8_t enable,
    const uint8_t set_number,
    const ext_adv_set_en_t * adv_sets);

uint8_t mt_gap_set_ext_adv_para(
    const uint8_t adv_handle,
    const adv_event_properties_t properties,
    const uint32_t interval_min,
    const uint32_t interval_max,
    const adv_channel_bits_t primary_adv_channel_map,
    const bd_addr_type_t own_addr_type,
    const bd_addr_type_t peer_addr_type,
    const uint8_t * peer_addr,
    const adv_filter_policy_t adv_filter_policy,
    const int8_t tx_power,
    const phy_type_t primary_adv_phy,
    const uint8_t secondary_adv_max_skip,
    const phy_type_t secondary_adv_phy,
    const uint8_t sid,
    const uint8_t scan_req_notification_enable);

uint8_t mt_gap_set_ext_adv_data(
    const uint8_t adv_handle,
    uint16_t length,
    const uint8_t * data);

uint8_t mt_gap_set_periodic_adv_data(
    const uint8_t adv_handle,
    uint16_t length,
    const uint8_t * data);

uint8_t mt_gap_set_periodic_adv_enable(
    const uint8_t enable,
    const uint8_t adv_handle);

uint8_t mt_gap_set_periodic_adv_para(
    const uint8_t adv_handle,
    const uint16_t interval_min,
    const uint16_t interval_max,
    const periodic_adv_properties_t properties);

uint8_t mt_gap_clr_adv_set(
    void);

uint8_t mt_gap_rmv_adv_set(
    const uint8_t adv_handle);

uint8_t mt_gap_periodic_adv_create_sync(
    const periodic_adv_filter_policy_t filter_policy,
    const uint8_t adv_sid,
    const bd_addr_type_t addr_type,
    const uint8_t * addr,
    const uint16_t skip,
    const uint16_t sync_timeout,
    const uint8_t sync_cte_type);

uint8_t mt_gap_periodic_adv_create_sync_cancel(
    void);

uint8_t mt_gap_periodic_adv_term_sync(
    const uint16_t sync_handle);

uint8_t mt_gap_add_dev_to_periodic_list(
    const uint8_t addr_type,
    const uint8_t * addr,
    const uint8_t sid);

uint8_t mt_gap_rmv_dev_from_periodic_list(
    const uint8_t addr_type,
    const uint8_t * addr,
    const uint8_t sid);

uint8_t mt_gap_clr_periodic_adv_list(
    void);

uint8_t mt_gap_read_periodic_adv_list_size(
    void);

uint8_t mt_gap_ext_create_connection(
    const initiating_filter_policy_t filter_policy,
    const bd_addr_type_t own_addr_type,
    const bd_addr_type_t peer_addr_type,
    const uint8_t * peer_addr,
    const uint8_t initiating_phy_num,
    const initiating_phy_config_t * phy_configs);

uint8_t mt_gap_create_connection_cancel(
    void);

uint8_t mt_gap_set_data_length(
    uint16_t connection_handle,
    uint16_t tx_octets,
    uint16_t tx_time);

uint8_t mt_gap_set_connectionless_cte_tx_param(
    const uint8_t adv_handle,
    const uint8_t cte_len,
    const cte_type_t cte_type,
    const uint8_t cte_count,
    const uint8_t switching_pattern_len,
    const uint8_t * antenna_ids);

uint8_t mt_gap_set_connectionless_cte_tx_enable(
    const uint8_t adv_handle,
    const uint8_t cte_enable);

uint8_t mt_gap_set_connectionless_iq_sampling_enable(
    const uint16_t sync_handle,
    const uint8_t sampling_enable,
    const cte_slot_duration_type_t slot_durations,
    const uint8_t max_sampled_ctes,
    const uint8_t switching_pattern_len,
    const uint8_t * antenna_ids);

uint8_t mt_gap_set_connection_cte_rx_param(
    const hci_con_handle_t conn_handle,
    const uint8_t sampling_enable,
    const cte_slot_duration_type_t slot_durations,
    const uint8_t switching_pattern_len,
    const uint8_t * antenna_ids);

uint8_t mt_gap_set_connection_cte_tx_param(
    const hci_con_handle_t conn_handle,
    const uint8_t cte_types,
    const uint8_t switching_pattern_len,
    const uint8_t * antenna_ids);

uint8_t mt_gap_set_connection_cte_request_enable(
    const hci_con_handle_t conn_handle,
    const uint8_t enable,
    const uint16_t requested_cte_interval,
    const uint8_t requested_cte_length,
    const cte_type_t requested_cte_type);

uint8_t mt_gap_set_connection_cte_response_enable(
    const hci_con_handle_t conn_handle,
    const uint8_t enable);

uint8_t mt_gap_read_antenna_info(
    void);

uint8_t mt_gap_set_periodic_adv_rx_enable(
    const uint16_t sync_handle,
    const uint8_t enable);

uint8_t mt_gap_periodic_adv_sync_transfer(
    const hci_con_handle_t conn_handle,
    const uint16_t service_data,
    const uint16_t sync_handle);

uint8_t mt_gap_periodic_adv_set_info_transfer(
    const hci_con_handle_t conn_handle,
    const uint16_t service_data,
    const uint8_t adv_handle);

uint8_t mt_gap_periodic_adv_sync_transfer_param(
    const hci_con_handle_t conn_handle,
    const periodic_adv_sync_transfer_mode_t mode,
    const uint16_t skip,
    const uint16_t sync_timeout,
    const uint8_t cte_excl_types);

uint8_t mt_gap_default_periodic_adv_sync_transfer_param(
    const periodic_adv_sync_transfer_mode_t mode,
    const uint16_t skip,
    const uint16_t sync_timeout,
    const uint8_t cte_excl_types);

uint8_t mt_gap_set_host_channel_classification(
    const uint32_t channel_low,
    const uint8_t channel_high);

int mt_gap_update_connection_parameters(
    hci_con_handle_t con_handle,
    uint16_t conn_interval_min,
    uint16_t conn_interval_max,
    uint16_t conn_latency,
    uint16_t supervision_timeout,
    uint16_t min_ce_len,
    uint16_t max_ce_len);

void mt_gap_get_connection_parameter_range(
    le_connection_parameter_range_t * range);

void mt_gap_set_connection_parameter_range(
    le_connection_parameter_range_t * range);

uint8_t mt_gap_read_local_tx_power_level(
    hci_con_handle_t con_handle,
    unified_phy_type_t phy);

uint8_t mt_gap_read_remote_tx_power_level(
    hci_con_handle_t con_handle,
    unified_phy_type_t phy);

uint8_t mt_gap_set_path_loss_reporting_param(
    hci_con_handle_t con_handle,
    uint8_t high_threshold,
    uint8_t high_hysteresis,
    uint8_t low_threshold,
    uint8_t low_hysteresis,
    uint8_t min_time_spent);

uint8_t mt_gap_set_path_loss_reporting_enable(
    hci_con_handle_t con_handle,
    uint8_t enable);

uint8_t mt_gap_set_tx_power_reporting_enable(
    hci_con_handle_t con_handle,
    uint8_t local_enable,
    uint8_t remote_enable);

uint8_t mt_gap_set_default_subrate(
    uint16_t subrate_min,
    uint16_t subrate_max,
    uint16_t max_latency,
    uint16_t continuation_number,
    uint16_t supervision_timeout);

uint8_t mt_gap_subrate_request(
    hci_con_handle_t con_handle,
    uint16_t subrate_min,
    uint16_t subrate_max,
    uint16_t max_latency,
    uint16_t continuation_number,
    uint16_t supervision_timeout);

uint8_t mt_gap_aes_encrypt(
    const uint8_t * key,
    const uint8_t * plaintext,
    gap_hci_cmd_complete_cb_t cb,
    void * user_data);

uint8_t mt_gap_rx_test_v2(
    uint8_t rx_channel,
    uint8_t phy,
    uint8_t modulation_index);

uint8_t mt_gap_rx_test_v3(
    uint8_t rx_channel,
    uint8_t phy,
    uint8_t modulation_index,
    uint8_t expected_cte_length,
    uint8_t expected_cte_type,
    uint8_t slot_durations,
    uint8_t switching_pattern_length,
    uint8_t * antenna_ids);

uint8_t mt_gap_tx_test_v2(
    uint8_t tx_channel,
    uint8_t test_data_length,
    uint8_t packet_payload,
    uint8_t phy);

uint8_t mt_gap_tx_test_v4(
    uint8_t tx_channel,
    uint8_t test_data_length,
    uint8_t packet_payload,
    uint8_t phy,
    uint8_t cte_length,
    uint8_t cte_type,
    uint8_t switching_pattern_length,
    uint8_t * antenna_ids,
    int8_t tx_power_level);

uint8_t mt_gap_test_end(
    void);

uint8_t mt_gap_vendor_tx_continuous_wave(
    uint8_t enable,
    uint8_t power_level_index,
    uint16_t freq);

uint8_t mt_gap_start_ccm(
    uint8_t type,
    uint8_t mic_size,
    uint16_t msg_len,
    uint16_t aad_len,
    uint32_t tag,
    const uint8_t *key,
    const uint8_t *nonce,
    const uint8_t *msg,
    const uint8_t *aad,
    uint8_t *out_msg,
    gap_hci_cmd_complete_cb_t cb,
    void *user_data);

int mt_att_server_deferred_read_response(
    hci_con_handle_t con_handle,
    uint16_t attribute_handle,
    const uint8_t * value,
    uint16_t value_len);

int mt_att_server_notify(
    hci_con_handle_t con_handle,
    uint16_t attribute_handle,
    uint8_t * value,
    uint16_t value_len);

int mt_att_server_indicate(
    hci_con_handle_t con_handle,
    uint16_t attribute_handle,
    uint8_t * value,
    uint16_t value_len);

uint16_t mt_att_server_get_mtu(
    hci_con_handle_t con_handle);

uint8_t mt_gatt_client_discover_primary_services(
    user_packet_handler_t callback,
    hci_con_handle_t con_handle);

uint8_t mt_gatt_client_discover_primary_services_by_uuid16(
    user_packet_handler_t callback,
    hci_con_handle_t con_handle,
    uint16_t uuid16);

uint8_t mt_gatt_client_discover_primary_services_by_uuid128(
    user_packet_handler_t callback,
    hci_con_handle_t con_handle,
    const uint8_t * uuid128);

uint8_t mt_gatt_client_find_included_services_for_service(
    user_packet_handler_t callback,
    hci_con_handle_t con_handle,
    const uint16_t start_group_handle,
    const uint16_t end_group_handle);

uint8_t mt_gatt_client_discover_characteristics_for_service(
    user_packet_handler_t callback,
    hci_con_handle_t con_handle,
    const uint16_t start_group_handle,
    const uint16_t end_group_handle);

uint8_t mt_gatt_client_discover_characteristics_for_handle_range_by_uuid16(
    btstack_packet_handler_t callback,
    const hci_con_handle_t con_handle,
    const uint16_t start_handle,
    const uint16_t end_handle,
    const uint16_t uuid16);

uint8_t mt_gatt_client_discover_characteristics_for_handle_range_by_uuid128(
    btstack_packet_handler_t callback,
    const hci_con_handle_t con_handle,
    const uint16_t start_handle,
    const uint16_t end_handle,
    const uint8_t * uuid128);

uint8_t mt_gatt_client_discover_characteristic_descriptors(
    btstack_packet_handler_t callback,
    hci_con_handle_t con_handle,
    gatt_client_characteristic_t * characteristic);

uint8_t mt_gatt_client_read_value_of_characteristic_using_value_handle(
    btstack_packet_handler_t callback,
    hci_con_handle_t con_handle,
    uint16_t characteristic_value_handle);

uint8_t mt_gatt_client_read_value_of_characteristics_by_uuid16(
    btstack_packet_handler_t callback,
    hci_con_handle_t con_handle,
    uint16_t start_handle,
    uint16_t end_handle,
    uint16_t uuid16);

uint8_t mt_gatt_client_read_value_of_characteristics_by_uuid128(
    btstack_packet_handler_t callback,
    hci_con_handle_t con_handle,
    uint16_t start_handle,
    uint16_t end_handle,
    uint8_t * uuid128);

uint8_t mt_gatt_client_read_long_value_of_characteristic_using_value_handle(
    btstack_packet_handler_t callback,
    hci_con_handle_t con_handle,
    uint16_t characteristic_value_handle);

uint8_t mt_gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset(
    btstack_packet_handler_t callback,
    hci_con_handle_t con_handle,
    uint16_t characteristic_value_handle,
    uint16_t offset);

uint8_t mt_gatt_client_read_multiple_characteristic_values(
    btstack_packet_handler_t callback,
    hci_con_handle_t con_handle,
    int num_value_handles,
    uint16_t * value_handles);

uint8_t mt_gatt_client_write_value_of_characteristic_without_response(
    hci_con_handle_t con_handle,
    uint16_t characteristic_value_handle,
    uint16_t length,
    const uint8_t * data);

uint8_t mt_gatt_client_signed_write_without_response(
    btstack_packet_handler_t callback,
    hci_con_handle_t con_handle,
    uint16_t handle,
    uint16_t message_len,
    const uint8_t * message);

uint8_t mt_gatt_client_write_value_of_characteristic(
    btstack_packet_handler_t callback,
    hci_con_handle_t con_handle,
    uint16_t characteristic_value_handle,
    uint16_t length,
    const uint8_t * data);

uint8_t mt_gatt_client_write_long_value_of_characteristic(
    btstack_packet_handler_t callback,
    hci_con_handle_t con_handle,
    uint16_t characteristic_value_handle,
    uint16_t length,
    const uint8_t * data);

uint8_t mt_gatt_client_write_long_value_of_characteristic_with_offset(
    btstack_packet_handler_t callback,
    hci_con_handle_t con_handle,
    uint16_t characteristic_value_handle,
    uint16_t offset,
    uint16_t length,
    const uint8_t * data);

uint8_t mt_gatt_client_reliable_write_long_value_of_characteristic(
    btstack_packet_handler_t callback,
    hci_con_handle_t con_handle,
    uint16_t characteristic_value_handle,
    uint16_t length,
    const uint8_t * data);

uint8_t mt_gatt_client_read_characteristic_descriptor_using_descriptor_handle(
    btstack_packet_handler_t callback,
    hci_con_handle_t con_handle,
    uint16_t descriptor_handle);

uint8_t mt_gatt_client_read_long_characteristic_descriptor_using_descriptor_handle(
    btstack_packet_handler_t callback,
    hci_con_handle_t con_handle,
    uint16_t descriptor_handle);

uint8_t mt_gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset(
    btstack_packet_handler_t callback,
    hci_con_handle_t con_handle,
    uint16_t descriptor_handle,
    uint16_t offset);

uint8_t mt_gatt_client_write_characteristic_descriptor_using_descriptor_handle(
    btstack_packet_handler_t callback,
    hci_con_handle_t con_handle,
    uint16_t descriptor_handle,
    uint16_t length,
    uint8_t * data);

uint8_t mt_gatt_client_write_long_characteristic_descriptor_using_descriptor_handle(
    btstack_packet_handler_t callback,
    hci_con_handle_t con_handle,
    uint16_t descriptor_handle,
    uint16_t length,
    uint8_t * data);

uint8_t mt_gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset(
    btstack_packet_handler_t callback,
    hci_con_handle_t con_handle,
    uint16_t descriptor_handle,
    uint16_t offset,
    uint16_t length,
    uint8_t * data);

uint8_t mt_gatt_client_write_client_characteristic_configuration(
    btstack_packet_handler_t callback,
    hci_con_handle_t con_handle,
    gatt_client_characteristic_t * characteristic,
    uint16_t configuration);

uint8_t mt_gatt_client_prepare_write(
    btstack_packet_handler_t callback,
    hci_con_handle_t con_handle,
    uint16_t attribute_handle,
    uint16_t offset,
    uint16_t length,
    uint8_t * data);

uint8_t mt_gatt_client_execute_write(
    btstack_packet_handler_t callback,
    hci_con_handle_t con_handle);

uint8_t mt_gatt_client_cancel_write(
    btstack_packet_handler_t callback,
    hci_con_handle_t con_handle);

int mt_gatt_client_is_ready(
    hci_con_handle_t con_handle);

uint8_t mt_gatt_client_get_mtu(
    hci_con_handle_t con_handle,
    uint16_t * mtu);

void mt_gatt_client_listen_for_characteristic_value_updates(
    gatt_client_notification_t * notification,
    btstack_packet_handler_t packet_handler,
    hci_con_handle_t con_handle,
    uint16_t value_handle);

#ifdef __cplusplus
}
#endif

#endif
