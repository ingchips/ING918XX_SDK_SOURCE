#include "peripheral_i2c.h"

#if INGCHIPS_FAMILY == INGCHIPS_FAMILY_918

// I2c_CTRL0
void I2C_CTRL0_SET(I2C_TypeDef *I2C_BASE, uint32_t data)
{
	I2C_BASE->I2C_CTRL0.SET = data;
}

void I2C_CTRL0_CLR(I2C_TypeDef *I2C_BASE, uint32_t data)
{
	I2C_BASE->I2C_CTRL0.CLR = data;
}

void I2C_CTRL0_TOG(I2C_TypeDef *I2C_BASE, uint32_t data)
{
	I2C_BASE->I2C_CTRL0.TOG = data;
}


// I2c_CTRL1
void I2C_CTRL1_SET(I2C_TypeDef *I2C_BASE, uint32_t data)
{
	I2C_BASE->I2C_CTRL1.SET = data;
}

void I2C_CTRL1_CLR(I2C_TypeDef *I2C_BASE, uint32_t data)
{
	I2C_BASE->I2C_CTRL1.CLR = data;
}

void I2C_CTRL1_TOG(I2C_TypeDef *I2C_BASE, uint32_t data)
{
	I2C_BASE->I2C_CTRL1.TOG = data;
}

// GET I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ
uint8_t GET_I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ(I2C_TypeDef *I2C_BASE)
{
	return((I2C_BASE->I2C_CTRL1.NRM >> bsI2C_CTRL1_DATA_ENGINE_CMPLT_IRQ) & BW2M(bwI2C_CTRL1_DATA_ENGINE_CMPLT_IRQ));
}

// I2C_QUEUECTRL
void I2C_QUEUECTRL_SET(I2C_TypeDef *I2C_BASE, uint32_t data)
{
	I2C_BASE->I2C_QUEUECTRL.SET = data;
}

void I2C_QUEUECTRL_CLR(I2C_TypeDef *I2C_BASE, uint32_t data)
{
	I2C_BASE->I2C_QUEUECTRL.CLR = data;
}

void I2C_QUEUECTRL_TOG(I2C_TypeDef *I2C_BASE, uint32_t data)
{
	I2C_BASE->I2C_QUEUECTRL.TOG = data;
}

// I2C_QUEUECMD
void I2C_QUEUECMD_SET(I2C_TypeDef *I2C_BASE, uint32_t data)
{
	I2C_BASE->I2C_QUEUECMD.SET = data;
}

void I2C_QUEUECMD_CLR(I2C_TypeDef *I2C_BASE, uint32_t data)
{
	I2C_BASE->I2C_QUEUECMD.CLR = data;
}

void I2C_QUEUECMD_TOG(I2C_TypeDef *I2C_BASE, uint32_t data)
{
	I2C_BASE->I2C_QUEUECMD.TOG = data;
}

// I2C_DEBUG0
void I2C_DEBUG0_SET(I2C_TypeDef *I2C_BASE, uint32_t data)
{
	I2C_BASE->I2C_DEBUG0.SET = data;
}

void I2C_DEBUG0_CLR(I2C_TypeDef *I2C_BASE, uint32_t data)
{
	I2C_BASE->I2C_DEBUG0.CLR = data;
}

void I2C_DEBUG0_TOG(I2C_TypeDef *I2C_BASE, uint32_t data)
{
	I2C_BASE->I2C_DEBUG0.TOG = data;
}
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916) || (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)

#define I2C_INT_FULL_MASK       ((1 << (I2C_INT_CMPL + 1)) - 1)

void I2C_Config(I2C_TypeDef *I2C_BASE, I2C_Role role, I2C_AddressingMode addr_mode, uint16_t addr)
{
    I2C_BASE->Setup &= ~0xful;
    I2C_BASE->Setup |= (addr_mode << 1) | (role << 2) | 1;
    I2C_BASE->Addr = addr;
    I2C_BASE->IntEn = 0;
    I2C_BASE->Status = I2C_INT_FULL_MASK;
    I2C_BASE->Cmd = I2C_COMMAND_NO_ACTION;
}

void I2C_Enable(I2C_TypeDef *I2C_BASE, uint8_t enable)
{
    if (enable)
        I2C_BASE->Setup |= 1;
    else
        I2C_BASE->Setup &= ~1ul;
}

void I2C_IntEnable(I2C_TypeDef *I2C_BASE, uint32_t mask)
{
    I2C_BASE->IntEn |= mask & I2C_INT_FULL_MASK;
}

void I2C_IntDisable(I2C_TypeDef *I2C_BASE, uint32_t mask)
{
    I2C_BASE->IntEn &= ~(mask & I2C_INT_FULL_MASK);
}

