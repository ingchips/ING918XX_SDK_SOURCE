--define_symbol att_dispatch_client_can_send_now=0x000059d1
--define_symbol att_dispatch_client_request_can_send_now_event=0x000059d7
--define_symbol att_dispatch_register_client=0x000059dd
--define_symbol att_dispatch_register_server=0x000059f1
--define_symbol att_dispatch_server_can_send_now=0x00005a05
--define_symbol att_dispatch_server_request_can_send_now_event=0x00005a0b
--define_symbol att_emit_general_event=0x00005abd
--define_symbol att_server_can_send_packet_now=0x000061d1
--define_symbol att_server_deferred_read_response=0x000061d5
--define_symbol att_server_get_mtu=0x000061ed
--define_symbol att_server_indicate=0x00006265
--define_symbol att_server_init=0x000062e9
--define_symbol att_server_notify=0x00006325
--define_symbol att_server_register_packet_handler=0x0000643d
--define_symbol att_server_request_can_send_now_event=0x00006449
--define_symbol att_set_db=0x00006465
--define_symbol att_set_read_callback=0x00006479
--define_symbol att_set_write_callback=0x00006485
--define_symbol bd_addr_cmp=0x000065f5
--define_symbol bd_addr_copy=0x000065fb
--define_symbol bd_addr_to_str=0x00006605
--define_symbol big_endian_read_16=0x0000663d
--define_symbol big_endian_read_32=0x00006645
--define_symbol big_endian_store_16=0x00006659
--define_symbol big_endian_store_32=0x00006665
--define_symbol btstack_config=0x000067b9
--define_symbol btstack_memory_pool_create=0x000068f7
--define_symbol btstack_memory_pool_free=0x00006921
--define_symbol btstack_memory_pool_get=0x00006981
--define_symbol btstack_push_user_msg=0x0000699d
--define_symbol char_for_nibble=0x00006c65
--define_symbol eTaskConfirmSleepModeStatus=0x00006f0d
--define_symbol gap_add_dev_to_periodic_list=0x000075bd
--define_symbol gap_add_whitelist=0x000075d5
--define_symbol gap_aes_encrypt=0x000075e9
--define_symbol gap_clear_white_lists=0x00007611
--define_symbol gap_clr_adv_set=0x00007621
--define_symbol gap_clr_periodic_adv_list=0x00007631
--define_symbol gap_create_connection_cancel=0x0000765d
--define_symbol gap_disconnect=0x0000766d
--define_symbol gap_disconnect_all=0x00007699
--define_symbol gap_ext_create_connection=0x00007741
--define_symbol gap_get_connection_parameter_range=0x00007805
--define_symbol gap_le_read_channel_map=0x0000783d
--define_symbol gap_periodic_adv_create_sync=0x000078b1
--define_symbol gap_periodic_adv_create_sync_cancel=0x000078d5
--define_symbol gap_periodic_adv_term_sync=0x000078e5
--define_symbol gap_read_periodic_adv_list_size=0x00007979
--define_symbol gap_read_phy=0x00007989
--define_symbol gap_read_remote_info=0x0000799d
--define_symbol gap_read_remote_used_features=0x000079b1
--define_symbol gap_read_rssi=0x000079c5
--define_symbol gap_remove_whitelist=0x000079d9
--define_symbol gap_rmv_adv_set=0x00007a65
--define_symbol gap_rmv_dev_from_periodic_list=0x00007a79
--define_symbol gap_rx_test_v2=0x00007a91
--define_symbol gap_set_adv_set_random_addr=0x00007ad1
--define_symbol gap_set_connection_parameter_range=0x00007b21
--define_symbol gap_set_data_length=0x00007b3d
--define_symbol gap_set_def_phy=0x00007b59
--define_symbol gap_set_ext_adv_data=0x00007b71
--define_symbol gap_set_ext_adv_enable=0x00007b89
--define_symbol gap_set_ext_adv_para=0x00007c05
--define_symbol gap_set_ext_scan_enable=0x00007ce5
--define_symbol gap_set_ext_scan_para=0x00007cfd
--define_symbol gap_set_ext_scan_response_data=0x00007da5
--define_symbol gap_set_host_channel_classification=0x00007dbd
--define_symbol gap_set_periodic_adv_data=0x00007dd1
--define_symbol gap_set_periodic_adv_enable=0x00007e45
--define_symbol gap_set_periodic_adv_para=0x00007e59
--define_symbol gap_set_phy=0x00007e71
--define_symbol gap_set_random_device_address=0x00007e8d
--define_symbol gap_start_ccm=0x00007ef1
--define_symbol gap_test_end=0x00007f25
--define_symbol gap_tx_test_v2=0x00007f35
--define_symbol gap_update_connection_parameters=0x00007f4d
--define_symbol gap_vendor_tx_continuous_wave=0x00007f71
--define_symbol gatt_client_cancel_write=0x00008499
--define_symbol gatt_client_discover_characteristic_descriptors=0x000084bf
--define_symbol gatt_client_discover_characteristics_for_handle_range_by_uuid128=0x000084ff
--define_symbol gatt_client_discover_characteristics_for_handle_range_by_uuid16=0x0000854f
--define_symbol gatt_client_discover_characteristics_for_service=0x0000859f
--define_symbol gatt_client_discover_primary_services=0x000085d5
--define_symbol gatt_client_discover_primary_services_by_uuid128=0x00008607
--define_symbol gatt_client_discover_primary_services_by_uuid16=0x0000864b
--define_symbol gatt_client_execute_write=0x00008687
--define_symbol gatt_client_find_included_services_for_service=0x000086ad
--define_symbol gatt_client_get_mtu=0x000086db
--define_symbol gatt_client_is_ready=0x0000877d
--define_symbol gatt_client_listen_for_characteristic_value_updates=0x00008793
--define_symbol gatt_client_prepare_write=0x000087b5
--define_symbol gatt_client_read_characteristic_descriptor_using_descriptor_handle=0x000087f1
--define_symbol gatt_client_read_long_characteristic_descriptor_using_descriptor_handle=0x0000881b
--define_symbol gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset=0x00008821
--define_symbol gatt_client_read_long_value_of_characteristic_using_value_handle=0x0000884f
--define_symbol gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset=0x00008855
--define_symbol gatt_client_read_multiple_characteristic_values=0x00008883
--define_symbol gatt_client_read_value_of_characteristic_using_value_handle=0x000088b3
--define_symbol gatt_client_read_value_of_characteristics_by_uuid128=0x000088e1
--define_symbol gatt_client_read_value_of_characteristics_by_uuid16=0x0000892d
--define_symbol gatt_client_register_handler=0x00008979
--define_symbol gatt_client_reliable_write_long_value_of_characteristic=0x00008985
--define_symbol gatt_client_signed_write_without_response=0x00008db5
--define_symbol gatt_client_write_characteristic_descriptor_using_descriptor_handle=0x00008e79
--define_symbol gatt_client_write_client_characteristic_configuration=0x00008eb3
--define_symbol gatt_client_write_long_characteristic_descriptor_using_descriptor_handle=0x00008f05
--define_symbol gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset=0x00008f15
--define_symbol gatt_client_write_long_value_of_characteristic=0x00008f51
--define_symbol gatt_client_write_long_value_of_characteristic_with_offset=0x00008f61
--define_symbol gatt_client_write_value_of_characteristic=0x00008f9d
--define_symbol gatt_client_write_value_of_characteristic_without_response=0x00008fd3
--define_symbol hci_add_event_handler=0x0000a4b9
--define_symbol hci_power_control=0x0000ac45
--define_symbol hci_register_acl_packet_handler=0x0000adf9
--define_symbol kv_commit=0x0000b469
--define_symbol kv_get=0x0000b4c1
--define_symbol kv_init=0x0000b4d9
--define_symbol kv_put=0x0000b541
--define_symbol kv_remove=0x0000b5b9
--define_symbol kv_remove_all=0x0000b5f5
--define_symbol kv_value_modified=0x0000b639
--define_symbol kv_visit=0x0000b63d
--define_symbol l2cap_can_send_fixed_channel_packet_now=0x0000b6fd
--define_symbol l2cap_can_send_packet_now=0x0000b701
--define_symbol l2cap_create_channel=0x0000b8b9
--define_symbol l2cap_disconnect=0x0000b9f1
--define_symbol l2cap_get_remote_mtu_for_local_cid=0x0000bc65
--define_symbol l2cap_init=0x0000c0e5
--define_symbol l2cap_le_send_flow_control_credit=0x0000c12d
--define_symbol l2cap_max_le_mtu=0x0000c3b1
--define_symbol l2cap_max_mtu=0x0000c3b5
--define_symbol l2cap_next_local_cid=0x0000c3b9
--define_symbol l2cap_next_sig_id=0x0000c3c9
--define_symbol l2cap_register_fixed_channel=0x0000c461
--define_symbol l2cap_register_packet_handler=0x0000c47d
--define_symbol l2cap_register_service=0x0000c489
--define_symbol l2cap_request_can_send_fix_channel_now_event=0x0000c571
--define_symbol l2cap_request_can_send_now_event=0x0000c595
--define_symbol l2cap_request_connection_parameter_update=0x0000c5af
--define_symbol l2cap_require_security_level_2_for_outgoing_sdp=0x0000c5e1
--define_symbol l2cap_send=0x0000c975
--define_symbol l2cap_send_connectionless=0x0000c9ed
--define_symbol l2cap_send_connectionless3=0x0000ca7d
--define_symbol l2cap_send_echo_request=0x0000cb15
--define_symbol l2cap_send_signaling_le=0x0000cb79
--define_symbol l2cap_unregister_service=0x0000cbd1
--define_symbol ll_free=0x0000cd9f
--define_symbol ll_hint_on_ce_len=0x0000cda9
--define_symbol ll_legacy_adv_set_interval=0x0000cde9
--define_symbol ll_malloc=0x0000cdf9
--define_symbol ll_query_timing_info=0x0000cf31
--define_symbol ll_scan_set_fixed_channel=0x0000cfd5
--define_symbol ll_set_adv_access_address=0x0000d0e9
--define_symbol ll_set_adv_coded_scheme=0x0000d0f5
--define_symbol ll_set_conn_coded_scheme=0x0000d125
--define_symbol ll_set_conn_latency=0x0000d151
--define_symbol ll_set_conn_tx_power=0x0000d181
--define_symbol ll_set_def_antenna=0x0000d1c9
--define_symbol ll_set_initiating_coded_scheme=0x0000d1e5
--define_symbol ll_set_max_conn_number=0x0000d1f1
--define_symbol nibble_for_char=0x0001ce29
--define_symbol platform_32k_rc_auto_tune=0x0001cec5
--define_symbol platform_32k_rc_tune=0x0001cf41
--define_symbol platform_calibrate_32k=0x0001cf55
--define_symbol platform_config=0x0001cf59
--define_symbol platform_get_heap_status=0x0001d011
--define_symbol platform_get_us_time=0x0001d029
--define_symbol platform_get_version=0x0001d02d
--define_symbol platform_hrng=0x0001d035
--define_symbol platform_install_isr_stack=0x0001d03d
--define_symbol platform_patch_rf_init_data=0x0001d049
--define_symbol platform_printf=0x0001d055
--define_symbol platform_raise_assertion=0x0001d069
--define_symbol platform_rand=0x0001d07d
--define_symbol platform_read_info=0x0001d081
--define_symbol platform_read_persistent_reg=0x0001d09d
--define_symbol platform_reset=0x0001d0ad
--define_symbol platform_set_evt_callback=0x0001d0e1
--define_symbol platform_set_irq_callback=0x0001d0f5
--define_symbol platform_set_rf_clk_source=0x0001d12d
--define_symbol platform_set_rf_init_data=0x0001d139
--define_symbol platform_set_rf_power_mapping=0x0001d145
--define_symbol platform_shutdown=0x0001d151
--define_symbol platform_switch_app=0x0001d155
--define_symbol platform_trace_raw=0x0001d181
--define_symbol platform_write_persistent_reg=0x0001d195
--define_symbol printf_hexdump=0x0001d1a5
--define_symbol pvPortMalloc=0x0001dc75
--define_symbol pvTaskIncrementMutexHeldCount=0x0001dd5d
--define_symbol pvTimerGetTimerID=0x0001dd75
--define_symbol pxPortInitialiseStack=0x0001dda1
--define_symbol reverse_128=0x0001df49
--define_symbol reverse_24=0x0001df4f
--define_symbol reverse_48=0x0001df55
--define_symbol reverse_56=0x0001df5b
--define_symbol reverse_64=0x0001df61
--define_symbol reverse_bd_addr=0x0001df67
--define_symbol reverse_bytes=0x0001df6d
--define_symbol sm_add_event_handler=0x0001e0d9
--define_symbol sm_address_resolution_lookup=0x0001e205
--define_symbol sm_authenticated=0x0001e2df
--define_symbol sm_authorization_decline=0x0001e2ed
--define_symbol sm_authorization_grant=0x0001e30d
--define_symbol sm_authorization_state=0x0001e32d
--define_symbol sm_bonding_decline=0x0001e349
--define_symbol sm_config=0x0001e721
--define_symbol sm_encryption_key_size=0x0001e82d
--define_symbol sm_just_works_confirm=0x0001efe5
--define_symbol sm_le_device_key=0x0001f23d
--define_symbol sm_passkey_input=0x0001f2d5
--define_symbol sm_private_random_address_generation_get=0x0001f661
--define_symbol sm_private_random_address_generation_get_mode=0x0001f669
--define_symbol sm_private_random_address_generation_set_mode=0x0001f675
--define_symbol sm_private_random_address_generation_set_update_period=0x0001f69d
--define_symbol sm_register_oob_data_callback=0x0001f6d5
--define_symbol sm_request_pairing=0x0001f6e1
--define_symbol sm_send_security_request=0x00020131
--define_symbol sm_set_accepted_stk_generation_methods=0x00020159
--define_symbol sm_set_authentication_requirements=0x00020165
--define_symbol sm_set_encryption_key_size_range=0x00020171
--define_symbol sscanf_bd_addr=0x00020461
--define_symbol sysSetPublicDeviceAddr=0x0002078d
--define_symbol uuid128_to_str=0x00020d31
--define_symbol uuid_add_bluetooth_prefix=0x00020d89
--define_symbol uuid_has_bluetooth_prefix=0x00020da9
--define_symbol uxQueueMessagesWaiting=0x00020ded
--define_symbol uxQueueMessagesWaitingFromISR=0x00020e15
--define_symbol uxQueueSpacesAvailable=0x00020e31
--define_symbol uxTaskGetStackHighWaterMark=0x00020e5d
--define_symbol vPortEnterCritical=0x00020f81
--define_symbol vPortExitCritical=0x00020fc1
--define_symbol vPortFree=0x00020fed
--define_symbol vPortSuppressTicksAndSleep=0x00021081
--define_symbol vPortValidateInterruptPriority=0x00021159
--define_symbol vQueueDelete=0x000211ad
--define_symbol vQueueWaitForMessageRestricted=0x000211d9
--define_symbol vTaskDelay=0x00021221
--define_symbol vTaskInternalSetTimeOutState=0x0002126d
--define_symbol vTaskMissedYield=0x0002127d
--define_symbol vTaskPlaceOnEventList=0x00021289
--define_symbol vTaskPlaceOnEventListRestricted=0x000212c1
--define_symbol vTaskPriorityDisinheritAfterTimeout=0x00021301
--define_symbol vTaskStartScheduler=0x000213ad
--define_symbol vTaskStepTick=0x0002143d
--define_symbol vTaskSuspendAll=0x0002146d
--define_symbol vTaskSwitchContext=0x0002147d
--define_symbol xPortStartScheduler=0x00021525
--define_symbol xQueueAddToSet=0x000215e1
--define_symbol xQueueCreateCountingSemaphore=0x00021605
--define_symbol xQueueCreateCountingSemaphoreStatic=0x00021641
--define_symbol xQueueCreateMutex=0x00021685
--define_symbol xQueueCreateMutexStatic=0x0002169b
--define_symbol xQueueCreateSet=0x000216b5
--define_symbol xQueueGenericCreate=0x000216bd
--define_symbol xQueueGenericCreateStatic=0x00021709
--define_symbol xQueueGenericReset=0x00021771
--define_symbol xQueueGenericSend=0x000217fd
--define_symbol xQueueGenericSendFromISR=0x00021969
--define_symbol xQueueGiveFromISR=0x00021a29
--define_symbol xQueueGiveMutexRecursive=0x00021acd
--define_symbol xQueueIsQueueEmptyFromISR=0x00021b0d
--define_symbol xQueueIsQueueFullFromISR=0x00021b31
--define_symbol xQueuePeek=0x00021b59
--define_symbol xQueuePeekFromISR=0x00021c81
--define_symbol xQueueReceive=0x00021ced
--define_symbol xQueueReceiveFromISR=0x00021e19
--define_symbol xQueueRemoveFromSet=0x00021ead
--define_symbol xQueueSelectFromSet=0x00021ecf
--define_symbol xQueueSelectFromSetFromISR=0x00021ee1
--define_symbol xQueueSemaphoreTake=0x00021ef5
--define_symbol xQueueTakeMutexRecursive=0x00022061
--define_symbol xTaskCheckForTimeOut=0x000220a5
--define_symbol xTaskCreate=0x00022115
--define_symbol xTaskCreateStatic=0x00022171
--define_symbol xTaskGetCurrentTaskHandle=0x000221e1
--define_symbol xTaskGetSchedulerState=0x000221ed
--define_symbol xTaskGetTickCount=0x00022209
--define_symbol xTaskGetTickCountFromISR=0x00022215
--define_symbol xTaskIncrementTick=0x00022225
--define_symbol xTaskPriorityDisinherit=0x000222f1
--define_symbol xTaskPriorityInherit=0x00022385
--define_symbol xTaskRemoveFromEventList=0x00022419
--define_symbol xTaskResumeAll=0x00022499
--define_symbol xTimerCreate=0x00022561
--define_symbol xTimerCreateStatic=0x00022595
--define_symbol xTimerCreateTimerTask=0x000225cd
--define_symbol xTimerGenericCommand=0x00022639
--define_symbol xTimerGetExpiryTime=0x000226a9
