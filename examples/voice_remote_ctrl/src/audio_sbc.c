#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#include "audio_adpcm.h"
#include "ingsoc.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"

#include "platform_api.h"

#include "audio_sbc.h"
#include "audio_sbc_tables.h"
#include "audio_sbc_types.h"

static void __sbc_analyze_four(const int32_t *in, int32_t *out);

static void __sbc_analyze_eight(const int32_t *in, int32_t *out);

static void sbc_analyze_four(sbc_encoder_state *state,
							 sbc_frame *frame, 
							 int ch, 
							 int blk);

static void sbc_analyze_eight(sbc_encoder_state *state,
							  sbc_frame *frame, 
							  int ch, 
							  int blk);


/*
 * Calculates the CRC-8 of the first len bits in data
 */
static const uint8_t crc_table[256] = {
	0x00, 0x1D, 0x3A, 0x27, 0x74, 0x69, 0x4E, 0x53,
	0xE8, 0xF5, 0xD2, 0xCF, 0x9C, 0x81, 0xA6, 0xBB,
	0xCD, 0xD0, 0xF7, 0xEA, 0xB9, 0xA4, 0x83, 0x9E,
	0x25, 0x38, 0x1F, 0x02, 0x51, 0x4C, 0x6B, 0x76,
	0x87, 0x9A, 0xBD, 0xA0, 0xF3, 0xEE, 0xC9, 0xD4,
	0x6F, 0x72, 0x55, 0x48, 0x1B, 0x06, 0x21, 0x3C,
	0x4A, 0x57, 0x70, 0x6D, 0x3E, 0x23, 0x04, 0x19,
	0xA2, 0xBF, 0x98, 0x85, 0xD6, 0xCB, 0xEC, 0xF1,
	0x13, 0x0E, 0x29, 0x34, 0x67, 0x7A, 0x5D, 0x40,
	0xFB, 0xE6, 0xC1, 0xDC, 0x8F, 0x92, 0xB5, 0xA8,
	0xDE, 0xC3, 0xE4, 0xF9, 0xAA, 0xB7, 0x90, 0x8D,
	0x36, 0x2B, 0x0C, 0x11, 0x42, 0x5F, 0x78, 0x65,
	0x94, 0x89, 0xAE, 0xB3, 0xE0, 0xFD, 0xDA, 0xC7,
	0x7C, 0x61, 0x46, 0x5B, 0x08, 0x15, 0x32, 0x2F,
	0x59, 0x44, 0x63, 0x7E, 0x2D, 0x30, 0x17, 0x0A,
	0xB1, 0xAC, 0x8B, 0x96, 0xC5, 0xD8, 0xFF, 0xE2,
	0x26, 0x3B, 0x1C, 0x01, 0x52, 0x4F, 0x68, 0x75,
	0xCE, 0xD3, 0xF4, 0xE9, 0xBA, 0xA7, 0x80, 0x9D,
	0xEB, 0xF6, 0xD1, 0xCC, 0x9F, 0x82, 0xA5, 0xB8,
	0x03, 0x1E, 0x39, 0x24, 0x77, 0x6A, 0x4D, 0x50,
	0xA1, 0xBC, 0x9B, 0x86, 0xD5, 0xC8, 0xEF, 0xF2,
	0x49, 0x54, 0x73, 0x6E, 0x3D, 0x20, 0x07, 0x1A,
	0x6C, 0x71, 0x56, 0x4B, 0x18, 0x05, 0x22, 0x3F,
	0x84, 0x99, 0xBE, 0xA3, 0xF0, 0xED, 0xCA, 0xD7,
	0x35, 0x28, 0x0F, 0x12, 0x41, 0x5C, 0x7B, 0x66,
	0xDD, 0xC0, 0xE7, 0xFA, 0xA9, 0xB4, 0x93, 0x8E,
	0xF8, 0xE5, 0xC2, 0xDF, 0x8C, 0x91, 0xB6, 0xAB,
	0x10, 0x0D, 0x2A, 0x37, 0x64, 0x79, 0x5E, 0x43,
	0xB2, 0xAF, 0x88, 0x95, 0xC6, 0xDB, 0xFC, 0xE1,
	0x5A, 0x47, 0x60, 0x7D, 0x2E, 0x33, 0x14, 0x09,
	0x7F, 0x62, 0x45, 0x58, 0x0B, 0x16, 0x31, 0x2C,
	0x97, 0x8A, 0xAD, 0xB0, 0xE3, 0xFE, 0xD9, 0xC4
};

static uint8_t sbc_crc8(const uint8_t *data, int len)
{
	uint8_t crc = 0x0f;
	int i;
	uint8_t octet;

	for (i = 0; i < len / 8; i++)
		crc = crc_table[crc ^ data[i]];

	octet = len % 8 ? data[i] : 0;
	for (i = 0; i < len % 8; i++) {
		char bit = ((octet ^ crc) & 0x80) >> 7;

		crc = ((crc & 0x7f) << 1) ^ (bit ? 0x1d : 0);

		octet = octet << 1;
	}

	return crc;
}

/*
 * Code straight from the spec to calculate the bits array
 * Takes a pointer to the frame in question, a pointer to the bits array and
 * the sampling frequency (as 2 bit integer)
 */
