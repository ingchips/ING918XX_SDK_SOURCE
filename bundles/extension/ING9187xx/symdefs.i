--define_symbol att_dispatch_client_can_send_now=0x000059c1
--define_symbol att_dispatch_client_request_can_send_now_event=0x000059c7
--define_symbol att_dispatch_register_client=0x000059cd
--define_symbol att_dispatch_register_server=0x000059e1
--define_symbol att_dispatch_server_can_send_now=0x000059f5
--define_symbol att_dispatch_server_request_can_send_now_event=0x000059fb
--define_symbol att_emit_general_event=0x00005aad
--define_symbol att_server_can_send_packet_now=0x000061c1
--define_symbol att_server_deferred_read_response=0x000061c5
--define_symbol att_server_get_mtu=0x000061dd
--define_symbol att_server_indicate=0x00006255
--define_symbol att_server_init=0x000062d9
--define_symbol att_server_notify=0x00006315
--define_symbol att_server_register_packet_handler=0x0000642d
--define_symbol att_server_request_can_send_now_event=0x00006439
--define_symbol att_set_db=0x00006455
--define_symbol att_set_read_callback=0x00006469
--define_symbol att_set_write_callback=0x00006475
--define_symbol bd_addr_cmp=0x000065e5
--define_symbol bd_addr_copy=0x000065eb
--define_symbol bd_addr_to_str=0x000065f5
--define_symbol big_endian_read_16=0x0000662d
--define_symbol big_endian_read_32=0x00006635
--define_symbol big_endian_store_16=0x00006649
--define_symbol big_endian_store_32=0x00006655
--define_symbol btstack_config=0x000067a9
--define_symbol btstack_memory_pool_create=0x000068e7
--define_symbol btstack_memory_pool_free=0x00006911
--define_symbol btstack_memory_pool_get=0x00006971
--define_symbol btstack_push_user_msg=0x0000698d
--define_symbol char_for_nibble=0x00006c55
--define_symbol eTaskConfirmSleepModeStatus=0x00006f01
--define_symbol gap_add_dev_to_periodic_list=0x00007521
--define_symbol gap_add_whitelist=0x00007539
--define_symbol gap_aes_encrypt=0x0000754d
--define_symbol gap_clear_white_lists=0x00007591
--define_symbol gap_clr_adv_set=0x000075a1
--define_symbol gap_clr_periodic_adv_list=0x000075b1
--define_symbol gap_create_connection_cancel=0x000075c1
--define_symbol gap_disconnect=0x000075d1
--define_symbol gap_disconnect_all=0x000075fd
--define_symbol gap_ext_create_connection=0x0000763d
--define_symbol gap_get_connection_parameter_range=0x0000772d
--define_symbol gap_le_read_channel_map=0x00007769
--define_symbol gap_periodic_adv_create_sync=0x000077dd
--define_symbol gap_periodic_adv_create_sync_cancel=0x00007801
--define_symbol gap_periodic_adv_term_sync=0x00007811
--define_symbol gap_read_periodic_adv_list_size=0x000078a5
--define_symbol gap_read_phy=0x000078b5
--define_symbol gap_read_remote_info=0x000078c9
--define_symbol gap_read_remote_used_features=0x000078dd
--define_symbol gap_read_rssi=0x000078f1
--define_symbol gap_remove_whitelist=0x00007905
--define_symbol gap_rmv_adv_set=0x00007989
--define_symbol gap_rmv_dev_from_periodic_list=0x0000799d
--define_symbol gap_rx_test_v2=0x000079b5
--define_symbol gap_set_adv_set_random_addr=0x000079f5
--define_symbol gap_set_connection_parameter_range=0x00007a41
--define_symbol gap_set_data_length=0x00007a59
--define_symbol gap_set_def_phy=0x00007a75
--define_symbol gap_set_ext_adv_data=0x00007a8d
--define_symbol gap_set_ext_adv_enable=0x00007aa5
--define_symbol gap_set_ext_adv_para=0x00007b21
--define_symbol gap_set_ext_scan_enable=0x00007c01
--define_symbol gap_set_ext_scan_para=0x00007c19
--define_symbol gap_set_ext_scan_response_data=0x00007cc1
--define_symbol gap_set_host_channel_classification=0x00007cd9
--define_symbol gap_set_periodic_adv_data=0x00007ced
--define_symbol gap_set_periodic_adv_enable=0x00007d61
--define_symbol gap_set_periodic_adv_para=0x00007d75
--define_symbol gap_set_phy=0x00007d8d
--define_symbol gap_set_random_device_address=0x00007da9
--define_symbol gap_start_ccm=0x00007dd9
--define_symbol gap_test_end=0x00007e0d
--define_symbol gap_tx_test_v2=0x00007e1d
--define_symbol gap_tx_test_v4=0x00007e35
--define_symbol gap_update_connection_parameters=0x00007e5d
--define_symbol gap_vendor_tx_continuous_wave=0x00007e81
--define_symbol gatt_client_cancel_write=0x000083a9
--define_symbol gatt_client_discover_characteristic_descriptors=0x000083cf
--define_symbol gatt_client_discover_characteristics_for_handle_range_by_uuid128=0x0000840f
--define_symbol gatt_client_discover_characteristics_for_handle_range_by_uuid16=0x0000845f
--define_symbol gatt_client_discover_characteristics_for_service=0x000084af
--define_symbol gatt_client_discover_primary_services=0x000084e5
--define_symbol gatt_client_discover_primary_services_by_uuid128=0x00008517
--define_symbol gatt_client_discover_primary_services_by_uuid16=0x0000855b
--define_symbol gatt_client_execute_write=0x00008597
--define_symbol gatt_client_find_included_services_for_service=0x000085bd
--define_symbol gatt_client_get_mtu=0x000085eb
--define_symbol gatt_client_is_ready=0x0000868d
--define_symbol gatt_client_listen_for_characteristic_value_updates=0x000086a3
--define_symbol gatt_client_prepare_write=0x000086c5
--define_symbol gatt_client_read_characteristic_descriptor_using_descriptor_handle=0x00008701
--define_symbol gatt_client_read_long_characteristic_descriptor_using_descriptor_handle=0x0000872b
--define_symbol gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset=0x00008731
--define_symbol gatt_client_read_long_value_of_characteristic_using_value_handle=0x0000875f
--define_symbol gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset=0x00008765
--define_symbol gatt_client_read_multiple_characteristic_values=0x00008793
--define_symbol gatt_client_read_value_of_characteristic_using_value_handle=0x000087c3
--define_symbol gatt_client_read_value_of_characteristics_by_uuid128=0x000087f1
--define_symbol gatt_client_read_value_of_characteristics_by_uuid16=0x0000883d
--define_symbol gatt_client_register_handler=0x00008889
--define_symbol gatt_client_reliable_write_long_value_of_characteristic=0x00008895
--define_symbol gatt_client_signed_write_without_response=0x00008cc5
--define_symbol gatt_client_write_characteristic_descriptor_using_descriptor_handle=0x00008d89
--define_symbol gatt_client_write_client_characteristic_configuration=0x00008dc3
--define_symbol gatt_client_write_long_characteristic_descriptor_using_descriptor_handle=0x00008e15
--define_symbol gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset=0x00008e25
--define_symbol gatt_client_write_long_value_of_characteristic=0x00008e61
--define_symbol gatt_client_write_long_value_of_characteristic_with_offset=0x00008e71
--define_symbol gatt_client_write_value_of_characteristic=0x00008ead
--define_symbol gatt_client_write_value_of_characteristic_without_response=0x00008ee3
--define_symbol hci_add_event_handler=0x0000a409
--define_symbol hci_power_control=0x0000ac01
--define_symbol hci_register_acl_packet_handler=0x0000adb5
--define_symbol kv_commit=0x0000b329
--define_symbol kv_get=0x0000b381
--define_symbol kv_init=0x0000b399
--define_symbol kv_put=0x0000b401
--define_symbol kv_remove=0x0000b479
--define_symbol kv_remove_all=0x0000b4b5
--define_symbol kv_value_modified=0x0000b4f9
--define_symbol kv_visit=0x0000b4fd
--define_symbol l2cap_can_send_fixed_channel_packet_now=0x0000b5bd
--define_symbol l2cap_can_send_packet_now=0x0000b5c1
--define_symbol l2cap_create_channel=0x0000b779
--define_symbol l2cap_disconnect=0x0000b8b1
--define_symbol l2cap_get_remote_mtu_for_local_cid=0x0000bb25
--define_symbol l2cap_init=0x0000bf1d
--define_symbol l2cap_le_send_flow_control_credit=0x0000bf5d
--define_symbol l2cap_max_le_mtu=0x0000c1e1
--define_symbol l2cap_max_mtu=0x0000c1e5
--define_symbol l2cap_next_local_cid=0x0000c1e9
--define_symbol l2cap_next_sig_id=0x0000c1f9
--define_symbol l2cap_register_fixed_channel=0x0000c291
--define_symbol l2cap_register_packet_handler=0x0000c2ad
--define_symbol l2cap_register_service=0x0000c2b9
--define_symbol l2cap_request_can_send_fix_channel_now_event=0x0000c39d
--define_symbol l2cap_request_can_send_now_event=0x0000c3c1
--define_symbol l2cap_request_connection_parameter_update=0x0000c3db
--define_symbol l2cap_send=0x0000c77d
--define_symbol l2cap_send_connectionless=0x0000c7f5
--define_symbol l2cap_send_connectionless3=0x0000c885
--define_symbol l2cap_send_echo_request=0x0000c91d
--define_symbol l2cap_send_signaling_le=0x0000c981
--define_symbol l2cap_unregister_service=0x0000c9d9
--define_symbol le_device_db_add=0x0000ca31
--define_symbol le_device_db_find=0x0000cb05
--define_symbol le_device_db_from_key=0x0000cb31
--define_symbol le_device_db_iter_cur=0x0000cb39
--define_symbol le_device_db_iter_cur_key=0x0000cb3d
--define_symbol le_device_db_iter_init=0x0000cb41
--define_symbol le_device_db_iter_next=0x0000cb49
--define_symbol le_device_db_remove_key=0x0000cb6f
--define_symbol ll_ackable_packet_alloc=0x0000cb9b
--define_symbol ll_ackable_packet_get_status=0x0000cca3
--define_symbol ll_ackable_packet_run=0x0000cd15
--define_symbol ll_ackable_packet_set_tx_data=0x0000cdbd
--define_symbol ll_free=0x0000cdd7
--define_symbol ll_hint_on_ce_len=0x0000cde1
--define_symbol ll_legacy_adv_set_interval=0x0000ce19
--define_symbol ll_malloc=0x0000ce29
--define_symbol ll_query_timing_info=0x0000cf61
--define_symbol ll_raw_packet_alloc=0x0000cfad
--define_symbol ll_raw_packet_free=0x0000d081
--define_symbol ll_raw_packet_get_bare_rx_data=0x0000d0ab
--define_symbol ll_raw_packet_get_rx_data=0x0000d171
--define_symbol ll_raw_packet_recv=0x0000d225
--define_symbol ll_raw_packet_send=0x0000d2e1
--define_symbol ll_raw_packet_set_bare_data=0x0000d3c9
--define_symbol ll_raw_packet_set_bare_mode=0x0000d407
--define_symbol ll_raw_packet_set_param=0x0000d50d
--define_symbol ll_raw_packet_set_tx_data=0x0000d56b
--define_symbol ll_scan_set_fixed_channel=0x0000d629
--define_symbol ll_set_adv_access_address=0x0000d73d
--define_symbol ll_set_adv_coded_scheme=0x0000d749
--define_symbol ll_set_conn_coded_scheme=0x0000d779
--define_symbol ll_set_conn_interval_unit=0x0000d7a5
--define_symbol ll_set_conn_latency=0x0000d7b1
--define_symbol ll_set_conn_tx_power=0x0000d7e1
--define_symbol ll_set_def_antenna=0x0000d829
--define_symbol ll_set_initiating_coded_scheme=0x0000d845
--define_symbol ll_set_max_conn_number=0x0000d851
--define_symbol nibble_for_char=0x0001d7c1
--define_symbol platform_32k_rc_auto_tune=0x0001d85d
--define_symbol platform_32k_rc_tune=0x0001d8d9
--define_symbol platform_calibrate_32k=0x0001d8ed
--define_symbol platform_config=0x0001d8f1
--define_symbol platform_get_heap_status=0x0001d9c5
--define_symbol platform_get_task_handle=0x0001d9dd
--define_symbol platform_get_us_time=0x0001d9fd
--define_symbol platform_get_version=0x0001da01
--define_symbol platform_hrng=0x0001da09
--define_symbol platform_install_isr_stack=0x0001da11
--define_symbol platform_patch_rf_init_data=0x0001da1d
--define_symbol platform_printf=0x0001da29
--define_symbol platform_raise_assertion=0x0001da3d
--define_symbol platform_rand=0x0001da51
--define_symbol platform_read_info=0x0001da55
--define_symbol platform_read_persistent_reg=0x0001da71
--define_symbol platform_reset=0x0001da81
--define_symbol platform_set_evt_callback=0x0001dab5
--define_symbol platform_set_irq_callback=0x0001dac9
--define_symbol platform_set_rf_clk_source=0x0001db01
--define_symbol platform_set_rf_init_data=0x0001db0d
--define_symbol platform_set_rf_power_mapping=0x0001db19
--define_symbol platform_set_timer=0x0001db25
--define_symbol platform_shutdown=0x0001db29
--define_symbol platform_switch_app=0x0001db2d
--define_symbol platform_trace_raw=0x0001db59
--define_symbol platform_write_persistent_reg=0x0001db71
--define_symbol printf_hexdump=0x0001db81
--define_symbol pvPortMalloc=0x0001e689
--define_symbol pvTaskIncrementMutexHeldCount=0x0001e771
--define_symbol pvTimerGetTimerID=0x0001e789
--define_symbol pxPortInitialiseStack=0x0001e7b5
--define_symbol reverse_128=0x0001e95d
--define_symbol reverse_24=0x0001e963
--define_symbol reverse_48=0x0001e969
--define_symbol reverse_56=0x0001e96f
--define_symbol reverse_64=0x0001e975
--define_symbol reverse_bd_addr=0x0001e97b
--define_symbol reverse_bytes=0x0001e981
--define_symbol sm_add_event_handler=0x0001eb0d
--define_symbol sm_address_resolution_lookup=0x0001ec65
--define_symbol sm_authenticated=0x0001efbd
--define_symbol sm_authorization_decline=0x0001efcb
--define_symbol sm_authorization_grant=0x0001efeb
--define_symbol sm_authorization_state=0x0001f00b
--define_symbol sm_bonding_decline=0x0001f025
--define_symbol sm_config=0x0001f445
--define_symbol sm_config_conn=0x0001f45d
--define_symbol sm_encryption_key_size=0x0001f613
--define_symbol sm_just_works_confirm=0x0001fb1d
--define_symbol sm_le_device_key=0x0001fe59
--define_symbol sm_passkey_input=0x0001feef
--define_symbol sm_private_random_address_generation_get=0x0002029d
--define_symbol sm_private_random_address_generation_get_mode=0x000202a5
--define_symbol sm_private_random_address_generation_set_mode=0x000202b1
--define_symbol sm_private_random_address_generation_set_update_period=0x000202d9
--define_symbol sm_register_oob_data_callback=0x00020415
--define_symbol sm_request_pairing=0x00020421
--define_symbol sm_send_security_request=0x00020e3f
--define_symbol sm_set_accepted_stk_generation_methods=0x00020e65
--define_symbol sm_set_authentication_requirements=0x00020e71
--define_symbol sm_set_encryption_key_size_range=0x00020e7d
--define_symbol sscanf_bd_addr=0x0002124d
--define_symbol sysSetPublicDeviceAddr=0x000215b5
--define_symbol uuid128_to_str=0x00021b99
--define_symbol uuid_add_bluetooth_prefix=0x00021bf1
--define_symbol uuid_has_bluetooth_prefix=0x00021c11
--define_symbol uxListRemove=0x00021c2d
--define_symbol uxQueueMessagesWaiting=0x00021c55
--define_symbol uxQueueMessagesWaitingFromISR=0x00021c7d
--define_symbol uxQueueSpacesAvailable=0x00021c99
--define_symbol uxTaskGetStackHighWaterMark=0x00021cc5
--define_symbol uxTaskPriorityGet=0x00021ce5
--define_symbol uxTaskPriorityGetFromISR=0x00021d01
--define_symbol vListInitialise=0x00021db3
--define_symbol vListInitialiseItem=0x00021dc9
--define_symbol vListInsert=0x00021dcf
--define_symbol vListInsertEnd=0x00021dff
--define_symbol vPortEndScheduler=0x00021e19
--define_symbol vPortEnterCritical=0x00021e41
--define_symbol vPortExitCritical=0x00021e85
--define_symbol vPortFree=0x00021eb5
--define_symbol vPortSuppressTicksAndSleep=0x00021f49
--define_symbol vPortValidateInterruptPriority=0x00022051
--define_symbol vQueueDelete=0x000220a9
--define_symbol vQueueWaitForMessageRestricted=0x000220d5
--define_symbol vTaskDelay=0x0002211d
--define_symbol vTaskInternalSetTimeOutState=0x00022169
--define_symbol vTaskMissedYield=0x00022179
--define_symbol vTaskPlaceOnEventList=0x00022185
--define_symbol vTaskPlaceOnEventListRestricted=0x000221bd
--define_symbol vTaskPriorityDisinheritAfterTimeout=0x000221fd
--define_symbol vTaskPrioritySet=0x000222a9
--define_symbol vTaskResume=0x00022371
--define_symbol vTaskStartScheduler=0x000223f5
--define_symbol vTaskStepTick=0x00022485
--define_symbol vTaskSuspend=0x000224b5
--define_symbol vTaskSuspendAll=0x00022571
--define_symbol vTaskSwitchContext=0x00022581
--define_symbol xPortStartScheduler=0x00022629
--define_symbol xQueueAddToSet=0x000226e9
--define_symbol xQueueCreateCountingSemaphore=0x0002270d
--define_symbol xQueueCreateCountingSemaphoreStatic=0x00022749
--define_symbol xQueueCreateMutex=0x0002278d
--define_symbol xQueueCreateMutexStatic=0x000227a3
--define_symbol xQueueCreateSet=0x000227bd
--define_symbol xQueueGenericCreate=0x000227c5
--define_symbol xQueueGenericCreateStatic=0x00022811
--define_symbol xQueueGenericReset=0x00022879
--define_symbol xQueueGenericSend=0x00022905
--define_symbol xQueueGenericSendFromISR=0x00022a71
--define_symbol xQueueGiveFromISR=0x00022b31
--define_symbol xQueueGiveMutexRecursive=0x00022bd5
--define_symbol xQueueIsQueueEmptyFromISR=0x00022c15
--define_symbol xQueueIsQueueFullFromISR=0x00022c39
--define_symbol xQueuePeek=0x00022c61
--define_symbol xQueuePeekFromISR=0x00022d89
--define_symbol xQueueReceive=0x00022df5
--define_symbol xQueueReceiveFromISR=0x00022f21
--define_symbol xQueueRemoveFromSet=0x00022fb5
--define_symbol xQueueSelectFromSet=0x00022fd7
--define_symbol xQueueSelectFromSetFromISR=0x00022fe9
--define_symbol xQueueSemaphoreTake=0x00022ffd
--define_symbol xQueueTakeMutexRecursive=0x00023169
--define_symbol xTaskCheckForTimeOut=0x000231ad
--define_symbol xTaskCreate=0x0002321d
--define_symbol xTaskCreateStatic=0x00023279
--define_symbol xTaskGetCurrentTaskHandle=0x000232e9
--define_symbol xTaskGetSchedulerState=0x000232f5
--define_symbol xTaskGetTickCount=0x00023311
--define_symbol xTaskGetTickCountFromISR=0x0002331d
--define_symbol xTaskIncrementTick=0x0002332d
--define_symbol xTaskPriorityDisinherit=0x000233f9
--define_symbol xTaskPriorityInherit=0x0002348d
--define_symbol xTaskRemoveFromEventList=0x00023521
--define_symbol xTaskResumeAll=0x000235a1
--define_symbol xTaskResumeFromISR=0x00023669
--define_symbol xTimerCreate=0x000236f5
--define_symbol xTimerCreateStatic=0x00023729
--define_symbol xTimerCreateTimerTask=0x00023761
--define_symbol xTimerGenericCommand=0x000237cd
--define_symbol xTimerGetExpiryTime=0x0002383d
--define_symbol xTimerGetTimerDaemonTaskHandle=0x0002385d
