
#include <stdint.h>
#include <stdio.h>

#include "mesh/mesh.h"
#include "mesh/mesh_node.h"
#include "mesh/mesh_generic_model.h"
#include "mesh/mesh_foundation.h"
#include "mesh/mesh_generic_on_off_server.h"
#include "mesh/provisioning_device.h"
#include "btstack_util.h"
#include "btstack_tlv.h"
#include "mesh_port_stack.h"
#include "platform_api.h"
#include "btstack_tlv_flash_bank.h"
#include "mesh_health_server.h"
#include "mesh_generic_level_server.h"
#include "mesh_configuration_client.h"
#include "mesh_configuration_server.h"
#include "gap.h"
#include "mesh_storage_app.h"
#include "app_debug.h"
#include "profile.h"
#include "app_config.h"
#include "mesh_port_low_level_init.h"
#include "mesh_profile.h"


#ifdef USE_MESH_APP_FLASH
#define MESH_UUID_USE_FLASH
#define MESH_NAME_USE_FLASH
#define MESH_GATT_ADV_ADDR_USE_FLASH
#define MESH_BEACON_ADV_ADDR_USE_FLASH
#endif


static const bt_mesh_prov_t *pMesh_prov = NULL;

// general
static mesh_generic_on_off_state_t  mesh_generic_on_off_state;

static mesh_health_fault_t          health_fault;
static mesh_publication_model_t     generic_on_off_server_publication;

static mesh_generic_level_state_t   mesh_generic_level_state;
static mesh_publication_model_t     generic_level_server_publication;

// Mandatory Confiuration Server 
static mesh_model_t                 *pMesh_configuration_server_model = NULL;
static mesh_configuration_server_model_context_t mesh_configuration_server_model_context;

// Mandatory Health Server
static mesh_publication_model_t     mesh_health_server_publication;
static mesh_health_state_t          mesh_health_server_model_context;


static void mesh_provisioning_message_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    UNUSED(packet_type);
    UNUSED(channel);
    UNUSED(size);

    if (packet_type != HCI_EVENT_PACKET) return;
    
    mesh_provisioning_data_t provisioning_data;

    switch(packet[0]){
        case HCI_EVENT_MESH_META:
            switch(packet[2]){
                case MESH_SUBEVENT_PB_TRANSPORT_LINK_OPEN:
                    printf("Provisioner link opened");
                    break;
                case MESH_SUBEVENT_ATTENTION_TIMER:
                    printf("Attention Timer: %u\n", mesh_subevent_attention_timer_get_attention_time(packet));
                    break;
                case MESH_SUBEVENT_PB_TRANSPORT_LINK_CLOSED:
                    printf("Provisioner link close");
                    break;
                case MESH_SUBEVENT_PB_PROV_COMPLETE:
                    printf("Provisioning complete\n");
                    provisioning_device_data_get(&provisioning_data);
                    app_assert(pMesh_prov != NULL);
                    pMesh_prov->complete(provisioning_data.network_key->netkey_index, provisioning_data.unicast_address);
                    break;
                case MESH_SUBEVENT_PB_PROV_START_EMIT_OUTPUT_OOB:{
                        uint16_t pb_trasport_cid = mesh_subevent_pb_prov_start_emit_output_oob_get_pb_transport_cid(packet);
                        uint32_t output_oob = mesh_subevent_pb_prov_start_emit_output_oob_get_output_oob(packet);
                        // printf("pb_trasport_cid: %u, output_oob: %u \n", pb_trasport_cid, output_oob);  
                        app_assert(pMesh_prov != NULL);                  
                        pMesh_prov->output_number(output_oob);
                    }break;                
                case MESH_SUBEVENT_PB_PROV_INPUT_OOB_REQUEST:
                    app_assert(pMesh_prov != NULL);
                    pMesh_prov->input_req();
                    break;
                case MESH_SUBEVENT_NODE_RESET:
                    mesh_port_node_reset_handler();
                    app_assert(pMesh_prov != NULL);
                    pMesh_prov->reset();
                    break;
                default:
                    break;
            }
            break;
        default:
           break;
    }
}

