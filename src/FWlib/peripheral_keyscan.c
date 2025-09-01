/*
** COPYRIGHT (c) 2023 by INGCHIPS
*/

#include "peripheral_keyscan.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

#elif ((INGCHIPS_FAMILY == INGCHIPS_FAMILY_916) || (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920))

static void KEYSCAN_reg_write_bits(volatile uint32_t *reg, uint8_t offset, uint8_t bits_width, const uint32_t data)
{
    uint32_t mask;

    mask = 0xFFFFFFFF;
    mask = mask >> (32 - bits_width);

    *reg = (*reg & (~(mask << offset))) | ((data & mask) << offset);
    return;
}

static uint32_t KEYSCAN_reg_read_bits(volatile uint32_t *reg, uint8_t offset, uint8_t bits_width)
{
    uint32_t data;
    uint32_t mask;

    mask = 0xFFFFFFFF;
    mask = mask >> (32 - bits_width);

    data = ((*reg) >> offset) & mask;
    return data;
}

void KEYSCAN_SetScannerEn(uint8_t enable)
{
    uint8_t offset = 0;
    uint8_t bits_width = 1;
    uint32_t data = (enable == 0) ? 0: 1;

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_scanner_ctrl0, offset, bits_width, data);
    return;
}

uint8_t KEYSCAN_GetScannerEn(void)
{
    uint8_t offset = 0;
    uint8_t bits_width = 1;
    uint8_t ret;

    ret = KEYSCAN_reg_read_bits(&APB_KEYSCAN->key_scanner_ctrl0, offset, bits_width);
    return ret;
}
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)
uint8_t  KEYSCAN_SetTableModeEn(uint8_t enable)
{
    uint8_t offset = 1;
    uint8_t bits_width = 1;
    uint32_t data = (enable == 0) ? 0: 1;

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_scanner_ctrl0, offset, bits_width, data);
    return 0;
}

uint8_t  KEYSCAN_SetLpkeyModeEn(uint8_t enable)
{
    uint8_t offset = 2;
    uint8_t bits_width = 1;
    uint32_t data = (enable == 0) ? 0: 1;

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_scanner_ctrl0, offset, bits_width, data);
    return 0;
}

uint8_t  KEYSCAN_SetPeReg(uint8_t enable)
{
    uint8_t offset = 3;
    uint8_t bits_width = 1;
    uint32_t data = (enable == 0) ? 0: 1;

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_scanner_ctrl0, offset, bits_width, data);
    return 0;
}

uint8_t  KEYSCAN_SetPsReg(uint8_t pull)
{
    uint8_t offset = 4;
    uint8_t bits_width = 1;
    uint32_t data = (pull == 0) ? 0: 1;

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_scanner_ctrl0, offset, bits_width, data);
    return 0;
}
#endif
void KEYSCAN_SetDebounceEn(uint32_t debounce_en_bits)
{
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    uint8_t offset = 1;
    uint8_t bits_width = 20;

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_scanner_ctrl0, offset, bits_width, debounce_en_bits);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)
    uint8_t offset = 0;
    uint8_t bits_width = 30;

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_debouncd_cfg0, offset, bits_width, debounce_en_bits);
#endif
    return;
}

void KEYSCAN_SetDebounceCounter(uint32_t cnt)
{
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    uint8_t offset = 21;
    uint8_t bits_width = 8;

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_scanner_ctrl0, offset, bits_width, cnt);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)
    uint8_t offset = 0;
    uint8_t bits_width = 8;

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_scanner_ctrl1, offset, bits_width, cnt);
#endif
    return;
}

void KEYSCAN_SetDmaEn(uint8_t enable)
{
    uint8_t offset = 29;
    uint8_t bits_width = 1;
    uint32_t data = (enable == 0) ? 0: 1;

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_scanner_ctrl0, offset, bits_width, data);
    return;
}

void KEYSCAN_DbClkSel(uint8_t sel)
{
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    uint8_t offset = 30;
    uint8_t bits_width = 1;
    uint32_t data = (sel == 0) ? 0: 1;

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_scanner_ctrl0, offset, bits_width, data);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)
    uint8_t offset = 8;
    uint8_t bits_width = 1;
    uint32_t data = (sel == 0) ? 0: 1;

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_debouncd_cfg1, offset, bits_width, data);
#endif
    return;
}

