
/** @file     mesh_api.h                                                 *                                                                            
*  @brief    function API and data structure for customer to work with stack               *                       
*  @version  1.0.0.                                                     *
*  @date     2019/10/10                                                 *
*                                                                       *
*  Copyright (c) 2019 IngChips corp.                                    *
*                                                                       *
*                                                                       *
 *************************************************************************************************************/

#ifndef _MESH_API_H
#define _MESH_API_H

#include <stdint.h>

#include "mesh_def.h"
#include "access.h"
#include "nimble_npl.h"
#include "nimble_npl_os.h"
#include "timers.h"
#include "btstack_event.h"

#define PRIVATE_FUNCTION
/**
 * @brief Bluetooth Mesh App API
 * @defgroup bt_mesh_cfg_srv Bluetooth Mesh Configuration Server Model
 * @ingroup bt_mesh
 * @{
 */
/**
* @def NODE_FEATURE_RELAY
*/
#define NODE_FEATURE_RELAY   1

/**
* @def NODE_FEATURE_PROXY
*/
#define NODE_FEATURE_PROXY   2

/**
* @def NODE_FEATURE_LP
*/
#define NODE_FEATURE_LP      4

/**
* @def NODE_FEATURE_FRIEND
*/
#define NODE_FEATURE_FRIEND  8

/**
* @def NODE_USE_BLE_5_ADV
*/

#define NODE_USE_BLE_5_ADV   0x80

/**
* @enum TRACE_DEF
*/
typedef enum TARACE_DEF
{
    TRACE_DISABLE,
    TRACE_ENABLE  
}trace_def_t;
    

/** @brief configure a mesh device 
 *
 *     to configure a mesh device:  prov capability  and support model etc.
 *
 *  @param a_prov  pointer to bt_mesh_prov structure that provide prov ability
 *
 *  @param a_comp  pointer to bt_mesh_comp that provide the models that device supports
 *
 *  @return  void
 *
 */
void mesh_setup(const struct bt_mesh_prov *a_prov,
                const struct bt_mesh_comp *a_comp);

#define MESH_MODEL_OP_CLIENT  0
#define MESH_MODEL_OP_SERVER  1

/** @brief set mesh device name
 *
 *     to setup mesh device name
 *  @param name   char buffer of the name.
 *
 *  @return  void
 *
 */
 void mesh_set_dev_name(const char *name);


/** @brief platform configure
 *
 *      Some platform only supports one of pb_gatt and pb_adv, and some details that associate with 
 *    the special platform. So to provide this API to setup special configuration of platform
 *
 *  @param platform    identify platform  1: PB_ADV platform
 *                                        2: PB_GATT platform
 *
 *  @param addr      mesh BLE address platform specifies, it should not be NULL.
 *
 *  @param param         the private param that platform specifies ,it could be NULL
 *
 *  @note   here support Alibaba tianmao ,and CSR mesh platform
 */									   
void mesh_platform_config(uint8_t platform , uint8_t* addr,uint8_t* param);

/** @brief  judge that the mesh device is provisioned or not.
 *
 *      Judge that the mesh device is provisioned or not.
 *
 *  @return  bool     TRUE:  provisioned   FASLE: not provisioned
 *
 *  @note    This API is used for application whether it could start a mesh service or not.
 */
bool is_provisioned_poweron(void);

/** @brief  To start a mesh service for the application level.
 *
 *      Provide to application to start a mesh client service 
 *
 *  @param msg     pointer to app_request_t structure
 *
 *  @param len   size of the msg
 *
 *  @return  void
 *
 *  @note  if this function is invoked inside mesh task ,it would do nothing. @link is_mesh_task() @endlink
           this message would deliver the service reuqest over advertising bearer.
 */
void mesh_service_trigger(uint8_t* msg, uint8_t len);

/** @brief  faciliate to write to NVM 
 *
 *     Currently Only use to remove the provsion information.
 *
 *  @param word   write a value into NVM which mark the device  provisioned  or not. If want to remove
 *                provision information the word should be any value other than 1,2 and 0xffffffff.
 *
 *  @return  void
 *
 */
void write_control_word2mirror(uint32_t word);

void flash_write(void);

/** @brief   transfer uart data to mesh stack
 *
 *     If application support uart input operation, then this API will transfer uart data from application 
 *     to mesh stack.             
 *
 *  @param mesh_msg    buffer from the uart
 *
 *  @param len     the size of mesh_msg
 */
