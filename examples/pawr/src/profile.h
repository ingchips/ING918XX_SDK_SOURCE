#ifndef _PROFILESTASK_H_
#define _PROFILESTASK_H_

#include <stdint.h>

// PAwR advertiser (when connected, it is Central)
#define ROLE_ADVERTISER     0

// PAwR scanner/receiver/responder (when connected, it is Peripheral)
#define ROLE_SCANNER        1

#define INVALID_HANDLE          0xffff
#define DATA_COUNTER_OFFSET     4

uint32_t setup_profile(void *data, void *user_data);

#endif