void KEYSCAN_SetReleaseTime(uint32_t time)
{
    uint8_t offset = 0;
    uint8_t bits_width = 11;

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_scanner_ctrl1, offset, bits_width, time);
    return;
}

void KEYSCAN_SetScanInterval(uint32_t scan_itv)
{
    uint8_t offset = 11;
    uint8_t bits_width = 16;

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_scanner_ctrl1, offset, bits_width, scan_itv);
    return;
}

void KEYSCAN_SetOutRowMask(uint32_t row_mask)
{
    uint8_t offset = 0;
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    uint8_t bits_width = 20;
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)
    uint8_t bits_width = 8;
#endif

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_row_mask_ctrl, offset, bits_width, row_mask);
    return;
}

void KEYSCAN_SetInColMask(uint32_t col_mask)
{
    uint8_t offset = 0;
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    uint8_t bits_width = 20;
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)
    uint8_t bits_width = 22;
#endif

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_col_mask_ctrl, offset, bits_width, col_mask);
    return;
}

void KEYSCAN_SetIntTrigEn(uint8_t enable)
{
    uint8_t offset = 0;
    uint8_t bits_width = 1;
    uint32_t data = (enable == 0) ? 0: 1;

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_int_en, offset, bits_width, data);
    return;
}

void KEYSCAN_SetFifoClrReg(void)
{
    uint8_t offset = 4;
    uint8_t bits_width = 1;
    uint32_t data = 1;

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_int_en, offset, bits_width, data);
    return;
}

void KEYSCAN_LoopIntClr(void)
{
    uint8_t offset = 5;
    uint8_t bits_width = 1;
    uint32_t data = 1;

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_int_en, offset, bits_width, data);
    return;
}

void KEYSCAN_LoopIntEn(uint8_t enable)
{
    uint8_t offset = 6;
    uint8_t bits_width = 1;
    uint32_t data = (enable == 0) ? 0: 1;

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_int_en, offset, bits_width, data);
    return;
}

uint8_t KEYSCAN_GetIntStateFifoFullRaw(void)
{
    uint8_t offset = 5;
    uint8_t bits_width = 1;
    uint8_t ret;

    ret = KEYSCAN_reg_read_bits(&APB_KEYSCAN->key_int, offset, bits_width);
    return ret;
}

uint8_t KEYSCAN_GetIntStateFifoEmptyRaw(void)
{
    uint8_t offset = 6;
    uint8_t bits_width = 1;
    uint8_t ret;

    ret = KEYSCAN_reg_read_bits(&APB_KEYSCAN->key_int, offset, bits_width);
    return ret;
}

uint8_t KEYSCAN_GetIntStateLoopTrig(void)
{
    uint8_t offset = 8;
    uint8_t bits_width = 1;
    uint8_t ret;

    ret = KEYSCAN_reg_read_bits(&APB_KEYSCAN->key_int, offset, bits_width);
    return ret;
}

uint8_t KEYSCAN_GetKeyScannerBusy(void)
{
    uint8_t offset = 6;
    uint8_t bits_width = 1;
    uint8_t ret;

    ret = KEYSCAN_reg_read_bits(&APB_KEYSCAN->fifo_status, offset, bits_width);
    return ret;
}

uint16_t KEYSCAN_GetKeyData(void)
{
    uint8_t offset = 0;
    uint8_t bits_width = 11;
    uint16_t ret;

    ret = KEYSCAN_reg_read_bits(&APB_KEYSCAN->key_data, offset, bits_width);
    return ret;
}

void KEYSCAN_SetFifoNumTrigInt(uint32_t trig_num)
{
    uint8_t offset = 0;
    uint8_t bits_width = 5;

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_trig, offset, bits_width, trig_num);
    return;
}

void KEYSCAN_SetDmaNumTrigInt(uint32_t trig_num)
{
    uint8_t offset = 5;
    uint8_t bits_width = 5;

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_trig, offset, bits_width, trig_num);
    return;
}

void KEYSCAN_SetLoopNumTrigInt(uint32_t trig_num)
{
    uint8_t offset = 10;
    uint8_t bits_width = 3;

    KEYSCAN_reg_write_bits(&APB_KEYSCAN->key_trig, offset, bits_width, trig_num);
    return;
}

