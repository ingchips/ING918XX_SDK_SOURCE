#ifndef __PINGPANG_H__
#define __PINGPANG_H__

#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#include <stdlib.h>
#include "ingsoc.h"

#if INGCHIPS_FAMILY == INGCHIPS_FAMILY_916

typedef enum {
    PING_WRITE,
    PANG_WRITE,
} DMA_PINGPANG_WRITE;

typedef struct {
    uint32_t *ping;
    uint32_t *pang;
    DMA_PINGPANG_WRITE status;
	DMA_Descriptor descriptor_pp[2] __attribute__((aligned (8)));
} DMA_PingPang_t;

/**
 * @brief Setup DMA pingpang transmission loop struct
 *
 * @param[in] *PingPang                 pingpang DMA struct, see 'DMA_PingPang_t'
 * @param[in] srcDev                    DMA data srouce ID
 * @param[in] transWordsNum             Number of words to transmit in half pingpang-loop
 * @param[in] burstNum                  DMA burst transmission number
 * @return state                        0:seccess, other value:failed
 */
uint8_t DMA_PingPangSetup(DMA_PingPang_t *PingPang, 
                          SYSCTRL_DMA  srcDev, 
                          uint32_t transWordsNum, 
                          uint8_t  burstNum);

/**
 * @brief DMA pingpang standard proccess of interrupt
 *
 * @param[in] *PingPang                 pingpang DMA struct, see 'DMA_PingPang_t'
 * @param[in] ch                        DMA channel ID(0-7)
 * @return *buff                        pointer of data-buff
 */
uint32_t * DMA_PingPangIntProc(DMA_PingPang_t *PingPang, uint8_t ch);

/**
 * @brief Get size of single pingpang transmission
 * @note Should get data-buff's pointer by 'DMA_PingPangIntProc' first,
 * and use this interface to know how many data in that data-buff.
 *
 * @param[in] *PingPang                 pingpang DMA struct, see 'DMA_PingPang_t'
 * @return number                       data-buff's size
 */
uint32_t DMA_PingPangGetTransSize(DMA_PingPang_t *PingPang);

/**
 * @brief Enable DMA pingpang transmission
 *
 * @param[in] *PingPang                 pingpang DMA struct, see 'DMA_PingPang_t'
 * @param[in] ch                        DMA channel ID(0-7)
 */
void DMA_PingPangEnable(DMA_PingPang_t *PingPang, uint8_t ch);

/**
 * @brief Disable DMA pingpang transmission
 *
 * @param[in] *PingPang                 pingpang DMA struct, see 'DMA_PingPang_t'
 * @param[in] ch                        DMA channel ID(0-7)
 */
void DMA_PingPangDisable(DMA_PingPang_t *PingPang, uint8_t ch);
#endif

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

