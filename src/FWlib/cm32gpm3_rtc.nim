const
  RTC_ENABLED* = 1
  RTC_DISABLED* = 0

## *
## ***************************************************************************************
##  @brief Enable RTC module
##
##  @param[in] flag     RTC_ENABLED or RTC_DISABLED
## ***************************************************************************************
##

proc RTC_Enable*(flag: uint8) {.importc: "RTC_Enable", header: "cm32gpm3_rtc.h".}
## *
## ***************************************************************************************
##  @brief Get current RTC value (lower 32bit)
##
##  @return              Current value
## ***************************************************************************************
##

proc RTC_Current*(): uint32 {.importc: "RTC_Current", header: "cm32gpm3_rtc.h".}
## *
## ***************************************************************************************
##  @brief Get current full RTC value (full 48bit)
##
##  @return              Current value
## ***************************************************************************************
##

proc RTC_CurrentFull*(): uint64_t {.importc: "RTC_CurrentFull",
                                 header: "cm32gpm3_rtc.h".}
## *
## ***************************************************************************************
##  @brief Set timing offset (number of cycles) of next RTC interrupt
##
##  @param[in] offset    Offset in cycles
## ***************************************************************************************
##

proc RTC_SetNextIntOffset*(offset: uint32) {.importc: "RTC_SetNextIntOffset",
    header: "cm32gpm3_rtc.h".}
## *
## ***************************************************************************************
##  @brief Clear RTC interrupt status
## ***************************************************************************************
##

proc RTC_ClearInt*() {.importc: "RTC_ClearInt", header: "cm32gpm3_rtc.h".}