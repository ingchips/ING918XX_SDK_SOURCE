
#ifndef __PERIPHERAL_I2C_H__
#define __PERIPHERAL_I2C_H__


#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#include "ingsoc.h"

#if INGCHIPS_FAMILY == INGCHIPS_FAMILY_918
/*
 * Description:
 * Bit shifts and widths for I2C_CTRL0  0x00
 */
#define bsI2C_CTRL0_XFER_COUNT                 0
#define bwI2C_CTRL0_XFER_COUNT                16
#define bsI2C_CTRL0_DIRECTION                 16
#define bwI2C_CTRL0_DIRECTION                  1
#define bsI2C_CTRL0_MASTER_MODE               17
#define bwI2C_CTRL0_MASTER_MODE                1
#define bsI2C_CTRL0_SLAVE_ADDRESS_ENABLE      18
#define bwI2C_CTRL0_SLAVE_ADDRESS_ENABLE       1
#define bsI2C_CTRL0_PRE_SEND_START            19
#define bwI2C_CTRL0_PRE_SEND_START             1
#define bsI2C_CTRL0_POST_SEND_STOP            20
#define bwI2C_CTRL0_POST_SEND_STOP             1
#define bsI2C_CTRL0_RETAIN_CLOCK              21
#define bwI2C_CTRL0_RETAIN_CLOCK               1
#define bsI2C_CTRL0_CLOCK_HELD                22
#define bwI2C_CTRL0_CLOCK_HELD                 1
#define bsI2C_CTRL0_MULTI_MASTER              23
#define bwI2C_CTRL0_MULTI_MASTER               1
#define bsI2C_CTRL0_RSVD1                     24
#define bwI2C_CTRL0_RSVD1                      1
#define bsI2C_CTRL0_SEND_NAK_ON_LAST          25
#define bwI2C_CTRL0_SEND_NAK_ON_LAST           1
#define bsI2C_CTRL0_ACKNOWLEDGE               26
#define bwI2C_CTRL0_ACKNOWLEDGE                1
#define bsI2C_CTRL0_PRE_ACK                   27
#define bwI2C_CTRL0_PRE_ACK                    1
#define bsI2C_CTRL0_RSVD2                     28
#define bwI2C_CTRL0_RSVD2                      1
#define bsI2C_CTRL0_RUN                       29
#define bwI2C_CTRL0_RUN                        1
#define bsI2C_CTRL0_CLKGATE                   30
#define bwI2C_CTRL0_CLKGATE                    1
#define bsI2C_CTRL0_SFTRST                    31
#define bwI2C_CTRL0_SFTRST                     1

#define I2C_CTRL0_DIRECTION                    (1 << bsI2C_CTRL0_DIRECTION            )
#define I2C_CTRL0_MASTER_MODE                  (1 << bsI2C_CTRL0_MASTER_MODE          )
#define I2C_CTRL0_SLAVE_ADDRESS_ENABLE         (1 << bsI2C_CTRL0_SLAVE_ADDRESS_ENABLE )
#define I2C_CTRL0_PRE_SEND_START               (1 << bsI2C_CTRL0_PRE_SEND_START       )
#define I2C_CTRL0_POST_SEND_STOP               (1 << bsI2C_CTRL0_POST_SEND_STOP       )
#define I2C_CTRL0_RETAIN_CLOCK                 (1 << bsI2C_CTRL0_RETAIN_CLOCK         )
#define I2C_CTRL0_CLOCK_HELD                   (1 << bsI2C_CTRL0_CLOCK_HELD           )
#define I2C_CTRL0_MULTI_MASTER                 (1 << bsI2C_CTRL0_MULTI_MASTER         )
#define I2C_CTRL0_RSVD1                        (1 << bsI2C_CTRL0_RSVD1                )
#define I2C_CTRL0_SEND_NAK_ON_LAST             (1 << bsI2C_CTRL0_SEND_NAK_ON_LAST     )
#define I2C_CTRL0_ACKNOWLEDGE                  (1 << bsI2C_CTRL0_ACKNOWLEDGE          )
#define I2C_CTRL0_PRE_ACK                      (1 << bsI2C_CTRL0_PRE_ACK              )
#define I2C_CTRL0_RSVD2                        (1 << bsI2C_CTRL0_RSVD2                )
#define I2C_CTRL0_RUN                          (1 << bsI2C_CTRL0_RUN                  )
#define I2C_CTRL0_CLKGATE                      (1 << bsI2C_CTRL0_CLKGATE              )
#define I2C_CTRL0_SFTRST                       (1UL << bsI2C_CTRL0_SFTRST               )


/*
 * Description:
 * Bit shifts and widths for I2C_TIMING0  0x10
 */
#define bsI2C_TIMING0_RCV_COUNT                 0
#define bwI2C_TIMING0_RCV_COUNT                10
#define bsI2C_TIMING0_HIGH_COUNT               16
#define bwI2C_TIMING0_HIGH_COUNT               10


/*
 * Description:
 * Bit shifts and widths for I2C_TIMING1  0x20
 */
