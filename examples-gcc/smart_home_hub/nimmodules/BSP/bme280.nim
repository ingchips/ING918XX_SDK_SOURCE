type
  INT8U* = cuchar

##  Unsigned  8 bit quantity

type
  INT8S* = cchar

##  Signed    8 bit quantity

type
  INT16U* = cushort

##  Unsigned 16 bit quantity

type
  INT16S* = cshort

##  Signed   16 bit quantity

type
  INT32U* = cuint

##  Unsigned 32 bit quantity

type
  INT32S* = cint

##  Signed   32 bit quantity
## **************************************************
## *\name	ERROR CODE DEFINITIONS
## *************************************************

const
  SUCCESS* = (cast[u8](0))
  E_BME280_NULL_PTR* = ((s8) - 127)
  E_BME280_COMM_RES* = ((s8) - 1)
  E_BME280_OUT_OF_RANGE* = ((s8) - 2)
  ERROR* = (cast[u8](0))

## **************************************************
## *\name	DEFINITIONS FOR ARRAY SIZE OF DATA
## *************************************************

const
  BME280_HUMIDITY_DATA_SIZE* = (2)
  BME280_TEMPERATURE_DATA_SIZE* = (3)
  BME280_PRESSURE_DATA_SIZE* = (3)
  BME280_DATA_FRAME_SIZE* = (8)

## **************************************************
## *\name	I2C ADDRESS DEFINITIONS
## *************************************************

const
  BME280_I2C_ADDRESS1* = (0x00000076)
  BME280_I2C_ADDRESS2* = (0x00000077)

## **************************************************
## *\name	POWER MODE DEFINITIONS
## *************************************************
##  Sensor Specific constants

const
  BME280_SLEEP_MODE* = (0x00000000)
  BME280_FORCED_MODE* = (0x00000001)
  BME280_NORMAL_MODE* = (0x00000003)
  BME280_SOFT_RESET_CODE* = (0x000000B6)

##  shift definitions

const
  BME280_SHIFT_BIT_POSITION_BY_01_BIT* = (1)
  BME280_SHIFT_BIT_POSITION_BY_02_BITS* = (2)
  BME280_SHIFT_BIT_POSITION_BY_03_BITS* = (3)
  BME280_SHIFT_BIT_POSITION_BY_04_BITS* = (4)
  BME280_SHIFT_BIT_POSITION_BY_07_BITS* = (7)
  BME280_SHIFT_BIT_POSITION_BY_08_BITS* = (8)
  BME280_SHIFT_BIT_POSITION_BY_10_BITS* = (10)
  BME280_SHIFT_BIT_POSITION_BY_11_BITS* = (11)
  BME280_SHIFT_BIT_POSITION_BY_12_BITS* = (12)
  BME280_SHIFT_BIT_POSITION_BY_13_BITS* = (13)
  BME280_SHIFT_BIT_POSITION_BY_14_BITS* = (14)
  BME280_SHIFT_BIT_POSITION_BY_15_BITS* = (15)
  BME280_SHIFT_BIT_POSITION_BY_16_BITS* = (16)
  BME280_SHIFT_BIT_POSITION_BY_17_BITS* = (17)
  BME280_SHIFT_BIT_POSITION_BY_18_BITS* = (18)
  BME280_SHIFT_BIT_POSITION_BY_19_BITS* = (19)
  BME280_SHIFT_BIT_POSITION_BY_20_BITS* = (20)
  BME280_SHIFT_BIT_POSITION_BY_25_BITS* = (25)
  BME280_SHIFT_BIT_POSITION_BY_31_BITS* = (31)
  BME280_SHIFT_BIT_POSITION_BY_33_BITS* = (33)
  BME280_SHIFT_BIT_POSITION_BY_35_BITS* = (35)
  BME280_SHIFT_BIT_POSITION_BY_47_BITS* = (47)

##  numeric definitions

const
  BME280_PRESSURE_TEMPERATURE_CALIB_DATA_LENGTH* = (26)
  BME280_HUMIDITY_CALIB_DATA_LENGTH* = (7)
  BME280_GEN_READ_WRITE_DATA_LENGTH* = (1)
  BME280_HUMIDITY_DATA_LENGTH* = (2)
  BME280_TEMPERATURE_DATA_LENGTH* = (3)
  BME280_PRESSURE_DATA_LENGTH* = (3)
  BME280_ALL_DATA_FRAME_LENGTH* = (8)
  BME280_INIT_VALUE* = (0)
  BME280_INVALID_DATA* = (0)

## **************************************************
## *\name	OVER SAMPLING DEFINITIONS
## *************************************************