void mesh_at_entry(uint8_t* mesh_msg ,uint8_t len);

/** @brief   check some appid is avaliable from stack inside
 *
 *    Use this API to check whther an appid exist in mesh stack, if exist then application could start the service 
 *    assocates with is appid.
 *
 *  @param appid     mesh application id
 * 
 *  @return bool    FALSE : not exist
 *                  TRUE:exist
 */
bool service_is_ready(uint16_t appid);

/** @brief   disable beacon advertising function
 *
 *     Sometimes application would require disable beacon advertising, so this API would take it effect. this API must be called in host task.
 *    
 */
void  gap_beacon_disable(void);

/** @brief   enable beacon advertising function
 *
 *     revert to beaconning adv,and  this API should must be called in host task.
 *    
 */
void  gap_beacon_enable(void);
/**
 * @brief a trace configurce
 * @defgroup trace_configure
 * @ingroup bt_mesh
 * @{
 */
 
/** @brief setup the uart data output function from APP
*
*  function pointer mesh_at_out declaration.
*  the system support AT command, and customer could nominate a uart trace port to 
*  print the response AT command  execute results,or some trace that the customer 
*  want to get from specified UART.So system needs to know such funcion pointer from
*  customer which will redirect such trace to the appointed UART.
*
*  @param buf  trace stream in bytes

*  @param len  length of the buf..
*
*/

typedef void (*mesh_at_out)(const char* buf, int len);


/** @brief   trace configuration
*
*    Due to mass trace information in mesh stack from upper access layer to networking layer. So it needs to control
*    the trace output according to which layer it belongs ,and which class the trace information belongs.
*
*  @param enable       enable or disable the trace output. @see trace_info
*
*  @param sel_bits     bitsets for which layer sets the trace information would print. 
*
*  @param cla_bit      bitsets which class trace will print associated to sel_bits  @see TRACE_CLA
* 
*           
*/

void mesh_trace_config(uint16_t sel_bits,uint16_t cla_bit);


/** @brief    an API configure for mesh stack to transfer  data to uart
 *
 *    Sometimes application would expect mesh stack trace info output to the desired port, so application would 
 *    configure this function through this API. Stack inside would call this function if trace is needed.
 *
 *  @param ptrfun     function pointer to typed mesh_at_out. @see mesh_at_out
 *
 */
void set_mesh_uart_output_func(mesh_at_out ptrfun);

/**
* @}
*/

/**
 * @brief mesh API for application
 * @defgroup API for mesh app
 * @ingroup bluetooth_mesh
 * @{
 */
 
/** @brief    to get the default event queue for  MESH stack
 *
 *  @return ble_npl_eventq  a pointer to this type
 *
 */
 
struct ble_npl_eventq *nimble_port_get_dflt_eventq(void);

/** @brief   enable or disable white list specified by mesh  
 *
 *  @param en           0: disable  1: enable
 * 
 *
 */
void mesh_whitlist_enable(uint8_t en);

/** @brief  start the scanning in mesh 
 *
 *  @note   this API must be only used in mesh task 
 *
 */
int bt_mesh_scan_enable(void);

/** @brief  stop the scanning in mesh  
 *
 *  @note   this API must be only used in mesh task
 *
 */
int bt_mesh_scan_disable(void);

/** @brief   setup the items in white list 
 *
 *  @param addr        BLE device address into white list
 *
 *  @param type        BLE device address type
 *
 *  @note this API must be only used in mesh task
 */
 void bt_mesh_set_white_list(uint8_t* addr, uint8_t type);


/** @brief   stop the advertising in mesh task. There are 2 advertising sets in mesh, and advertising handle are specified internally. 
 *           One for proxy service ,the advertising handle is 7 , the other one for beacon service, the advertising handle is 6.
 *
 *  @param proxy       TRUE: stop advertising for proxy service  FALSE: stop advertisiong for beacon service
 *
 */
int bt_le_adv_stop(bool proxy);

/** @brief   Once mesh deliver the advertising data to host for assure of that the advertising data would be successfully sent to peer,
 *           the mesh task would sleep for a little time (duration) avoid to sending new advertising data to host. application could adjust this 
 *           duration as times of the adverting interval.
 *
 *  @param   ms        sleeping duration. 20ms is the minimum value. normally as 60ms.
 *
 *  @return            0:sucess     others: failed
 *
 *  @note    if application does not explicitly setup this duration ,then mesh would start this duration value=60ms as default.
 */
