
/*
** COPYRIGHT (c) 2023 by INGCHIPS
*/

#ifndef __PERIPHERAL_KEYSCAN_H__
#define __PERIPHERAL_KEYSCAN_H__

#include "ingsoc.h"

#ifdef __cplusplus
    extern "C" {
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

#elif ((INGCHIPS_FAMILY == INGCHIPS_FAMILY_916) || (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920))

typedef enum
{
    KEY_IN_COL_0  ,
    KEY_IN_COL_1  ,
    KEY_IN_COL_2  ,
    KEY_IN_COL_3  ,
    KEY_IN_COL_4  ,
    KEY_IN_COL_5  ,
    KEY_IN_COL_6  ,
    KEY_IN_COL_7  ,
    KEY_IN_COL_8  ,
    KEY_IN_COL_9  ,
    KEY_IN_COL_10  ,
    KEY_IN_COL_11  ,
    KEY_IN_COL_12  ,
    KEY_IN_COL_13  ,
    KEY_IN_COL_14  ,
    KEY_IN_COL_15  ,
    KEY_IN_COL_16  ,
    KEY_IN_COL_17  ,
    KEY_IN_COL_18  ,
    KEY_IN_COL_19  ,
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)
    KEY_IN_COL_20  ,
    KEY_IN_COL_21  ,
#endif
    KEY_IN_COL_NUMBER,
} KEYSCAN_InColIndex_t;

typedef enum
{
    KEY_OUT_ROW_0  ,
    KEY_OUT_ROW_1  ,
    KEY_OUT_ROW_2  ,
    KEY_OUT_ROW_3  ,
    KEY_OUT_ROW_4  ,
    KEY_OUT_ROW_5  ,
    KEY_OUT_ROW_6  ,
    KEY_OUT_ROW_7  ,
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    KEY_OUT_ROW_8  ,
    KEY_OUT_ROW_9  ,
    KEY_OUT_ROW_10  ,
    KEY_OUT_ROW_11  ,
    KEY_OUT_ROW_12  ,
    KEY_OUT_ROW_13  ,
    KEY_OUT_ROW_14  ,
    KEY_OUT_ROW_15  ,
    KEY_OUT_ROW_16  ,
    KEY_OUT_ROW_17  ,
    KEY_OUT_ROW_18  ,
    KEY_OUT_ROW_19  ,
#endif
    KEY_OUT_ROW_NUMBER,
} KEYSCAN_OutRowIndex_t;

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)
typedef  enum {
    SCAN_HIGH,
    SCAN_LOW,
    SCAN_NUMBER,
}KEYSCAN_ScanMode_t;
typedef enum {
    NORMAL_MODE,
    LPKEY_MODE,
}KEYSCAN_RunMode_t;
#endif

typedef struct {
    KEYSCAN_InColIndex_t in_col;
    GIO_Index_t gpio;
} KEYSCAN_InColList;

typedef struct {
    KEYSCAN_OutRowIndex_t out_row;
    GIO_Index_t gpio;
} KEYSCAN_OutRowList;

typedef struct
{
    uint8_t row_to_idx[KEY_OUT_ROW_NUMBER];
    uint8_t col_to_idx[KEY_IN_COL_NUMBER];
} KEYSCAN_Ctx;

typedef struct {
    KEYSCAN_InColList *col;
    int col_num;

    KEYSCAN_OutRowList *row;
    int row_num;

    uint8_t fifo_num_trig_int;
    uint8_t dma_num_trig_int;
    uint8_t loop_num_trig_int;
    uint8_t dma_en;
    uint8_t int_trig_en;
    uint8_t int_loop_en;
    uint16_t release_time;
    uint16_t scan_interval;
    uint8_t debounce_counter;
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)
    uint8_t table_mode_en;
    uint8_t lpkey_mode_en;
#endif
} KEYSCAN_SetStateStruct;

/**
 * @brief Initialize keyscan module
 *
 * @param[in] keyscan_set       Initial parameter struct
 * @return                      0 if success else non-0
 */
int KEYSCAN_InitializeScanParameter(const KEYSCAN_SetStateStruct* keyscan_set);

/**
 * @brief Initialize keyscan module and start scan
 *
 * @param[in] keyscan_set       Initial parameter struct
 * @return                      0 if success else non-0
 */
int KEYSCAN_Initialize(const KEYSCAN_SetStateStruct* keyscan_set);

/**
 * @brief Clear fifo.
 *
 * @return                      none
 */
void KEYSCAN_SetFifoClrReg(void);

/**
 * @brief Clear loop interrupt status.
 *
 * @return                      none
 */
void KEYSCAN_LoopIntClr(void);

/**
 * @brief Keyscan loop interrupt enable/disable.
 *
 * @return                      none
 */
void KEYSCAN_LoopIntEn(uint8_t enable);

/**
 * @brief Check keyscan FIFO full or not
 *
 * @return                      0: FIFO no data; 1: FIFO is full
 */
uint8_t KEYSCAN_GetIntStateFifoFullRaw(void);

/**
 * @brief Check keyscan FIFO empty or not
 *
 * @return                      0: FIFO have data; 1: empty
 */
uint8_t KEYSCAN_GetIntStateFifoEmptyRaw(void);