const
  BME280_OVERSAMP_SKIPPED* = (0x00000000)
  BME280_OVERSAMP_1X* = (0x00000001)
  BME280_OVERSAMP_2X* = (0x00000002)
  BME280_OVERSAMP_4X* = (0x00000003)
  BME280_OVERSAMP_8X* = (0x00000004)
  BME280_OVERSAMP_16X* = (0x00000005)

## **************************************************
## *\name	WORK MODE DEFINITIONS
## *************************************************
##

const
  BME280_ULTRALOWPOWER_MODE* = (0x00000000)
  BME280_LOWPOWER_MODE* = (0x00000001)
  BME280_STANDARDRESOLUTION_MODE* = (0x00000002)
  BME280_HIGHRESOLUTION_MODE* = (0x00000003)
  BME280_ULTRAHIGHRESOLUTION_MODE* = (0x00000004)
  BME280_ULTRALOWPOWER_OSRS_P* = BME280_OVERSAMP_1X
  BME280_ULTRALOWPOWER_OSRS_T* = BME280_OVERSAMP_1X
  BME280_LOWPOWER_OSRS_P* = BME280_OVERSAMP_2X
  BME280_LOWPOWER_OSRS_T* = BME280_OVERSAMP_1X
  BME280_STANDARDRESOLUTION_OSRS_P* = BME280_OVERSAMP_4X
  BME280_STANDARDRESOLUTION_OSRS_T* = BME280_OVERSAMP_1X
  BME280_HIGHRESOLUTION_OSRS_P* = BME280_OVERSAMP_8X
  BME280_HIGHRESOLUTION_OSRS_T* = BME280_OVERSAMP_1X
  BME280_ULTRAHIGHRESOLUTION_OSRS_P* = BME280_OVERSAMP_16X
  BME280_ULTRAHIGHRESOLUTION_OSRS_T* = BME280_OVERSAMP_2X

## *< data frames includes temperature,
## pressure and humidity

const
  BME280_CALIB_DATA_SIZE* = (26)
  BME280_TEMPERATURE_MSB_DATA* = (0)
  BME280_TEMPERATURE_LSB_DATA* = (1)
  BME280_TEMPERATURE_XLSB_DATA* = (2)
  BME280_PRESSURE_MSB_DATA* = (0)
  BME280_PRESSURE_LSB_DATA* = (1)
  BME280_PRESSURE_XLSB_DATA* = (2)
  BME280_HUMIDITY_MSB_DATA* = (0)
  BME280_HUMIDITY_LSB_DATA* = (1)
  BME280_DATA_FRAME_PRESSURE_MSB_BYTE* = (0)
  BME280_DATA_FRAME_PRESSURE_LSB_BYTE* = (1)
  BME280_DATA_FRAME_PRESSURE_XLSB_BYTE* = (2)
  BME280_DATA_FRAME_TEMPERATURE_MSB_BYTE* = (3)
  BME280_DATA_FRAME_TEMPERATURE_LSB_BYTE* = (4)
  BME280_DATA_FRAME_TEMPERATURE_XLSB_BYTE* = (5)
  BME280_DATA_FRAME_HUMIDITY_MSB_BYTE* = (6)
  BME280_DATA_FRAME_HUMIDITY_LSB_BYTE* = (7)

## **************************************************
## *\name	ARRAY PARAMETER FOR CALIBRATION
## *************************************************

const
  BME280_TEMPERATURE_CALIB_DIG_T1_LSB* = (0)
  BME280_TEMPERATURE_CALIB_DIG_T1_MSB* = (1)
  BME280_TEMPERATURE_CALIB_DIG_T2_LSB* = (2)
  BME280_TEMPERATURE_CALIB_DIG_T2_MSB* = (3)
  BME280_TEMPERATURE_CALIB_DIG_T3_LSB* = (4)
  BME280_TEMPERATURE_CALIB_DIG_T3_MSB* = (5)

## **************************************************
## *\name	CALIBRATION REGISTER ADDRESS DEFINITIONS
## *************************************************
## calibration parameters

