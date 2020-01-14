import
  cm32gpm3_i2c

const
  bsPINCTRL_PULL_UP* = 0
  bwPINCTRL_PULL_UP* = 1
  bsPINCTRL_PULL_DOWN* = 1
  bwPINCTRL_PULL_DOWN* = 1
  bsPINCTRL_SCHMITT_TRIGGER* = 2
  bwPINCTRL_SCHMITT_TRIGGER* = 1
  bsPINCTRL_SLEW_RATE* = 3
  bwPINCTRL_SLEW_RATE* = 1
  bsPINCTRL_DRIVER_STRENGTH* = 4
  bwPINCTRL_DRIVER_STRENGTH* = 3
  bsPINCTRL_FUNC_MUX* = 8
  bwPINCTRL_FUNC_MUX* = 2
  PINCTRL_PULL_UP* = (1 shl bsPINCTRL_PULL_UP)
  PINCTRL_PULL_DOWN* = (1 shl bsPINCTRL_PULL_DOWN)
  PINCTRL_SCHMITT_TRIGGER* = (1 shl bsPINCTRL_SCHMITT_TRIGGER)
  PINCTRL_SLEW_RATE* = (1 shl bsPINCTRL_SLEW_RATE)
  PINCTRL_DS_0* = 0
  PINCTRL_DS_1* = 1
  PINCTRL_DS_2* = 2
  PINCTRL_DS_4* = 4
  PINCTRL_FUNC_0* = 0
  PINCTRL_FUNC_1* = 1
  PINCTRL_FUNC_2* = 2
  PINCTRL_FUNC_3* = 3
  IO_PIN_NUMBER* = 32

type
  io_source_t* {.size: sizeof(cint).} = enum
    IO_SOURCE_GENERAL,        ##  GPIO or PWM selected by io_sel_pwm
    IO_SOURCE_SPI0_CLK, IO_SOURCE_SPI0_DO, IO_SOURCE_SPI0_SSN, IO_SOURCE_UART0_TXD,
    IO_SOURCE_I2C0_SCL_O, IO_SOURCE_I2C0_SDO, IO_SOURCE_SPI1_CLK,
    IO_SOURCE_SPI1_DO, IO_SOURCE_SPI1_SSN, IO_SOURCE_UART1_TXD,
    IO_SOURCE_I2C1_SCL_O, IO_SOURCE_I2C1_SDO, IO_SOURCE_UART0_RST,
    IO_SOURCE_UART1_RST, IO_SOURCE_DEBUG_BUS


const
  IO_SOURCE_MASK* = 0x0000000F

proc PINCTRL_SetPadMux*(io_pin_index: uint8; source: io_source_t) {.
    importc: "PINCTRL_SetPadMux", header: "cm32gpm3_pinctrl.h".}
##  Select input io_pin for UART RXD

proc PINCTRL_SelUartRxdIn*(port: uart_port_t; io_pin_index: uint8) {.
    importc: "PINCTRL_SelUartRxdIn", header: "cm32gpm3_pinctrl.h".}
##  Select input io_pin for UART CTS

proc PINCTRL_SelUartCtsIn*(port: uart_port_t; io_pin_index: uint8) {.
    importc: "PINCTRL_SelUartCtsIn", header: "cm32gpm3_pinctrl.h".}
##  Select input io_pin for SPI DI

proc PINCTRL_SelSpiDiIn*(port: spi_port_t; io_pin_index: uint8) {.
    importc: "PINCTRL_SelSpiDiIn", header: "cm32gpm3_pinctrl.h".}
##  Select input io_pin for SPI CLK

proc PINCTRL_SelSpiClkIn*(port: spi_port_t; io_pin_index: uint8) {.
    importc: "PINCTRL_SelSpiClkIn", header: "cm32gpm3_pinctrl.h".}
##  Select input io_pin for I2C SCL

proc PINCTRL_SelI2cSclIn*(port: i2c_port_t; io_pin_index: uint8) {.
    importc: "PINCTRL_SelI2cSclIn", header: "cm32gpm3_pinctrl.h".}
##  Disable all input io_pins for UART/SPI/I2C

proc PINCTRL_DisableAllInputs*() {.importc: "PINCTRL_DisableAllInputs",
                                 header: "cm32gpm3_pinctrl.h".}
##  io_pin_index: 0~11

proc PINCTRL_SetPadPwmSel*(io_pin_index: uint8; pwm1_gpio0: uint8) {.
    importc: "PINCTRL_SetPadPwmSel", header: "cm32gpm3_pinctrl.h".}