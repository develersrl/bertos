/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief WAV audio utils.
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "wav"
 */

#ifndef  ALGO_WAV_H
#define  ALGO_WAV_H

#include <cfg/debug.h>

#include <cpu/types.h>
#include <cpu/byteorder.h>

#include <string.h>


typedef struct WavHdr
{
	char chunk_id[4];
	uint32_t chunk_size;
	char format[4];

	uint8_t subchunk1_id[4];
	uint32_t subchunk1_size;
	uint16_t audio_format;
	uint16_t num_channels;
	uint32_t sample_rate;
	uint32_t byte_rate;
	uint16_t block_align;
	uint16_t bits_per_sample;

	uint8_t subchunk2_id[4];
	uint32_t subchunk2_size;

} PACKED WavHdr;

INLINE int wav_checkHdr(WavHdr *wav, uint16_t audio_format, uint16_t num_channels, uint16_t sample_rate, uint16_t bits_per_sample)
{
	ASSERT(wav);

	if (strncmp(wav->chunk_id, "RIFF", 4))
	{
		kputs("RIFF tag not found\n");
		goto error;
	}

	if (strncmp(wav->format, "WAVE", 4))
	{
		kputs("WAVE tag not found\n");
		goto error;
	}

	if (le16_to_cpu(wav->audio_format) != audio_format)
	{
		kprintf("Audio format not valid, found [%d]\n", le16_to_cpu(wav->audio_format));
		goto error;
	}

	if (le16_to_cpu(wav->num_channels) != num_channels)
	{
		kprintf("Channels number not valid, found [%d]\n", le16_to_cpu(wav->num_channels));
		goto error;
	}


	if (le32_to_cpu(wav->sample_rate) != sample_rate)
	{
		kprintf("Sample rate not valid, found [%ld]\n", le32_to_cpu(wav->sample_rate));
		goto error;
	}

	if (le16_to_cpu(wav->bits_per_sample) != bits_per_sample)
	{
		kprintf("Bits per sample not valid, found [%d]\n", le16_to_cpu(wav->bits_per_sample));
		goto error;
	}
	return 0;

error:
	return -1;
}


INLINE void wav_writeHdr(WavHdr *wav, size_t sample_num, uint16_t audio_format, uint16_t num_channels, uint32_t sample_rate, uint16_t bits_per_sample)
{
	ASSERT(wav);

	uint32_t sub2 = sample_num * num_channels * bits_per_sample / 8;

	memcpy(&wav->chunk_id, "RIFF", 4);
	memcpy(&wav->format, "WAVE", 4);

	memcpy(&wav->subchunk1_id, "fmt ", 4);
	wav->subchunk1_size = cpu_to_le32(16);

	memcpy(&wav->subchunk2_id, "data", 4);
	wav->subchunk2_size = cpu_to_le32(sub2);

	wav->audio_format = cpu_to_le16(audio_format);
	wav->num_channels = cpu_to_le16(num_channels);
	wav->sample_rate = cpu_to_le32(sample_rate);
	wav->bits_per_sample = cpu_to_le16(bits_per_sample);

	wav->chunk_size = cpu_to_le32(36 + sub2);
	kprintf("CHUNK_SIZE %ld\n", 36 + sub2);
}

#endif /* ALGO_WAV_H */
