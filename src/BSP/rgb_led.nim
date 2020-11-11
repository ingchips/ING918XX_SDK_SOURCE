proc set_rgb_led_color*(r: uint8; g: uint8; b: uint8) {.importc: "set_rgb_led_color",
    header: "rgb_led.h".}
proc setup_rgb_led*() {.importc: "setup_rgb_led", header: "rgb_led.h".}