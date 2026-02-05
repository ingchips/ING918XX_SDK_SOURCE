#include "aes_ccm.h"
#include <string.h>

// AES-128 S-box
static const uint8_t sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

// AES-128 Round constant
static const uint8_t rcon[11] = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

// AES key expansion for 128-bit key
static void aes_key_expansion(const uint8_t *key, uint8_t *round_key)
{
    uint8_t temp[4];
    uint8_t i, k;
    
    // First round key is the key itself
    for (i = 0; i < 16; i++) {
        round_key[i] = key[i];
    }
    
    // Generate remaining 10 round keys
    for (i = 1; i <= 10; i++) {
        // RotWord
        temp[0] = round_key[16 * i - 3];
        temp[1] = round_key[16 * i - 2];
        temp[2] = round_key[16 * i - 1];
        temp[3] = round_key[16 * i - 4];
        
        // SubWord
        for (k = 0; k < 4; k++) {
            temp[k] = sbox[temp[k]];
        }
        
        // XOR with Rcon
        temp[0] ^= rcon[i];
        
        // Generate new round key
        for (k = 0; k < 4; k++) {
            round_key[16 * i + k] = round_key[16 * (i - 1) + k] ^ temp[k];
        }
        for (k = 4; k < 16; k++) {
            round_key[16 * i + k] = round_key[16 * i + k - 4] ^ round_key[16 * (i - 1) + k];
        }
    }
}

// SubBytes transformation
static void sub_bytes(uint8_t *state)
{
    uint8_t i;
    for (i = 0; i < 16; i++) {
        state[i] = sbox[state[i]];
    }
}

// ShiftRows transformation
static void shift_rows(uint8_t *state)
{
    uint8_t temp;
    
    // Row 1: shift left by 1
    temp = state[1];
    state[1] = state[5];
    state[5] = state[9];
    state[9] = state[13];
    state[13] = temp;
    
    // Row 2: shift left by 2
    temp = state[2];
    state[2] = state[10];
    state[10] = temp;
    temp = state[6];
    state[6] = state[14];
    state[14] = temp;
    
    // Row 3: shift left by 3
    temp = state[15];
    state[15] = state[11];
    state[11] = state[7];
    state[7] = state[3];
    state[3] = temp;
}

// GF(2^8) multiplication
static uint8_t gf_mul(uint8_t a, uint8_t b)
{
    uint8_t p = 0;
    uint8_t i;
    
    for (i = 0; i < 8; i++) {
        if (b & 1) {
            p ^= a;
        }
        uint8_t hi_bit_set = a & 0x80;
        a <<= 1;
        if (hi_bit_set) {
            a ^= 0x1b; // x^8 + x^4 + x^3 + x + 1
        }
        b >>= 1;
    }
    return p;
}

// MixColumns transformation
static void mix_columns(uint8_t *state)
{
    uint8_t i, a, b, c, d;
    
    for (i = 0; i < 4; i++) {
        a = state[i * 4];
        b = state[i * 4 + 1];
        c = state[i * 4 + 2];
        d = state[i * 4 + 3];
        
        state[i * 4]     = gf_mul(a, 2) ^ gf_mul(b, 3) ^ c ^ d;
        state[i * 4 + 1] = a ^ gf_mul(b, 2) ^ gf_mul(c, 3) ^ d;
        state[i * 4 + 2] = a ^ b ^ gf_mul(c, 2) ^ gf_mul(d, 3);
        state[i * 4 + 3] = gf_mul(a, 3) ^ b ^ c ^ gf_mul(d, 2);
    }
}

// AddRoundKey transformation
static void add_round_key(uint8_t *state, const uint8_t *round_key)
{
    uint8_t i;
    for (i = 0; i < 16; i++) {
        state[i] ^= round_key[i];
    }
}

