#ifndef _MESH_PORT_LOW_LEVEL_INIT_H_
#define _MESH_PORT_LOW_LEVEL_INIT_H_

#include <stdint.h>
#include "bt_types.h"
#include "profile.h"

void mesh_elems_and_models_ll_init(const bt_mesh_comp_t *a_comp);
void mesh_prov_ll_init(const bt_mesh_prov_t *prov);

#endif