static uint8_t KEYSCAN_CheckStatePara(const KEYSCAN_SetStateStruct* keyscan_set)
{
    if (keyscan_set == 0) {
        return 1;
    }

    if ((keyscan_set->row == 0) || (keyscan_set->col == 0)) {
        return 1;
    }

    return 0;
}

void KEYSCAN_InitKeyScanToIdx(const KEYSCAN_SetStateStruct* keyscan_set, KEYSCAN_Ctx *ctx)
{
    int i;

    for (i = 0; i < keyscan_set->col_num; i++) {
        ctx->col_to_idx[keyscan_set->col[i].in_col] =  i;
    }

    for (i = 0; i < keyscan_set->row_num; i++) {
        ctx->row_to_idx[keyscan_set->row[i].out_row] =  i;
    }

    return;
}

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

uint8_t KEYSCAN_KeyDataToRowColIdx(const KEYSCAN_Ctx *ctx, uint32_t key_data, uint8_t *row, uint8_t *col)
{

    if (key_data == 0x00000400) {
        return 0;
    }

    *row = ctx->row_to_idx[(key_data >> 5) & 0x1f];
    *col = ctx->col_to_idx[key_data & 0x1f];


        return 1;
    }
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)

uint8_t KEYSCAN_GetScanMode(uint8_t table_en, KEYSCAN_ScanMode_t* scan_mode, uint32_t key_data)
{
    uint8_t key_val = 0;
    key_val = (key_data>>8) & 0xf;
    if(table_en){
        switch (key_val) {
            case 0x3:
                *scan_mode = SCAN_HIGH;
            case 0x2:
                *scan_mode = SCAN_LOW;
            case 0x0:
                *scan_mode = SCAN_NUMBER;
            case 0x4:
                return 0;
            default:
                return 0;
        }
    } else {
        switch (key_val) {
            case 0x7:
                *scan_mode = SCAN_HIGH;
                break;
            case 0x6:
                *scan_mode = SCAN_LOW;
                break;
            case 0x4:
                *scan_mode = SCAN_NUMBER;
                break;
            case 0x3:
                *scan_mode = SCAN_HIGH;
                break;
            case 0x2:
                *scan_mode = SCAN_LOW;
                break;
            case 0x0:
                *scan_mode = SCAN_NUMBER;
                break;
            default:
                return 0;
        }
    }
    return 1;
}

uint8_t KEYSCAN_NormalHighLowDataToIdex(const KEYSCAN_Ctx *ctx, uint32_t key_data, uint8_t *row, uint8_t *col)
{
    if((key_data & 0x1f) > (KEY_IN_COL_NUMBER - 1)){
        *row = ctx->row_to_idx[((key_data & 0x1f) - KEY_IN_COL_NUMBER + 1)];
        return 1;
    }
    else{
        *col = ctx->col_to_idx[(key_data & 0x1f)];
    return 0;
}
}

uint8_t KEYSCAN_TableHighLowDataToIdex(const KEYSCAN_Ctx *ctx, uint32_t key_data, uint8_t *row, uint8_t *col, uint8_t *key_stae)
{
    uint8_t key_val = 0;
    key_val = (key_data>>8) & 0xf;
    if((key_val == 0x7) || (key_val == 0x6)){
        *key_stae = 1;
    } else {
        *key_stae = 0;
    }

    if((key_data & 0x1f) > (KEY_IN_COL_NUMBER - 1)){
        *row = ctx->row_to_idx[((key_data & 0x1f) - KEY_IN_COL_NUMBER + 1)];
        return 1;
    }
    else{
        *col = ctx->col_to_idx[(key_data & 0x1f)];
        return 0;
    }


}

void KEYSCAN_ScanDataToIdex(const KEYSCAN_Ctx *ctx, KEYSCAN_RunMode_t mode, uint32_t key_data, uint8_t *row, uint8_t *col)
{
    if(mode == NORMAL_MODE){
        *row = ctx->row_to_idx[(key_data >> 5) & 0x07];
    *col = ctx->col_to_idx[key_data & 0x1f];
    } else if (mode == LPKEY_MODE) {
        *row = ctx->row_to_idx[(key_data >> 4) & 0xf];
        *col = ctx->col_to_idx[key_data & 0xf];
}
}

