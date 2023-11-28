#include <string.h>
#include <stdint.h>
#include "platform_api.h"

#include "port_gen_os_driver.h"

#define GEN_OS          ((const gen_os_driver_t *)platform_get_gen_os_driver())

#include "ing_uecc.h"

#define CURVE   uECC_secp256r1()

// See BLUETOOTH CORE SPECIFICATION Version 5.4 | Vol 3, Part H
// 2.3.5.6 LE Secure Connections pairing phase 2

// When the Security Manager is placed in a Debug mode it shall use the
// following Diffie-Hellman private / public key pair:
// Private key: 3f49f6d4 a3c55f38 74c9b3e3 d2103f50 4aff607b eb40b799
// 5899b8a6 cd3c1abd
// Public key (X): 20b003d2 f297be2c 5e2c83a7 e9f9a5b9 eff49111 acf4fddb
// cc030148 0e359de6
// Public key (Y): dc809c49 652aeb6d 63329abf 5a52155c 766345c2 8fed3024
// 741c8ed0 1589d28b

const static uint8_t debug_sk[32] =
{
    0x3f, 0x49, 0xf6, 0xd4, 0xa3, 0xc5, 0x5f, 0x38, 0x74, 0xc9, 0xb3, 0xe3, 0xd2,
    0x10, 0x3f, 0x50, 0x4a, 0xff, 0x60, 0x7b, 0xeb, 0x40, 0xb7, 0x99, 0x58, 0x99,
    0xb8, 0xa6, 0xcd, 0x3c, 0x1a, 0xbd
};

/*
const static uint8_t debug_pk[32 * 2] =
{
    0x20, 0xb0, 0x03, 0xd2, 0xf2, 0x97, 0xbe, 0x2c,
    0x5e, 0x2c, 0x83, 0xa7, 0xe9, 0xf9, 0xa5, 0xb9,
    0xef, 0xf4, 0x91, 0x11, 0xac, 0xf4, 0xfd, 0xdb,
    0xcc, 0x03, 0x01, 0x48, 0x0e, 0x35, 0x9d, 0xe6,
    0xdc, 0x80, 0x9c, 0x49, 0x65, 0x2a, 0xeb, 0x6d,
    0x63, 0x32, 0x9a, 0xbf, 0x5a, 0x52, 0x15, 0x5c,
    0x76, 0x63, 0x45, 0xc2, 0x8f, 0xed, 0x30, 0x24,
    0x74, 0x1c, 0x8e, 0xd0, 0x15, 0x89, 0xd2, 0x8b
};
*/

struct ecc_keys
{
    gen_handle_t cmd_queue_handle;
    uint8_t key_type;
    
    uint8_t sk[32];
    uint8_t pk[64];
    uint8_t peer_pk[64];
    uint8_t dh_sk[32];
} ecc_keys = {0};

typedef uint8_t ecc_msg_t;

enum ecc_cmd
{
    ECC_GEN_KEY_PAIR,
    ECC_GEN_DHKEY,
};

int ecc_rng(uint8_t *dest, unsigned size)
{
    platform_hrng(dest, size);
    return 1;
}

static void ecc_task(void *param)
{
    ecc_msg_t msg;
    int r;
    for (;;)
    {
        if (GEN_OS->queue_recv_msg(ecc_keys.cmd_queue_handle, &msg) != 0)
            continue;
        
        switch (msg)
        {
        case ECC_GEN_KEY_PAIR:
            r = uECC_make_key(ecc_keys.pk, ecc_keys.sk, CURVE);
            ll_p256_key_pair_generated(r ? 0 : 1, ecc_keys.pk);
            break;
        case ECC_GEN_DHKEY:
            r = uECC_shared_secret(ecc_keys.peer_pk, 
                ecc_keys.key_type == 0 ? ecc_keys.sk : debug_sk, 
                ecc_keys.dh_sk, CURVE);
            ll_dhkey_generated(r ? 0 : 1, ecc_keys.dh_sk);
            break;
        }
    }
}

static void start_generate_p256_key_pair(void)
{
    ecc_msg_t msg = ECC_GEN_KEY_PAIR;
    GEN_OS->queue_send_msg(ecc_keys.cmd_queue_handle, &msg);
}

static int start_generate_dhkey(int key_type, const uint8_t *remote_pub_key)
{
    if (uECC_valid_public_key(remote_pub_key, CURVE))
    {
        ecc_msg_t msg = ECC_GEN_DHKEY;
        ecc_keys.key_type = key_type;
        memcpy(ecc_keys.peer_pk, remote_pub_key, sizeof(ecc_keys.peer_pk));
        GEN_OS->queue_send_msg(ecc_keys.cmd_queue_handle, &msg);
        return 0;
    }
    else
        return 0x12; // Invalid HCI Command Parameters 
}

void install_ecc_driver(void)
{
    ecc_keys.cmd_queue_handle = GEN_OS->queue_create(1, sizeof(ecc_msg_t));
    
    uECC_set_rng(ecc_rng);
    ll_install_ecc_engine(start_generate_p256_key_pair, start_generate_dhkey);
    GEN_OS->task_create("ecc",
           ecc_task,
           NULL,
           256 * 4,
           GEN_TASK_PRIORITY_LOW);
}
