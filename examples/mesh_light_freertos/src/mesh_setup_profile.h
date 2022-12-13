#ifndef _MESH_SETUP_PROFILE_H_
#define _MESH_SETUP_PROFILE_H_

#include <stdint.h>
#include "bluetooth.h"

#define USER_MSG_ID_KEY_EVENT          (0x40) // key event.

void mesh_setup_profile(void);
void mesh_server_restart(void);
void mesh_setup_scan(void);
void mesh_scan_start(void);
void mesh_scan_stop(void);

#endif

