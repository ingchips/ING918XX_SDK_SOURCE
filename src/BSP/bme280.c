#include "bme280.h"	  
 
struct bme280_t *p_bme280; 

#include "iic.h"

#define DEV_ADDR        0x76

/*!
 *	@brief This API is used to
 *	calibration parameters used for calculation in the registers
 *
 *  parameter | Register address |   bit
 *------------|------------------|----------------
 *	dig_T1    |  0x88 and 0x89   | from 0 : 7 to 8: 15
 *	dig_T2    |  0x8A and 0x8B   | from 0 : 7 to 8: 15
 *	dig_T3    |  0x8C and 0x8D   | from 0 : 7 to 8: 15
 *	dig_P1    |  0x8E and 0x8F   | from 0 : 7 to 8: 15
 *	dig_P2    |  0x90 and 0x91   | from 0 : 7 to 8: 15
 *	dig_P3    |  0x92 and 0x93   | from 0 : 7 to 8: 15
 *	dig_P4    |  0x94 and 0x95   | from 0 : 7 to 8: 15
 *	dig_P5    |  0x96 and 0x97   | from 0 : 7 to 8: 15
 *	dig_P6    |  0x98 and 0x99   | from 0 : 7 to 8: 15
 *	dig_P7    |  0x9A and 0x9B   | from 0 : 7 to 8: 15
 *	dig_P8    |  0x9C and 0x9D   | from 0 : 7 to 8: 15
 *	dig_P9    |  0x9E and 0x9F   | from 0 : 7 to 8: 15
 *	dig_H1    |         0xA1     | from 0 to 7
 *	dig_H2    |  0xE1 and 0xE2   | from 0 : 7 to 8: 15
 *	dig_H3    |         0xE3     | from 0 to 7
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/

INT8U bme280_get_calib_param(void)
{
	/* used to return the communication result*/
	INT8U a_data_u8[BME280_CALIB_DATA_SIZE];
	/* check the p_bme280 structure pointer as NULL*/
	a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T1_LSB]=bme280_read_register(BME280_TEMPERATURE_CALIB_DIG_T1_LSB_REG);
	a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T1_MSB]=bme280_read_register(BME280_TEMPERATURE_CALIB_DIG_T1_MSB_REG);
	a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T2_LSB]=bme280_read_register(BME280_TEMPERATURE_CALIB_DIG_T2_LSB_REG);
	a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T2_MSB]=bme280_read_register(BME280_TEMPERATURE_CALIB_DIG_T2_MSB_REG);
	a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T3_LSB]=bme280_read_register(BME280_TEMPERATURE_CALIB_DIG_T3_LSB_REG);
	a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T3_MSB]=bme280_read_register(BME280_TEMPERATURE_CALIB_DIG_T3_MSB_REG);

	p_bme280->cal_param.dig_T1 = (INT16U)((((INT16U)((INT8U)a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T1_MSB])) <<BME280_SHIFT_BIT_POSITION_BY_08_BITS)| a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T1_LSB]);
	p_bme280->cal_param.dig_T2 = (INT16S)((((INT16S)((INT8S)a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T2_MSB])) <<BME280_SHIFT_BIT_POSITION_BY_08_BITS)| a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T2_LSB]);
	p_bme280->cal_param.dig_T3 = (INT16S)((((INT16S)((INT8S)a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T3_MSB])) <<BME280_SHIFT_BIT_POSITION_BY_08_BITS)| a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T3_LSB]);
