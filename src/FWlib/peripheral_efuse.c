
#include "peripheral_efuse.h"

#if INGCHIPS_FAMILY == INGCHIPS_FAMILY_916

static void nop(uint32_t n)
{
    uint32_t i;
    for(i=0;i<n;i++){
        __asm volatile ("nop");
    }
}

void EFUSE_Single_Program(EFUSE_REG_H* EFUSE_BASE, uint32_t SrcData[], uint32_t DstData[])
{
    uint8_t i, j;
    EFUSE_BASE->efuse_cfg0.f.prom_mode = 0x0;
    EFUSE_BASE->efuse_cfg0.f.efuse_lock = 0xef5e;
    for(i = 0; i < EFUSE_REG_WORD_NUM; i++){
        for(j = 0; j < 32; j++){
            if(((SrcData[i] >> j )& 0x1) == 0x1){
                EFUSE_BASE->efuse_cfg0.f.prom_addr = i*32+j;
                EFUSE_BASE->efuse_cfg0.f.pg_efuse = 0x1;
                nop(20);
                while(EFUSE_BASE->efuse_status.f.busy == 0x1);
            }
        }
    }
    EFUSE_BASE->efuse_cfg0.f.rd_efuse = 0x1;
    while(EFUSE_BASE->efuse_cfg0.f.data_valid == 0x0);
    DstData[0] = EFUSE_BASE->efuse_rdata0.r;
    DstData[1] = EFUSE_BASE->efuse_rdata1.r;
    DstData[2] = EFUSE_BASE->efuse_rdata2.r;
    DstData[3] = EFUSE_BASE->efuse_rdata3.r;
}

void EFUSE_Whole_Program(EFUSE_REG_H* EFUSE_BASE, uint32_t SrcData[], uint32_t DstData[])
{
    //sys_ctrl_reg->rstu_cfg1.r = sys_ctrl_reg->rstu_cfg1.r & (~(0x1<<1)) & (~(0x1<<15));
    //nop(10);
    //sys_ctrl_reg->rstu_cfg1.r = sys_ctrl_reg->rstu_cfg1.r | (0x1<<1) | (0x1<<15);
    EFUSE_BASE->efuse_cfg0.f.prom_mode = 0x1;
    EFUSE_BASE->efuse_cfg1.r = SrcData[0];
    EFUSE_BASE->efuse_cfg2.r = SrcData[1];
    EFUSE_BASE->efuse_cfg3.r = SrcData[2];
    EFUSE_BASE->efuse_cfg4.r = SrcData[3];
    EFUSE_BASE->efuse_cfg0.f.efuse_lock = 0xef5e;
    EFUSE_BASE->efuse_cfg0.f.pg_efuse = 0x1;
    nop(20);
    while(EFUSE_BASE->efuse_status.f.busy == 0x1);
    EFUSE_BASE->efuse_cfg0.f.rd_efuse = 0x1;
    while(EFUSE_BASE->efuse_cfg0.f.data_valid == 0x0);
    DstData[0] = EFUSE_BASE->efuse_rdata0.r;
    DstData[1] = EFUSE_BASE->efuse_rdata1.r;
    DstData[2] = EFUSE_BASE->efuse_rdata2.r;
    DstData[3] = EFUSE_BASE->efuse_rdata3.r;
}

#endif
