#define OPTIONAL_RF_CLK
#include "profilestask.h"
#include "ingsoc.h"
#include <stdio.h>
#include <string.h>

#include "platform_api.h"
#include "ing_uecc.h"
#include "peripheral_gpio.h"
#include "iic.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
#define I2C_SCL         GIO_GPIO_10
#define I2C_SDA         GIO_GPIO_11

uint8_t master_write_flag = 0;

#define DATA_CNT (2)
uint8_t write_data[DATA_CNT] = {0xCC,0x44};
uint8_t write_data_cnt = 0;
uint8_t read_data[DATA_CNT + 1] = {0,};
uint8_t read_data_cnt = 0;

void setup_peripherals_i2c_module(void);

void peripheral_i2c_write_data(void)
{
  master_write_flag = 1;
  // 设置方向，Master读取
  I2C_CtrlUpdateDirection(APB_I2C0,I2C_TRANSACTION_MASTER2SLAVE);
  // 设置每次传输的大小
  I2C_CtrlUpdateDataCnt(APB_I2C0, DATA_CNT);
  // 触发传输
  I2C_CommandWrite(APB_I2C0, I2C_COMMAND_ISSUE_DATA_TRANSACTION);
  
}

void peripheral_i2c_read_data(void)
{
  master_write_flag = 0;
  I2C_CtrlUpdateDirection(APB_I2C0,I2C_TRANSACTION_SLAVE2MASTER);
  I2C_CtrlUpdateDataCnt(APB_I2C0, DATA_CNT+1);
  I2C_CommandWrite(APB_I2C0, I2C_COMMAND_ISSUE_DATA_TRANSACTION);
}

static uint32_t peripherals_i2c_isr(void *user_data)
{
  uint8_t i;
  uint32_t status = I2C_GetIntState(APB_I2C0);
  
  // 传输结束中断，代表DATA_CNT个字节接收或者发射完成
  if(status & (1 << I2C_STATUS_CMPL))
  {
    // master写DATA_CNT个字节成功
    if(master_write_flag)
    {
      platform_printf("wr cmp %d\n", I2C_CtrlGetDataCnt(APB_I2C0));
      I2C_ClearIntState(APB_I2C0, (1 << I2C_STATUS_CMPL));

      peripheral_i2c_read_data();
    }
    else// master读DATA_CNT个字节成功
    {
      // 将剩余的fifo中的数据读出来
      for(; read_data_cnt < DATA_CNT + 1; read_data_cnt++)
      {
        read_data[read_data_cnt] = I2C_DataRead(APB_I2C0);
      }

      I2C_ClearIntState(APB_I2C0, (1 << I2C_STATUS_CMPL));
      
      // debug trace
      platform_printf("rd cmp %d ",read_data_cnt);
      for(i=0;i<DATA_CNT + 1;i++){platform_printf(" 0x%x -", read_data[i]);};
      printf("\n");
      
      // prepare for next
      read_data_cnt = 0;
    }
  }
  
  // 有slave响应了master的地址
  if(status & (1 << I2C_STATUS_ADDRHIT))
  {
    // 打开相应中断，主要是用来填写或者读取fifo
    if(master_write_flag)
    {
      I2C_IntDisable(APB_I2C0,(1 << I2C_INT_FIFO_FULL));
      I2C_IntEnable(APB_I2C0,(1<<I2C_INT_CMPL)|(1 << I2C_INT_FIFO_EMPTY));
    }
    else
    {
      I2C_IntDisable(APB_I2C0,(1 << I2C_INT_FIFO_EMPTY));
      I2C_IntEnable(APB_I2C0,(1<<I2C_INT_CMPL)|(1 << I2C_INT_FIFO_FULL));
    }
    I2C_ClearIntState(APB_I2C0, (1 << I2C_STATUS_ADDRHIT));
    platform_printf("addr hit\n");
  }
  
  // 该中断在master_write_flag==1打开，代表fifo为空，需要填充待发送的数据
  if(status & (1 << I2C_STATUS_FIFO_EMPTY))
  {
    if(master_write_flag)
    {
      // push data until fifo is full
      for(; write_data_cnt < DATA_CNT; write_data_cnt++)
      {
        //platform_printf("ept: %d \n",write_data_cnt);
        if(I2C_FifoFull(APB_I2C0)){break;}
        I2C_DataWrite(APB_I2C0,write_data[write_data_cnt]);
      }
      
      // if its the last, disable empty int
      if(write_data_cnt == DATA_CNT)
      {
        I2C_IntDisable(APB_I2C0,(1 << I2C_INT_FIFO_EMPTY));
      }
      
    }
  }
  
  // 该中断在master_write_flag==0打开，FIFO 满之后，触发中断，此时需要将所有数据都读出来
  if(status & (1 << I2C_STATUS_FIFO_FULL))
  {
    if(!master_write_flag)
    {
      for(; read_data_cnt < DATA_CNT + 1; read_data_cnt++)
      {
        if(I2C_FifoEmpty(APB_I2C0)){break;}
        read_data[read_data_cnt] = I2C_DataRead(APB_I2C0);
      }
      platform_printf("rd full %d \n", read_data_cnt);
    }
  }
  
  return 0;
}