/**
 * @brief Check keyscan loop interrupt is triggered or not
 *
 * @return                      0: Not triggerred; 1: Triggerred.
 */
uint8_t KEYSCAN_GetIntStateLoopTrig(void);

/**
 * @brief GET keyscan FIFO data
 *
 * @return                      0~4 bits: col; 5~9 bits: row; 10 bit: scan cycle end flag
 */
uint16_t KEYSCAN_GetKeyData(void);

/**
 * @brief Initialize mapping table of keyboard array row and col
 *
 * @param[in]  keyscan_set       Initial parameter struct
 * @param[out] ctx               keyboard array mapping table
 */
void KEYSCAN_InitKeyScanToIdx(const KEYSCAN_SetStateStruct* keyscan_set, KEYSCAN_Ctx *ctx);

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
/**
 * @brief Transfer keyscan FIFO raw data to keyboard array row and col
 *
 * To use this helper function, `ctx` must be initialized with `KEYSCAN_InitKeyScanToIdx`.
 *
 * @param[in]  ctx              keyboard array mapping table
 * @param[in]  key_data         keyscan FIFO raw data
 * @param[out] row              pressed key's 0-based row index in keyboard array
 * @param[out] col              pressed key's 0-based col index in keyboard array
 * @return                      0: scan cycle end data;
 *                              1: find key pressed, *row and *col are key positions in keyboard array
 */
uint8_t KEYSCAN_KeyDataToRowColIdx(const KEYSCAN_Ctx *ctx, uint32_t key_data, uint8_t *row, uint8_t *col);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)
/**
 * @brief Gets the value of the key scan mode
 *
 * @param[in]  table_en          keyscan table mode enable?
 * @param[in]  scan_mode         key scan mode
 * @param[out] key_data          keyscan FIFO raw data
 * @return                       0: scan cycle end data or error;
 *                               1: find key pressed, *row and *col are key positions in keyboard array
 */
uint8_t KEYSCAN_GetScanMode(uint8_t table_en, KEYSCAN_ScanMode_t* scan_mode, uint32_t key_data);
/**
 * @brief In normal mode, transfer keyscan FIFO raw data to keyboard array row and col
 *
 * To use this helper function, `ctx` must be initialized with `KEYSCAN_InitKeyScanToIdx`.
 *
 * @param[in]  ctx              keyboard array mapping table
 * @param[in]  key_data         keyscan FIFO raw data
 * @param[out] row              pressed key's 0-based row index in keyboard array
 * @param[out] col              pressed key's 0-based col index in keyboard array
 * @return                      0: scan key vale is row
 *                              1: scan key vale is col
 */
uint8_t KEYSCAN_NormalHighLowDataToIdex(const KEYSCAN_Ctx *ctx, uint32_t key_data, uint8_t *row, uint8_t *col);
/**
 * @brief In table mode, transfer keyscan FIFO raw data to keyboard array row and col
 *
 * To use this helper function, `ctx` must be initialized with `KEYSCAN_InitKeyScanToIdx`.
 *
 * @param[in]  ctx              keyboard array mapping table
 * @param[in]  key_data         keyscan FIFO raw data
 * @param[out] row              pressed key's 0-based row index in keyboard array
 * @param[out] col              pressed key's 0-based col index in keyboard array
 * @param[out] key_stae         key state press or unpress
 * @return                      0: scan key vale is row
 *                              1: scan key vale is col
 */
uint8_t KEYSCAN_TableHighLowDataToIdex(const KEYSCAN_Ctx *ctx, uint32_t key_data, uint8_t *row, uint8_t *col, uint8_t *key_stae);
/**
 * @brief Transfer keyscan FIFO raw data to keyboard array row and col
 *
 * To use this helper function, `ctx` must be initialized with `KEYSCAN_InitKeyScanToIdx`.
 *
 * @param[in]  ctx              keyboard array mapping table
 * @param[in]  mode             keyscan mode lpkey or normal
 * @param[in]  key_data         keyscan FIFO raw data
 * @param[out] row              pressed key's 0-based row index in keyboard array
 * @param[out] col              pressed key's 0-based col index in keyboard array
 *
 */
void KEYSCAN_ScanDataToIdex(const KEYSCAN_Ctx *ctx, KEYSCAN_RunMode_t mode, uint32_t key_data, uint8_t *row, uint8_t *col);
#endif
/**
 * @brief Set keyscan start scan
 *
 * @param[in] enable            0: disable scan; 1: enable scan
 */
void KEYSCAN_SetScannerEn(uint8_t enable);

/**
 * @brief Set the interval between two keyscan loops.
 * 
 * The time of interval is calculated by interval_time(ms) = scan_itv / keyscan_clk(khz), 
 * and keyscan_clk is keyscan's clock, which selected by 'SYSCTRL_SelectKeyScanClk'.
 * 
 * @note Initialize keyscan module will also change this interval, such as 
 *      'KEYSCAN_InitializeScanParameter' or 'KEYSCAN_Initialize'.
 *
 * @param[in] scan_itv          the clock count for keyscan interval. range: 0 ~ 65535
 */
void KEYSCAN_SetScanInterval(uint32_t scan_itv);

#endif

#ifdef __cplusplus
    }
#endif

#endif
