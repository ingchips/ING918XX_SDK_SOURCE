#include "peripheral_pte.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

void PTE_ClearInt(SYSCTRL_PTE_CHANNEL_ID ch)
{
    APB_PTE->Channels[ch].Int = 1;
}

uint8_t PTE_GetIntState(SYSCTRL_PTE_CHANNEL_ID ch)
{
    return (APB_PTE->Channels[ch].Int);
}

void PTE_EnableChannel(SYSCTRL_PTE_CHANNEL_ID ch)
{
    APB_PTE->Channels[ch].En = 1;
}

void PTE_DisableChannel(SYSCTRL_PTE_CHANNEL_ID ch)
{
    APB_PTE->Channels[ch].En = 0;
}

void PTE_SetChannelIntMask(SYSCTRL_PTE_CHANNEL_ID ch, uint32_t options)
{
    uint32_t *m;
    uint32_t Cm;

    if (options) 
        APB_PTE->Channels[ch].IntMask = 1;
    else {
        m = (uint32_t *)&(APB_PTE->Channels[ch]);
        Cm = PTE_MAKE_CHANNEL_INT_MASK_OPTION(!options);
        *m &= Cm;
    }
}

void PTE_IrqProc(SYSCTRL_PTE_CHANNEL_ID ch)
{
    PTE_SetChannelIntMask(ch, 0);
}

void PTE_OutPeripheralContinueProc(SYSCTRL_PTE_CHANNEL_ID ch)
{
    PTE_ClearInt(ch);
}

void PTE_OutPeripheralEndProc(SYSCTRL_PTE_CHANNEL_ID ch)
{
    PTE_ClearInt(ch);
    PTE_DisableChannel(ch);
}

void PTE_ChannelClose(SYSCTRL_PTE_CHANNEL_ID ch)
{
    PTE_ClearInt(ch);
    PTE_DisableChannel(ch);
    PTE_SetChannelIntMask(ch, 0);
}

int PTE_ConnectPeripheral(SYSCTRL_PTE_CHANNEL_ID ch, 
                          SYSCTRL_PTE_SRC_INT src, 
                          SYSCTRL_PTE_DST_EN dst)
{
    if (ch  >  SYSCTRL_PTE_CHANNEL_3   ||
        src >= SYSCTRL_PTE_SRC_INT_MAX ||
        dst >= SYSCTRL_PTE_DST_EN_MAX) {
        return -1;
    }
    uint32_t Sb = PTE_MAKE_MASK_SIZE_OPTION(SYSCTRL_PTE_SRC_INT_MAX);
    uint32_t Db = PTE_MAKE_MASK_SIZE_OPTION(SYSCTRL_PTE_DST_EN_MAX);

    PTE_ClearInt(ch);
    APB_PTE->Channels[ch].InMask  &= (~Sb);
    APB_PTE->Channels[ch].OutMask &= (~Db);
    APB_PTE->Channels[ch].InMask  |= (1 << src);
    APB_PTE->Channels[ch].OutMask |= (1 << dst);
    PTE_SetChannelIntMask(ch, 1);
    PTE_EnableChannel(ch);

    return 0;
}

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)

static void PTE_SetRegBit(volatile uint32_t *reg, uint8_t v, uint8_t bit_offset)
{
    uint32_t mask = 1ul << bit_offset;
    *reg = (*reg & ~mask) | (v << bit_offset);
}

uint32_t PTE_ChxGetEnableState(void)
{
    return APB_PTE->reg_ch_en;
}

void PTE_ChxEnable(uint32_t items)
{
    APB_PTE->reg_ch_en_set = items;
}

void PTE_ChxDisable(uint32_t items)
{
    APB_PTE->reg_ch_en_clr = items;
}

