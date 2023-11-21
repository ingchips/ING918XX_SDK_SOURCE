case 1001 :{
#pragma pack (push, 1)
struct _param {} *_param = (struct _param *)call->body;
#pragma pack (pop)
platform_reset();}break;
case 1002 :{
#pragma pack (push, 1)
struct _param {const uint32_t app_addr;} *_param = (struct _param *)call->body;
#pragma pack (pop)
platform_switch_app(_param->app_addr);}break;
case 1003 :{
#pragma pack (push, 1)
struct _param {const uint8_t value;} *_param = (struct _param *)call->body;
#pragma pack (pop)
platform_write_persistent_reg(_param->value);}break;
case 1004 :{
#pragma pack (push, 1)
struct _param {} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = platform_read_persistent_reg();send_ret_response(1004,sizeof(_ret),&_ret);}break;
case 1005 :{
#pragma pack (push, 1)
struct _param {const platform_cfg_item_t item;const uint32_t flag;} *_param = (struct _param *)call->body;
#pragma pack (pop)
platform_config(_param->item,_param->flag);}break;
case 1006 :{
#pragma pack (push, 1)
struct _param {const platform_info_item_t item;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint32_t _ret = platform_read_info(_param->item);send_ret_response(1006,sizeof(_ret),&_ret);}break;
case 1007 :{
#pragma pack (push, 1)
struct _param {} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint32_t _ret = platform_calibrate_rt_clk();send_ret_response(1007,sizeof(_ret),&_ret);}break;
case 1008 :{
#pragma pack (push, 1)
struct _param {uint16_t value;} *_param = (struct _param *)call->body;
#pragma pack (pop)
platform_rt_rc_tune(_param->value);}break;
case 1009 :{
#pragma pack (push, 1)
struct _param {} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint16_t _ret = platform_rt_rc_auto_tune();send_ret_response(1009,sizeof(_ret),&_ret);}break;
case 1010 :{
#pragma pack (push, 1)
struct _param {} *_param = (struct _param *)call->body;
#pragma pack (pop)
int _ret = platform_rand();send_ret_response(1010,sizeof(_ret),&_ret);}break;
case 1011 :{
#pragma pack (push, 1)
struct _param {} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint64_t _ret = platform_get_us_time();send_ret_response(1011,sizeof(_ret),&_ret);}break;
case 1012 :{
#pragma pack (push, 1)
struct _param {const uint8_t source;} *_param = (struct _param *)call->body;
#pragma pack (pop)
platform_set_rf_clk_source(_param->source);}break;
case 1013 :{
#pragma pack (push, 1)
struct _param {platform_task_id_t id;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uintptr_t _ret = platform_get_task_handle(_param->id);send_ret_response(1013,sizeof(_ret),&_ret);}break;
case 1014 :{
#pragma pack (push, 1)
struct _param {const uint8_t * address;} *_param = (struct _param *)call->body;
#pragma pack (pop)
gap_set_random_device_address((const uint8_t *)sys_mem_map(_param->address));}break;
case 1015 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_disconnect(_param->handle);send_ret_response(1015,sizeof(_ret),&_ret);}break;
case 1016 :{
#pragma pack (push, 1)
struct _param {} *_param = (struct _param *)call->body;
#pragma pack (pop)
gap_disconnect_all();}break;
case 1017 :{
#pragma pack (push, 1)
struct _param {const uint8_t * address;bd_addr_type_t addtype;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_add_whitelist((const uint8_t *)sys_mem_map(_param->address),_param->addtype);send_ret_response(1017,sizeof(_ret),&_ret);}break;
case 1018 :{
#pragma pack (push, 1)
struct _param {const uint8_t * address;bd_addr_type_t addtype;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_remove_whitelist((const uint8_t *)sys_mem_map(_param->address),_param->addtype);send_ret_response(1018,sizeof(_ret),&_ret);}break;
case 1019 :{
#pragma pack (push, 1)
struct _param {} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_clear_white_lists();send_ret_response(1019,sizeof(_ret),&_ret);}break;
case 1020 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_read_rssi(_param->handle);send_ret_response(1020,sizeof(_ret),&_ret);}break;
case 1021 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_read_remote_used_features(_param->handle);send_ret_response(1021,sizeof(_ret),&_ret);}break;
case 1022 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_read_remote_version(_param->handle);send_ret_response(1022,sizeof(_ret),&_ret);}break;
case 1023 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_le_read_channel_map(_param->handle);send_ret_response(1023,sizeof(_ret),&_ret);}break;
case 1024 :{
#pragma pack (push, 1)
struct _param {const uint16_t con_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_read_phy(_param->con_handle);send_ret_response(1024,sizeof(_ret),&_ret);}break;
case 1025 :{
#pragma pack (push, 1)
struct _param {const uint8_t all_phys;const phy_bittypes_t tx_phys;const phy_bittypes_t rx_phys;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_def_phy(_param->all_phys,_param->tx_phys,_param->rx_phys);send_ret_response(1025,sizeof(_ret),&_ret);}break;
case 1026 :{
#pragma pack (push, 1)
struct _param {const uint16_t con_handle;const uint8_t all_phys;const phy_bittypes_t tx_phys;const phy_bittypes_t rx_phys;const phy_option_t phy_opt;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_phy(_param->con_handle,_param->all_phys,_param->tx_phys,_param->rx_phys,_param->phy_opt);send_ret_response(1026,sizeof(_ret),&_ret);}break;
case 1027 :{
#pragma pack (push, 1)
struct _param {const uint8_t adv_handle;const uint8_t * random_addr;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_adv_set_random_addr(_param->adv_handle,(const uint8_t *)sys_mem_map(_param->random_addr));send_ret_response(1027,sizeof(_ret),&_ret);}break;
case 1028 :{
#pragma pack (push, 1)
struct _param {const bd_addr_type_t own_addr_type;const scan_filter_policy_t filter;const uint8_t config_num;const scan_phy_config_t * configs;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_ext_scan_para(_param->own_addr_type,_param->filter,_param->config_num,(const scan_phy_config_t *)sys_mem_map(_param->configs));send_ret_response(1028,sizeof(_ret),&_ret);}break;
case 1029 :{
#pragma pack (push, 1)
struct _param {const uint8_t adv_handle;const uint16_t length;const uint8_t * data;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_ext_scan_response_data(_param->adv_handle,_param->length,(const uint8_t *)sys_mem_map(_param->data));send_ret_response(1029,sizeof(_ret),&_ret);}break;
case 1030 :{
#pragma pack (push, 1)
struct _param {const uint8_t enable;const uint8_t filter;const uint16_t duration;const uint16_t period;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_ext_scan_enable(_param->enable,_param->filter,_param->duration,_param->period);send_ret_response(1030,sizeof(_ret),&_ret);}break;
case 1031 :{
#pragma pack (push, 1)
struct _param {const uint8_t enable;const uint8_t set_number;const ext_adv_set_en_t * adv_sets;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_ext_adv_enable(_param->enable,_param->set_number,(const ext_adv_set_en_t *)sys_mem_map(_param->adv_sets));send_ret_response(1031,sizeof(_ret),&_ret);}break;
case 1032 :{
#pragma pack (push, 1)
struct _param {const uint8_t adv_handle;const adv_event_properties_t properties;const uint32_t interval_min;const uint32_t interval_max;const adv_channel_bits_t primary_adv_channel_map;const bd_addr_type_t own_addr_type;const bd_addr_type_t peer_addr_type;const uint8_t * peer_addr;const adv_filter_policy_t adv_filter_policy;const int8_t tx_power;const phy_type_t primary_adv_phy;const uint8_t secondary_adv_max_skip;const phy_type_t secondary_adv_phy;const uint8_t sid;const uint8_t scan_req_notification_enable;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_ext_adv_para(_param->adv_handle,_param->properties,_param->interval_min,_param->interval_max,_param->primary_adv_channel_map,_param->own_addr_type,_param->peer_addr_type,(const uint8_t *)sys_mem_map(_param->peer_addr),_param->adv_filter_policy,_param->tx_power,_param->primary_adv_phy,_param->secondary_adv_max_skip,_param->secondary_adv_phy,_param->sid,_param->scan_req_notification_enable);send_ret_response(1032,sizeof(_ret),&_ret);}break;
case 1033 :{
#pragma pack (push, 1)
struct _param {const uint8_t adv_handle;uint16_t length;const uint8_t * data;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_ext_adv_data(_param->adv_handle,_param->length,(const uint8_t *)sys_mem_map(_param->data));send_ret_response(1033,sizeof(_ret),&_ret);}break;
case 1034 :{
#pragma pack (push, 1)
struct _param {const uint8_t adv_handle;uint16_t length;const uint8_t * data;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_periodic_adv_data(_param->adv_handle,_param->length,(const uint8_t *)sys_mem_map(_param->data));send_ret_response(1034,sizeof(_ret),&_ret);}break;
case 1035 :{
#pragma pack (push, 1)
struct _param {const uint8_t enable;const uint8_t adv_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_periodic_adv_enable(_param->enable,_param->adv_handle);send_ret_response(1035,sizeof(_ret),&_ret);}break;
case 1036 :{
#pragma pack (push, 1)
struct _param {const uint8_t adv_handle;const uint16_t interval_min;const uint16_t interval_max;const periodic_adv_properties_t properties;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_periodic_adv_para(_param->adv_handle,_param->interval_min,_param->interval_max,_param->properties);send_ret_response(1036,sizeof(_ret),&_ret);}break;
case 1037 :{
#pragma pack (push, 1)
struct _param {} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_clr_adv_set();send_ret_response(1037,sizeof(_ret),&_ret);}break;
case 1038 :{
#pragma pack (push, 1)
struct _param {const uint8_t adv_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_rmv_adv_set(_param->adv_handle);send_ret_response(1038,sizeof(_ret),&_ret);}break;
case 1039 :{
#pragma pack (push, 1)
struct _param {const periodic_adv_filter_policy_t filter_policy;const uint8_t adv_sid;const bd_addr_type_t addr_type;const uint8_t * addr;const uint16_t skip;const uint16_t sync_timeout;const uint8_t sync_cte_type;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_periodic_adv_create_sync(_param->filter_policy,_param->adv_sid,_param->addr_type,(const uint8_t *)sys_mem_map(_param->addr),_param->skip,_param->sync_timeout,_param->sync_cte_type);send_ret_response(1039,sizeof(_ret),&_ret);}break;
case 1040 :{
#pragma pack (push, 1)
struct _param {} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_periodic_adv_create_sync_cancel();send_ret_response(1040,sizeof(_ret),&_ret);}break;
case 1041 :{
#pragma pack (push, 1)
struct _param {const uint16_t sync_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_periodic_adv_term_sync(_param->sync_handle);send_ret_response(1041,sizeof(_ret),&_ret);}break;
case 1042 :{
#pragma pack (push, 1)
struct _param {const uint8_t addr_type;const uint8_t * addr;const uint8_t sid;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_add_dev_to_periodic_list(_param->addr_type,(const uint8_t *)sys_mem_map(_param->addr),_param->sid);send_ret_response(1042,sizeof(_ret),&_ret);}break;
case 1043 :{
#pragma pack (push, 1)
struct _param {const uint8_t addr_type;const uint8_t * addr;const uint8_t sid;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_rmv_dev_from_periodic_list(_param->addr_type,(const uint8_t *)sys_mem_map(_param->addr),_param->sid);send_ret_response(1043,sizeof(_ret),&_ret);}break;
case 1044 :{
#pragma pack (push, 1)
struct _param {} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_clr_periodic_adv_list();send_ret_response(1044,sizeof(_ret),&_ret);}break;
case 1045 :{
#pragma pack (push, 1)
struct _param {} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_read_periodic_adv_list_size();send_ret_response(1045,sizeof(_ret),&_ret);}break;
case 1046 :{
#pragma pack (push, 1)
struct _param {const initiating_filter_policy_t filter_policy;const bd_addr_type_t own_addr_type;const bd_addr_type_t peer_addr_type;const uint8_t * peer_addr;const uint8_t initiating_phy_num;const initiating_phy_config_t * phy_configs;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_ext_create_connection(_param->filter_policy,_param->own_addr_type,_param->peer_addr_type,(const uint8_t *)sys_mem_map(_param->peer_addr),_param->initiating_phy_num,(const initiating_phy_config_t *)sys_mem_map(_param->phy_configs));send_ret_response(1046,sizeof(_ret),&_ret);}break;
case 1047 :{
#pragma pack (push, 1)
struct _param {} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_create_connection_cancel();send_ret_response(1047,sizeof(_ret),&_ret);}break;
case 1048 :{
#pragma pack (push, 1)
struct _param {uint16_t connection_handle;uint16_t tx_octets;uint16_t tx_time;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_data_length(_param->connection_handle,_param->tx_octets,_param->tx_time);send_ret_response(1048,sizeof(_ret),&_ret);}break;
case 1049 :{
#pragma pack (push, 1)
struct _param {const uint8_t adv_handle;const uint8_t cte_len;const cte_type_t cte_type;const uint8_t cte_count;const uint8_t switching_pattern_len;const uint8_t * antenna_ids;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_connectionless_cte_tx_param(_param->adv_handle,_param->cte_len,_param->cte_type,_param->cte_count,_param->switching_pattern_len,(const uint8_t *)sys_mem_map(_param->antenna_ids));send_ret_response(1049,sizeof(_ret),&_ret);}break;
case 1050 :{
#pragma pack (push, 1)
struct _param {const uint8_t adv_handle;const uint8_t cte_enable;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_connectionless_cte_tx_enable(_param->adv_handle,_param->cte_enable);send_ret_response(1050,sizeof(_ret),&_ret);}break;
case 1051 :{
#pragma pack (push, 1)
struct _param {const uint16_t sync_handle;const uint8_t sampling_enable;const cte_slot_duration_type_t slot_durations;const uint8_t max_sampled_ctes;const uint8_t switching_pattern_len;const uint8_t * antenna_ids;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_connectionless_iq_sampling_enable(_param->sync_handle,_param->sampling_enable,_param->slot_durations,_param->max_sampled_ctes,_param->switching_pattern_len,(const uint8_t *)sys_mem_map(_param->antenna_ids));send_ret_response(1051,sizeof(_ret),&_ret);}break;
case 1052 :{
#pragma pack (push, 1)
struct _param {const hci_con_handle_t conn_handle;const uint8_t sampling_enable;const cte_slot_duration_type_t slot_durations;const uint8_t switching_pattern_len;const uint8_t * antenna_ids;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_connection_cte_rx_param(_param->conn_handle,_param->sampling_enable,_param->slot_durations,_param->switching_pattern_len,(const uint8_t *)sys_mem_map(_param->antenna_ids));send_ret_response(1052,sizeof(_ret),&_ret);}break;
case 1053 :{
#pragma pack (push, 1)
struct _param {const hci_con_handle_t conn_handle;const uint8_t cte_types;const uint8_t switching_pattern_len;const uint8_t * antenna_ids;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_connection_cte_tx_param(_param->conn_handle,_param->cte_types,_param->switching_pattern_len,(const uint8_t *)sys_mem_map(_param->antenna_ids));send_ret_response(1053,sizeof(_ret),&_ret);}break;
case 1054 :{
#pragma pack (push, 1)
struct _param {const hci_con_handle_t conn_handle;const uint8_t enable;const uint16_t requested_cte_interval;const uint8_t requested_cte_length;const cte_type_t requested_cte_type;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_connection_cte_request_enable(_param->conn_handle,_param->enable,_param->requested_cte_interval,_param->requested_cte_length,_param->requested_cte_type);send_ret_response(1054,sizeof(_ret),&_ret);}break;
case 1055 :{
#pragma pack (push, 1)
struct _param {const hci_con_handle_t conn_handle;const uint8_t enable;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_connection_cte_response_enable(_param->conn_handle,_param->enable);send_ret_response(1055,sizeof(_ret),&_ret);}break;
case 1056 :{
#pragma pack (push, 1)
struct _param {} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_read_antenna_info();send_ret_response(1056,sizeof(_ret),&_ret);}break;
case 1057 :{
#pragma pack (push, 1)
struct _param {const uint16_t sync_handle;const uint8_t enable;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_periodic_adv_rx_enable(_param->sync_handle,_param->enable);send_ret_response(1057,sizeof(_ret),&_ret);}break;
case 1058 :{
#pragma pack (push, 1)
struct _param {const hci_con_handle_t conn_handle;const uint16_t service_data;const uint16_t sync_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_periodic_adv_sync_transfer(_param->conn_handle,_param->service_data,_param->sync_handle);send_ret_response(1058,sizeof(_ret),&_ret);}break;
case 1059 :{
#pragma pack (push, 1)
struct _param {const hci_con_handle_t conn_handle;const uint16_t service_data;const uint8_t adv_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_periodic_adv_set_info_transfer(_param->conn_handle,_param->service_data,_param->adv_handle);send_ret_response(1059,sizeof(_ret),&_ret);}break;
case 1060 :{
#pragma pack (push, 1)
struct _param {const hci_con_handle_t conn_handle;const periodic_adv_sync_transfer_mode_t mode;const uint16_t skip;const uint16_t sync_timeout;const uint8_t cte_excl_types;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_periodic_adv_sync_transfer_param(_param->conn_handle,_param->mode,_param->skip,_param->sync_timeout,_param->cte_excl_types);send_ret_response(1060,sizeof(_ret),&_ret);}break;
case 1061 :{
#pragma pack (push, 1)
struct _param {const periodic_adv_sync_transfer_mode_t mode;const uint16_t skip;const uint16_t sync_timeout;const uint8_t cte_excl_types;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_default_periodic_adv_sync_transfer_param(_param->mode,_param->skip,_param->sync_timeout,_param->cte_excl_types);send_ret_response(1061,sizeof(_ret),&_ret);}break;
case 1062 :{
#pragma pack (push, 1)
struct _param {const uint32_t channel_low;const uint8_t channel_high;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_host_channel_classification(_param->channel_low,_param->channel_high);send_ret_response(1062,sizeof(_ret),&_ret);}break;
case 1063 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;uint16_t conn_interval_min;uint16_t conn_interval_max;uint16_t conn_latency;uint16_t supervision_timeout;uint16_t min_ce_len;uint16_t max_ce_len;} *_param = (struct _param *)call->body;
#pragma pack (pop)
int _ret = gap_update_connection_parameters(_param->con_handle,_param->conn_interval_min,_param->conn_interval_max,_param->conn_latency,_param->supervision_timeout,_param->min_ce_len,_param->max_ce_len);send_ret_response(1063,sizeof(_ret),&_ret);}break;
case 1064 :{
#pragma pack (push, 1)
struct _param {le_connection_parameter_range_t * range;} *_param = (struct _param *)call->body;
#pragma pack (pop)
gap_get_connection_parameter_range((le_connection_parameter_range_t *)sys_mem_map(_param->range));}break;
case 1065 :{
#pragma pack (push, 1)
struct _param {le_connection_parameter_range_t * range;} *_param = (struct _param *)call->body;
#pragma pack (pop)
gap_set_connection_parameter_range((le_connection_parameter_range_t *)sys_mem_map(_param->range));}break;
case 1066 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;unified_phy_type_t phy;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_read_local_tx_power_level(_param->con_handle,_param->phy);send_ret_response(1066,sizeof(_ret),&_ret);}break;
case 1067 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;unified_phy_type_t phy;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_read_remote_tx_power_level(_param->con_handle,_param->phy);send_ret_response(1067,sizeof(_ret),&_ret);}break;
case 1068 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;uint8_t high_threshold;uint8_t high_hysteresis;uint8_t low_threshold;uint8_t low_hysteresis;uint8_t min_time_spent;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_path_loss_reporting_param(_param->con_handle,_param->high_threshold,_param->high_hysteresis,_param->low_threshold,_param->low_hysteresis,_param->min_time_spent);send_ret_response(1068,sizeof(_ret),&_ret);}break;
case 1069 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;uint8_t enable;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_path_loss_reporting_enable(_param->con_handle,_param->enable);send_ret_response(1069,sizeof(_ret),&_ret);}break;
case 1070 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;uint8_t local_enable;uint8_t remote_enable;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_tx_power_reporting_enable(_param->con_handle,_param->local_enable,_param->remote_enable);send_ret_response(1070,sizeof(_ret),&_ret);}break;
case 1071 :{
#pragma pack (push, 1)
struct _param {uint16_t subrate_min;uint16_t subrate_max;uint16_t max_latency;uint16_t continuation_number;uint16_t supervision_timeout;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_set_default_subrate(_param->subrate_min,_param->subrate_max,_param->max_latency,_param->continuation_number,_param->supervision_timeout);send_ret_response(1071,sizeof(_ret),&_ret);}break;
case 1072 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;uint16_t subrate_min;uint16_t subrate_max;uint16_t max_latency;uint16_t continuation_number;uint16_t supervision_timeout;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_subrate_request(_param->con_handle,_param->subrate_min,_param->subrate_max,_param->max_latency,_param->continuation_number,_param->supervision_timeout);send_ret_response(1072,sizeof(_ret),&_ret);}break;
case 1073 :{
#pragma pack (push, 1)
struct _param {const uint8_t * key;const uint8_t * plaintext;gap_hci_cmd_complete_cb_t cb;void * user_data;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_aes_encrypt((const uint8_t *)sys_mem_map(_param->key),(const uint8_t *)sys_mem_map(_param->plaintext),_param->cb,(void *)sys_mem_map(_param->user_data));send_ret_response(1073,sizeof(_ret),&_ret);}break;
case 1074 :{
#pragma pack (push, 1)
struct _param {uint8_t rx_channel;uint8_t phy;uint8_t modulation_index;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_rx_test_v2(_param->rx_channel,_param->phy,_param->modulation_index);send_ret_response(1074,sizeof(_ret),&_ret);}break;
case 1075 :{
#pragma pack (push, 1)
struct _param {uint8_t rx_channel;uint8_t phy;uint8_t modulation_index;uint8_t expected_cte_length;uint8_t expected_cte_type;uint8_t slot_durations;uint8_t switching_pattern_length;uint8_t * antenna_ids;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_rx_test_v3(_param->rx_channel,_param->phy,_param->modulation_index,_param->expected_cte_length,_param->expected_cte_type,_param->slot_durations,_param->switching_pattern_length,(uint8_t *)sys_mem_map(_param->antenna_ids));send_ret_response(1075,sizeof(_ret),&_ret);}break;
case 1076 :{
#pragma pack (push, 1)
struct _param {uint8_t tx_channel;uint8_t test_data_length;uint8_t packet_payload;uint8_t phy;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_tx_test_v2(_param->tx_channel,_param->test_data_length,_param->packet_payload,_param->phy);send_ret_response(1076,sizeof(_ret),&_ret);}break;
case 1077 :{
#pragma pack (push, 1)
struct _param {uint8_t tx_channel;uint8_t test_data_length;uint8_t packet_payload;uint8_t phy;uint8_t cte_length;uint8_t cte_type;uint8_t switching_pattern_length;uint8_t * antenna_ids;int8_t tx_power_level;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_tx_test_v4(_param->tx_channel,_param->test_data_length,_param->packet_payload,_param->phy,_param->cte_length,_param->cte_type,_param->switching_pattern_length,(uint8_t *)sys_mem_map(_param->antenna_ids),_param->tx_power_level);send_ret_response(1077,sizeof(_ret),&_ret);}break;
case 1078 :{
#pragma pack (push, 1)
struct _param {} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_test_end();send_ret_response(1078,sizeof(_ret),&_ret);}break;
case 1079 :{
#pragma pack (push, 1)
struct _param {uint8_t enable;uint8_t power_level_index;uint16_t freq;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gap_vendor_tx_continuous_wave(_param->enable,_param->power_level_index,_param->freq);send_ret_response(1079,sizeof(_ret),&_ret);}break;
case 1080 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;uint16_t attribute_handle;const uint8_t * value;uint16_t value_len;} *_param = (struct _param *)call->body;
#pragma pack (pop)
int _ret = att_server_deferred_read_response(_param->con_handle,_param->attribute_handle,(const uint8_t *)sys_mem_map(_param->value),_param->value_len);send_ret_response(1080,sizeof(_ret),&_ret);}break;
case 1081 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;uint16_t attribute_handle;uint8_t * value;uint16_t value_len;} *_param = (struct _param *)call->body;
#pragma pack (pop)
int _ret = att_server_notify(_param->con_handle,_param->attribute_handle,(uint8_t *)sys_mem_map(_param->value),_param->value_len);send_ret_response(1081,sizeof(_ret),&_ret);}break;
case 1082 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;uint16_t attribute_handle;uint8_t * value;uint16_t value_len;} *_param = (struct _param *)call->body;
#pragma pack (pop)
int _ret = att_server_indicate(_param->con_handle,_param->attribute_handle,(uint8_t *)sys_mem_map(_param->value),_param->value_len);send_ret_response(1082,sizeof(_ret),&_ret);}break;
case 1083 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint16_t _ret = att_server_get_mtu(_param->con_handle);send_ret_response(1083,sizeof(_ret),&_ret);}break;
case 1084 :{
#pragma pack (push, 1)
struct _param {user_packet_handler_t callback;hci_con_handle_t con_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_discover_primary_services(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle);send_ret_response(1084,sizeof(_ret),&_ret);}break;
case 1085 :{
#pragma pack (push, 1)
struct _param {user_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t uuid16;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_discover_primary_services_by_uuid16(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->uuid16);send_ret_response(1085,sizeof(_ret),&_ret);}break;
case 1086 :{
#pragma pack (push, 1)
struct _param {user_packet_handler_t callback;hci_con_handle_t con_handle;const uint8_t * uuid128;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_discover_primary_services_by_uuid128(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,(const uint8_t *)sys_mem_map(_param->uuid128));send_ret_response(1086,sizeof(_ret),&_ret);}break;
case 1087 :{
#pragma pack (push, 1)
struct _param {user_packet_handler_t callback;hci_con_handle_t con_handle;const uint16_t start_group_handle;const uint16_t end_group_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_find_included_services_for_service(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->start_group_handle,_param->end_group_handle);send_ret_response(1087,sizeof(_ret),&_ret);}break;
case 1088 :{
#pragma pack (push, 1)
struct _param {user_packet_handler_t callback;hci_con_handle_t con_handle;const uint16_t start_group_handle;const uint16_t end_group_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_discover_characteristics_for_service(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->start_group_handle,_param->end_group_handle);send_ret_response(1088,sizeof(_ret),&_ret);}break;
case 1089 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;const hci_con_handle_t con_handle;const uint16_t start_handle;const uint16_t end_handle;const uint16_t uuid16;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_discover_characteristics_for_handle_range_by_uuid16(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->start_handle,_param->end_handle,_param->uuid16);send_ret_response(1089,sizeof(_ret),&_ret);}break;
case 1090 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;const hci_con_handle_t con_handle;const uint16_t start_handle;const uint16_t end_handle;const uint8_t * uuid128;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_discover_characteristics_for_handle_range_by_uuid128(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->start_handle,_param->end_handle,(const uint8_t *)sys_mem_map(_param->uuid128));send_ret_response(1090,sizeof(_ret),&_ret);}break;
case 1091 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;hci_con_handle_t con_handle;gatt_client_characteristic_t * characteristic;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_discover_characteristic_descriptors(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,(gatt_client_characteristic_t *)sys_mem_map(_param->characteristic));send_ret_response(1091,sizeof(_ret),&_ret);}break;
case 1092 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t characteristic_value_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_read_value_of_characteristic_using_value_handle(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->characteristic_value_handle);send_ret_response(1092,sizeof(_ret),&_ret);}break;
case 1093 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t start_handle;uint16_t end_handle;uint16_t uuid16;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_read_value_of_characteristics_by_uuid16(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->start_handle,_param->end_handle,_param->uuid16);send_ret_response(1093,sizeof(_ret),&_ret);}break;
case 1094 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t start_handle;uint16_t end_handle;uint8_t * uuid128;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_read_value_of_characteristics_by_uuid128(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->start_handle,_param->end_handle,(uint8_t *)sys_mem_map(_param->uuid128));send_ret_response(1094,sizeof(_ret),&_ret);}break;
case 1095 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t characteristic_value_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_read_long_value_of_characteristic_using_value_handle(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->characteristic_value_handle);send_ret_response(1095,sizeof(_ret),&_ret);}break;
case 1096 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t characteristic_value_handle;uint16_t offset;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->characteristic_value_handle,_param->offset);send_ret_response(1096,sizeof(_ret),&_ret);}break;
case 1097 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;hci_con_handle_t con_handle;int num_value_handles;uint16_t * value_handles;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_read_multiple_characteristic_values(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->num_value_handles,(uint16_t *)sys_mem_map(_param->value_handles));send_ret_response(1097,sizeof(_ret),&_ret);}break;
case 1098 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;uint16_t characteristic_value_handle;uint16_t length;const uint8_t * data;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_write_value_of_characteristic_without_response(_param->con_handle,_param->characteristic_value_handle,_param->length,(const uint8_t *)sys_mem_map(_param->data));send_ret_response(1098,sizeof(_ret),&_ret);}break;
case 1099 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t handle;uint16_t message_len;const uint8_t * message;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_signed_write_without_response(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->handle,_param->message_len,(const uint8_t *)sys_mem_map(_param->message));send_ret_response(1099,sizeof(_ret),&_ret);}break;
case 1100 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t characteristic_value_handle;uint16_t length;const uint8_t * data;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_write_value_of_characteristic(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->characteristic_value_handle,_param->length,(const uint8_t *)sys_mem_map(_param->data));send_ret_response(1100,sizeof(_ret),&_ret);}break;
case 1101 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t characteristic_value_handle;uint16_t length;const uint8_t * data;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_write_long_value_of_characteristic(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->characteristic_value_handle,_param->length,(const uint8_t *)sys_mem_map(_param->data));send_ret_response(1101,sizeof(_ret),&_ret);}break;
case 1102 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t characteristic_value_handle;uint16_t offset;uint16_t length;const uint8_t * data;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_write_long_value_of_characteristic_with_offset(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->characteristic_value_handle,_param->offset,_param->length,(const uint8_t *)sys_mem_map(_param->data));send_ret_response(1102,sizeof(_ret),&_ret);}break;
case 1103 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t characteristic_value_handle;uint16_t length;const uint8_t * data;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_reliable_write_long_value_of_characteristic(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->characteristic_value_handle,_param->length,(const uint8_t *)sys_mem_map(_param->data));send_ret_response(1103,sizeof(_ret),&_ret);}break;
case 1104 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t descriptor_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_read_characteristic_descriptor_using_descriptor_handle(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->descriptor_handle);send_ret_response(1104,sizeof(_ret),&_ret);}break;
case 1105 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t descriptor_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_read_long_characteristic_descriptor_using_descriptor_handle(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->descriptor_handle);send_ret_response(1105,sizeof(_ret),&_ret);}break;
case 1106 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t descriptor_handle;uint16_t offset;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->descriptor_handle,_param->offset);send_ret_response(1106,sizeof(_ret),&_ret);}break;
case 1107 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t descriptor_handle;uint16_t length;uint8_t * data;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_write_characteristic_descriptor_using_descriptor_handle(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->descriptor_handle,_param->length,(uint8_t *)sys_mem_map(_param->data));send_ret_response(1107,sizeof(_ret),&_ret);}break;
case 1108 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t descriptor_handle;uint16_t length;uint8_t * data;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_write_long_characteristic_descriptor_using_descriptor_handle(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->descriptor_handle,_param->length,(uint8_t *)sys_mem_map(_param->data));send_ret_response(1108,sizeof(_ret),&_ret);}break;
case 1109 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t descriptor_handle;uint16_t offset;uint16_t length;uint8_t * data;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->descriptor_handle,_param->offset,_param->length,(uint8_t *)sys_mem_map(_param->data));send_ret_response(1109,sizeof(_ret),&_ret);}break;
case 1110 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;hci_con_handle_t con_handle;gatt_client_characteristic_t * characteristic;uint16_t configuration;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_write_client_characteristic_configuration(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,(gatt_client_characteristic_t *)sys_mem_map(_param->characteristic),_param->configuration);send_ret_response(1110,sizeof(_ret),&_ret);}break;
case 1111 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t attribute_handle;uint16_t offset;uint16_t length;uint8_t * data;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_prepare_write(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle,_param->attribute_handle,_param->offset,_param->length,(uint8_t *)sys_mem_map(_param->data));send_ret_response(1111,sizeof(_ret),&_ret);}break;
case 1112 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;hci_con_handle_t con_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_execute_write(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle);send_ret_response(1112,sizeof(_ret),&_ret);}break;
case 1113 :{
#pragma pack (push, 1)
struct _param {btstack_packet_handler_t callback;hci_con_handle_t con_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
uint8_t _ret = gatt_client_cancel_write(sys_fun_map(_param->callback, _param->con_handle),_param->con_handle);send_ret_response(1113,sizeof(_ret),&_ret);}break;
case 1114 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
int _ret = gatt_client_is_ready(_param->con_handle);send_ret_response(1114,sizeof(_ret),&_ret);}break;
case 1115 :{
#pragma pack (push, 1)
struct _param {gatt_client_notification_t * notification;btstack_packet_handler_t packet_handler;hci_con_handle_t con_handle;uint16_t value_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
gatt_client_listen_for_characteristic_value_updates((gatt_client_notification_t *)sys_mem_map(_param->notification),sys_fun_map(_param->packet_handler, _param->con_handle),_param->con_handle,_param->value_handle);}break;
case 1116 :{
#pragma pack (push, 1)
struct _param {uint8_t enable;io_capability_t io_capability;int request_security;const sm_persistent_t * persistent;} *_param = (struct _param *)call->body;
#pragma pack (pop)
sm_config(_param->enable,_param->io_capability,_param->request_security,(const sm_persistent_t *)sys_mem_map(_param->persistent));}break;
case 1117 :{
#pragma pack (push, 1)
struct _param {gap_random_address_type_t random_address_type;} *_param = (struct _param *)call->body;
#pragma pack (pop)
sm_private_random_address_generation_set_mode(_param->random_address_type);}break;
case 1118 :{
#pragma pack (push, 1)
struct _param {} *_param = (struct _param *)call->body;
#pragma pack (pop)
gap_random_address_type_t _ret = sm_private_random_address_generation_get_mode();send_ret_response(1118,sizeof(_ret),&_ret);}break;
case 1119 :{
#pragma pack (push, 1)
struct _param {int period_ms;} *_param = (struct _param *)call->body;
#pragma pack (pop)
sm_private_random_address_generation_set_update_period(_param->period_ms);}break;
case 1120 :{
#pragma pack (push, 1)
struct _param {uint8_t accepted_stk_generation_methods;} *_param = (struct _param *)call->body;
#pragma pack (pop)
sm_set_accepted_stk_generation_methods(_param->accepted_stk_generation_methods);}break;
case 1121 :{
#pragma pack (push, 1)
struct _param {uint8_t min_size;uint8_t max_size;} *_param = (struct _param *)call->body;
#pragma pack (pop)
sm_set_encryption_key_size_range(_param->min_size,_param->max_size);}break;
case 1122 :{
#pragma pack (push, 1)
struct _param {uint8_t auth_req;} *_param = (struct _param *)call->body;
#pragma pack (pop)
sm_set_authentication_requirements(_param->auth_req);}break;
case 1123 :{
#pragma pack (push, 1)
struct _param {uint8_t addr_type;bd_addr_t addr;} *_param = (struct _param *)call->body;
#pragma pack (pop)
int _ret = sm_address_resolution_lookup(_param->addr_type,_param->addr);send_ret_response(1123,sizeof(_ret),&_ret);}break;
case 1124 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;io_capability_t io_capability;uint8_t auth_req;} *_param = (struct _param *)call->body;
#pragma pack (pop)
sm_config_conn(_param->con_handle,_param->io_capability,_param->auth_req);}break;
case 1125 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
sm_bonding_decline(_param->con_handle);}break;
case 1126 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
sm_just_works_confirm(_param->con_handle);}break;
case 1127 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;uint32_t passkey;} *_param = (struct _param *)call->body;
#pragma pack (pop)
sm_passkey_input(_param->con_handle,_param->passkey);}break;
case 1128 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
sm_send_security_request(_param->con_handle);}break;
case 1129 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
int _ret = sm_encryption_key_size(_param->con_handle);send_ret_response(1129,sizeof(_ret),&_ret);}break;
case 1130 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
int _ret = sm_authenticated(_param->con_handle);send_ret_response(1130,sizeof(_ret),&_ret);}break;
case 1131 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
authorization_state_t _ret = sm_authorization_state(_param->con_handle);send_ret_response(1131,sizeof(_ret),&_ret);}break;
case 1132 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
sm_request_pairing(_param->con_handle);}break;
case 1133 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
sm_authorization_decline(_param->con_handle);}break;
case 1134 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
sm_authorization_grant(_param->con_handle);}break;
case 1135 :{
#pragma pack (push, 1)
struct _param {hci_con_handle_t con_handle;} *_param = (struct _param *)call->body;
#pragma pack (pop)
int _ret = sm_le_device_key(_param->con_handle);send_ret_response(1135,sizeof(_ret),&_ret);}break;
