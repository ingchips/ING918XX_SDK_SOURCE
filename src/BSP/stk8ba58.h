#ifndef _STK8BA58_H_
#define _STK8BA58_H_

#include "stdint.h"

/*
 * Description:
 * Bus read and write function pointers
 */
#define STK8BA58_WR_FUNC_PTR int8_t(*bus_write)\
(uint8_t, uint8_t, uint8_t *, uint8_t)

#define STK8BA58_BUS_WRITE_FUNC(dev_addr, reg_addr, reg_data, wr_len)\
bus_write(dev_addr, reg_addr, reg_data, wr_len)

#define STK8BA58_RD_FUNC_PTR int8_t(*bus_read)\
(uint8_t, uint8_t, uint8_t *, uint8_t)
#define STK8BA58_BRD_FUNC_PTR int8_t(*burst_read)\
(uint8_t, uint8_t, uint8_t *, uint32_t)

#define STK8BA58_BUS_READ_FUNC(dev_addr, reg_addr, reg_data, r_len)\
bus_read(dev_addr, reg_addr, reg_data, r_len)
#define STK8BA58_BURST_READ_FUNC(device_addr,\
register_addr, register_data, rd_len)\
burst_read(device_addr, register_addr, register_data, rd_len)

/**
 * @brief I2C address definition
 */
#define STK8BA58_I2C_ADDR   (0x18)

/**
 * @brief Constants definition
 */
#define         STK8BA58_INIT_VALUE                       (0)
#define         STK8BA58_GEN_READ_WRITE_LENGTH            (1)
#define		    STK8BA58_INTERFACE_IDLE_TIME_DELAY	      (1)
#define         STK8BA58_LSB_MSB_READ_LENGTH		      (2)
#define         STK8BA58_SHIFT_TWO_BITS                   (2)
#define         STK8BA58_SHIFT_FOUR_BITS                  (4)
#define         STK8BA58_SHIFT_FIVE_BITS                  (5)
#define         STK8BA58_SHIFT_SIX_BITS                   (6)
#define         STK8BA58_SHIFT_EIGHT_BITS                 (8)


/**
 * @brief STK8BA58 register address definition
 */
#define STK8BA58_CHIP_ID_ADDR                    (0x00)
#define STK8BA58_X_OUT_LSB_ADDR                  (0x02)
#define STK8BA58_X_OUT_MSB_ADDR                  (0x03)
#define STK8BA58_Y_OUT_LSB_ADDR                  (0x04)
#define STK8BA58_Y_OUT_MSB_ADDR                  (0x05)
#define STK8BA58_Z_OUT_LSB_ADDR                  (0x06)
#define STK8BA58_Z_OUT_MSB_ADDR                  (0x07)
#define STK8BA58_INISTS1_ADDR                    (0x09) 
#define STK8BA58_INISTS2_ADDR                    (0x0A)  
#define STK8BA58_EVENTINFO1_ADDR                 (0x0B)
#define STK8BA58_RANGE_SELECT_ADDR               (0x0F)
#define STK8BA58_BWSEL_ADDR                      (0x10)
#define STK8BA58_POWMODE_ADDR                    (0x11)
#define STK8BA58_DATASETUP_ADDR                  (0x13)
#define STK8BA58_SWRST_ADDR                      (0x14)
#define STK8BA58_INTEN1_ADDR                     (0x16)
#define STK8BA58_INTEN2_ADDR                     (0x17)
#define STK8BA58_INTMAP1_ADDR                    (0x19)
#define STK8BA58_INTMAP2_ADDR                    (0x1A)
#define STK8BA58_INTCFG1_ADDR                    (0x20)
#define STK8BA58_INTCFG2_ADDR                    (0x21)
#define STK8BA58_SLOPEDLY_ADDR                   (0x27)
#define STK8BA58_SLOPETHD_ADDR                   (0x28)
#define STK8BA58_SIGMOT1_ADDR                    (0x29)
#define STK8BA58_SIGMOT2_ADDR                    (0x2A)
#define STK8BA58_SIGMOT3_ADDR                    (0x2B)
#define STK8BA58_INTFCFG_ADDR                    (0x34)
#define STK8BA58_OFSTCOMP_ADDR                   (0x36) 
#define STK8BA58_X_OFST_ADDR                     (0x38)
#define STK8BA58_Y_OFST_ADDR                     (0x39)
#define STK8BA58_Z_OFST_ADDR                     (0x3A)