// 	p_bme280->cal_param.dig_P1 = (u16)((((u16)((u8)a_data_u8[BME280_PRESSURE_CALIB_DIG_P1_MSB])) <<BME280_SHIFT_BIT_POSITION_BY_08_BITS)| a_data_u8[BME280_PRESSURE_CALIB_DIG_P1_LSB]);
// 	p_bme280->cal_param.dig_P2 = (s16)((((s16)((s8)a_data_u8[BME280_PRESSURE_CALIB_DIG_P2_MSB])) <<BME280_SHIFT_BIT_POSITION_BY_08_BITS)| a_data_u8[BME280_PRESSURE_CALIB_DIG_P2_LSB]);
// 	p_bme280->cal_param.dig_P3 = (s16)((((s16)((s8)a_data_u8[BME280_PRESSURE_CALIB_DIG_P3_MSB])) <<BME280_SHIFT_BIT_POSITION_BY_08_BITS)| a_data_u8[BME280_PRESSURE_CALIB_DIG_P3_LSB]);
// 	p_bme280->cal_param.dig_P4 = (s16)((((s16)((s8)a_data_u8[BME280_PRESSURE_CALIB_DIG_P4_MSB])) <<BME280_SHIFT_BIT_POSITION_BY_08_BITS)| a_data_u8[BME280_PRESSURE_CALIB_DIG_P4_LSB]);
// 	p_bme280->cal_param.dig_P5 = (s16)((((s16)((s8)a_data_u8[BME280_PRESSURE_CALIB_DIG_P5_MSB])) <<BME280_SHIFT_BIT_POSITION_BY_08_BITS)| a_data_u8[BME280_PRESSURE_CALIB_DIG_P5_LSB]);
// 	p_bme280->cal_param.dig_P6 = (s16)((((s16)((s8)a_data_u8[BME280_PRESSURE_CALIB_DIG_P6_MSB])) <<BME280_SHIFT_BIT_POSITION_BY_08_BITS)| a_data_u8[BME280_PRESSURE_CALIB_DIG_P6_LSB]);
// 	p_bme280->cal_param.dig_P7 = (s16)((((s16)((s8)a_data_u8[BME280_PRESSURE_CALIB_DIG_P7_MSB])) <<BME280_SHIFT_BIT_POSITION_BY_08_BITS)| a_data_u8[BME280_PRESSURE_CALIB_DIG_P7_LSB]);
// 	p_bme280->cal_param.dig_P8 = (s16)((((s16)((s8)a_data_u8[BME280_PRESSURE_CALIB_DIG_P8_MSB])) <<BME280_SHIFT_BIT_POSITION_BY_08_BITS)| a_data_u8[BME280_PRESSURE_CALIB_DIG_P8_LSB]);
// 	p_bme280->cal_param.dig_P9 = (s16)((((s16)((s8)a_data_u8[BME280_PRESSURE_CALIB_DIG_P9_MSB])) <<BME280_SHIFT_BIT_POSITION_BY_08_BITS)| a_data_u8[BME280_PRESSURE_CALIB_DIG_P9_LSB]);
// 	p_bme280->cal_param.dig_H1 =a_data_u8[BME280_HUMIDITY_CALIB_DIG_H1];
//
// 	com_rslt += p_bme280->BME280_BUS_READ_FUNC(p_bme280->dev_addr,BME280_HUMIDITY_CALIB_DIG_H2_LSB_REG, a_data_u8,BME280_HUMIDITY_CALIB_DATA_LENGTH);
// 	p_bme280->cal_param.dig_H2 = (s16)((((s16)((s8)a_data_u8[BME280_HUMIDITY_CALIB_DIG_H2_MSB])) <<BME280_SHIFT_BIT_POSITION_BY_08_BITS)| a_data_u8[BME280_HUMIDITY_CALIB_DIG_H2_LSB]);
// 	p_bme280->cal_param.dig_H3 =a_data_u8[BME280_HUMIDITY_CALIB_DIG_H3];
// 	p_bme280->cal_param.dig_H4 = (s16)((((s16)((s8)a_data_u8[BME280_HUMIDITY_CALIB_DIG_H4_MSB])) <<BME280_SHIFT_BIT_POSITION_BY_04_BITS) |(((u8)BME280_MASK_DIG_H4) &a_data_u8[BME280_HUMIDITY_CALIB_DIG_H4_LSB]));
// 	p_bme280->cal_param.dig_H5 = (s16)((((s16)((s8)a_data_u8[BME280_HUMIDITY_CALIB_DIG_H5_MSB])) <<BME280_SHIFT_BIT_POSITION_BY_04_BITS) |(a_data_u8[BME280_HUMIDITY_CALIB_DIG_H4_LSB] >>BME280_SHIFT_BIT_POSITION_BY_04_BITS));
// 	p_bme280->cal_param.dig_H6 =(s8)a_data_u8[BME280_HUMIDITY_CALIB_DIG_H6];

	return 1;
}
/**< pointer to BME280 */
/*!
 * @brief Used to reset the sensor
 * The value 0xB6 is written to the 0xE0
 * register the device is reset using the
 * complete power-on-reset procedure.
 * @note Soft reset can be easily set using bme280_set_softreset().
 * @note Usage Hint : bme280_set_softreset()
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
void bme280_set_soft_rst(void)
{
	/* used to return the communication result*/
	INT8U v_data_u8 = BME280_SOFT_RESET_CODE;
	/* check the p_bme280 structure pointer as NULL*/
	bme280_write_register(BME280_RST_REG,v_data_u8);
}


