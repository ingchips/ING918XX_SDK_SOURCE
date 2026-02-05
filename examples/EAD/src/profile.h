#ifndef _PROFILESTASK_H_
#define _PROFILESTASK_H_

#include <stdint.h>
#define BLUETOOTH_DATA_TYPE_FLAGS                                              0x01 // Flags
#define BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME                                0x09 // Complete Local Name
uint32_t setup_profile(void *data, void *user_data);

// EAD encryption function
void ead_encrypt(const uint8_t *session_key, const uint8_t *iv, const uint8_t *aad,
                 const uint8_t *payload, uint16_t payload_size, uint8_t *encrypted_payload);

#endif


