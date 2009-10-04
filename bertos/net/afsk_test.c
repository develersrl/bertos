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
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief AFSK demodulator test.
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 * $test$: cp bertos/cfg/cfg_ax25.h $cfgdir/
 * $test$: echo "#undef AX25_LOG_LEVEL" >> $cfgdir/cfg_ax25.h
 * $test$: echo "#define AX25_LOG_LEVEL LOG_LVL_INFO" >> $cfgdir/cfg_ax25.h
 */


#include "afsk.h"
#include "cfg/cfg_afsk.h"

#include <drv/timer.h>
#include <net/ax25.h>

#include <cfg/test.h>
#include <cfg/debug.h>

#include <cpu/byteorder.h>

#include <stdio.h>
#include <string.h>

FILE *fp_adc;
FILE *fp_dac;
uint32_t data_size;
uint32_t data_written;
bool afsk_tx_test;
Afsk afsk_fd;
AX25Ctx ax25;

int8_t afsk_adc_val;

int msg_cnt;
static void message_hook(UNUSED_ARG(struct AX25Msg *, msg))
{
	msg_cnt++;
}

int afsk_testSetup(void)
{
	kdbg_init();
	#if CPU_AVR
		#warning TODO: open the file?
	#else
		fp_adc = fopen("test/afsk_test.au", "rb");
	#endif
	ASSERT(fp_adc);

	char snd[5];
	ASSERT(fread(snd, 1, 4, fp_adc) == 4);
	snd[4] = 0;
	ASSERT(strcmp(snd, ".snd") == 0);

	uint32_t offset;
	ASSERT(fread(&offset, 1, sizeof(offset), fp_adc) == sizeof(offset));
	offset = be32_to_cpu(offset);
	kprintf("AU file offset: %ld\n", offset);
	ASSERT(offset >= 24);

	ASSERT(fread(&data_size, 1, sizeof(data_size), fp_adc) == sizeof(data_size));
	data_size = be32_to_cpu(data_size);
	kprintf("AU file data_size: %ld\n", data_size);
	ASSERT(data_size);

	uint32_t encoding;
	ASSERT(fread(&encoding, 1, sizeof(encoding), fp_adc) == sizeof(encoding));
	encoding = be32_to_cpu(encoding);
	kprintf("AU file encoding: %ld\n", encoding);
	ASSERT(encoding == 2); // 8 bit linear PCM

	uint32_t sample_rate;
	ASSERT(fread(&sample_rate, 1, sizeof(sample_rate), fp_adc) == sizeof(sample_rate));
	sample_rate = be32_to_cpu(sample_rate);
	kprintf("AU file sample_rate: %ld\n", sample_rate);
	ASSERT(sample_rate == 9600);

	uint32_t channels;
	ASSERT(fread(&channels, 1, sizeof(channels), fp_adc) == sizeof(channels));
	channels = be32_to_cpu(channels);
	kprintf("AU file channels: %ld\n", channels);
	ASSERT(channels == 1);

	#if CPU_AVR
		#warning TODO: fseek?
	#else
		ASSERT(fseek(fp_adc, offset, SEEK_SET) == 0);
	#endif

	#if 0
	fp_dac = fopen("test/afsk_test_out.au", "w+b");
	ASSERT(fp_dac);
	#define FS_HH ((CONFIG_AFSK_DAC_SAMPLERATE) >> 24)
	#define FS_HL (((CONFIG_AFSK_DAC_SAMPLERATE) >> 16) & 0xff)
	#define FS_LH (((CONFIG_AFSK_DAC_SAMPLERATE) >> 8) & 0xff)
	#define FS_LL ((CONFIG_AFSK_DAC_SAMPLERATE) & 0xff)

	uint8_t snd_header[] = { '.','s','n','d', 0,0,0,24, 0,0,0,0, 0,0,0,2, FS_HH,FS_HL,FS_LH,FS_LL, 0,0,0,1};

	ASSERT(fwrite(snd_header, 1, sizeof(snd_header), fp_dac) == sizeof(snd_header));
	#endif
	timer_init();
	afsk_init(&afsk_fd);
	afsk_fd.fd.error = kfile_genericClose;
	ax25_init(&ax25, &afsk_fd.fd, message_hook);
	return 0;
}


int afsk_testRun(void)
{
	int c;
	while ((c = fgetc(fp_adc)) != EOF)
	{
		afsk_adc_val = (int8_t)c;
		afsk_adc_isr();

		ax25_poll(&ax25);
	}
	kprintf("Messages correctly received: %d\n", msg_cnt);
	ASSERT(msg_cnt >= 15);
	#if 0

	for (int i = 0; i < 75; i++)
		kfile_putc(HDLC_FLAG, &afsk_fd.fd);

	uint8_t tst[] =
	{
		0x92, 0xAE, 0x6A, 0x84, 0x9C, 0xB2, 0xF2, 0x92, 0xA4, 0x6A, 0xA0, 0x40, 0xC0, 0xE1, 0xAE, 0x92,
		0x88, 0x8A, 0x6E, 0x40, 0x6B, 0x03, 0xF0, 0x60, 0x27, 0x5B, 0x1E, 0x6C, 0x23, 0x43, 0x6A, 0x2F,
		0x5D, 0x22, 0x35, 0x70, 0x7D, 0x6F, 0x70, 0x2E, 0x52, 0x4F, 0x42, 0x45, 0x52, 0x54, 0x4F, 0x2C,
		0x5F, 0x51, 0x52, 0x56, 0x3A, 0x34, 0x33, 0x30, 0x2E, 0x31, 0x36, 0x32, 0x2C, 0x35, 0x0D, 0x09,
		0xCB,
	};

	for (int i = 0; i < sizeof(tst); i++)
	{
		if (tst[i] == AFSK_ESC || tst[i] == HDLC_FLAG || tst[i] == HDLC_RESET)
			kfile_putc(AFSK_ESC, &afsk_fd.fd);
		kfile_putc(tst[i], &afsk_fd.fd);
	}

	kfile_putc(0xE0, &afsk_fd.fd);
	kfile_putc(0x03, &afsk_fd.fd);

	kfile_putc(HDLC_FLAG, &afsk_fd.fd);

	while (afsk_tx_test)
		afsk_dac_isr();

	#endif
	return 0;
}

#define SND_DATASIZE_OFF 8

int afsk_testTearDown(void)
{
	#if 0
	ASSERT(fseek(fp_dac, SND_DATASIZE_OFF, SEEK_SET) == 0);
	data_written = cpu_to_be32(data_written);
	ASSERT(fwrite(&data_written, 1, sizeof(data_written), fp_dac) == sizeof(data_written));
	return fclose(fp_adc) + fclose(fp_dac);
	#endif
	return fclose(fp_adc);
}

TEST_MAIN(afsk);