#define bsI2C_TIMING1_XMIT_COUNT                0
#define bwI2C_TIMING1_XMIT_COUNT               10
#define bsI2C_TIMING1_LOW_COUNT                16
#define bwI2C_TIMING1_LOW_COUNT                10


/*
 * Description:
 * Bit shifts and widths for I2C_TIMING2  0x30
 */
#define bsI2C_TIMING2_LEADIN_COUNT                0
#define bwI2C_TIMING2_LEADIN_COUNT               10
#define bsI2C_TIMING2_BUS_FREE                   16
#define bwI2C_TIMING2_BUS_FREE                   10

/*
 * Description:
 * Bit shifts and widths for I2C_CTRL1  0x40
 */
#define bsI2C_CTRL1_SLAVE_IRQ                        0
#define bwI2C_CTRL1_SLAVE_IRQ                        1
#define bsI2C_CTRL1_SLAVE_STOP_IRQ                   1
#define bwI2C_CTRL1_SLAVE_STOP_IRQ                   1
#define bsI2C_CTRL1_MASTER_LOSS_IRQ                  2
#define bwI2C_CTRL1_MASTER_LOSS_IRQ                  1
#define bsI2C_CTRL1_EARLY_TERM_IRQ                   3
#define bwI2C_CTRL1_EARLY_TERM_IRQ                   1
#define bsI2C_CTRL1_OVERSIZE_XFER_TERM_IRQ           4
#define bwI2C_CTRL1_OVERSIZE_XFER_TERM_IRQ           1
#define bsI2C_CTRL1_NO_SLAVE_ACK_IRQ                 5
#define bwI2C_CTRL1_NO_SLAVE_ACK_IRQ                 1
#define bsI2C_CTRL1_DATA_ENGINE_CMPLT_IRQ            6
#define bwI2C_CTRL1_DATA_ENGINE_CMPLT_IRQ            1
#define bsI2C_CTRL1_BUS_FREE_IRQ                     7
#define bwI2C_CTRL1_BUS_FREE_IRQ                     1
#define bsI2C_CTRL1_SLAVE_IRQ_EN                     8
#define bwI2C_CTRL1_SLAVE_IRQ_EN                     1
#define bsI2C_CTRL1_SLAVE_STOP_IRQ_EN                9
#define bwI2C_CTRL1_SLAVE_STOP_IRQ_EN                1
#define bsI2C_CTRL1_MASTER_LOSS_IRQ_EN              10
#define bwI2C_CTRL1_MASTER_LOSS_IRQ_EN               1
#define bsI2C_CTRL1_EARLY_TERM_IRQ_EN               11
#define bwI2C_CTRL1_EARLY_TERM_IRQ_EN                1
#define bsI2C_CTRL1_OVERSIZE_XFER_TERM_IRQ_EN       12
#define bwI2C_CTRL1_OVERSIZE_XFER_TERM_IRQ_EN        1
#define bsI2C_CTRL1_NO_SLAVE_ACK_IRQ_EN             13
#define bwI2C_CTRL1_NO_SLAVE_ACK_IRQ_EN              1
#define bsI2C_CTRL1_DATA_ENGINE_CMPLT_IRQ_EN        14
#define bwI2C_CTRL1_DATA_ENGINE_CMPLT_IRQ_EN         1
#define bsI2C_CTRL1_BUS_FREE_IRQ_EN                 15
#define bwI2C_CTRL1_BUS_FREE_IRQ_EN                  1
#define bsI2C_CTRL1_SLAVE_ADDRESS_BYTE              16
#define bwI2C_CTRL1_SLAVE_ADDRESS_BYTE               8
#define bsI2C_CTRL1_BCAST_SLAVE_EN                  24
#define bwI2C_CTRL1_BCAST_SLAVE_EN                   1
#define bsI2C_CTRL1_FORCE_CLK_IDLE                  25
#define bwI2C_CTRL1_FORCE_CLK_IDLE                   1
#define bsI2C_CTRL1_FORCE_DATA_IDLE                 26
#define bwI2C_CTRL1_FORCE_DATA_IDLE                  1
#define bsI2C_CTRL1_ACK_MODE                        27
#define bwI2C_CTRL1_ACK_MODE                         1
#define bsI2C_CTRL1_CLR_GOT_A_NAK                   28
#define bwI2C_CTRL1_CLR_GOT_A_NAK                    1
#define bsI2C_CTRL1_WR_QUEUE_IRQ                    29
#define bwI2C_CTRL1_WR_QUEUE_IRQ                     1
#define bsI2C_CTRL1_RD_QUEUE_IRQ                    30
#define bwI2C_CTRL1_RD_QUEUE_IRQ                     1