#endif
int KEYSCAN_InitializeScanParameter(const KEYSCAN_SetStateStruct* keyscan_set)
{
    int r = 0;
    int i;
    uint32_t row;
    uint32_t col;

    if (KEYSCAN_CheckStatePara(keyscan_set) != 0) {
        return 1;
    }

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    row = 0;
    for (i = 0; i < keyscan_set->row_num; i++) {
        int io_source = IO_SOURCE_KEYSCN_ROW_0 + keyscan_set->row[i].out_row;
        row = row | (0x1 << keyscan_set->row[i].out_row);
        r = PINCTRL_SetPadMux(keyscan_set->row[i].gpio, (io_source_t)io_source);
        if (r) return r;
        PINCTRL_Pull(keyscan_set->row[i].gpio, PINCTRL_PULL_DOWN);
    }

    col = 0;
    for (i = 0; i < keyscan_set->col_num; i++) {
        col = col | (0x1 << keyscan_set->col[i].in_col);
        r = PINCTRL_SelKeyScanColIn(keyscan_set->col[i].in_col, keyscan_set->col[i].gpio);
        if (r) return r;
        PINCTRL_Pull(keyscan_set->col[i].gpio, PINCTRL_PULL_DOWN);
    }
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)
    row = 0;
    if(keyscan_set->row_num) {
        for (i = 0; i < keyscan_set->row_num; i++) {
            uint32_t io_source = (uint32_t)IO_SOURCE_KEYSCN_ROW_0 + keyscan_set->row[i].out_row;
            row = row | (0x1 << keyscan_set->row[i].out_row);
            r = PINCTRL_SelKeyScanRowIn(keyscan_set->row[i].out_row, keyscan_set->row[i].gpio);
            if (r) return r;
            r = PINCTRL_SetPadMux(keyscan_set->row[i].gpio, (io_source_t) io_source);
            if (r) return r;
            PINCTRL_KeyScanPullSel(keyscan_set->row[i].gpio,1);
        }
    }

    col = 0;
    if(keyscan_set->col_num) {
        for (i = 0; i < keyscan_set->col_num; i++) {
            uint32_t io_source = (uint32_t)IO_SOURCE_KEYSCN_COL_0 + keyscan_set->col[i].in_col;
            col = col | (0x1 << keyscan_set->col[i].in_col);
            r = PINCTRL_SelKeyScanColIn(keyscan_set->col[i].in_col, keyscan_set->col[i].gpio);
            if (r) return r;
            r = PINCTRL_SetPadMux(keyscan_set->col[i].gpio, (io_source_t) io_source);
            if (r) return r;
            PINCTRL_KeyScanPullSel(keyscan_set->col[i].gpio,1);
        }
    }
#endif

    KEYSCAN_SetOutRowMask(row);
    KEYSCAN_SetInColMask(col);

    KEYSCAN_SetFifoNumTrigInt(keyscan_set->fifo_num_trig_int);
    KEYSCAN_SetDmaNumTrigInt(keyscan_set->dma_num_trig_int);
    KEYSCAN_SetLoopNumTrigInt(keyscan_set->loop_num_trig_int);
    KEYSCAN_SetDmaEn(keyscan_set->dma_en);
    KEYSCAN_SetIntTrigEn(keyscan_set->int_trig_en);
    KEYSCAN_LoopIntEn(keyscan_set->int_loop_en);
    KEYSCAN_SetReleaseTime(keyscan_set->release_time);
    KEYSCAN_SetScanInterval(keyscan_set->scan_interval);
    KEYSCAN_SetDebounceCounter(keyscan_set->debounce_counter);
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)
    KEYSCAN_SetTableModeEn(keyscan_set->table_mode_en);
    KEYSCAN_SetLpkeyModeEn(keyscan_set->lpkey_mode_en);
#endif
    return 0;
}

int KEYSCAN_Initialize(const KEYSCAN_SetStateStruct* keyscan_set)
{
    KEYSCAN_InitializeScanParameter(keyscan_set);
    KEYSCAN_DbClkSel(1);
    KEYSCAN_SetDebounceEn(0xfffff);
    KEYSCAN_SetScannerEn(1);

    return 0;
}

#endif

