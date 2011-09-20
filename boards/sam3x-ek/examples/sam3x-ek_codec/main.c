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
 * \brief Atmel SAM3X-EK testcase
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#include "bitmaps.h"

#include "hw/hw_led.h"
#include "hw/hw_lcd.h"
#include "hw/hw_adc.h"
#include "hw/hw_sdram.h"
#include "hw/hw_sd.h"

#include "cfg/cfg_i2s.h"

#include <cfg/debug.h>

#include <cpu/irq.h>
#include <cpu/irq.h>
#include <cpu/byteorder.h>

#include <drv/kbd.h>
#include <drv/i2c.h>
#include <drv/i2s.h>
#include <drv/sd.h>
#include <drv/timer.h>
#include <drv/lcd_hx8347.h>
#include <drv/adc.h>
#include <drv/wm8731.h>
#include <drv/dmac_sam3.h>
#include <drv/sd.h>

#include <gfx/gfx.h>
#include <gfx/font.h>
#include <gfx/text.h>
#include <gui/menu.h>
#include <icons/logo.h>

#include <io/kfile.h>
#include <io/kfile_block.h>

#include <kern/signal.h>
#include <kern/proc.h>

#include <fs/fat.h>

#include <struct/list.h>

#include <verstag.h>
#include <buildrev.h>

#include <string.h>

// Keyboard
#define KEY_MASK (K_LEFT | K_RIGHT)

// Kernel
#define PROC_STACK_SIZE	KERN_MINSTACKSIZE * 2

static PROC_DEFINE_STACK(hp_stack, PROC_STACK_SIZE);
static PROC_DEFINE_STACK(lp_stack, PROC_STACK_SIZE);

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


static uint8_t raster[RAST_SIZE(LCD_WIDTH, LCD_HEIGHT)];
static Bitmap lcd_bitmap;
extern Font font_gohu;
static int lcd_brightness = LCD_BACKLIGHT_MAX;
static Process *hp_proc, *lp_proc;
static hptime_t start, end;


uint8_t tmp[4096];

// SD fat filesystem context
FATFS fs;
FatFile log_file;
FatFile acq_file;

static I2c i2c;
static I2s i2s;
static Wm8731 wm8731_ctx;


static void screen_saver(Bitmap *bm)
{
	int x1, y1, x2, y2;
	int i;

	for (i = 0; ; i++)
	{
		x1 = i % LCD_WIDTH;
		y1 = i % LCD_HEIGHT;

		x2 = LCD_WIDTH - i % LCD_WIDTH;
		y2 = LCD_HEIGHT - i % LCD_HEIGHT;

		gfx_bitmapClear(bm);
		gfx_rectDraw(bm, x1, y1, x2, y2);
		lcd_hx8347_blitBitmap(bm);
		if (kbd_peek() & KEY_MASK)
			break;
	}
}


INLINE hptime_t get_hp_ticks(void)
{
	return (timer_clock_unlocked() * TIMER_HW_CNT) + timer_hw_hpread();
}

static void NORETURN hp_process(void)
{
	while (1)
	{
		sig_wait(SIG_USER0);
		end = get_hp_ticks();
		timer_delay(100);
		sig_send(lp_proc, SIG_USER0);
	}
}

static void NORETURN lp_process(void)
{
	while (1)
	{
		start = get_hp_ticks();
		sig_send(hp_proc, SIG_USER0);
		sig_wait(SIG_USER0);
	}
}

/*
 * Lcd
 */
static void setBrightness(Bitmap *bm)
{
	while (1)
	{
		gfx_bitmapClear(bm);
		text_xprintf(bm, 1, 0, TEXT_FILL | TEXT_CENTER, "Brightness: %d", lcd_brightness);
		text_xprintf(bm, 3, 0, TEXT_FILL | TEXT_CENTER, "RIGHT key: change");
		text_xprintf(bm, 4, 0, TEXT_FILL | TEXT_CENTER, "LEFT  key: back  ");
		lcd_hx8347_blitBitmap(bm);

		keymask_t mask = kbd_get();

		if (mask & K_LEFT)
			break;
		else if (mask & K_RIGHT)
		{
			if (++lcd_brightness > LCD_BACKLIGHT_MAX)
				lcd_brightness = 0;
			lcd_setBacklight(lcd_brightness);
		}
	}
}


