#ifndef _SBC_H_
#define _SBC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef int16_t pcm_sample_t;

typedef void (*sbc_encode_output_cb_f)(uint8_t output, void *param);
typedef void (*sbc_decode_output_cb_f)(pcm_sample_t output, void* param);

/* sampling frequency */
#define SBC_FREQ_16000		0x00
#define SBC_FREQ_32000		0x01
#define SBC_FREQ_44100		0x02
#define SBC_FREQ_48000		0x03

/* blocks */
#define SBC_BLK_4		0x00
#define SBC_BLK_8		0x01
#define SBC_BLK_12		0x02
#define SBC_BLK_16		0x03

/* channel mode */
#define SBC_MODE_MONO		0x00
#define SBC_MODE_DUAL_CHANNEL	0x01
#define SBC_MODE_STEREO		0x02
#define SBC_MODE_JOINT_STEREO	0x03

/* allocation method */
#define SBC_AM_LOUDNESS		0x00
#define SBC_AM_SNR		0x01

/* subbands */
#define SBC_SB_4		0x00
#define SBC_SB_8		0x01

/* data endianess */
#define SBC_LE			0x00
#define SBC_BE			0x01

typedef struct{
	uint8_t flags;
	uint8_t frequency;
	uint8_t blocks;
	uint8_t channels;
	uint8_t subbands;
	uint8_t mode;
	uint8_t allocation;
	uint8_t bitpool;
	uint8_t endian;

	void *priv;
	void *priv_alloc_base;
	sbc_encode_output_cb_f callback;
}sbc_t;

int sbc_enc_init(sbc_t *sbc, sbc_encode_output_cb_f callback, uint8_t flags);
int sbc_reinit(sbc_t *sbc, uint8_t flags);

/* Encodes ONE input block into ONE output block */
int sbc_encode(sbc_t *sbc, 
			   void *input, 
			   int input_len,
			   void *output, 
			   int output_len);

/* Returns the compressed block size in bytes */
int sbc_get_frame_length(sbc_t *sbc);

/* Returns the uncompressed block size in bytes */
int sbc_get_codesize(sbc_t *sbc);

void sbc_finish(sbc_t *sbc);


#ifdef __cplusplus
}
#endif

#endif // _SBC_H_


 

