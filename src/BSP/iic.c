#include "iic.h"

#include <string.h>
#include "ingsoc.h"
#include "peripheral_i2c.h"

#define I2C_BASE(port)    ((port) == I2C_PORT_0 ? APB_I2C0 : APB_I2C1)

#define while_with_timeout(statement)  \
    do { timeout = I2C_HW_TIME_OUT; while (statement) { timeout--; if (timeout <= 0) return -1; } } while (0)

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

#ifndef I2C_HW_TIME_OUT
#define I2C_HW_TIME_OUT      200000
#endif

void i2c_init(const i2c_port_t port)
{
    I2C_CTRL0_CLR(I2C_BASE(port), I2C_CTRL0_SFTRST | I2C_CTRL0_CLKGATE);
}

int i2c_do_write(const i2c_port_t port, const uint32_t nrm, uint16_t addr, const uint8_t *byte_data, int16_t length)
{
    uint32_t *p_data = (uint32_t *)(byte_data + 3);
    uint32_t data = (addr <<  1) | 0;     // control: write
    I2C_TypeDef *BASE = I2C_BASE(port);
    int timeout = I2C_HW_TIME_OUT;

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
        while_with_timeout(I2C_QUEUESTAT_WR_QUEUE_FULL(BASE));
        BASE->I2C_DATA = data;
        length -= 4;
        if (length <= 0)
            break;
        data = *p_data;
        p_data++;
    }

    // WAIT I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ (software polling)
    while_with_timeout(GET_I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ(BASE) == 0);
    I2C_CTRL1_CLR(BASE, I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ);

    // NOTE : MUST SET I2C_QUEUECTRL_WR_CLEAR
    I2C_QUEUECTRL_SET(BASE, I2C_QUEUECTRL_WR_CLEAR);
    I2C_QUEUECTRL_CLR(BASE, I2C_QUEUECTRL_WR_CLEAR);

    return 0;
}

int i2c_write(const i2c_port_t port, uint16_t addr, const uint8_t *byte_data, int16_t length)
{
    return i2c_do_write(port, I2C_QUEUECMD_POST_SEND_STOP | I2C_QUEUECMD_PRE_SEND_START | I2C_QUEUECMD_MASTER_MODE | I2C_QUEUECMD_DIRECTION,
                 addr, byte_data, length);
}

static int write_bytes(uint8_t *data, uint32_t d32, int len)
{
    len = len >= 4 ? 4 : len;
    if (len < 4)
        memcpy(data, &d32, len);
    else
        *(uint32_t *)data = d32;
    return len;
}

int i2c_read(const i2c_port_t port, uint16_t addr,
              const uint8_t *write_data, int16_t write_len,
              uint8_t *read_data, int16_t read_length)
{
    I2C_TypeDef *BASE = I2C_BASE(port);
    int timeout = I2C_HW_TIME_OUT;

    if (write_len)
    {
        // STEP 1: send write command
        int r = i2c_do_write(port, I2C_QUEUECMD_PRE_SEND_START | I2C_QUEUECMD_MASTER_MODE | I2C_QUEUECMD_DIRECTION,
                     addr, write_data, write_len);
        if (r != 0) return r;
    }
    else
    {
        I2C_CTRL0_CLR(BASE, I2C_CTRL0_SFTRST | I2C_CTRL0_CLKGATE);

        // ONLY SUPPORT PIO QUEUE MODE, SET HW_I2C_QUEUECTRL_PIO_QUEUE_MODE AT FRIST
        I2C_QUEUECTRL_SET(BASE, I2C_QUEUECTRL_PIO_QUEUE_MODE);
    }

    // STEP 2 : transmit (control byte + Read command), need hold SCL (I2C_QUEUECMD_RETAIN_CLOCK)
    BASE->I2C_QUEUECMD.NRM = (I2C_QUEUECMD_RETAIN_CLOCK | I2C_QUEUECMD_PRE_SEND_START | I2C_QUEUECMD_MASTER_MODE | I2C_QUEUECMD_DIRECTION |
          1);

    I2C_QUEUECTRL_SET(BASE, I2C_QUEUECTRL_QUEUE_RUN);

    BASE->I2C_DATA = 0xA5UL << 24 | 0x5A   << 16 |0xAA   <<  8 | (addr <<  1) | 1;

    while_with_timeout(GET_I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ(BASE) == 0);

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
        while_with_timeout(I2C_QUEUESTAT_RD_QUEUE_EMPTY(BASE));

        int len = write_bytes(read_data, BASE->I2C_QUEUEDATA, read_length);
        read_data += len;
        read_length -= len;
    }

    // WAIT I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ (software polling)
    while_with_timeout(GET_I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ(BASE) == 0);

    // cLEAR I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ
    I2C_CTRL1_CLR(BASE, I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ);

    // NOTE : CLEAR I2C_QUEUECTRL_RD_CLEAR
    I2C_QUEUECTRL_SET(BASE, I2C_QUEUECTRL_RD_CLEAR);
    I2C_QUEUECTRL_CLR(BASE, I2C_QUEUECTRL_RD_CLEAR);