static void sbc_calculate_bits_internal(const sbc_frame *frame, 
										int (*bits)[8], 
										int subbands)
{
	uint8_t sf = frame->frequency;

	if(frame->mode == MONO || frame->mode == DUAL_CHANNEL)
	{
		int bitneed[2][8], loudness, max_bitneed, bitcount, slicecount, bitslice;
		int ch, sb;

		for(ch = 0; ch < frame->channels; ch++)
		{
			max_bitneed = 0;
			if(frame->allocation == SNR)
			{
				for(sb = 0; sb < subbands; sb++)
				{
					bitneed[ch][sb] = frame->scale_factor[ch][sb];
					if (bitneed[ch][sb] > max_bitneed)
						max_bitneed = bitneed[ch][sb];
				}
			} 
			else
			{
				for(sb = 0; sb < subbands; sb++)
				{
					if(frame->scale_factor[ch][sb] == 0)
						bitneed[ch][sb] = -5;
					else
					{
						if (subbands == 4)
							loudness = frame->scale_factor[ch][sb] - sbc_offset4[sf][sb];
						else
							loudness = frame->scale_factor[ch][sb] - sbc_offset8[sf][sb];
						if (loudness > 0)
							bitneed[ch][sb] = loudness / 2;
						else
							bitneed[ch][sb] = loudness;
					}
					if (bitneed[ch][sb] > max_bitneed)
						max_bitneed = bitneed[ch][sb];
				}
			}

			bitcount = 0;
			slicecount = 0;
			bitslice = max_bitneed + 1;
			do {
				bitslice--;
				bitcount += slicecount;
				slicecount = 0;
				for(sb = 0; sb < subbands; sb++)
				{
					if ((bitneed[ch][sb] > bitslice + 1) && (bitneed[ch][sb] < bitslice + 16))
						slicecount++;
					else if (bitneed[ch][sb] == bitslice + 1)
						slicecount += 2;
				}
			} while (bitcount + slicecount < frame->bitpool);

			if(bitcount + slicecount == frame->bitpool)
			{
				bitcount += slicecount;
				bitslice--;
			}

			for(sb = 0; sb < subbands; sb++)
			{
				if (bitneed[ch][sb] < bitslice + 2)
					bits[ch][sb] = 0;
				else
				{
					bits[ch][sb] = bitneed[ch][sb] - bitslice;
					if (bits[ch][sb] > 16)
						bits[ch][sb] = 16;
				}
			}

			for(sb = 0; bitcount < frame->bitpool && sb < subbands; sb++)
			{
				if((bits[ch][sb] >= 2) && (bits[ch][sb] < 16))
				{
					bits[ch][sb]++;
					bitcount++;
				}
				else if((bitneed[ch][sb] == bitslice + 1) && (frame->bitpool > bitcount + 1))
				{
					bits[ch][sb] = 2;
					bitcount += 2;
				}
			}

			for(sb = 0; bitcount < frame->bitpool && sb < subbands; sb++)
			{
				if(bits[ch][sb] < 16)
				{
					bits[ch][sb]++;
					bitcount++;
				}
			}
		} 
	}
	else if (frame->mode == STEREO || frame->mode == JOINT_STEREO) {
		int bitneed[2][8], loudness, max_bitneed, bitcount, slicecount, bitslice;
		int ch, sb;

		max_bitneed = 0;
		if (frame->allocation == SNR) {
			for (ch = 0; ch < 2; ch++) {
				for (sb = 0; sb < subbands; sb++) {
					bitneed[ch][sb] = frame->scale_factor[ch][sb];
					if (bitneed[ch][sb] > max_bitneed)
						max_bitneed = bitneed[ch][sb];
				}
			}
		} else {
			for (ch = 0; ch < 2; ch++) {
				for (sb = 0; sb < subbands; sb++) {
					if (frame->scale_factor[ch][sb] == 0)
						bitneed[ch][sb] = -5;
					else {
						if (subbands == 4)
							loudness = frame->scale_factor[ch][sb] - sbc_offset4[sf][sb];
						else
							loudness = frame->scale_factor[ch][sb] - sbc_offset8[sf][sb];
						if (loudness > 0)
							bitneed[ch][sb] = loudness / 2;
						else
							bitneed[ch][sb] = loudness;
					}
					if (bitneed[ch][sb] > max_bitneed)
						max_bitneed = bitneed[ch][sb];
				}
			}
		}

		bitcount = 0;
		slicecount = 0;
		bitslice = max_bitneed + 1;
		do {
			bitslice--;
			bitcount += slicecount;
			slicecount = 0;
			for (ch = 0; ch < 2; ch++) {
				for (sb = 0; sb < subbands; sb++) {
					if ((bitneed[ch][sb] > bitslice + 1) && (bitneed[ch][sb] < bitslice + 16))
						slicecount++;
					else if (bitneed[ch][sb] == bitslice + 1)
						slicecount += 2;
				}
			}
		} while (bitcount + slicecount < frame->bitpool);

		if (bitcount + slicecount == frame->bitpool) {
			bitcount += slicecount;
			bitslice--;
		}

		for (ch = 0; ch < 2; ch++) {
			for (sb = 0; sb < subbands; sb++) {
				if (bitneed[ch][sb] < bitslice + 2) {
					bits[ch][sb] = 0;
				} else {
					bits[ch][sb] = bitneed[ch][sb] - bitslice;
					if (bits[ch][sb] > 16)
						bits[ch][sb] = 16;
				}
			}
		}

		ch = 0;
		sb = 0;
		while (bitcount < frame->bitpool) {
			if ((bits[ch][sb] >= 2) && (bits[ch][sb] < 16)) {
				bits[ch][sb]++;
				bitcount++;
			} else if ((bitneed[ch][sb] == bitslice + 1) && (frame->bitpool > bitcount + 1)) {
				bits[ch][sb] = 2;
				bitcount += 2;
			}
			if (ch == 1) {
				ch = 0;
				sb++;
				if (sb >= subbands)
					break;
			} else
				ch = 1;
		}

		ch = 0;
		sb = 0;
		while (bitcount < frame->bitpool) {
			if (bits[ch][sb] < 16) {
				bits[ch][sb]++;
				bitcount++;
			}
			if (ch == 1) {
				ch = 0;
				sb++;
				if (sb >= subbands)
					break;
			} else
				ch = 1;
		}
	} else {
		printf("other\r\n");
	}

}