const
  BME280_TEMPERATURE_CALIB_DIG_T1_LSB_REG* = (0x00000088)
  BME280_TEMPERATURE_CALIB_DIG_T1_MSB_REG* = (0x00000089)
  BME280_TEMPERATURE_CALIB_DIG_T2_LSB_REG* = (0x0000008A)
  BME280_TEMPERATURE_CALIB_DIG_T2_MSB_REG* = (0x0000008B)
  BME280_TEMPERATURE_CALIB_DIG_T3_LSB_REG* = (0x0000008C)
  BME280_TEMPERATURE_CALIB_DIG_T3_MSB_REG* = (0x0000008D)
  BME280_PRESSURE_CALIB_DIG_P1_LSB_REG* = (0x0000008E)
  BME280_PRESSURE_CALIB_DIG_P1_MSB_REG* = (0x0000008F)
  BME280_PRESSURE_CALIB_DIG_P2_LSB_REG* = (0x00000090)
  BME280_PRESSURE_CALIB_DIG_P2_MSB_REG* = (0x00000091)
  BME280_PRESSURE_CALIB_DIG_P3_LSB_REG* = (0x00000092)
  BME280_PRESSURE_CALIB_DIG_P3_MSB_REG* = (0x00000093)
  BME280_PRESSURE_CALIB_DIG_P4_LSB_REG* = (0x00000094)
  BME280_PRESSURE_CALIB_DIG_P4_MSB_REG* = (0x00000095)
  BME280_PRESSURE_CALIB_DIG_P5_LSB_REG* = (0x00000096)
  BME280_PRESSURE_CALIB_DIG_P5_MSB_REG* = (0x00000097)
  BME280_PRESSURE_CALIB_DIG_P6_LSB_REG* = (0x00000098)
  BME280_PRESSURE_CALIB_DIG_P6_MSB_REG* = (0x00000099)
  BME280_PRESSURE_CALIB_DIG_P7_LSB_REG* = (0x0000009A)
  BME280_PRESSURE_CALIB_DIG_P7_MSB_REG* = (0x0000009B)
  BME280_PRESSURE_CALIB_DIG_P8_LSB_REG* = (0x0000009C)
  BME280_PRESSURE_CALIB_DIG_P8_MSB_REG* = (0x0000009D)
  BME280_PRESSURE_CALIB_DIG_P9_LSB_REG* = (0x0000009E)
  BME280_PRESSURE_CALIB_DIG_P9_MSB_REG* = (0x0000009F)
  BME280_HUMIDITY_CALIB_DIG_H1_REG* = (0x000000A1)
  BME280_HUMIDITY_CALIB_DIG_H2_LSB_REG* = (0x000000E1)
  BME280_HUMIDITY_CALIB_DIG_H2_MSB_REG* = (0x000000E2)
  BME280_HUMIDITY_CALIB_DIG_H3_REG* = (0x000000E3)
  BME280_HUMIDITY_CALIB_DIG_H4_MSB_REG* = (0x000000E4)
  BME280_HUMIDITY_CALIB_DIG_H4_LSB_REG* = (0x000000E5)
  BME280_HUMIDITY_CALIB_DIG_H5_MSB_REG* = (0x000000E6)
  BME280_HUMIDITY_CALIB_DIG_H6_REG* = (0x000000E7)

## **************************************************
## *\name	REGISTER ADDRESS DEFINITIONS
## *************************************************

const
  BME280_CHIP_ID_REG* = (0x000000D0) ## Chip ID Register
  BME280_RST_REG* = (0x000000E0) ## Softreset Register
  BME280_STAT_REG* = (0x000000F3) ## Status Register
  BME280_CTRL_MEAS_REG* = (0x000000F4) ## Ctrl Measure Register
  BME280_CTRL_HUMIDITY_REG* = (0x000000F2) ## Ctrl Humidity Register
  BME280_CONFIG_REG* = (0x000000F5) ## Configuration Register
  BME280_PRESSURE_MSB_REG* = (0x000000F7) ## Pressure MSB Register
  BME280_PRESSURE_LSB_REG* = (0x000000F8) ## Pressure LSB Register
  BME280_PRESSURE_XLSB_REG* = (0x000000F9) ## Pressure XLSB Register
  BME280_TEMPERATURE_MSB_REG* = (0x000000FA) ## Temperature MSB Reg
  BME280_TEMPERATURE_LSB_REG* = (0x000000FB) ## Temperature LSB Reg
  BME280_TEMPERATURE_XLSB_REG* = (0x000000FC) ## Temperature XLSB Reg
  BME280_HUMIDITY_MSB_REG* = (0x000000FD) ## Humidity MSB Reg
  BME280_HUMIDITY_LSB_REG* = (0x000000FE) ## Humidity LSB Reg

## **************************************************
## *\name	BIT MASK, LENGTH AND POSITION DEFINITIONS
## *************************************************
##  Status Register

const
  BME280_STAT_REG_MEASURING__POS* = (3)
  BME280_STAT_REG_MEASURING__MSK* = (0x00000008)
  BME280_STAT_REG_MEASURING__LEN* = (1)
  BME280_STAT_REG_MEASURING__REG* = (BME280_STAT_REG)
  BME280_STAT_REG_IM_UPDATE__POS* = (0)
  BME280_STAT_REG_IM_UPDATE__MSK* = (0x00000001)
  BME280_STAT_REG_IM_UPDATE__LEN* = (1)
  BME280_STAT_REG_IM_UPDATE__REG* = (BME280_STAT_REG)

