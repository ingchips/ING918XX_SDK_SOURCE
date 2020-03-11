import
  ingsoc, peripheral_uart,
  FreeRTOS, platform_api, profile

proc cbHardFault(info: ptr hardFaultInfoT; _: pointer): uint32 {.noconv.} =
  platformPrintf("HARDFAULT:\nPC : 0x%08X\nLR : 0x%08X\nPSR: 0x%08X\n" &
                  "R0 : 0x%08X\nR1 : 0x%08X\nR2 : 0x%08X\nP3 : 0x%08X\n" &
                  "R12: 0x%08X\n",
                  info.pc, info.lr, info.psr, 
                  info.r0, info.r1, info.r2, info.r3, info.r12)
  while true: discard

proc cbAssertion(info: ptr assertionInfoT, _: pointer): uint32 {.noconv.} =
  platformPrintf("[ASSERTION] @ %s:%d\n",
                  info.fileName, info.lineNo)
  while true: discard

const
  PRINT_PORT = APB_UART0

proc cbPutC(c: ptr char; _: pointer): uint32 {.noconv.} =
  while (apUART_Check_TXFIFO_FULL(PRINT_PORT) == 1): discard
  UART_SendData(PRINT_PORT, cast[uint8](c[]));
  return 0

proc write(_: int, p: ptr UncheckedArray[char], len: int): int {.exportc: "_write" noconv.} =
  var i: int = 0
  while i < len:
      discard cbPutC(addr p[i], nil)
      inc(i)

  return len

proc flockfile*(_: File) {.exportc noconv.}= discard

proc funlockfile*(_: File) {.exportc noconv.}= discard

proc configUart(freq: uint32; baud: uint32) =  
  var config = UART_sStateStruct(
    UART_en           : 1,
    word_length       : UART_WLEN_8_BITS,
    parity            : UART_PARITY_NOT_CHECK,
    fifo_enable       : 1, two_stop_bits     : 0,
    receive_en        : 1, transmit_en       : 1,    
    cts_en            : 0, rts_en            : 0,
    rxfifo_waterlevel : 1, txfifo_waterlevel : 1,
    ClockFrequency    : freq,
    BaudRate          : baud)

  apUART_Initialize(PRINT_PORT, addr config, 0)

proc setupPeripherals() =
  configUart(OSC_CLK_FREQ, 921600)

proc onDeepSleepWakeup(dummy: pointer; userData: pointer): uint32 {.noconv.} =
  setupPeripherals()
  return 0

proc queryDeepSleepAllowed(dummy: pointer; userData: pointer): uint32 {.noconv.} =
  return 0

proc appMain*(): int {.exportc noconv.} =
  # If there are *three* crystals on board, *uncomment* below line.
  # Otherwise, below line should be kept commented out.
  #platformSetRfClkSource(0)

  platformSetEvtCallback(PLATFORM_CB_EVT_PROFILE_INIT, setupProfile, nil)

  platformSetEvtCallback(PLATFORM_CB_EVT_HARD_FAULT, cast[fPlatformEvtCB](cbHardFault), nil)
  #platformSetEvtCallback(PLATFORM_CB_EVT_ASSERTION, cast[fPlatformEvtCB](cbAssertion), nil)
  platformSetEvtCallback(PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP, onDeepSleepWakeup, nil)
  platformSetEvtCallback(PLATFORM_CB_EVT_QUERY_DEEP_SLEEP_ALLOWED, queryDeepSleepAllowed, nil)
  platformSetEvtCallback(PLATFORM_CB_EVT_PUTC, cast[fPlatformEvtCB](cbPutc), nil)
  
  # platform_config(PLATFORM_CFG_LOG_HCI, PLATFORM_CFG_ENABLE)

  setupPeripherals()

  return 0
