void platform_reset(void){LOG_D("platform_reset...");;
#pragma pack (push, 1)
struct _param {
} _param = {};
#pragma pack (pop)
call_void_fun(1001,&_param,sizeof(_param));}
void platform_switch_app(const uint32_t app_addr){LOG_D("platform_switch_app...");;
#pragma pack (push, 1)
struct _param {
const uint32_t app_addr;} _param = {.app_addr = app_addr};
#pragma pack (pop)
call_void_fun(1002,&_param,sizeof(_param));}
void platform_write_persistent_reg(const uint8_t value){LOG_D("platform_write_persistent_reg...");;
#pragma pack (push, 1)
struct _param {
const uint8_t value;} _param = {.value = value};
#pragma pack (pop)
call_void_fun(1003,&_param,sizeof(_param));}
uint8_t platform_read_persistent_reg(void){LOG_D("platform_read_persistent_reg...");;
#pragma pack (push, 1)
struct _param {
} _param = {};
#pragma pack (pop)
struct node * _node = call_fun(1004,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
void platform_config(const platform_cfg_item_t item,
const uint32_t flag){LOG_D("platform_config...");;
#pragma pack (push, 1)
struct _param {
const platform_cfg_item_t item;const uint32_t flag;} _param = {.item = item,
.flag = flag};
#pragma pack (pop)
call_void_fun(1005,&_param,sizeof(_param));}
uint32_t platform_read_info(const platform_info_item_t item){LOG_D("platform_read_info...");;
#pragma pack (push, 1)
struct _param {
const platform_info_item_t item;} _param = {.item = item};
#pragma pack (pop)
struct node * _node = call_fun(1006,&_param,sizeof(_param));uint32_t _r= *(uint32_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint32_t platform_calibrate_rt_clk(void){LOG_D("platform_calibrate_rt_clk...");;
#pragma pack (push, 1)
struct _param {
} _param = {};
#pragma pack (pop)
struct node * _node = call_fun(1007,&_param,sizeof(_param));uint32_t _r= *(uint32_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
void platform_rt_rc_tune(uint16_t value){LOG_D("platform_rt_rc_tune...");;
#pragma pack (push, 1)
struct _param {
uint16_t value;} _param = {.value = value};
#pragma pack (pop)
call_void_fun(1008,&_param,sizeof(_param));}
uint16_t platform_rt_rc_auto_tune(void){LOG_D("platform_rt_rc_auto_tune...");;
#pragma pack (push, 1)
struct _param {
} _param = {};
#pragma pack (pop)
struct node * _node = call_fun(1009,&_param,sizeof(_param));uint16_t _r= *(uint16_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
int platform_rand(void){LOG_D("platform_rand...");;
#pragma pack (push, 1)
struct _param {
} _param = {};
#pragma pack (pop)
struct node * _node = call_fun(1010,&_param,sizeof(_param));int _r= *(int *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint64_t platform_get_us_time(void){LOG_D("platform_get_us_time...");;
#pragma pack (push, 1)
struct _param {
} _param = {};
#pragma pack (pop)
struct node * _node = call_fun(1011,&_param,sizeof(_param));uint64_t _r= *(uint64_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
void platform_set_rf_clk_source(const uint8_t source){LOG_D("platform_set_rf_clk_source...");;
#pragma pack (push, 1)
struct _param {
const uint8_t source;} _param = {.source = source};
#pragma pack (pop)
call_void_fun(1012,&_param,sizeof(_param));}
uintptr_t platform_get_task_handle(platform_task_id_t id){LOG_D("platform_get_task_handle...");;
#pragma pack (push, 1)
struct _param {
platform_task_id_t id;} _param = {.id = id};
#pragma pack (pop)
struct node * _node = call_fun(1013,&_param,sizeof(_param));uintptr_t _r= *(uintptr_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_random_device_address(const uint8_t * address){LOG_D("gap_set_random_device_address...");remote_mem_map(address, BD_ADDR_LEN * sizeof(* address));
#pragma pack (push, 1)
struct _param {
const uint8_t * address;} _param = {.address = address};
#pragma pack (pop)
struct node * _node = call_fun(1014,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_disconnect(hci_con_handle_t handle){LOG_D("gap_disconnect...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t handle;} _param = {.handle = handle};
#pragma pack (pop)
struct node * _node = call_fun(1015,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
void gap_disconnect_all(void){LOG_D("gap_disconnect_all...");;
#pragma pack (push, 1)
struct _param {
} _param = {};
#pragma pack (pop)
call_void_fun(1016,&_param,sizeof(_param));}
uint8_t gap_add_whitelist(const uint8_t * address,
bd_addr_type_t addtype){LOG_D("gap_add_whitelist...");remote_mem_map(address, BD_ADDR_LEN * sizeof(* address));
#pragma pack (push, 1)
struct _param {
const uint8_t * address;bd_addr_type_t addtype;} _param = {.address = address,
.addtype = addtype};
#pragma pack (pop)
struct node * _node = call_fun(1017,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_remove_whitelist(const uint8_t * address,
bd_addr_type_t addtype){LOG_D("gap_remove_whitelist...");remote_mem_map(address, BD_ADDR_LEN * sizeof(* address));
#pragma pack (push, 1)
struct _param {
const uint8_t * address;bd_addr_type_t addtype;} _param = {.address = address,
.addtype = addtype};
#pragma pack (pop)
struct node * _node = call_fun(1018,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_clear_white_lists(void){LOG_D("gap_clear_white_lists...");;
#pragma pack (push, 1)
struct _param {
} _param = {};
#pragma pack (pop)
struct node * _node = call_fun(1019,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_read_rssi(hci_con_handle_t handle){LOG_D("gap_read_rssi...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t handle;} _param = {.handle = handle};
#pragma pack (pop)
struct node * _node = call_fun(1020,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_read_remote_used_features(hci_con_handle_t handle){LOG_D("gap_read_remote_used_features...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t handle;} _param = {.handle = handle};
#pragma pack (pop)
struct node * _node = call_fun(1021,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_read_remote_version(hci_con_handle_t handle){LOG_D("gap_read_remote_version...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t handle;} _param = {.handle = handle};
#pragma pack (pop)
struct node * _node = call_fun(1022,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_le_read_channel_map(hci_con_handle_t handle){LOG_D("gap_le_read_channel_map...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t handle;} _param = {.handle = handle};
#pragma pack (pop)
struct node * _node = call_fun(1023,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_read_phy(const uint16_t con_handle){LOG_D("gap_read_phy...");;
#pragma pack (push, 1)
struct _param {
const uint16_t con_handle;} _param = {.con_handle = con_handle};
#pragma pack (pop)
struct node * _node = call_fun(1024,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_def_phy(const uint8_t all_phys,
const phy_bittypes_t tx_phys,
const phy_bittypes_t rx_phys){LOG_D("gap_set_def_phy...");;
#pragma pack (push, 1)
struct _param {
const uint8_t all_phys;const phy_bittypes_t tx_phys;const phy_bittypes_t rx_phys;} _param = {.all_phys = all_phys,
.tx_phys = tx_phys,
.rx_phys = rx_phys};
#pragma pack (pop)
struct node * _node = call_fun(1025,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_phy(const uint16_t con_handle,
const uint8_t all_phys,
const phy_bittypes_t tx_phys,
const phy_bittypes_t rx_phys,
const phy_option_t phy_opt){LOG_D("gap_set_phy...");;
#pragma pack (push, 1)
struct _param {
const uint16_t con_handle;const uint8_t all_phys;const phy_bittypes_t tx_phys;const phy_bittypes_t rx_phys;const phy_option_t phy_opt;} _param = {.con_handle = con_handle,
.all_phys = all_phys,
.tx_phys = tx_phys,
.rx_phys = rx_phys,
.phy_opt = phy_opt};
#pragma pack (pop)
struct node * _node = call_fun(1026,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_adv_set_random_addr(const uint8_t adv_handle,
const uint8_t * random_addr){LOG_D("gap_set_adv_set_random_addr...");remote_mem_map(random_addr, BD_ADDR_LEN * sizeof(* random_addr));
#pragma pack (push, 1)
struct _param {
const uint8_t adv_handle;const uint8_t * random_addr;} _param = {.adv_handle = adv_handle,
.random_addr = random_addr};
#pragma pack (pop)
struct node * _node = call_fun(1027,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_ext_scan_para(const bd_addr_type_t own_addr_type,
const scan_filter_policy_t filter,
const uint8_t config_num,
const scan_phy_config_t * configs){LOG_D("gap_set_ext_scan_para...");remote_mem_map(configs, config_num * sizeof(* configs));
#pragma pack (push, 1)
struct _param {
const bd_addr_type_t own_addr_type;const scan_filter_policy_t filter;const uint8_t config_num;const scan_phy_config_t * configs;} _param = {.own_addr_type = own_addr_type,
.filter = filter,
.config_num = config_num,
.configs = configs};
#pragma pack (pop)
struct node * _node = call_fun(1028,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_ext_scan_response_data(const uint8_t adv_handle,
const uint16_t length,
const uint8_t * data){LOG_D("gap_set_ext_scan_response_data...");remote_mem_map(data, length * sizeof(* data));
#pragma pack (push, 1)
struct _param {
const uint8_t adv_handle;const uint16_t length;const uint8_t * data;} _param = {.adv_handle = adv_handle,
.length = length,
.data = data};
#pragma pack (pop)
struct node * _node = call_fun(1029,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_ext_scan_enable(const uint8_t enable,
const uint8_t filter,
const uint16_t duration,
const uint16_t period){LOG_D("gap_set_ext_scan_enable...");;
#pragma pack (push, 1)
struct _param {
const uint8_t enable;const uint8_t filter;const uint16_t duration;const uint16_t period;} _param = {.enable = enable,
.filter = filter,
.duration = duration,
.period = period};
#pragma pack (pop)
struct node * _node = call_fun(1030,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_ext_adv_enable(const uint8_t enable,
const uint8_t set_number,
const ext_adv_set_en_t * adv_sets){LOG_D("gap_set_ext_adv_enable...");remote_mem_map(adv_sets, set_number * sizeof(* adv_sets));
#pragma pack (push, 1)
struct _param {
const uint8_t enable;const uint8_t set_number;const ext_adv_set_en_t * adv_sets;} _param = {.enable = enable,
.set_number = set_number,
.adv_sets = adv_sets};
#pragma pack (pop)
struct node * _node = call_fun(1031,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_ext_adv_para(const uint8_t adv_handle,
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
const uint8_t scan_req_notification_enable){LOG_D("gap_set_ext_adv_para...");remote_mem_map(peer_addr, BD_ADDR_LEN * sizeof(* peer_addr));
#pragma pack (push, 1)
struct _param {
const uint8_t adv_handle;const adv_event_properties_t properties;const uint32_t interval_min;const uint32_t interval_max;const adv_channel_bits_t primary_adv_channel_map;const bd_addr_type_t own_addr_type;const bd_addr_type_t peer_addr_type;const uint8_t * peer_addr;const adv_filter_policy_t adv_filter_policy;const int8_t tx_power;const phy_type_t primary_adv_phy;const uint8_t secondary_adv_max_skip;const phy_type_t secondary_adv_phy;const uint8_t sid;const uint8_t scan_req_notification_enable;} _param = {.adv_handle = adv_handle,
.properties = properties,
.interval_min = interval_min,
.interval_max = interval_max,
.primary_adv_channel_map = primary_adv_channel_map,
.own_addr_type = own_addr_type,
.peer_addr_type = peer_addr_type,
.peer_addr = peer_addr,
.adv_filter_policy = adv_filter_policy,
.tx_power = tx_power,
.primary_adv_phy = primary_adv_phy,
.secondary_adv_max_skip = secondary_adv_max_skip,
.secondary_adv_phy = secondary_adv_phy,
.sid = sid,
.scan_req_notification_enable = scan_req_notification_enable};
#pragma pack (pop)
struct node * _node = call_fun(1032,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_ext_adv_data(const uint8_t adv_handle,
uint16_t length,
const uint8_t * data){LOG_D("gap_set_ext_adv_data...");remote_mem_map(data, length * sizeof(* data));
#pragma pack (push, 1)
struct _param {
const uint8_t adv_handle;uint16_t length;const uint8_t * data;} _param = {.adv_handle = adv_handle,
.length = length,
.data = data};
#pragma pack (pop)
struct node * _node = call_fun(1033,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_periodic_adv_data(const uint8_t adv_handle,
uint16_t length,
const uint8_t * data){LOG_D("gap_set_periodic_adv_data...");remote_mem_map(data, length * sizeof(* data));
#pragma pack (push, 1)
struct _param {
const uint8_t adv_handle;uint16_t length;const uint8_t * data;} _param = {.adv_handle = adv_handle,
.length = length,
.data = data};
#pragma pack (pop)
struct node * _node = call_fun(1034,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_periodic_adv_enable(const uint8_t enable,
const uint8_t adv_handle){LOG_D("gap_set_periodic_adv_enable...");;
#pragma pack (push, 1)
struct _param {
const uint8_t enable;const uint8_t adv_handle;} _param = {.enable = enable,
.adv_handle = adv_handle};
#pragma pack (pop)
struct node * _node = call_fun(1035,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_periodic_adv_para(const uint8_t adv_handle,
const uint16_t interval_min,
const uint16_t interval_max,
const periodic_adv_properties_t properties){LOG_D("gap_set_periodic_adv_para...");;
#pragma pack (push, 1)
struct _param {
const uint8_t adv_handle;const uint16_t interval_min;const uint16_t interval_max;const periodic_adv_properties_t properties;} _param = {.adv_handle = adv_handle,
.interval_min = interval_min,
.interval_max = interval_max,
.properties = properties};
#pragma pack (pop)
struct node * _node = call_fun(1036,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_clr_adv_set(void){LOG_D("gap_clr_adv_set...");;
#pragma pack (push, 1)
struct _param {
} _param = {};
#pragma pack (pop)
struct node * _node = call_fun(1037,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_rmv_adv_set(const uint8_t adv_handle){LOG_D("gap_rmv_adv_set...");;
#pragma pack (push, 1)
struct _param {
const uint8_t adv_handle;} _param = {.adv_handle = adv_handle};
#pragma pack (pop)
struct node * _node = call_fun(1038,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_periodic_adv_create_sync(const periodic_adv_filter_policy_t filter_policy,
const uint8_t adv_sid,
const bd_addr_type_t addr_type,
const uint8_t * addr,
const uint16_t skip,
const uint16_t sync_timeout,
const uint8_t sync_cte_type){LOG_D("gap_periodic_adv_create_sync...");remote_mem_map(addr, BD_ADDR_LEN * sizeof(* addr));
#pragma pack (push, 1)
struct _param {
const periodic_adv_filter_policy_t filter_policy;const uint8_t adv_sid;const bd_addr_type_t addr_type;const uint8_t * addr;const uint16_t skip;const uint16_t sync_timeout;const uint8_t sync_cte_type;} _param = {.filter_policy = filter_policy,
.adv_sid = adv_sid,
.addr_type = addr_type,
.addr = addr,
.skip = skip,
.sync_timeout = sync_timeout,
.sync_cte_type = sync_cte_type};
#pragma pack (pop)
struct node * _node = call_fun(1039,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_periodic_adv_create_sync_cancel(void){LOG_D("gap_periodic_adv_create_sync_cancel...");;
#pragma pack (push, 1)
struct _param {
} _param = {};
#pragma pack (pop)
struct node * _node = call_fun(1040,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_periodic_adv_term_sync(const uint16_t sync_handle){LOG_D("gap_periodic_adv_term_sync...");;
#pragma pack (push, 1)
struct _param {
const uint16_t sync_handle;} _param = {.sync_handle = sync_handle};
#pragma pack (pop)
struct node * _node = call_fun(1041,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_add_dev_to_periodic_list(const uint8_t addr_type,
const uint8_t * addr,
const uint8_t sid){LOG_D("gap_add_dev_to_periodic_list...");remote_mem_map(addr, BD_ADDR_LEN * sizeof(* addr));
#pragma pack (push, 1)
struct _param {
const uint8_t addr_type;const uint8_t * addr;const uint8_t sid;} _param = {.addr_type = addr_type,
.addr = addr,
.sid = sid};
#pragma pack (pop)
struct node * _node = call_fun(1042,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_rmv_dev_from_periodic_list(const uint8_t addr_type,
const uint8_t * addr,
const uint8_t sid){LOG_D("gap_rmv_dev_from_periodic_list...");remote_mem_map(addr, BD_ADDR_LEN * sizeof(* addr));
#pragma pack (push, 1)
struct _param {
const uint8_t addr_type;const uint8_t * addr;const uint8_t sid;} _param = {.addr_type = addr_type,
.addr = addr,
.sid = sid};
#pragma pack (pop)
struct node * _node = call_fun(1043,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_clr_periodic_adv_list(void){LOG_D("gap_clr_periodic_adv_list...");;
#pragma pack (push, 1)
struct _param {
} _param = {};
#pragma pack (pop)
struct node * _node = call_fun(1044,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_read_periodic_adv_list_size(void){LOG_D("gap_read_periodic_adv_list_size...");;
#pragma pack (push, 1)
struct _param {
} _param = {};
#pragma pack (pop)
struct node * _node = call_fun(1045,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_ext_create_connection(const initiating_filter_policy_t filter_policy,
const bd_addr_type_t own_addr_type,
const bd_addr_type_t peer_addr_type,
const uint8_t * peer_addr,
const uint8_t initiating_phy_num,
const initiating_phy_config_t * phy_configs){LOG_D("gap_ext_create_connection...");remote_mem_map(peer_addr, BD_ADDR_LEN * sizeof(* peer_addr));remote_mem_map(phy_configs, initiating_phy_num * sizeof(*phy_configs));
#pragma pack (push, 1)
struct _param {
const initiating_filter_policy_t filter_policy;const bd_addr_type_t own_addr_type;const bd_addr_type_t peer_addr_type;const uint8_t * peer_addr;const uint8_t initiating_phy_num;const initiating_phy_config_t * phy_configs;} _param = {.filter_policy = filter_policy,
.own_addr_type = own_addr_type,
.peer_addr_type = peer_addr_type,
.peer_addr = peer_addr,
.initiating_phy_num = initiating_phy_num,
.phy_configs = phy_configs};
#pragma pack (pop)
struct node * _node = call_fun(1046,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_create_connection_cancel(void){LOG_D("gap_create_connection_cancel...");;
#pragma pack (push, 1)
struct _param {
} _param = {};
#pragma pack (pop)
struct node * _node = call_fun(1047,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_data_length(uint16_t connection_handle,
uint16_t tx_octets,
uint16_t tx_time){LOG_D("gap_set_data_length...");;
#pragma pack (push, 1)
struct _param {
uint16_t connection_handle;uint16_t tx_octets;uint16_t tx_time;} _param = {.connection_handle = connection_handle,
.tx_octets = tx_octets,
.tx_time = tx_time};
#pragma pack (pop)
struct node * _node = call_fun(1048,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_connectionless_cte_tx_param(const uint8_t adv_handle,
const uint8_t cte_len,
const cte_type_t cte_type,
const uint8_t cte_count,
const uint8_t switching_pattern_len,
const uint8_t * antenna_ids){LOG_D("gap_set_connectionless_cte_tx_param...");remote_mem_map(antenna_ids, switching_pattern_len * sizeof(* antenna_ids));
#pragma pack (push, 1)
struct _param {
const uint8_t adv_handle;const uint8_t cte_len;const cte_type_t cte_type;const uint8_t cte_count;const uint8_t switching_pattern_len;const uint8_t * antenna_ids;} _param = {.adv_handle = adv_handle,
.cte_len = cte_len,
.cte_type = cte_type,
.cte_count = cte_count,
.switching_pattern_len = switching_pattern_len,
.antenna_ids = antenna_ids};
#pragma pack (pop)
struct node * _node = call_fun(1049,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_connectionless_cte_tx_enable(const uint8_t adv_handle,
const uint8_t cte_enable){LOG_D("gap_set_connectionless_cte_tx_enable...");;
#pragma pack (push, 1)
struct _param {
const uint8_t adv_handle;const uint8_t cte_enable;} _param = {.adv_handle = adv_handle,
.cte_enable = cte_enable};
#pragma pack (pop)
struct node * _node = call_fun(1050,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_connectionless_iq_sampling_enable(const uint16_t sync_handle,
const uint8_t sampling_enable,
const cte_slot_duration_type_t slot_durations,
const uint8_t max_sampled_ctes,
const uint8_t switching_pattern_len,
const uint8_t * antenna_ids){LOG_D("gap_set_connectionless_iq_sampling_enable...");remote_mem_map(antenna_ids, switching_pattern_len * sizeof(* antenna_ids));
#pragma pack (push, 1)
struct _param {
const uint16_t sync_handle;const uint8_t sampling_enable;const cte_slot_duration_type_t slot_durations;const uint8_t max_sampled_ctes;const uint8_t switching_pattern_len;const uint8_t * antenna_ids;} _param = {.sync_handle = sync_handle,
.sampling_enable = sampling_enable,
.slot_durations = slot_durations,
.max_sampled_ctes = max_sampled_ctes,
.switching_pattern_len = switching_pattern_len,
.antenna_ids = antenna_ids};
#pragma pack (pop)
struct node * _node = call_fun(1051,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_connection_cte_rx_param(const hci_con_handle_t conn_handle,
const uint8_t sampling_enable,
const cte_slot_duration_type_t slot_durations,
const uint8_t switching_pattern_len,
const uint8_t * antenna_ids){LOG_D("gap_set_connection_cte_rx_param...");remote_mem_map(antenna_ids, switching_pattern_len * sizeof(* antenna_ids));
#pragma pack (push, 1)
struct _param {
const hci_con_handle_t conn_handle;const uint8_t sampling_enable;const cte_slot_duration_type_t slot_durations;const uint8_t switching_pattern_len;const uint8_t * antenna_ids;} _param = {.conn_handle = conn_handle,
.sampling_enable = sampling_enable,
.slot_durations = slot_durations,
.switching_pattern_len = switching_pattern_len,
.antenna_ids = antenna_ids};
#pragma pack (pop)
struct node * _node = call_fun(1052,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_connection_cte_tx_param(const hci_con_handle_t conn_handle,
const uint8_t cte_types,
const uint8_t switching_pattern_len,
const uint8_t * antenna_ids){LOG_D("gap_set_connection_cte_tx_param...");remote_mem_map(antenna_ids, switching_pattern_len * sizeof(* antenna_ids));
#pragma pack (push, 1)
struct _param {
const hci_con_handle_t conn_handle;const uint8_t cte_types;const uint8_t switching_pattern_len;const uint8_t * antenna_ids;} _param = {.conn_handle = conn_handle,
.cte_types = cte_types,
.switching_pattern_len = switching_pattern_len,
.antenna_ids = antenna_ids};
#pragma pack (pop)
struct node * _node = call_fun(1053,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_connection_cte_request_enable(const hci_con_handle_t conn_handle,
const uint8_t enable,
const uint16_t requested_cte_interval,
const uint8_t requested_cte_length,
const cte_type_t requested_cte_type){LOG_D("gap_set_connection_cte_request_enable...");;
#pragma pack (push, 1)
struct _param {
const hci_con_handle_t conn_handle;const uint8_t enable;const uint16_t requested_cte_interval;const uint8_t requested_cte_length;const cte_type_t requested_cte_type;} _param = {.conn_handle = conn_handle,
.enable = enable,
.requested_cte_interval = requested_cte_interval,
.requested_cte_length = requested_cte_length,
.requested_cte_type = requested_cte_type};
#pragma pack (pop)
struct node * _node = call_fun(1054,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_connection_cte_response_enable(const hci_con_handle_t conn_handle,
const uint8_t enable){LOG_D("gap_set_connection_cte_response_enable...");;
#pragma pack (push, 1)
struct _param {
const hci_con_handle_t conn_handle;const uint8_t enable;} _param = {.conn_handle = conn_handle,
.enable = enable};
#pragma pack (pop)
struct node * _node = call_fun(1055,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_read_antenna_info(void){LOG_D("gap_read_antenna_info...");;
#pragma pack (push, 1)
struct _param {
} _param = {};
#pragma pack (pop)
struct node * _node = call_fun(1056,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_periodic_adv_rx_enable(const uint16_t sync_handle,
const uint8_t enable){LOG_D("gap_set_periodic_adv_rx_enable...");;
#pragma pack (push, 1)
struct _param {
const uint16_t sync_handle;const uint8_t enable;} _param = {.sync_handle = sync_handle,
.enable = enable};
#pragma pack (pop)
struct node * _node = call_fun(1057,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_periodic_adv_sync_transfer(const hci_con_handle_t conn_handle,
const uint16_t service_data,
const uint16_t sync_handle){LOG_D("gap_periodic_adv_sync_transfer...");;
#pragma pack (push, 1)
struct _param {
const hci_con_handle_t conn_handle;const uint16_t service_data;const uint16_t sync_handle;} _param = {.conn_handle = conn_handle,
.service_data = service_data,
.sync_handle = sync_handle};
#pragma pack (pop)
struct node * _node = call_fun(1058,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_periodic_adv_set_info_transfer(const hci_con_handle_t conn_handle,
const uint16_t service_data,
const uint8_t adv_handle){LOG_D("gap_periodic_adv_set_info_transfer...");;
#pragma pack (push, 1)
struct _param {
const hci_con_handle_t conn_handle;const uint16_t service_data;const uint8_t adv_handle;} _param = {.conn_handle = conn_handle,
.service_data = service_data,
.adv_handle = adv_handle};
#pragma pack (pop)
struct node * _node = call_fun(1059,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_periodic_adv_sync_transfer_param(const hci_con_handle_t conn_handle,
const periodic_adv_sync_transfer_mode_t mode,
const uint16_t skip,
const uint16_t sync_timeout,
const uint8_t cte_excl_types){LOG_D("gap_periodic_adv_sync_transfer_param...");;
#pragma pack (push, 1)
struct _param {
const hci_con_handle_t conn_handle;const periodic_adv_sync_transfer_mode_t mode;const uint16_t skip;const uint16_t sync_timeout;const uint8_t cte_excl_types;} _param = {.conn_handle = conn_handle,
.mode = mode,
.skip = skip,
.sync_timeout = sync_timeout,
.cte_excl_types = cte_excl_types};
#pragma pack (pop)
struct node * _node = call_fun(1060,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_default_periodic_adv_sync_transfer_param(const periodic_adv_sync_transfer_mode_t mode,
const uint16_t skip,
const uint16_t sync_timeout,
const uint8_t cte_excl_types){LOG_D("gap_default_periodic_adv_sync_transfer_param...");;
#pragma pack (push, 1)
struct _param {
const periodic_adv_sync_transfer_mode_t mode;const uint16_t skip;const uint16_t sync_timeout;const uint8_t cte_excl_types;} _param = {.mode = mode,
.skip = skip,
.sync_timeout = sync_timeout,
.cte_excl_types = cte_excl_types};
#pragma pack (pop)
struct node * _node = call_fun(1061,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_host_channel_classification(const uint32_t channel_low,
const uint8_t channel_high){LOG_D("gap_set_host_channel_classification...");;
#pragma pack (push, 1)
struct _param {
const uint32_t channel_low;const uint8_t channel_high;} _param = {.channel_low = channel_low,
.channel_high = channel_high};
#pragma pack (pop)
struct node * _node = call_fun(1062,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
int gap_update_connection_parameters(hci_con_handle_t con_handle,
uint16_t conn_interval_min,
uint16_t conn_interval_max,
uint16_t conn_latency,
uint16_t supervision_timeout,
uint16_t min_ce_len,
uint16_t max_ce_len){LOG_D("gap_update_connection_parameters...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;uint16_t conn_interval_min;uint16_t conn_interval_max;uint16_t conn_latency;uint16_t supervision_timeout;uint16_t min_ce_len;uint16_t max_ce_len;} _param = {.con_handle = con_handle,
.conn_interval_min = conn_interval_min,
.conn_interval_max = conn_interval_max,
.conn_latency = conn_latency,
.supervision_timeout = supervision_timeout,
.min_ce_len = min_ce_len,
.max_ce_len = max_ce_len};
#pragma pack (pop)
struct node * _node = call_fun(1063,&_param,sizeof(_param));int _r= *(int *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
void gap_get_connection_parameter_range(le_connection_parameter_range_t * range){LOG_D("gap_get_connection_parameter_range...");remote_mem_map(range, 1 * sizeof(* range));
#pragma pack (push, 1)
struct _param {
le_connection_parameter_range_t * range;} _param = {.range = range};
#pragma pack (pop)
call_void_fun(1064,&_param,sizeof(_param));}
void gap_set_connection_parameter_range(le_connection_parameter_range_t * range){LOG_D("gap_set_connection_parameter_range...");remote_mem_map(range, 1 * sizeof(* range));
#pragma pack (push, 1)
struct _param {
le_connection_parameter_range_t * range;} _param = {.range = range};
#pragma pack (pop)
call_void_fun(1065,&_param,sizeof(_param));}
uint8_t gap_read_local_tx_power_level(hci_con_handle_t con_handle,
unified_phy_type_t phy){LOG_D("gap_read_local_tx_power_level...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;unified_phy_type_t phy;} _param = {.con_handle = con_handle,
.phy = phy};
#pragma pack (pop)
struct node * _node = call_fun(1066,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_read_remote_tx_power_level(hci_con_handle_t con_handle,
unified_phy_type_t phy){LOG_D("gap_read_remote_tx_power_level...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;unified_phy_type_t phy;} _param = {.con_handle = con_handle,
.phy = phy};
#pragma pack (pop)
struct node * _node = call_fun(1067,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_path_loss_reporting_param(hci_con_handle_t con_handle,
uint8_t high_threshold,
uint8_t high_hysteresis,
uint8_t low_threshold,
uint8_t low_hysteresis,
uint8_t min_time_spent){LOG_D("gap_set_path_loss_reporting_param...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;uint8_t high_threshold;uint8_t high_hysteresis;uint8_t low_threshold;uint8_t low_hysteresis;uint8_t min_time_spent;} _param = {.con_handle = con_handle,
.high_threshold = high_threshold,
.high_hysteresis = high_hysteresis,
.low_threshold = low_threshold,
.low_hysteresis = low_hysteresis,
.min_time_spent = min_time_spent};
#pragma pack (pop)
struct node * _node = call_fun(1068,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_path_loss_reporting_enable(hci_con_handle_t con_handle,
uint8_t enable){LOG_D("gap_set_path_loss_reporting_enable...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;uint8_t enable;} _param = {.con_handle = con_handle,
.enable = enable};
#pragma pack (pop)
struct node * _node = call_fun(1069,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_tx_power_reporting_enable(hci_con_handle_t con_handle,
uint8_t local_enable,
uint8_t remote_enable){LOG_D("gap_set_tx_power_reporting_enable...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;uint8_t local_enable;uint8_t remote_enable;} _param = {.con_handle = con_handle,
.local_enable = local_enable,
.remote_enable = remote_enable};
#pragma pack (pop)
struct node * _node = call_fun(1070,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_set_default_subrate(uint16_t subrate_min,
uint16_t subrate_max,
uint16_t max_latency,
uint16_t continuation_number,
uint16_t supervision_timeout){LOG_D("gap_set_default_subrate...");;
#pragma pack (push, 1)
struct _param {
uint16_t subrate_min;uint16_t subrate_max;uint16_t max_latency;uint16_t continuation_number;uint16_t supervision_timeout;} _param = {.subrate_min = subrate_min,
.subrate_max = subrate_max,
.max_latency = max_latency,
.continuation_number = continuation_number,
.supervision_timeout = supervision_timeout};
#pragma pack (pop)
struct node * _node = call_fun(1071,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_subrate_request(hci_con_handle_t con_handle,
uint16_t subrate_min,
uint16_t subrate_max,
uint16_t max_latency,
uint16_t continuation_number,
uint16_t supervision_timeout){LOG_D("gap_subrate_request...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;uint16_t subrate_min;uint16_t subrate_max;uint16_t max_latency;uint16_t continuation_number;uint16_t supervision_timeout;} _param = {.con_handle = con_handle,
.subrate_min = subrate_min,
.subrate_max = subrate_max,
.max_latency = max_latency,
.continuation_number = continuation_number,
.supervision_timeout = supervision_timeout};
#pragma pack (pop)
struct node * _node = call_fun(1072,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_rx_test_v2(uint8_t rx_channel,
uint8_t phy,
uint8_t modulation_index){LOG_D("gap_rx_test_v2...");;
#pragma pack (push, 1)
struct _param {
uint8_t rx_channel;uint8_t phy;uint8_t modulation_index;} _param = {.rx_channel = rx_channel,
.phy = phy,
.modulation_index = modulation_index};
#pragma pack (pop)
struct node * _node = call_fun(1073,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_rx_test_v3(uint8_t rx_channel,
uint8_t phy,
uint8_t modulation_index,
uint8_t expected_cte_length,
uint8_t expected_cte_type,
uint8_t slot_durations,
uint8_t switching_pattern_length,
const uint8_t * antenna_ids){LOG_D("gap_rx_test_v3...");remote_mem_map(antenna_ids, switching_pattern_length * sizeof(* antenna_ids));
#pragma pack (push, 1)
struct _param {
uint8_t rx_channel;uint8_t phy;uint8_t modulation_index;uint8_t expected_cte_length;uint8_t expected_cte_type;uint8_t slot_durations;uint8_t switching_pattern_length;const uint8_t * antenna_ids;} _param = {.rx_channel = rx_channel,
.phy = phy,
.modulation_index = modulation_index,
.expected_cte_length = expected_cte_length,
.expected_cte_type = expected_cte_type,
.slot_durations = slot_durations,
.switching_pattern_length = switching_pattern_length,
.antenna_ids = antenna_ids};
#pragma pack (pop)
struct node * _node = call_fun(1074,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_tx_test_v2(uint8_t tx_channel,
uint8_t test_data_length,
uint8_t packet_payload,
uint8_t phy){LOG_D("gap_tx_test_v2...");;
#pragma pack (push, 1)
struct _param {
uint8_t tx_channel;uint8_t test_data_length;uint8_t packet_payload;uint8_t phy;} _param = {.tx_channel = tx_channel,
.test_data_length = test_data_length,
.packet_payload = packet_payload,
.phy = phy};
#pragma pack (pop)
struct node * _node = call_fun(1075,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_tx_test_v4(uint8_t tx_channel,
uint8_t test_data_length,
uint8_t packet_payload,
uint8_t phy,
uint8_t cte_length,
uint8_t cte_type,
uint8_t switching_pattern_length,
const uint8_t * antenna_ids,
int8_t tx_power_level){LOG_D("gap_tx_test_v4...");remote_mem_map(antenna_ids, switching_pattern_length * sizeof(* antenna_ids));
#pragma pack (push, 1)
struct _param {
uint8_t tx_channel;uint8_t test_data_length;uint8_t packet_payload;uint8_t phy;uint8_t cte_length;uint8_t cte_type;uint8_t switching_pattern_length;const uint8_t * antenna_ids;int8_t tx_power_level;} _param = {.tx_channel = tx_channel,
.test_data_length = test_data_length,
.packet_payload = packet_payload,
.phy = phy,
.cte_length = cte_length,
.cte_type = cte_type,
.switching_pattern_length = switching_pattern_length,
.antenna_ids = antenna_ids,
.tx_power_level = tx_power_level};
#pragma pack (pop)
struct node * _node = call_fun(1076,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_test_end(void){LOG_D("gap_test_end...");;
#pragma pack (push, 1)
struct _param {
} _param = {};
#pragma pack (pop)
struct node * _node = call_fun(1077,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gap_vendor_tx_continuous_wave(uint8_t enable,
uint8_t power_level_index,
uint16_t freq){LOG_D("gap_vendor_tx_continuous_wave...");;
#pragma pack (push, 1)
struct _param {
uint8_t enable;uint8_t power_level_index;uint16_t freq;} _param = {.enable = enable,
.power_level_index = power_level_index,
.freq = freq};
#pragma pack (pop)
struct node * _node = call_fun(1078,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
int att_server_deferred_read_response(hci_con_handle_t con_handle,
uint16_t attribute_handle,
const uint8_t * value,
uint16_t value_len){LOG_D("att_server_deferred_read_response...");remote_mem_map(value, value_len * sizeof(* value));
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;uint16_t attribute_handle;const uint8_t * value;uint16_t value_len;} _param = {.con_handle = con_handle,
.attribute_handle = attribute_handle,
.value = value,
.value_len = value_len};
#pragma pack (pop)
struct node * _node = call_fun(1079,&_param,sizeof(_param));int _r= *(int *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
int att_server_notify(hci_con_handle_t con_handle,
uint16_t attribute_handle,
const uint8_t * value,
uint16_t value_len){LOG_D("att_server_notify...");remote_mem_map(value, value_len * sizeof(* value));
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;uint16_t attribute_handle;const uint8_t * value;uint16_t value_len;} _param = {.con_handle = con_handle,
.attribute_handle = attribute_handle,
.value = value,
.value_len = value_len};
#pragma pack (pop)
struct node * _node = call_fun(1080,&_param,sizeof(_param));int _r= *(int *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
int att_server_indicate(hci_con_handle_t con_handle,
uint16_t attribute_handle,
const uint8_t * value,
uint16_t value_len){LOG_D("att_server_indicate...");remote_mem_map(value, value_len * sizeof(* value));
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;uint16_t attribute_handle;const uint8_t * value;uint16_t value_len;} _param = {.con_handle = con_handle,
.attribute_handle = attribute_handle,
.value = value,
.value_len = value_len};
#pragma pack (pop)
struct node * _node = call_fun(1081,&_param,sizeof(_param));int _r= *(int *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint16_t att_server_get_mtu(hci_con_handle_t con_handle){LOG_D("att_server_get_mtu...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;} _param = {.con_handle = con_handle};
#pragma pack (pop)
struct node * _node = call_fun(1082,&_param,sizeof(_param));uint16_t _r= *(uint16_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_discover_primary_services(user_packet_handler_t callback,
hci_con_handle_t con_handle){LOG_D("gatt_client_discover_primary_services...");;
#pragma pack (push, 1)
struct _param {
user_packet_handler_t callback;hci_con_handle_t con_handle;} _param = {.callback = callback,
.con_handle = con_handle};
#pragma pack (pop)
struct node * _node = call_fun(1083,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_discover_primary_services_by_uuid16(user_packet_handler_t callback,
hci_con_handle_t con_handle,
uint16_t uuid16){LOG_D("gatt_client_discover_primary_services_by_uuid16...");;
#pragma pack (push, 1)
struct _param {
user_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t uuid16;} _param = {.callback = callback,
.con_handle = con_handle,
.uuid16 = uuid16};
#pragma pack (pop)
struct node * _node = call_fun(1084,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_discover_primary_services_by_uuid128(user_packet_handler_t callback,
hci_con_handle_t con_handle,
const uint8_t * uuid128){LOG_D("gatt_client_discover_primary_services_by_uuid128...");remote_mem_map(uuid128, 16 * sizeof(* uuid128));
#pragma pack (push, 1)
struct _param {
user_packet_handler_t callback;hci_con_handle_t con_handle;const uint8_t * uuid128;} _param = {.callback = callback,
.con_handle = con_handle,
.uuid128 = uuid128};
#pragma pack (pop)
struct node * _node = call_fun(1085,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_find_included_services_for_service(user_packet_handler_t callback,
hci_con_handle_t con_handle,
const uint16_t start_group_handle,
const uint16_t end_group_handle){LOG_D("gatt_client_find_included_services_for_service...");;
#pragma pack (push, 1)
struct _param {
user_packet_handler_t callback;hci_con_handle_t con_handle;const uint16_t start_group_handle;const uint16_t end_group_handle;} _param = {.callback = callback,
.con_handle = con_handle,
.start_group_handle = start_group_handle,
.end_group_handle = end_group_handle};
#pragma pack (pop)
struct node * _node = call_fun(1086,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_discover_characteristics_for_service(user_packet_handler_t callback,
hci_con_handle_t con_handle,
const uint16_t start_group_handle,
const uint16_t end_group_handle){LOG_D("gatt_client_discover_characteristics_for_service...");;
#pragma pack (push, 1)
struct _param {
user_packet_handler_t callback;hci_con_handle_t con_handle;const uint16_t start_group_handle;const uint16_t end_group_handle;} _param = {.callback = callback,
.con_handle = con_handle,
.start_group_handle = start_group_handle,
.end_group_handle = end_group_handle};
#pragma pack (pop)
struct node * _node = call_fun(1087,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_discover_characteristics_for_handle_range_by_uuid16(btstack_packet_handler_t callback,
const hci_con_handle_t con_handle,
const uint16_t start_handle,
const uint16_t end_handle,
const uint16_t uuid16){LOG_D("gatt_client_discover_characteristics_for_handle_range_by_uuid16...");;
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;const hci_con_handle_t con_handle;const uint16_t start_handle;const uint16_t end_handle;const uint16_t uuid16;} _param = {.callback = callback,
.con_handle = con_handle,
.start_handle = start_handle,
.end_handle = end_handle,
.uuid16 = uuid16};
#pragma pack (pop)
struct node * _node = call_fun(1088,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_discover_characteristics_for_handle_range_by_uuid128(btstack_packet_handler_t callback,
const hci_con_handle_t con_handle,
const uint16_t start_handle,
const uint16_t end_handle,
const uint8_t * uuid128){LOG_D("gatt_client_discover_characteristics_for_handle_range_by_uuid128...");remote_mem_map(uuid128, 16 * sizeof(* uuid128));
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;const hci_con_handle_t con_handle;const uint16_t start_handle;const uint16_t end_handle;const uint8_t * uuid128;} _param = {.callback = callback,
.con_handle = con_handle,
.start_handle = start_handle,
.end_handle = end_handle,
.uuid128 = uuid128};
#pragma pack (pop)
struct node * _node = call_fun(1089,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_discover_characteristic_descriptors(btstack_packet_handler_t callback,
hci_con_handle_t con_handle,
gatt_client_characteristic_t * characteristic){LOG_D("gatt_client_discover_characteristic_descriptors...");remote_mem_map(characteristic, 1 * sizeof(* characteristic));
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;hci_con_handle_t con_handle;gatt_client_characteristic_t * characteristic;} _param = {.callback = callback,
.con_handle = con_handle,
.characteristic = characteristic};
#pragma pack (pop)
struct node * _node = call_fun(1090,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_read_value_of_characteristic_using_value_handle(btstack_packet_handler_t callback,
hci_con_handle_t con_handle,
uint16_t characteristic_value_handle){LOG_D("gatt_client_read_value_of_characteristic_using_value_handle...");;
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t characteristic_value_handle;} _param = {.callback = callback,
.con_handle = con_handle,
.characteristic_value_handle = characteristic_value_handle};
#pragma pack (pop)
struct node * _node = call_fun(1091,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_read_value_of_characteristics_by_uuid16(btstack_packet_handler_t callback,
hci_con_handle_t con_handle,
uint16_t start_handle,
uint16_t end_handle,
uint16_t uuid16){LOG_D("gatt_client_read_value_of_characteristics_by_uuid16...");;
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t start_handle;uint16_t end_handle;uint16_t uuid16;} _param = {.callback = callback,
.con_handle = con_handle,
.start_handle = start_handle,
.end_handle = end_handle,
.uuid16 = uuid16};
#pragma pack (pop)
struct node * _node = call_fun(1092,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_read_value_of_characteristics_by_uuid128(btstack_packet_handler_t callback,
hci_con_handle_t con_handle,
uint16_t start_handle,
uint16_t end_handle,
uint8_t * uuid128){LOG_D("gatt_client_read_value_of_characteristics_by_uuid128...");remote_mem_map(uuid128, 16 * sizeof(* uuid128));
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t start_handle;uint16_t end_handle;uint8_t * uuid128;} _param = {.callback = callback,
.con_handle = con_handle,
.start_handle = start_handle,
.end_handle = end_handle,
.uuid128 = uuid128};
#pragma pack (pop)
struct node * _node = call_fun(1093,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_read_long_value_of_characteristic_using_value_handle(btstack_packet_handler_t callback,
hci_con_handle_t con_handle,
uint16_t characteristic_value_handle){LOG_D("gatt_client_read_long_value_of_characteristic_using_value_handle...");;
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t characteristic_value_handle;} _param = {.callback = callback,
.con_handle = con_handle,
.characteristic_value_handle = characteristic_value_handle};
#pragma pack (pop)
struct node * _node = call_fun(1094,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset(btstack_packet_handler_t callback,
hci_con_handle_t con_handle,
uint16_t characteristic_value_handle,
uint16_t offset){LOG_D("gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset...");;
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t characteristic_value_handle;uint16_t offset;} _param = {.callback = callback,
.con_handle = con_handle,
.characteristic_value_handle = characteristic_value_handle,
.offset = offset};
#pragma pack (pop)
struct node * _node = call_fun(1095,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_read_multiple_characteristic_values(btstack_packet_handler_t callback,
hci_con_handle_t con_handle,
int num_value_handles,
uint16_t * value_handles){LOG_D("gatt_client_read_multiple_characteristic_values...");remote_mem_map(value_handles, num_value_handles * sizeof(*value_handles));
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;hci_con_handle_t con_handle;int num_value_handles;uint16_t * value_handles;} _param = {.callback = callback,
.con_handle = con_handle,
.num_value_handles = num_value_handles,
.value_handles = value_handles};
#pragma pack (pop)
struct node * _node = call_fun(1096,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_write_value_of_characteristic_without_response(hci_con_handle_t con_handle,
uint16_t characteristic_value_handle,
uint16_t length,
const uint8_t * data){LOG_D("gatt_client_write_value_of_characteristic_without_response...");remote_mem_map(data, length * sizeof(* data));
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;uint16_t characteristic_value_handle;uint16_t length;const uint8_t * data;} _param = {.con_handle = con_handle,
.characteristic_value_handle = characteristic_value_handle,
.length = length,
.data = data};
#pragma pack (pop)
struct node * _node = call_fun(1097,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_signed_write_without_response(btstack_packet_handler_t callback,
hci_con_handle_t con_handle,
uint16_t handle,
uint16_t message_len,
const uint8_t * message){LOG_D("gatt_client_signed_write_without_response...");remote_mem_map(message, message_len * sizeof(* message));
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t handle;uint16_t message_len;const uint8_t * message;} _param = {.callback = callback,
.con_handle = con_handle,
.handle = handle,
.message_len = message_len,
.message = message};
#pragma pack (pop)
struct node * _node = call_fun(1098,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_write_value_of_characteristic(btstack_packet_handler_t callback,
hci_con_handle_t con_handle,
uint16_t characteristic_value_handle,
uint16_t length,
const uint8_t * data){LOG_D("gatt_client_write_value_of_characteristic...");remote_mem_map(data, length * sizeof(* data));
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t characteristic_value_handle;uint16_t length;const uint8_t * data;} _param = {.callback = callback,
.con_handle = con_handle,
.characteristic_value_handle = characteristic_value_handle,
.length = length,
.data = data};
#pragma pack (pop)
struct node * _node = call_fun(1099,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_write_long_value_of_characteristic(btstack_packet_handler_t callback,
hci_con_handle_t con_handle,
uint16_t characteristic_value_handle,
uint16_t length,
const uint8_t * data){LOG_D("gatt_client_write_long_value_of_characteristic...");remote_mem_map(data, length * sizeof(* data));
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t characteristic_value_handle;uint16_t length;const uint8_t * data;} _param = {.callback = callback,
.con_handle = con_handle,
.characteristic_value_handle = characteristic_value_handle,
.length = length,
.data = data};
#pragma pack (pop)
struct node * _node = call_fun(1100,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_write_long_value_of_characteristic_with_offset(btstack_packet_handler_t callback,
hci_con_handle_t con_handle,
uint16_t characteristic_value_handle,
uint16_t offset,
uint16_t length,
const uint8_t * data){LOG_D("gatt_client_write_long_value_of_characteristic_with_offset...");remote_mem_map(data, length * sizeof(* data));
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t characteristic_value_handle;uint16_t offset;uint16_t length;const uint8_t * data;} _param = {.callback = callback,
.con_handle = con_handle,
.characteristic_value_handle = characteristic_value_handle,
.offset = offset,
.length = length,
.data = data};
#pragma pack (pop)
struct node * _node = call_fun(1101,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_reliable_write_long_value_of_characteristic(btstack_packet_handler_t callback,
hci_con_handle_t con_handle,
uint16_t characteristic_value_handle,
uint16_t length,
const uint8_t * data){LOG_D("gatt_client_reliable_write_long_value_of_characteristic...");remote_mem_map(data, length * sizeof(* data));
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t characteristic_value_handle;uint16_t length;const uint8_t * data;} _param = {.callback = callback,
.con_handle = con_handle,
.characteristic_value_handle = characteristic_value_handle,
.length = length,
.data = data};
#pragma pack (pop)
struct node * _node = call_fun(1102,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_read_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback,
hci_con_handle_t con_handle,
uint16_t descriptor_handle){LOG_D("gatt_client_read_characteristic_descriptor_using_descriptor_handle...");;
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t descriptor_handle;} _param = {.callback = callback,
.con_handle = con_handle,
.descriptor_handle = descriptor_handle};
#pragma pack (pop)
struct node * _node = call_fun(1103,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_read_long_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback,
hci_con_handle_t con_handle,
uint16_t descriptor_handle){LOG_D("gatt_client_read_long_characteristic_descriptor_using_descriptor_handle...");;
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t descriptor_handle;} _param = {.callback = callback,
.con_handle = con_handle,
.descriptor_handle = descriptor_handle};
#pragma pack (pop)
struct node * _node = call_fun(1104,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset(btstack_packet_handler_t callback,
hci_con_handle_t con_handle,
uint16_t descriptor_handle,
uint16_t offset){LOG_D("gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset...");;
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t descriptor_handle;uint16_t offset;} _param = {.callback = callback,
.con_handle = con_handle,
.descriptor_handle = descriptor_handle,
.offset = offset};
#pragma pack (pop)
struct node * _node = call_fun(1105,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_write_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback,
hci_con_handle_t con_handle,
uint16_t descriptor_handle,
uint16_t length,
uint8_t * data){LOG_D("gatt_client_write_characteristic_descriptor_using_descriptor_handle...");remote_mem_map(data, length * sizeof(* data));
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t descriptor_handle;uint16_t length;uint8_t * data;} _param = {.callback = callback,
.con_handle = con_handle,
.descriptor_handle = descriptor_handle,
.length = length,
.data = data};
#pragma pack (pop)
struct node * _node = call_fun(1106,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_write_long_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback,
hci_con_handle_t con_handle,
uint16_t descriptor_handle,
uint16_t length,
uint8_t * data){LOG_D("gatt_client_write_long_characteristic_descriptor_using_descriptor_handle...");remote_mem_map(data, length * sizeof(* data));
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t descriptor_handle;uint16_t length;uint8_t * data;} _param = {.callback = callback,
.con_handle = con_handle,
.descriptor_handle = descriptor_handle,
.length = length,
.data = data};
#pragma pack (pop)
struct node * _node = call_fun(1107,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset(btstack_packet_handler_t callback,
hci_con_handle_t con_handle,
uint16_t descriptor_handle,
uint16_t offset,
uint16_t length,
uint8_t * data){LOG_D("gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset...");remote_mem_map(data, length * sizeof(* data));
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t descriptor_handle;uint16_t offset;uint16_t length;uint8_t * data;} _param = {.callback = callback,
.con_handle = con_handle,
.descriptor_handle = descriptor_handle,
.offset = offset,
.length = length,
.data = data};
#pragma pack (pop)
struct node * _node = call_fun(1108,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_write_client_characteristic_configuration(btstack_packet_handler_t callback,
hci_con_handle_t con_handle,
gatt_client_characteristic_t * characteristic,
uint16_t configuration){LOG_D("gatt_client_write_client_characteristic_configuration...");remote_mem_map(characteristic, 1 * sizeof(* characteristic));
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;hci_con_handle_t con_handle;gatt_client_characteristic_t * characteristic;uint16_t configuration;} _param = {.callback = callback,
.con_handle = con_handle,
.characteristic = characteristic,
.configuration = configuration};
#pragma pack (pop)
struct node * _node = call_fun(1109,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_prepare_write(btstack_packet_handler_t callback,
hci_con_handle_t con_handle,
uint16_t attribute_handle,
uint16_t offset,
uint16_t length,
uint8_t * data){LOG_D("gatt_client_prepare_write...");remote_mem_map(data, length * sizeof(* data));
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;hci_con_handle_t con_handle;uint16_t attribute_handle;uint16_t offset;uint16_t length;uint8_t * data;} _param = {.callback = callback,
.con_handle = con_handle,
.attribute_handle = attribute_handle,
.offset = offset,
.length = length,
.data = data};
#pragma pack (pop)
struct node * _node = call_fun(1110,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_execute_write(btstack_packet_handler_t callback,
hci_con_handle_t con_handle){LOG_D("gatt_client_execute_write...");;
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;hci_con_handle_t con_handle;} _param = {.callback = callback,
.con_handle = con_handle};
#pragma pack (pop)
struct node * _node = call_fun(1111,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
uint8_t gatt_client_cancel_write(btstack_packet_handler_t callback,
hci_con_handle_t con_handle){LOG_D("gatt_client_cancel_write...");;
#pragma pack (push, 1)
struct _param {
btstack_packet_handler_t callback;hci_con_handle_t con_handle;} _param = {.callback = callback,
.con_handle = con_handle};
#pragma pack (pop)
struct node * _node = call_fun(1112,&_param,sizeof(_param));uint8_t _r= *(uint8_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
int gatt_client_is_ready(hci_con_handle_t con_handle){LOG_D("gatt_client_is_ready...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;} _param = {.con_handle = con_handle};
#pragma pack (pop)
struct node * _node = call_fun(1113,&_param,sizeof(_param));int _r= *(int *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
void gatt_client_listen_for_characteristic_value_updates(gatt_client_notification_t * notification,
btstack_packet_handler_t packet_handler,
hci_con_handle_t con_handle,
uint16_t value_handle){LOG_D("gatt_client_listen_for_characteristic_value_updates...");alloc_heap_for_conn(notification,sizeof(* notification),con_handle);
#pragma pack (push, 1)
struct _param {
gatt_client_notification_t * notification;btstack_packet_handler_t packet_handler;hci_con_handle_t con_handle;uint16_t value_handle;} _param = {.notification = notification,
.packet_handler = packet_handler,
.con_handle = con_handle,
.value_handle = value_handle};
#pragma pack (pop)
call_void_fun(1114,&_param,sizeof(_param));}
int sm_config(uint8_t enable,
io_capability_t io_capability,
int request_security,
const sm_persistent_t * persistent){LOG_D("sm_config...");;
#pragma pack (push, 1)
struct _param {
uint8_t enable;io_capability_t io_capability;int request_security;const sm_persistent_t * persistent;} _param = {.enable = enable,
.io_capability = io_capability,
.request_security = request_security,
.persistent = persistent};
#pragma pack (pop)
struct node * _node = call_fun(1115,&_param,sizeof(_param));int _r= *(int *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
void sm_private_random_address_generation_set_mode(gap_random_address_type_t random_address_type){LOG_D("sm_private_random_address_generation_set_mode...");;
#pragma pack (push, 1)
struct _param {
gap_random_address_type_t random_address_type;} _param = {.random_address_type = random_address_type};
#pragma pack (pop)
call_void_fun(1116,&_param,sizeof(_param));}
gap_random_address_type_t sm_private_random_address_generation_get_mode(void){LOG_D("sm_private_random_address_generation_get_mode...");;
#pragma pack (push, 1)
struct _param {
} _param = {};
#pragma pack (pop)
struct node * _node = call_fun(1117,&_param,sizeof(_param));gap_random_address_type_t _r= *(gap_random_address_type_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
void sm_private_random_address_generation_set_update_period(int period_ms){LOG_D("sm_private_random_address_generation_set_update_period...");;
#pragma pack (push, 1)
struct _param {
int period_ms;} _param = {.period_ms = period_ms};
#pragma pack (pop)
call_void_fun(1118,&_param,sizeof(_param));}
void sm_set_accepted_stk_generation_methods(uint8_t accepted_stk_generation_methods){LOG_D("sm_set_accepted_stk_generation_methods...");;
#pragma pack (push, 1)
struct _param {
uint8_t accepted_stk_generation_methods;} _param = {.accepted_stk_generation_methods = accepted_stk_generation_methods};
#pragma pack (pop)
call_void_fun(1119,&_param,sizeof(_param));}
void sm_set_encryption_key_size_range(uint8_t min_size,
uint8_t max_size){LOG_D("sm_set_encryption_key_size_range...");;
#pragma pack (push, 1)
struct _param {
uint8_t min_size;uint8_t max_size;} _param = {.min_size = min_size,
.max_size = max_size};
#pragma pack (pop)
call_void_fun(1120,&_param,sizeof(_param));}
void sm_set_authentication_requirements(uint8_t auth_req){LOG_D("sm_set_authentication_requirements...");;
#pragma pack (push, 1)
struct _param {
uint8_t auth_req;} _param = {.auth_req = auth_req};
#pragma pack (pop)
call_void_fun(1121,&_param,sizeof(_param));}
int sm_address_resolution_lookup(uint8_t addr_type,
bd_addr_t addr){LOG_D("sm_address_resolution_lookup...");remote_mem_map(addr, BD_ADDR_LEN * sizeof(* addr));
#pragma pack (push, 1)
struct _param {
uint8_t addr_type;const uint8_t * addr;} _param = {.addr_type = addr_type,
.addr = addr};
#pragma pack (pop)
struct node * _node = call_fun(1122,&_param,sizeof(_param));int _r= *(int *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
void sm_config_conn(hci_con_handle_t con_handle,
io_capability_t io_capability,
uint8_t auth_req){LOG_D("sm_config_conn...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;io_capability_t io_capability;uint8_t auth_req;} _param = {.con_handle = con_handle,
.io_capability = io_capability,
.auth_req = auth_req};
#pragma pack (pop)
call_void_fun(1123,&_param,sizeof(_param));}
void sm_bonding_decline(hci_con_handle_t con_handle){LOG_D("sm_bonding_decline...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;} _param = {.con_handle = con_handle};
#pragma pack (pop)
call_void_fun(1124,&_param,sizeof(_param));}
void sm_just_works_confirm(hci_con_handle_t con_handle){LOG_D("sm_just_works_confirm...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;} _param = {.con_handle = con_handle};
#pragma pack (pop)
call_void_fun(1125,&_param,sizeof(_param));}
void sm_passkey_input(hci_con_handle_t con_handle,
uint32_t passkey){LOG_D("sm_passkey_input...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;uint32_t passkey;} _param = {.con_handle = con_handle,
.passkey = passkey};
#pragma pack (pop)
call_void_fun(1126,&_param,sizeof(_param));}
void sm_send_security_request(hci_con_handle_t con_handle){LOG_D("sm_send_security_request...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;} _param = {.con_handle = con_handle};
#pragma pack (pop)
call_void_fun(1127,&_param,sizeof(_param));}
int sm_encryption_key_size(hci_con_handle_t con_handle){LOG_D("sm_encryption_key_size...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;} _param = {.con_handle = con_handle};
#pragma pack (pop)
struct node * _node = call_fun(1128,&_param,sizeof(_param));int _r= *(int *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
int sm_authenticated(hci_con_handle_t con_handle){LOG_D("sm_authenticated...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;} _param = {.con_handle = con_handle};
#pragma pack (pop)
struct node * _node = call_fun(1129,&_param,sizeof(_param));int _r= *(int *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
authorization_state_t sm_authorization_state(hci_con_handle_t con_handle){LOG_D("sm_authorization_state...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;} _param = {.con_handle = con_handle};
#pragma pack (pop)
struct node * _node = call_fun(1130,&_param,sizeof(_param));authorization_state_t _r= *(authorization_state_t *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
void sm_request_pairing(hci_con_handle_t con_handle){LOG_D("sm_request_pairing...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;} _param = {.con_handle = con_handle};
#pragma pack (pop)
call_void_fun(1131,&_param,sizeof(_param));}
void sm_authorization_decline(hci_con_handle_t con_handle){LOG_D("sm_authorization_decline...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;} _param = {.con_handle = con_handle};
#pragma pack (pop)
call_void_fun(1132,&_param,sizeof(_param));}
void sm_authorization_grant(hci_con_handle_t con_handle){LOG_D("sm_authorization_grant...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;} _param = {.con_handle = con_handle};
#pragma pack (pop)
call_void_fun(1133,&_param,sizeof(_param));}
int sm_le_device_key(hci_con_handle_t con_handle){LOG_D("sm_le_device_key...");;
#pragma pack (push, 1)
struct _param {
hci_con_handle_t con_handle;} _param = {.con_handle = con_handle};
#pragma pack (pop)
struct node * _node = call_fun(1134,&_param,sizeof(_param));int _r= *(int *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
void kv_remove_all(void){LOG_D("kv_remove_all...");;
#pragma pack (push, 1)
struct _param {
} _param = {};
#pragma pack (pop)
call_void_fun(1135,&_param,sizeof(_param));}
void kv_remove(const kvkey_t key){LOG_D("kv_remove...");;
#pragma pack (push, 1)
struct _param {
const kvkey_t key;} _param = {.key = key};
#pragma pack (pop)
call_void_fun(1136,&_param,sizeof(_param));}
int kv_put(const kvkey_t key,
const uint8_t * data,
int16_t length){LOG_D("kv_put...");remote_mem_map(data, length * sizeof(* data));
#pragma pack (push, 1)
struct _param {
const kvkey_t key;const uint8_t * data;int16_t length;} _param = {.key = key,
.data = data,
.length = length};
#pragma pack (pop)
struct node * _node = call_fun(1137,&_param,sizeof(_param));int _r= *(int *)extract_ret_param(_node, NULL);GEN_OS()->free(_node);return _r;}
void kv_value_modified(void){LOG_D("kv_value_modified...");;
#pragma pack (push, 1)
struct _param {
} _param = {};
#pragma pack (pop)
call_void_fun(1138,&_param,sizeof(_param));}
void kv_commit(int flag_always_write){LOG_D("kv_commit...");;
#pragma pack (push, 1)
struct _param {
int flag_always_write;} _param = {.flag_always_write = flag_always_write};
#pragma pack (pop)
call_void_fun(1139,&_param,sizeof(_param));}
void le_device_db_remove_key(int index){LOG_D("le_device_db_remove_key...");;
#pragma pack (push, 1)
struct _param {
int index;} _param = {.index = index};
#pragma pack (pop)
call_void_fun(1140,&_param,sizeof(_param));}