#define I2C_CTRL1_SLAVE_IRQ                          (1 <<  bsI2C_CTRL1_SLAVE_IRQ                  )
#define I2C_CTRL1_SLAVE_STOP_IRQ                     (1 <<  bsI2C_CTRL1_SLAVE_STOP_IRQ             )
#define I2C_CTRL1_MASTER_LOSS_IRQ                    (1 <<  bsI2C_CTRL1_MASTER_LOSS_IRQ            )
#define I2C_CTRL1_EARLY_TERM_IRQ                     (1 <<  bsI2C_CTRL1_EARLY_TERM_IRQ             )
#define I2C_CTRL1_OVERSIZE_XFER_TERM_IRQ             (1 <<  bsI2C_CTRL1_OVERSIZE_XFER_TERM_IRQ     )
#define I2C_CTRL1_NO_SLAVE_ACK_IRQ                   (1 <<  bsI2C_CTRL1_NO_SLAVE_ACK_IRQ           )
#define I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ              (1 <<  bsI2C_CTRL1_DATA_ENGINE_CMPLT_IRQ      )
#define I2C_CTRL1_BUS_FREE_IRQ                       (1 <<  bsI2C_CTRL1_BUS_FREE_IRQ               )
#define I2C_CTRL1_SLAVE_IRQ_EN                       (1 <<  bsI2C_CTRL1_SLAVE_IRQ_EN               )
#define I2C_CTRL1_SLAVE_STOP_IRQ_EN                  (1 <<  bsI2C_CTRL1_SLAVE_STOP_IRQ_EN          )
#define I2C_CTRL1_MASTER_LOSS_IRQ_EN                 (1 <<  bsI2C_CTRL1_MASTER_LOSS_IRQ_EN         )
#define I2C_CTRL1_EARLY_TERM_IRQ_EN                  (1 <<  bsI2C_CTRL1_EARLY_TERM_IRQ_EN          )
#define I2C_CTRL1_OVERSIZE_XFER_TERM_IRQ_EN          (1 <<  bsI2C_CTRL1_OVERSIZE_XFER_TERM_IRQ_EN  )
#define I2C_CTRL1_NO_SLAVE_ACK_IRQ_EN                (1 <<  bsI2C_CTRL1_NO_SLAVE_ACK_IRQ_EN        )
#define I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ_EN           (1 <<  bsI2C_CTRL1_DATA_ENGINE_CMPLT_IRQ_EN   )
#define I2C_CTRL1_BUS_FREE_IRQ_EN                    (1 <<  bsI2C_CTRL1_BUS_FREE_IRQ_EN            )
#define I2C_CTRL1_BCAST_SLAVE_EN                     (1 <<  bsI2C_CTRL1_BCAST_SLAVE_EN             )
#define I2C_CTRL1_FORCE_CLK_IDLE                     (1 <<  bsI2C_CTRL1_FORCE_CLK_IDLE             )
#define I2C_CTRL1_FORCE_DATA_IDLE                    (1 <<  bsI2C_CTRL1_FORCE_DATA_IDLE            )
#define I2C_CTRL1_ACK_MODE                           (1 <<  bsI2C_CTRL1_ACK_MODE                   )
#define I2C_CTRL1_CLR_GOT_A_NAK                      (1 <<  bsI2C_CTRL1_CLR_GOT_A_NAK              )
#define I2C_CTRL1_WR_QUEUE_IRQ                       (1 <<  bsI2C_CTRL1_WR_QUEUE_IRQ               )
#define I2C_CTRL1_RD_QUEUE_IRQ                       (1 <<  bsI2C_CTRL1_RD_QUEUE_IRQ               )


/*
 * Description:
 * Bit shifts and widths for I2C_STAT  0x50
 */
#define bsI2C_STAT_SLAVE_IRQ_SUMMARY                    0
#define bwI2C_STAT_SLAVE_IRQ_SUMMARY                    1
#define bsI2C_STAT_SLAVE_STOP_IRQ_SUMMARY               1
#define bwI2C_STAT_SLAVE_STOP_IRQ_SUMMARY               1
#define bsI2C_STAT_MASTER_LOSS_IRQ_SUMMARY              2
#define bwI2C_STAT_MASTER_LOSS_IRQ_SUMMARY              1
#define bsI2C_STAT_EARLY_TERM_IRQ_SUMMARY               3
#define bwI2C_STAT_EARLY_TERM_IRQ_SUMMARY               1
#define bsI2C_STAT_OVERSIZE_XFER_TERM_IRQ_SUMMARY       4
#define bwI2C_STAT_OVERSIZE_XFER_TERM_IRQ_SUMMARY       1
#define bsI2C_STAT_NO_SLAVE_ACK_IRQ_SUMMARY             5
#define bwI2C_STAT_NO_SLAVE_ACK_IRQ_SUMMARY             1
#define bsI2C_STAT_DATA_ENGINE_CMPLT_IRQ_SUMMARY        6
#define bwI2C_STAT_DATA_ENGINE_CMPLT_IRQ_SUMMARY        1
#define bsI2C_STAT_BUS_FREE_IRQ_SUMMARY                 7
#define bwI2C_STAT_BUS_FREE_IRQ_SUMMARY                 1
#define bsI2C_STAT_SLAVE_BUSY                           8
#define bwI2C_STAT_SLAVE_BUSY                           1
#define bsI2C_STAT_DATA_ENGINE_BUSY                     9
#define bwI2C_STAT_DATA_ENGINE_BUSY                     1
#define bsI2C_STAT_CLK_GEN_BUSY                        10
#define bwI2C_STAT_CLK_GEN_BUSY                         1
#define bsI2C_STAT_BUS_BUSY                            11
#define bwI2C_STAT_BUS_BUSY                             1
#define bsI2C_STAT_DATA_ENGINE_DMA_WAIT                12
#define bwI2C_STAT_DATA_ENGINE_DMA_WAIT                 1
#define bsI2C_STAT_SLAVE_SEARCHING                     13
#define bwI2C_STAT_SLAVE_SEARCHING                      1
#define bsI2C_STAT_SLAVE_FOUND                         14
#define bwI2C_STAT_SLAVE_FOUND                          1
#define bsI2C_STAT_SLAVE_ADDR_EQ_ZERO                  15
#define bwI2C_STAT_SLAVE_ADDR_EQ_ZERO                   1
#define bsI2C_STAT_RCVD_SLAVE_ADDR                     16
#define bwI2C_STAT_RCVD_SLAVE_ADDR                      8
#define bsI2C_STAT_GOT_A_NAK                           28
#define bwI2C_STAT_GOT_A_NAK                            1
#define bsI2C_STAT_ANY_ENABLED_IRQ                     29
#define bwI2C_STAT_ANY_ENABLED_IRQ                      1
#define bsI2C_STAT_SLAVE_PRESENT                       30
#define bwI2C_STAT_SLAVE_PRESENT                        1
#define bsI2C_STAT_MASTER_PRESENT                      31
#define bwI2C_STAT_MASTER_PRESENT                       1


