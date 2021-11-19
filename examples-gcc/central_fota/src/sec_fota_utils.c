#include <stdint.h>
#include <stdlib.h>
#include "ing918_uecc.h"
#include "fota_client.h"

#define CURVE       uECC_secp256r1()
#define SK_SIZE     32
#define PK_SIZE     64
#define HASH_SIZE   32

static uint8_t session_pk[PK_SIZE];
static uint8_t session_sk[SK_SIZE];

const static uint8_t root_sk[SK_SIZE] =
{
    0x5c, 0x77, 0x17, 0x11, 0x67, 0xd6, 0x40, 0xa3, 0x36, 0x0d, 0xe2,
    0x69, 0xfe, 0x0b, 0xb7, 0x8f, 0x5e, 0x94, 0xd8, 0xf2, 0xf4, 0x80,
    0x94, 0x0a, 0xc2, 0xf2, 0x6e, 0x43, 0xbb, 0x69, 0x5f, 0xa7
};

static void sha_256(uint8_t hash[32], const void *input, int len)
{
    calc_sha_256(hash, input, len);
}

static int verify(const uint8_t *pk, const uint8_t *msg, int msg_len, const uint8_t *signature)
{
    static uint8_t hash[HASH_SIZE];
    calc_sha_256(hash, msg, msg_len);
    return uECC_verify(pk, hash, sizeof(hash), signature, CURVE);
}

static void sign(const uint8_t *sk, const uint8_t *msg, int msg_len, uint8_t *signature)
{
    static uint8_t hash[HASH_SIZE];
    calc_sha_256(hash, msg, msg_len);
    uECC_sign(sk, hash, sizeof(hash), signature, CURVE);
}

static int validate_pk(const uint8_t *pk)
{
    return uECC_valid_public_key(pk, CURVE);
}

static void shared_secret(const uint8_t *local_sk, const uint8_t *peer_pk, uint8_t *shared) {
    uECC_shared_secret(peer_pk, local_sk, shared, CURVE);
}

const static ecc_driver_t driver =
{
    .session_pk = session_pk,
    .session_sk = session_sk,
    .root_sk = root_sk,
    .sha_256 = sha_256,
    .verify = verify,
    .sign = sign,
    .validate_pk = validate_pk,
    .shared_secret = shared_secret,
    .alloc = malloc,
    .free = free,
};

const ecc_driver_t *get_ecc_driver(void)
{
    while (uECC_make_key(session_pk, session_sk, CURVE) == 0) ;
    return &driver;
}
