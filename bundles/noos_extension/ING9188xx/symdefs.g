att_dispatch_client_can_send_now = 0x00005a4d;
att_dispatch_client_request_can_send_now_event = 0x00005a53;
att_dispatch_register_client = 0x00005a59;
att_dispatch_register_server = 0x00005a6d;
att_dispatch_server_can_send_now = 0x00005a81;
att_dispatch_server_request_can_send_now_event = 0x00005a87;
att_emit_general_event = 0x00005b39;
att_server_can_send_packet_now = 0x0000624d;
att_server_deferred_read_response = 0x00006251;
att_server_get_mtu = 0x00006269;
att_server_indicate = 0x000062e1;
att_server_init = 0x00006365;
att_server_notify = 0x000063a1;
att_server_register_packet_handler = 0x000064b9;
att_server_request_can_send_now_event = 0x000064c5;
att_set_db = 0x000064e1;
att_set_read_callback = 0x000064f5;
att_set_write_callback = 0x00006501;
bd_addr_cmp = 0x00006671;
bd_addr_copy = 0x00006677;
bd_addr_to_str = 0x00006681;
big_endian_read_16 = 0x000066b9;
big_endian_read_32 = 0x000066c1;
big_endian_store_16 = 0x000066d5;
big_endian_store_32 = 0x000066e1;
btstack_config = 0x00006819;
btstack_memory_pool_create = 0x00006967;
btstack_memory_pool_free = 0x00006991;
btstack_memory_pool_get = 0x000069f1;
btstack_push_user_msg = 0x00006a0d;
char_for_nibble = 0x00006c9d;
gap_add_dev_to_periodic_list = 0x000075a9;
gap_add_whitelist = 0x000075c1;
gap_aes_encrypt = 0x000075d5;
gap_clear_white_lists = 0x00007619;
gap_clr_adv_set = 0x00007629;
gap_clr_periodic_adv_list = 0x00007639;
gap_create_connection_cancel = 0x00007649;
gap_default_periodic_adv_sync_transfer_param = 0x00007659;
gap_disconnect = 0x00007671;
gap_disconnect_all = 0x0000769d;
gap_ext_create_connection = 0x000076dd;
gap_get_connection_parameter_range = 0x000077cd;
gap_le_read_channel_map = 0x00007809;
gap_periodic_adv_create_sync = 0x0000787d;
gap_periodic_adv_create_sync_cancel = 0x000078a1;
gap_periodic_adv_set_info_transfer = 0x000078b1;
gap_periodic_adv_sync_transfer = 0x000078c9;
gap_periodic_adv_sync_transfer_param = 0x000078e1;
gap_periodic_adv_term_sync = 0x000078fd;
gap_read_antenna_info = 0x00007991;
gap_read_periodic_adv_list_size = 0x000079a1;
gap_read_phy = 0x000079b1;
gap_read_remote_info = 0x000079c5;
gap_read_remote_used_features = 0x000079d9;
gap_read_rssi = 0x000079ed;
gap_remove_whitelist = 0x00007a01;
gap_rmv_adv_set = 0x00007a85;
gap_rmv_dev_from_periodic_list = 0x00007a99;
gap_rx_test_v2 = 0x00007ab1;
gap_rx_test_v3 = 0x00007ac9;
gap_set_adv_set_random_addr = 0x00007b19;
gap_set_connection_cte_request_enable = 0x00007b65;
gap_set_connection_cte_response_enable = 0x00007b81;
gap_set_connection_cte_rx_param = 0x00007b95;
gap_set_connection_cte_tx_param = 0x00007bf1;
gap_set_connection_parameter_range = 0x00007c45;
gap_set_connectionless_cte_tx_enable = 0x00007c5d;
gap_set_connectionless_cte_tx_param = 0x00007c71;
gap_set_connectionless_iq_sampling_enable = 0x00007cd1;
gap_set_data_length = 0x00007d35;
gap_set_def_phy = 0x00007d51;
gap_set_ext_adv_data = 0x00007d69;
gap_set_ext_adv_enable = 0x00007d81;
gap_set_ext_adv_para = 0x00007dfd;
gap_set_ext_scan_enable = 0x00007edd;
gap_set_ext_scan_para = 0x00007ef5;
gap_set_ext_scan_response_data = 0x00007f9d;
gap_set_host_channel_classification = 0x00007fb5;
gap_set_periodic_adv_data = 0x00007fc9;
gap_set_periodic_adv_enable = 0x0000803d;
gap_set_periodic_adv_para = 0x00008051;
gap_set_periodic_adv_rx_enable = 0x00008069;
gap_set_phy = 0x0000807d;
gap_set_random_device_address = 0x00008099;
gap_start_ccm = 0x000080c9;
gap_test_end = 0x000080fd;
gap_tx_test_v2 = 0x0000810d;
gap_tx_test_v4 = 0x00008125;
gap_update_connection_parameters = 0x0000814d;
gap_vendor_tx_continuous_wave = 0x00008171;
gatt_client_cancel_write = 0x00008699;
gatt_client_discover_characteristic_descriptors = 0x000086bf;
gatt_client_discover_characteristics_for_handle_range_by_uuid128 = 0x000086ff;
gatt_client_discover_characteristics_for_handle_range_by_uuid16 = 0x0000874f;
gatt_client_discover_characteristics_for_service = 0x0000879f;
gatt_client_discover_primary_services = 0x000087d5;
gatt_client_discover_primary_services_by_uuid128 = 0x00008807;
gatt_client_discover_primary_services_by_uuid16 = 0x0000884b;
gatt_client_execute_write = 0x00008887;
gatt_client_find_included_services_for_service = 0x000088ad;
gatt_client_get_mtu = 0x000088db;
gatt_client_is_ready = 0x0000897d;
gatt_client_listen_for_characteristic_value_updates = 0x00008993;
gatt_client_prepare_write = 0x000089b5;
gatt_client_read_characteristic_descriptor_using_descriptor_handle = 0x000089f1;
gatt_client_read_long_characteristic_descriptor_using_descriptor_handle = 0x00008a1b;
gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset = 0x00008a21;
gatt_client_read_long_value_of_characteristic_using_value_handle = 0x00008a4f;
gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset = 0x00008a55;
gatt_client_read_multiple_characteristic_values = 0x00008a83;
gatt_client_read_value_of_characteristic_using_value_handle = 0x00008ab3;
gatt_client_read_value_of_characteristics_by_uuid128 = 0x00008ae1;
gatt_client_read_value_of_characteristics_by_uuid16 = 0x00008b2d;
gatt_client_register_handler = 0x00008b79;
gatt_client_reliable_write_long_value_of_characteristic = 0x00008b85;
gatt_client_signed_write_without_response = 0x00008fb5;
gatt_client_write_characteristic_descriptor_using_descriptor_handle = 0x00009079;
gatt_client_write_client_characteristic_configuration = 0x000090b3;
gatt_client_write_long_characteristic_descriptor_using_descriptor_handle = 0x00009105;
gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset = 0x00009115;
gatt_client_write_long_value_of_characteristic = 0x00009151;
gatt_client_write_long_value_of_characteristic_with_offset = 0x00009161;
gatt_client_write_value_of_characteristic = 0x0000919d;
gatt_client_write_value_of_characteristic_without_response = 0x000091d3;
hci_add_event_handler = 0x0000a715;
hci_power_control = 0x0000af0d;
hci_register_acl_packet_handler = 0x0000b0c1;
kv_commit = 0x0000b63d;
kv_get = 0x0000b695;
kv_init = 0x0000b6ad;
kv_put = 0x0000b715;
kv_remove = 0x0000b78d;
kv_remove_all = 0x0000b7c9;
kv_value_modified = 0x0000b80d;
kv_visit = 0x0000b811;
l2cap_can_send_fixed_channel_packet_now = 0x0000b8d1;
l2cap_can_send_packet_now = 0x0000b8d5;
l2cap_create_channel = 0x0000ba8d;
l2cap_disconnect = 0x0000bbc5;
l2cap_get_remote_mtu_for_local_cid = 0x0000be39;
l2cap_init = 0x0000c231;
l2cap_le_send_flow_control_credit = 0x0000c271;
l2cap_max_le_mtu = 0x0000c4f5;
l2cap_max_mtu = 0x0000c4f9;
l2cap_next_local_cid = 0x0000c4fd;
l2cap_next_sig_id = 0x0000c50d;
l2cap_register_fixed_channel = 0x0000c5a5;
l2cap_register_packet_handler = 0x0000c5c1;
l2cap_register_service = 0x0000c5cd;
l2cap_request_can_send_fix_channel_now_event = 0x0000c6b1;
l2cap_request_can_send_now_event = 0x0000c6d5;
l2cap_request_connection_parameter_update = 0x0000c6ef;
l2cap_send = 0x0000ca91;
l2cap_send_connectionless = 0x0000cb09;
l2cap_send_connectionless3 = 0x0000cb99;
l2cap_send_echo_request = 0x0000cc31;
l2cap_send_signaling_le = 0x0000cc95;
l2cap_unregister_service = 0x0000cced;
le_device_db_add = 0x0000cd45;
le_device_db_find = 0x0000ce19;
le_device_db_from_key = 0x0000ce45;
le_device_db_iter_cur = 0x0000ce4d;
le_device_db_iter_cur_key = 0x0000ce51;
le_device_db_iter_init = 0x0000ce55;
le_device_db_iter_next = 0x0000ce5d;
le_device_db_remove_key = 0x0000ce83;
ll_ackable_packet_alloc = 0x0000ceaf;
ll_ackable_packet_get_status = 0x0000cfb7;
ll_ackable_packet_run = 0x0000d029;
ll_ackable_packet_set_tx_data = 0x0000d0d1;
ll_attach_cte_to_adv_set = 0x0000d0ed;
ll_free = 0x0000d285;
ll_hint_on_ce_len = 0x0000d291;
ll_legacy_adv_set_interval = 0x0000d2c9;
ll_malloc = 0x0000d2d9;
ll_query_timing_info = 0x0000d411;
ll_raw_packet_alloc = 0x0000d45d;
ll_raw_packet_free = 0x0000d531;
ll_raw_packet_get_bare_rx_data = 0x0000d55b;
ll_raw_packet_get_iq_samples = 0x0000d621;
ll_raw_packet_get_rx_data = 0x0000d6bb;
ll_raw_packet_recv = 0x0000d751;
ll_raw_packet_send = 0x0000d80d;
ll_raw_packet_set_bare_data = 0x0000d8f5;
ll_raw_packet_set_bare_mode = 0x0000d933;
ll_raw_packet_set_param = 0x0000da39;
ll_raw_packet_set_rx_cte = 0x0000da97;
ll_raw_packet_set_tx_cte = 0x0000db2d;
ll_raw_packet_set_tx_data = 0x0000db6b;
ll_scan_set_fixed_channel = 0x0000dc21;
ll_scanner_enable_iq_sampling = 0x0000dc2d;
ll_set_adv_access_address = 0x0000dddd;
ll_set_adv_coded_scheme = 0x0000dde9;
ll_set_conn_coded_scheme = 0x0000de19;
ll_set_conn_interval_unit = 0x0000de45;
ll_set_conn_latency = 0x0000de51;
ll_set_conn_tx_power = 0x0000de81;
ll_set_def_antenna = 0x0000dec9;
ll_set_initiating_coded_scheme = 0x0000dee5;
ll_set_max_conn_number = 0x0000def1;
nibble_for_char = 0x0001edf1;
platform_32k_rc_auto_tune = 0x0001ee8d;
platform_32k_rc_tune = 0x0001ef09;
platform_calibrate_32k = 0x0001ef1d;
platform_config = 0x0001ef21;
platform_controller_run = 0x0001efdd;
platform_get_task_handle = 0x0001f015;
platform_get_us_time = 0x0001f02d;
platform_get_version = 0x0001f031;
platform_hrng = 0x0001f039;
platform_init_controller = 0x0001f041;
platform_os_idle_resumed_hook = 0x0001f05d;
platform_patch_rf_init_data = 0x0001f061;
platform_post_sleep_processing = 0x0001f06d;
platform_pre_sleep_processing = 0x0001f073;
platform_pre_suppress_ticks_and_sleep_processing = 0x0001f079;
platform_printf = 0x0001f07d;
platform_raise_assertion = 0x0001f091;
platform_rand = 0x0001f0a5;
platform_read_info = 0x0001f0a9;
platform_read_persistent_reg = 0x0001f0c5;
platform_reset = 0x0001f0d5;
platform_set_evt_callback = 0x0001f0f9;
platform_set_irq_callback = 0x0001f10d;
platform_set_rf_clk_source = 0x0001f145;
platform_set_rf_init_data = 0x0001f151;
platform_set_rf_power_mapping = 0x0001f15d;
platform_set_timer = 0x0001f169;
platform_shutdown = 0x0001f16d;
platform_switch_app = 0x0001f171;
platform_trace_raw = 0x0001f19d;
platform_write_persistent_reg = 0x0001f1b5;
printf_hexdump = 0x0001f1c5;
reverse_128 = 0x0001f545;
reverse_24 = 0x0001f54b;
reverse_48 = 0x0001f551;
reverse_56 = 0x0001f557;
reverse_64 = 0x0001f55d;
reverse_bd_addr = 0x0001f563;
reverse_bytes = 0x0001f569;
sm_add_event_handler = 0x0001f849;
sm_address_resolution_lookup = 0x0001f9a1;
sm_authenticated = 0x0001fced;
sm_authorization_decline = 0x0001fcfb;
sm_authorization_grant = 0x0001fd1b;
sm_authorization_state = 0x0001fd3b;
sm_bonding_decline = 0x0001fd55;
sm_config = 0x00020175;
sm_config_conn = 0x0002018d;
sm_encryption_key_size = 0x00020343;
sm_just_works_confirm = 0x0002087d;
sm_le_device_key = 0x00020bb9;
sm_passkey_input = 0x00020c4f;
sm_private_random_address_generation_get = 0x00020ffd;
sm_private_random_address_generation_get_mode = 0x00021005;
sm_private_random_address_generation_set_mode = 0x00021011;
sm_private_random_address_generation_set_update_period = 0x00021039;
sm_register_oob_data_callback = 0x00021175;
sm_request_pairing = 0x00021181;
sm_send_security_request = 0x00021b87;
sm_set_accepted_stk_generation_methods = 0x00021bad;
sm_set_authentication_requirements = 0x00021bb9;
sm_set_encryption_key_size_range = 0x00021bc5;
sscanf_bd_addr = 0x00021f89;
sysSetPublicDeviceAddr = 0x000222f1;
uuid128_to_str = 0x00022a7d;
uuid_add_bluetooth_prefix = 0x00022ad5;
uuid_has_bluetooth_prefix = 0x00022af5;