void setup_peripherals_i2c_pin(void)
{
  SYSCTRL_ClearClkGateMulti(    (1 << SYSCTRL_ITEM_APB_I2C0)
                                | (1 << SYSCTRL_ITEM_APB_SysCtrl)
                                | (1 << SYSCTRL_ITEM_APB_PinCtrl)
                                | (1 << SYSCTRL_ITEM_APB_GPIO1)
                                | (1 << SYSCTRL_ITEM_APB_GPIO0));
  #ifdef DRIVE
  APB_PINCTRL->DR_CTRL[0] |= (3 << 20) | (3 << 22); 
  #endif
  
  #if 1
  PINCTRL_Pull(I2C_SCL,PINCTRL_PULL_UP);
  PINCTRL_Pull(I2C_SDA,PINCTRL_PULL_UP);
  PINCTRL_SelI2cIn(I2C_PORT_0,I2C_SCL,I2C_SDA);
  PINCTRL_SetPadMux(I2C_SCL,IO_SOURCE_I2C0_SCL_OUT);
  PINCTRL_SetPadMux(I2C_SDA,IO_SOURCE_I2C0_SDA_OUT);
  #else
  SetI2C0PIN(I2C_SCL,I2C_SDA);
  #endif
  
  //SYSCTRL_ResetBlock(SYSCTRL_ITEM_APB_I2C0);
  //SYSCTRL_ReleaseBlock(SYSCTRL_ITEM_APB_I2C0);
  
  platform_printf("DR_CTRL 0x%x \n",*(uint32_t*)(APB_IOMUX_BASE+0x108));
}

#define ADDRESS (0x44)
void setup_peripherals_i2c_module(void)
{

  I2C_Config(APB_I2C0,I2C_ROLE_MASTER,I2C_ADDRESSING_MODE_07BIT,ADDRESS);
  
  
  I2C_ConfigClkFrequency(APB_I2C0,I2C_CLOCKFREQUENY_STANDARD);
  
  I2C_Enable(APB_I2C0,1);
  I2C_IntEnable(APB_I2C0,(1<<I2C_INT_CMPL)|(1<<I2C_INT_ADDR_HIT));

}

void setup_peripherals_i2c(void)
{
  setup_peripherals_i2c_pin();
  setup_peripherals_i2c_module();

  platform_set_irq_callback(PLATFORM_CB_IRQ_I2C0, peripherals_i2c_isr, NULL);
  
  //platform_printf("setup done... I2C_BASE->Setup 0x%x 0x%x APB_PINCTRL->OUT_CTRL 0x%x 0x%x\n", 
  //APB_I2C0->Setup, APB_I2C0->Ctrl, APB_PINCTRL->OUT_CTRL[2], APB_PINCTRL->IN_CTRL[4]);
  //platform_printf("clk %d hclk %d pclk %d\n",SYSCTRL_GetPLLClk(),SYSCTRL_GetHClk(),SYSCTRL_GetPClk());
  
}
#endif

uint32_t cb_putc(char *c, void *dummy)
{
    uint8_t ch = (uint8_t)*c;
    UART_TypeDef * pBase = APB_UART0;
    
    while (apUART_Check_TXFIFO_FULL(pBase) == 1);
    UART_SendData(pBase, ch);

 	return 0;
}

int fputc(int ch, FILE *f)
{
    cb_putc((char *)&ch, NULL);
 	return ch;
}