/*!
 * @brief Used to set_config rate  filter and interface
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/

INT8U bme280_set_config(INT8U v_data_u8)
{
	/* used to return the communication result*/
	INT8U com_rslt = 0;
	/* check the p_bme280 structure pointer as NULL*/
	com_rslt = bme280_write_register(BME280_CONFIG_REG,v_data_u8);

	return com_rslt;
}

 /*!
 *	@brief This API used to get the
 *	Operational Mode from the sensor in the register 0xF4 bit 0 and 1
 *
 *
 *
 *	@param v_power_mode_u8 : The value of power mode
 *  value           |    mode
 * -----------------|------------------
 *	0x00            | BME280_SLEEP_MODE
 *	0x01 and 0x02   | BME280_FORCED_MODE
 *	0x03            | BME280_NORMAL_MODE
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
INT8U bme280_get_power_mode(INT8U v_power_u8)
{
	/* used to return the communication result*/
	INT8U com_rslt = 0;
	/* check the p_bme280 structure pointer as NULL*/
  bme280_read_register(BME280_CTRL_MEAS_REG);
	return com_rslt;
}

INT8U bme280_set_power_mode(INT8U  v_power_mode_u8)
{
	/* used to return the communication result*/
	INT8U com_rslt = 0;

	bme280_read_register(BME280_CTRL_MEAS_REG);
	//v_data_u8=(v_power_mode_u8&0x03)|(v_value_u8&0xFC);
	/* check the p_bme280 structure pointer as NULL*/
	com_rslt = bme280_write_register(BME280_CTRL_MEAS_REG,v_power_mode_u8);
	return com_rslt;
}

/*
 * @brief Writes the working mode to the sensor
 *
 *
 *
 *
 *  @param v_work_mode_u8 : Mode to be set
 *  value    | Working mode
 * ----------|--------------------
 *   0       | BME280_ULTRALOWPOWER_MODE
 *   1       | BME280_LOWPOWER_MODE
 *   2       | BME280_STANDARDRESOLUTION_MODE
 *   3       | BME280_HIGHRESOLUTION_MODE
 *   4       | BME280_ULTRAHIGHRESOLUTION_MODE
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/

INT8U bme280_set_work_mode(INT8U v_work_mode_u8)
{
	INT8U com_rslt = 0;
	INT8U v_data_u8 = 0;
	INT8U v_power_u8 = 0;
	bme280_get_power_mode(v_power_u8);
	if(v_power_u8==BME280_SLEEP_MODE)
	{
		com_rslt = 0;
	}
	else
	{
		if (v_work_mode_u8 <= BME280_ULTRAHIGHRESOLUTION_MODE) {

			switch (v_work_mode_u8) {
			case BME280_ULTRALOWPOWER_MODE:
				p_bme280->oversamp_temperature =BME280_ULTRALOWPOWER_OSRS_T;
				p_bme280->osrs_p =BME280_ULTRALOWPOWER_OSRS_P;
				break;
			case BME280_LOWPOWER_MODE:
				p_bme280->oversamp_temperature =BME280_LOWPOWER_OSRS_T;
				p_bme280->osrs_p = BME280_LOWPOWER_OSRS_P;
				break;
			case BME280_STANDARDRESOLUTION_MODE:
				p_bme280->oversamp_temperature =BME280_STANDARDRESOLUTION_OSRS_T;
				p_bme280->osrs_p =BME280_STANDARDRESOLUTION_OSRS_P;
				break;
			case BME280_HIGHRESOLUTION_MODE:
				p_bme280->oversamp_temperature =BME280_HIGHRESOLUTION_OSRS_T;
				p_bme280->osrs_p = BME280_HIGHRESOLUTION_OSRS_P;
				break;
			case BME280_ULTRAHIGHRESOLUTION_MODE:
				p_bme280->oversamp_temperature = BME280_ULTRAHIGHRESOLUTION_OSRS_T;
				p_bme280->osrs_p = BME280_ULTRAHIGHRESOLUTION_OSRS_P;
				break;
			}

			v_data_u8=p_bme280->oversamp_temperature|p_bme280->osrs_p;
			com_rslt = bme280_write_register(BME280_CTRL_MEAS_REG,v_data_u8);
		} else {
			com_rslt = 2;
		}
	}
	return com_rslt;
}

INT8U bme280_write_register(INT8U v_addr_u8,INT8U v_data_u8)
{
	/* used to return the communication result*/

	INT8U data[2];
    data[0] = v_addr_u8;
    data[1] = v_data_u8;
	i2c_write(I2C_PORT_0, DEV_ADDR, data, 2);
 
    return 1;
}

