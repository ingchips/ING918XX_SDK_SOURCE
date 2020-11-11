import
  ingsoc, peripheral_i2c

const
  IO_PIN_NUMBER* = 32

type
  io_source_t* {.size: sizeof(cint).} = enum
    IO_SOURCE_GENERAL,        ##  GPIO or PWM selected by io_sel_pwm
    IO_SOURCE_SPI0_CLK, IO_SOURCE_SPI0_DO, IO_SOURCE_SPI0_SSN, IO_SOURCE_UART0_TXD,
    IO_SOURCE_I2C0_SCL_O, IO_SOURCE_I2C0_SDO, IO_SOURCE_SPI1_CLK,
    IO_SOURCE_SPI1_DO, IO_SOURCE_SPI1_SSN, IO_SOURCE_UART1_TXD,
    IO_SOURCE_I2C1_SCL_O, IO_SOURCE_I2C1_SDO, IO_SOURCE_UART0_RTS,
    IO_SOURCE_UART1_RTS, IO_SOURCE_DEBUG_BUS


const
  IO_SOURCE_MASK* = 0x0000000F

proc PINCTRL_SetPadMux*(io_pin_index: uint8; source: io_source_t) {.
    importc: "PINCTRL_SetPadMux", header: "peripheral_pinctrl.h".}
type
  uart_port_t* {.size: sizeof(cint).} = enum
    UART_PORT_0, UART_PORT_1
  i2c_port_t* {.size: sizeof(cint).} = enum
    I2C_PORT_0, I2C_PORT_1
  spi_port_t* {.size: sizeof(cint).} = enum
    SPI_PORT_0, SPI_PORT_1




##  Select input io_pin for UART RXD

proc PINCTRL_SelUartRxdIn*(port: uart_port_t; io_pin_index: uint8) {.
    importc: "PINCTRL_SelUartRxdIn", header: "peripheral_pinctrl.h".}
##  Select input io_pin for UART CTS

proc PINCTRL_SelUartCtsIn*(port: uart_port_t; io_pin_index: uint8) {.
    importc: "PINCTRL_SelUartCtsIn", header: "peripheral_pinctrl.h".}
##  Select input io_pin for SPI DI

proc PINCTRL_SelSpiDiIn*(port: spi_port_t; io_pin_index: uint8) {.
    importc: "PINCTRL_SelSpiDiIn", header: "peripheral_pinctrl.h".}
##  Select input io_pin for SPI CLK

proc PINCTRL_SelSpiClkIn*(port: spi_port_t; io_pin_index: uint8) {.
    importc: "PINCTRL_SelSpiClkIn", header: "peripheral_pinctrl.h".}
##  Select input io_pin for I2C SCL

proc PINCTRL_SelI2cSclIn*(port: i2c_port_t; io_pin_index: uint8) {.
    importc: "PINCTRL_SelI2cSclIn", header: "peripheral_pinctrl.h".}
##  Disable all input io_pins for UART/SPI/I2C

proc PINCTRL_DisableAllInputs*() {.importc: "PINCTRL_DisableAllInputs",
                                 header: "peripheral_pinctrl.h".}
type
  pinctrl_pull_mode_t* {.size: sizeof(cint).} = enum
    PINCTRL_PULL_DISABLE, PINCTRL_PULL_UP, PINCTRL_PULL_DOWN


## *
##  @brief Set pull mode of a GPIO
##
##  @param io_pin_index      The io pad to be configured.
##  @param mode              The mode to be configured
##

proc PINCTRL_Pull*(io_pin_index: uint8; mode: pinctrl_pull_mode_t) {.
    importc: "PINCTRL_Pull", header: "peripheral_pinctrl.h".}
type
  pinctrl_slew_rate_t* {.size: sizeof(cint).} = enum
    PINCTRL_SLEW_RATE_FAST, PINCTRL_SLEW_RATE_SLOW


## *
##  @brief Set slew rate of a GPIO
##
##  @param io_pin_index      The io pad to be configured.
##  @param rate              The rate to be configured (default: SLOW)
##

proc PINCTRL_SetSlewRate*(io_pin_index: uint8; rate: pinctrl_slew_rate_t) {.
    importc: "PINCTRL_SetSlewRate", header: "peripheral_pinctrl.h".}
type
  pinctrl_drive_strenght_t* {.size: sizeof(cint).} = enum
    PINCTRL_DRIVE_2mA, PINCTRL_DRIVE_4mA, PINCTRL_DRIVE_8mA, PINCTRL_DRIVE_12mA


## *
##  @brief Set slew rate of a GPIO
##
##  @param io_pin_index      The io pad to be configured.
##  @param strenght          The strenght to be configured (default: 8mA)
##

proc PINCTRL_SetDriveStrength*(io_pin_index: uint8;
                              strenght: pinctrl_drive_strenght_t) {.
    importc: "PINCTRL_SetDriveStrength", header: "peripheral_pinctrl.h".}
type
  gio_mode_t* {.size: sizeof(cint).} = enum
    IO_MODE_GPIO, IO_MODE_PWM, IO_MODE_ANT_SEL


## *
##  @brief Set working mode of a pad which has been mux-ed as IO_SOURCE_GENERAL
##
##  @param io_pin_index      The io pad to be configured. Valid range:
##                           For IO_MODE_GPIO    : [0..32]
##                           For IO_MODE_PWM     : [0..11]
##                           For IO_MODE_ANT_SEL : {7, 8, 10, 11, 16, 17, 18, 19}
##  @param mode              The mode to be configured
##  @param pwm_channel       0..6. (Ignored when mode != IO_MODE_PWM)
##  @param pwm_neg           If use the negated PWM signal. (Ignored when mode != IO_MODE_PWM)
##

proc PINCTRL_SetGeneralPadMode*(io_pin_index: uint8; mode: gio_mode_t;
                               pwm_channel: uint8; pwm_neg: uint8) {.
    importc: "PINCTRL_SetGeneralPadMode", header: "peripheral_pinctrl.h".}
##  io_pin_index: 0~11 (obsoleted, use PINCTRL_SetGeneralPadMode instead)

proc PINCTRL_SetPadPwmSel*(io_pin_index: uint8; pwm1_gpio0: uint8) {.
    importc: "PINCTRL_SetPadPwmSel", header: "peripheral_pinctrl.h".}