/*
 * Descripton:
 * Bit shifts and widths for I2C_QUEUECTRL  0x60
 */
#define bsI2C_QUEUECTRL_WR_QUEUE_IRQ_EN                  0
#define bwI2C_QUEUECTRL_WR_QUEUE_IRQ_EN                  1
#define bsI2C_QUEUECTRL_RD_QUEUE_IRQ_EN                  1
#define bwI2C_QUEUECTRL_RD_QUEUE_IRQ_EN                  1
#define bsI2C_QUEUECTRL_PIO_QUEUE_MODE                   2
#define bwI2C_QUEUECTRL_PIO_QUEUE_MODE                   1
#define bsI2C_QUEUECTRL_WR_CLEAR                         3
#define bwI2C_QUEUECTRL_WR_CLEAR                         1
#define bsI2C_QUEUECTRL_RD_CLEAR                         4
#define bwI2C_QUEUECTRL_RD_CLEAR                         1
#define bsI2C_QUEUECTRL_QUEUE_RUN                        5
#define bwI2C_QUEUECTRL_QUEUE_RUN                        1
#define bsI2C_QUEUECTRL_WR_THRESH                        8
#define bwI2C_QUEUECTRL_WR_THRESH                        5
#define bsI2C_QUEUECTRL_RD_THRESH                       16
#define bwI2C_QUEUECTRL_RD_THRESH                        5


#define I2C_QUEUECTRL_WR_QUEUE_IRQ_EN                    (1 <<  bsI2C_QUEUECTRL_WR_QUEUE_IRQ_EN      )
#define I2C_QUEUECTRL_RD_QUEUE_IRQ_EN                    (1 <<  bsI2C_QUEUECTRL_RD_QUEUE_IRQ_EN      )
#define I2C_QUEUECTRL_PIO_QUEUE_MODE                     (1 <<  bsI2C_QUEUECTRL_PIO_QUEUE_MODE       )
#define I2C_QUEUECTRL_WR_CLEAR                           (1 <<  bsI2C_QUEUECTRL_WR_CLEAR             )
#define I2C_QUEUECTRL_RD_CLEAR                           (1 <<  bsI2C_QUEUECTRL_RD_CLEAR             )
#define I2C_QUEUECTRL_QUEUE_RUN                          (1 <<  bsI2C_QUEUECTRL_QUEUE_RUN            )


/*
 * Descripton:
 * Bit shifts and widths for I2C_QUEUESTAT  0x70
 */
#define bsI2C_QUEUESTAT_WR_QUEUE_CNT                       0
#define bwI2C_QUEUESTAT_WR_QUEUE_CNT                       5
#define bsI2C_QUEUESTAT_WR_QUEUE_EMPTY                     5
#define bwI2C_QUEUESTAT_WR_QUEUE_EMPTY                     1
#define bsI2C_QUEUESTAT_WR_QUEUE_FULL                      6
#define bwI2C_QUEUESTAT_WR_QUEUE_FULL                      1
#define bsI2C_QUEUESTAT_RD_QUEUE_CNT                       8
#define bwI2C_QUEUESTAT_RD_QUEUE_CNT                       5
#define bsI2C_QUEUESTAT_RD_QUEUE_EMPTY                    13
#define bwI2C_QUEUESTAT_RD_QUEUE_EMPTY                     1
#define bsI2C_QUEUESTAT_RD_QUEUE_FULL                     14
#define bwI2C_QUEUESTAT_RD_QUEUE_FULL                      1

