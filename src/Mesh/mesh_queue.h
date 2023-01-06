/** @file     mesh_queue.h                                                 *
*  @brief    function API and data structure in MESH stack              *
*  @version  1.0.0.                                                     *
*  @date     2020/10/July                                                 *
*                                                                       *
*  Copyright (c) 2019 IngChips corp.                                    *
*                                                                       *
*                                                                       *
 *************************************************************************************************************/

#ifndef MESH_QUEUE_H
#define MESH_QUEUE_H
#include "type_def.h"
#define GET_MSG_BLOCK_DATA_BASE_HEAD(ptr) \
((uint8_t*)(ptr) + sizeof(btstack_linked_item_t))

#define GET_MSG_BLOCK_HEAD_BASE_DATA(ptr) \
((uint8_t*)(ptr) - sizeof(btstack_linked_item_t))

#define MEM_CHECK_CPY(to,from,len)  \
 { \
    ASSERT_ERR((HCI_CMDS_PAYLOAD_SIZE>len));                        \
    if(GET_MSG_BLOCK_HEAD_BASE_DATA(to)==NULL)      \
    {              \
        console_printf("mem out\n"); \
        return;\
    } \
    memcpy((uint8_t*)to,(uint8_t*)from,len); \
 }

/*a internal queue for allocate the buffer of message from MESH to HOST*/
#define HCI_CMDS_PAYLOAD_SIZE 52
typedef struct DTBT_small_msg
{
	 btstack_linked_item_t  item;
	 uint8_t  data[HCI_CMDS_PAYLOAD_SIZE];
} DTBT_small_msg_t;

/** @brief get internal memory queue to cach the received data.
 *
 *  @return  the pointer to the item in the queue.
 */
DTBT_msg_t * btstack_memory_dtbt_msg_get(void);

/** @brief free the memory which allocated by btstack_memory_dtbt_msg_get .
 *
 *  @param dtbt_msg : pointer to the memory blokc need to be released
 *
 */
void btstack_memory_dtbt_msg_free(DTBT_msg_t *dtbt_msg);

/** @brief get internal memory queue to cach the received data special for the  HCI cmds from MESH to HOST
 *
 *  @return  the pointer to the item in the queue.
 */
DTBT_small_msg_t * btstack_memory_dtbt_small_msg_get(void);

/** @brief free the memory which allocated by btstack_memory_dtbt_small_msg_get .
 *
 *  @param dtbt_msg : pointer to the memory blokc need to be released
 *
 */
void btstack_memory_dtbt_small_msg_free(DTBT_small_msg_t *dtbt_msg);

/**
 * @brief CCM and AES structure declaration
 * @defgroup encryption
 * @ingroup bt_mesh
 * @{
 */

/** @brief need to notify the ccm result to mesh stack
 *
 *  @param result : 0 EOK , others failed
 *
 */
void set_ccm_result(uint8_t result);

/** @brief need to notify the aes result to mesh stack
 *
 *  @param result : 0 EOK , ohters failed
 *
 */
void set_aes_result(uint8_t result);
/*
 * @}
 */

/** @brief allocate the queue for memory allocate
 *
 */
void mesh_memory_init(void);
void create_mesh_task (void);
int Host2Mesh_msg_send(uint8_t* mesh_msg ,uint8_t len);

#endif
