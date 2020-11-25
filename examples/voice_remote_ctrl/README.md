# Voice Remote Control

This example demonstrates how to:

* Real-time 4-bit ADPCM encoding of audio data;
* Use hardware timer & ADC (loop mode);
* Use SPI/PWM to emulate I2S;
* Scan keyboard state;
* Update connection parameters.

Note: Define `DEV_BOARD` to run this on development board (no keyboard),
otherwise, this app is built for a special remote controller.

Use `AUDIO_INPUT` to select input between Data/ADC/I2S (INMP441).
