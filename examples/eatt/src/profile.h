#ifndef _PROFILESTASK_H_
#define _PROFILESTASK_H_


#include <stdint.h>
#define STORAGE_SIZE            (1024 * 4)
#define NUM_EATT_BEARERS        (5)
#define USER_MSG_START_EATT_CLIENT  1
#define USER_MSG_READ_CHAR          2
#define USER_MSG_WRITE_CHAR         3
#define USER_MSG_WRITE_NO_RSP_CHAR  4
#define USER_MSG_NOTIFY_CHAR        5
#define USER_MSG_INDICATE_CHAR      6
#define USER_MSG_DISCONNECT         7
#define USER_MSG_START_ADV          8
#define USER_MSG_START_SCONN        9
#define USER_MSG_PAIRING            10

uint32_t setup_profile(void *data, void *user_data);

#endif