## **************************************************
## *\name	BIT MASK, LENGTH AND POSITION DEFINITIONS
## FOR TEMPERATURE OVERSAMPLING
## *************************************************
##  Control Measurement Register

const
  BME280_CTRL_MEAS_REG_OVERSAMP_TEMPERATURE__POS* = (5)
  BME280_CTRL_MEAS_REG_OVERSAMP_TEMPERATURE__MSK* = (0x000000E0)
  BME280_CTRL_MEAS_REG_OVERSAMP_TEMPERATURE__LEN* = (3)
  BME280_CTRL_MEAS_REG_OVERSAMP_TEMPERATURE__REG* = (BME280_CTRL_MEAS_REG)

## ************************************************************
## *\name	STRUCTURE DEFINITIONS
## ************************************************************
## !
##  @brief This structure holds all device specific calibration parameters
##

type
  bme280_calibration_param_t* {.importc: "bme280_calibration_param_t",
                               header: "bme280.h", bycopy.} = object
    dig_T1* {.importc: "dig_T1".}: INT16U ## *<calibration T1 data
    dig_T2* {.importc: "dig_T2".}: INT16S ## *<calibration T2 data
    dig_T3* {.importc: "dig_T3".}: INT16S ## *<calibration T3 data
    t_fine* {.importc: "t_fine".}: INT32S ## *<calibration T_FINE data


## !
##  @brief This structure holds BME280 initialization parameters
##

type
  bme280_t* {.importc: "bme280_t", header: "bme280.h", bycopy.} = object
    cal_param* {.importc: "cal_param".}: bme280_calibration_param_t ## *< calibration parameters
    chip_id* {.importc: "chip_id".}: uint8 ## *< chip id of the sensor
    dev_addr* {.importc: "dev_addr".}: uint8 ## *< device address of the sensor
    osrs_p* {.importc: "osrs_p".}: uint8
    oversamp_temperature* {.importc: "oversamp_temperature".}: uint8 ## *< temperature over sampling
    oversamp_pressure* {.importc: "oversamp_pressure".}: uint8 ## *< pressure over sampling
    oversamp_humidity* {.importc: "oversamp_humidity".}: uint8 ## *< humidity over sampling
    ctrl_hum_reg* {.importc: "ctrl_hum_reg".}: uint8 ## *< status of control humidity register
    ctrl_meas_reg* {.importc: "ctrl_meas_reg".}: uint8 ## *< status of control measurement register
    config_reg* {.importc: "config_reg".}: uint8 ## *< status of configuration register


proc Chip_id_read*(): uint8 {.importc: "Chip_id_read", header: "bme280.h".}
proc bme280_set_soft_rst*() {.importc: "bme280_set_soft_rst", header: "bme280.h".}
proc bme280_get_calib_param*(): INT8U {.importc: "bme280_get_calib_param",
                                     header: "bme280.h".}
proc bme280_init*(data: ptr bme280_t): INT8U {.importc: "bme280_init",
    header: "bme280.h".}
proc bme280_write_register*(v_addr_u8: INT8U; v_data_u8: INT8U): INT8U {.
    importc: "bme280_write_register", header: "bme280.h".}
proc bme280_read_register*(v_addr_u8: INT8U): INT8U {.
    importc: "bme280_read_register", header: "bme280.h".}
proc bme280_set_config*(v_data_u8: INT8U): INT8U {.importc: "bme280_set_config",
    header: "bme280.h".}
proc bme280_set_power_mode*(v_power_mode_u8: INT8U): INT8U {.
    importc: "bme280_set_power_mode", header: "bme280.h".}
proc bme280_set_work_mode*(v_work_mode_u8: INT8U): INT8U {.
    importc: "bme280_set_work_mode", header: "bme280.h".}
proc bme280_read_uncomp_temperature*(): INT32S {.
    importc: "bme280_read_uncomp_temperature", header: "bme280.h".}
proc bme280_compensate_temperature_int32_sixteen_bit_output*(
    v_uncomp_temperature_s32: INT32S): INT16S {.
    importc: "bme280_compensate_temperature_int32_sixteen_bit_output",
    header: "bme280.h".}
proc bme280_compensate_temperature_int32*(v_uncomp_temperature_s32: INT32S): INT32S {.
    importc: "bme280_compensate_temperature_int32", header: "bme280.h".}
proc bme280_compensate_temperature_read*(): INT32S {.
    importc: "bme280_compensate_temperature_read", header: "bme280.h".}