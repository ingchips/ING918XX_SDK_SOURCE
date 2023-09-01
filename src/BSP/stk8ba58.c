#include "stk8ba58.h"

static struct stk8ba58_t *p_stk8ba58;

STK8BA58_RETURN_FUNCTION_TYPE stk8ba58_write_reg(uint8_t addr, uint8_t *data, uint8_t len)
{
	STK8BA58_RETURN_FUNCTION_TYPE com_rslt = ERROR;

	if(p_stk8ba58 == STK8BA58_NULL)
	{
		/* Check the struct p_STK8BA58 is empty */
		return E_STK8BA58_NULL_PTR;
	}
	else
	{
		/* Write the data to the register*/
		com_rslt = p_stk8ba58->STK8BA58_BUS_WRITE_FUNC
		(p_stk8ba58->dev_addr, addr, data, len);
	}
	return com_rslt;
}

STK8BA58_RETURN_FUNCTION_TYPE stk8ba58_read_reg(uint8_t addr, uint8_t *data, uint8_t len)
{
	STK8BA58_RETURN_FUNCTION_TYPE com_rslt = ERROR;

	if(p_stk8ba58 == STK8BA58_NULL)
	{
		/* Check the struct p_STK8BA58 is empty */
		return E_STK8BA58_NULL_PTR;
	}
	else
	{
		/*Read the data from the register*/
		com_rslt = p_stk8ba58->STK8BA58_BUS_READ_FUNC(p_stk8ba58->dev_addr, addr, data, len);
	}
	return com_rslt;
}

STK8BA58_RETURN_FUNCTION_TYPE stk8ba58_init(struct stk8ba58_t *stk8ba58)
{
	STK8BA58_RETURN_FUNCTION_TYPE com_rslt = ERROR; 
	uint8_t data = 0;
	uint8_t config_data = 0;
	/* assign stk8ba58 ptr */
	p_stk8ba58 = stk8ba58;
	if(p_stk8ba58 == STK8BA58_NULL)
	{
		/* Check the struct p_stk8ba58 is empty */
		com_rslt = E_STK8BA58_NULL_PTR;
	} 
	else
	{
 		/* read Chip Id */ 
    	com_rslt = p_stk8ba58->STK8BA58_BUS_READ_FUNC(p_stk8ba58->dev_addr,STK8BA58_CHIP_ID_ADDR,&data, 1);
    	p_stk8ba58->chip_id = data;
	}

	return com_rslt;
}

STK8BA58_RETURN_FUNCTION_TYPE stk8ba58_read_accel_xyz(struct stk8ba58_accel_data *accel)
{
	STK8BA58_RETURN_FUNCTION_TYPE com_rslt = ERROR;
	/* Array holding the accel x value
							    | 7 | 6 | 5 | 4 | 3 | 2 | 1 |   0   |
		                        --------------------------------------						
		data[0] - x->LSB		|   XOUT[3:0]   |  reverse  | NEW_X |
		data[1] - x->MSB		|		      XOUT[11:4]		    |
		data[2] - y->LSB		|   YOUT[3:0]   |  reverse  | NEW_Y |
		data[3] - y->MSB		|		      YOUT[11:4]		    |
		data[4] - z->LSB		|   ZOUT[3:0]   |  reverse  | NEW_Z |
		data[5] - z->MSB		|		      ZOUT[11:4]		    |

	*/
	uint8_t data[6] = {0};

	if (p_stk8ba58 == STK8BA58_NULL)
	{
		/* Check the struct p_STK8BA58 is empty */
		return E_STK8BA58_NULL_PTR;
	} 
	else
	{
		/* read the x & y & z data[6]*/
		com_rslt = p_stk8ba58->STK8BA58_BUS_READ_FUNC(p_stk8ba58->dev_addr, STK8BA58_X_OUT_LSB_ADDR,data, STK8BA58_SHIFT_SIX_BITS);
		/* read the x data*/
		accel->x = (int16_t)((((int32_t)((int8_t)data[STK8BA58_SENSOR_DATA_XYZ_X_MSB])) << STK8BA58_SHIFT_EIGHT_BITS) |
												(data[STK8BA58_SENSOR_DATA_XYZ_X_LSB] & STK8BA58_RESOLUTION_12_MASK));
		accel->x = accel->x >> STK8BA58_SHIFT_FOUR_BITS;
		/* read the y data*/
		accel->y = (int16_t)((((int32_t)((int8_t)data[STK8BA58_SENSOR_DATA_XYZ_Y_MSB])) << STK8BA58_SHIFT_EIGHT_BITS) |
												(data[STK8BA58_SENSOR_DATA_XYZ_Y_LSB] & STK8BA58_RESOLUTION_12_MASK));
		accel->y = accel->y >> STK8BA58_SHIFT_FOUR_BITS;
		/* read the z data*/
		accel->z = (int16_t)((((int32_t)((int8_t)data[STK8BA58_SENSOR_DATA_XYZ_Z_MSB])) << STK8BA58_SHIFT_EIGHT_BITS) |
												(data[STK8BA58_SENSOR_DATA_XYZ_Z_LSB] & STK8BA58_RESOLUTION_12_MASK));
		accel->z = accel->z >> STK8BA58_SHIFT_FOUR_BITS;
	}	
	return com_rslt;
}

