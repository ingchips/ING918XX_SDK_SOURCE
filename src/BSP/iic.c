#include "iic.h"      

#include "cm32gpm3.h"

#define I2C_BASE(port)    ((port) == I2C_PORT_0 ? APB_I2C0 : APB_I2C1)

void i2c_init(const i2c_port_t port)
{
    I2C_CTRL0_CLR(I2C_BASE(port), I2C_CTRL0_SFTRST | I2C_CTRL0_CLKGATE);
    SYSCTRL_SetBlockRst_APB_I2C();
}

void i2c_do_write(const i2c_port_t port, const uint32_t nrm, uint8_t addr, const uint8_t *byte_data, int16_t length)
{
    uint32_t *p_data = (uint32_t *)(byte_data + 3);
    uint32_t data = (addr <<  1) | 0;     // control: write
    I2C_TypeDef *BASE = I2C_BASE(port);
    
    if (length > 0)
        data |= (byte_data[0] <<  8) | (byte_data[1] << 16) | (byte_data[2] << 24);        

    I2C_CTRL0_CLR(BASE, I2C_CTRL0_SFTRST | I2C_CTRL0_CLKGATE);

    // ONLY SUPPORT PIO QUEUE MODE, SET HW_I2C_QUEUECTRL_PIO_QUEUE_MODE AT FRIST
    I2C_QUEUECTRL_SET(BASE, I2C_QUEUECTRL_PIO_QUEUE_MODE);

    // frist operation, do not need clear I2C_QUEUECTRL and I2C_QUEUECMD.
    BASE->I2C_QUEUECMD.NRM = nrm + 1 + length;
    
    I2C_QUEUECTRL_SET(BASE, I2C_QUEUECTRL_QUEUE_RUN);

#if 0
    printf("I2C_QUEUECTRL Value : %x \n", BASE->I2C_QUEUECTRL .NRM); // 0x24
    printf("I2C_QUEUECMD  Value : %x \n", BASE->I2C_QUEUECMD  .NRM); // 0x1b0012
#endif

    length += 1;
    while (1) 
    {
        while (I2C_QUEUESTAT_WR_QUEUE_FULL(BASE)); 
        BASE->I2C_DATA = data;
        length -= 4;
        if (length <= 0)
            break;
        data = *p_data;
        p_data++;        
    }

    // WAIT I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ (software polling)
    while (GET_I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ(BASE) == 0);
    I2C_CTRL1_CLR(BASE, I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ);

    // NOTE : MUST SET I2C_QUEUECTRL_WR_CLEAR
    I2C_QUEUECTRL_SET(BASE, I2C_QUEUECTRL_WR_CLEAR);
    I2C_QUEUECTRL_CLR(BASE, I2C_QUEUECTRL_WR_CLEAR);
}

void i2c_write(const i2c_port_t port, uint8_t addr, const uint8_t *byte_data, int16_t length)
{
    i2c_do_write(port, I2C_QUEUECMD_POST_SEND_STOP | I2C_QUEUECMD_PRE_SEND_START | I2C_QUEUECMD_MASTER_MODE | I2C_QUEUECMD_DIRECTION,
                 addr, byte_data, length);
}

void i2c_read(const i2c_port_t port, uint8_t addr, 
              const uint8_t *write_data, int16_t write_len,
              uint8_t *read_data, int16_t read_length)
{
    uint32_t *p_data = (uint32_t *)(read_data);
    I2C_TypeDef *BASE = I2C_BASE(port);

    // STEP 1: send write command
    i2c_do_write(port, I2C_QUEUECMD_PRE_SEND_START | I2C_QUEUECMD_MASTER_MODE | I2C_QUEUECMD_DIRECTION,
                 addr, write_data, write_len);

    // STEP 2 : transmit (control byte + Read command), need hold SCL (I2C_QUEUECMD_RETAIN_CLOCK)
    BASE->I2C_QUEUECMD.NRM = (I2C_QUEUECMD_RETAIN_CLOCK | I2C_QUEUECMD_PRE_SEND_START | I2C_QUEUECMD_MASTER_MODE | I2C_QUEUECMD_DIRECTION | 
          1);

    I2C_QUEUECTRL_SET(BASE, I2C_QUEUECTRL_QUEUE_RUN);

    BASE->I2C_DATA = 0xA5UL << 24 | 0x5A   << 16 |0xAA   <<  8 | (addr <<  1) | 1;

    while (GET_I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ(BASE) == 0);

    // CLEAR I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ
    I2C_CTRL1_CLR(BASE, I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ);

    // NOTE : MUST SET I2C_QUEUECTRL_WR_CLEAR
    I2C_QUEUECTRL_SET(BASE, I2C_QUEUECTRL_WR_CLEAR);
    I2C_QUEUECTRL_CLR(BASE, I2C_QUEUECTRL_WR_CLEAR);                    

#if 0    
    printf("READ : Step 2 - ... \n");    
#endif

    // 
    // STEP 3 : read data byte + (NO ACK) + STOP
    //

    BASE->I2C_QUEUECMD.NRM = (I2C_QUEUECMD_SEND_NAK_ON_LAST | I2C_QUEUECMD_POST_SEND_STOP | I2C_QUEUECMD_MASTER_MODE | 
         /*I2C_QUEUECMD_XFER_COUNT*/
         read_length);

    I2C_QUEUECTRL_SET(BASE, I2C_QUEUECTRL_QUEUE_RUN);

    // Receive DATA use I2C_QUEUEDATA;
    while (read_length > 0)
    {
        // check whether rdFIFO is empty
        while (I2C_QUEUESTAT_RD_QUEUE_EMPTY(BASE));

        *p_data = BASE->I2C_QUEUEDATA;
        p_data++;
        read_length -= sizeof(*p_data);
    }

    // WAIT I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ (software polling)
    while (GET_I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ(BASE) == 0);

    // cLEAR I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ
    I2C_CTRL1_CLR(BASE, I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ);

    // NOTE : CLEAR I2C_QUEUECTRL_RD_CLEAR
    I2C_QUEUECTRL_SET(BASE, I2C_QUEUECTRL_RD_CLEAR);
    I2C_QUEUECTRL_CLR(BASE, I2C_QUEUECTRL_RD_CLEAR);

#if 0
    printf("READ DONE ... \n");    
#endif    
}