/**
 * @brief Error code definition.
 */
#define E_OUT_OF_RANGE              ((int8_t)-2)
#define E_STK8BA58_NULL_PTR         ((int8_t)-127)
#define STK8BA58_NULL               ((void *)0)
#define ERROR			            ((int8_t)-1)
#define	SUCCESS			            ((uint8_t)0)

/**
 * @brief Return type definition.
 */
#define	STK8BA58_RETURN_FUNCTION_TYPE        int8_t


struct stk8ba58_accel_data
{
    int16_t x;
    int16_t y;
    int16_t z;
};

struct stk8ba58_t
{
	uint8_t power_mode;
	uint8_t chip_id;
	uint8_t ctrl_mode_reg;
	uint8_t low_mode_reg;
	uint8_t dev_addr;
    STK8BA58_WR_FUNC_PTR;
	STK8BA58_RD_FUNC_PTR;
	STK8BA58_BRD_FUNC_PTR;
};

/**
 * @brief Bit shift and width for STK8BA58 register.
 */
#define bsSTK8BA58_CHIP_ID                  (0)
#define bwSTK8BA58_CHIP_ID                  (8)
#define bsSTK8BA58_NEW_DATA                 (0)
#define bwSTK8BA58_NEW_DATA_LEN             (1)
#define bsSTK8BA58_ACCEL_DATA_LSB           (4)
#define bwSTK8BA58_ACCEL_DATA_LSB           (4)
#define bsSTK8BA58_ACCEL_DATA_MSB           (0)
#define bwSTK8BA58_ACCEL_DATA_MSB           (8)
#define bsSTK8BA58_ANY_MOT_INTR_STS         (0)
#define bwSTK8BA58_ANY_MOT_INTR_STS         (1)
#define bsSTK8BA58_SIG_MOT_INTR_STS         (2)
#define bwSTK8BA58_SIG_MOT_INTR_STS         (1)
#define bsSTK8BA58_NEW_DATA_INTR_STS        (7)
#define bwSTK8BA58_NEW_DATA_INTR_STS        (1)
#define bsSTK8BA58_RANGE_SELECT             (0)
#define bwSTK8BA58_RANGE_SELECT             (4)
#define bsSTK8BA58_BWSEL                    (0)
#define bwSTK8BA58_BWSEL                    (5)


/**
 * @brief Bit mask for STK8BA58 register.
 */
#define bmSTK8BA58_CHIP_ID                  (0xFF)
#define bmSTK8BA58_NEW_DATA                 (0x01)
#define bmSTK8BA58_ACCEL_DATA_LSB           (0xF0)
#define bmSTK8BA58_ACCEL_DATA_MSB           (0xFF)
#define bmSTK8BA58_RANGE_SELECT             (0x0F)
#define bmSTK8BA58_BWSEL                    (0x1F)
#define STK8BA58_RESOLUTION_12_MASK		    (0xF0)


/**
 * @brief Bislice function definition.
 */
