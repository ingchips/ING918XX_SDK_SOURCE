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
    uint8_t addr_type;      // address type, 0: public, 1: random, 2: static random, 3: resolvable private, 4: non-resolvable private
    uint8_t key_size;       // key size, 0: no key, 7: 7 bytes, 8: 8 bytes, 9: 9 bytes, 10: 10 bytes, 11: 11 bytes, 12: 12 bytes, 13: 13 bytes, 14: 14 bytes, 15: 15 bytes, 16: 16 bytes
    uint8_t authenticated;  // 0: not authenticated, 1: authenticated
    uint8_t authorized:1;   // 0: not authorized, 1: authorized
    uint8_t sm_sc:1;        // 0: not using secure connections, 1: using secure connections

    bd_addr_t addr;         // address of the device, 6 bytes long
    sm_key_t irk;           // Identity Resolving Key (IRK) used to resolve private addresses

    // Stored pairing information allows to re-establish an encrypted connection
    // with a peripheral that doesn't have any persistent memory
    sm_key_t ltk;           // Long Term Key (LTK) used to encrypt the connection
    uint16_t ediv;          // Encrypted Diversifier (EDIV) used to reconstruct the LTK
    uint8_t  rand[8];       // Random value used to reconstruct the LTK, 8 bytes long

    // Signed Writes by remote
    sm_key_t remote_csrk;       // Connection Signature Resolving Key (CSRK) used to generate the signature on the remote device
    uint32_t remote_counter;    // Last used counter for the CSRK on the remote device

    // Signed Writes by us
    sm_key_t local_csrk;    // Connection Signature Resolving Key (CSRK) used to generate the signature on our device
    uint32_t local_counter; // Last used counter for the CSRK on our device
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

/** le device memory db iter */
typedef struct le_device_memory_db_iter
{
    kvkey_t key;                // current key in the iteration
    le_device_memory_db_t *cur; // current device in the iteration
} le_device_memory_db_iter_t;

/**
 * @brief Initialize an iterator for the LE Device DB.
 * @param iter Pointer to the iterator to be initialized.
 */
void le_device_db_iter_init(le_device_memory_db_iter_t *iter);

/**
 * @brief get current device in the iteration.
 * @param iter Pointer to the iterator.
 */
le_device_memory_db_t *le_device_db_iter_cur(le_device_memory_db_iter_t *iter);

/**
 * @brief Move to the next device in the iteration.
 * @param iter Pointer to the iterator.
 */
le_device_memory_db_t *le_device_db_iter_next(le_device_memory_db_iter_t *iter);

/**
 * @brief Get the current key in the iteration.
 * @param iter Pointer to the iterator.
 */
int le_device_db_iter_cur_key(le_device_memory_db_iter_t *iter);

/**
 * @brief remove the current device in the iteration.
 * @param addr_type, address type of the device
 * @param addr, address of the device
 */
// void le_device_db_remove(const int addr_type, const bd_addr_t addr);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief remove the device in the db by index
 * @param index, index of the device in the db
 */
void le_device_db_remove_key(int index);

/* API_END */

#ifdef __cplusplus
}
#endif

#endif // __LE_DEVICE_DB_H
