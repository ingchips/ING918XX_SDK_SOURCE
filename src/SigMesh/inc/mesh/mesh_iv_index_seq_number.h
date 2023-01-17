#ifndef __MESH_IV_INDEX_SEQ_NUMBER_H
#define __MESH_IV_INDEX_SEQ_NUMBER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * IV Index
 */

uint32_t mesh_get_iv_index(void);

uint32_t mesh_get_iv_index_for_tx(void);

/**
 * @brief Get IV Update state
 */
int mesh_iv_update_active(void);

/**
 * Sequence Number
 */
uint32_t mesh_sequence_number_peek(void);


#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif
