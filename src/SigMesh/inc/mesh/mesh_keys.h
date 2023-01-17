#ifndef __MESH_KEYS_H
#define __MESH_KEYS_H

#include <stdint.h>
#include <stdbool.h>

#include "mesh_linked_list.h"

#include "mesh/adv_bearer.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MESH_KEYS_INVALID_INDEX 0xffff

typedef struct {
    mesh_linked_item_t item;

    // internal index [0..MAX_NR_MESH_NETWORK_KEYS-1]
    uint16_t internal_index;

    // index into shared global key list
    uint16_t netkey_index;

    // internal version - allows for newer-than relation between keys with same netkey_index
    uint8_t version;

    // net_key from provisioner or Config Model Client
    uint8_t net_key[16];

    // derived data

    // k1
    uint8_t identity_key[16];
    uint8_t beacon_key[16];

    // k3
    uint8_t network_id[8];

    // k2
    uint8_t nid;
    uint8_t encryption_key[16];
    uint8_t privacy_key[16];

} mesh_network_key_t;

typedef struct {
    mesh_linked_list_iterator_t it;
    mesh_network_key_t * key;
    uint8_t nid;
} mesh_network_key_iterator_t;

typedef struct {
    mesh_linked_item_t item;

    // internal index [0..MAX_NR_MESH_TRANSPORT_KEYS-1]
    uint16_t internal_index;

    // netkey_index of subnet this app key is used with
    uint16_t netkey_index;

    // index into shared global app key list
    uint16_t appkey_index;

    // app_key
    uint8_t key[16];

    // internal version - allows for newer-than relation between keys with same appkey_index
    uint8_t version;

    // old key - mark key as 'older' in app key update or startup
    uint8_t old_key;

    // application key flag, 0 for device key
    uint8_t akf;

    // application key hash id
    uint8_t aid;

    // key refresth
    uint8_t key_refresh;

} mesh_transport_key_t;

typedef struct {
    mesh_linked_list_iterator_t it;
    mesh_transport_key_t * key;
    uint16_t netkey_index;
    uint8_t  akf;
    uint8_t  aid;
} mesh_transport_key_iterator_t;

/**
 * @brief Get internal index of free network key storage entry
 * @note index 0 is reserved for primary network key
 * @return index or MESH_KEYS_INVALID_INDEX if none found
 */
uint16_t mesh_network_key_get_free_index(void);

/**
 * @brief Get number of stored network_keys
 * @return count
 */
int mesh_network_key_list_count(void);

/**
 * Transport Keys = Application Keys + Device Key
 */

/**
 * @brief Get internal index of free transport key storage entry
 * @note index 0 is reserved for device key
 * @return index or 0u if none found
 */
uint16_t mesh_transport_key_get_free_index(void);

/**
 * Get transport key for appkey_index
 * @param appkey_index
 * @return
 */
mesh_transport_key_t * mesh_transport_key_get(uint16_t appkey_index);


#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif
