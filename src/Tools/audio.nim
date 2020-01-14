##  References for ADPCM:
##  1. Text Books on Communication Systems
##  2. http://www.cs.columbia.edu/~hgs/audio/dvi/IMA_ADPCM.pdf

type
  pcm_sample_t* = int16
  adpcm_encode_output_cb_f* = proc (output: uint8; param: pointer) {.noconv.}
  adpcm_decode_output_cb_f* = proc (output: pcm_sample_t; param: pointer) {.noconv.}
  adpcm_state_t* {.importc: "adpcm_state_t", header: "audio.h", bycopy.} = object
    predicated* {.importc: "predicated".}: pcm_sample_t
    index* {.importc: "index".}: int16

  adpcm_enc_t* {.importc: "adpcm_enc_t", header: "audio.h", bycopy.} = object
    callback* {.importc: "callback".}: adpcm_encode_output_cb_f
    param* {.importc: "param".}: pointer
    state* {.importc: "state".}: adpcm_state_t
    output* {.importc: "output".}: uint8
    output_index* {.importc: "output_index".}: uint8

  adpcm_dec_t* {.importc: "adpcm_dec_t", header: "audio.h", bycopy.} = object
    callback* {.importc: "callback".}: adpcm_decode_output_cb_f
    param* {.importc: "param".}: pointer
    state* {.importc: "state".}: adpcm_state_t


proc adpcm_enc_init*(adpcm: ptr adpcm_enc_t; callback: adpcm_encode_output_cb_f;
                    param: pointer) {.importc: "adpcm_enc_init", header: "audio.h".}
proc adpcm_dec_init*(adpcm: ptr adpcm_dec_t; callback: adpcm_decode_output_cb_f;
                    param: pointer) {.importc: "adpcm_dec_init", header: "audio.h".}
proc adpcm_encode*(adpcm: ptr adpcm_enc_t; sample: pcm_sample_t) {.
    importc: "adpcm_encode", header: "audio.h".}
proc adpcm_decode*(adpcm: ptr adpcm_dec_t; data: uint8) {.importc: "adpcm_decode",
    header: "audio.h".}