#define STK8BA58_GET_BITSLICE(regvar, bitname)\
((regvar & bm##bitname) >> bs##bitname)

#define STK8BA58_SET_BITSLICE(regvar, bitname, val)\
((regvar & ~bm##bitname) | ((val<<bs##bitname)&bm##bitname))


typedef enum
{
    STK8BA58_RANGE_2G   = 0x03,
    STK8BA58_RANGE_4G   = 0x05,
    STK8BA58_RANGE_8G   = 0x08,
}stk8ba58_range_sel;

typedef enum
{
    STK8BA58_BW_7_81HZ  = 0x08,	// 7.81 Hz   64000 uS
    STK8BA58_BW_15_63HZ = 0x09, // 15.63 Hz   32000 uS
    STK8BA58_BW_31_25HZ = 0x0A, // 31.25 Hz   16000 uS
    STK8BA58_BW_62_50HZ = 0x0B, // 62.50 Hz   8000 uS
    STK8BA58_BW_125HZ   = 0x0C, // 125 Hz   4000 uS
    STK8BA58_BW_250HZ   = 0x0D, // 250 Hz   2000 uS
    STK8BA58_BW_500HZ   = 0x0E, // 500 Hz   1000 uS
    STK8BA58_BW_1000HZ  = 0x0F, // 1000 Hz   500 uS
}stk8ba58_bw_sel;

typedef enum
{
    STK8BA58_SLEEP_DURN_0_5MS   = 0x05,
    STK8BA58_SLEEP_DURN_1MS     = 0x06,
    STK8BA58_SLEEP_DURN_2MS     = 0x07,
    STK8BA58_SLEEP_DURN_4MS     = 0x08,
    STK8BA58_SLEEP_DURN_6MS     = 0x09,
    STK8BA58_SLEEP_DURN_10MS    = 0x0A,
    STK8BA58_SLEEP_DURN_25MS    = 0x0B,
    STK8BA58_SLEEP_DURN_50MS    = 0x0C,
    STK8BA58_SLEEP_DURN_100MS   = 0x0D,
    STK8BA58_SLEEP_DURN_500MS   = 0x0E,
    STK8BA58_SLEEP_DURN_1S      = 0x0F,
}stk8ba58_slp_durn;

typedef enum
{
    STK8BA58_LATCH_INT_NON_LATCH0   = 0x00,
    STK8BA58_LATCH_INT_TEMP_250MS   = 0x01,
    STK8BA58_LATCH_INT_TEMP_500MS   = 0x02,
    STK8BA58_LATCH_INT_TEMP_1S      = 0x03,
    STK8BA58_LATCH_INT_TEMP_2S      = 0x04,
    STK8BA58_LATCH_INT_TEMP_4S      = 0x05,
    STK8BA58_LATCH_INT_TEMP_8S      = 0x06,
    STK8BA58_LATCH_INT_LATCH0       = 0x07,
    STK8BA58_LATCH_INT_NON_LATCH1   = 0x08,
    STK8BA58_LATCH_INT_TEMP_250US   = 0x09,
    STK8BA58_LATCH_INT_TEMP_500US   = 0x0A,
    STK8BA58_LATCH_INT_TEMP_1MS     = 0x0B,
    STK8BA58_LATCH_INT_TEMP_12_5MS  = 0x0C,
    STK8BA58_LATCH_INT_TEMP_25MS    = 0x0D,
    STK8BA58_LATCH_INT_TEMP_50MS    = 0x0E,
    STK8BA58_LATCH_INT_LATCH1       = 0x0F,
}stk8ba58_int_latch;

/**
 * @brief Array parameters.
 */
#define STK8BA58_SENSOR_DATA_ACCEL_LSB	(0)
#define STK8BA58_SENSOR_DATA_ACCEL_MSB	(1)
#define STK8BA58_SENSOR_DATA_XYZ_X_LSB	(0)
#define STK8BA58_SENSOR_DATA_XYZ_X_MSB	(1)
#define STK8BA58_SENSOR_DATA_XYZ_Y_LSB	(2)
#define STK8BA58_SENSOR_DATA_XYZ_Y_MSB	(3)
#define STK8BA58_SENSOR_DATA_XYZ_Z_LSB	(4)
#define STK8BA58_SENSOR_DATA_XYZ_Z_MSB	(5)


/**
 * @brief Function declaration.
 */
STK8BA58_RETURN_FUNCTION_TYPE stk8ba58_init(struct stk8ba58_t *stk8ba58);
STK8BA58_RETURN_FUNCTION_TYPE stk8ba58_write_reg(uint8_t addr, uint8_t *data, uint8_t len);
STK8BA58_RETURN_FUNCTION_TYPE stk8ba58_read_reg(uint8_t addr, uint8_t *data, uint8_t len);

STK8BA58_RETURN_FUNCTION_TYPE stk8ba58_read_accel_xyz(struct stk8ba58_accel_data *accel);

STK8BA58_RETURN_FUNCTION_TYPE stk8ba58_get_range(uint8_t *range);
STK8BA58_RETURN_FUNCTION_TYPE stk8ba58_set_range(uint8_t range);

STK8BA58_RETURN_FUNCTION_TYPE stk8ba58_get_bw(uint8_t *bw);
STK8BA58_RETURN_FUNCTION_TYPE stk8ba58_set_bw(uint8_t bw);

#endif  //_STK8BA58_H_