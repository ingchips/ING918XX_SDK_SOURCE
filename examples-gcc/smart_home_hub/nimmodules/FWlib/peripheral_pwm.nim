when defined(TARGET_FPGA):
  const
    PWM_CLOCK_FREQ* = 16000000
else:
  const
    PWM_CLOCK_FREQ* = 24000000
const
  PWM_CHANNEL_NUMBER* = 6

type
  PWM_WordMode_t* {.size: sizeof(cint).} = enum
    PWM_WORK_MODE_UP_WITHOUT_DIED_ZONE = 0x00000000,
    PWM_WORK_MODE_UP_WITH_DIED_ZONE = 0x00000001,
    PWM_WORK_MODE_UPDOWN_WITHOUT_DIED_ZONE = 0x00000002,
    PWM_WORK_MODE_UPDOWN_WITH_DIED_ZONE = 0x00000003,
    PWM_WORK_MODE_SINGLE_WITHOUT_DIED_ZONE = 0x00000004


proc PWM_Enable*(channel_index: uint8; enable: uint8) {.importc: "PWM_Enable",
    header: "peripheral_pwm.h".}
proc PWM_SetMask*(channel_index: uint8; mask_a: uint8; mask_b: uint8) {.
    importc: "PWM_SetMask", header: "peripheral_pwm.h".}
proc PWM_SetMode*(channel_index: uint8; mode: PWM_WordMode_t) {.
    importc: "PWM_SetMode", header: "peripheral_pwm.h".}
##  PMW Halt Mode
##  When the register HALT_ENABLE is set to 1, PWM will controlled by HALT_CONFIG registers.
##  The PWM_OUT_A will output the value of HALT_CONFIG[0], and the PWM_OUT_B will output the value of HALT_CONFIG[1]

proc PWM_HaltCtrlEnable*(channel_index: uint8; enable: uint8) {.
    importc: "PWM_HaltCtrlEnable", header: "peripheral_pwm.h".}
proc PWM_HaltCtrlCfg*(channel_index: uint8; out_a: uint8; out_b: uint8) {.
    importc: "PWM_HaltCtrlCfg", header: "peripheral_pwm.h".}
##  The PWM waveform can be set to multi-duty-cycle if it's not in single step mode or halt mode
##  comp_num is in [0..3], which means [1..4] duty-cycles are used (\ref PWM_SetHighThreshold)

proc PWM_SetMultiDutyCycleCtrl*(channel_index: uint8; comp_num: uint8) {.
    importc: "PWM_SetMultiDutyCycleCtrl", header: "peripheral_pwm.h".}
##  thresholds

proc PWM_SetPeraThreshold*(channel_index: uint8; threshold: uint32) {.
    importc: "PWM_SetPeraThreshold", header: "peripheral_pwm.h".}
proc PWM_SetDiedZoneThreshold*(channel_index: uint8; threshold: uint32) {.
    importc: "PWM_SetDiedZoneThreshold", header: "peripheral_pwm.h".}
##  multi_duty_index is in [0..3]

proc PWM_SetHighThreshold*(channel_index: uint8; multi_duty_index: uint8;
                          threshold: uint32) {.importc: "PWM_SetHighThreshold",
    header: "peripheral_pwm.h".}
##  Below is an example:
##  a PWM channel is set up to have a specified `frequency`, and `on_dudy` in each cycle (on out_a)
##  `frequency` is in Hz
##  `on_duty` is in percent, i.e. is in [0..100]

proc PWM_SetupSimple*(channel_index: uint8; frequency: uint32; on_duty: uint16) {.
    importc: "PWM_SetupSimple", header: "peripheral_pwm.h".}