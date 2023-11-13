/**
 * @title Tag-Value-Length Persistent Storage (TLV)
 *
 * Inteface for BTstack's Tag Value Length Persistent Storage implementations
 * used to store pairing/bonding data.
 *
 */

#ifndef BTSTACK_TLV_H
#define BTSTACK_TLV_H

#include <stdint.h>

#if defined __cplusplus
extern "C" {
#endif

/* API_START */

typedef struct {

    /**
     * Get Value for Tag
     * @param context
     * @param tag
     * @param buffer
     * @param buffer_size
     * @return size of value
     */
    int (*get_tag)(void * context, uint32_t tag, uint8_t * buffer, uint32_t buffer_size);

    /**
     * Store Tag 
     * @param context
     * @param tag
     * @param data
     * @param data_size
     * @return 0 on success
     */
    int (*store_tag)(void * context, uint32_t tag, const uint8_t * data, uint32_t data_size);

    /**
     * Delete Tag
     *  @note it is not expected that delete operation fails, please use at least MESH_LOG_ERR in case of errors
     * @param context
     * @param tag
     */
    void (*delete_tag)(void * context,  uint32_t tag);

} btstack_tlv_t;

/** 
 * @brief Make TLV implementation available to BTstack components via Singleton
 * @note Usually called by port after BD_ADDR was retrieved from Bluetooth Controller
 * @param tlv_impl
 * @param tlv_context
 */
void btstack_tlv_set_instance(const btstack_tlv_t * tlv_impl, void * tlv_context);

/* API_END */

#if defined __cplusplus
}
#endif
#endif // BTSTACK_TLV_H