// AES-128 encryption of a single 16-byte block
static void aes_encrypt_block(const uint8_t *plaintext, const uint8_t *key, uint8_t *ciphertext)
{
    uint8_t state[16];
    uint8_t round_key[176]; // 11 round keys * 16 bytes
    uint8_t round;
    
    // Copy input to state
    memcpy(state, plaintext, 16);
    
    // Key expansion
    aes_key_expansion(key, round_key);
    
    // Initial round
    add_round_key(state, round_key);
    
    // Main rounds (9 rounds for AES-128)
    for (round = 1; round < 10; round++) {
        sub_bytes(state);
        shift_rows(state);
        mix_columns(state);
        add_round_key(state, round_key + 16 * round);
    }
    
    // Final round (no MixColumns)
    sub_bytes(state);
    shift_rows(state);
    add_round_key(state, round_key + 160);
    
    // Copy state to output
    memcpy(ciphertext, state, 16);
}

// XOR two blocks
static void xor_block(uint8_t *dst, const uint8_t *src, uint8_t len)
{
    uint8_t i;
    for (i = 0; i < len; i++) {
        dst[i] ^= src[i];
    }
}

// AES-CCM encryption
// Returns 1 on success, 0 on failure
int aes128_ccm_encrypt(const uint8_t *key, const uint8_t *nonce, uint8_t nonce_len,
                       const uint8_t *aad, uint16_t aad_len,
                       uint16_t msg_len,
                       const uint8_t *msg, uint8_t *encrypted_msg,
                       uint8_t *mic, uint8_t mic_len)
{
    uint8_t b[16], a[16], s[16];
    uint16_t i, len;
    uint8_t L;
    
    // L is the size of the length field in octets (15 - nonce_len)
    L = 15 - nonce_len;
    
    // === Step 1: Compute CBC-MAC (Authentication) ===
    
    // Build B_0: Flags | Nonce | Length
    memset(b, 0, 16);
    b[0] = (aad_len > 0 ? 0x40 : 0) | ((mic_len - 2) / 2 << 3) | (L - 1);
    memcpy(b + 1, nonce, nonce_len);
    
    // Encode message length in last L bytes
    for (i = 0; i < L; i++) {
        b[15 - i] = (msg_len >> (8 * i)) & 0xff;
    }
    
    // Initialize MAC with B_0
    aes_encrypt_block(b, key, b);
    
    // Add AAD to MAC if present
    if (aad_len > 0) {
        memset(a, 0, 16);
        // Encode AAD length
        a[0] = (aad_len >> 8) & 0xff;
        a[1] = aad_len & 0xff;
        
        // Add AAD data
        len = aad_len < 14 ? aad_len : 14;
        memcpy(a + 2, aad, len);
        xor_block(b, a, 16);
        aes_encrypt_block(b, key, b);
        
        // Process remaining AAD blocks
        for (i = len; i < aad_len; i += 16) {
            memset(a, 0, 16);
            len = (aad_len - i) < 16 ? (aad_len - i) : 16;
            memcpy(a, aad + i, len);
            xor_block(b, a, 16);
            aes_encrypt_block(b, key, b);
        }
    }
    
    // Add message to MAC
    for (i = 0; i < msg_len; i += 16) {
        memset(a, 0, 16);
        len = (msg_len - i) < 16 ? (msg_len - i) : 16;
        memcpy(a, msg + i, len);
        xor_block(b, a, 16);
        aes_encrypt_block(b, key, b);
    }
    
    // === Step 2: CTR mode encryption ===
    
    // Encrypt MAC to produce MIC
    memset(a, 0, 16);
    a[0] = L - 1;
    memcpy(a + 1, nonce, nonce_len);
    aes_encrypt_block(a, key, s);
    for (i = 0; i < mic_len; i++) {
        mic[i] = b[i] ^ s[i];
    }
    
    // Encrypt message
    for (i = 0; i < msg_len; i += 16) {
        // Build counter block: Flags | Nonce | Counter
        memset(a, 0, 16);
        a[0] = L - 1;
        memcpy(a + 1, nonce, nonce_len);
        
        // Set counter value (starting from 1)
        uint16_t ctr = (i / 16) + 1;
        for (len = 0; len < L; len++) {
            a[15 - len] = (ctr >> (8 * len)) & 0xff;
        }
        
        // Encrypt counter
        aes_encrypt_block(a, key, s);
        
        // XOR with plaintext
        len = (msg_len - i) < 16 ? (msg_len - i) : 16;
        for (uint8_t j = 0; j < len; j++) {
            encrypted_msg[i + j] = msg[i + j] ^ s[j];
        }
    }
    
    return 1;
}

// AES-CCM decryption
// Returns 1 on success (MAC verified), 0 on failure
int aes128_ccm_decrypt(const uint8_t *key, const uint8_t *nonce, uint8_t nonce_len,
                       const uint8_t *aad, uint16_t aad_len,
                       uint16_t msg_len,
                       const uint8_t *encrypted_msg, uint8_t *msg,
                       const uint8_t *mic, uint8_t mic_len)
{
    uint8_t b[16], a[16], s[16];
    uint8_t computed_mic[16];
    uint16_t i, len;
    uint8_t L;
    
    // L is the size of the length field in octets
    L = 15 - nonce_len;
    
    // === Step 1: CTR mode decryption ===
    
    // Decrypt message
    for (i = 0; i < msg_len; i += 16) {
        // Build counter block
        memset(a, 0, 16);
        a[0] = L - 1;
        memcpy(a + 1, nonce, nonce_len);
        
        // Set counter value (starting from 1)
        uint16_t ctr = (i / 16) + 1;
        for (len = 0; len < L; len++) {
            a[15 - len] = (ctr >> (8 * len)) & 0xff;
        }
        
        // Encrypt counter
        aes_encrypt_block(a, key, s);
        
        // XOR with ciphertext
        len = (msg_len - i) < 16 ? (msg_len - i) : 16;
        for (uint8_t j = 0; j < len; j++) {
            msg[i + j] = encrypted_msg[i + j] ^ s[j];
        }
    }
    
    // === Step 2: Compute CBC-MAC and verify ===
    
    // Build B_0
    memset(b, 0, 16);
    b[0] = (aad_len > 0 ? 0x40 : 0) | ((mic_len - 2) / 2 << 3) | (L - 1);
    memcpy(b + 1, nonce, nonce_len);
    
    // Encode message length
    for (i = 0; i < L; i++) {
        b[15 - i] = (msg_len >> (8 * i)) & 0xff;
    }
    
    // Initialize MAC
    aes_encrypt_block(b, key, b);
    
    // Add AAD to MAC if present
    if (aad_len > 0) {
        memset(a, 0, 16);
        a[0] = (aad_len >> 8) & 0xff;
        a[1] = aad_len & 0xff;
        
        len = aad_len < 14 ? aad_len : 14;
        memcpy(a + 2, aad, len);
        xor_block(b, a, 16);
        aes_encrypt_block(b, key, b);
        
        for (i = len; i < aad_len; i += 16) {
            memset(a, 0, 16);
            len = (aad_len - i) < 16 ? (aad_len - i) : 16;
            memcpy(a, aad + i, len);
            xor_block(b, a, 16);
            aes_encrypt_block(b, key, b);
        }
    }
    
    // Add decrypted message to MAC
    for (i = 0; i < msg_len; i += 16) {
        memset(a, 0, 16);
        len = (msg_len - i) < 16 ? (msg_len - i) : 16;
        memcpy(a, msg + i, len);
        xor_block(b, a, 16);
        aes_encrypt_block(b, key, b);
    }
    
    // Decrypt MAC
    memset(a, 0, 16);
    a[0] = L - 1;
    memcpy(a + 1, nonce, nonce_len);
    aes_encrypt_block(a, key, s);
    for (i = 0; i < mic_len; i++) {
        computed_mic[i] = b[i] ^ s[i];
    }
    
    // Verify MIC (constant-time comparison)
    uint8_t diff = 0;
    for (i = 0; i < mic_len; i++) {
        diff |= computed_mic[i] ^ mic[i];
    }
    
    return (diff == 0) ? 1 : 0;
}