#define I2C_QUEUESTAT_WR_QUEUE_FULL(BASE)              ((BASE->I2C_QUEUESTAT >> bsI2C_QUEUESTAT_WR_QUEUE_FULL) & BW2M(bwI2C_QUEUESTAT_WR_QUEUE_FULL))
#define I2C_QUEUESTAT_WR_QUEUE_EMPTY(BASE)             ((BASE->I2C_QUEUESTAT >> bsI2C_QUEUESTAT_WR_QUEUE_EMPTY) & BW2M(bwI2C_QUEUESTAT_WR_QUEUE_EMPTY))

#define I2C_QUEUESTAT_RD_QUEUE_FULL(BASE)              ((BASE->I2C_QUEUESTAT >> bsI2C_QUEUESTAT_RD_QUEUE_FULL) & BW2M(bwI2C_QUEUESTAT_RD_QUEUE_FULL))
#define I2C_QUEUESTAT_RD_QUEUE_EMPTY(BASE)             ((BASE->I2C_QUEUESTAT >> bsI2C_QUEUESTAT_RD_QUEUE_EMPTY) & BW2M(bwI2C_QUEUESTAT_RD_QUEUE_EMPTY))


/*
 * Descripton:
 * Bit shifts and widths for I2C_QUEUECMD  0x80
 */
#define bsI2C_QUEUECMD_XFER_COUNT                         0
#define bwI2C_QUEUECMD_XFER_COUNT                        16
#define bsI2C_QUEUECMD_DIRECTION                         16
#define bwI2C_QUEUECMD_DIRECTION                          1
#define bsI2C_QUEUECMD_MASTER_MODE                       17
#define bwI2C_QUEUECMD_MASTER_MODE                        1
#define bsI2C_QUEUECMD_SLAVE_ADDRESS_ENABLE              18
#define bwI2C_QUEUECMD_SLAVE_ADDRESS_ENABLE               1
#define bsI2C_QUEUECMD_PRE_SEND_START                    19
#define bwI2C_QUEUECMD_PRE_SEND_START                     1
#define bsI2C_QUEUECMD_POST_SEND_STOP                    20
#define bwI2C_QUEUECMD_POST_SEND_STOP                     1
#define bsI2C_QUEUECMD_RETAIN_CLOCK                      21
#define bwI2C_QUEUECMD_RETAIN_CLOCK                       1
#define bsI2C_QUEUECMD_CLOCK_HELD                        22
#define bwI2C_QUEUECMD_CLOCK_HELD                         1
#define bsI2C_QUEUECMD_MULTI_MASTER                      23
#define bwI2C_QUEUECMD_MULTI_MASTER                       1
#define bsI2C_QUEUECMD_SEND_NAK_ON_LAST                  25
#define bwI2C_QUEUECMD_SEND_NAK_ON_LAST                   1
#define bsI2C_QUEUECMD_ACKNOWLEDGE                       26
#define bwI2C_QUEUECMD_ACKNOWLEDGE                        1
#define bsI2C_QUEUECMD_PRE_ACK                           27
#define bwI2C_QUEUECMD_PRE_ACK                            1

#define I2C_QUEUECMD_DIRECTION                            (1 <<  bsI2C_QUEUECMD_DIRECTION                )
#define I2C_QUEUECMD_MASTER_MODE                          (1 <<  bsI2C_QUEUECMD_MASTER_MODE              )
#define I2C_QUEUECMD_SLAVE_ADDRESS_ENABLE                 (1 <<  bsI2C_QUEUECMD_SLAVE_ADDRESS_ENABLE     )
#define I2C_QUEUECMD_PRE_SEND_START                       (1 <<  bsI2C_QUEUECMD_PRE_SEND_START           )
#define I2C_QUEUECMD_POST_SEND_STOP                       (1 <<  bsI2C_QUEUECMD_POST_SEND_STOP           )
#define I2C_QUEUECMD_RETAIN_CLOCK                         (1 <<  bsI2C_QUEUECMD_RETAIN_CLOCK             )
#define I2C_QUEUECMD_CLOCK_HELD                           (1 <<  bsI2C_QUEUECMD_CLOCK_HELD               )
#define I2C_QUEUECMD_MULTI_MASTER                         (1 <<  bsI2C_QUEUECMD_MULTI_MASTER             )
#define I2C_QUEUECMD_SEND_NAK_ON_LAST                     (1 <<  bsI2C_QUEUECMD_SEND_NAK_ON_LAST         )
#define I2C_QUEUECMD_ACKNOWLEDGE                          (1 <<  bsI2C_QUEUECMD_ACKNOWLEDGE              )
#define I2C_QUEUECMD_PRE_ACK                              (1 <<  bsI2C_QUEUECMD_PRE_ACK                  )



/*
 * Descripton:
 * Bit shifts and widths for I2C_QUEUEDATA  0x90
 */

/*
 * Descripton:
 * Bit shifts and widths for I2C_DATA   0xa0
 */

/*
 * Descripton:
 * Bit shifts and widths for I2C_DEBUG0  0xb0
 */