static void NORETURN soft_reset(Bitmap * bm)
{
	int i;

	gfx_bitmapClear(bm);
	for (i = 5; i; --i)
	{
		text_xprintf(bm, 2, 0, TEXT_FILL | TEXT_CENTER, "%d", i);
		lcd_hx8347_blitBitmap(bm);
		timer_delay(1000);
	}
	text_xprintf(bm, 2, 0, TEXT_FILL | TEXT_CENTER, "REBOOT");
	lcd_hx8347_blitBitmap(bm);
	timer_delay(1000);

	/* Perform a software reset request */
	HWREG(NVIC_APINT) = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ;
	UNREACHABLE();
}

static void read_adc(Bitmap *bm)
{
	gfx_bitmapClear(bm);
	text_xprintf(bm, 0, 0, TEXT_FILL | TEXT_CENTER, "ADC Value");
	while (1)
	{
		uint16_t value = ADC_RANGECONV(adc_read(1), 0, 3300);
		uint16_t temp = hw_convertToDegree (adc_read(ADC_TEMPERATURE_CH));

		text_xprintf(&lcd_bitmap, 2, 0, TEXT_FILL | TEXT_CENTER,
									"Voltage on VR1: %d.%dV", value / 1000, value % 1000);
		text_xprintf(&lcd_bitmap, 3, 0, TEXT_FILL | TEXT_CENTER,
									"CPU temperature: %d.%dC", temp / 10, temp % 10);
		lcd_hx8347_blitBitmap(bm);
		timer_delay(400);
		if (kbd_peek() & KEY_MASK)
			break;
	}
}
#define FILE_NAME  "outfile.wav"
#define ACQ_FILE_NAME  FILE_NAME


uint8_t tmp[4096];

// SD fat filesystem context
FATFS fs;
FatFile log_file;
FatFile acq_file;



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


static int wav_writeHdr(KFile *fd, uint16_t rate, uint16_t channels, uint16_t bits)
{
	WavHdr header;

	memcpy(&header.chunk_id, "RIFF", 4);
	memcpy(&header.format, "WAVE", 4);
	header.audio_format = cpu_to_le16((uint16_t)1);
	header.num_channels = cpu_to_le16(channels);
	header.sample_rate = cpu_to_le16(rate);
	header.bits_per_sample = cpu_to_le16(bits);

	kfile_seek(fd, 0, KSM_SEEK_SET);
	kfile_write(fd, &header, sizeof(header));

	return 0;
}

size_t count = 0;
static void codec_play(struct I2s *i2s, void *_buf, size_t len)
{
	count += kfile_read(&log_file.fd, _buf, len);
	if (count >= log_file.fat_file.fsize - sizeof(WavHdr))
	{
		kprintf("stop %d\n", count);
		i2s_dmaTxStop(i2s);
		count = 0;
	}
}

static void codec_rec(struct I2s *i2s, void *_buf, size_t len)
{
	count += kfile_write(&acq_file.fd, _buf, len);
	if (count >= 1024 * 1024)
	{
		kprintf("stop %d\n", count);
		i2s_dmaRxStop(i2s);
		count = 0;
	}
}



#define MAX_ITEM_NODES    30
#define MAX_ITEMS_ROW     15
#define NEXT_ITEM_COL     10

typedef struct FileItemNode
{
	Node n;
	char file_name[13];
} FileItemNode;

FileItemNode item_nodes[MAX_ITEM_NODES];
static Sd sd;