void I2C_CtrlUpdateDirection(I2C_TypeDef *I2C_BASE, I2C_TransactionDir dir)
{
    I2C_BASE->Ctrl &= (~(BW2M(bwI2C_CTRL_TRANSACTION_DIR) << bsI2C_CTRL_TRANSACTION_DIR));
    I2C_BASE->Ctrl |= ((dir) << bsI2C_CTRL_TRANSACTION_DIR);
}

void I2C_CtrlUpdateDataCnt(I2C_TypeDef *I2C_BASE, uint8_t cnt)
{
    I2C_BASE->Ctrl &= (~(BW2M(bwI2C_CTRL_DATA_CNT)));
    I2C_BASE->Ctrl |= cnt;
}

uint8_t I2C_CtrlGetDataCnt(I2C_TypeDef *I2C_BASE)
{
    return (I2C_BASE->Ctrl & BW2M(bwI2C_CTRL_DATA_CNT));
}

void I2C_CommandWrite(I2C_TypeDef *I2C_BASE, uint8_t cmd)
{
    I2C_BASE->Cmd = cmd;
}

void I2C_DataWrite(I2C_TypeDef *I2C_BASE, uint8_t data)
{
    I2C_BASE->Data = data;
}

uint8_t I2C_DataRead(I2C_TypeDef *I2C_BASE)
{
    return (I2C_BASE->Data);
}

uint32_t I2C_GetIntState(I2C_TypeDef *I2C_BASE)
{
    return I2C_BASE->Status & I2C_INT_FULL_MASK;
}

#define I2C_INT_STATE_MASK  (0x7f<<3)
void I2C_ClearIntState(I2C_TypeDef *I2C_BASE, uint32_t mask)
{
    uint32_t write_mask = (mask & I2C_INT_STATE_MASK);
    I2C_BASE->Status = write_mask;
}

int I2C_TransactionComplete(I2C_TypeDef *I2C_BASE)
{
    return (I2C_BASE->Status >> I2C_INT_CMPL) & 1;
}

int I2C_FifoEmpty(I2C_TypeDef *I2C_BASE)
{
    return (I2C_BASE->Status >> I2C_INT_FIFO_EMPTY) & 1;
}

int I2C_FifoFull(I2C_TypeDef *I2C_BASE)
{
    return (I2C_BASE->Status >> I2C_INT_FIFO_FULL) & 1;
}

void I2C_ConfigSCLTiming(I2C_TypeDef *I2C_BASE, uint32_t scl_hi, uint32_t scl_ratio,
                         uint32_t hddat, uint32_t sp, uint32_t sudat)
{
    uint32_t t = I2C_BASE->Setup & 0xf;
    t |= (scl_hi & 0x1ff) << 4;
    t |= (scl_ratio & 0x1) << 13;
    t |= (hddat & 0x1f) << 16;
    t |= (sp & 0x7) << 21;
    t |= (sudat & 0x1f) << 24;
    I2C_BASE->Setup = t;
}

static uint8_t I2C_SCL_ParamExhaustiveSearch(uint32_t pclk_hz,uint32_t sclk_hz,uint8_t scl_ratio, uint8_t *pTPM, uint8_t *pSCLHi, uint8_t *pT_SP) {
    uint8_t best_T_SCLHi = 0;
    uint8_t best_T_SP = 0;
    uint8_t best_TPM = 0;
    float  best_error = 999999.0;
    float  tpclk = (float)(1000000000.0 / pclk_hz);
    float  tsclk_h = (float)(1000000000.0 / sclk_hz)/(2.0+scl_ratio);
    uint8_t find_flag = 1;
    uint16_t T_SCLHi, T_SP, TPM;
    for (T_SCLHi = 1; T_SCLHi < 256; T_SCLHi++) {
        for (T_SP = 0; T_SP < 8; T_SP++) {
            for (TPM = 0; TPM < 32; TPM++) {
                float calculated_P_SCL_H = (float)((2.0 * tpclk) + (2.0 + T_SP + T_SCLHi) * (TPM + 1) * tpclk);
                float error = fabs(calculated_P_SCL_H - tsclk_h);

                if ((T_SCLHi > T_SP) && (error < best_error)) {
                    best_T_SCLHi = (uint8_t)T_SCLHi;
                    best_T_SP = T_SP;
                    best_TPM = TPM;
                    best_error = error;
                    find_flag = 0;
                }
            }
        }
    }
    *pTPM = (uint8_t )best_TPM & 0x1f;
    *pSCLHi = (uint8_t )best_T_SCLHi & 0xff;
    *pT_SP =  (uint8_t )best_T_SP & 0x07;
    return find_flag;

}