#define bsI2C_DEBUG0_SLAVE_STATE                       0
#define bwI2C_DEBUG0_SLAVE_STATE                      10
#define bsI2C_DEBUG0_SLAVE_HOLD_CLK                   10
#define bwI2C_DEBUG0_SLAVE_HOLD_CLK                    1
#define bsI2C_DEBUG0_STATE_LATCH                      11
#define bwI2C_DEBUG0_STATE_LATCH                       1
#define bsI2C_DEBUG0_CHANGE_TOGGLE                    12
#define bwI2C_DEBUG0_CHANGE_TOGGLE                     1
#define bsI2C_DEBUG0_GRAB_TOGGLE                      13
#define bwI2C_DEBUG0_GRAB_TOGGLE                       1
#define bsI2C_DEBUG0_STOP_TOGGLE                      14
#define bwI2C_DEBUG0_STOP_TOGGLE                       1
#define bsI2C_DEBUG0_START_TOGGLE                     15
#define bwI2C_DEBUG0_START_TOGGLE                      1
#define bsI2C_DEBUG0_DMA_STATE                        16
#define bwI2C_DEBUG0_DMA_STATE                        10
#define bsI2C_DEBUG0_STATE_VALUE                      26
#define bwI2C_DEBUG0_STATE_VALUE                       2
#define bsI2C_DEBUG0_DMATERMINATE                     28
#define bwI2C_DEBUG0_DMATERMINATE                      1
#define bsI2C_DEBUG0_DMAKICK                          29
#define bwI2C_DEBUG0_DMAKICK                           1
#define bsI2C_DEBUG0_DMAENDCMD                        30
#define bwI2C_DEBUG0_DMAENDCMD                         1
#define bsI2C_DEBUG0_DMAREQ                           31
#define bwI2C_DEBUG0_DMAREQ                            1

#define I2C_DEBUG0_DMAREQ                              (1UL << bsI2C_DEBUG0_DMAREQ)


/*
 * Descripton:
 * Bit shifts and widths for I2C_DEBUG1  0xc0
 */
#define bsI2C_DEBUG1_FORCE_I2C_CLK_OE                    0
#define bwI2C_DEBUG1_FORCE_I2C_CLK_OE                    1
#define bsI2C_DEBUG1_FORCE_I2C_DATA_OE                   1
#define bwI2C_DEBUG1_FORCE_I2C_DATA_OE                   1
#define bsI2C_DEBUG1_FORCE_RCV_ACK                       2
#define bwI2C_DEBUG1_FORCE_RCV_ACK                       1
#define bsI2C_DEBUG1_FORCE_ARB_LOSS                      3
#define bwI2C_DEBUG1_FORCE_ARB_LOSS                      1
#define bsI2C_DEBUG1_FORCE_CLK_ON                        4
#define bwI2C_DEBUG1_FORCE_CLK_ON                        1
#define bsI2C_DEBUG1_LOCAL_SLAVE_TEST                    8
#define bwI2C_DEBUG1_LOCAL_SLAVE_TEST                    1
#define bsI2C_DEBUG1_LST_MODE                            9
#define bwI2C_DEBUG1_LST_MODE                            2
#define bsI2C_DEBUG1_CLK_GEN_STATE                      16
#define bwI2C_DEBUG1_CLK_GEN_STATE                       8
#define bsI2C_DEBUG1_DMA_BYTE_ENABLES                   24
#define bwI2C_DEBUG1_DMA_BYTE_ENABLES                    4
#define bsI2C_DEBUG1_I2C_DATA_IN                        30
#define bwI2C_DEBUG1_I2C_DATA_IN                         1
#define bsI2C_DEBUG1_I2C_CLK_IN                         31
#define bwI2C_DEBUG1_I2C_CLK_IN                          1

/*
 * Descripton:
 * Bit shifts and widths for I2C_VERSION  0xd0
 */
#define bsI2C_VERSION_STEP                               0
#define bwI2C_VERSION_STEP                              16
#define bsI2C_VERSION_MINOR                             16
#define bwI2C_VERSION_MINOR                              8
#define bsI2C_VERSION_MAJOR                             24
#define bwI2C_VERSION_MAJOR                              8


// I2c_CTRL0
void I2C_CTRL0_SET(I2C_TypeDef *I2C_BASE, uint32_t data);
void I2C_CTRL0_CLR(I2C_TypeDef *I2C_BASE, uint32_t data);
void I2C_CTRL0_TOG(I2C_TypeDef *I2C_BASE, uint32_t data);

// I2c_CTRL1
void I2C_CTRL1_SET(I2C_TypeDef *I2C_BASE, uint32_t data);
void I2C_CTRL1_CLR(I2C_TypeDef *I2C_BASE, uint32_t data);
void I2C_CTRL1_TOG(I2C_TypeDef *I2C_BASE, uint32_t data);

uint8_t GET_I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ(I2C_TypeDef *I2C_BASE);