static void wav_play(Bitmap *bm, char *file_name)
{
	gfx_bitmapClear(bm);

	kprintf("Mount FAT filesystem.\n");
	FRESULT result = f_mount(0, &fs);
	bool sd_ok = true;
	if (result != FR_OK)
	{
		kprintf("Mounting FAT volumes error[%d]\n", result);
		sd_ok = false;
	}

	if (sd_ok)
	{
		result = fatfile_open(&log_file, file_name,  FA_OPEN_EXISTING | FA_READ);
		if (result == FR_OK)
		{
			text_xprintf(bm, 1, 0, TEXT_CENTER, "Play wav file: %s", file_name);
			text_xprintf(bm, 2, 0, TEXT_CENTER, "File: %ld", log_file.fat_file.fsize);
			text_xprintf(bm, 3, 0, TEXT_CENTER, "Volume level %ld", ADC_RANGECONV(adc_read(1), 0, 100));
			kprintf("Open file: %s\n", file_name);
			wm8731_setVolume(&wm8731_ctx, WM8731_HEADPHONE, ADC_RANGECONV(adc_read(1), 0, 100));

			lcd_hx8347_blitBitmap(bm);
			if (wav_check(&log_file.fd) >= 0)
			{
				kputs("Wav file play..\n");
				i2s_dmaStartTxStreaming(&i2s, tmp, sizeof(tmp), sizeof(tmp) / 4, codec_play);
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
	}
	f_mount(0, NULL);

	lcd_hx8347_blitBitmap(bm);
}


INLINE FileItemNode *refresh_cursor(Bitmap *bm, List *file_list, int select_idx)
{
	FileItemNode *item;
	FileItemNode *select_node;
	int col = 0;
	int row = 0;

	gfx_bitmapClear(bm);
	select_node = (FileItemNode *)LIST_HEAD(file_list);
    FOREACH_NODE(item, file_list)
	{
		if (row > MAX_ITEMS_ROW)
		{
			row = 1;
			col = NEXT_ITEM_COL;
		}

		text_style(bm, 0, STYLEF_MASK);
		if (select_idx <= MAX_ITEMS_ROW)
		{
			if (row == select_idx && col == 0)
				text_style(bm, STYLEF_INVERT, STYLEF_INVERT);
				select_node = item;
		}
		else
		{
			if (row == (select_idx - MAX_ITEMS_ROW) && col == NEXT_ITEM_COL)
				text_style(bm, STYLEF_INVERT, STYLEF_INVERT);
				select_node = item;
		}

		text_xprintf(bm, row, col, TEXT_NORMAL, "%s", item->file_name);
		row++;
	}

	lcd_hx8347_blitBitmap(bm);

	return select_node;
}

static void sd_explorer(Bitmap *bm)
{
	List file_list;
	LIST_INIT(&file_list);
	int file_list_size = 0;
	gfx_bitmapClear(bm);

	memcpy (&item_nodes[0].file_name, "<- Return..", sizeof(item_nodes[0].file_name));
	ADDTAIL(&file_list, &item_nodes[0].n);

	if (SD_CARD_PRESENT())
	{
		timer_delay(10);

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
				FILINFO fno;
				DIR dir;

				kputs("open dir\n");
				/* Open the directory */
				result = f_opendir(&dir, "/");
				if (result == FR_OK)
				{
					// First element is reserved for "return" label
					for (int i = 1;; i++)
					{
						/* Read a directory item */
						result = f_readdir(&dir, &fno);
						if (result != FR_OK || fno.fname[0] == 0)
							break;  /* Break on error or end of dir */
						if (fno.fname[0] == '.')
							continue; /* Ignore dot entry */

						if (fno.fattrib & AM_DIR)
							continue;
						else
						{
							if (i < MAX_ITEM_NODES)
							{
								memcpy (&item_nodes[i].file_name, fno.fname, sizeof(item_nodes[i].file_name));
								ADDTAIL(&file_list, &item_nodes[i].n);
								file_list_size++;
							}
							else
							{
								kputs("No enought spase to show file list..\n");
								break;
							}
						}
					}
				}
			}
			f_mount(0, NULL);
			kprintf("Umount\n");
		}
	}
	else
	{
		kputs("No card insert..\n");
		text_xprintf(bm, 5, 0, TEXT_CENTER | TEXT_FILL, "%s", "No card insert..");
	}

	int idx = 0;
	FileItemNode *selected_node = refresh_cursor(bm, &file_list, idx);
	while (1)
	{
		keymask_t key = kbd_peek();
		if (key & K_LEFT)
		{
			idx++;
			if (idx > file_list_size)
				idx = 0;

			selected_node = refresh_cursor(bm, &file_list, idx);
			kprintf("lidx[%d]\n", idx);
		}
		if (key & K_RIGHT)
		{
			kprintf("ridx[%d]\n", idx);
			wav_play(bm, selected_node->file_name);
			if (idx == 0)
				break;
		}
	}
}