INT8U bme280_read_register(INT8U v_addr_u8)
{
    INT8U data;
	i2c_read(I2C_PORT_0, DEV_ADDR, &v_addr_u8, 1, &data, 1);
    return data;
}


/*!
 *	@brief This API is used to read uncompensated temperature
 *	in the registers 0xFA, 0xFB and 0xFC
 *	@note 0xFA -> MSB -> bit from 0 to 7
 *	@note 0xFB -> LSB -> bit from 0 to 7
 *	@note 0xFC -> LSB -> bit from 4 to 7
 *
 * @param v_uncomp_temperature_s32 : The value of uncompensated temperature
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
INT32S bme280_read_uncomp_temperature(void)
{
	/* used to return the communication result*/

	INT32S  v_uncomp_temperature_s32;
	/* Array holding the MSB and LSb value
	a_data_u8r[0] - Temperature MSB
	a_data_u8r[1] - Temperature LSB
	a_data_u8r[2] - Temperature XLSB
	*/
	INT8U a_data_u8r[BME280_TEMPERATURE_DATA_SIZE] = {BME280_INIT_VALUE, BME280_INIT_VALUE, BME280_INIT_VALUE};
	/* check the p_bme280 structure pointer as NULL*/

	a_data_u8r[0]=bme280_read_register(BME280_TEMPERATURE_MSB_REG);

	a_data_u8r[1]=bme280_read_register(BME280_TEMPERATURE_LSB_REG);
	a_data_u8r[2]=bme280_read_register(BME280_TEMPERATURE_XLSB_REG);

	v_uncomp_temperature_s32 = (INT32S)((((INT32S) (a_data_u8r[BME280_TEMPERATURE_MSB_DATA]))<< BME280_SHIFT_BIT_POSITION_BY_12_BITS) |(((INT32U)(a_data_u8r[BME280_TEMPERATURE_LSB_DATA]))<< BME280_SHIFT_BIT_POSITION_BY_04_BITS)|
															((INT32U)a_data_u8r[BME280_TEMPERATURE_XLSB_DATA] >>BME280_SHIFT_BIT_POSITION_BY_04_BITS));

	return v_uncomp_temperature_s32;
}

/*!
 * @brief Reads actual temperature from uncompensated temperature
 * @note Returns the value in 0.01 degree Centigrade
 * Output value of "5123" equals 51.23 DegC.
 *
 *
 *
 *  @param  v_uncomp_temperature_s32 : value of uncompensated temperature
 *
 *
 *  @return Returns the actual temperature
 *
*/