// I2C_QUEUECTRL
void I2C_QUEUECTRL_SET(I2C_TypeDef *I2C_BASE, uint32_t data);
void I2C_QUEUECTRL_CLR(I2C_TypeDef *I2C_BASE, uint32_t data);
void I2C_QUEUECTRL_TOG(I2C_TypeDef *I2C_BASE, uint32_t data);

// I2C_QUEUECMD
void I2C_QUEUECMD_SET(I2C_TypeDef *I2C_BASE, uint32_t data);
void I2C_QUEUECMD_CLR(I2C_TypeDef *I2C_BASE, uint32_t data);
void I2C_QUEUECMD_TOG(I2C_TypeDef *I2C_BASE, uint32_t data);

// I2C_DEBUG0
void I2C_DEBUG0_SET(I2C_TypeDef *I2C_BASE, uint32_t data);
void I2C_DEBUG0_CLR(I2C_TypeDef *I2C_BASE, uint32_t data);
void I2C_DEBUG0_TOG(I2C_TypeDef *I2C_BASE, uint32_t data);

uint8_t GET_I2C_DEBUG0_DMAREQ(I2C_TypeDef *I2C_BASE);

#endif

#if INGCHIPS_FAMILY == INGCHIPS_FAMILY_916

#define I2C_FIFO_DEPTH      8

typedef enum
{
  I2C_ADDRESSING_MODE_07BIT,
  I2C_ADDRESSING_MODE_10BIT,
} I2C_AddressingMode;

typedef enum
{
    I2C_INT_FIFO_EMPTY ,
    I2C_INT_FIFO_FULL  ,
    I2C_INT_FIFO_HALF  ,
    I2C_INT_ADDR_HIT   ,
    I2C_INT_ARB_LOSE   ,
    I2C_INT_STOP       ,
    I2C_INT_START      ,
    I2C_INT_BYTE_TRANS ,
    I2C_INT_BYTE_RECV  ,
    I2C_INT_CMPL       ,
} I2C_IntBit;

typedef enum
{
    I2C_STATUS_FIFO_EMPTY = 0,
    I2C_STATUS_FIFO_FULL = 1,
    I2C_STATUS_ADDRHIT = 3, // indicates a transaction is addressed on this controller
    I2C_STATUS_CMPL = 9   , // indicates transaction completion
    I2C_STATUS_ACK  = 10  , // indicates the type of the last received/transmitted
    I2C_STATUS_BUS_BUSY   , // if the bus is busy
    I2C_STATUS_GEN_CALL   , // if the address of the current transaction is a general call address
    I2C_STATUS_LINE_SCL   , // current status of the SCL line on the bus
    I2C_STATUS_LINE_SDA   , // current status of the SDA line on the bus
} I2C_StatusBit;

typedef enum
{
    I2C_COMMAND_NO_ACTION,
    I2C_COMMAND_ISSUE_DATA_TRANSACTION, // master only
    I2C_COMMAND_RESPOND_ACK,
    I2C_COMMAND_RESPOND_NACK,
    I2C_COMMAND_CLEAR_FIFO,
    I2C_COMMAND_RESET,
} I2C_Command;

typedef enum
{
    I2C_ROLE_SLAVE,
    I2C_ROLE_MASTER,
} I2C_Role;

typedef enum
{
    I2C_TRANSACTION_MASTER2SLAVE,
    I2C_TRANSACTION_SLAVE2MASTER,
} I2C_TransactionDir;

typedef enum
{
    I2C_CLOCKFREQUENY_NULL,
    I2C_CLOCKFREQUENY_STANDARD,//100kbit/s
    I2C_CLOCKFREQUENY_FASTMODE,//400kbit/s
    I2C_CLOCKFREQUENY_FASTMODE_PLUS,//1Mbit/s
    I2C_CLOCKFREQUENY_MANUAL
} I2C_ClockFrequenyOptions;

#define bsI2C_CTRL_TRANSACTION_DIR               8
#define bsI2C_CTRL_MASTER_PRE_SEND_START         12
#define bsI2C_CTRL_MASTER_SEND_ADDR_AFTER_START  11
#define bsI2C_CTRL_MASTER_SEND_DATA_AFTER_ADDR   10
#define bsI2C_CTRL_MASTER_POST_SEND_STOP         9

#define I2C_CTRL_MASTER_ALL_PHASES  ((1 << bsI2C_CTRL_MASTER_PRE_SEND_START) \
                                    | (1 << bsI2C_CTRL_MASTER_SEND_ADDR_AFTER_START) \
                                    | (1 << bsI2C_CTRL_MASTER_SEND_DATA_AFTER_ADDR) \
                                    | (1 << bsI2C_CTRL_MASTER_POST_SEND_STOP))

// bit width of data cnt reg
#define bwI2C_CTRL_DATA_CNT                      8
// bit width of direction reg
#define bwI2C_CTRL_TRANSACTION_DIR               1

/**
 * @brief Configure I2C
 *
 * @param[in] I2C_BASE              base address
 * @param[in] role                  role
 * @param[in] addr_mode             addressing mode
 * @param[in] addr                  address
 */
void I2C_Config(I2C_TypeDef *I2C_BASE, I2C_Role role, I2C_AddressingMode addr_mode, uint16_t addr);

