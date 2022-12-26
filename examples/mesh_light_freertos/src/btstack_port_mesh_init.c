
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
#include "mesh_port.h"
#include "btstack_tlv_flash_bank.h"
#include "mesh_health_server.h"
#include "mesh_generic_level_server.h"
#include "mesh_configuration_client.h"
#include "mesh_setup_profile.h"
#include "gap.h"
#include "mesh_storage.h"

// #define USE_DEFAULT_UUID

static char dev_name[] = "ing-mesh-xxxxxxxx";

#ifdef USE_DEFAULT_UUID
const char * device_uuid_string = "001BDC0810210B0E0A0C000B0E0A0C00";
#endif

// general
#define MESH_BLUEKITCHEN_MODEL_ID_TEST_SERVER   0x0000u

// static mesh_model_t                 mesh_vendor_model;

static mesh_model_t                 mesh_generic_on_off_server_model;
static mesh_generic_on_off_state_t  mesh_generic_on_off_state;

static mesh_health_fault_t          health_fault;
static mesh_publication_model_t     generic_on_off_server_publication;
static mesh_model_t                 mesh_generic_level_server_model;
static mesh_generic_level_state_t   mesh_generic_level_state;
static mesh_publication_model_t     generic_level_server_publication;
static mesh_model_t                 mesh_configuration_client_model;

static mesh_model_t                 mesh_generic_on_off_client_model;
static mesh_model_t                 mesh_generic_level_client_model;
static mesh_model_t                 mesh_light_lightness_server_model;
static mesh_model_t                 mesh_light_HSL_server_model;




static void mesh_provisioning_message_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
   UNUSED(packet_type);
   UNUSED(channel);
   UNUSED(size);

   if (packet_type != HCI_EVENT_PACKET) return;

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
                case MESH_SUBEVENT_STATE_UPDATE_BOOL:
                    printf("State update: model identifier 0x%08x, state identifier 0x%08x, reason %u, state %u\n",
                        mesh_subevent_state_update_bool_get_model_identifier(packet),
                        mesh_subevent_state_update_bool_get_state_identifier(packet),
                        mesh_subevent_state_update_bool_get_reason(packet),
                        mesh_subevent_state_update_bool_get_value(packet));

                    // Test code of controlling light state to on or off , optimize here later.
                    #include "rgb_led.h"
                    if(mesh_subevent_state_update_bool_get_value(packet)){
                        set_rgb_led_color(50, 50, 50);
                    } else {
                        set_rgb_led_color(0, 0, 0);
                    }

                    // mesh_scan_start(); //led control check if scan is started.
                    mesh_on_off_server_control_callback();
                    
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

#ifdef USE_DEFAULT_UUID

static int scan_hex_byte(const char * byte_string){
    int upper_nibble = nibble_for_char(*byte_string++);
    if (upper_nibble < 0) return -1;
    int lower_nibble = nibble_for_char(*byte_string);
    if (lower_nibble < 0) return -1;
    return (upper_nibble << 4) | lower_nibble;
}
static int btstack_parse_hex(const char * string, uint16_t len, uint8_t * buffer){
    int i;
    for (i = 0; i < len; i++) {
        int single_byte = scan_hex_byte(string);
        if (single_byte < 0) return 0;
        string += 2;
        buffer[i] = (uint8_t)single_byte;
        // don't check seperator after last byte
        if (i == len - 1) {
            return 1;
        }
        // optional seperator
        char separator = *string;
        if (separator == ':' && separator == '-' && separator == ' ') {
            string++;
        }
    }
    return 1;
}
#endif

