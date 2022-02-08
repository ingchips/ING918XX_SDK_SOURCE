#include "oled_ssd1306.h"
#include "oled_ssd1306_font.h"
#include "ingsoc.h"
#include "FreeRTOS.h"
#include "task.h"

#ifdef USE_I2C

#include "iic.h"

/**********************************************
// IIC Write Command
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
    uint8_t data[] = {0x00, IIC_Command};    
    i2c_write(I2C_PORT_0, OLED_ADDR >> 1, data, sizeof(data));
}

void Write_IIC_Data(unsigned char IIC_Data)
{
    uint8_t data[] = {0x40, IIC_Data};
    i2c_write(I2C_PORT_0, OLED_ADDR >> 1, data, sizeof(data));
}

#else

#define OLED_SCLK_Clr() GIO_WriteValue((GIO_Index_t)PIN_SCL, 0)
#define OLED_SCLK_Set() GIO_WriteValue((GIO_Index_t)PIN_SCL, 1)

#define OLED_SDIN_Clr() GIO_WriteValue((GIO_Index_t)PIN_SDA, 0)
#define OLED_SDIN_Set() GIO_WriteValue((GIO_Index_t)PIN_SDA, 1)

void Write_IIC_Byte(unsigned char IIC_Byte)
{
    unsigned char i;
    unsigned char m,da;
    da = IIC_Byte;
    OLED_SCLK_Clr();
    for(i = 0; i < 8; i++)        
    {
        m = da & 0x80;
        if (m == 0x80)
            OLED_SDIN_Set();
        else
            OLED_SDIN_Clr();
        da <<= 1;
        OLED_SCLK_Set();
        OLED_SCLK_Clr();
    }
}

void IIC_Start()
{
    OLED_SCLK_Set() ;
    OLED_SDIN_Set();
    OLED_SDIN_Clr();
    OLED_SCLK_Clr();
}

/**********************************************
//IIC Stop
**********************************************/
void IIC_Stop()
{
    OLED_SCLK_Set() ;
    OLED_SDIN_Clr();
    OLED_SDIN_Set();
}

void IIC_Wait_Ack()
{
    OLED_SCLK_Set() ;
    OLED_SCLK_Clr();
}

void Write_IIC_Command(unsigned char IIC_Command)
{
    IIC_Start();
    Write_IIC_Byte(OLED_ADDR);            //Slave address,SA0=0
    IIC_Wait_Ack();    
    Write_IIC_Byte(0x00);            //write command
    IIC_Wait_Ack();    
    Write_IIC_Byte(IIC_Command); 
    IIC_Wait_Ack();    
    IIC_Stop();
}
/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
    IIC_Start();
    Write_IIC_Byte(OLED_ADDR);            //D/C#=0; R/W#=0
    IIC_Wait_Ack();    
    Write_IIC_Byte(0x40);            //write data
    IIC_Wait_Ack();    
    Write_IIC_Byte(IIC_Data);
    IIC_Wait_Ack();    
    IIC_Stop();
}

#endif

extern void delay_ms(int ms);

//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127

void OLED_WR_Byte(unsigned dat,unsigned cmd0data1)
{
    if(cmd0data1)
    {
        Write_IIC_Data(dat);
    }
    else
    {
        Write_IIC_Command(dat);
    }
}


/********************************************
// fill_Picture
********************************************/
void OLED_Fill_Picture(unsigned char fill_Data)
{
    unsigned char m,n;
    for(m=0;m<8;m++)
    {
        OLED_WR_Byte(0xb0+m,0);        //page0-page1
        OLED_WR_Byte(0x00,0);        //low column start address
        OLED_WR_Byte(0x10,0);        //high column start address
        for(n=0;n<128;n++)
        {
            OLED_WR_Byte(fill_Data,1);
        }
    }
}

//坐标设置

void OLED_Set_Pos(unsigned char x, unsigned char y) 
{
    OLED_WR_Byte(0xb0+y,OLED_CMD);
    OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
    OLED_WR_Byte((x&0x0f),OLED_CMD); 
}