STK8BA58_RETURN_FUNCTION_TYPE stk8ba58_get_range(uint8_t *range)
{
	STK8BA58_RETURN_FUNCTION_TYPE com_rslt = ERROR;
	uint8_t data = 0;

	if (p_stk8ba58 == STK8BA58_NULL)
	{
		/* Check the struct p_STK8BA58 is empty */
		return E_STK8BA58_NULL_PTR;
	} 
	else 
	{
		/* Read the range register 0x0F*/
		com_rslt = p_stk8ba58->STK8BA58_BUS_READ_FUNC(p_stk8ba58->dev_addr, STK8BA58_RANGE_SELECT_ADDR, &data, STK8BA58_GEN_READ_WRITE_LENGTH);
		data = STK8BA58_GET_BITSLICE(data, STK8BA58_RANGE_SELECT);
		*range = data;
	}
	return com_rslt;
}

STK8BA58_RETURN_FUNCTION_TYPE stk8ba58_set_range(uint8_t range)
{
	STK8BA58_RETURN_FUNCTION_TYPE com_rslt = ERROR;
	uint8_t data = 0;

	if (p_stk8ba58 == STK8BA58_NULL)
	{
		/* Check the struct p_STK8BA58 is empty */
		return E_STK8BA58_NULL_PTR;
	} 
	else
	{
		if ((range == STK8BA58_RANGE_2G) || (range == STK8BA58_RANGE_4G) || (range == STK8BA58_RANGE_8G))
		{
			com_rslt = p_stk8ba58->STK8BA58_BUS_READ_FUNC(p_stk8ba58->dev_addr, STK8BA58_RANGE_SELECT_ADDR, &data, STK8BA58_GEN_READ_WRITE_LENGTH);
			switch (range)
			{
				case STK8BA58_RANGE_2G:
					data  = STK8BA58_SET_BITSLICE(data,STK8BA58_RANGE_SELECT,STK8BA58_RANGE_2G);
				break;
				case STK8BA58_RANGE_4G:
					data  = STK8BA58_SET_BITSLICE(data,STK8BA58_RANGE_SELECT,STK8BA58_RANGE_4G);
				break;
				case STK8BA58_RANGE_8G:
					data  = STK8BA58_SET_BITSLICE(data,STK8BA58_RANGE_SELECT,STK8BA58_RANGE_8G);
				break;
				default:
				break;
			}
			/* Write the range register 0x0F*/
			com_rslt += stk8ba58_write_reg(STK8BA58_RANGE_SELECT_ADDR, &data, STK8BA58_GEN_READ_WRITE_LENGTH);
		} 
		else 
		{
			com_rslt = E_OUT_OF_RANGE;
		}
	}
	return com_rslt;
}

STK8BA58_RETURN_FUNCTION_TYPE stk8ba58_get_bw(uint8_t *bw)
{
	/*  Variable used to return value of
	communication routine*/
	STK8BA58_RETURN_FUNCTION_TYPE com_rslt = ERROR;
	uint8_t data = 0;

	if (p_stk8ba58 == STK8BA58_NULL)
	{
		/* Check the struct p_stk8ba58 is empty */
		return E_STK8BA58_NULL_PTR;
	}
	else
	{
		/* Read the bandwidth register 0x10*/
		com_rslt = p_stk8ba58->STK8BA58_BUS_READ_FUNC(p_stk8ba58->dev_addr,STK8BA58_BWSEL_ADDR, &data,STK8BA58_GEN_READ_WRITE_LENGTH);
		data = STK8BA58_GET_BITSLICE(data, STK8BA58_BWSEL);
		*bw = data;
	}
	return com_rslt;	
}