INT32S bme280_compensate_temperature_int32(INT32S v_uncomp_temperature_s32)
{
	INT32S v_x1_u32r = 0;
	INT32S v_x2_u32r = 0;
	INT32S temperature = 0;
	/* used to return the communication result*/
	INT8U a_data_u8[BME280_CALIB_DATA_SIZE];
	/* check the p_bme280 structure pointer as NULL*/
	a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T1_LSB]=bme280_read_register(BME280_TEMPERATURE_CALIB_DIG_T1_LSB_REG);
	a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T1_MSB]=bme280_read_register(BME280_TEMPERATURE_CALIB_DIG_T1_MSB_REG);
	a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T2_LSB]=bme280_read_register(BME280_TEMPERATURE_CALIB_DIG_T2_LSB_REG);
	a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T2_MSB]=bme280_read_register(BME280_TEMPERATURE_CALIB_DIG_T2_MSB_REG);
	a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T3_LSB]=bme280_read_register(BME280_TEMPERATURE_CALIB_DIG_T3_LSB_REG);
	a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T3_MSB]=bme280_read_register(BME280_TEMPERATURE_CALIB_DIG_T3_MSB_REG);

	p_bme280->cal_param.dig_T1 = (INT16U)((((INT16U)((INT8U)a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T1_MSB])) <<BME280_SHIFT_BIT_POSITION_BY_08_BITS)| a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T1_LSB]);
	p_bme280->cal_param.dig_T2 = (INT16S)((((INT16S)((INT8S)a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T2_MSB])) <<BME280_SHIFT_BIT_POSITION_BY_08_BITS)| a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T2_LSB]);

	/* calculate x1*/
	v_x1_u32r  =((((v_uncomp_temperature_s32>> BME280_SHIFT_BIT_POSITION_BY_03_BITS) -((INT32S)p_bme280->cal_param.dig_T1<< BME280_SHIFT_BIT_POSITION_BY_01_BIT))) *((INT32S)p_bme280->cal_param.dig_T2)) >>BME280_SHIFT_BIT_POSITION_BY_11_BITS;


	//更新T2 然后虚拟T3
	p_bme280->cal_param.dig_T2 = (INT16S)((((INT16S)((INT8S)a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T3_MSB])) <<BME280_SHIFT_BIT_POSITION_BY_08_BITS)| a_data_u8[BME280_TEMPERATURE_CALIB_DIG_T3_LSB]);
	/* calculate x2*/
	v_x2_u32r  = (((((v_uncomp_temperature_s32>> BME280_SHIFT_BIT_POSITION_BY_04_BITS)- ((INT32S)p_bme280->cal_param.dig_T1))* ((v_uncomp_temperature_s32 >> BME280_SHIFT_BIT_POSITION_BY_04_BITS)-
								((INT32S)p_bme280->cal_param.dig_T1)))>> BME280_SHIFT_BIT_POSITION_BY_12_BITS) *((INT32S)p_bme280->cal_param.dig_T2))>> BME280_SHIFT_BIT_POSITION_BY_14_BITS;
	/* calculate t_fine*/
	p_bme280->cal_param.t_fine = v_x1_u32r + v_x2_u32r;
	/* calculate temperature*/
	temperature  = (p_bme280->cal_param.t_fine * 5 + 128)>> BME280_SHIFT_BIT_POSITION_BY_08_BITS;
	return temperature-1800;
}
/*!
 * @brief Reads actual temperature from uncompensated temperature
 * @note Returns the value with 500LSB/DegC centred around 24 DegC
 * output value of "5123" equals(5123/500)+24 = 34.246DegC
 *
 *
 * @param v_uncomp_temperature_s32: value of uncompensated temperature
 *
 * @return Return the actual temperature as s16 output
 *
*/
INT16S bme280_compensate_temperature_int32_sixteen_bit_output(INT32S v_uncomp_temperature_s32)
{
	INT16S temperature = 0;

	bme280_compensate_temperature_int32(v_uncomp_temperature_s32);
	temperature  = (INT16S)((((p_bme280->cal_param.t_fine - 122880) * 25) + 128)>> BME280_SHIFT_BIT_POSITION_BY_08_BITS);
	temperature  = temperature/500;
	return temperature;
}

uint8_t Chip_id_read(void)
{
    uint8_t reg_addr = 0xd0;
    uint8_t id;
	i2c_read(I2C_PORT_0, DEV_ADDR, &reg_addr, 1, &id, 1);
    
	if (id == 0x60)
        return 1;
	else
        return 0;
}

/*!
 *	@brief This function is used for initialize
 *	the bus read and bus write functions
 *  and assign the chip id and I2C address of the BME280 sensor
 *	chip id is read in the register 0xD0 bit from 0 to 7
 *
 *	@param bme280 structure pointer.
 *
 *	@note While changing the parameter of the bme280_t
 *	@note consider the following point:
 *	Changing the reference value of the parameter
 *	will changes the local copy or local reference
 *	make sure your changes will not
 *	affect the reference value of the parameter
 *	(Better case don't change the reference value of the parameter)

 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
INT8U bme280_init(struct bme280_t *data)
{
	/* used to return the communication result*/
    p_bme280 = data;
	bme280_set_soft_rst();
	if(Chip_id_read()==0) return 0;
	bme280_set_config(0);	// in Normal mode standby 0.5ms/filter off/iic bus
	//bme280_set_power_mode(0x03);
	//bme280_set_work_mode();
	bme280_write_register(BME280_CTRL_MEAS_REG,0x83); // pressure skipped /temperarure oversampling*8 /Normal mode
	return 1;
}

INT32S bme280_compensate_temperature_read(void)
{
		INT32S v_temperature_s32;
 		v_temperature_s32=bme280_read_uncomp_temperature();
		//bme280_temperature=bme280_compensate_temperature_int32(v_temperature_s32);
		return bme280_compensate_temperature_int32(v_temperature_s32);
}


