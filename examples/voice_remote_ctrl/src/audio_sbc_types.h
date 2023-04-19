#ifndef _AUDIO_SBC_TYPES_H_
#define _AUDIO_SBC_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "audio_sbc_math.h"

typedef enum
{
    MONO		= SBC_MODE_MONO,
	DUAL_CHANNEL	= SBC_MODE_DUAL_CHANNEL,
	STEREO		= SBC_MODE_STEREO,
	JOINT_STEREO	= SBC_MODE_JOINT_STEREO
}sbc_mdoe;

typedef enum
{
	LOUDNESS	= SBC_AM_LOUDNESS,
	SNR		= SBC_AM_SNR
}sbc_allocation;

/* This structure contains an unpacked SBC frame.
   Yes, there is probably quite some unused space herein */
typedef struct
{
	uint8_t frequency;
	uint8_t block_mode;
	uint8_t blocks;
    sbc_mdoe mode;
	uint8_t channels;
	sbc_allocation allocation;
	uint8_t subband_mode;
	uint8_t subbands;
	uint8_t bitpool;
	uint16_t codesize;
	uint16_t length;
	//计数
	uint16_t frame_count;

	/* bit number x set means joint stereo has been used in subband x */
	uint8_t joint;

	/* only the lower 4 bits of every element are to be used */
	uint32_t scale_factor[2][8];

	/* raw integer subband samples in the frame */
	int32_t sb_sample_f[16][2][8];

	/* modified subband samples */
	int32_t sb_sample[16][2][8];

	/* original pcm audio samples */
	int16_t pcm_sample[2][16*8];
}sbc_frame;

typedef struct
{
	int subbands;
	int position[2];
	int32_t X[2][160];
}sbc_encoder_state;

typedef struct
{
	int subbands;
	int32_t  V[2][170];
	int offset[2][16];
}sbc_decoder_state;

#ifdef __cplusplus
}
#endif

#endif // _AUDIO_SBC_TYPES_H_