int8_t set_mesh_sleep_duration(uint32_t ms);

/** @brief   add a group address to a node bound to a special model. This provide a simple way to bind a group address locally not
 *           through BLE mesh standard message echanges between server and client.
 *
 *  @param elem_addr   the element address that would bound to group address
 *
 *  @param sub_addr    the group address that would be bound to.
 *
 *  @param id          model is vendor specified. so it is the id of vendor model.
 *
 *  @param company     the commpay id that specifies the vendor model id.
 * 
 *  @return            0: success   others: failed
 *
 *  @note    typical scenario for this API is that many light models are groupped, so they will be light on/off simultaneously.
 *           if this group address is same and not changed, so local device could be internally added into this group quickly.
 */
int8_t mesh_add_local_group_addr(uint16_t elem_addr, uint16_t sub_addr, uint32_t id,uint32_t company);

/** @brief   delete a group address to a node bound to a special model. This provide a simple way to bind a group address locally not
 *           through BLE mesh standard message echanges between server and client.
 *
 *  @param elem_addr   the element address that would bound to group address
 *
 *  @param sub_addr    the group address that would be deleted to.
 *
 *  @param id          model is vendor specified. so it is the id of vendor model.
 *
 *  @param company     the commpay id that specifies the vendor model id.
 * 
 *  @return            0: success   others: failed
 *
 *  @note    typical scenario for this API is that many light models are groupped, so they will be light on/off simultaneously.
 *           if this group address is same and not changed, so local device could be internally deleted this group quickly.
 */
int8_t mesh_delete_local_group_addr(uint16_t elem_addr, uint16_t sub_addr, uint32_t id,uint32_t company);

/** @brief   to mark a flag that application would set current node as a provisioner.
 *
 *  @note    once a device is provisioned and turn to be a node. In consideration of the fast provision, the node could play 
 *           the role of a provisioner, so call this API to mark this role. This API should be called in HOST task.
 *
 *  @return             0: success  others: failed
 */
int8_t mesh_provision_start(void);

/** @brief   to mark a flag that the deivce would not be a provisioner.
 *
 *  @return  EOK
 *
 *  @note    revert a node that has provisioning capability to a normal device. This API should be called in HOST task.
 *
 */
int8_t mesh_provision_disable(void);

/** @brief   set the mark that advertising is always on or stopped once sent out.
 *
 *  @param mark         0: not stop advertising    1:stop advertising once sent out
 *
 *  @note    There are 2 modes to transfer the advertising package. One is advertising enable always on ,and controller would 
 *           advertise the last advertising data even if the data has been received by peer, so it would increase the power 
 *           consumption,and the other one is advertising is stopped once the advertising data has been sent for a piece of time,and 
 *           it can save the power.
 *           This piece of time could be defined by the API: int8_t set_mesh_sleep_duration(uint32_t ms).           
 */
void set_flag_for_adv_sent(uint8_t  mark);

/** @brief   set callback function where to indicat the status of configure client process.
 *
 *  @param   f_status   this function should be implemented by application. see the definition: config_status
 *
 */
void cfg_cli_callback_set(config_status f_stauts);

/** @brief   to clear the whitlist in the controller
 *
 */
void mesh_clear_whitlist(void);

/** @brief   set the uuid filter, if need to provision other device
 *
 */
void uuid_filter_set(const uint8_t * uuid,uint8_t len);

/** @brief  call this API to revert proxy_gatt functionality if device supports.
 *
 *  @note   this func must be called in host task.
 */
void gap_mesh_gatt_restart(void);



void conf_feature_from_app(uint8_t role);


/** @brief this API used to judge mesh task is running or not
 *
 *  @return   true: mesh task     false: not mesh task
 *  @note  some APIs only take effects in the special task, be sure to the safe running environment, need to call this
 *         API to juduge which task is running now prior to calling the APIs.
 */
bool is_mesh_task(void);

/** @brief used to setup the mesh feature once power on.
 *
 *  @param support_bits : the supported feature this device need
 *
 *  @param enable_bits:   the bit set marks the feature enable , and the bit unset means the feature diabled.
 *
 *  @note  this API is used to configure a device support feature once it is provisioned. here are the bit definition refer to mesh_def.h
 *          #define BT_MESH_FEAT_RELAY                  BIT(0)
 *          #define BT_MESH_FEAT_PROXY                  BIT(1)
 *          #define BT_MESH_FEAT_FRIEND                 BIT(2)
 *          #define BT_MESH_FEAT_PROXY_SERVER           BIT(4)
 *
 *          BT_MESH_FEAT_LOW_POWER MUST not be configured with other feature due to low power functionality. if not any bit set , BT_MESH_FEAT_LOW_POWER is default option.
 */
