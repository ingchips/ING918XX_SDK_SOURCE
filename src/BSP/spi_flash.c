 
#include "spi_flash.h"

/* to read the BLE device security info in flash added by dengyiyun*/
/* once power on , the data should read from flash and stored in this */
/* and the flash driver should be responsible of the work to write back this block into flash before power off*/
T_GROUP0_MIRROR_NVM_HOST  host_group0; 
// TODO: need to read the data from flash into this mirror memory once device power on.
  
//static flash_memory_group_info_t mem_group_info_tbl[]=
//{
//	{DEF_FLASH_HOST_ADDR,512,&host_group0}
//};

/********************************************/
//
//
//
//********************************************/
void spi_flash_init(void)
{
	apSSP_sDeviceControlBlock* pParam; 
	pParam = (apSSP_sDeviceControlBlock *)(SRAM_BASE + 0x600);
	
  apSSP_Initialize(AHB_SSP0);
 
	/* Set Device Parameters */
  pParam->ClockRate        = 0;  // sspclkout = sspclk/(ClockPrescale*(ClockRate+1))
  pParam->ClockPrescale    = 2;  // Must be an even number from 2 to 254
  pParam->eSCLKPhase       = apSSP_SCLKPHASE_TRAILINGEDGE;
  pParam->eSCLKPolarity    = apSSP_SCLKPOLARITY_IDLELOW;
  pParam->eFrameFormat     = apSSP_FRAMEFORMAT_MOTOROLASPI;
  pParam->eDataSize        = apSSP_DATASIZE_8BITS;
  pParam->eLoopBackMode    = apSSP_LOOPBACKOFF;
  pParam->eMasterSlaveMode = apSSP_MASTER;
  pParam->eSlaveOutput     = apSSP_SLAVEOUTPUTDISABLED;
  apSSP_DeviceParametersSet(AHB_SSP0, pParam); 
 
	gio_set_bit(GIO_L32_OEB, SPI_FLASH_CS);  // set CS output
	gio_set_bit(GIO_L32_OEB, SPI_FLASH_WP);  // set WP output
	gio_set_bit(GIO_L32_OEB, SPI_FLASH_RST); // set RST output
 
	FLASH_Enable; 
 
}
/********************************************/
//
//
//
//********************************************/
static uint8_t FLASH_BUSY(void)
{
	uint8_t i=0;
	uint16_t busy_bit;
	FLASH_Enable;
	spi_flash_send_byte(RdStatusRegCMD);
	busy_bit=spi_flash_rece_byte();
	while ((busy_bit& 0x01))
	{
		i++;
		io_sleep(); 
		if(i==255) break;
		busy_bit=spi_flash_rece_byte();
	}
	FLASH_Disable;
	if(i==255) return 0;
	else return 1;
}
static uint8_t FLASH_WEL(void)
{
	uint8_t i=0;
	uint16_t busy_bit;
	FLASH_Enable;
	spi_flash_send_byte(RdStatusRegCMD);
	busy_bit=spi_flash_rece_byte();
	while (!(busy_bit& 0x02))
	{
		i++;
		io_sleep(); 
		if(i==255) break;
		busy_bit=spi_flash_rece_byte();
	}
	FLASH_Disable;
		if(i==255) return 0;
	else return 1;
}
/********************************************/
//
//
//
//********************************************/
static void Write_Enable(void)
{

	FLASH_Enable;
	spi_flash_send_byte(WriteEnableCMD); 
	FLASH_Disable;
}

/********************************************/
//
//
//
//********************************************/
uint8_t display_flash_id(void)
{
	uint8_t i;
	uint8_t IData[2]; 
	FLASH_Enable; 
	spi_flash_send_byte(0x90);//RdIDCMD
 	spi_flash_send_byte(0x00);//RdIDCMD
	spi_flash_send_byte(0x00);//RdIDCMD
	spi_flash_send_byte(0x00);//RdIDCMD
	for (i=0;i<2;i++)
	{
		IData[i] = spi_flash_rece_byte();
	} 
	FLASH_Disable; 
	if((IData[0]==0x14)&&(IData[1]==0xEF)) return 1;
	else return 0;
}

/********************************************/
//
//
//
//********************************************/
void display_flash_status_reg(void)
{
	uint8_t status;
	uint8_t i;
	for (i=0;i<1;i++) {
		FLASH_Enable;
		spi_flash_send_byte(RdStatusRegCMD);
		status = spi_flash_rece_byte();
	  FLASH_Disable;
		printf("DISPLAY: flash_status_reg = %x \n", status);
	}  
}
 
 
/********************************************/
//
// Page Write (0x02)
//
//********************************************/
uint8_t PageWrite(uint32_t addr, uint8_t* databuff)
{
	uint32_t i;
	//SectorErase(addr);
// 	FLASH_BUSY(); 
	Write_Enable();
	if(FLASH_WEL()==0) return 0; 
	FLASH_Enable;
	spi_flash_send_byte(PageWriteCMD);
	spi_flash_send_byte(W8_2(addr));
	spi_flash_send_byte(W8_1(addr));
	//spi_flash_send_byte(W8_0(addr));
	spi_flash_send_byte(0x00);
	for (i=0;i<256;i++)
	{  
		spi_flash_send_byte(databuff[i]);
	} 
 	FLASH_Disable; 
	if(FLASH_BUSY()==0) return 0; 
	else return 1;
}
 