static void sbc_calculate_bits(const sbc_frame *frame, int (*bits)[8])
{
	if (frame->subbands == 4)
		sbc_calculate_bits_internal(frame, bits, 4);
	else
		sbc_calculate_bits_internal(frame, bits, 8);
}

static  int16_t sbc_clip16(int32_t s)
{
	if (s > 0x7FFF)    //32767
		return 0x7FFF;
	else if (s < -0x8000)  //-32768
		return -0x8000;
	else
		return s;
}

static int sbc_analyze_audio(sbc_encoder_state *state, sbc_frame *frame)
{
	int ch, blk;

	switch (frame->subbands) {
	case 4:
		for (ch = 0; ch < frame->channels; ch++)
			for (blk = 0; blk < frame->blocks; blk++)
				sbc_analyze_four(state, frame, ch, blk);
		return frame->blocks * 4;

	case 8:
		for (ch = 0; ch < frame->channels; ch++)
			for (blk = 0; blk < frame->blocks; blk++)
				sbc_analyze_eight(state, frame, ch, blk);
		return frame->blocks * 8;

	default:
		return -EIO;
	}
}

/* Supplementary bitstream writing macros for 'sbc_pack_frame' */
#define PUT_BITS(data_ptr, bits_cache, bits_count, v, n)		\
	do {								\
		bits_cache = (v) | (bits_cache << (n));			\
		bits_count += (n);					    \
		if (bits_count >= 16) {					\
			bits_count -= 8;				\
			*data_ptr++ = (uint8_t)				\
				(bits_cache >> bits_count); 		\
			bits_count -= 8;				\
			*data_ptr++ = (uint8_t)				\
				(bits_cache >> bits_count);			\
		}							\
	} while (0)

#define FLUSH_BITS(data_ptr, bits_cache, bits_count)			\
	do {								\
		while (bits_count >= 8) {				\
			bits_count -= 8;				\
			*data_ptr++ = (uint8_t)				\
				(bits_cache >> bits_count);		\
		}							\
		if (bits_count > 0)					\
			*data_ptr++ = (uint8_t)				\
				(bits_cache << (8 - bits_count));	\
	} while (0)


/*
 * Packs the SBC frame from frame into the memory at data. At most len
 * bytes will be used, should more memory be needed an appropriate
 * error code will be returned. Returns the length of the packed frame
 * on success or a negative value on error.
 *
 * The error codes are:
 * -1 Not enough memory reserved
 * -2 Unsupported sampling rate
 * -3 Unsupported number of blocks
 * -4 Unsupported number of subbands
 * -5 Bitpool value out of bounds
 * -99 not implemented
 */