void mesh_feature_set(uint16_t support_bits , uint16_t enable_bits);

/** @brief used to check the device  is provisioned or not.
 *
 */
bool bt_mesh_is_provisioned(void);

/** @brief internal used for filter mesh ADV after receiving from HOST
 *
 *  @param type : always is HCI_EVENT_PACKET
 *
 *  @param packet: the advertising data.
 *
 *  @param size: the size of advertising data.
 *
 *  @return  1: the package is discarded
 *           0: the package is passed to mesh stack.
 *
 *  @note  In order to decrease the memory allocation for the duplicated advertising packet of mesh package,the functionality will compare
 *         current advertising packet with last received advertising packet,if they are same ,then the current packet would 
 *         be dropped.
 */
uint8_t filtermeshADV(uint8_t type, uint8_t *packet, uint8_t size)  PRIVATE_FUNCTION;

/** @brief Initialize Mesh support
 *
 *  After calling this API, the node will not automatically advertise as
 *  unprovisioned, rather the bt_mesh_prov_enable() API needs to be called
 *  to enable unprovisioned advertising on one or more provisioning bearers.
 *
 *  @param own_addr_type Node address type
 *  @param prov Node provisioning information.
 *  @param comp Node Composition.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_init(u8_t own_addr_type, const struct bt_mesh_prov *prov, const struct bt_mesh_comp *comp)    PRIVATE_FUNCTION ;

/** @brief beacon functionality initialization
 *
 */
void bt_mesh_beacon_init(void)  PRIVATE_FUNCTION;

/** @brief called in mesh task.
 *  
 *  @note should call gap_beacon_disable instead of it ,if needed.
 */
void bt_mesh_beacon_disable(void) PRIVATE_FUNCTION;

/** @brief called in mesh task.
 *  
 *  @note should call gap_beacon_enable instead of it ,if needed.
 */
void bt_mesh_beacon_enable(void) PRIVATE_FUNCTION;

/** @brief configure parameters for different platform
 *  
 *  @note currently support Tianmao genie platform PB_ADV mode,and also support PB_GATT mode.
 */
void mesh_platform_setup(void);

/** @brief notify the attribut handle of database.
 *  
 */
void bt_mesh_handle_set(uint16_t prov_service, uint16_t prov_in, uint16_t prov_out, 
                        uint16_t proxy_service, uint16_t proxy_in, uint16_t proxy_out);

/**@brief a initialize for mesh AT commands environement
 *
 */
void ble_mesh_shell_init(void);

/**@brief configure the proxy service role 
 *
 * @param role  typed of svc_role;
 */
 typedef enum {
	MESH_GATT_NONE,
	MESH_GATT_PROV,
	MESH_GATT_PROXY,
}svc_role;
void bt_mesh_gatt_config(svc_role role) PRIVATE_FUNCTION;

/**@brief start the mesh proxy service advertising;
 *
 */
void mesh_start_advertising(void) PRIVATE_FUNCTION;

/**@brief get current node relay status
 *
 * @return  0: BT_MESH_RELAY_DISABLED
            1: BT_MESH_RELAY_ENABLED
            2: BT_MESH_RELAY_NOT_SUPPORTED
 */
u8_t bt_mesh_relay_get(void);

/**@brief get current node proxy status
 *
 * @return  0: BT_MESH_GATT_PROXY_DISABLED
            1: BT_MESH_GATT_PROXY_ENABLED
            2: BT_MESH_GATT_PROXY_NOT_SUPPORTED
 */
u8_t bt_mesh_gatt_proxy_get(void);

/**@brief get the platform, currently check whether it is genie platform
 *
 * @return  1: genie platform.
 */
uint8_t get_prov_platform(void);

/**@brief configure the beacon periodical timer parameters and advertising interval for unconnected advertising packets
 *
 * @param[in] unprov_inter  the periodical timer length to start unprov beacons sending
 *
 * @param[in] prov_inter    the periodical timer length to start secure beacon sending
 *
 * @param[in] adv_inter     the expected advertsing parameter interval for unconnected advertising.
 */
void beacon_param_config(uint32_t unprov_inter, uint32_t prov_inter, uint32_t adv_inter);
/*
* @}
*/
#endif
