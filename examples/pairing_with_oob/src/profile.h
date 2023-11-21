#ifndef _PROFILESTASK_H_
#define _PROFILESTASK_H_

#include <stdint.h>
#include "bluetooth.h"

#ifndef ROLE
#define ROLE        HCI_ROLE_SLAVE
#endif

uint32_t setup_profile(void *data, void *user_data);

#endif