void config_uart(uint32_t freq, uint32_t baud)
{
    UART_sStateStruct UART_0;

    UART_0.word_length       = UART_WLEN_8_BITS;
    UART_0.parity            = UART_PARITY_NOT_CHECK;
    UART_0.fifo_enable       = 1;
    UART_0.two_stop_bits     = 0;   //used 2 stop bit ,    0
    UART_0.receive_en        = 1;
    UART_0.transmit_en       = 1;
    UART_0.UART_en           = 1;
    UART_0.cts_en            = 0;
    UART_0.rts_en            = 0;
    UART_0.rxfifo_waterlevel = 1;    //UART_FIFO_ONE_SECOND;
    UART_0.txfifo_waterlevel = 1;    //UART_FIFO_ONE_SECOND;
    UART_0.ClockFrequency    = freq;
    UART_0.BaudRate          = baud;

    apUART_Initialize(APB_UART0, &UART_0, 0);
}

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916) 
#define KB_KEY_1        GIO_GPIO_5
#define THERMO_NRESET   GIO_GPIO_6
#define KEY_MASK        (1 << KB_KEY_1)

extern void setup_peripherals_i2c(void);

uint32_t gpio_isr(void *user_data)
{
    uint32_t current = ~GIO_ReadAll();
    uint16_t v = 0;
    // report which keys are pressed
    if (current & (1 << KB_KEY_1))
    {
      
        GIO_WriteValue(THERMO_NRESET, 0);
        printf("1");
        GIO_WriteValue(THERMO_NRESET, 1);
    }

    GIO_ClearAllIntStatus();
    return 0;
}
#endif

void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);
    SYSCTRL_ClearClkGateMulti( (1 << SYSCTRL_ClkGate_APB_I2C0)
                                 | (1 << SYSCTRL_ITEM_APB_SysCtrl)
                              | (1 << SYSCTRL_ClkGate_APB_GPIO0)
                                 | (1 << SYSCTRL_ITEM_APB_GPIO1)
                              |(1 << SYSCTRL_ClkGate_APB_PinCtrl));
    //usb11_phy_config
    //SYSCTRL_USBPhyConfig(BSP_USB_PHY_ENABLE,BSP_USB_PHY_DP_PULL_UP);
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916) 
    PINCTRL_SetPadMux(KB_KEY_1, IO_SOURCE_GPIO);
    PINCTRL_SetPadMux(THERMO_NRESET, IO_SOURCE_GPIO);
    GIO_SetDirection(KB_KEY_1, GIO_DIR_INPUT);
    GIO_SetDirection(THERMO_NRESET,GIO_DIR_OUTPUT);
    GIO_ConfigIntSource(KB_KEY_1,GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE | GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE,
                            GIO_INT_EDGE);

    //PINCTRL_Pull((GIO_Index_t)THERMO_NRESET,PINCTRL_PULL_UP);
    GIO_WriteValue(THERMO_NRESET, 1);
	
    setup_peripherals_i2c();
// while(1){};
// 	PINCTRL_DisableAllInputs();
//     PINCTRL_SelI2cIn(I2C_PORT_0, 15, 16);
//     PINCTRL_SetPadMux(15, IO_SOURCE_I2C0_SCL_OUT);
//     PINCTRL_SetPadMux(16, IO_SOURCE_I2C0_SDA_OUT);

//     PINCTRL_Pull(IO_SOURCE_I2C0_SCL_OUT,PINCTRL_PULL_UP);
//     PINCTRL_Pull(IO_SOURCE_I2C0_SDA_OUT,PINCTRL_PULL_UP);
//     PINCTRL_Pull(IO_SOURCE_I2C0_SCL_IN,PINCTRL_PULL_UP);
//     PINCTRL_Pull(IO_SOURCE_I2C0_SDA_IN,PINCTRL_PULL_UP);

//     i2c_init(I2C_PORT_0);
#endif
}



uint32_t setup_profile(void *, void *);

int ecc_rng(uint8_t *dest, unsigned size)
{
    platform_hrng(dest, size);
    return 1;
}

int app_main()
{
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    // platform_set_rf_clk_source(0);
    setup_peripherals();

#ifdef SECURE_FOTA
    uECC_set_rng(ecc_rng);
#endif

    // platform_config(PLATFORM_CFG_LOG_HCI, PLATFORM_CFG_ENABLE);
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    peripheral_i2c_write_data();


#endif
    return 0;
}

