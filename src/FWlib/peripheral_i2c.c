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

#if INGCHIPS_FAMILY == INGCHIPS_FAMILY_916

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

void I2C_ClearIntState(I2C_TypeDef *I2C_BASE, uint32_t mask)
{
    I2C_BASE->Status |= mask & I2C_INT_FULL_MASK;
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

void I2C_ConfigClkFrequency(I2C_TypeDef *I2C_BASE, I2C_ClockFrequenyOptions option)
{
    switch (option)
    {
      case I2C_CLOCKFREQUENY_STANDARD:
      {
        I2C_BASE->TPM = 3;
        I2C_ConfigSCLTiming(I2C_BASE,157,0x0,0x5,0x1,0x5);
      }break;
      case I2C_CLOCKFREQUENY_FASTMODE:
      {
        I2C_BASE->TPM = 2;
        I2C_ConfigSCLTiming(I2C_BASE,32,0x1,0x5,0x1,0x5);
      }break;
      case I2C_CLOCKFREQUENY_FASTMODE_PLUS:
      {
        I2C_BASE->TPM = 0;
        I2C_ConfigSCLTiming(I2C_BASE,38,0x1,0x5,0x1,0x5);
      }break;
      default:
        break;
    }
}

void I2C_ConfigSlave(I2C_TypeDef *I2C_BASE, I2C_AddressingMode addr_mode, uint8_t addr)
{
    uint8_t i;
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

