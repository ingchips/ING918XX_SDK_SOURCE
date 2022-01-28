
#ifndef __PERIPHERAL_UART_H__
#define __PERIPHERAL_UART_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "ingsoc.h"

/*
 * Data read register UARTDR
 */
#define bwUART_DATA                    8   /* The actual data byte */
#define bsUART_DATA                    0

/* Not used, left in for completeness */
//#define bwUART_ERR_FRAME               1   /* Framing error */
//#define bwUART_ERR_PARITY              1   /* Parity error */
//#define bwUART_ERR_BREAK               1   /* Break error */
//#define bwUART_ERR_OVERRUN             1   /* Overrun error */

//#define bsUART_ERR_FRAME               8
//#define bsUART_ERR_PARITY              9
//#define bsUART_ERR_BREAK              10
//#define bsUART_ERR_OVERRUN            11
/* End of not used */


/*
 * Receive status / Error clear register UARTRSR/UARTECR
 */
#define bwUART_STAT_FRAME              1   /* Framing error */
#define bwUART_STAT_PARITY             1   /* Parity error */
#define bwUART_STAT_BREAK              1   /* Break error */
#define bwUART_STAT_OVERRUN            1   /* Overrun error */

#define bsUART_STAT_FRAME              0
#define bsUART_STAT_PARITY             1
#define bsUART_STAT_BREAK              2
#define bsUART_STAT_OVERRUN            3


/*
 * Flags register UARTFR
 */
#define bwUART_CLEAR_TO_SEND           1   /* Clear to send */
#define bwUART_READY                   1   /* Data set ready */
#define bwUART_CARRIER                 1   /* Data carrier detect */
#define bwUART_BUSY                    1   /* UART busy */
#define bwUART_RECEIVE_EMPTY           1   /* Receive FIFO empty */
#define bwUART_TRANSMIT_FULL           1   /* Transmit FIFO full */
#define bwUART_RECEIVE_FULL            1   /* Receive FIFO full */
#define bwUART_TRANSMIT_EMPTY          1   /* Transmit FIFO empty */
#define bwUART_RING                    1   /* Complement of UART ring indicator modem status input */

#define bsUART_CLEAR_TO_SEND           0
#define bsUART_READY                   1
#define bsUART_CARRIER                 2
#define bsUART_BUSY                    3
#define bsUART_RECEIVE_EMPTY           4
#define bsUART_TRANSMIT_FULL           5
#define bsUART_RECEIVE_FULL            6  /* Not used, left in for completeness */
#define bsUART_TRANSMIT_EMPTY          7
#define bsUART_RING                    8  /* Not used, left in for completeness */


/*
 * Low power divisor register UARTILPR
 */
#define bwUART_LP_DIVISOR              8   /* IrDA low power divisor */


/*
 * Baud rate divisor register
 */
#define bwUART_BAUD_DIVISOR           16   /* PL_011 Baud rate divisor */


/*
 * Line control register UARTLCR_H
 */
#define bwUART_SEND_BREAK              1   /* Send break */
#define bwUART_PARITY_SELECT           2   /* Even, odd or no parity checking */
#define bwUART_TWO_STOP_BITS           1   /* Transmit two stop bits at end of frame */
#define bwUART_FIFO_ENABLE             1   /* FIFO enable */
#define bwUART_WORD_LENGTH             2   /* Word length */
#define bwUART_STICK_PARITY            1   /* Stick parity */

#define bsUART_SEND_BREAK              0
#define bsUART_PARITY_SELECT           1
#define bsUART_TWO_STOP_BITS           3
#define bsUART_FIFO_ENABLE             4
#define bsUART_WORD_LENGTH             5
#define bsUART_STICK_PARITY            7


/*
 * Control register UARTCR
 */
#define bwUART_ENABLE                  1   /* UART master enable */
#define bwUART_SIR_ENABLE              1   /* SIR enable     - Not Used on Integrator */
#define bwUART_LOW_POWER_MODE          1   /* IrDA encoding low power mode - Not Used on Integrator */
#define bwUART_LOOP_BACK               1   /* Loop back enable */

#define bsUART_ENABLE                  0
#define bsUART_SIR_ENABLE              1
#define bsUART_LOW_POWER_MODE          2
#define bsUART_LOOP_BACK               7


#define bwUART_CONTROL_LOWBITS         3   /* Used to allow rapid setting of control register */
#define bwUART_CONTROL_HIGHBITS        9   /* without disturbing reserved bits */
#define bwUART_TRANSMIT_ENABLE         1   /* Transmit enable */
#define bwUART_RECEIVE_ENABLE          1   /* Receive enable */
#define bwUART_TRANSMIT_READY          1   /* Data transmit ready */
#define bwUART_REQUEST_SEND            1   /* Request to Send */
#define bwUART_OUT1                    1   /* Complement of UART Out1 modem status output */
#define bwUART_OUT2                    1   /* Complement of UART Out2 modem status output */
#define bwUART_HWFLOW_ENABLE           2   /* HW flow control enable/disable */