#define RAND_BUF_MAX (1024)
#define RAND_BUF_MAX_INT (RAND_BUF_MAX / 4)
static int rand_buf[RAND_BUF_MAX_INT];
static uint8_t *m_p_rand_buf = (uint8_t*)rand_buf;
static int rand_buf_index = 0;
//static void init_rand_buf(void)
//{
//    for (rand_buf_index = 0; rand_buf_index < RAND_BUF_MAX_INT; rand_buf_index++) {
//        rand_buf[rand_buf_index] = platform_rand();
//    }
//    rand_buf_index = 0;
//    return;
//}
void m_platform_rand_8(uint8_t *p_dst)
{
    int i;
    for (i = 0; i < 8; i++) {
        p_dst[i] = m_p_rand_buf[rand_buf_index];
        rand_buf_index++;
        if (rand_buf_index >= RAND_BUF_MAX) {
            rand_buf_index = 0;
        }
    }
    return;
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

static void mesh_generate_random_uuid(uint8_t * dev_uuid, uint16_t len){
    if(len != 16)
        return;

    if(!mesh_storage_is_device_uuid_set()){
        // generate random beacon address.
        big_endian_store_32(dev_uuid, 0, (uint32_t)platform_rand());
        big_endian_store_32(dev_uuid, 4, (uint32_t)platform_rand());
        big_endian_store_32(dev_uuid, 8, (uint32_t)platform_rand());
        big_endian_store_32(dev_uuid, 12, (uint32_t)platform_rand());
        // write addr to database and flash.
        mesh_storage_device_uuid_set(dev_uuid, len);
    } else {
        // read addr from database.
        mesh_storage_device_uuid_get(dev_uuid, &len);
    }

    printf("dev uuid: ");
    printf_hexdump(dev_uuid, len);
}



static void mesh_generate_random_name(uint8_t * name, uint16_t len){

    if(!mesh_storage_is_name_set()){
        // generate random area.
        int rand = platform_rand();
        char line[8];
        int pos = 0;
        line[pos++] = char_for_nibble((rand >> 28) & 0x0f);
        line[pos++] = char_for_nibble((rand >> 24) & 0x0f);
        line[pos++] = char_for_nibble((rand >> 20) & 0x0f);
        line[pos++] = char_for_nibble((rand >> 16) & 0x0f);
        line[pos++] = char_for_nibble((rand >> 12) & 0x0f);
        line[pos++] = char_for_nibble((rand >>  8) & 0x0f);
        line[pos++] = char_for_nibble((rand >>  4) & 0x0f);
        line[pos++] = char_for_nibble((rand >>  0) & 0x0f);
        if(len > 8){
            memcpy(name+len-8, line, 8);
        } else {
            return;
        }
        
        // write name to database and flash.
        mesh_storage_name_set(name, len);
    } else {
        // read name from database.
        mesh_storage_name_get(name, &len);
    }

    name[len] = '\0';
    printf("dev name[%d]: %s\n", len, name);
}

void ble_port_generate_name_and_load_name(void){
    mesh_generate_random_name((uint8_t *)dev_name, strlen(dev_name));
    adv_bearer_adv_set_scan_rsp_data((uint8_t *)dev_name, strlen(dev_name));
}

void ble_port_generate_uuid_and_load_uuid(void){
    uint8_t device_uuid[16];

#ifdef USE_DEFAULT_UUID
    btstack_parse_hex(device_uuid_string, 16, device_uuid);// use default uuid.
#else
    mesh_generate_random_uuid(device_uuid, 16);// generate uuid.
#endif
    mesh_node_set_device_uuid(device_uuid);
}

#if 1
#include "hal_flash_bank_eflash.h"
#include "mesh_storage.h"
static btstack_tlv_flash_bank_t btstack_tlv_flash_bank_context;
static hal_flash_bank_eflash_t   hal_flash_bank_context;
#endif

void bt_port_mesh_setup(void)
{

    // setup TLV Flash Sector implementation
    const hal_flash_bank_t * hal_flash_bank_impl = NULL;
    hal_flash_bank_impl = hal_flash_bank_eflash_init_instance(
    		&hal_flash_bank_context,
    		HAL_FLASH_BANK_SIZE,
			HAL_FLASH_BANK_0_ADDR,
			HAL_FLASH_BANK_1_ADDR);
    const btstack_tlv_t * btstack_tlv_impl = btstack_tlv_flash_bank_init_instance(
    		&btstack_tlv_flash_bank_context,
			hal_flash_bank_impl,
			&hal_flash_bank_context);

    // setup global tlv
    btstack_tlv_set_instance(btstack_tlv_impl, &btstack_tlv_flash_bank_context);

    printf("bt_port_mesh_setup start\r\n");
    mesh_init();

#ifdef ENABLE_MESH_GATT_BEARER
    // setup connectable advertisments
    bd_addr_t null_addr;
    memset(null_addr, 0, 6);
    uint8_t adv_type = CONNECTABLE_ADV_BIT | SCANNABLE_ADV_BIT | LEGACY_PDU_BIT;   // connectable, scannable, legacy
    uint16_t adv_int_min = 0x0030;
    uint16_t adv_int_max = 0x0030;
    adv_bearer_advertisements_set_params(adv_int_min, adv_int_max, adv_type, BD_ADDR_TYPE_LE_PUBLIC, null_addr, PRIMARY_ADV_ALL_CHANNELS, ADV_FILTER_ALLOW_ALL);
    
    ble_port_generate_name_and_load_name();
#endif

    // Track Provisioning as device role
    mesh_register_provisioning_device_packet_handler((btstack_packet_handler_t)&mesh_provisioning_message_handler);

    // Loc - bottom - https://www.bluetooth.com/specifications/assigned-numbers/gatt-namespace-descriptors
    mesh_node_set_element_location(mesh_node_get_primary_element(), 0x0000);//lizhk modified. 0x0000=unknow, 0x0103=bottom,

    // Setup node info
    mesh_node_set_info(BLUETOOTH_COMPANY_ID_INGCHIPS_TECHNOLOGY_CO_LTD, 0, 0);

    // setup health server
    mesh_health_server_add_fault_state(mesh_node_get_health_server(), BLUETOOTH_COMPANY_ID_INGCHIPS_TECHNOLOGY_CO_LTD, &health_fault);



    // Setup Configuration Client model
    mesh_configuration_client_model.model_identifier = mesh_model_get_model_identifier_bluetooth_sig(MESH_SIG_MODEL_ID_CONFIGURATION_CLIENT);
    mesh_configuration_client_register_packet_handler(&mesh_configuration_client_model, (btstack_packet_handler_t)&mesh_configuration_message_handler);
    mesh_element_add_model(mesh_node_get_primary_element(), &mesh_configuration_client_model);

    // Setup Generic On/Off server model
    mesh_generic_on_off_server_model.model_identifier = mesh_model_get_model_identifier_bluetooth_sig(MESH_SIG_MODEL_ID_GENERIC_ON_OFF_SERVER);
    mesh_generic_on_off_server_model.operations = mesh_generic_on_off_server_get_operations();
    mesh_generic_on_off_server_model.model_data = (void *) &mesh_generic_on_off_state;
    mesh_generic_on_off_server_register_packet_handler(&mesh_generic_on_off_server_model, (btstack_packet_handler_t)&mesh_state_update_message_handler);
    mesh_generic_on_off_server_set_publication_model(&mesh_generic_on_off_server_model, &generic_on_off_server_publication);
    mesh_element_add_model(mesh_node_get_primary_element(), &mesh_generic_on_off_server_model);

    // Setup Generic On/Off client model
    mesh_generic_on_off_client_model.model_identifier = mesh_model_get_model_identifier_bluetooth_sig(MESH_SIG_MODEL_ID_GENERIC_ON_OFF_CLIENT);
    mesh_configuration_client_register_packet_handler(&mesh_generic_on_off_client_model, (btstack_packet_handler_t)&mesh_state_update_message_handler);
    mesh_element_add_model(mesh_node_get_primary_element(), &mesh_generic_on_off_client_model);

    // Setup Generic level server model
    mesh_generic_level_server_model.model_identifier = mesh_model_get_model_identifier_bluetooth_sig(MESH_SIG_MODEL_ID_GENERIC_LEVEL_SERVER);
    mesh_generic_level_server_model.operations = mesh_generic_level_server_get_operations();
    mesh_generic_level_server_model.model_data = (void *) &mesh_generic_level_state;
    mesh_generic_level_server_register_packet_handler(&mesh_generic_level_server_model, (btstack_packet_handler_t)&mesh_state_update_message_handler);
    mesh_generic_level_server_set_publication_model(&mesh_generic_level_server_model, &generic_level_server_publication);
    mesh_element_add_model(mesh_node_get_primary_element(), &mesh_generic_level_server_model);

    // Setup Generic level client model
    mesh_generic_level_client_model.model_identifier = mesh_model_get_model_identifier_bluetooth_sig(MESH_SIG_MODEL_ID_GENERIC_LEVEL_CLIENT);
    mesh_configuration_client_register_packet_handler(&mesh_generic_level_client_model, (btstack_packet_handler_t)&mesh_state_update_message_handler);
    mesh_element_add_model(mesh_node_get_primary_element(), &mesh_generic_level_client_model);

    // Setup Light lightness server model
    mesh_light_lightness_server_model.model_identifier = mesh_model_get_model_identifier_bluetooth_sig(0x1300u);
    mesh_light_lightness_server_model.operations = mesh_generic_level_server_get_operations();
    mesh_light_lightness_server_model.model_data = (void *) &mesh_generic_level_state;//change later
    mesh_generic_level_server_register_packet_handler(&mesh_light_lightness_server_model, (btstack_packet_handler_t)&mesh_state_update_message_handler);
    mesh_generic_level_server_set_publication_model(&mesh_light_lightness_server_model, &generic_level_server_publication);//change later
    mesh_element_add_model(mesh_node_get_primary_element(), &mesh_light_lightness_server_model);

    // Setup Light HSL server model
    mesh_light_HSL_server_model.model_identifier = mesh_model_get_model_identifier_bluetooth_sig(0x1307u);
    mesh_light_HSL_server_model.operations = mesh_generic_level_server_get_operations();
    mesh_light_HSL_server_model.model_data = (void *) &mesh_generic_level_state;//change later
    mesh_generic_level_server_register_packet_handler(&mesh_light_HSL_server_model, (btstack_packet_handler_t)&mesh_state_update_message_handler);
    mesh_generic_level_server_set_publication_model(&mesh_light_HSL_server_model, &generic_level_server_publication);//change later
    mesh_element_add_model(mesh_node_get_primary_element(), &mesh_light_HSL_server_model);

    // Setup our custom model
    // mesh_vendor_model.model_identifier = mesh_model_get_model_identifier(BLUETOOTH_COMPANY_ID_INGCHIPS_TECHNOLOGY_CO_LTD, MESH_BLUEKITCHEN_MODEL_ID_TEST_SERVER);
    // mesh_element_add_model(mesh_node_get_primary_element(), &mesh_vendor_model);

    // Enable Output OOB
    // provisioning_device_set_output_oob_actions(0x08, 0x08);

    // Enable PROXY and RELAY
    mesh_foundation_gatt_proxy_set(1);
    mesh_foundation_relay_set(1);

    // Set uuid.
    ble_port_generate_uuid_and_load_uuid();

    //btstack_run_loop_execute();

    printf("bt_port_mesh_setup end\r\n");
}
