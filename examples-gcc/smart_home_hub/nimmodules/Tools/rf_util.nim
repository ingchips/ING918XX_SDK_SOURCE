when not defined(_rf_util_h):
  proc rf_enable_powerboost*() {.importc: "rf_enable_powerboost",
                               header: "rf_util.h".}