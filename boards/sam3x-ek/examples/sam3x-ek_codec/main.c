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
 * \author Stefano Federico <aleph@develer.com>
 *
 * \brief Empty project.
 *
 * This is a minimalist project, it just initializes the hardware of the
 * supported board and proposes an empty main.
 */

#include "hw/hw_led.h"
#include "hw/hw_sd.h"
#include "cfg/cfg_i2s.h"

#include <cfg/debug.h>

#include <cpu/irq.h>
#include <cpu/byteorder.h>

#include <io/kfile.h>
#include <io/kfile_block.h>

#include <drv/i2c.h>
#include <drv/i2s.h>
#include <drv/sd.h>
#include <drv/eeprom.h>
#include <drv/timer.h>
#include <drv/lcd_hx8347.h>
#include <drv/adc.h>
#include <drv/wm8731.h>
#include <drv/dmac_sam3.h>

#include <fs/fat.h>

#include <verstag.h>
#include <buildrev.h>

#include <string.h>

/*
 * Codec has 7-bit address, the eighth is the R/W bit, so we
 * write the codec address with one bit shifted left
 */
#define CODEC_ADDR 0x36



typedef struct WavHdr
{
	char chunk_id[4];
	uint32_t chunk_size;
	char format[4];

	char subchunk1_id[4];
	uint32_t subchunk1_size;
	uint16_t audio_format;
	uint16_t num_channels;
	uint32_t sample_rate;
	uint32_t byte_rate;
	uint16_t block_align;
	uint16_t bits_per_sample;

	uint8_t subchunk2[8];
} WavHdr;


//#define FILE_NAME  "input0.wav"
//#define FILE_NAME  "due.wav"
//#define FILE_NAME  "sample.wav"
//#define FILE_NAME  "testaa"
#define FILE_NAME  "outfile.wav"
#define ACQ_FILE_NAME  "acq.wav"

uint8_t tmp[4096];

// SD fat filesystem context
FATFS fs;
FatFile log_file;
FatFile acq_file;

static I2c i2c;
static I2s i2s;
static Wm8731 wm8731_ctx;

static void init(void)
{
	IRQ_ENABLE;
	kdbg_init();
	kprintf("sam3x %s: %d times\n", VERS_HOST, VERS_BUILD);

	timer_init();
	LED_INIT();

	adc_init();
	dmac_init();
	i2c_init(&i2c, I2C_BITBANG0, CONFIG_I2C_FREQ);
	i2s_init(&i2s, SSC0);

	wm8731_init(&wm8731_ctx, &i2c, CODEC_ADDR);
	wm8731_setVolume(&wm8731_ctx, WM8731_HEADPHONE, 0);
}

size_t count = 0;

static void codec(struct I2s *i2s, void *_buf, size_t len)
{
	count += kfile_read(&log_file.fd, _buf, len);
	if (count >= log_file.fat_file.fsize - sizeof(WavHdr))
	{
		kprintf("stop %d\n", count);
		i2s_dmaTxStop(i2s);
		count = 0;
	}
}

static int wav_check(KFile *fd)
{

	WavHdr header;

	if (kfile_read(fd, &header, sizeof(header)) != sizeof(header))
	{
		kputs("Error reading wave file header\n");
		return EOF;
	}

	if (strncmp(header.chunk_id, "RIFF", 4))
	{
		kputs("RIFF tag not found\n");
		goto error;
	}

	if (strncmp(header.format, "WAVE", 4))
	{
		kputs("WAVE tag not found\n");
		goto error;
	}

	if (le16_to_cpu(header.audio_format) != 1)
	{
		kprintf("Audio format not valid, found [%d]\n", le16_to_cpu(header.audio_format));
		goto error;
	}

	if (le16_to_cpu(header.num_channels) != 2)
	{
		kprintf("Channels number not valid, found [%d]\n", le16_to_cpu(header.num_channels));
		goto error;
	}


	if (le32_to_cpu(header.sample_rate) != 44100)
	{
		kprintf("Sample rate not valid, found [%ld]\n", le32_to_cpu(header.sample_rate));
		goto error;
	}

	if (le16_to_cpu(header.bits_per_sample) != 16)
	{
		kprintf("Bits per sample not valid, found [%d]\n", le16_to_cpu(header.bits_per_sample));
		goto error;
	}
	return 0;

error:
	return 1;
}

int main(void)
{
	init();

	for (;;)
	{
		if (SD_CARD_PRESENT())
		{
			timer_delay(10);
			Sd sd;
			bool sd_ok = sd_init(&sd, NULL, 0);
			FRESULT result;

			if (sd_ok)
			{
				kprintf("Mount FAT filesystem.\n");
				result = f_mount(0, &fs);
				if (result != FR_OK)
				{
					kprintf("Mounting FAT volumes error[%d]\n", result);
					sd_ok = false;
				}

				if (sd_ok)
				{
					result = fatfile_open(&log_file, FILE_NAME,  FA_OPEN_EXISTING | FA_READ);
					if (result == FR_OK)
					{
						kprintf("Opened log file '%s' size %ld\n", FILE_NAME, log_file.fat_file.fsize);
						wm8731_setVolume(&wm8731_ctx, WM8731_HEADPHONE, adc_read(1));
						if (wav_check(&log_file.fd) >= 0)
						{
							kputs("Wav file play..\n");
							i2s_dmaStartTxStreaming(&i2s, tmp, sizeof(tmp), sizeof(tmp) / 4, codec);
						}

						wm8731_setVolume(&wm8731_ctx, WM8731_HEADPHONE, 0);

						// Flush data and close the files.
						kfile_flush(&log_file.fd);
						kfile_close(&log_file.fd);
					}
					else
					{
						kprintf("Unable to open file: '%s' error[%d]\n", FILE_NAME, result);
					}

					//Unmount always to prevent accidental sd remove.
					f_mount(0, NULL);
					kprintf("Umount\n");

				}
				f_mount(0, NULL);
			}

			timer_delay(5000);
		}
		else
		{
			kputs("No card insert..\n");
			timer_delay(500);
		}
	}

	return 0;
}