uint8_t PTE_SetModuleTaskEventConfig(PTE_Module SetPTEModule, PTE_Channal SetTaskChannal, PTE_ModuleTaskEvt PTETaskEvt, uint32_t TaskEvtSer)
{
    uint32_t  reg_num = 0;
    switch (SetPTEModule)
    {
        case PTE_QDEC_MODULE:
            if(PTETaskEvt > PTE_TASK_QDEC_TMR_START)
                return 0;
            if(PTETaskEvt > PTE_EVENT_QDEC_TMR_STOP)
            {
                reg_num = (PTETaskEvt - PTE_TASK_QDEC_TMR_STOP) + (PTE_TASK_QDEC_TMR_START - PTE_TASK_QDEC_TMR_STOP + 1ul) * TaskEvtSer;
                APB_PTE_BUS->PTE_QDEC.reg_sub_task[reg_num] = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
            else
            {
                APB_PTE_BUS->PTE_QDEC.reg_pub_event[PTETaskEvt] = ((uint32_t)SetTaskChannal | (1ul<<31));
            }    
        break;
        case PTE_DMA_MODULE:
            if(PTETaskEvt > PTE_EVENT_DMA_TRANS_CMPL)
                return 0;
            APB_PTE_BUS->PTE_DMA.reg_pub_event[PTETaskEvt] = ((uint32_t)SetTaskChannal | (1ul<<31));
        break;
        case PTE_GPIOTE_MODULE:
            if(PTETaskEvt > PTE_TASK_GPIO_CLR )
            return 0;
            if(PTETaskEvt > PTE_EVENT_GPIO_INT)
            {
                reg_num = (PTETaskEvt - PTR_TASK_GPIO_OUT) + (PTE_TASK_GPIO_CLR - PTR_TASK_GPIO_OUT) * TaskEvtSer;
                APB_PTE_BUS->PTE_GPIOTE.reg_sub_task[reg_num] = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
            else
            {
                APB_PTE_BUS->PTE_GPIOTE.reg_pub_event[PTETaskEvt] = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
        break;
        case PTE_TIMER0_MODULE:
            if(PTETaskEvt > PTE_TASK_TIMER_CHX_TMRX_DISABLE )
            return 0;
            if(PTETaskEvt > PTE_EVENT_TIMER_CHX_TMRX)
            {
                reg_num = (PTETaskEvt - PTE_TASK_TIMER_CHX_TMRX_ENABLE) * (PTE_TASK_TIMER_CHX_TMRX_DISABLE - PTE_TASK_TIMER_CHX_TMRX_ENABLE + 1) + TaskEvtSer;
                APB_PTE_BUS->PTE_TIMER0.reg_sub_task[reg_num] = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
            else
            {
                APB_PTE_BUS->PTE_TIMER0.reg_pub_event[PTETaskEvt] = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
        break;
        case PTE_QSPI_MODULE:
            if(PTETaskEvt > PTE_TASK_SPI_TERMINATE )
                return 0;
            if(PTETaskEvt > PTE_EVENT_SPI_TRANS_END)
            {
                reg_num = (PTETaskEvt - PTE_TASK_SPI_INITIATE) + (PTE_TASK_SPI_TERMINATE - PTE_TASK_SPI_INITIATE) * TaskEvtSer;
                APB_PTE_BUS->PTE_QSPI.reg_sub_task[reg_num] = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
            else
            {
                APB_PTE_BUS->PTE_QSPI.reg_pub_event0 = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
        break;
        case PTE_I2C_MODULE:
            if(PTETaskEvt > PTE_TASK_IIC_ISSUE_TRANS )
                return 0;
            if(PTETaskEvt > PTE_EVENT_IIC_TRANS_CMPL)
            {
                APB_PTE_BUS->PTE_I2C.reg_sub_task0 = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
            else
            {
                APB_PTE_BUS->PTE_I2C.reg_pub_event0 = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
        break;
        case PTE_ADC_MODULE:
            if(PTETaskEvt > PTE_TASK_ADC_LOOP_SMP_STOP )
                return 0;
            if(PTETaskEvt > PTE_EVENT_ADC_SMP_STOP)
            {
                reg_num = (PTETaskEvt - PTE_TASK_ADC_SINGLE_START) + (PTE_TASK_ADC_LOOP_SMP_STOP - PTE_TASK_ADC_SINGLE_START) * TaskEvtSer;
                APB_PTE_BUS->PTE_SADC.reg_sub_task[reg_num] = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
            else
            {
                reg_num = (PTETaskEvt - PTE_EVENT_ADC_SMP_DONE) + (PTE_EVENT_ADC_SMP_STOP - PTE_EVENT_ADC_SMP_DONE) * TaskEvtSer;
                APB_PTE_BUS->PTE_SADC.reg_pub_event[reg_num] = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
        break;
        case PTE_UART0_MODULE:
            if(PTETaskEvt > PTE_TASK_UART_RX_DISABLE )
                return 0;
            if(PTETaskEvt > PTE_EVENT_ADC_SMP_STOP)
            {
                reg_num = (PTETaskEvt - PTE_TASK_UART_TX_ENABLE) + (PTE_TASK_UART_RX_DISABLE - PTE_TASK_UART_TX_ENABLE) * TaskEvtSer;
                APB_PTE_BUS->PTE_UART0.reg_sub_task[reg_num] = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
            else
            {
                reg_num = (PTETaskEvt - PTE_EVENT_UART_TX_FIFO_TRIGE) + (PTE_EVENT_UART_RX_FIFO_TRIGE - PTE_EVENT_UART_TX_FIFO_TRIGE) * TaskEvtSer;
                APB_PTE_BUS->PTE_UART0.reg_pub_event[reg_num] = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
        break;
        case PTE_UART1_MODULE:
            if(PTETaskEvt > PTE_TASK_UART_RX_DISABLE )
                return 0;
            if(PTETaskEvt > PTE_EVENT_ADC_SMP_STOP)
            {
                reg_num = (PTETaskEvt - PTE_TASK_UART_TX_ENABLE) + (PTE_TASK_UART_RX_DISABLE - PTE_TASK_UART_TX_ENABLE) * TaskEvtSer;
                APB_PTE_BUS->PTE_UART1.reg_sub_task[reg_num] = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
            else
            {
                reg_num = (PTETaskEvt - PTE_EVENT_UART_TX_FIFO_TRIGE) + (PTE_EVENT_UART_RX_FIFO_TRIGE - PTE_EVENT_UART_TX_FIFO_TRIGE) * TaskEvtSer;
                APB_PTE_BUS->PTE_UART1.reg_pub_event[reg_num] = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
        break;
        case PTE_SPI_MODULE:
            if(PTETaskEvt > PTE_TASK_SPI_TERMINATE )
                return 0;
            if(PTETaskEvt > PTE_EVENT_SPI_TRANS_END)
            {
                reg_num = (PTETaskEvt - PTE_TASK_SPI_INITIATE) + (PTE_TASK_SPI_TERMINATE - PTE_TASK_SPI_INITIATE) * TaskEvtSer;
                APB_PTE_BUS->PTE_SPI.reg_sub_task[reg_num] = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
            else
            {
                APB_PTE_BUS->PTE_SPI.reg_pub_event0 = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
        break;
        case PTE_PWMC_MODULE:
            if(PTETaskEvt > PTE_TASK_PWM_CX_PCM_STOP )
                return 0;
            if(PTETaskEvt > PTE_EVENT_PWM_CX_PCM_STOP)
            {
                reg_num = (PTETaskEvt - PTE_TASK_PWM_CX_ONE_PWM) + (PTE_TASK_PWM_CX_PCM_STOP - PTE_TASK_PWM_CX_ONE_PWM) * TaskEvtSer;
                APB_PTE_BUS->PTE_PWM.reg_sub_task[reg_num] = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
            else
            {
                reg_num = (PTETaskEvt - PTE_EVENT_PWM_CX_PWM_STOP) + (PTE_TASK_PWM_CX_PCM_STOP - PTE_EVENT_PWM_CX_PWM_STOP) * TaskEvtSer;
                APB_PTE_BUS->PTE_PWM.reg_pub_event[reg_num] = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
        break;
        case PTE_TIMER1_MODULE:
            if(PTETaskEvt > PTE_TASK_TIMER_CHX_TMRX_DISABLE )
                return 0;
            if(PTETaskEvt > PTE_EVENT_TIMER_CHX_TMRX)
            {
                reg_num = (PTETaskEvt - PTE_TASK_TIMER_CHX_TMRX_ENABLE) * (PTE_TASK_TIMER_CHX_TMRX_DISABLE - PTE_TASK_TIMER_CHX_TMRX_ENABLE + 1) + TaskEvtSer;
                APB_PTE_BUS->PTE_TIMER1.reg_sub_task[reg_num] = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
            else
            {
                APB_PTE_BUS->PTE_TIMER1.reg_pub_event[PTETaskEvt] = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
        break;
        case PTE_I2S_MODULE:
            if(PTETaskEvt > PTE_TASK_I2S_RX_WR_RST_MEM_DISABLE )
                return 0;
            reg_num = (PTETaskEvt - PTE_TASK_I2S_TX_FIFO_TRIGE_ENABLE) + (PTE_TASK_I2S_RX_WR_RST_MEM_DISABLE - PTE_TASK_I2S_TX_FIFO_TRIGE_ENABLE) * TaskEvtSer;
            APB_PTE_BUS->PTE_I2S.reg_sub_task[reg_num] = ((uint32_t)SetTaskChannal | (1ul<<31));
        break;
        case PTE_ASDM_MODULE:
            if(PTETaskEvt > PTE_TASK_ASDM_CONVERSION_STOP )
                return 0;
            if(PTETaskEvt > PTE_EVENT_ASDM_RX_STOP)
            {
                reg_num = (PTETaskEvt - PTE_TASK_ASDM_CONVERSION_START) + (PTE_TASK_ASDM_CONVERSION_STOP - PTE_TASK_ASDM_CONVERSION_START) * TaskEvtSer;
                if(reg_num == 0)
                    APB_PTE_BUS->PTE_ASDM.reg_sub_task0 = ((uint32_t)SetTaskChannal | (1ul<<31));
                if(reg_num == 1ul)
                    APB_PTE_BUS->PTE_ASDM.reg_sub_task1 = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
            else
            {
                APB_PTE_BUS->PTE_ASDM.reg_pub_event0 = ((uint32_t)SetTaskChannal | (1ul<<31));
            }
            break;
        
        default:
            break;
    }
    return 1;
}

uint8_t PTE_TaskEnable(PTE_Module SetPTEModule, PTE_ModuleTaskEvt PTETaskEvt, uint8_t enable, uint32_t TaskEvtSer)
{
    uint32_t  reg_num = 0;
    switch (SetPTEModule)
    {
        case PTE_QDEC_MODULE:
            if(PTETaskEvt > PTE_TASK_QDEC_TMR_START)
                return 0;
            if(PTETaskEvt > PTE_EVENT_QDEC_TMR_STOP)
            {
                reg_num = (PTETaskEvt - PTE_TASK_QDEC_TMR_STOP) + (PTE_TASK_QDEC_TMR_START - PTE_TASK_QDEC_TMR_STOP + 1ul) * TaskEvtSer;
                PTE_SetRegBit(&APB_PTE_BUS->PTE_QDEC.reg_sub_task[reg_num], enable, 31);
            }
            else
            {
                PTE_SetRegBit(&APB_PTE_BUS->PTE_QDEC.reg_pub_event[PTETaskEvt], enable, 31);
            }    
        break;
        case PTE_DMA_MODULE:
            if(PTETaskEvt > PTE_EVENT_DMA_TRANS_CMPL)
                return 0;
            PTE_SetRegBit(&APB_PTE_BUS->PTE_DMA.reg_pub_event[PTETaskEvt], enable, 31);
        break;
        case PTE_GPIOTE_MODULE:
            if(PTETaskEvt > PTE_TASK_GPIO_CLR )
            return 0;
            if(PTETaskEvt > PTE_EVENT_GPIO_INT)
            {
                reg_num = (PTETaskEvt - PTR_TASK_GPIO_OUT) + (PTE_TASK_GPIO_CLR - PTR_TASK_GPIO_OUT) * TaskEvtSer;
                PTE_SetRegBit(&APB_PTE_BUS->PTE_GPIOTE.reg_sub_task[reg_num], enable, 31);
            }
            else
            {
                PTE_SetRegBit(&APB_PTE_BUS->PTE_GPIOTE.reg_pub_event[PTETaskEvt], enable, 31);
            }
        break;
        case PTE_TIMER0_MODULE:
            if(PTETaskEvt > PTE_TASK_TIMER_CHX_TMRX_DISABLE )
            return 0;
            if(PTETaskEvt > PTE_EVENT_TIMER_CHX_TMRX)
            {
                reg_num = (PTETaskEvt - PTE_TASK_TIMER_CHX_TMRX_ENABLE) * (PTE_TASK_TIMER_CHX_TMRX_DISABLE - PTE_TASK_TIMER_CHX_TMRX_ENABLE + 1) + TaskEvtSer;
                PTE_SetRegBit(&APB_PTE_BUS->PTE_TIMER0.reg_sub_task[reg_num], enable, 31);
            }
            else
            {
                PTE_SetRegBit(&APB_PTE_BUS->PTE_TIMER0.reg_pub_event[PTETaskEvt], enable, 31);
            }
        break;
        case PTE_QSPI_MODULE:
            if(PTETaskEvt > PTE_TASK_SPI_TERMINATE )
                return 0;
            if(PTETaskEvt > PTE_EVENT_SPI_TRANS_END)
            {
                reg_num = (PTETaskEvt - PTE_TASK_SPI_INITIATE) + (PTE_TASK_SPI_TERMINATE - PTE_TASK_SPI_INITIATE) * TaskEvtSer;
                PTE_SetRegBit(&APB_PTE_BUS->PTE_QSPI.reg_sub_task[reg_num], enable, 31);
            }
            else
            {
                PTE_SetRegBit(&APB_PTE_BUS->PTE_QSPI.reg_pub_event0, enable, 31);
            }
        break;
        case PTE_I2C_MODULE:
            if(PTETaskEvt > PTE_TASK_IIC_ISSUE_TRANS )
                return 0;
            if(PTETaskEvt > PTE_EVENT_IIC_TRANS_CMPL)
            {
                PTE_SetRegBit(&APB_PTE_BUS->PTE_I2C.reg_sub_task0, enable, 31);
            }
            else
            {
                PTE_SetRegBit(&APB_PTE_BUS->PTE_I2C.reg_pub_event0, enable, 31);
            }
        break;
        case PTE_ADC_MODULE:
            if(PTETaskEvt > PTE_TASK_ADC_LOOP_SMP_STOP )
                return 0;
            if(PTETaskEvt > PTE_EVENT_ADC_SMP_STOP)
            {
                reg_num = (PTETaskEvt - PTE_TASK_ADC_SINGLE_START) + (PTE_TASK_ADC_LOOP_SMP_STOP - PTE_TASK_ADC_SINGLE_START) * TaskEvtSer;
                PTE_SetRegBit(&APB_PTE_BUS->PTE_SADC.reg_sub_task[reg_num], enable, 31);
            }
            else
            {
                reg_num = (PTETaskEvt - PTE_EVENT_ADC_SMP_DONE) + (PTE_EVENT_ADC_SMP_STOP - PTE_EVENT_ADC_SMP_DONE) * TaskEvtSer;
                PTE_SetRegBit(&APB_PTE_BUS->PTE_SADC.reg_pub_event[reg_num], enable, 31);
            }
        break;
        case PTE_UART0_MODULE:
            if(PTETaskEvt > PTE_TASK_UART_RX_DISABLE )
                return 0;
            if(PTETaskEvt > PTE_EVENT_ADC_SMP_STOP)
            {
                reg_num = (PTETaskEvt - PTE_TASK_UART_TX_ENABLE) + (PTE_TASK_UART_RX_DISABLE - PTE_TASK_UART_TX_ENABLE) * TaskEvtSer;
                PTE_SetRegBit(&APB_PTE_BUS->PTE_UART0.reg_sub_task[reg_num], enable, 31);
            }
            else
            {
                reg_num = (PTETaskEvt - PTE_EVENT_UART_TX_FIFO_TRIGE) + (PTE_EVENT_UART_RX_FIFO_TRIGE - PTE_EVENT_UART_TX_FIFO_TRIGE) * TaskEvtSer;
                PTE_SetRegBit(&APB_PTE_BUS->PTE_UART0.reg_pub_event[reg_num], enable, 31);
            }
        break;
        case PTE_UART1_MODULE:
            if(PTETaskEvt > PTE_TASK_UART_RX_DISABLE )
                return 0;
            if(PTETaskEvt > PTE_EVENT_ADC_SMP_STOP)
            {
                reg_num = (PTETaskEvt - PTE_TASK_UART_TX_ENABLE) + (PTE_TASK_UART_RX_DISABLE - PTE_TASK_UART_TX_ENABLE) * TaskEvtSer;
                PTE_SetRegBit(&APB_PTE_BUS->PTE_UART1.reg_sub_task[reg_num], enable, 31);
            }
            else
            {
                reg_num = (PTETaskEvt - PTE_EVENT_UART_TX_FIFO_TRIGE) + (PTE_EVENT_UART_RX_FIFO_TRIGE - PTE_EVENT_UART_TX_FIFO_TRIGE) * TaskEvtSer;
                PTE_SetRegBit(&APB_PTE_BUS->PTE_UART1.reg_pub_event[reg_num], enable, 31);
            }
        break;
        case PTE_SPI_MODULE:
            if(PTETaskEvt > PTE_TASK_SPI_TERMINATE )
                return 0;
            if(PTETaskEvt > PTE_EVENT_SPI_TRANS_END)
            {
                reg_num = (PTETaskEvt - PTE_TASK_SPI_INITIATE) + (PTE_TASK_SPI_TERMINATE - PTE_TASK_SPI_INITIATE) * TaskEvtSer;
                PTE_SetRegBit(&APB_PTE_BUS->PTE_SPI.reg_sub_task[reg_num], enable, 31);
            }
            else
            {
                PTE_SetRegBit(&APB_PTE_BUS->PTE_SPI.reg_pub_event0, enable, 31);
            }
        break;
        case PTE_PWMC_MODULE:
            if(PTETaskEvt > PTE_TASK_PWM_CX_PCM_STOP )
                return 0;
            if(PTETaskEvt > PTE_EVENT_PWM_CX_PCM_STOP)
            {
                reg_num = (PTETaskEvt - PTE_TASK_PWM_CX_ONE_PWM) + (PTE_TASK_PWM_CX_PCM_STOP - PTE_TASK_PWM_CX_ONE_PWM) * TaskEvtSer;
                PTE_SetRegBit(&APB_PTE_BUS->PTE_PWM.reg_sub_task[reg_num], enable, 31);
            }
            else
            {
                reg_num = (PTETaskEvt - PTE_EVENT_PWM_CX_PWM_STOP) + (PTE_TASK_PWM_CX_PCM_STOP - PTE_EVENT_PWM_CX_PWM_STOP) * TaskEvtSer;
                PTE_SetRegBit(&APB_PTE_BUS->PTE_PWM.reg_pub_event[reg_num], enable, 31);
            }
        break;
        case PTE_TIMER1_MODULE:
            if(PTETaskEvt > PTE_TASK_TIMER_CHX_TMRX_DISABLE )
                return 0;
            if(PTETaskEvt > PTE_EVENT_TIMER_CHX_TMRX)
            {
                reg_num = (PTETaskEvt - PTE_TASK_TIMER_CHX_TMRX_ENABLE) * (PTE_TASK_TIMER_CHX_TMRX_DISABLE - PTE_TASK_TIMER_CHX_TMRX_ENABLE + 1) + TaskEvtSer;
                PTE_SetRegBit(&APB_PTE_BUS->PTE_TIMER1.reg_sub_task[reg_num], enable, 31);
            }
            else
            {
                PTE_SetRegBit(&APB_PTE_BUS->PTE_TIMER1.reg_pub_event[PTETaskEvt], enable, 31);
            }
        break;
        case PTE_I2S_MODULE:
            if(PTETaskEvt > PTE_TASK_I2S_RX_WR_RST_MEM_DISABLE )
                return 0;
            reg_num = (PTETaskEvt - PTE_TASK_I2S_TX_FIFO_TRIGE_ENABLE) + (PTE_TASK_I2S_RX_WR_RST_MEM_DISABLE - PTE_TASK_I2S_TX_FIFO_TRIGE_ENABLE) * TaskEvtSer;
            PTE_SetRegBit(&APB_PTE_BUS->PTE_I2S.reg_sub_task[reg_num], enable, 31);
        break;
        case PTE_ASDM_MODULE:
            if(PTETaskEvt > PTE_TASK_ASDM_CONVERSION_STOP )
                return 0;
            if(PTETaskEvt > PTE_EVENT_ASDM_RX_STOP)
            {
                reg_num = (PTETaskEvt - PTE_TASK_ASDM_CONVERSION_START) + (PTE_TASK_ASDM_CONVERSION_STOP - PTE_TASK_ASDM_CONVERSION_START) * TaskEvtSer;
                if(reg_num == 0)
                    PTE_SetRegBit(&APB_PTE_BUS->PTE_ASDM.reg_sub_task0, enable, 31);
                if(reg_num == 1ul)
                    PTE_SetRegBit(&APB_PTE_BUS->PTE_ASDM.reg_sub_task1, enable, 31);
            }
            else
            {
                PTE_SetRegBit(&APB_PTE_BUS->PTE_ASDM.reg_pub_event0 , enable, 31);
            }
            break;
        
        default:
            break;
    }
    return 1;
}

uint8_t PTE_TriggerTask(PTE_Module SetPTEModule, PTE_ModuleTaskEvt PTETaskEvt, uint32_t TaskEvtSer)
{
        uint32_t  reg_num = 0;
    switch (SetPTEModule)
    {
        case PTE_QDEC_MODULE:
            if(PTETaskEvt < PTE_TASK_QDEC_TMR_STOP)
                return 0;
                reg_num = (PTETaskEvt - PTE_TASK_QDEC_TMR_STOP) + (PTE_TASK_QDEC_TMR_START - PTE_TASK_QDEC_TMR_STOP + 1ul) * TaskEvtSer;
                APB_PTE_BUS->PTE_QDEC.reg_task[reg_num] = 1ul;
        break;
        case PTE_GPIOTE_MODULE:
            if(PTETaskEvt < PTR_TASK_GPIO_OUT )
            return 0;
                reg_num = (PTETaskEvt - PTR_TASK_GPIO_OUT) + (PTE_TASK_GPIO_CLR - PTR_TASK_GPIO_OUT) * TaskEvtSer;
                APB_PTE_BUS->PTE_GPIOTE.reg_task[reg_num] = 1ul;
        break;
        case PTE_TIMER0_MODULE:
            if(PTETaskEvt < PTE_TASK_TIMER_CHX_TMRX_ENABLE )
            return 0;
                reg_num = (PTETaskEvt - PTE_TASK_TIMER_CHX_TMRX_ENABLE) * (PTE_TASK_TIMER_CHX_TMRX_DISABLE - PTE_TASK_TIMER_CHX_TMRX_ENABLE + 1) + TaskEvtSer;
                APB_PTE_BUS->PTE_TIMER0.reg_task[reg_num] = 1ul;
            
        break;
        case PTE_QSPI_MODULE:
            if(PTETaskEvt < PTE_TASK_SPI_INITIATE )
                return 0;
                reg_num = (PTETaskEvt - PTE_TASK_SPI_INITIATE) + (PTE_TASK_SPI_TERMINATE - PTE_TASK_SPI_INITIATE) * TaskEvtSer;
                APB_PTE_BUS->PTE_QSPI.reg_task[reg_num] = 1ul;
        break;
        case PTE_I2C_MODULE:
            if(PTETaskEvt < PTE_TASK_IIC_ISSUE_TRANS )
                return 0;
                APB_PTE_BUS->PTE_I2C.reg_task0 = 1ul;
        break;
        case PTE_ADC_MODULE:
            if(PTETaskEvt < PTE_TASK_ADC_SINGLE_START )
                return 0;

                reg_num = (PTETaskEvt - PTE_TASK_ADC_SINGLE_START) + (PTE_TASK_ADC_LOOP_SMP_STOP - PTE_TASK_ADC_SINGLE_START) * TaskEvtSer;
                APB_PTE_BUS->PTE_SADC.reg_task[reg_num] = 1ul;
        break;
        case PTE_UART0_MODULE:
            if(PTETaskEvt < PTE_TASK_UART_TX_ENABLE )
                return 0;
                reg_num = (PTETaskEvt - PTE_TASK_UART_TX_ENABLE) + (PTE_TASK_UART_RX_DISABLE - PTE_TASK_UART_TX_ENABLE) * TaskEvtSer;
                APB_PTE_BUS->PTE_UART0.reg_task[reg_num] = 1ul;
        break;
        case PTE_UART1_MODULE:
            if(PTETaskEvt < PTE_TASK_UART_TX_ENABLE )
                return 0;
                reg_num = (PTETaskEvt - PTE_TASK_UART_TX_ENABLE) + (PTE_TASK_UART_RX_DISABLE - PTE_TASK_UART_TX_ENABLE) * TaskEvtSer;
                APB_PTE_BUS->PTE_UART1.reg_task[reg_num] = 1ul;

        break;
        case PTE_SPI_MODULE:
            if(PTETaskEvt < PTE_TASK_I2S_TX_WR_RST_MEM_DISABLE )
                return 0;
                reg_num = (PTETaskEvt - PTE_TASK_SPI_INITIATE) + (PTE_TASK_SPI_TERMINATE - PTE_TASK_SPI_INITIATE) * TaskEvtSer;
                APB_PTE_BUS->PTE_SPI.reg_task[reg_num] = 1ul;
        break;
        case PTE_PWMC_MODULE:
            if(PTETaskEvt < PTE_TASK_PWM_CX_ONE_PWM )
                return 0;

                reg_num = (PTETaskEvt - PTE_TASK_PWM_CX_ONE_PWM) + (PTE_TASK_PWM_CX_PCM_STOP - PTE_TASK_PWM_CX_ONE_PWM) * TaskEvtSer;
                APB_PTE_BUS->PTE_PWM.reg_task[reg_num] = 1ul;
        break;
        case PTE_TIMER1_MODULE:
            if(PTETaskEvt < PTE_TASK_TIMER_CHX_TMRX_DISABLE )
                return 0;
                reg_num = (PTETaskEvt - PTE_TASK_TIMER_CHX_TMRX_ENABLE) * (PTE_TASK_TIMER_CHX_TMRX_DISABLE - PTE_TASK_TIMER_CHX_TMRX_ENABLE + 1) + TaskEvtSer;
                APB_PTE_BUS->PTE_TIMER1.reg_task[reg_num] = 1ul;
        break;
        case PTE_I2S_MODULE:
            if(PTETaskEvt < PTE_TASK_I2S_RX_WR_RST_MEM_DISABLE )
                return 0;
            reg_num = (PTETaskEvt - PTE_TASK_I2S_TX_FIFO_TRIGE_ENABLE) + (PTE_TASK_I2S_RX_WR_RST_MEM_DISABLE - PTE_TASK_I2S_TX_FIFO_TRIGE_ENABLE) * TaskEvtSer;
            APB_PTE_BUS->PTE_I2S.reg_task[reg_num] = 1ul;
        break;
        case PTE_ASDM_MODULE:
            if(PTETaskEvt < PTE_TASK_ASDM_CONVERSION_START )
                return 0;
                reg_num = (PTETaskEvt - PTE_TASK_ASDM_CONVERSION_START) + (PTE_TASK_ASDM_CONVERSION_STOP - PTE_TASK_ASDM_CONVERSION_START) * TaskEvtSer;
            if(reg_num == 0)
                APB_PTE_BUS->PTE_ASDM.reg_task0 = 1ul;
            if(reg_num == 1ul)
                APB_PTE_BUS->PTE_ASDM.reg_task1 = 1ul;
            break;
        
        default:
            break;
    }
    return 1;
}

void PTE_SetTaskChxGroupEN(PTEC_ChannelGroup SetChannelGroup)
{   
    APB_PTE->reg_task_chg_en[SetChannelGroup] = 1ul;
}

void PTE_SetTaskChxGroupDis(PTEC_ChannelGroup SetChannelGroup)
{
    APB_PTE->reg_task_chg_DIS[SetChannelGroup] = 1ul;
}

void PTE_SetTaskChxGroupEnConfig(PTEC_ChannelGroup SetChannelGroup,uint32_t SetSubChannel)
{
    APB_PTE->reg_sub_task_chg_en[SetChannelGroup] = ((uint32_t)SetSubChannel | (1ul<<31));
}

void PTE_SetTaskChxGroupDisConfig(PTEC_ChannelGroup SetChannelGroup,uint32_t SetSubChannel)
{
    APB_PTE->reg_sub_task_chg_DIS[SetChannelGroup] = ((uint32_t)SetSubChannel | (1ul<<31));
}

void PTE_SetTaskChxGroupMap(PTEC_ChannelGroup SetChannelGroup,uint32_t SetGroupMap)
{
    APB_PTE->reg_chg_map[SetChannelGroup] = SetGroupMap;
}

#endif
