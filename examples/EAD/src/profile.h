#ifndef _PROFILESTASK_H_
#define _PROFILESTASK_H_

#include <stdint.h>
#define BLUETOOTH_DATA_TYPE_FLAGS                                              0x01 // Flags
#define BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME                                0x09 // Complete Local Name
#define BLUETOOTH_DATA_TYPE_ENCRYPTED_ADVERTISING_DATA                         0x31 // Encrypted Advertising Data
uint32_t setup_profile(void *data, void *user_data);

#endif