#define bsUART_CONTROL_LOWBITS         0
#define bsUART_CONTROL_HIGHBITS        7
#define bsUART_TRANSMIT_ENABLE         8
#define bsUART_RECEIVE_ENABLE          9
#define bsUART_TRANSMIT_READY         10
#define bsUART_REQUEST_SEND           11
#define bsUART_OUT1                   12
#define bsUART_OUT2                   13
#define bsUART_HWFLOW_ENABLE          14
#define bsUART_RTS_ENA                14
#define bsUART_CTS_ENA                15


/*
 * Interrupt identification / clear register UARTIIR/UARTICR
 */
#define bwUART_MODEMRI                 1   /* Modem RI interrupt */
#define bwUART_MODEMCTS                1   /* Modem CTS interrupt */
#define bwUART_MODEMDCD                1   /* Modem DCD interrupt */
#define bwUART_MODEMDSR                1   /* Modem DSR interrupt */
#define bwUART_RECEIVE                 1   /* Receive interrupt */
#define bwUART_TRANSMIT                1   /* Transmit interrupt */
#define bwUART_TIMEOUT                 1   /* Receive timeout interrupt */
#define bwUART_FRAME                   1   /* Framing error */
#define bwUART_PARITY                  1   /* Parity error */
#define bwUART_BREAK                   1   /* Break error */
#define bwUART_OVERRUN                 1   /* Overrun error */

#define bwUART_MODEM                   4   /* All Modem interrupts */

/* Not used, left in for completeness */
//#define bwUART_ERROR                   4   /* All Error interrupts */

#define bwUART_ALL                    11   /* All interrupts */

#define bsUART_MODEMRI                 0
#define bsUART_MODEMCTS                1
#define bsUART_MODEMDCD                2
#define bsUART_MODEMDSR                3
#define bsUART_RECEIVE                 4
#define bsUART_TRANSMIT                5
#define bsUART_TIMEOUT                 6
#define bsUART_FRAME                   7
#define bsUART_PARITY                  8
#define bsUART_BREAK                   9
#define bsUART_OVERRUN                10

#define bsUART_MODEM                   0

/* Not used, left in for completeness */
//#define bsUART_ERROR                   7

#define bsUART_ALL                     0



/* Interrupt Mask */
/* PL_011 has seperate mask set/clear register UARTIMSC*/
#define bwUART_MODEM_INTENAB           4   /* Modem interrupt enable */
#define bwUART_RECEIVE_INTENAB         1   /* Receive interrupt enable */
#define bwUART_TRANSMIT_INTENAB        1   /* Transmit interrupt enable */
#define bwUART_TIMEOUT_INTENAB         1   /* Receive timeout interrupt enable */
#define bwUART_ERROR_INTENAB           4
#define bwUART_FRAME_INTENAB           1   /* Framing error */
#define bwUART_PARITY_INTENAB          1   /* Parity error */
#define bwUART_BREAK_INTENAB           1   /* Break error */
#define bwUART_OVERRUN_INTENAB         1   /* Overrun error */
#define bwUART_ALL_INTENAB            11   /* All interrupts */


//p64
#define bsUART_MODEM_INTENAB           0
#define bsUART_RECEIVE_INTENAB         4
#define bsUART_TRANSMIT_INTENAB        5
#define bsUART_TIMEOUT_INTENAB         6
#define bsUART_ERROR_INTENAB           7
#define bsUART_FRAME_INTENAB           7
#define bsUART_PARITY_INTENAB          8
#define bsUART_BREAK_INTENAB           9
#define bsUART_OVERRUN_INTENAB        10
#define bsUART_ALL_INTENAB             0



#define UART_RECEIVE_INTCLR           4



/* Used by INTTEST within ISR */
#define UART_INTBIT_RECEIVE            0x10
#define UART_INTBIT_TRANSMIT           0x20
#define UART_INTBIT_TIMEOUT            0x40
#define UART_INTBIT_MODEM              0xF
#define UART_INTBIT_MODEMRI            0x1
#define UART_INTBIT_MODEMCTS           0x2
#define UART_INTBIT_MODEMDCD           0x4
#define UART_INTBIT_MODEMDSR           0x8
#define UART_INTBIT_ERROR              0x780 // bit7-bit10
#define UART_INTBIT_FRAME              0x80
#define UART_INTBIT_PARITY             0x100
#define UART_INTBIT_BREAK              0x200
#define UART_INTBIT_OVERRUN            0x400


/*
 * Descripton:
 * Bit shifts and widths for DMA Control Register UARTDMACR
 */
#define bsUART_DMA_ON_ERROR_DISABLE        2
#define bwUART_DMA_ON_ERROR_DISABLE        1
#define bsUART_DMA_TRANSMIT_ENABLE         1
#define bwUART_DMA_TRANSMIT_ENABLE         1
#define bsUART_DMA_RECEIVE_ENABLE          0
#define bwUART_DMA_RECEIVE_ENABLE          1


/*
 * FIFO select register UARTIFLS
 */
