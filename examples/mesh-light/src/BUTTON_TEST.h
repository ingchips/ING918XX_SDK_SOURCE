#ifndef _BUTTON_TEST_H_
#define _BUTTON_TEST_H_

#include <stdint.h>
#include "bluetooth.h"
#include "btstack_event.h"

#define USER_MSG_ID_KEY_PRESSED_EVENT          (0x40) // key pressed event.
#define USER_MSG_ID_KEY_RELEASED_EVENT         (0x41) // key released event.


void button_msg_handler(btstack_user_msg_t * usrmsg);

void button_test_init(void);

#endif

