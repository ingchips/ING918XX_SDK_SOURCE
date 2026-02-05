#ifndef _AES_CCM_H_
#define _AES_CCM_H_

#include <stdint.h>

// AES-CCM encryption
// Returns 1 on success, 0 on failure
int aes128_ccm_encrypt(const uint8_t *key, const uint8_t *nonce, uint8_t nonce_len,
                       const uint8_t *aad, uint16_t aad_len,
                       uint16_t msg_len,
                       const uint8_t *msg, uint8_t *encrypted_msg,
                       uint8_t *mic, uint8_t mic_len);

// AES-CCM decryption
// Returns 1 on success (MAC verified), 0 on failure
int aes128_ccm_decrypt(const uint8_t *key, const uint8_t *nonce, uint8_t nonce_len,
                       const uint8_t *aad, uint16_t aad_len,
                       uint16_t msg_len,
                       const uint8_t *encrypted_msg, uint8_t *msg,
                       const uint8_t *mic, uint8_t mic_len);

#endif // _AES_CCM_H_
