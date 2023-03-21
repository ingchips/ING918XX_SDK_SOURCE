__attribute((weak)) void platform_reset() {UNDEF_FUN(1001); }
__attribute((weak)) void platform_switch_app(const uint32_t app_addr) {UNDEF_FUN(1002); }
__attribute((weak)) void platform_write_persistent_reg(const uint8_t value) {UNDEF_FUN(1003); }
__attribute((weak)) uint8_t platform_read_persistent_reg() {UNDEF_FUN(1004); return (uint8_t)0; }
__attribute((weak)) void platform_config(const platform_cfg_item_t item,const uint32_t flag) {UNDEF_FUN(1005); }
__attribute((weak)) uint32_t platform_read_info(const platform_info_item_t item) {UNDEF_FUN(1006); return (uint32_t)0; }
__attribute((weak)) uint32_t platform_calibrate_32k() {UNDEF_FUN(1007); return (uint32_t)0; }
__attribute((weak)) void platform_32k_rc_tune(uint16_t value) {UNDEF_FUN(1008); }
__attribute((weak)) uint16_t platform_32k_rc_auto_tune() {UNDEF_FUN(1009); return (uint16_t)0; }
__attribute((weak)) int platform_rand() {UNDEF_FUN(1010); return (int)0; }
__attribute((weak)) uint64_t platform_get_us_time() {UNDEF_FUN(1011); return (uint64_t)0; }
__attribute((weak)) void platform_set_rf_clk_source(const uint8_t source) {UNDEF_FUN(1012); }
__attribute((weak)) uintptr_t platform_get_task_handle(platform_task_id_t id) {UNDEF_FUN(1013); return (uintptr_t)0; }
__attribute((weak)) void gap_set_random_device_address(const uint8_t * address) {UNDEF_FUN(1014); }
__attribute((weak)) uint8_t gap_disconnect(hci_con_handle_t handle) {UNDEF_FUN(1015); return (uint8_t)0; }
__attribute((weak)) void gap_disconnect_all() {UNDEF_FUN(1016); }
__attribute((weak)) uint8_t gap_add_whitelist(const uint8_t * address,bd_addr_type_t addtype) {UNDEF_FUN(1017); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_remove_whitelist(const uint8_t * address,bd_addr_type_t addtype) {UNDEF_FUN(1018); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_clear_white_lists() {UNDEF_FUN(1019); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_read_rssi(hci_con_handle_t handle) {UNDEF_FUN(1020); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_read_remote_used_features(hci_con_handle_t handle) {UNDEF_FUN(1021); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_read_remote_version(hci_con_handle_t handle) {UNDEF_FUN(1022); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_le_read_channel_map(hci_con_handle_t handle) {UNDEF_FUN(1023); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_read_phy(const uint16_t con_handle) {UNDEF_FUN(1024); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_def_phy(const uint8_t all_phys,const phy_bittypes_t tx_phys,const phy_bittypes_t rx_phys) {UNDEF_FUN(1025); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_phy(const uint16_t con_handle,const uint8_t all_phys,const phy_bittypes_t tx_phys,const phy_bittypes_t rx_phys,const phy_option_t phy_opt) {UNDEF_FUN(1026); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_adv_set_random_addr(const uint8_t adv_handle,const uint8_t * random_addr) {UNDEF_FUN(1027); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_ext_scan_para(const bd_addr_type_t own_addr_type,const scan_filter_policy_t filter,const uint8_t config_num,const scan_phy_config_t * configs) {UNDEF_FUN(1028); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_ext_scan_response_data(const uint8_t adv_handle,const uint16_t length,const uint8_t * data) {UNDEF_FUN(1029); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_ext_scan_enable(const uint8_t enable,const uint8_t filter,const uint16_t duration,const uint16_t period) {UNDEF_FUN(1030); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_ext_adv_enable(const uint8_t enable,const uint8_t set_number,const ext_adv_set_en_t * adv_sets) {UNDEF_FUN(1031); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_ext_adv_para(const uint8_t adv_handle,const adv_event_properties_t properties,const uint32_t interval_min,const uint32_t interval_max,const adv_channel_bits_t primary_adv_channel_map,const bd_addr_type_t own_addr_type,const bd_addr_type_t peer_addr_type,const uint8_t * peer_addr,const adv_filter_policy_t adv_filter_policy,const int8_t tx_power,const phy_type_t primary_adv_phy,const uint8_t secondary_adv_max_skip,const phy_type_t secondary_adv_phy,const uint8_t sid,const uint8_t scan_req_notification_enable) {UNDEF_FUN(1032); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_ext_adv_data(const uint8_t adv_handle,uint16_t length,const uint8_t * data) {UNDEF_FUN(1033); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_periodic_adv_data(const uint8_t adv_handle,uint16_t length,const uint8_t * data) {UNDEF_FUN(1034); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_periodic_adv_enable(const uint8_t enable,const uint8_t adv_handle) {UNDEF_FUN(1035); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_periodic_adv_para(const uint8_t adv_handle,const uint16_t interval_min,const uint16_t interval_max,const periodic_adv_properties_t properties) {UNDEF_FUN(1036); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_clr_adv_set() {UNDEF_FUN(1037); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_rmv_adv_set(const uint8_t adv_handle) {UNDEF_FUN(1038); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_periodic_adv_create_sync(const periodic_adv_filter_policy_t filter_policy,const uint8_t adv_sid,const bd_addr_type_t addr_type,const uint8_t * addr,const uint16_t skip,const uint16_t sync_timeout,const uint8_t sync_cte_type) {UNDEF_FUN(1039); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_periodic_adv_create_sync_cancel() {UNDEF_FUN(1040); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_periodic_adv_term_sync(const uint16_t sync_handle) {UNDEF_FUN(1041); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_add_dev_to_periodic_list(const uint8_t addr_type,const uint8_t * addr,const uint8_t sid) {UNDEF_FUN(1042); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_rmv_dev_from_periodic_list(const uint8_t addr_type,const uint8_t * addr,const uint8_t sid) {UNDEF_FUN(1043); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_clr_periodic_adv_list() {UNDEF_FUN(1044); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_read_periodic_adv_list_size() {UNDEF_FUN(1045); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_ext_create_connection(const initiating_filter_policy_t filter_policy,const bd_addr_type_t own_addr_type,const bd_addr_type_t peer_addr_type,const uint8_t * peer_addr,const uint8_t initiating_phy_num,const initiating_phy_config_t * phy_configs) {UNDEF_FUN(1046); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_create_connection_cancel() {UNDEF_FUN(1047); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_data_length(uint16_t connection_handle,uint16_t tx_octets,uint16_t tx_time) {UNDEF_FUN(1048); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_connectionless_cte_tx_param(const uint8_t adv_handle,const uint8_t cte_len,const cte_type_t cte_type,const uint8_t cte_count,const uint8_t switching_pattern_len,const uint8_t * antenna_ids) {UNDEF_FUN(1049); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_connectionless_cte_tx_enable(const uint8_t adv_handle,const uint8_t cte_enable) {UNDEF_FUN(1050); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_connectionless_iq_sampling_enable(const uint16_t sync_handle,const uint8_t sampling_enable,const cte_slot_duration_type_t slot_durations,const uint8_t max_sampled_ctes,const uint8_t switching_pattern_len,const uint8_t * antenna_ids) {UNDEF_FUN(1051); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_connection_cte_rx_param(const hci_con_handle_t conn_handle,const uint8_t sampling_enable,const cte_slot_duration_type_t slot_durations,const uint8_t switching_pattern_len,const uint8_t * antenna_ids) {UNDEF_FUN(1052); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_connection_cte_tx_param(const hci_con_handle_t conn_handle,const uint8_t cte_types,const uint8_t switching_pattern_len,const uint8_t * antenna_ids) {UNDEF_FUN(1053); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_connection_cte_request_enable(const hci_con_handle_t conn_handle,const uint8_t enable,const uint16_t requested_cte_interval,const uint8_t requested_cte_length,const cte_type_t requested_cte_type) {UNDEF_FUN(1054); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_connection_cte_response_enable(const hci_con_handle_t conn_handle,const uint8_t enable) {UNDEF_FUN(1055); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_read_antenna_info() {UNDEF_FUN(1056); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_periodic_adv_rx_enable(const uint16_t sync_handle,const uint8_t enable) {UNDEF_FUN(1057); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_periodic_adv_sync_transfer(const hci_con_handle_t conn_handle,const uint16_t service_data,const uint16_t sync_handle) {UNDEF_FUN(1058); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_periodic_adv_set_info_transfer(const hci_con_handle_t conn_handle,const uint16_t service_data,const uint8_t adv_handle) {UNDEF_FUN(1059); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_periodic_adv_sync_transfer_param(const hci_con_handle_t conn_handle,const periodic_adv_sync_transfer_mode_t mode,const uint16_t skip,const uint16_t sync_timeout,const uint8_t cte_excl_types) {UNDEF_FUN(1060); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_default_periodic_adv_sync_transfer_param(const periodic_adv_sync_transfer_mode_t mode,const uint16_t skip,const uint16_t sync_timeout,const uint8_t cte_excl_types) {UNDEF_FUN(1061); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_host_channel_classification(const uint32_t channel_low,const uint8_t channel_high) {UNDEF_FUN(1062); return (uint8_t)0; }
__attribute((weak)) int gap_update_connection_parameters(hci_con_handle_t con_handle,uint16_t conn_interval_min,uint16_t conn_interval_max,uint16_t conn_latency,uint16_t supervision_timeout,uint16_t min_ce_len,uint16_t max_ce_len) {UNDEF_FUN(1063); return (int)0; }
__attribute((weak)) void gap_get_connection_parameter_range(le_connection_parameter_range_t * range) {UNDEF_FUN(1064); }
__attribute((weak)) void gap_set_connection_parameter_range(le_connection_parameter_range_t * range) {UNDEF_FUN(1065); }
__attribute((weak)) uint8_t gap_read_local_tx_power_level(hci_con_handle_t con_handle,unified_phy_type_t phy) {UNDEF_FUN(1066); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_read_remote_tx_power_level(hci_con_handle_t con_handle,unified_phy_type_t phy) {UNDEF_FUN(1067); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_path_loss_reporting_param(hci_con_handle_t con_handle,uint8_t high_threshold,uint8_t high_hysteresis,uint8_t low_threshold,uint8_t low_hysteresis,uint8_t min_time_spent) {UNDEF_FUN(1068); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_path_loss_reporting_enable(hci_con_handle_t con_handle,uint8_t enable) {UNDEF_FUN(1069); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_tx_power_reporting_enable(hci_con_handle_t con_handle,uint8_t local_enable,uint8_t remote_enable) {UNDEF_FUN(1070); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_set_default_subrate(uint16_t subrate_min,uint16_t subrate_max,uint16_t max_latency,uint16_t continuation_number,uint16_t supervision_timeout) {UNDEF_FUN(1071); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_subrate_request(hci_con_handle_t con_handle,uint16_t subrate_min,uint16_t subrate_max,uint16_t max_latency,uint16_t continuation_number,uint16_t supervision_timeout) {UNDEF_FUN(1072); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_aes_encrypt(const uint8_t * key,const uint8_t * plaintext,gap_hci_cmd_complete_cb_t cb,void * user_data) {UNDEF_FUN(1073); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_rx_test_v2(uint8_t rx_channel,uint8_t phy,uint8_t modulation_index) {UNDEF_FUN(1074); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_rx_test_v3(uint8_t rx_channel,uint8_t phy,uint8_t modulation_index,uint8_t expected_cte_length,uint8_t expected_cte_type,uint8_t slot_durations,uint8_t switching_pattern_length,uint8_t * antenna_ids) {UNDEF_FUN(1075); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_tx_test_v2(uint8_t tx_channel,uint8_t test_data_length,uint8_t packet_payload,uint8_t phy) {UNDEF_FUN(1076); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_tx_test_v4(uint8_t tx_channel,uint8_t test_data_length,uint8_t packet_payload,uint8_t phy,uint8_t cte_length,uint8_t cte_type,uint8_t switching_pattern_length,uint8_t * antenna_ids,int8_t tx_power_level) {UNDEF_FUN(1077); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_test_end() {UNDEF_FUN(1078); return (uint8_t)0; }
__attribute((weak)) uint8_t gap_vendor_tx_continuous_wave(uint8_t enable,uint8_t power_level_index,uint16_t freq) {UNDEF_FUN(1079); return (uint8_t)0; }
__attribute((weak)) int att_server_deferred_read_response(hci_con_handle_t con_handle,uint16_t attribute_handle,const uint8_t * value,uint16_t value_len) {UNDEF_FUN(1080); return (int)0; }
__attribute((weak)) int att_server_notify(hci_con_handle_t con_handle,uint16_t attribute_handle,const uint8_t * value,uint16_t value_len) {UNDEF_FUN(1081); return (int)0; }
__attribute((weak)) int att_server_indicate(hci_con_handle_t con_handle,uint16_t attribute_handle,const uint8_t * value,uint16_t value_len) {UNDEF_FUN(1082); return (int)0; }
__attribute((weak)) uint16_t att_server_get_mtu(hci_con_handle_t con_handle) {UNDEF_FUN(1083); return (uint16_t)0; }
__attribute((weak)) uint8_t gatt_client_discover_primary_services(user_packet_handler_t callback,hci_con_handle_t con_handle) {UNDEF_FUN(1084); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_discover_primary_services_by_uuid16(user_packet_handler_t callback,hci_con_handle_t con_handle,uint16_t uuid16) {UNDEF_FUN(1085); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_discover_primary_services_by_uuid128(user_packet_handler_t callback,hci_con_handle_t con_handle,const uint8_t * uuid128) {UNDEF_FUN(1086); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_find_included_services_for_service(user_packet_handler_t callback,hci_con_handle_t con_handle,const uint16_t start_group_handle,const uint16_t end_group_handle) {UNDEF_FUN(1087); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_discover_characteristics_for_service(user_packet_handler_t callback,hci_con_handle_t con_handle,const uint16_t start_group_handle,const uint16_t end_group_handle) {UNDEF_FUN(1088); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_discover_characteristics_for_handle_range_by_uuid16(btstack_packet_handler_t callback,const hci_con_handle_t con_handle,const uint16_t start_handle,const uint16_t end_handle,const uint16_t uuid16) {UNDEF_FUN(1089); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_discover_characteristics_for_handle_range_by_uuid128(btstack_packet_handler_t callback,const hci_con_handle_t con_handle,const uint16_t start_handle,const uint16_t end_handle,const uint8_t * uuid128) {UNDEF_FUN(1090); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_discover_characteristic_descriptors(btstack_packet_handler_t callback,hci_con_handle_t con_handle,gatt_client_characteristic_t * characteristic) {UNDEF_FUN(1091); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_read_value_of_characteristic_using_value_handle(btstack_packet_handler_t callback,hci_con_handle_t con_handle,uint16_t characteristic_value_handle) {UNDEF_FUN(1092); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_read_value_of_characteristics_by_uuid16(btstack_packet_handler_t callback,hci_con_handle_t con_handle,uint16_t start_handle,uint16_t end_handle,uint16_t uuid16) {UNDEF_FUN(1093); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_read_value_of_characteristics_by_uuid128(btstack_packet_handler_t callback,hci_con_handle_t con_handle,uint16_t start_handle,uint16_t end_handle,uint8_t * uuid128) {UNDEF_FUN(1094); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_read_long_value_of_characteristic_using_value_handle(btstack_packet_handler_t callback,hci_con_handle_t con_handle,uint16_t characteristic_value_handle) {UNDEF_FUN(1095); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset(btstack_packet_handler_t callback,hci_con_handle_t con_handle,uint16_t characteristic_value_handle,uint16_t offset) {UNDEF_FUN(1096); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_read_multiple_characteristic_values(btstack_packet_handler_t callback,hci_con_handle_t con_handle,int num_value_handles,uint16_t * value_handles) {UNDEF_FUN(1097); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_write_value_of_characteristic_without_response(hci_con_handle_t con_handle,uint16_t characteristic_value_handle,uint16_t length,const uint8_t * data) {UNDEF_FUN(1098); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_signed_write_without_response(btstack_packet_handler_t callback,hci_con_handle_t con_handle,uint16_t handle,uint16_t message_len,const uint8_t * message) {UNDEF_FUN(1099); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_write_value_of_characteristic(btstack_packet_handler_t callback,hci_con_handle_t con_handle,uint16_t characteristic_value_handle,uint16_t length,const uint8_t * data) {UNDEF_FUN(1100); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_write_long_value_of_characteristic(btstack_packet_handler_t callback,hci_con_handle_t con_handle,uint16_t characteristic_value_handle,uint16_t length,const uint8_t * data) {UNDEF_FUN(1101); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_write_long_value_of_characteristic_with_offset(btstack_packet_handler_t callback,hci_con_handle_t con_handle,uint16_t characteristic_value_handle,uint16_t offset,uint16_t length,const uint8_t * data) {UNDEF_FUN(1102); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_reliable_write_long_value_of_characteristic(btstack_packet_handler_t callback,hci_con_handle_t con_handle,uint16_t characteristic_value_handle,uint16_t length,const uint8_t * data) {UNDEF_FUN(1103); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_read_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback,hci_con_handle_t con_handle,uint16_t descriptor_handle) {UNDEF_FUN(1104); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_read_long_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback,hci_con_handle_t con_handle,uint16_t descriptor_handle) {UNDEF_FUN(1105); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset(btstack_packet_handler_t callback,hci_con_handle_t con_handle,uint16_t descriptor_handle,uint16_t offset) {UNDEF_FUN(1106); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_write_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback,hci_con_handle_t con_handle,uint16_t descriptor_handle,uint16_t length,uint8_t * data) {UNDEF_FUN(1107); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_write_long_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback,hci_con_handle_t con_handle,uint16_t descriptor_handle,uint16_t length,uint8_t * data) {UNDEF_FUN(1108); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset(btstack_packet_handler_t callback,hci_con_handle_t con_handle,uint16_t descriptor_handle,uint16_t offset,uint16_t length,uint8_t * data) {UNDEF_FUN(1109); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_write_client_characteristic_configuration(btstack_packet_handler_t callback,hci_con_handle_t con_handle,gatt_client_characteristic_t * characteristic,uint16_t configuration) {UNDEF_FUN(1110); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_prepare_write(btstack_packet_handler_t callback,hci_con_handle_t con_handle,uint16_t attribute_handle,uint16_t offset,uint16_t length,uint8_t * data) {UNDEF_FUN(1111); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_execute_write(btstack_packet_handler_t callback,hci_con_handle_t con_handle) {UNDEF_FUN(1112); return (uint8_t)0; }
__attribute((weak)) uint8_t gatt_client_cancel_write(btstack_packet_handler_t callback,hci_con_handle_t con_handle) {UNDEF_FUN(1113); return (uint8_t)0; }
__attribute((weak)) int gatt_client_is_ready(hci_con_handle_t con_handle) {UNDEF_FUN(1114); return (int)0; }
__attribute((weak)) void gatt_client_listen_for_characteristic_value_updates(gatt_client_notification_t * notification,btstack_packet_handler_t packet_handler,hci_con_handle_t con_handle,uint16_t value_handle) {UNDEF_FUN(1115); }
__attribute((weak)) void sm_config(uint8_t enable,io_capability_t io_capability,int request_security,const sm_persistent_t * persistent) {UNDEF_FUN(1116); }
__attribute((weak)) void sm_private_random_address_generation_set_mode(gap_random_address_type_t random_address_type) {UNDEF_FUN(1117); }
__attribute((weak)) gap_random_address_type_t sm_private_random_address_generation_get_mode() {UNDEF_FUN(1118); return (gap_random_address_type_t)0; }
__attribute((weak)) void sm_private_random_address_generation_set_update_period(int period_ms) {UNDEF_FUN(1119); }
__attribute((weak)) void sm_set_accepted_stk_generation_methods(uint8_t accepted_stk_generation_methods) {UNDEF_FUN(1120); }
__attribute((weak)) void sm_set_encryption_key_size_range(uint8_t min_size,uint8_t max_size) {UNDEF_FUN(1121); }
__attribute((weak)) void sm_set_authentication_requirements(uint8_t auth_req) {UNDEF_FUN(1122); }
__attribute((weak)) int sm_address_resolution_lookup(uint8_t addr_type,bd_addr_t addr) {UNDEF_FUN(1123); return (int)0; }
__attribute((weak)) void sm_config_conn(hci_con_handle_t con_handle,io_capability_t io_capability,uint8_t auth_req) {UNDEF_FUN(1124); }
__attribute((weak)) void sm_bonding_decline(hci_con_handle_t con_handle) {UNDEF_FUN(1125); }
__attribute((weak)) void sm_just_works_confirm(hci_con_handle_t con_handle) {UNDEF_FUN(1126); }
__attribute((weak)) void sm_passkey_input(hci_con_handle_t con_handle,uint32_t passkey) {UNDEF_FUN(1127); }
__attribute((weak)) void sm_send_security_request(hci_con_handle_t con_handle) {UNDEF_FUN(1128); }
__attribute((weak)) int sm_encryption_key_size(hci_con_handle_t con_handle) {UNDEF_FUN(1129); return (int)0; }
__attribute((weak)) int sm_authenticated(hci_con_handle_t con_handle) {UNDEF_FUN(1130); return (int)0; }
__attribute((weak)) authorization_state_t sm_authorization_state(hci_con_handle_t con_handle) {UNDEF_FUN(1131); return (authorization_state_t)0; }
__attribute((weak)) void sm_request_pairing(hci_con_handle_t con_handle) {UNDEF_FUN(1132); }
__attribute((weak)) void sm_authorization_decline(hci_con_handle_t con_handle) {UNDEF_FUN(1133); }
__attribute((weak)) void sm_authorization_grant(hci_con_handle_t con_handle) {UNDEF_FUN(1134); }
__attribute((weak)) int sm_le_device_key(hci_con_handle_t con_handle) {UNDEF_FUN(1135); return (int)0; }