static void mesh_state_update_message_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    UNUSED(channel);
    UNUSED(size);

    if (packet_type != HCI_EVENT_PACKET) return;

    switch(packet[0]){
        case HCI_EVENT_MESH_META:
            switch(packet[2]){
                case MESH_SUBEVENT_STATE_UPDATE_BOOL:{
                        printf("State update: model identifier 0x%08x, state identifier 0x%08x, reason %u, state %u\n",
                            mesh_subevent_state_update_bool_get_model_identifier(packet),
                            mesh_subevent_state_update_bool_get_state_identifier(packet),
                            mesh_subevent_state_update_bool_get_reason(packet),
                            mesh_subevent_state_update_bool_get_value(packet));
                        
                        uint8_t element_index = mesh_subevent_state_update_bool_get_element_index(packet);
                        uint32_t model_identifier = mesh_subevent_state_update_bool_get_model_identifier(packet);
                        uint8_t state = mesh_subevent_state_update_bool_get_value(packet);
                        mesh_model_t * model = mesh_model_get_by_identifier(mesh_node_element_for_index(element_index), model_identifier);
                        if ( mesh_model_is_bluetooth_sig(model_identifier) ) {
                            switch( mesh_model_get_model_id(model_identifier) )
                            {
                                case MESH_SIG_MODEL_ID_GENERIC_ON_OFF_SERVER:{                                
                                        // Set Generic On/Off state
                                        mesh_on_off_server_control_callback();
                                    }break;
                                
                                default:
                                    break;
                            }
                        }
                    }break;
                case MESH_SUBEVENT_STATE_UPDATE_INT16:{
                        printf("int16 update: model identifier 0x%08x, state identifier 0x%08x, reason %u, value %u\n",
                            mesh_subevent_state_update_int16_get_model_identifier(packet),
                            mesh_subevent_state_update_int16_get_state_identifier(packet),
                            mesh_subevent_state_update_int16_get_reason(packet),
                            mesh_subevent_state_update_int16_get_value(packet));
                        
                        uint8_t element_index = mesh_subevent_state_update_int16_get_element_index(packet);
                        uint32_t model_identifier = mesh_subevent_state_update_int16_get_model_identifier(packet);
                        int16_t value = mesh_subevent_state_update_int16_get_value(packet);
                        mesh_model_t * model = mesh_model_get_by_identifier(mesh_node_element_for_index(element_index), model_identifier);
                        if ( mesh_model_is_bluetooth_sig(model_identifier) ) {
                            switch( mesh_model_get_model_id(model_identifier) )
                            {
                                case MESH_SIG_MODEL_ID_GENERIC_LEVEL_SERVER:{                                
                                        // Set Generic level
                                        mesh_on_off_server_control_callback();
                                    }break;
                                
                                default:
                                    break;
                            }
                        }
                    }break; 
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

static void mesh_configuration_message_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    UNUSED(channel);
    UNUSED(size);

    if (packet_type != HCI_EVENT_PACKET) return;

    switch(packet[0]){
        case HCI_EVENT_MESH_META:
            switch(packet[2]){
                default:
                    printf("mesh_configuration_message_handler: event not parsed");
                    break;
            }
            break;
        default:
            break;
    }
}

void mesh_platform_adv_params_init(void)
{
#ifdef ENABLE_MESH_GATT_BEARER
    // setup connectable advertisments
    bd_addr_t null_addr;
    memset(null_addr, 0, 6);
    uint8_t adv_type = CONNECTABLE_ADV_BIT | SCANNABLE_ADV_BIT | LEGACY_PDU_BIT;   // connectable, scannable, legacy
    uint16_t adv_int_min = 0x0030;
    uint16_t adv_int_max = 0x0030;
    adv_bearer_advertisements_set_params(adv_int_min, adv_int_max, adv_type, BD_ADDR_TYPE_LE_PUBLIC, null_addr, PRIMARY_ADV_ALL_CHANNELS, ADV_FILTER_ALLOW_ALL);
    
#endif
    
}

void mesh_platform_config(bt_mesh_config_t type, void *data, uint32_t data_len){
    
    switch(type)
    {
        case MESH_CFG_NAME:{            
#ifdef MESH_NAME_USE_FLASH
            char dev_name[29]; //"ing-mesh-xxxxxxxx"
            uint32_t name_len = (data_len + 9); //add random tail: -xxxxxxxx , len: 9bytes.
            memset(dev_name, 0, sizeof(dev_name));
            app_assert(name_len <= sizeof(dev_name));
            memcpy(dev_name, (uint8_t *)data, data_len);            
            mesh_generate_random_name((uint8_t *)dev_name, name_len);
            adv_bearer_adv_set_scan_rsp_data((uint8_t *)dev_name, name_len);
#else
            app_assert(data_len <= 29);
            adv_bearer_adv_set_scan_rsp_data((uint8_t *)data, data_len);
#endif
        }break;
        
        case MESH_CFG_GATT_ADV_ADDR:{
#ifdef MESH_GATT_ADV_ADDR_USE_FLASH
            bd_addr_t addr;
            mesh_gatt_addr_generate_and_get(addr);
            mesh_gatt_adv_addr_set(addr);
#else
            mesh_gatt_adv_addr_set((uint8_t *) data);
#endif
        }break;
        
        case MESH_CFG_BEACON_ADV_ADDR:{
#ifdef MESH_BEACON_ADV_ADDR_USE_FLASH
            bd_addr_t addr;
            mesh_beacon_addr_generate_and_get(addr);
            mesh_beacon_adv_addr_set(addr);
#else
            mesh_beacon_adv_addr_set((uint8_t *) data);
#endif
        }break;
        
        default:{
        }break;
    }
}

int bt_mesh_input_string(const uint8_t * str, uint16_t len){
    provisioning_device_input_oob_complete_alphanumeric(1, str, len);
    return 0;
}

int bt_mesh_input_number(uint32_t num){
    provisioning_device_input_oob_complete_numeric(1, num);
    return 0;
}

void mesh_prov_ll_init(const bt_mesh_prov_t *prov)
{
    pMesh_prov = prov;
    
    // Track Provisioning as device role
    mesh_register_provisioning_device_packet_handler((btstack_packet_handler_t)&mesh_provisioning_message_handler);
    
    // Config Public Key OOB
    if(prov->public_key != NULL && prov->private_key != NULL)
        provisioning_device_set_public_key_oob(prov->public_key, prov->private_key);
    // Config Static OOB
    if(prov->static_oob_len)
        provisioning_device_set_static_oob(prov->static_oob_len, prov->static_oob_data);
    // Config Output OOB
    if(prov->output_oob_max_size)
        provisioning_device_set_output_oob_actions(prov->output_oob_action, prov->output_oob_max_size);
    // Config Input OOB
    if(prov->input_oob_max_size)
        provisioning_device_set_input_oob_actions(prov->input_oob_action, prov->input_oob_max_size);
    
    // Set uuid.    
#ifdef MESH_UUID_USE_FLASH
    uint8_t rand_uuid[16];
    mesh_generate_random_uuid(rand_uuid, 16);// generate random uuid and store.
    mesh_node_set_device_uuid(rand_uuid);
#else
    app_assert(pMesh_prov != NULL);
    mesh_node_set_device_uuid(pMesh_prov->uuid);
#endif
    
}

mesh_model_t * get_configuration_server_model(void){
    app_assert(pMesh_configuration_server_model != NULL);
    return pMesh_configuration_server_model;
}

static void mesh_model_foundation_init(mesh_model_t * model){
    bt_mesh_cfg_srv_t *pCfg_srv = (bt_mesh_cfg_srv_t *)model->user_data;
    mesh_foundation_relay_set(pCfg_srv->relay);
    mesh_foundation_gatt_proxy_set(pCfg_srv->gatt_proxy);
    mesh_foundation_friend_set(pCfg_srv->frnd);
    mesh_foundation_low_power_set(pCfg_srv->low_pwr);
    mesh_foundation_beacon_set(pCfg_srv->beacon);
    mesh_foundation_default_ttl_set(pCfg_srv->default_ttl);
    mesh_foundation_network_transmit_set(pCfg_srv->net_transmit);
    mesh_foundation_relay_retransmit_set(pCfg_srv->relay_retransmit);
}

static void mesh_sig_models_init(mesh_element_t * element, mesh_model_t * model, const node_info_t *info){    
    if ( !mesh_model_is_bluetooth_sig(model->model_identifier) ) 
        return;
    
    switch( mesh_model_get_model_id(model->model_identifier) )
    {
        case MESH_SIG_MODEL_ID_CONFIGURATION_SERVER:
            // configure Config Server
            model->operations = mesh_configuration_server_get_operations();   
            model->model_data = (void *) &mesh_configuration_server_model_context;
            mesh_element_add_model(element, model);
            pMesh_configuration_server_model = model;
            app_assert(pMesh_configuration_server_model != NULL);
            mesh_model_foundation_init(model);
            break;
        
        case MESH_SIG_MODEL_ID_HEALTH_SERVER:
            // Config Health Server
            model->model_data       = &mesh_health_server_model_context;
            model->operations       = mesh_health_server_get_operations();
            model->publication_model = &mesh_health_server_publication;
            mesh_element_add_model(element, model);
            mesh_health_server_set_publication_model(model, &mesh_health_server_publication);
            // setup health server
            mesh_health_server_add_fault_state(model, info->cid, &health_fault);
            break;
        
        case MESH_SIG_MODEL_ID_CONFIGURATION_CLIENT:
            // Setup Configuration Client model
            mesh_configuration_client_register_packet_handler(model, (btstack_packet_handler_t)&mesh_configuration_message_handler);
            mesh_element_add_model(element, model);
            break;
        
        case MESH_SIG_MODEL_ID_GENERIC_ON_OFF_SERVER:
            // Setup Generic On/Off server model
            model->operations = mesh_generic_on_off_server_get_operations();
            model->model_data = (void *) &mesh_generic_on_off_state;
            mesh_generic_on_off_server_register_packet_handler(model, (btstack_packet_handler_t)&mesh_state_update_message_handler);
            mesh_generic_on_off_server_set_publication_model(model, &generic_on_off_server_publication);
            mesh_element_add_model(element, model);
            break;
        
        case MESH_SIG_MODEL_ID_GENERIC_ON_OFF_CLIENT:
            // Setup Generic On/Off client model
            mesh_configuration_client_register_packet_handler(model, (btstack_packet_handler_t)&mesh_configuration_message_handler);
            mesh_element_add_model(element, model);
            break;
        
        case MESH_SIG_MODEL_ID_GENERIC_LEVEL_SERVER:
            // Setup Generic level server model
            model->operations = mesh_generic_level_server_get_operations();
            model->model_data = (void *) &mesh_generic_level_state;
            mesh_generic_level_server_register_packet_handler(model, (btstack_packet_handler_t)&mesh_state_update_message_handler);
            mesh_generic_level_server_set_publication_model(model, &generic_level_server_publication);
            mesh_element_add_model(element, model);
            break;
        
        case MESH_SIG_MODEL_ID_GENERIC_LEVEL_CLIENT:
            // Setup Generic level client model
            mesh_configuration_client_register_packet_handler(model, (btstack_packet_handler_t)&mesh_configuration_message_handler);
            mesh_element_add_model(element, model);
            break;
        
        case MESH_SIG_MODEL_ID_LIGHT_LIGHTNESS_SERVER: //Meshlib not support this model, add later.
            // Setup Light lightness server model
            mesh_element_add_model(element, model);
            break;
        
        case MESH_SIG_MODEL_ID_LIGHT_HSL_SERVER: //Meshlib not support this model, add later.
            // Setup Light HSL server model
            mesh_element_add_model(element, model);
            break;
        
        default:
            printf("#error: unknown sig model id: 0x%04X\n", mesh_model_get_model_id(model->model_identifier));
            break;
    }
}

static void mesh_vendor_models_init(mesh_element_t * element, mesh_model_t * model, const node_info_t *info){   
    // check vendor id valid.
    if ( mesh_model_is_bluetooth_sig(model->model_identifier) ) 
        return;
    
    switch( mesh_model_get_model_id(model->model_identifier) )
    {
        case INGCHIPS_VND_ID_1:
            // Setup vendor model
            mesh_element_add_model(element, model);
            break;
        default:
            printf("#error: unknown vendor model id: 0x%04X\n", mesh_model_get_model_id(model->model_identifier));
            break;
    }
}

void mesh_elems_and_models_ll_init(const bt_mesh_comp_t *a_comp){
    
    printf("#elem_count: %d\n", a_comp->elem_count);
    
    // check element valid.
    if (a_comp->elem_count == 0) return;
    
    const node_info_t *info = &a_comp->info;
    
    // setup basic information.
    mesh_node_set_info(a_comp->info.cid, a_comp->info.pid, a_comp->info.vid);
    
    // setup primary element pointer.
    mesh_primary_elem_init(&a_comp->elem[0]);
    
    // add element to mesh stack.
    size_t i,j;
    for(i=0; i<a_comp->elem_count; i++){
        // add element.
        mesh_node_add_element(&a_comp->elem[i]);
        
        /* add models to element. */
        mesh_element_t *pElement = &a_comp->elem[i];
        uint16_t loc = pElement->loc;
        mesh_model_t *pSigModel = pElement->sig_models;
        mesh_model_t *pVndModel = pElement->vnd_models;
        
        // init location.
        mesh_node_set_element_location(pElement, loc);
        
        printf("models_count_sig: %d, models_count_vendor: %d\n", pElement->models_count_sig, pElement->models_count_vendor);
        
        // add sig models to element.
        if(pElement->models_count_sig){
            for(j=0; j<pElement->models_count_sig; j++){
                mesh_sig_models_init(pElement, &pSigModel[j], info);
            }
        }
        
        // add vendor models to element.
        if(pElement->models_count_vendor){
            for(j=0; j<pElement->models_count_vendor; j++){
                mesh_vendor_models_init(pElement, &pVndModel[j], info);
            }
        }
    }
}