static uint8_t I2C_GetSetupDataTime(uint32_t pclk_hz, uint8_t Tpm,
        uint8_t T_sp, float su_data_time_max)
{
    uint8_t T_SUDATA = 0;
    float setup_time = 0;
    float  tpclk = (float)(1000000000.0 / pclk_hz);
    while (setup_time < su_data_time_max)
    {
        setup_time = (2 * tpclk) + (2 + T_sp + T_SUDATA)
                 * tpclk * (Tpm + 1);
        T_SUDATA++;
        if(T_SUDATA > 31) return 0xff;
    }
    if(T_SUDATA == 0)   return 0;

    return T_SUDATA - 1;
}

static uint8_t I2C_GetDataHoldTime(uint32_t pclk_hz,uint8_t T_SCLHi, uint8_t Tpm,
            uint8_t T_sp, float min_hold_time)
{
    float  tpclk = (float)(1000000000.0 / pclk_hz);
    float data_hold_time = 0;
    uint8_t T_HDDATA = 0;
    while (data_hold_time > min_hold_time)
    {
        if(T_HDDATA > 31 || T_HDDATA >= T_SCLHi) return 0xff;
        data_hold_time = (2 * tpclk) + (2 + T_sp + T_HDDATA)
                 * tpclk *(Tpm + 1);
        T_HDDATA +=1 ;

    }
    if(T_HDDATA == 0) return 0;
    return T_HDDATA - 1;
}


uint32_t I2C_ConfigClkFrequencyCalc(I2C_TypeDef *I2C_BASE, I2C_ClockFrequencyOptions option)
{
    uint8_t TPM,T_SP,T_SCLHi,T_SUDATA,T_HDDATA;
    uint32_t pclk_hz = SYSCTRL_GetPClk();
    uint8_t scl_ratio = 0;
    uint32_t sclk_hz = 100000;
    float max_su_data_time = 250;
    float min_hold_time = 300;
    switch (option)
    {
      case I2C_CLOCKFREQUENY_STANDARD:
      {
           scl_ratio = 0;
           max_su_data_time = 250.0;
           min_hold_time = 300.0;
           sclk_hz = 100000;
      }break;
      case I2C_CLOCKFREQUENY_FASTMODE:
      {
          scl_ratio = 1;
          max_su_data_time = 100.0;
          min_hold_time = 300.0;
          sclk_hz = 400000;

      }break;
      case I2C_CLOCKFREQUENY_FASTMODE_PLUS:
      {
            scl_ratio = 1;
            max_su_data_time = 50.0;
            min_hold_time = 150.0;
            sclk_hz = 1000000;

      }break;
      default:{
           scl_ratio = 0;
           max_su_data_time = 250.0;
           min_hold_time = 300.0;
           sclk_hz = 100000;
      }
        break;
    }
    if(I2C_SCL_ParamExhaustiveSearch(pclk_hz,sclk_hz,scl_ratio,&TPM,&T_SCLHi,&T_SP))
    {
        return 1;
    }
    T_SUDATA = I2C_GetSetupDataTime(pclk_hz, TPM, T_SP, max_su_data_time);
    if(0xff == T_SUDATA){
        return 2;
    }
    T_HDDATA = I2C_GetDataHoldTime(pclk_hz,T_SCLHi, TPM, T_SP, min_hold_time);
    if(0xff == T_HDDATA){
        return 3;
    }
    I2C_BASE->TPM = TPM;
    I2C_ConfigSCLTiming(I2C_BASE,T_SCLHi,scl_ratio,T_HDDATA,T_SP,T_SUDATA);
    return 0;
}


void I2C_ConfigClkFrequency(I2C_TypeDef *I2C_BASE, I2C_ClockFrequencyOptions option)
{   uint32_t ret;
    ret = I2C_ConfigClkFrequencyCalc(I2C_BASE, option);
    if(ret != 0){
        I2C_BASE->TPM = 3;
        I2C_ConfigSCLTiming(I2C_BASE,157,0x0,0x5,0x1,0x5);
        return;
    }
}

void I2C_ConfigSlave(I2C_TypeDef *I2C_BASE, I2C_AddressingMode addr_mode, uint8_t addr)
{
    I2C_Config(I2C_BASE, I2C_ROLE_SLAVE, addr_mode, addr);
    I2C_BASE->IntEn = (1 << I2C_INT_ADDR_HIT) | (1 << I2C_INT_CMPL);
}

I2C_TransactionDir I2C_GetTransactionDir(I2C_TypeDef *I2C_BASE)
{
    return (I2C_TransactionDir)((I2C_BASE->Ctrl >> bsI2C_CTRL_TRANSACTION_DIR) & 1);
}

void I2C_DmaEnable(I2C_TypeDef *I2C_BASE, uint8_t enable)
{
    uint32_t bit = (uint32_t)1 << 3;
    if (enable)
        I2C_BASE->Setup |= bit;
    else
        I2C_BASE->Setup &= ~bit;
}

#endif