#if 0
    printf("READ DONE ... \n");
#endif
    return 0;
}

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

#ifndef I2C_HW_TIME_OUT
#define I2C_HW_TIME_OUT      600000
#endif

void i2c_init(const i2c_port_t port)
{
    I2C_Enable(I2C_BASE(port), 0);
}

int I2C_MasterWrite(I2C_TypeDef *I2C_BASE, I2C_AddressingMode AddrMode, uint16_t Addr, const uint8_t *Data, uint8_t DataCnt)
{
    uint8_t i;
    int timeout = I2C_HW_TIME_OUT;

    I2C_Config(I2C_BASE, I2C_ROLE_MASTER, AddrMode, Addr);

    I2C_BASE->Ctrl = DataCnt
                    | (I2C_TRANSACTION_MASTER2SLAVE << bsI2C_CTRL_TRANSACTION_DIR)
                    | I2C_CTRL_MASTER_ALL_PHASES;

    I2C_BASE->IntEn = (1 << I2C_INT_CMPL) | (1 << I2C_INT_FIFO_EMPTY);

    I2C_BASE->Cmd = 0x1;

    for (i = 0; i < DataCnt; i++) {
        while_with_timeout(I2C_FifoFull(I2C_BASE)) ;
        I2C_BASE->Data = Data[i];
    }
    I2C_BASE->IntEn = (1 << I2C_INT_CMPL);
    while_with_timeout(I2C_TransactionComplete(I2C_BASE) == 0);
    I2C_BASE->IntEn = 0;

    return 0;
}

int I2C_MasterRead(I2C_TypeDef *I2C_BASE, I2C_AddressingMode AddrMode, uint16_t Addr, uint8_t *Data, uint8_t DataCnt)
{
    uint8_t i;
    int timeout = I2C_HW_TIME_OUT;
    I2C_Config(I2C_BASE, I2C_ROLE_MASTER, AddrMode, Addr);

    I2C_BASE->Ctrl = DataCnt
                    | (I2C_TRANSACTION_SLAVE2MASTER << bsI2C_CTRL_TRANSACTION_DIR)
                    | I2C_CTRL_MASTER_ALL_PHASES;

    I2C_BASE->IntEn = (1 << I2C_INT_CMPL);

    I2C_BASE->Cmd = 0x1;

    for (i = 0; i < DataCnt; i++) {
        while_with_timeout(I2C_FifoEmpty(I2C_BASE)) ;
        Data[i] = I2C_BASE->Data;
    }

    while_with_timeout(I2C_TransactionComplete(I2C_BASE) == 0);
    I2C_BASE->IntEn = 0;
    return 0;
}

int i2c_write(const i2c_port_t port, uint16_t addr, const uint8_t *byte_data, int16_t length)
{
    return I2C_MasterWrite(I2C_BASE(port), I2C_ADDRESSING_MODE_07BIT, addr, byte_data, length);
}

int i2c_read(const i2c_port_t port, uint16_t addr,
              const uint8_t *write_data, int16_t write_len,
              uint8_t *read_data, int16_t read_length)
{
    I2C_TypeDef *BASE = I2C_BASE(port);

    if (write_len)
    {
        int r = I2C_MasterWrite(BASE, I2C_ADDRESSING_MODE_07BIT, addr, write_data, write_len);
        if (r != 0) return r;
    }
    else;

    return I2C_MasterRead(BASE, I2C_ADDRESSING_MODE_07BIT, addr, read_data, read_length);
}

#endif
