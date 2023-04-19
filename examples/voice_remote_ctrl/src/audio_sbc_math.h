#ifndef _AUDIO_SBC_MATH_H_
#define _AUDIO_SBC_MATH_H_



#include <stdint.h>

#define SBC_SYNCWORD	0x9C

#define MSBC_SYNCWORD	0xAD
#define MSBC_BLOCKS	15

#define A2DP_SAMPLING_FREQ_16000		(1 << 3)
#define A2DP_SAMPLING_FREQ_32000		(1 << 2)
#define A2DP_SAMPLING_FREQ_44100		(1 << 1)
#define A2DP_SAMPLING_FREQ_48000		(1 << 0)

#define A2DP_CHANNEL_MODE_MONO			(1 << 3)
#define A2DP_CHANNEL_MODE_DUAL_CHANNEL		(1 << 2)
#define A2DP_CHANNEL_MODE_STEREO		(1 << 1)
#define A2DP_CHANNEL_MODE_JOINT_STEREO		(1 << 0)

#define A2DP_BLOCK_LENGTH_4			(1 << 3)
#define A2DP_BLOCK_LENGTH_8			(1 << 2)
#define A2DP_BLOCK_LENGTH_12			(1 << 1)
#define A2DP_BLOCK_LENGTH_16			(1 << 0)

#define A2DP_SUBBANDS_4				(1 << 1)
#define A2DP_SUBBANDS_8				(1 << 0)

#define A2DP_ALLOCATION_SNR			(1 << 1)
#define A2DP_ALLOCATION_LOUDNESS		(1 << 0)

#define SCALE_OUT_BITS 15

#define EIO 5
#define ENOSPC 28

#define fabs(x) ((x) < 0 ? -(x) : (x))
/* C does not provide an explicit arithmetic shift right but this will
   always be correct and every compiler *should* generate optimal code */
#define ASR(val, bits) ((-2 >> 1 == -1) ? \
		 ((int32_t)(val)) >> (bits) : ((int32_t) (val)) / (1 << (bits)))

#define SCALE_PROTO4_TBL	15
#define SCALE_ANA4_TBL		17
#define SCALE_PROTO8_TBL			(16)			/*   1八子带enc的cos系数 :16    */
#define SCALE_ANA8_TBL				(17)			/*   八子带enc的角度pi系数 :17    */
#define SCALE_SPROTO4_TBL	12
#define SCALE_SPROTO8_TBL			14				/*   八子带enc的系数/-8的等系数：14   */
#define SCALE_NPROTO4_TBL	11
#define SCALE_NPROTO8_TBL			11				/*  八子带dec sbc_proto_8_80m1系数：11*/
#define SCALE4_STAGE1_BITS	15		
#define SCALE4_STAGE2_BITS	16		
#define SCALE4_STAGED1_BITS  15
#define SCALE4_STAGED2_BITS  16
#define SCALE8_STAGE1_BITS			(15)			/* 2 八子带enc cos乘加后的系数 :15    */	
#define SCALE8_STAGE2_BITS			(15)			/*  八子带enc乘加后求out的系数 :15    */	
#define SCALE8_STAGED1_BITS		(15)				/*  八子带dec synmatrix8之后求V的系数：15    */	
#define SCALE8_STAGED2_BITS		(16)				/*  八子带dec sbc_proto_8_80m1之后求V的系数：16*/	

typedef int32_t sbc_fixed_t;

#define SCALE4_STAGE1(src)  ASR(src, SCALE4_STAGE1_BITS)
#define SCALE4_STAGE2(src)  ASR(src, SCALE4_STAGE2_BITS)
#define SCALE4_STAGED1(src) ASR(src, SCALE4_STAGED1_BITS)
#define SCALE4_STAGED2(src) ASR(src, SCALE4_STAGED2_BITS)
#define SCALE8_STAGE1(src)  ASR(src, SCALE8_STAGE1_BITS)
#define SCALE8_STAGE2(src)  ASR(src, SCALE8_STAGE2_BITS)
#define SCALE8_STAGED1(src) ASR(src, SCALE8_STAGED1_BITS)
#define SCALE8_STAGED2(src) ASR(src, SCALE8_STAGED2_BITS)

#define SBC_FIXED_0(val) { val = 0; }
#define MUL(a, b)        ((a) * (b))
#ifdef __arm__
#define MULA(a, b, res) ({				\
		int tmp = res;			\
		__asm__(				\
			"mla %0, %2, %3, %0"		\
			: "=&r" (tmp)			\
			: "0" (tmp), "r" (a), "r" (b));	\
		tmp; })
#else
#define MULA(a, b, res)  ((a) * (b) + (res))
#endif

#if defined(_MSC_VER)
#define SBC_EXPORT __declspec(dllexport)
#else
#define SBC_EXPORT __attribute__ ((visibility("default")))
#define FMT_EXPORT __attribute__ ((visibility("default")))
#endif


#endif // _AUDIO_SBC_MATH_H_