/********************************************/
//
// bytes Write (0x02)
//
//********************************************/
uint8_t ByteWrite(uint32_t addr, uint8_t* databuff,uint8_t writesize)
{
	uint32_t i; 
	FLASH_BUSY(); 
	Write_Enable();
	if(FLASH_WEL()==0) return 0; 
	FLASH_Enable;
	spi_flash_send_byte(PageWriteCMD);
	spi_flash_send_byte(W8_2(addr));
	spi_flash_send_byte(W8_1(addr));
	spi_flash_send_byte(W8_0(addr));
 
	for (i=0;i<writesize;i++)
	{  
		spi_flash_send_byte(databuff[i]);
	} 
 	FLASH_Disable; 
	if(FLASH_BUSY()==0) 
		return 0; 
	else 
		return 1;
} 
/********************************************/
//
// Byte Read (0x02)
//
//********************************************/
void ByteRead(uint32_t addr, uint8_t* databuff , uint32_t  length)
{
	uint32_t i;
	FLASH_BUSY();
	FLASH_Enable; 
	spi_flash_send_byte(ByteReadCMD);
	spi_flash_send_byte(W8_2(addr)); 
	spi_flash_send_byte(W8_1(addr));
	spi_flash_send_byte(W8_0(addr));
	//spi_flash_rece_byte();
	for (i=0;i<length;i++)
	{
	 databuff[i]= spi_flash_rece_byte();
	}
	FLASH_Disable;
}
/********************************************/
//
// fast Read (0x0B)
//
//********************************************/
void FastRead(uint32_t addr, uint8_t* databuff)
{
 	uint8_t i;
	FLASH_BUSY();
	FLASH_Enable; 
	spi_flash_send_byte(ByteReadCMD);
	spi_flash_send_byte(W8_2(addr));
	spi_flash_send_byte(W8_1(addr));
	spi_flash_send_byte(W8_0(addr)); 
	spi_flash_rece_byte();
 	for (i=0;i<2;i++)
	*(databuff+i)=spi_flash_rece_byte(); 
	FLASH_Disable;
}
 

 
//********************************************/
//
// Block Erase (0xd8)
//
//********************************************/
uint8_t BlockErase(uint32_t addr)
{
 
	FLASH_Enable;
	Write_Enable();
	FLASH_WEL();
	spi_flash_send_byte(BlockEraseCMD);
	spi_flash_send_byte(W8_2(addr));
	spi_flash_send_byte(W8_1(addr));
	spi_flash_send_byte(W8_0(addr));
	FLASH_Disable;
	if(FLASH_BUSY()==0) return 0; 
	else return 1;
}


//********************************************/
//
//  Sector Erase (7.6) 0x20
//
//  TODO: becareful sector 0 addr (0a and 0b)
//
//********************************************/ 
uint8_t SectorErase(uint32_t addr)
{
 
	Write_Enable();	
	if(FLASH_WEL()==0) return 0; 
	FLASH_Enable;
	spi_flash_send_byte(SectorEraseCMD);
	spi_flash_send_byte(W8_2(addr));
	spi_flash_send_byte(W8_1(addr));
	spi_flash_send_byte(W8_0(addr));
	FLASH_Disable;
	io_sleep();
	io_sleep(); 
	if(FLASH_BUSY()==0) return 0; 
	else return 1;
}
//********************************************/
//
//   Erase (7.6) 0x20
//
//  TODO: becareful sector 0 addr (0a and 0b)
//
//********************************************/ 
void PageErase(uint32_t addr)
{
	FLASH_BUSY(); 
	Write_Enable();	
	FLASH_WEL();
	FLASH_Enable;
	spi_flash_send_byte(PageEraseCMD);
	spi_flash_send_byte(W8_2(addr));
	spi_flash_send_byte(W8_1(addr));
	spi_flash_send_byte(W8_0(addr));
	FLASH_Disable;
	io_sleep(); io_sleep(); 
}
//********************************************/
//
//  Chip Erase (7.7)
//
//********************************************/ 
uint8_t ChipErase(void)
{
	uint8_t ret;
	FLASH_BUSY(); 
	Write_Enable();
	if(FLASH_WEL()==0) return 0; 
	FLASH_Enable;
	spi_flash_send_byte(ChipEraseCMD1); 
	FLASH_Disable;
	ret=FLASH_BUSY();
	return(ret);
}

 
void io_sleep(void)
{
		uint8_t t;
    for(t=255;t>0;t--) 
		{
			Delay(); 
		}
}
void Delay(void)
{
		uint32_t t;
    for(t=2255;t>0;t--) 
		{ 
			__NOP();__NOP();__NOP(); 
		}
}
 
//********************************************/
//
//
//
//********************************************/ 
void spi_flash_send_byte(uint8_t data)
{
	// send data
	apSSP_WriteFIFO(AHB_SSP0, data);
	// wait data send complete
	apSSP_DeviceEnable(AHB_SSP0);
	while(apSSP_DeviceBusyGet(AHB_SSP0));
	apSSP_DeviceDisable(AHB_SSP0);
	// clear dummy data
	apSSP_ReadFIFO(AHB_SSP0);
}

uint8_t spi_flash_rece_byte(void)
{
	uint8_t rec;
	// send dummy data
	apSSP_WriteFIFO(AHB_SSP0, 0x00);
	// wait data send complete
	apSSP_DeviceEnable(AHB_SSP0);
	while(apSSP_DeviceBusyGet(AHB_SSP0));
	apSSP_DeviceDisable(AHB_SSP0);
	rec=apSSP_ReadFIFO(AHB_SSP0);
	// read receive data
	return  rec;
}







