#ifndef __LE_DEVICE_DB_H
#define __LE_DEVICE_DB_H

#include "btstack_util.h"
#include "bt_types.h"
#include "kv_storage.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack (push, 1)

// Central Device db implemenation using static memory
typedef struct le_device_memory_db {

    // Identification
    uint8_t addr_type;
    uint8_t  key_size;
    uint8_t  authenticated;
    uint8_t  authorized;

    bd_addr_t addr;
    sm_key_t irk;

    // Stored pairing information allows to re-establish an enncrypted connection
    // with a peripheral that doesn't have any persistent memory
    sm_key_t ltk;
    uint16_t ediv;
    uint8_t  rand[8];

    // Signed Writes by remote
    sm_key_t remote_csrk;
    uint32_t remote_counter;

    // Signed Writes by us
    sm_key_t local_csrk;
    uint32_t local_counter;

} le_device_memory_db_t;

#pragma pack (pop)

/**

	LE Device DB is only required for signed writes

	Per bonded device, it can store
	- it stores the Identity Resolving Key (IRK) and its address to resolve private addresses
    - it stores the LTK + EDIV, RAND. EDIV + RAND allow a LE Perihperal to reconstruct the LTK
    - it stores the Connection Signature Resolving Key (CSRK) and the last used counter.
    	The CSRK is used to generate the signatur on the remote device and is needed to verify the signature itself
		The Counter is necessary to prevent reply attacks

*/

// LE Device db interface

/* API_START */

/**
 * @brief add device to db
 * @param addr_type, address of the device
 * @param irk of the device
 * @returns index if successful, -1 otherwise
 */
le_device_memory_db_t *le_device_db_add(const int addr_type, const bd_addr_t addr, const sm_key_t irk, int *index);

le_device_memory_db_t *le_device_db_find(const int addr_type, const bd_addr_t addr, int *index);

le_device_memory_db_t *le_device_db_from_key(const int key);

typedef struct le_device_memory_db_iter
{
    kvkey_t key;
    le_device_memory_db_t *cur;
} le_device_memory_db_iter_t;

void le_device_db_iter_init(le_device_memory_db_iter_t *iter);
le_device_memory_db_t *le_device_db_iter_cur(le_device_memory_db_iter_t *iter);
le_device_memory_db_t *le_device_db_iter_next(le_device_memory_db_iter_t *iter);
int le_device_db_iter_cur_key(le_device_memory_db_iter_t *iter);

// void le_device_db_remove(const int addr_type, const bd_addr_t addr);
// WARNING: ^^^ this API is not available in this release

void le_device_db_remove_key(int index);

/* API_END */

#ifdef __cplusplus
}
#endif

#endif // __LE_DEVICE_DB_H
