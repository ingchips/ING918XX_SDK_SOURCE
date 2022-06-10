att_dispatch_client_can_send_now = 0x000059c1;
att_dispatch_client_request_can_send_now_event = 0x000059c7;
att_dispatch_register_client = 0x000059cd;
att_dispatch_register_server = 0x000059e1;
att_dispatch_server_can_send_now = 0x000059f5;
att_dispatch_server_request_can_send_now_event = 0x000059fb;
att_emit_general_event = 0x00005aad;
att_server_can_send_packet_now = 0x000061c1;
att_server_deferred_read_response = 0x000061c5;
att_server_get_mtu = 0x000061dd;
att_server_indicate = 0x00006255;
att_server_init = 0x000062d9;
att_server_notify = 0x00006315;
att_server_register_packet_handler = 0x0000642d;
att_server_request_can_send_now_event = 0x00006439;
att_set_db = 0x00006455;
att_set_read_callback = 0x00006469;
att_set_write_callback = 0x00006475;
bd_addr_cmp = 0x000065e5;
bd_addr_copy = 0x000065eb;
bd_addr_to_str = 0x000065f5;
big_endian_read_16 = 0x0000662d;
big_endian_read_32 = 0x00006635;
big_endian_store_16 = 0x00006649;
big_endian_store_32 = 0x00006655;
btstack_config = 0x000067a9;
btstack_memory_pool_create = 0x000068e7;
btstack_memory_pool_free = 0x00006911;
btstack_memory_pool_get = 0x00006971;
btstack_push_user_msg = 0x0000698d;
char_for_nibble = 0x00006c55;
eTaskConfirmSleepModeStatus = 0x00006efd;
gap_add_dev_to_periodic_list = 0x0000751d;
gap_add_whitelist = 0x00007535;
gap_aes_encrypt = 0x00007549;
gap_clear_white_lists = 0x0000758d;
gap_clr_adv_set = 0x0000759d;
gap_clr_periodic_adv_list = 0x000075ad;
gap_create_connection_cancel = 0x000075bd;
gap_disconnect = 0x000075cd;
gap_disconnect_all = 0x000075f9;
gap_ext_create_connection = 0x00007639;
gap_get_connection_parameter_range = 0x00007729;
gap_le_read_channel_map = 0x00007765;
gap_periodic_adv_create_sync = 0x000077d9;
gap_periodic_adv_create_sync_cancel = 0x000077fd;
gap_periodic_adv_term_sync = 0x0000780d;
gap_read_periodic_adv_list_size = 0x000078a1;
gap_read_phy = 0x000078b1;
gap_read_remote_info = 0x000078c5;
gap_read_remote_used_features = 0x000078d9;
gap_read_rssi = 0x000078ed;
gap_remove_whitelist = 0x00007901;
gap_rmv_adv_set = 0x00007985;
gap_rmv_dev_from_periodic_list = 0x00007999;
gap_rx_test_v2 = 0x000079b1;
gap_set_adv_set_random_addr = 0x000079f1;
gap_set_connection_parameter_range = 0x00007a3d;
gap_set_data_length = 0x00007a55;
gap_set_def_phy = 0x00007a71;
gap_set_ext_adv_data = 0x00007a89;
gap_set_ext_adv_enable = 0x00007aa1;
gap_set_ext_adv_para = 0x00007b1d;
gap_set_ext_scan_enable = 0x00007bfd;
gap_set_ext_scan_para = 0x00007c15;
gap_set_ext_scan_response_data = 0x00007cbd;
gap_set_host_channel_classification = 0x00007cd5;
gap_set_periodic_adv_data = 0x00007ce9;
gap_set_periodic_adv_enable = 0x00007d5d;
gap_set_periodic_adv_para = 0x00007d71;
gap_set_phy = 0x00007d89;
gap_set_random_device_address = 0x00007da5;
gap_start_ccm = 0x00007dd5;
gap_test_end = 0x00007e09;
gap_tx_test_v2 = 0x00007e19;
gap_tx_test_v4 = 0x00007e31;
gap_update_connection_parameters = 0x00007e59;
gap_vendor_tx_continuous_wave = 0x00007e7d;
gatt_client_cancel_write = 0x000083a5;
gatt_client_discover_characteristic_descriptors = 0x000083cb;
gatt_client_discover_characteristics_for_handle_range_by_uuid128 = 0x0000840b;
gatt_client_discover_characteristics_for_handle_range_by_uuid16 = 0x0000845b;
gatt_client_discover_characteristics_for_service = 0x000084ab;
gatt_client_discover_primary_services = 0x000084e1;
gatt_client_discover_primary_services_by_uuid128 = 0x00008513;
gatt_client_discover_primary_services_by_uuid16 = 0x00008557;
gatt_client_execute_write = 0x00008593;
gatt_client_find_included_services_for_service = 0x000085b9;
gatt_client_get_mtu = 0x000085e7;
gatt_client_is_ready = 0x00008689;
gatt_client_listen_for_characteristic_value_updates = 0x0000869f;
gatt_client_prepare_write = 0x000086c1;
gatt_client_read_characteristic_descriptor_using_descriptor_handle = 0x000086fd;
gatt_client_read_long_characteristic_descriptor_using_descriptor_handle = 0x00008727;
gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset = 0x0000872d;
gatt_client_read_long_value_of_characteristic_using_value_handle = 0x0000875b;
gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset = 0x00008761;
gatt_client_read_multiple_characteristic_values = 0x0000878f;
gatt_client_read_value_of_characteristic_using_value_handle = 0x000087bf;
gatt_client_read_value_of_characteristics_by_uuid128 = 0x000087ed;
gatt_client_read_value_of_characteristics_by_uuid16 = 0x00008839;
gatt_client_register_handler = 0x00008885;
gatt_client_reliable_write_long_value_of_characteristic = 0x00008891;
gatt_client_signed_write_without_response = 0x00008cc1;
gatt_client_write_characteristic_descriptor_using_descriptor_handle = 0x00008d85;
gatt_client_write_client_characteristic_configuration = 0x00008dbf;
gatt_client_write_long_characteristic_descriptor_using_descriptor_handle = 0x00008e11;
gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset = 0x00008e21;
gatt_client_write_long_value_of_characteristic = 0x00008e5d;
gatt_client_write_long_value_of_characteristic_with_offset = 0x00008e6d;
gatt_client_write_value_of_characteristic = 0x00008ea9;
gatt_client_write_value_of_characteristic_without_response = 0x00008edf;
hci_add_event_handler = 0x0000a405;
hci_power_control = 0x0000abf9;
hci_register_acl_packet_handler = 0x0000adad;
kv_commit = 0x0000b321;
kv_get = 0x0000b379;
kv_init = 0x0000b391;
kv_put = 0x0000b3f9;
kv_remove = 0x0000b471;
kv_remove_all = 0x0000b4ad;
kv_value_modified = 0x0000b4f1;
kv_visit = 0x0000b4f5;
l2cap_can_send_fixed_channel_packet_now = 0x0000b5b5;
l2cap_can_send_packet_now = 0x0000b5b9;
l2cap_create_channel = 0x0000b771;
l2cap_disconnect = 0x0000b8a9;
l2cap_get_remote_mtu_for_local_cid = 0x0000bb1d;
l2cap_init = 0x0000bf15;
l2cap_le_send_flow_control_credit = 0x0000bf55;
l2cap_max_le_mtu = 0x0000c1d9;
l2cap_max_mtu = 0x0000c1dd;
l2cap_next_local_cid = 0x0000c1e1;
l2cap_next_sig_id = 0x0000c1f1;
l2cap_register_fixed_channel = 0x0000c289;
l2cap_register_packet_handler = 0x0000c2a5;
l2cap_register_service = 0x0000c2b1;
l2cap_request_can_send_fix_channel_now_event = 0x0000c395;
l2cap_request_can_send_now_event = 0x0000c3b9;
l2cap_request_connection_parameter_update = 0x0000c3d3;
l2cap_send = 0x0000c775;
l2cap_send_connectionless = 0x0000c7ed;
l2cap_send_connectionless3 = 0x0000c87d;
l2cap_send_echo_request = 0x0000c915;
l2cap_send_signaling_le = 0x0000c979;
l2cap_unregister_service = 0x0000c9d1;
le_device_db_add = 0x0000ca29;
le_device_db_find = 0x0000cafd;
le_device_db_from_key = 0x0000cb29;
le_device_db_iter_cur = 0x0000cb31;
le_device_db_iter_cur_key = 0x0000cb35;
le_device_db_iter_init = 0x0000cb39;
le_device_db_iter_next = 0x0000cb41;
le_device_db_remove_key = 0x0000cb67;
ll_free = 0x0000cb93;
ll_hint_on_ce_len = 0x0000cb9d;
ll_legacy_adv_set_interval = 0x0000cbd5;
ll_malloc = 0x0000cbe5;
ll_query_timing_info = 0x0000cd1d;
ll_scan_set_fixed_channel = 0x0000cdc1;
ll_set_adv_access_address = 0x0000ced5;
ll_set_adv_coded_scheme = 0x0000cee1;
ll_set_conn_coded_scheme = 0x0000cf11;
ll_set_conn_latency = 0x0000cf3d;
ll_set_conn_tx_power = 0x0000cf6d;
ll_set_def_antenna = 0x0000cfb5;
ll_set_initiating_coded_scheme = 0x0000cfd1;
ll_set_max_conn_number = 0x0000cfdd;
nibble_for_char = 0x0001cdf5;
platform_32k_rc_auto_tune = 0x0001ce91;
platform_32k_rc_tune = 0x0001cf0d;
platform_calibrate_32k = 0x0001cf21;
platform_config = 0x0001cf25;
platform_get_heap_status = 0x0001cff9;
platform_get_task_handle = 0x0001d011;
platform_get_us_time = 0x0001d031;
platform_get_version = 0x0001d035;
platform_hrng = 0x0001d03d;
platform_install_isr_stack = 0x0001d045;
platform_patch_rf_init_data = 0x0001d051;
platform_printf = 0x0001d05d;
platform_raise_assertion = 0x0001d071;
platform_rand = 0x0001d085;
platform_read_info = 0x0001d089;
platform_read_persistent_reg = 0x0001d0a5;
platform_reset = 0x0001d0b5;
platform_set_evt_callback = 0x0001d0e9;
platform_set_irq_callback = 0x0001d0fd;
platform_set_rf_clk_source = 0x0001d135;
platform_set_rf_init_data = 0x0001d141;
platform_set_rf_power_mapping = 0x0001d14d;
platform_set_timer = 0x0001d159;
platform_shutdown = 0x0001d15d;
platform_switch_app = 0x0001d161;
platform_trace_raw = 0x0001d18d;
platform_write_persistent_reg = 0x0001d1a5;
printf_hexdump = 0x0001d1b5;
pvPortMalloc = 0x0001dcc1;
pvTaskIncrementMutexHeldCount = 0x0001dda9;
pvTimerGetTimerID = 0x0001ddc1;
pxPortInitialiseStack = 0x0001dded;
reverse_128 = 0x0001df95;
reverse_24 = 0x0001df9b;
reverse_48 = 0x0001dfa1;
reverse_56 = 0x0001dfa7;
reverse_64 = 0x0001dfad;
reverse_bd_addr = 0x0001dfb3;
reverse_bytes = 0x0001dfb9;
sm_add_event_handler = 0x0001e125;
sm_address_resolution_lookup = 0x0001e27d;
sm_authenticated = 0x0001e5d5;
sm_authorization_decline = 0x0001e5e3;
sm_authorization_grant = 0x0001e603;
sm_authorization_state = 0x0001e623;
sm_bonding_decline = 0x0001e63d;
sm_config = 0x0001ea5d;
sm_config_conn = 0x0001ea75;
sm_encryption_key_size = 0x0001ec2b;
sm_just_works_confirm = 0x0001f135;
sm_le_device_key = 0x0001f471;
sm_passkey_input = 0x0001f507;
sm_private_random_address_generation_get = 0x0001f8b5;
sm_private_random_address_generation_get_mode = 0x0001f8bd;
sm_private_random_address_generation_set_mode = 0x0001f8c9;
sm_private_random_address_generation_set_update_period = 0x0001f8f1;
sm_register_oob_data_callback = 0x0001fa2d;
sm_request_pairing = 0x0001fa39;
sm_send_security_request = 0x00020457;
sm_set_accepted_stk_generation_methods = 0x0002047d;
sm_set_authentication_requirements = 0x00020489;
sm_set_encryption_key_size_range = 0x00020495;
sscanf_bd_addr = 0x000207f1;
sysSetPublicDeviceAddr = 0x00020b59;
uuid128_to_str = 0x0002113d;
uuid_add_bluetooth_prefix = 0x00021195;
uuid_has_bluetooth_prefix = 0x000211b5;
uxListRemove = 0x000211d1;
uxQueueMessagesWaiting = 0x000211f9;
uxQueueMessagesWaitingFromISR = 0x00021221;
uxQueueSpacesAvailable = 0x0002123d;
uxTaskGetStackHighWaterMark = 0x00021269;
uxTaskPriorityGet = 0x00021289;
uxTaskPriorityGetFromISR = 0x000212a5;
vListInitialise = 0x00021357;
vListInitialiseItem = 0x0002136d;
vListInsert = 0x00021373;
vListInsertEnd = 0x000213a3;
vPortEndScheduler = 0x000213bd;
vPortEnterCritical = 0x000213e5;
vPortExitCritical = 0x00021429;
vPortFree = 0x00021459;
vPortSuppressTicksAndSleep = 0x000214ed;
vPortValidateInterruptPriority = 0x000215f5;
vQueueDelete = 0x0002164d;
vQueueWaitForMessageRestricted = 0x00021679;
vTaskDelay = 0x000216c1;
vTaskInternalSetTimeOutState = 0x0002170d;
vTaskMissedYield = 0x0002171d;
vTaskPlaceOnEventList = 0x00021729;
vTaskPlaceOnEventListRestricted = 0x00021761;
vTaskPriorityDisinheritAfterTimeout = 0x000217a1;
vTaskPrioritySet = 0x0002184d;
vTaskResume = 0x00021915;
vTaskStartScheduler = 0x00021999;
vTaskStepTick = 0x00021a29;
vTaskSuspend = 0x00021a59;
vTaskSuspendAll = 0x00021b15;
vTaskSwitchContext = 0x00021b25;
xPortStartScheduler = 0x00021bcd;
xQueueAddToSet = 0x00021c8d;
xQueueCreateCountingSemaphore = 0x00021cb1;
xQueueCreateCountingSemaphoreStatic = 0x00021ced;
xQueueCreateMutex = 0x00021d31;
xQueueCreateMutexStatic = 0x00021d47;
xQueueCreateSet = 0x00021d61;
xQueueGenericCreate = 0x00021d69;
xQueueGenericCreateStatic = 0x00021db5;
xQueueGenericReset = 0x00021e1d;
xQueueGenericSend = 0x00021ea9;
xQueueGenericSendFromISR = 0x00022015;
xQueueGiveFromISR = 0x000220d5;
xQueueGiveMutexRecursive = 0x00022179;
xQueueIsQueueEmptyFromISR = 0x000221b9;
xQueueIsQueueFullFromISR = 0x000221dd;
xQueuePeek = 0x00022205;
xQueuePeekFromISR = 0x0002232d;
xQueueReceive = 0x00022399;
xQueueReceiveFromISR = 0x000224c5;
xQueueRemoveFromSet = 0x00022559;
xQueueSelectFromSet = 0x0002257b;
xQueueSelectFromSetFromISR = 0x0002258d;
xQueueSemaphoreTake = 0x000225a1;
xQueueTakeMutexRecursive = 0x0002270d;
xTaskCheckForTimeOut = 0x00022751;
xTaskCreate = 0x000227c1;
xTaskCreateStatic = 0x0002281d;
xTaskGetCurrentTaskHandle = 0x0002288d;
xTaskGetSchedulerState = 0x00022899;
xTaskGetTickCount = 0x000228b5;
xTaskGetTickCountFromISR = 0x000228c1;
xTaskIncrementTick = 0x000228d1;
xTaskPriorityDisinherit = 0x0002299d;
xTaskPriorityInherit = 0x00022a31;
xTaskRemoveFromEventList = 0x00022ac5;
xTaskResumeAll = 0x00022b45;
xTaskResumeFromISR = 0x00022c0d;
xTimerCreate = 0x00022c99;
xTimerCreateStatic = 0x00022ccd;
xTimerCreateTimerTask = 0x00022d05;
xTimerGenericCommand = 0x00022d71;
xTimerGetExpiryTime = 0x00022de1;
xTimerGetTimerDaemonTaskHandle = 0x00022e01;