static void test_draw(Bitmap *bm)
{
	gfx_bitmapClear(bm);
	text_xprintf(bm, 0, 0, TEXT_NORMAL, "%s\n", "12345678.123");
	text_xprintf(bm, 0, 10, TEXT_NORMAL, "%s\n", "12345678.123");
	text_xprintf(bm, 1, 0, TEXT_NORMAL, "%s\n", "12345678.123");
	text_xprintf(bm, 2, 0, TEXT_NORMAL, "%s\n", "12345678.123");
	text_xprintf(bm, 3, 0, TEXT_NORMAL, "%s\n", "12345678.123");
	lcd_hx8347_blitBitmap(bm);

	timer_delay(400);
	while (1)
		if (kbd_peek() & KEY_MASK)
			break;
}

static struct MenuItem sub_items[] =
{
	{ (const_iptr_t)"un", 0, (MenuHook)0, NULL },
	{ (const_iptr_t)"du", 0, (MenuHook)0, NULL },
	{ (const_iptr_t)"tr", 0, (MenuHook)0, NULL },
	{ (const_iptr_t)0, 0, NULL, (iptr_t)0 }
};
static struct Menu sub_menu = { sub_items, "BeRTOS", MF_SAVESEL, &lcd_bitmap, 0, lcd_hx8347_blitBitmap };

static struct MenuItem main_items[] =
{
	{ (const_iptr_t)"Screen saver demo",  0, (MenuHook)screen_saver,  &lcd_bitmap },
	{ (const_iptr_t)"Display brightness", 0, (MenuHook)setBrightness, &lcd_bitmap },
	{ (const_iptr_t)"SD dir",             0, (MenuHook)sd_explorer,   (iptr_t)&lcd_bitmap },
	{ (const_iptr_t)"Reboot",             0, (MenuHook)soft_reset,    &lcd_bitmap },
	{ (const_iptr_t)0,                    0, NULL,                    (iptr_t)0   }
};
static struct Menu main_menu = { main_items, "BeRTOS", MF_STICKY | MF_SAVESEL, &lcd_bitmap, 0, lcd_hx8347_blitBitmap };


int main(void)
{
	IRQ_ENABLE;
	kdbg_init();

	LED_INIT();
	timer_init();

	proc_init();
	sdram_init();
	adc_init();

	kprintf("sam3x %s: %d times\n", VERS_HOST, VERS_BUILD);

	dmac_init();
	i2c_init(&i2c, I2C_BITBANG0, CONFIG_I2C_FREQ);
	i2s_init(&i2s, SSC0);

	wm8731_init(&wm8731_ctx, &i2c, CODEC_ADDR);
	wm8731_setVolume(&wm8731_ctx, WM8731_HEADPHONE, 0);

	kprintf("CPU Frequecy:%ld\n", CPU_FREQ);


	/* Enable the adc to read internal temperature sensor */
	hw_enableTempRead();


	lcd_hx8347_init();
	lcd_setBacklight(lcd_brightness);

	gfx_bitmapInit(&lcd_bitmap, raster, LCD_WIDTH, LCD_HEIGHT);
	gfx_setFont(&lcd_bitmap, &font_luBS14);
	lcd_hx8347_blitBitmap(&lcd_bitmap);

	kbd_init();

	hp_proc = proc_new(hp_process, NULL, PROC_STACK_SIZE, hp_stack);
	lp_proc = proc_new(lp_process, NULL, PROC_STACK_SIZE, lp_stack);

	proc_setPri(hp_proc, 2);
	proc_setPri(lp_proc, 1);

	lcd_hx8347_blitBitmap24(10, 52, BMP_LOGO_WIDTH, BMP_LOGO_HEIGHT, bmp_logo);
	timer_delay(500);


	while (1)
	{
		menu_handle(&main_menu);
		cpu_relax();
	}

}
