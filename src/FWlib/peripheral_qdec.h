#ifndef _PERIPHERAL_QDEC_H
#define _PERIPHERAL_QDEC_H

#include <stdint.h>

#include "ingsoc.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
typedef enum {
    QDEC_CH_CTRL        = 0x00,
    QDEC_CH_MODE        = 0x04,
    QDEC_CH_WRITE_A     = 0x14,
    QDEC_CH_WRITE_B     = 0x18,
    QDEC_CH_WRITE_C     = 0x1c,
    QDEC_CH_INT_EN      = 0x24,
    QDEC_BMR            = 0xc4,
} QDEC_qdecReg;

typedef enum {
    QDEC_DIV_4          = 0x0,
    QDEC_DIV_16         = 0x1,
    QDEC_DIV_64         = 0x2,
    QDEC_DIV_256        = 0x3,
    QDEC_DIV_1024       = 0x4,
    QDEC_DIV_4096       = 0x5,
    QDEC_DIV_16384      = 0x6,
    QDEC_DIV_65536      = 0x7,
} QDEC_indexCfg;

typedef enum {
    QDEC_CH_0           = 0x0,
    QDEC_CH_1,
    QDEC_CH_2,
} QDEC_channelId;

/**
 * @brief Set QDEC index div and enable QDEC index register
 *
 * @param[in] div            QDEC index div, see 'QDEC_indexCfg'
 * @return                   null
 */
void QDEC_EnableQdecDiv(QDEC_indexCfg div);

/**
 * @brief configurate timer counter for A, B and C
 *
 * @param[in] ch             QDEC channel id, see 'QDEC_channelId'
 * @param[in] tc_ra          timer counter for A(0-0xffff)
 * @param[in] tc_rb          timer counter for B(0-0xffff)
 * @param[in] tc_rc          timer counter for C(0-0xffff)
 * @return                   null
 */
void QDEC_TcCfg(QDEC_channelId ch, uint16_t tc_ra, uint16_t tc_rb, uint16_t tc_rc);

/**
 * @brief QDEC stantard configuration interface
 *
 * @param[in] ch             QDEC channel id, see 'QDEC_channelId'
 * @param[in] tc_ra          timer counter for A(0-0xffff)
 * @param[in] tc_rb          timer counter for B(0-0xffff)
 * @param[in] tc_rc          timer counter for C(0-0xffff)
 * @param[in] fliter         QDEC signal filter value(0-63)
 * @param[in] miss           max value of QDEC miss signal(0-15)
 * @return                   null
 */
void QDEC_QdecCfg(QDEC_channelId ch, uint16_t tc_ra, uint16_t tc_rb, uint16_t tc_rc, 
    uint8_t fliter, uint8_t miss);

/**
 * @brief Enable/disable QDEC channel
 *
 * @param[in] ch             QDEC channel id, see 'QDEC_channelId'
 * @param[in] enable         enable/disable
 * @return                   null
 */
void QDEC_ChannelEnable(QDEC_channelId ch, uint8_t enable);

/**
 * @brief Read QDEC data
 *
 * @param[in] ch             QDEC channel id, see 'QDEC_channelId'
 * @return                   QDEC data
 */
uint16_t QDEC_ReadData(QDEC_channelId ch);


#endif

#ifdef __cplusplus
}
#endif

#endif
