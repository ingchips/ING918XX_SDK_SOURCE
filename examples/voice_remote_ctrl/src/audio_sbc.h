#ifndef __SBC_H
#define __SBC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
//#include <sys/types.h>
// #if defined(_MSC_VER)
// #define  __declspec(dllexport)
// #else
// #define  __attribute__ ((visibility("default")))
// #endif

// #ifdef __GNUC__
// #define SBC_ALWAYS_INLINE inline __attribute__((always_inline))
// #else
// #define SBC_ALWAYS_INLINE inline
// #endif

typedef int8_t sbc_sample_t;

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

struct sbc_struct {
	unsigned long flags;

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
};

typedef struct sbc_struct sbc_t;

int sbc_init(sbc_t *sbc, unsigned long flags);
int sbc_reinit(sbc_t *sbc, unsigned long flags);

/* Encodes ONE input block into ONE output block */
int sbc_encode(sbc_t *sbc, void *input, int input_len,
			void *output, int output_len, int *written);

/* Returns the compressed block size in bytes */
int sbc_get_frame_length(sbc_t *sbc);


/* Returns the uncompressed block size in bytes */
int sbc_get_codesize(sbc_t *sbc);

void sbc_finish(sbc_t *sbc);


#ifdef __cplusplus
}
#endif

#endif /* __SBC_H */


 