STK8BA58_RETURN_FUNCTION_TYPE stk8ba58_set_bw(uint8_t bw)
{
	/*  Variable used to return value of
	communication routine*/
	STK8BA58_RETURN_FUNCTION_TYPE com_rslt = ERROR;
	uint8_t data = 0;
	uint8_t data_bw = 0;
	if (p_stk8ba58 == STK8BA58_NULL)
	{
		/* Check the struct p_stk8ba58 is empty */
		com_rslt = E_STK8BA58_NULL_PTR;
	} 
	else 
	{
		/* Check the chip id 0xFB, it support upto 500Hz*/
		if (p_stk8ba58->chip_id == 0xFB) 
		{
			if (bw > 7 && bw < 15)
			{
				switch (bw) {
				case STK8BA58_BW_7_81HZ:
					data_bw = STK8BA58_BW_7_81HZ;
				break;
				case STK8BA58_BW_15_63HZ:
					data_bw = STK8BA58_BW_15_63HZ;
				break;
				case STK8BA58_BW_31_25HZ:
					data_bw = STK8BA58_BW_31_25HZ;
					break;
				case STK8BA58_BW_62_50HZ:
					data_bw = STK8BA58_BW_62_50HZ;
				break;
				case STK8BA58_BW_125HZ:
					data_bw = STK8BA58_BW_125HZ;
				break;
				case STK8BA58_BW_250HZ:
					data_bw = STK8BA58_BW_250HZ;
				break;
				case STK8BA58_BW_500HZ:
					data_bw = STK8BA58_BW_500HZ;
				break;
				default:
				break;
				}
				/* Write the bandwidth register */
				com_rslt = p_stk8ba58->STK8BA58_BUS_READ_FUNC(p_stk8ba58->dev_addr, STK8BA58_BWSEL_ADDR, &data, STK8BA58_GEN_READ_WRITE_LENGTH);
				data = STK8BA58_SET_BITSLICE(data, STK8BA58_BWSEL, data_bw);
				com_rslt += stk8ba58_write_reg(STK8BA58_BWSEL_ADDR, &data, STK8BA58_GEN_READ_WRITE_LENGTH);
			} 
			else 
			{
				com_rslt = E_OUT_OF_RANGE;
			}
		} 
		else 
		{
			if (bw > 7 && bw < 16) 
			{
				switch (bw) 
				{
					case STK8BA58_BW_7_81HZ:
						data_bw = STK8BA58_BW_7_81HZ;
					break;
					case STK8BA58_BW_15_63HZ:
						data_bw = STK8BA58_BW_15_63HZ;
					break;
					case STK8BA58_BW_31_25HZ:
						data_bw = STK8BA58_BW_31_25HZ;
					break;
					case STK8BA58_BW_62_50HZ:
						data_bw = STK8BA58_BW_62_50HZ;
					break;
					case STK8BA58_BW_125HZ:
						data_bw = STK8BA58_BW_125HZ;
					break;
					case STK8BA58_BW_250HZ:
						data_bw = STK8BA58_BW_250HZ;
					break;
					case STK8BA58_BW_500HZ:
						data_bw = STK8BA58_BW_500HZ;
					break;
					case STK8BA58_BW_1000HZ:
						data_bw = STK8BA58_BW_1000HZ;
					break;
					default:
					break;
				}
				/* Write the bandwidth register */
				com_rslt = p_stk8ba58->STK8BA58_BUS_READ_FUNC(p_stk8ba58->dev_addr, STK8BA58_BWSEL_ADDR, &data, STK8BA58_GEN_READ_WRITE_LENGTH);
				data = STK8BA58_SET_BITSLICE(data, STK8BA58_BWSEL, data_bw);
				com_rslt += stk8ba58_write_reg(STK8BA58_BWSEL_ADDR, &data, STK8BA58_GEN_READ_WRITE_LENGTH);
			} 
			else 
			{
				com_rslt = E_OUT_OF_RANGE;
			}
		}
	}
	return com_rslt;
}