static  int sbc_pack_frame_internal(uint8_t *data,
									sbc_frame *frame, 
									int len,
									int frame_subbands, 
									int frame_channels,
									int joint)
{
	/* Bitstream writer starts from the fourth byte */
	//
	uint8_t *data_ptr = data + 4;
	uint32_t bits_cache = 0;
	uint32_t bits_count = 0;

	/* Will copy the header parts for CRC-8 calculation here */
	uint8_t crc_header[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int crc_pos = 0;

	uint32_t audio_sample;

	int ch, sb, blk;	/* channel, subband, block and bit counters */
	int bits[2][8];		/* bits distribution */
	uint32_t levels[2][8];	/* levels are derived from that */
	uint32_t sb_sample_delta[2][8];

	/* Can't fill in crc yet */

	crc_header[0] = data[1];
	crc_header[1] = data[2];
	crc_pos = 16;

	if (frame->mode == JOINT_STEREO) {
		PUT_BITS(data_ptr, bits_cache, bits_count,
			joint, frame_subbands);
		crc_header[crc_pos >> 3] = joint;
		crc_pos += frame_subbands;
	}

	for (ch = 0; ch < frame_channels; ch++) {
		for (sb = 0; sb < frame_subbands; sb++) {
			PUT_BITS(data_ptr, bits_cache, bits_count,
				frame->scale_factor[ch][sb] & 0x0F, 4);
			crc_header[crc_pos >> 3] <<= 4;
			crc_header[crc_pos >> 3] |= frame->scale_factor[ch][sb] & 0x0F;
			crc_pos += 4;
		}
	}

	/* align the last crc byte */
	if (crc_pos % 8)
		crc_header[crc_pos >> 3] <<= 8 - (crc_pos % 8);

	data[3] = sbc_crc8(crc_header, crc_pos);

	/* calculate levels*/
	for (ch = 0; ch < frame_channels; ch++) {
		for (sb = 0; sb < frame_subbands; sb++) {
			levels[ch][sb] = ((1 << bits[ch][sb]) - 1) <<
				(32 - (frame->scale_factor[ch][sb] +
					SCALE_OUT_BITS + 2));
			sb_sample_delta[ch][sb] = (uint32_t) 1 <<
				(frame->scale_factor[ch][sb] +
					SCALE_OUT_BITS + 1);
		}
	}

	//quantization
	for (blk = 0; blk < frame->blocks; blk++) {
		for (ch = 0; ch < frame_channels; ch++) {
			for (sb = 0; sb < frame_subbands; sb++) {

				if (bits[ch][sb] == 0) {
					continue;
				}

				audio_sample = ((uint64_t) levels[ch][sb] *
					(sb_sample_delta[ch][sb] +
					frame->sb_sample_f[blk][ch][sb])) >> 32;

				PUT_BITS(data_ptr, bits_cache, bits_count,
					audio_sample, bits[ch][sb]);
			}
		}
	}
	
	FLUSH_BITS(data_ptr, bits_cache, bits_count);
	return data_ptr - data;
}

static int sbc_pack_frame(uint8_t *data, sbc_frame *frame, int len)
{
	int produced;
	/* Will copy the header parts for CRC-8 calculation here */
	uint8_t crc_header[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int crc_pos = 0;

	uint16_t audio_sample;

	int ch, sb, blk, bit;	/* channel, subband, block and bit counters */
	int bits[2][8];		/* bits distribution */
	int levels[2][8];	/* levels are derived from that */

	uint32_t scalefactor[2][8];	/* derived from frame->scale_factor */

	data[0] = SBC_SYNCWORD;

	data[1] = (frame->frequency & 0x03) << 6;

	data[1] |= (frame->block_mode & 0x03) << 4;

	data[1] |= (frame->mode & 0x03) << 2;

	data[1] |= (frame->allocation & 0x01) << 1;

	switch (frame->subbands) {
	case 4:
		/* Nothing to do */
		break;
	case 8:
		data[1] |= 0x01;
		break;
	default:
		return -4;
		break;
	}

	data[2] = frame->bitpool;

	if ((frame->mode == SBC_MODE_MONO || frame->mode == SBC_MODE_DUAL_CHANNEL) &&
			frame->bitpool > frame->subbands << 4)
		return -5;

	if ((frame->mode == SBC_MODE_STEREO || frame->mode == SBC_MODE_JOINT_STEREO) &&
			frame->bitpool > frame->subbands << 5)
		return -5;

	/* Can't fill in crc yet */

	produced = 32;
	crc_header[0] = data[1];
	crc_header[1] = data[2];
	crc_pos = 16;

	//scale_factor计算结果相对于官方编码多了7
	printf("[scale_factor]=(");
	for (ch = 0; ch < frame->channels; ch++) {
		for (sb = 0; sb < frame->subbands; sb++) {
			frame->scale_factor[ch][sb] = 0;
			scalefactor[ch][sb] = 2;
			for (blk = 0; blk < frame->blocks; blk++) {
				while (scalefactor[ch][sb] < fabs(frame->sb_sample_f[blk][ch][sb])) {
					frame->scale_factor[ch][sb]++;
					scalefactor[ch][sb] *= 2;
				}
			}
			//+
			//frame->scale_factor[ch][sb] -= 7;
			printf("%d, ", frame->scale_factor[ch][sb]);
		}
		printf(")\r\n");
	}

	//STEP2
	if (frame->mode == SBC_MODE_JOINT_STEREO) {
		/* like frame->sb_sample but joint stereo */
		int32_t sb_sample_j[16][2];
		/* scalefactor and scale_factor in joint case */
		uint32_t scalefactor_j[2];
		uint8_t scale_factor_j[2];

		frame->joint = 0;

		for (sb = 0; sb < frame->subbands - 1; sb++) {
			scale_factor_j[0] = 0;
			scalefactor_j[0] = 2;
			scale_factor_j[1] = 0;
			scalefactor_j[1] = 2;

			for (blk = 0; blk < frame->blocks; blk++) {
				/* Calculate joint stereo signal */
				sb_sample_j[blk][0] =
					(frame->sb_sample_f[blk][0][sb] +
						frame->sb_sample_f[blk][1][sb]) >> 1;
				sb_sample_j[blk][1] =
					(frame->sb_sample_f[blk][0][sb] -
						frame->sb_sample_f[blk][1][sb]) >> 1;

				/* calculate scale_factor_j and scalefactor_j for joint case */
				while (scalefactor_j[0] < fabs(sb_sample_j[blk][0])) {
					scale_factor_j[0]++;
					scalefactor_j[0] *= 2;
				}
				while (scalefactor_j[1] < fabs(sb_sample_j[blk][1])) {
					scale_factor_j[1]++;
					scalefactor_j[1] *= 2;
				}
			}

			/* decide whether to join this subband */
			if ((scalefactor[0][sb] + scalefactor[1][sb]) >
					(scalefactor_j[0] + scalefactor_j[1]) ) {
				/* use joint stereo for this subband */
				frame->joint |= 1 << sb;
				frame->scale_factor[0][sb] = scale_factor_j[0];
				frame->scale_factor[1][sb] = scale_factor_j[1];
				scalefactor[0][sb] = scalefactor_j[0];
				scalefactor[1][sb] = scalefactor_j[1];
				for (blk = 0; blk < frame->blocks; blk++) {
					frame->sb_sample_f[blk][0][sb] =
							sb_sample_j[blk][0];
					frame->sb_sample_f[blk][1][sb] =
							sb_sample_j[blk][1];
				}
			}
		}

		data[4] = 0;
		for (sb = 0; sb < frame->subbands - 1; sb++)
			data[4] |= ((frame->joint >> sb) & 0x01) << (frame->subbands - 1 - sb);

		crc_header[crc_pos >> 3] = data[4];

		produced += frame->subbands;
		crc_pos += frame->subbands;
	}

	for (ch = 0; ch < frame->channels; ch++) {
		for (sb = 0; sb < frame->subbands; sb++) {
			data[produced >> 3] <<= 4;
			crc_header[crc_pos >> 3] <<= 4;
			data[produced >> 3] |= frame->scale_factor[ch][sb] & 0x0F;
			crc_header[crc_pos >> 3] |= frame->scale_factor[ch][sb] & 0x0F;
			produced += 4;
			crc_pos += 4;
		}
	}

	/* align the last crc byte */
	if (crc_pos % 8)
		crc_header[crc_pos >> 3] <<= 8 - (crc_pos % 8);

	data[3] = sbc_crc8(crc_header, crc_pos);
	printf("[----bits----]=(");
	for (ch = 0; ch < frame->channels; ch++) {
		for (sb = 0; sb < frame->subbands; sb++) {
			printf("%d, ", bits[ch][sb]);
		}
		printf(")\r\n");
	}

	sbc_calculate_bits(frame, bits);

	printf("[---levels---]=(");
	for (ch = 0; ch < frame->channels; ch++)
	{
		for (sb = 0; sb < frame->subbands; sb++)
		{
			levels[ch][sb] = (1 << bits[ch][sb]) - 1;
			printf("%d, ", levels[ch][sb]);
		}
		printf(")\r\n");
	}
	printf("\r\n");


	for (blk = 0; blk < frame->blocks; blk++) {
		for (ch = 0; ch < frame->channels; ch++) {
			for (sb = 0; sb < frame->subbands; sb++) {
				if (levels[ch][sb] > 0) {
				audio_sample =
					(uint16_t) ((((frame->sb_sample_f[blk][ch][sb]*levels[ch][sb]) >>
									(frame->scale_factor[ch][sb] + 1)) +
								levels[ch][sb]) >> 1);
					audio_sample <<= 16 - bits[ch][sb];
					for (bit = 0; bit < bits[ch][sb]; bit++) {
						data[produced >> 3] <<= 1;
						if (audio_sample & 0x8000)
							data[produced >> 3] |= 0x1;
						audio_sample <<= 1;
						produced++;
					}
				 }
			}
		}
	}

	/* align the last byte */
	if (produced % 8) {
		data[produced >> 3] <<= 8 - (produced % 8);
	}

	return (produced + 7) >> 3;
}


static void sbc_encoder_init(sbc_encoder_state *state, const sbc_frame *frame)
{
	memset(&state->X, 0, sizeof(state->X));
	state->subbands = frame->subbands;
	state->position[0] = state->position[1] = 9 * frame->subbands;
}

struct sbc_priv {
	bool init;
	sbc_frame frame;
	sbc_encoder_state enc_state;
};

static void sbc_set_defaults(sbc_t *sbc, uint8_t flags)
{
	struct sbc_priv *priv = sbc->priv;

	sbc->flags = flags;
	sbc->frequency = SBC_FREQ_16000;
	sbc->mode = SBC_MODE_MONO;
	sbc->channels = ((sbc->mode == SBC_MODE_MONO) ? 1 : 2);
	sbc->subbands = SBC_SB_4;
	sbc->blocks = SBC_BLK_16;
    sbc->allocation = SBC_AM_SNR;
	sbc->bitpool = 9;
// #if __BYTE_ORDER == __LITTLE_ENDIAN
	sbc->endian = SBC_LE;
// #elif __BYTE_ORDER == __BIG_ENDIAN
	// sbc->endian = SBC_BE;
// #else
// #error "Unknown byte order"
// #endif
}

int sbc_enc_init(sbc_t *sbc,  sbc_encode_output_cb_f callback, uint8_t flags)
{
	if (!sbc)
		return -EIO;

	memset(sbc, 0, sizeof(sbc_t));

	sbc->priv_alloc_base = malloc(sizeof(struct sbc_priv) + SBC_ALIGN_MASK);
	if (!sbc->priv_alloc_base)
		return -ENOMEM;

	sbc->priv = (void *) (((uintptr_t) sbc->priv_alloc_base +
			SBC_ALIGN_MASK) & ~((uintptr_t) SBC_ALIGN_MASK));

	memset(sbc->priv, 0, sizeof(struct sbc_priv));

	sbc_set_defaults(sbc, flags);

	sbc->callback = callback;

	return 0;
}

static inline void sbc_analyze_four(sbc_encoder_state *state,
									sbc_frame *frame, 
									int ch, 
									int blk)
{
	int32_t *x = &state->X[ch][state->position[ch]];
	int16_t *pcm = &frame->pcm_sample[ch][blk * 4];

	/* Input 4 Audio Samples */
	x[40] = x[0] = pcm[3];
	x[41] = x[1] = pcm[2];
	x[42] = x[2] = pcm[1];
	x[43] = x[3] = pcm[0];

	__sbc_analyze_four(x, frame->sb_sample_f[blk][ch]);

	state->position[ch] -= 4;
	if (state->position[ch] < 0)
		state->position[ch] = 36;
}

void sbc_analyze_eight(sbc_encoder_state *state,
					   sbc_frame *frame, 
		   			   int ch,
		   			   int blk)
{
	int32_t *x = &state->X[ch][state->position[ch]];
	int16_t *pcm = &frame->pcm_sample[ch][blk * 8];

	/* Input 8 Audio Samples */
	x[80] = x[0] = pcm[7];
	x[81] = x[1] = pcm[6];
	x[82] = x[2] = pcm[5];
	x[83] = x[3] = pcm[4];
	x[84] = x[4] = pcm[3];
	x[85] = x[5] = pcm[2];
	x[86] = x[6] = pcm[1];
	x[87] = x[7] = pcm[0];

	__sbc_analyze_eight(x, frame->sb_sample_f[blk][ch]);

	state->position[ch] -= 8;
	if (state->position[ch] < 0)
		state->position[ch] = 72;
}

static int sbc_clz(uint32_t x)
{
#ifdef __GNUC__
	return __builtin_clz(x);
#else
	/* TODO: this should be replaced with something better if good
	 * performance is wanted when using compilers other than gcc */
	int cnt = 0;
	while (x) {
		cnt++;
		x >>= 1;
	}
	return 32 - cnt;
#endif
}

static void sbc_calc_scalefactors(int32_t sb_sample_f[16][2][8],
								  uint32_t scale_factor[2][8],
								  int blocks, 
								  int channels, 
								  int subbands)
{
	int ch, sb, blk;
	for (ch = 0; ch < channels; ch++) {
		for (sb = 0; sb < subbands; sb++) {
			uint32_t x = 1 << SCALE_OUT_BITS;
			for (blk = 0; blk < blocks; blk++) {
				int32_t tmp = fabs(sb_sample_f[blk][ch][sb]);
				if (tmp != 0)
					x |= tmp - 1;
			}
			scale_factor[ch][sb] = (31 - SCALE_OUT_BITS) -
				sbc_clz(x);
		}
	}
}

static uint64_t pack_timer_tick_ms = 0;
static uint64_t now = 0;

int sbc_encode(sbc_t *sbc, 
			   void *input, 
			   int input_len,
			   void *output, 
			   int output_len)
{
	struct sbc_priv *priv;
	int samples, ch;
	int  framelen;
	int16_t *ptr;

	if (!sbc || !input)
		return -EIO;

	priv = sbc->priv;

	//打印输入数组
	// for(int i=0;i<input_len;i++)
	// {
	// 	printf("[%d] ",*((int16_t *)input + i));
	// }
	// printf("\n");

	if (!priv->init) {
		priv->frame.frequency = sbc->frequency;
		priv->frame.mode = sbc->mode;
		priv->frame.channels = sbc->mode == SBC_MODE_MONO ? 1 : 2;
		priv->frame.allocation = sbc->allocation;
		priv->frame.subband_mode = sbc->subbands;
		priv->frame.subbands = sbc->subbands ? 8 : 4;
		priv->frame.block_mode = sbc->blocks;
		priv->frame.blocks = 4 + (sbc->blocks * 4);
		priv->frame.bitpool = sbc->bitpool;
		priv->frame.frame_count = 0;
		priv->frame.codesize = sbc_get_codesize(sbc);
		priv->frame.length = sbc_get_frame_length(sbc);
		sbc_encoder_init(&priv->enc_state, &priv->frame);
		priv->init = true;
	} else if (priv->frame.bitpool != sbc->bitpool) {
		//ref section 12.9 of A2DP 
		priv->frame.length = sbc_get_frame_length(sbc);
		priv->frame.bitpool = sbc->bitpool;
	}

	/* input must be large enough to encode a complete frame */
	if (input_len < priv->frame.codesize)
		return 0;

	/* output must be large enough to receive the encoded frame */
	if (!output || output_len < priv->frame.length)
		return -ENOSPC;

	ptr = (int16_t *)input;

	for (int i = 0; i < priv->frame.subbands * priv->frame.blocks; i++) {
		for (ch = 0; ch < sbc->channels; ch++) {
			int16_t s;

			if (sbc->endian == SBC_LE)
				s = ((ptr[0] & 0x00ff) << 8) | ((ptr[0] & 0xff00) >> 8);
			else
				s = ptr[0];
			ptr++;

			priv->frame.pcm_sample[ch][i] = s;
		}
	}

	samples = sbc_analyze_audio(&priv->enc_state, &priv->frame);
	framelen = sbc_pack_frame(output,&priv->frame, output_len); 

	return samples * priv->frame.channels * 2;
}

void sbc_finish(sbc_t *sbc)
{
	if (!sbc)
		return;

	free(sbc->priv_alloc_base);
	memset(sbc, 0, sizeof(sbc_t));
}

int sbc_get_frame_length(sbc_t *sbc)
{
	int ret;
	uint8_t subbands, channels, blocks, joint, bitpool;
	struct sbc_priv *priv;

	priv = sbc->priv;

	if (priv->init && priv->frame.bitpool == sbc->bitpool)
		return priv->frame.length;

	subbands = sbc->subbands ? 8 : 4;
	blocks = 4 + (sbc->blocks * 4);
	channels = sbc->mode == SBC_MODE_MONO ? 1 : 2;
	joint = sbc->mode == SBC_MODE_JOINT_STEREO ? 1 : 0;
	bitpool = sbc->bitpool;

	ret = 4 + (4 * subbands * channels) / 8;
	/* This term is not always evenly divide so we round it up */
	if (channels == 1 || sbc->mode == SBC_MODE_DUAL_CHANNEL)
		//''MONO' or 'DUAL_CHANNEL'
		ret += ((blocks * channels * bitpool) + 7) / 8;
	else
		//'STEREO' or 'JOINT STEREO'
		ret += (((joint ? subbands : 0) + blocks * bitpool) + 7) / 8;

	return ret;
}

int sbc_get_codesize(sbc_t *sbc)
{
	uint16_t subbands, channels, blocks;
	struct sbc_priv *priv;
	
	priv = sbc->priv;

	if (!priv->init) {
		subbands = sbc->subbands ? 8 : 4;
        blocks = 4 + (sbc->blocks * 4);
		channels = sbc->mode == SBC_MODE_MONO ? 1 : 2;
	} else {
		subbands = priv->frame.subbands;
		blocks = priv->frame.blocks;
		channels = priv->frame.channels;
	}

	return subbands * blocks * channels * 2;
}

int sbc_reinit(sbc_t *sbc, uint8_t flags)
{
	struct sbc_priv *priv;

	if (!sbc || !sbc->priv)
		return -EIO;

	priv = sbc->priv;

	if (priv->init)
		memset(sbc->priv, 0, sizeof(struct sbc_priv));

	sbc_set_defaults(sbc, flags);

	return 0;
}

static void __sbc_analyze_four(const int32_t *in, int32_t *out)
{
	sbc_fixed_t t[8], s[5];

	t[0] = SCALE4_STAGE1(MULA(_sbc_proto_4[0], in[8] - in[32],
						 MUL( _sbc_proto_4[1], in[16] - in[24])));

	t[1] = SCALE4_STAGE1(MULA(_sbc_proto_4[2], in[1],
						 MULA(_sbc_proto_4[3], in[9],
						 MULA(_sbc_proto_4[4], in[17],
						 MULA(_sbc_proto_4[5], in[25],
						 MUL( _sbc_proto_4[6], in[33]))))));

	t[2] = SCALE4_STAGE1(MULA(_sbc_proto_4[7], in[2],
						 MULA(_sbc_proto_4[8], in[10],
						 MULA(_sbc_proto_4[9], in[18],
						 MULA(_sbc_proto_4[10], in[26],
						 MUL( _sbc_proto_4[11], in[34]))))));

	t[3] = SCALE4_STAGE1(MULA(_sbc_proto_4[12], in[3],
						 MULA(_sbc_proto_4[13], in[11],
						 MULA(_sbc_proto_4[14], in[19],
						 MULA(_sbc_proto_4[15], in[27],
						 MUL( _sbc_proto_4[16], in[35]))))));

	t[4] = SCALE4_STAGE1(MULA(_sbc_proto_4[17], in[4] + in[36],
						 MULA(_sbc_proto_4[18], in[12] + in[28],
						 MUL( _sbc_proto_4[19], in[20]))));

	t[5] = SCALE4_STAGE1(MULA(_sbc_proto_4[16], in[5],
						 MULA(_sbc_proto_4[15], in[13],
						 MULA(_sbc_proto_4[14], in[21],
						 MULA(_sbc_proto_4[13], in[29],
						 MUL( _sbc_proto_4[12], in[37]))))));

	/* don't compute t[6]... this term always multiplies
	 * with cos(pi/2) = 0 */

	t[7] = SCALE4_STAGE1(MULA(_sbc_proto_4[6], in[7],
						 MULA(_sbc_proto_4[5], in[15],
						 MULA(_sbc_proto_4[4], in[23],
						 MULA(_sbc_proto_4[3], in[31],
						 MUL( _sbc_proto_4[2], in[39]))))));

	s[0] = MUL( _anamatrix4[0], t[0] + t[4]);
	s[1] = MUL( _anamatrix4[2], t[2]);
	s[2] = MULA(_anamatrix4[1], t[1] + t[3], MUL(_anamatrix4[3], t[5]));
	s[3] = MULA(_anamatrix4[3], t[1] + t[3], MUL(_anamatrix4[1], -t[5] + t[7]));
	s[4] = MUL( _anamatrix4[3], t[7]);

	out[0] = SCALE4_STAGE2( s[0] + s[1] + s[2] - s[4]); /* Q0 */
	out[1] = SCALE4_STAGE2(-s[0] + s[1] + s[3]);
	out[2] = SCALE4_STAGE2(-s[0] + s[1] - s[3]);
	out[3] = SCALE4_STAGE2( s[0] + s[1] - s[2] + s[4]);
}

static void __sbc_analyze_eight(const int32_t *in, int32_t *out)
{
	sbc_fixed_t t[8], s[10];

	t[2] = SCALE8_STAGE1(MULA(_sbc_proto_8[6], in[2],
						 MULA(_sbc_proto_8[7], in[18],
		                 MULA(_sbc_proto_8[8], in[34],
		                 MULA(_sbc_proto_8[9], in[50],
		                 MULA(_sbc_proto_8[10], in[66],
		                 MULA(_sbc_proto_8[26], in[6],
		                 MULA(_sbc_proto_8[27], in[22],
		                 MULA(_sbc_proto_8[28], in[38],
		                 MULA(_sbc_proto_8[29], in[54],
		                 MUL( _sbc_proto_8[30], in[70])))))))))));

	t[4] = SCALE8_STAGE1(MULA(_sbc_proto_8[2], in[4],
		                 MULA(_sbc_proto_8[3], in[20],
		                 MULA(_sbc_proto_8[4], in[36],
		                 MULA(_sbc_proto_8[5], in[52],
		                 MUL(_sbc_proto_8[39], in[68]))))));

	t[3] = SCALE8_STAGE1(MULA(_sbc_proto_8[16], in[3],
		                 MULA(_sbc_proto_8[17], in[19],
		                 MULA(_sbc_proto_8[18], in[35],
		                 MULA(_sbc_proto_8[19], in[51],
		                 MULA(_sbc_proto_8[20], in[67],
		                 MULA(_sbc_proto_8[21], in[5],
		                 MULA(_sbc_proto_8[22], in[21],
		                 MULA(_sbc_proto_8[23], in[37],
		                 MULA(_sbc_proto_8[24], in[53],
		                 MUL( _sbc_proto_8[25], in[69])))))))))));

	t[1] = SCALE8_STAGE1(MULA( _sbc_proto_8[11], in[1],
		                 MULA( _sbc_proto_8[12], in[17],
		                 MULA( _sbc_proto_8[13], in[33],
		                 MULA( _sbc_proto_8[14], in[49],
		                 MULA( _sbc_proto_8[15], in[65],
		                 MULA( _sbc_proto_8[31], in[7],
		                 MULA( _sbc_proto_8[32], in[23],
		                 MULA( _sbc_proto_8[33], in[39],
		                 MULA( _sbc_proto_8[34], in[55],
		                 MUL(  _sbc_proto_8[35], in[71])))))))))));

	t[0] = SCALE8_STAGE1(MULA( _sbc_proto_8[0], (in[16] - in[64]),
		                 MULA( _sbc_proto_8[1], (in[32] - in[48]),
		                 MULA( _sbc_proto_8[36],(in[8] + in[72]),
		                 MULA( _sbc_proto_8[37],(in[24] + in[56]),
		                 MUL(  _sbc_proto_8[38], in[40]))))));

	t[5] = SCALE8_STAGE1(MULA( _sbc_proto_8[35], in[9],
		                 MULA( _sbc_proto_8[34], in[25],
		                 MULA( _sbc_proto_8[33], in[41],
		                 MULA( _sbc_proto_8[32], in[57],
		                 MULA( _sbc_proto_8[31], in[73],
		                 MULA(-_sbc_proto_8[15], in[15],
		                 MULA(-_sbc_proto_8[14], in[31],
		                 MULA(-_sbc_proto_8[13], in[47],
		                 MULA(-_sbc_proto_8[12], in[63],
		                 MUL( -_sbc_proto_8[11], in[79])))))))))));

	t[6] = SCALE8_STAGE1(MULA( _sbc_proto_8[30], in[10],
		                 MULA( _sbc_proto_8[29], in[26],
		                 MULA( _sbc_proto_8[28], in[42],
		                 MULA( _sbc_proto_8[27], in[58],
		                 MULA( _sbc_proto_8[26], in[74],
		                 MULA(-_sbc_proto_8[10], in[14],
		                 MULA(-_sbc_proto_8[9], in[30],
		                 MULA(-_sbc_proto_8[8], in[46],
		                 MULA(-_sbc_proto_8[7], in[62],
		                 MUL( -_sbc_proto_8[6], in[78])))))))))));

	t[7] = SCALE8_STAGE1(MULA( _sbc_proto_8[25], in[11],
		                 MULA( _sbc_proto_8[24], in[27],
		                 MULA( _sbc_proto_8[23], in[43],
		                 MULA( _sbc_proto_8[22], in[59],
		                 MULA( _sbc_proto_8[21], in[75],
		                 MULA(-_sbc_proto_8[20], in[13],
		                 MULA(-_sbc_proto_8[19], in[29],
		                 MULA(-_sbc_proto_8[18], in[45],
		                 MULA(-_sbc_proto_8[17], in[61],
		                 MUL( -_sbc_proto_8[16], in[77])))))))))));

		s[0] = MUL(_anamatrix8[7],t[4]); 
		s[1] = MUL(_anamatrix8[6],t[0]);	
        s[2] = MUL(_anamatrix8[0],t[2]);  // 0 2
		s[3] = MUL(_anamatrix8[1],t[6]);  // 1 6	
		s[8] = MUL(_anamatrix8[1],t[2]); //  1 2
		s[9] = MUL(_anamatrix8[0],t[6]); // 0 6

		s[4] = MULA( _anamatrix8[2], t[3], MULA( _anamatrix8[3], t[1],
		            					   MULA( _anamatrix8[4], t[5],
		            					   MUL(  _anamatrix8[5], t[7]))));										   
		s[5] = MULA(-_anamatrix8[2], t[5], MULA( _anamatrix8[3], t[3],
		            					   MULA(-_anamatrix8[4], t[7],
		            					   MUL( -_anamatrix8[5], t[1]))));
		s[6] = MULA( _anamatrix8[4], t[3], MULA( -_anamatrix8[2], t[1],
		            					   MULA( _anamatrix8[5], t[5],
		            					   MUL(  _anamatrix8[3], t[7]))));
		s[7] = MULA(-_anamatrix8[2], t[7], MULA( _anamatrix8[3], t[5],
		            					   MULA(-_anamatrix8[4], t[1],
		            					   MUL(  _anamatrix8[5], t[3]))));
        
		out[0] = SCALE8_STAGE2( (s[0] + s[1]) + (s[2] + s[3]) + s[4]);     
		out[1] = SCALE8_STAGE2( (s[0] - s[1]) + (s[8] - s[9]) + s[5]);
        out[2] = SCALE8_STAGE2( (s[0] - s[1]) - s[8] + (s[9] + s[6]));
        out[3] = SCALE8_STAGE2( (s[0] + s[1]) - (s[2] + s[3]) + s[7]);
        out[4] = SCALE8_STAGE2( (s[0] + s[1]) - (s[2] + s[3]) - s[7]);
        out[5] = SCALE8_STAGE2( (s[0] - s[1]) - s[8] + (s[9] - s[6]));
        out[6] = SCALE8_STAGE2( (s[0] - s[1]) + (s[8] - s[9]) - s[5]);	
		out[7] = SCALE8_STAGE2( (s[0] + s[1]) + (s[2] + s[3]) - s[4] );
}
