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

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)

typedef struct
{
    uint16_t task_offset;
    uint16_t sub_task_offset;
    uint16_t event_offset;
    uint8_t task_size;
    uint8_t event_size;
} PTE_REG_OFFSET;

static const PTE_REG_OFFSET reg_Table[13] = {
    {0x0, 0x20, 0x40, 6, 3},// QDEC
    {0x0, 0x0, 0x50,0, 8},// DMA
    {0x80, 0xe0, 0x140, 24, 8},// GPIOTE
    {0x170, 0x1b0, 0x1f0, 16, 8},// TIMER0
    {0x220, 0x228, 0x230, 2, 1},// SPI0
    {0x240, 0x244, 0x248, 1, 1},// I2C
    {0x250, 0x260, 0x270, 3, 2},// ADC
    {0x280, 0x290, 0x2a0, 4, 2},// UART
    {0x2b0, 0x2c0, 0x2d0, 4, 2},//UART1
    {0x2e0, 0x2e8, 0x2f0, 2, 1},// SPI1
    {0x300, 0x360, 0x3c0, 21, 9},// PWM
    {0x3f0, 0x430, 0x470,16, 8},// TIMER1
    {0x4a0, 0x4b0, 0x0, 4, 0},// I2S
};

uint8_t PTE_ConfigTask(PTE_Module module, PTE_Task Task, PTE_Channel SetTaskChannel, uint8_t enable)
{
    volatile uint32_t* reg;
    
    if (PTE_CHANNEL_15 < SetTaskChannel)
        return 1;

    if (PTE_ASDM_MODULE == module)
    {
        if (PTE_TASK_ASDM_RX_START == Task)
            reg = &APB_PTE_BUS->PTE_ASDM.reg_sub_task0;
        else if (PTE_TASK_ASDM_RX_STOP == Task)
            reg = &APB_PTE_BUS->PTE_ASDM.reg_sub_task1;
        else
            return 1;
    }
    else
    {
        if (Task < reg_Table[module].task_size)
            reg = (volatile uint32_t *)(APB_PTE_BUS_BASE + reg_Table[(uint8_t)module].sub_task_offset + (uint8_t)Task * 4);
        else
            return 1;
    }
    if (enable)
        *reg = SetTaskChannel | (1ul<<31);
    else
        *reg = SetTaskChannel;

    return 0;
}

uint8_t PTE_ConfigEvent(PTE_Module module, PTE_Event Event, PTE_Channel SetEventChannel, uint8_t enable)
{
    volatile uint32_t* reg;
    
    if (PTE_CHANNEL_15 < SetEventChannel)
        return 1;

    if (PTE_ASDM_MODULE == module)
    {
        if (PTE_EVENT_ASDM_RX_DATA == Event)
            reg = &APB_PTE_BUS->PTE_ASDM.reg_pub_event0;
        else
            return 1;
    }
    else
    {
        if (Event < reg_Table[module].event_size)
            reg = (volatile uint32_t *)(APB_PTE_BUS_BASE + reg_Table[(uint8_t)module].event_offset + ((uint8_t)Event + 1) * 4);
        else
            return 1;
    }
    if (enable)
        *reg = SetEventChannel | (1ul<<31);
    else
        *reg = SetEventChannel;

    return 0;
}

uint8_t PTE_TriggerTask(PTE_Module module, PTE_Task Task)
{
    if (module == PTE_ASDM_MODULE)
    {
        if (PTE_TASK_ASDM_RX_START == Task)
            APB_PTE_BUS->PTE_ASDM.reg_task0 = 1;
        else if (PTE_TASK_ASDM_RX_STOP == Task)
            APB_PTE_BUS->PTE_ASDM.reg_task1 = 1;
        else
            return 1;
    }
    else
    {
        if (Task < reg_Table[module].task_size)
            *(volatile uint32_t *)(APB_PTE_BUS_BASE + reg_Table[(uint8_t)module].task_offset + (uint8_t)Task * 4) = 1;
        else
            return 1;
    }
    return 0;
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

void PTE_SetTaskChxGroupEN(PTEC_ChannelGroup SetChannelGroup)
{
    APB_PTE->reg_task_chg_en[SetChannelGroup] = 1ul;
}

void PTE_SetTaskChxGroupDis(PTEC_ChannelGroup SetChannelGroup)
{
    APB_PTE->reg_task_chg_DIS[SetChannelGroup] = 1ul;
}

void PTE_SetTaskSubChxGroupEn(PTEC_ChannelGroup SetChannelGroup,PTE_Channel SetSubChannel)
{
    APB_PTE->reg_sub_task_chg_DIS[SetChannelGroup] = (uint32_t)SetSubChannel;
    APB_PTE->reg_sub_task_chg_en[SetChannelGroup] = ((uint32_t)SetSubChannel | (1ul<<31));
}

void PTE_SetTaskSubChxGroupDis(PTEC_ChannelGroup SetChannelGroup,PTE_Channel SetSubChannel)
{
    APB_PTE->reg_sub_task_chg_en[SetChannelGroup] = (uint32_t)SetSubChannel;
    APB_PTE->reg_sub_task_chg_DIS[SetChannelGroup] = ((uint32_t)SetSubChannel | (1ul<<31));
}

void PTE_SetTaskChxGroupMap(PTEC_ChannelGroup SetChannelGroup,uint32_t SetGroupMap)
{
    APB_PTE->reg_chg_map[SetChannelGroup] = SetGroupMap;
}

#define GPIOTE_REG_BASE (volatile uint32_t*)(0x400001f0)
void GPIOTE_Configuration(GPIOTE_Channel channel, GPIOTE_Mode mode, uint8_t pin, uint8_t out_level)
{
    *(GPIOTE_REG_BASE+(uint8_t)channel) = (mode&0x3) | ((pin&0x3f) << 8) | ((out_level&0x1) << 24);
}

#endif