#define bwUART_TRANS_INT_LEVEL        3   /* Interrupt level for Transmit FIFO */
#define bwUART_RECV_INT_LEVEL         3   /* Interrupt level for Receive FIFO */

#define bsUART_TRANS_INT_LEVEL        0
#define bsUART_RECV_INT_LEVEL         8   // 3

typedef enum UART_xWLEN
{
    UART_WLEN_8_BITS = 3,
    UART_WLEN_7_BITS = 2,
    UART_WLEN_6_BITS = 1,
    UART_WLEN_5_BITS = 0
} UART_eWLEN;


// SPS EPS PEN
typedef enum UART_xPARITY
{
    UART_PARITY_NOT_CHECK   = 0,
    UART_PARITY_EVEN_PARITY = 3,
    UART_PARITY_ODD_PARITY  = 1,
    UART_PARITY_FIX_ONE     = 5,
    UART_PARITY_FIX_ZERO    = 7
} UART_ePARITY;

//p63
typedef enum UART_xFIFO_WATERLEVEL
{
	  UART_FIFO_ONE_EIGHTH    = 0,
	  UART_FIFO_ONE_FOURTH    = 1,
	  UART_FIFO_ONE_SECOND    = 2,
	  UART_FIFO_THREE_FOURTH  = 3,
	  UART_FIFO_SEVEN_EIGHTH  = 4
}UART_eFIFO_WATERLEVEL;

/*
 * UART status
 */
typedef struct UART_xStateStruct
{
	// Line Control Register, UARTLCR_H
  UART_eWLEN      word_length;   // WLEN
  UART_ePARITY    parity;        // PEN, EPS, SPS
  uint8_t         fifo_enable;   // FEN
  uint8_t         two_stop_bits; // STP2
  // Control Register, UARTCR
  uint8_t         receive_en;        // RXE
  uint8_t         transmit_en;       // TXE
  uint8_t         UART_en;           // UARTEN
  uint8_t         cts_en;            //CTSEN
  uint8_t         rts_en;            //RTSEN
  // Interrupt FIFO Level Select Register, UARTIFLS
  uint8_t         rxfifo_waterlevel; // RXIFLSEL
  uint8_t         txfifo_waterlevel; // TXIFLSEL
  //UART_eFIFO_WATERLEVEL    rxfifo_waterlevel; // RXIFLSEL
  //UART_eFIFO_WATERLEVEL    txfifo_watchlevel; // TXIFLSEL

  // UART Clock Frequency
  uint32_t        ClockFrequency;
  uint32_t        BaudRate;

} UART_sStateStruct;

void apUART_BaudRateSet(UART_TypeDef* pBase, uint32_t ClockFrequency, uint32_t BaudRate);
uint32_t apUART_BaudRateGet (UART_TypeDef* pBase, uint32_t ClockFrequency);
uint8_t apUART_Check_Rece_ERROR(UART_TypeDef* pBase);
uint8_t apUART_Check_RXFIFO_EMPTY(UART_TypeDef* pBase);
uint8_t apUART_Check_RXFIFO_FULL(UART_TypeDef* pBase);
uint8_t apUART_Check_TXFIFO_EMPTY(UART_TypeDef* pBase);
uint8_t apUART_Check_TXFIFO_FULL(UART_TypeDef* pBase);

void apUART_Enable_TRANSMIT_INT(UART_TypeDef* pBase);
void apUART_Disable_TRANSMIT_INT(UART_TypeDef* pBase);
void apUART_Enable_RECEIVE_INT(UART_TypeDef* pBase);
void apUART_Disable_RECEIVE_INT(UART_TypeDef* pBase);

void apUART_Clr_RECEIVE_INT(UART_TypeDef* pBase);
void apUART_Clr_TX_INT(UART_TypeDef* pBase);
void apUART_Clr_NonRx_INT(UART_TypeDef* pBase);

uint8_t apUART_Get_ITStatus(UART_TypeDef* pBase,uint8_t UART_IT);
uint32_t apUART_Get_all_raw_int_stat(UART_TypeDef* pBase);

uint8_t apUART_Check_BUSY(UART_TypeDef* pBase);
void apUART_Initialize(UART_TypeDef* pBase, UART_sStateStruct* UARTx, uint32_t IntMask);

void UART_SendData(UART_TypeDef* pBase, uint8_t Data);
uint8_t UART_ReceData(UART_TypeDef* pBase);
void uart_reset(UART_TypeDef* pBase);

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
/**
 * @brief Enable/Disable DMA transfer
 * @param[in] pBase                 base address
 * @param[in] tx_enable             enable(1)/disable(0) on TX
 * @param[in] rx_enable             enable(1)/disable(0) on RX
 * @param[in] dma_on_err            DMA on error
 */
void UART_DmaEnable(UART_TypeDef *pBase, uint8_t tx_enable, uint8_t rx_enable, uint8_t dma_on_err);
#endif

#ifdef __cplusplus
  }
#endif

#endif // __CM32GPM0_UART_H__
