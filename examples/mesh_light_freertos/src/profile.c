#include "profile.h"
#include "platform_api.h"
#include "mesh_storage.h"
#include "mesh.h"
#include "mesh_port_stack.h"
#include "mesh_model.h"

static void mesh_flash_init(void){
    mesh_storage_init();
    mesh_stack_storage_init();
}

void mesh_elements_init(void){
    mesh_elems_model_init();
}

extern void mesh_platform_config(void);
static void mesh_platform_init(void){
    // name, addr.
    mesh_platform_config();
    
}

extern void mesh_prov_config(void);
static void mesh_provising_init(void){
    mesh_port_init();
    
}

void mesh_init(void){
    platform_printf("mesh start.\n");
    mesh_flash_init();
    mesh_stack_init(&mesh_elements_init);
    mesh_platform_init();
    mesh_provising_init();
}