/**
 * @brief Configure I2C as slave
 *
 * Note: `I2C_INT_ADDR_HIT` & `I2C_INT_CMPL are enabled.
 *
 * @param[in] I2C_BASE              base address
 * @param[in] addr_mode             addressing mode
 * @param[in] addr                  address
 */
void I2C_ConfigSlave(I2C_TypeDef *I2C_BASE, I2C_AddressingMode addr_mode, uint8_t addr);

/**
 * @brief Enable/Disable I2C
 *
 * @param[in] I2C_BASE              base address
 * @param[in] enable                enable (1), disable (0)
 */
void I2C_Enable(I2C_TypeDef *I2C_BASE, uint8_t enable);

/**
 * @brief Get direction of last transaction (slave role only)
 *
 * @param[in] I2C_BASE              base address
 * @return                          direction of last transaction
 */
I2C_TransactionDir I2C_GetTransactionDir(I2C_TypeDef *I2C_BASE);

/**
 * @brief Enable/Disable interrupts of I2C
 *
 * @param[in] I2C_BASE              base address
 * @param[in] mask                  bits combinatoin of `I2C_IntBit`
 */
void I2C_IntEnable(I2C_TypeDef *I2C_BASE, uint32_t mask);
void I2C_IntDisable(I2C_TypeDef *I2C_BASE, uint32_t mask);
/**
 * @brief Get interrupts status of I2C
 *
 * @param[in] I2C_BASE              base address
 * @return                          interrupt status (bits combinatoin of `I2C_IntBit`)
 */
uint32_t I2C_GetIntState(I2C_TypeDef *I2C_BASE);

/**
 * @brief Clear interrupts of I2C
 *
 * @param[in] I2C_BASE              base address
 * @param[in] mask                  bits combinatoin of `I2C_IntBit`
 */
void I2C_ClearIntState(I2C_TypeDef *I2C_BASE, uint32_t mask);

/**
 * @brief Check if I2C transaction is completed
 *
 * @param[in] I2C_BASE              base address
 * @return                          1 if completed else 0
 */
int I2C_TransactionComplete(I2C_TypeDef *I2C_BASE);

/**
 * @brief Check if I2C FIFO is empty
 * @param[in] I2C_BASE              base address
 * @return                          1 if empty else 0
 */
int I2C_FifoEmpty(I2C_TypeDef *I2C_BASE);

/**
 * @brief Check if I2C FIFO is full
 * @param[in] I2C_BASE              base address
 * @return                          1 if full else 0
 */
int I2C_FifoFull(I2C_TypeDef *I2C_BASE);

/**
 * @brief Enable/Disable DMA transfer
 * @param[in] I2C_BASE              base address
 * @param[in] enable                enable(1)/disable(0)
 */
void I2C_DmaEnable(I2C_TypeDef *I2C_BASE, uint8_t enable);

/**
 * @brief Configure timing of I2C SCL generation
 *
 * @param[in] I2C_BASE              base address
 * @param[in] sci_hi                ref to datasheet
 * @param[in] scl_ratio             ref to datasheet
 * @param[in] hddat                 ref to datasheet
 * @param[in] sp                    ref to datasheet
 * @param[in] sudat                 ref to datasheet
 */
void I2C_ConfigSCLTiming( I2C_TypeDef *I2C_BASE, uint32_t scl_hi, uint32_t scl_ratio,
                         uint32_t hddat, uint32_t sp, uint32_t sudat);

/**
 * @brief Update the data number that need to be transferred or received
 * @param[in] I2C_BASE              base address
 * @param[in] cnt                   maximum 8 bit
 */
void I2C_CtrlUpdateDataCnt(I2C_TypeDef *I2C_BASE, uint8_t cnt);

/**
 * @brief Write a command to trigger an action
 * @param[in] I2C_BASE              base address
 * @param[in] cmd                   see I2C_Command
 */
void I2C_CommandWrite(I2C_TypeDef *I2C_BASE, uint8_t cmd);

/**
 * @brief Get Data from I2C FIFO
 * @param[in] I2C_BASE              base address
 */
void I2C_DataWrite(I2C_TypeDef *I2C_BASE, uint8_t data);
uint8_t I2C_DataRead(I2C_TypeDef *I2C_BASE);

/**
 * @brief Config a direction for controller
 * @param[in] I2C_BASE              base address
 * @param[in] dir                   see I2C_TransactionDir
 */
void I2C_CtrlUpdateDirection(I2C_TypeDef *I2C_BASE, I2C_TransactionDir dir);

/**
 * @brief Get data cnt number that has been transferred.
 * @param[in] I2C_BASE              base address
 */
uint8_t I2C_CtrlGetDataCnt(I2C_TypeDef *I2C_BASE);

/**
 * @brief Set clk frequency for controller.
 * @param[in] I2C_BASE              base address
 * @param[in] option                see I2C_ClockFrequenyOptions
 */
void I2C_ConfigClkFrequency(I2C_TypeDef *I2C_BASE, I2C_ClockFrequenyOptions option);
#endif

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

