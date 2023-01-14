#include "mesh_storage_app.h"
#include "mesh_storage_low_level.h"
#include "platform_api.h"
#include "eflash.h"
#include "app_debug.h"

// Flash distribution.
#define HAL_FLASH_BANK_SIZE     EFLASH_PAGE_SIZE  //For ingchips eflash, the true unit of bank is page, and one page = 8KB(0x2000). 
#define HAL_FLASH_BANK_0_ADDR   ((uint32_t)0x00070000)                                      //Bank for mesh stack.
#define HAL_FLASH_BANK_1_ADDR   ((uint32_t)(HAL_FLASH_BANK_0_ADDR + HAL_FLASH_BANK_SIZE))   //Bank for mesh stack.
#define HAL_FLASH_BANK_2_ADDR   ((uint32_t)(HAL_FLASH_BANK_1_ADDR + HAL_FLASH_BANK_SIZE))   //Bank for application info.


//////////////////////////////////////////////////////////////////////////////////////////
// addr
void mesh_gatt_addr_generate_and_get(bd_addr_t addr){
    if(!mesh_storage_is_gatt_addr_set()){
        // generate random gatt address.
        int rand = platform_rand();
        addr[5] = rand & 0xFF;
        addr[4] = (rand >> 8) & 0xFF;
        addr[3] = (rand >> 16) & 0xFF;
        addr[2] = (rand >> 24) & 0xFF;
        // write addr to database and flash.
        mesh_storage_gatt_addr_set(addr);
    } else {
        // read addr from database.
        mesh_storage_gatt_addr_get(addr);
    }

    app_log_debug("gatt_addr: ");
    app_log_debug_hexdump(addr, sizeof(bd_addr_t));
}

void mesh_beacon_addr_generate_and_get(bd_addr_t addr){
    if(!mesh_storage_is_beacon_addr_set()){
        // generate random beacon address.
        int rand = platform_rand();
        addr[5] = rand & 0xFF;
        addr[4] = (rand >> 8) & 0xFF;
        addr[3] = (rand >> 16) & 0xFF;
        addr[2] = (rand >> 24) & 0xFF;
        // write addr to database and flash.
        mesh_storage_beacon_addr_set(addr);
    } else {
        // read addr from database.
        mesh_storage_beacon_addr_get(addr);
    }

    app_log_debug("beacon_addr: ");
    app_log_debug_hexdump(addr, sizeof(bd_addr_t));
}

//////////////////////////////////////////////////////////////////////////////////////////
void mesh_generate_random_uuid(uint8_t * dev_uuid, uint16_t len){
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

    app_log_debug("dev uuid: ");
    app_log_debug_hexdump(dev_uuid, len);
}


//////////////////////////////////////////////////////////////////////////////////////////
void mesh_generate_random_name(uint8_t * name, uint16_t len){

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
            memcpy(name+len-8, line, 8);//change last 8 chars.
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
    app_log_debug("dev name[%d]: %s\n", len, name);
}

//////////////////////////////////////
void mesh_storage_app_init(void){
    mesh_storage_low_level_init(HAL_FLASH_BANK_2_ADDR);
}

void mesh_storage_app_reinit(void){
    mesh_storage_low_level_reinit();
    mesh_storage_app_init();
}

///////////////////////////////////////////////////////////
#include "hal_flash_bank_eflash.h"
#include "btstack_tlv_flash_bank.h"
static btstack_tlv_flash_bank_t  btstack_tlv_flash_bank_context;
static hal_flash_bank_eflash_t   hal_flash_bank_context;

// mesh stack storage init.
void mesh_storage_stack_init(void){
    
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
}



