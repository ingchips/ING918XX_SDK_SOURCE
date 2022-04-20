# Voice Remote Control

This example demonstrates how to:

* Real-time 4-bit ADPCM encoding of audio data;
* Use hardware timer & ADC (loop mode);
* Use I2S/PDM;
* Scan keyboard state;
* Update connection parameters.

Note: Define `BOARD=BOARD_DEV` to run this on development board (no keyboard);
define `BOARD=BOARD_REM` to a special remote controller.

Use `AUDIO_INPUT` to select input.
