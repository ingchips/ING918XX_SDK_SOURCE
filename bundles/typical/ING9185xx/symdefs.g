att_dispatch_client_can_send_now = 0x000059d1;
att_dispatch_client_request_can_send_now_event = 0x000059d7;
att_dispatch_register_client = 0x000059dd;
att_dispatch_register_server = 0x000059f1;
att_dispatch_server_can_send_now = 0x00005a05;
att_dispatch_server_request_can_send_now_event = 0x00005a0b;
att_emit_general_event = 0x00005abd;
att_server_can_send_packet_now = 0x000061d1;
att_server_deferred_read_response = 0x000061d5;
att_server_get_mtu = 0x000061ed;
att_server_indicate = 0x00006265;
att_server_init = 0x000062e9;
att_server_notify = 0x00006325;
att_server_register_packet_handler = 0x0000643d;
att_server_request_can_send_now_event = 0x00006449;
att_set_db = 0x00006465;
att_set_read_callback = 0x00006479;
att_set_write_callback = 0x00006485;
bd_addr_cmp = 0x000065f5;
bd_addr_copy = 0x000065fb;
bd_addr_to_str = 0x00006605;
big_endian_read_16 = 0x0000663d;
big_endian_read_32 = 0x00006645;
big_endian_store_16 = 0x00006659;
big_endian_store_32 = 0x00006665;
btstack_config = 0x000067b9;
btstack_memory_pool_create = 0x000068f7;
btstack_memory_pool_free = 0x00006921;
btstack_memory_pool_get = 0x00006981;
btstack_push_user_msg = 0x0000699d;
char_for_nibble = 0x00006c65;
eTaskConfirmSleepModeStatus = 0x00006f0d;
gap_add_dev_to_periodic_list = 0x000075b9;
gap_add_whitelist = 0x000075d1;
gap_aes_encrypt = 0x000075e5;
gap_clear_white_lists = 0x0000760d;
gap_clr_adv_set = 0x0000761d;
gap_clr_periodic_adv_list = 0x0000762d;
gap_create_connection_cancel = 0x00007659;
gap_disconnect = 0x00007669;
gap_disconnect_all = 0x00007695;
gap_ext_create_connection = 0x0000773d;
gap_get_connection_parameter_range = 0x00007801;
gap_le_read_channel_map = 0x00007839;
gap_periodic_adv_create_sync = 0x000078ad;
gap_periodic_adv_create_sync_cancel = 0x000078d1;
gap_periodic_adv_term_sync = 0x000078e1;
gap_read_periodic_adv_list_size = 0x00007975;
gap_read_phy = 0x00007985;
gap_read_remote_info = 0x00007999;
gap_read_remote_used_features = 0x000079ad;
gap_read_rssi = 0x000079c1;
gap_remove_whitelist = 0x000079d5;
gap_rmv_adv_set = 0x00007a61;
gap_rmv_dev_from_periodic_list = 0x00007a75;
gap_rx_test_v2 = 0x00007a8d;
gap_set_adv_set_random_addr = 0x00007acd;
gap_set_connection_parameter_range = 0x00007b19;
gap_set_data_length = 0x00007b31;
gap_set_def_phy = 0x00007b4d;
gap_set_ext_adv_data = 0x00007b65;
gap_set_ext_adv_enable = 0x00007b7d;
gap_set_ext_adv_para = 0x00007bf9;
gap_set_ext_scan_enable = 0x00007cd9;
gap_set_ext_scan_para = 0x00007cf1;
gap_set_ext_scan_response_data = 0x00007d99;
gap_set_host_channel_classification = 0x00007db1;
gap_set_periodic_adv_data = 0x00007dc5;
gap_set_periodic_adv_enable = 0x00007e39;
gap_set_periodic_adv_para = 0x00007e4d;
gap_set_phy = 0x00007e65;
gap_set_random_device_address = 0x00007e81;
gap_start_ccm = 0x00007ee5;
gap_test_end = 0x00007f19;
gap_tx_test_v2 = 0x00007f29;
gap_update_connection_parameters = 0x00007f41;
gap_vendor_tx_continuous_wave = 0x00007f65;
gatt_client_cancel_write = 0x0000848d;
gatt_client_discover_characteristic_descriptors = 0x000084b3;
gatt_client_discover_characteristics_for_handle_range_by_uuid128 = 0x000084f3;
gatt_client_discover_characteristics_for_handle_range_by_uuid16 = 0x00008543;
gatt_client_discover_characteristics_for_service = 0x00008593;
gatt_client_discover_primary_services = 0x000085c9;
gatt_client_discover_primary_services_by_uuid128 = 0x000085fb;
gatt_client_discover_primary_services_by_uuid16 = 0x0000863f;
gatt_client_execute_write = 0x0000867b;
gatt_client_find_included_services_for_service = 0x000086a1;
gatt_client_get_mtu = 0x000086cf;
gatt_client_is_ready = 0x00008771;
gatt_client_listen_for_characteristic_value_updates = 0x00008787;
gatt_client_prepare_write = 0x000087a9;
gatt_client_read_characteristic_descriptor_using_descriptor_handle = 0x000087e5;
gatt_client_read_long_characteristic_descriptor_using_descriptor_handle = 0x0000880f;
gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset = 0x00008815;
gatt_client_read_long_value_of_characteristic_using_value_handle = 0x00008843;
gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset = 0x00008849;
gatt_client_read_multiple_characteristic_values = 0x00008877;
gatt_client_read_value_of_characteristic_using_value_handle = 0x000088a7;
gatt_client_read_value_of_characteristics_by_uuid128 = 0x000088d5;
gatt_client_read_value_of_characteristics_by_uuid16 = 0x00008921;
gatt_client_register_handler = 0x0000896d;
gatt_client_reliable_write_long_value_of_characteristic = 0x00008979;
gatt_client_signed_write_without_response = 0x00008da9;
gatt_client_write_characteristic_descriptor_using_descriptor_handle = 0x00008e6d;
gatt_client_write_client_characteristic_configuration = 0x00008ea7;
gatt_client_write_long_characteristic_descriptor_using_descriptor_handle = 0x00008ef9;
gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset = 0x00008f09;
gatt_client_write_long_value_of_characteristic = 0x00008f45;
gatt_client_write_long_value_of_characteristic_with_offset = 0x00008f55;
gatt_client_write_value_of_characteristic = 0x00008f91;
gatt_client_write_value_of_characteristic_without_response = 0x00008fc7;
hci_add_event_handler = 0x0000a4ad;
hci_power_control = 0x0000ac91;
hci_register_acl_packet_handler = 0x0000ae45;
kv_commit = 0x0000b449;
kv_get = 0x0000b4a1;
kv_init = 0x0000b4b9;
kv_put = 0x0000b521;
kv_remove = 0x0000b599;
kv_remove_all = 0x0000b5d5;
kv_value_modified = 0x0000b619;
kv_visit = 0x0000b61d;
l2cap_can_send_fixed_channel_packet_now = 0x0000b6dd;
l2cap_can_send_packet_now = 0x0000b6e1;
l2cap_create_channel = 0x0000b899;
l2cap_disconnect = 0x0000b9d1;
l2cap_get_remote_mtu_for_local_cid = 0x0000bc45;
l2cap_init = 0x0000c0c5;
l2cap_le_send_flow_control_credit = 0x0000c10d;
l2cap_max_le_mtu = 0x0000c391;
l2cap_max_mtu = 0x0000c395;
l2cap_next_local_cid = 0x0000c399;
l2cap_next_sig_id = 0x0000c3a9;
l2cap_register_fixed_channel = 0x0000c441;
l2cap_register_packet_handler = 0x0000c45d;
l2cap_register_service = 0x0000c469;
l2cap_request_can_send_fix_channel_now_event = 0x0000c551;
l2cap_request_can_send_now_event = 0x0000c575;
l2cap_request_connection_parameter_update = 0x0000c58f;
l2cap_require_security_level_2_for_outgoing_sdp = 0x0000c5c1;
l2cap_send = 0x0000c955;
l2cap_send_connectionless = 0x0000c9cd;
l2cap_send_connectionless3 = 0x0000ca5d;
l2cap_send_echo_request = 0x0000caf5;
l2cap_send_signaling_le = 0x0000cb59;
l2cap_unregister_service = 0x0000cbb1;
ll_free = 0x0000cd7f;
ll_hint_on_ce_len = 0x0000cd89;
ll_legacy_adv_set_interval = 0x0000cdc9;
ll_malloc = 0x0000cdd9;
ll_query_timing_info = 0x0000cf11;
ll_scan_set_fixed_channel = 0x0000cfb5;
ll_set_adv_access_address = 0x0000d0c9;
ll_set_adv_coded_scheme = 0x0000d0d5;
ll_set_conn_coded_scheme = 0x0000d105;
ll_set_conn_latency = 0x0000d131;
ll_set_conn_tx_power = 0x0000d161;
ll_set_def_antenna = 0x0000d1a9;
ll_set_initiating_coded_scheme = 0x0000d1c5;
ll_set_max_conn_number = 0x0000d1d1;
nibble_for_char = 0x0001cde5;
platform_32k_rc_auto_tune = 0x0001ce81;
platform_32k_rc_tune = 0x0001cefd;
platform_calibrate_32k = 0x0001cf11;
platform_config = 0x0001cf15;
platform_get_heap_status = 0x0001cfcd;
platform_get_us_time = 0x0001cfe5;
platform_get_version = 0x0001cfe9;
platform_hrng = 0x0001cff1;
platform_install_isr_stack = 0x0001cff9;
platform_patch_rf_init_data = 0x0001d005;
platform_printf = 0x0001d011;
platform_raise_assertion = 0x0001d025;
platform_rand = 0x0001d039;
platform_read_info = 0x0001d03d;
platform_read_persistent_reg = 0x0001d059;
platform_reset = 0x0001d069;
platform_set_evt_callback = 0x0001d09d;
platform_set_irq_callback = 0x0001d0b1;
platform_set_rf_clk_source = 0x0001d0e9;
platform_set_rf_init_data = 0x0001d0f5;
platform_set_rf_power_mapping = 0x0001d101;
platform_shutdown = 0x0001d10d;
platform_switch_app = 0x0001d111;
platform_trace_raw = 0x0001d13d;
platform_write_persistent_reg = 0x0001d151;
printf_hexdump = 0x0001d161;
pvPortMalloc = 0x0001dc31;
pvTaskIncrementMutexHeldCount = 0x0001dd19;
pvTimerGetTimerID = 0x0001dd31;
pxPortInitialiseStack = 0x0001dd5d;
reverse_128 = 0x0001df05;
reverse_24 = 0x0001df0b;
reverse_48 = 0x0001df11;
reverse_56 = 0x0001df17;
reverse_64 = 0x0001df1d;
reverse_bd_addr = 0x0001df23;
reverse_bytes = 0x0001df29;
sm_add_event_handler = 0x0001e095;
sm_address_resolution_lookup = 0x0001e1c1;
sm_authenticated = 0x0001e29b;
sm_authorization_decline = 0x0001e2a9;
sm_authorization_grant = 0x0001e2c9;
sm_authorization_state = 0x0001e2e9;
sm_bonding_decline = 0x0001e305;
sm_config = 0x0001e6dd;
sm_encryption_key_size = 0x0001e7e9;
sm_just_works_confirm = 0x0001efa1;
sm_le_device_key = 0x0001f1f9;
sm_passkey_input = 0x0001f291;
sm_private_random_address_generation_get = 0x0001f61d;
sm_private_random_address_generation_get_mode = 0x0001f625;
sm_private_random_address_generation_set_mode = 0x0001f631;
sm_private_random_address_generation_set_update_period = 0x0001f659;
sm_register_oob_data_callback = 0x0001f691;
sm_request_pairing = 0x0001f69d;
sm_send_security_request = 0x000200ed;
sm_set_accepted_stk_generation_methods = 0x00020115;
sm_set_authentication_requirements = 0x00020121;
sm_set_encryption_key_size_range = 0x0002012d;
sscanf_bd_addr = 0x0002041d;
sysSetPublicDeviceAddr = 0x00020749;
uuid128_to_str = 0x00020ced;
uuid_add_bluetooth_prefix = 0x00020d45;
uuid_has_bluetooth_prefix = 0x00020d65;
uxQueueMessagesWaiting = 0x00020da9;
uxQueueMessagesWaitingFromISR = 0x00020dd1;
uxQueueSpacesAvailable = 0x00020ded;
uxTaskGetStackHighWaterMark = 0x00020e19;
vPortEnterCritical = 0x00020f3d;
vPortExitCritical = 0x00020f7d;
vPortFree = 0x00020fa9;
vPortSuppressTicksAndSleep = 0x0002103d;
vPortValidateInterruptPriority = 0x00021115;
vQueueDelete = 0x00021169;
vQueueWaitForMessageRestricted = 0x00021195;
vTaskDelay = 0x000211dd;
vTaskInternalSetTimeOutState = 0x00021229;
vTaskMissedYield = 0x00021239;
vTaskPlaceOnEventList = 0x00021245;
vTaskPlaceOnEventListRestricted = 0x0002127d;
vTaskPriorityDisinheritAfterTimeout = 0x000212bd;
vTaskStartScheduler = 0x00021369;
vTaskStepTick = 0x000213f9;
vTaskSuspendAll = 0x00021429;
vTaskSwitchContext = 0x00021439;
xPortStartScheduler = 0x000214e1;
xQueueAddToSet = 0x0002159d;
xQueueCreateCountingSemaphore = 0x000215c1;
xQueueCreateCountingSemaphoreStatic = 0x000215fd;
xQueueCreateMutex = 0x00021641;
xQueueCreateMutexStatic = 0x00021657;
xQueueCreateSet = 0x00021671;
xQueueGenericCreate = 0x00021679;
xQueueGenericCreateStatic = 0x000216c5;
xQueueGenericReset = 0x0002172d;
xQueueGenericSend = 0x000217b9;
xQueueGenericSendFromISR = 0x00021925;
xQueueGiveFromISR = 0x000219e5;
xQueueGiveMutexRecursive = 0x00021a89;
xQueueIsQueueEmptyFromISR = 0x00021ac9;
xQueueIsQueueFullFromISR = 0x00021aed;
xQueuePeek = 0x00021b15;
xQueuePeekFromISR = 0x00021c3d;
xQueueReceive = 0x00021ca9;
xQueueReceiveFromISR = 0x00021dd5;
xQueueRemoveFromSet = 0x00021e69;
xQueueSelectFromSet = 0x00021e8b;
xQueueSelectFromSetFromISR = 0x00021e9d;
xQueueSemaphoreTake = 0x00021eb1;
xQueueTakeMutexRecursive = 0x0002201d;
xTaskCheckForTimeOut = 0x00022061;
xTaskCreate = 0x000220d1;
xTaskCreateStatic = 0x0002212d;
xTaskGetCurrentTaskHandle = 0x0002219d;
xTaskGetSchedulerState = 0x000221a9;
xTaskGetTickCount = 0x000221c5;
xTaskGetTickCountFromISR = 0x000221d1;
xTaskIncrementTick = 0x000221e1;
xTaskPriorityDisinherit = 0x000222ad;
xTaskPriorityInherit = 0x00022341;
xTaskRemoveFromEventList = 0x000223d5;
xTaskResumeAll = 0x00022455;
xTimerCreate = 0x0002251d;
xTimerCreateStatic = 0x00022551;
xTimerCreateTimerTask = 0x00022589;
xTimerGenericCommand = 0x000225f5;
xTimerGetExpiryTime = 0x00022665;
