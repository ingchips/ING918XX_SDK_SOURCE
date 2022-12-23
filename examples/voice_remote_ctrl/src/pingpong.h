#ifndef __PINGPONG_H__
#define __PINGPONG_H__

#ifdef    __cplusplus
extern "C" {    /* allow C++ to use these headers */
#endif    /* __cplusplus */

#include <stdlib.h>
#include "peripheral_dma.h"

#if INGCHIPS_FAMILY == INGCHIPS_FAMILY_916

typedef enum {
    PING_WRITE,
    PONG_WRITE,
} DMA_PINGPONG_WRITE;

typedef struct {
    uint32_t *ping;
    uint32_t *pong;
    DMA_PINGPONG_WRITE status;
    DMA_Descriptor descriptor_pp[2] __attribute__((aligned (8)));
} DMA_PingPong_t;

/**
 * @brief Setup DMA pingpong transmission loop struct
 *
 * @param[in] *PingPong                 pingpong DMA struct, see 'DMA_PingPong_t'
 * @param[in] srcDev                    DMA data srouce ID
 * @param[in] transWordsNum             Number of words to transmit in half pingpong-loop
 * @param[in] burstNum                  DMA burst transmission number
 * @return state                        0:seccess, other value:failed
 */
uint8_t DMA_PingPongSetup(DMA_PingPong_t *PingPong, 
                          SYSCTRL_DMA  srcDev, 
                          uint32_t transWordsNum, 
                          uint8_t  burstNum);

/**
 * @brief DMA pingpong standard proccess of interrupt
 *
 * @param[in] *PingPong                 pingpong DMA struct, see 'DMA_PingPong_t'
 * @param[in] ch                        DMA channel ID(0-7)
 * @return *buff                        pointer of data-buff
 */
uint32_t * DMA_PingPongIntProc(DMA_PingPong_t *PingPong, uint8_t ch);

/**
 * @brief Get size of single pingpong transmission
 * @note Should get data-buff's pointer by 'DMA_PingPongIntProc' first,
 * and use this interface to know how many data in that data-buff.
 *
 * @param[in] *PingPong                 pingpong DMA struct, see 'DMA_PingPong_t'
 * @return number                       data-buff's size
 */
uint32_t DMA_PingPongGetTransSize(DMA_PingPong_t *PingPong);

/**
 * @brief Enable DMA pingpong transmission
 *
 * @param[in] *PingPong                 pingpong DMA struct, see 'DMA_PingPong_t'
 * @param[in] ch                        DMA channel ID(0-7)
 */
void DMA_PingPongEnable(DMA_PingPong_t *PingPong, uint8_t ch);

/**
 * @brief Disable DMA pingpong transmission
 *
 * @param[in] *PingPong                 pingpong DMA struct, see 'DMA_PingPong_t'
 * @param[in] ch                        DMA channel ID(0-7)
 */
void DMA_PingPongDisable(DMA_PingPong_t *PingPong, uint8_t ch);
#endif

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif    /* __cplusplus */

#endif

