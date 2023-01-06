#ifndef __TYPE_DEF_H
#define __TYPE_DEF_H
#include "bt_types.h"

typedef struct DTBT_msg
{
	 btstack_linked_item_t  item;
	 uint8_t  data[HCI_ACL_PAYLOAD_SIZE+4];    //4 means HCI head size
} DTBT_msg_t;
#endif