//开启OLED显示    
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
    OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
    OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
    OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
    OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}                        
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!      
void OLED_Clear(void)  
{  
    u8 i,n;            
    for(i=0;i<8;i++)  
    {  
        OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
        OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
        OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
        for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
    } //更新显示
}
void OLED_On(void)  
{  
    u8 i,n;            
    for(i=0;i<8;i++)  
    {  
        OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
        OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
        OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
        for(n=0;n<128;n++)OLED_WR_Byte(1,OLED_DATA); 
    } //更新显示
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示                 
//size:选择字体 16/12 
void OLED_ShowChar(u8 x,u8 y,char chr, u8 mode, u8 Char_Size)
{          
    unsigned char c=0,i=0;    
    c=chr-' ';//得到偏移后的值            
    if(x>Max_Column-1){x=0;y=y+2;}
    if(Char_Size ==16)
    {
        OLED_Set_Pos(x,y);    
        for(i=0;i<8;i++)
        OLED_WR_Byte(mode ? F8X16[c*16+i] : ~F8X16[c*16+i],OLED_DATA);
        OLED_Set_Pos(x,y+1);
        for(i=0;i<8;i++)
        OLED_WR_Byte(mode ? F8X16[c*16+i+8] : ~F8X16[c*16+i+8],OLED_DATA);
    }
    else
    {    
        OLED_Set_Pos(x,y);
        for(i=0;i<6;i++)
        OLED_WR_Byte(mode ? F6x8[c][i] : ~F6x8[c][i],OLED_DATA);
    }
}

//显示一个字符号串
void OLED_ShowString(u8 x,u8 y, const char *chr,u8 mode, u8 Char_Size)
{
    unsigned char j=0;
    while (chr[j]!='\0')
    {
        OLED_ShowChar(x,y,chr[j],mode,Char_Size);
        x+=8;
        if(x>120)
        {
            x=0;
            y+=2;
        }
        j++;
    }
}

/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1, const uint8_t *BMP)
{     
    unsigned int j=0;
    unsigned char x,y;

    for(y = y0; y< y1; y++)
    {
        OLED_Set_Pos(x0, y);
        for(x = x0; x < x1; x++)
        {      
            OLED_WR_Byte(BMP[j++], OLED_DATA);            
        }
       
    }
}

static const uint8_t Init_CMD_Data[]={
    0xAE, 0x00, 0x10, 0x40, 0xB0, 0x81, 0xFF, 0xA1, 0xA6, 0xA8, 0x3F,
    0xC8, 0xD3, 0x00, 0xD5, 0x80, 0xD8, 0x05, 0xD9, 0xF1, 0xDA, 0x12,
    0xD8, 0x30, 0x8D, 0x14, 0xAF};

//初始化SSD1306                        
void OLED_Init(void)
{
    int i;

#ifdef USE_I2C

    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_APB_I2C0));
    PINCTRL_SetPadMux(PIN_SCL, IO_SOURCE_I2C0_SCL_OUT);
    PINCTRL_SetPadMux(PIN_SDA, IO_SOURCE_I2C0_SDA_OUT);
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    PINCTRL_SelI2cSclIn(I2C_PORT, PIN_SCL);    
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    PINCTRL_SelI2cIn(I2C_PORT, PIN_SCL, PIN_SDA);
#else
    #error unknown or unsupported chip family
#endif
    i2c_init(I2C_PORT);

#else

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ITEM_APB_GPIO));
    PINCTRL_SetPadMux(PIN_SCL, IO_SOURCE_GPIO);
    PINCTRL_SetPadMux(PIN_SDA, IO_SOURCE_GPIO);
    GIO_SetDirection((GIO_Index_t)PIN_SCL, GIO_DIR_OUTPUT);
    GIO_SetDirection((GIO_Index_t)PIN_SDA, GIO_DIR_OUTPUT);

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ITEM_APB_GPIO0) | (1 << SYSCTRL_ITEM_APB_GPIO1));
    PINCTRL_SetPadMux(PIN_SCL, IO_SOURCE_GPIO);
    PINCTRL_SetPadMux(PIN_SDA, IO_SOURCE_GPIO);
    GIO_SetDirection((GIO_Index_t)PIN_SCL, GIO_DIR_OUTPUT);
    GIO_SetDirection((GIO_Index_t)PIN_SDA, GIO_DIR_OUTPUT);
#endif

#endif

    vTaskDelay(pdMS_TO_TICKS(800));
    for (i = 0; i < sizeof(Init_CMD_Data); i++)
        OLED_WR_Byte(Init_CMD_Data[i], OLED_CMD);
}
