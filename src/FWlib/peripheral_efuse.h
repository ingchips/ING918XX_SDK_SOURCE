
#ifndef __PERIPHERAL_EFUSE_H__
#define __PERIPHERAL_EFUSE_H__


#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#include "ingsoc.h"


#if INGCHIPS_FAMILY == INGCHIPS_FAMILY_916

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t prom_mode                        : 1 ;
        volatile uint32_t prom_addr                        : 7 ;
        volatile uint32_t pg_efuse                         : 1 ;
        volatile uint32_t rd_efuse                         : 1 ;
        volatile uint32_t data_valid                       : 1 ;
        volatile uint32_t efuse_lock_flag                  : 1 ;
        volatile uint32_t efuse_lock                       : 16;
        volatile uint32_t unused0                          : 4 ;
    }f;
}efuse_REG_efuse_cfg0;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t prom_cfg0                        : 32;
    }f;
}efuse_REG_efuse_cfg1;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t prom_cfg1                        : 32;
    }f;
}efuse_REG_efuse_cfg2;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t prom_cfg2                        : 32;
    }f;
}efuse_REG_efuse_cfg3;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t prom_cfg3                        : 32;
    }f;
}efuse_REG_efuse_cfg4;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t t_hp_a_cfg                       : 8 ;
        volatile uint32_t t_hppgm_cfg                      : 3 ;
        volatile uint32_t t_hppgavdd_cfg                   : 8 ;
        volatile uint32_t t_sra_cfg                        : 3 ;
        volatile uint32_t t_rd_cfg                         : 4 ;
        volatile uint32_t t_srrd_cfg                       : 3 ;
        volatile uint32_t t_hr_a_cfg                       : 3 ;
    }f;
}efuse_REG_efuse_dly_cfg0;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t t_hrrd_cfg                       : 3 ;
        volatile uint32_t t_end_dly                        : 3 ;
        volatile uint32_t t_sppgavdd_cfg                   : 8 ;
        volatile uint32_t t_spa_cfg                        : 3 ;
        volatile uint32_t t_a2sppgm_cfg                    : 4 ;
        volatile uint32_t t_pgm_cfg                        : 8 ;
        volatile uint32_t unused1                          : 3 ;
    }f;
}efuse_REG_efuse_dly_cfg1;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t busy                             : 1 ;
        volatile uint32_t state                            : 4 ;
        volatile uint32_t unused2                          : 27;
    }f;
}efuse_REG_efuse_status;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t rdata_efuse0                     : 32;
    }f;
}efuse_REG_efuse_rdata0;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t rdata_efuse1                     : 32;
    }f;
}efuse_REG_efuse_rdata1;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t rdata_efuse2                     : 32;
    }f;
}efuse_REG_efuse_rdata2;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t rdata_efuse3                     : 32;
    }f;
}efuse_REG_efuse_rdata3;

typedef struct
{
  volatile efuse_REG_efuse_cfg0                                        efuse_cfg0                    ; //0x0
  volatile efuse_REG_efuse_cfg1                                        efuse_cfg1                    ; //0x4
  volatile efuse_REG_efuse_cfg2                                        efuse_cfg2                    ; //0x8
  volatile efuse_REG_efuse_cfg3                                        efuse_cfg3                    ; //0xC
  volatile uint32_t                                                    null_reg0                     ; //0x10
  volatile uint32_t                                                    null_reg1                     ; //0x14
  volatile uint32_t                                                    null_reg2                     ; //0x18
  volatile efuse_REG_efuse_cfg4                                        efuse_cfg4                    ; //0x1C
  volatile efuse_REG_efuse_dly_cfg0                                    efuse_dly_cfg0                ; //0x20
  volatile efuse_REG_efuse_dly_cfg1                                    efuse_dly_cfg1                ; //0x24
  volatile uint32_t                                                    null_reg3                     ; //0x28
  volatile uint32_t                                                    null_reg4                     ; //0x2C
  volatile efuse_REG_efuse_status                                      efuse_status                  ; //0x30
  volatile uint32_t                                                    null_reg5                     ; //0x34
  volatile uint32_t                                                    null_reg6                     ; //0x38
  volatile uint32_t                                                    null_reg7                     ; //0x3C
  volatile efuse_REG_efuse_rdata0                                      efuse_rdata0                  ; //0x40
  volatile efuse_REG_efuse_rdata1                                      efuse_rdata1                  ; //0x44
  volatile efuse_REG_efuse_rdata2                                      efuse_rdata2                  ; //0x48
  volatile efuse_REG_efuse_rdata3                                      efuse_rdata3                  ; //0x4C
}EFUSE_REG_H;

#define EFUSE_REG_WORD_NUM (4)

#endif

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

