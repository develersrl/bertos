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
 * \brief Atmel SAM3X-EK wav player example.
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Daniele Basile <asterix@develer.com>
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
#include <kern/msg.h>

#include <fs/fat.h>

#include <algo/wav.h>

#include <struct/list.h>

#include <verstag.h>
#include <buildrev.h>

#include <stdio.h>
#include <string.h>

// Keyboard
#define KEY_MASK (K_LEFT | K_RIGHT)

/*
 * Codec has 7-bit address, the eighth is the R/W bit, so we
 * write the codec address with one bit shifted left
 */
#define CODEC_ADDR 0x36

// Kernel
static PROC_DEFINE_STACK(play_proc_stack, 1024);
static PROC_DEFINE_STACK(rec_proc_stack, 1024);
MsgPort proc_play_inPort;
MsgPort proc_rec_inPort;

#define MAX_ITEM_NODES    30
#define MAX_ITEMS_ROW     15
#define NEXT_ITEM_COL     10

typedef struct FileItemNode
{
	Node n;
	char file_name[13];
} FileItemNode;

typedef struct AudioMsg
{
	Msg msg;
	char file_name[13];
} AudioMsg;

FileItemNode item_nodes[MAX_ITEM_NODES];

uint8_t tmp[10240];

// SD fat filesystem context
FATFS fs;
FatFile play_file;
FatFile rec_file;

static Sd sd;
static I2c i2c;
static I2s i2s;
static Wm8731 wm8731_ctx;

static uint8_t raster[RAST_SIZE(LCD_WIDTH, LCD_HEIGHT)];
static Bitmap lcd_bitmap;
extern Font font_gohu;
static int lcd_brightness = LCD_BACKLIGHT_MAX;
static uint16_t headphone_volume = 90;
static size_t played_size = 0;
static size_t recorded_size = 0;
static bool is_playing = false;
static bool is_recording = false;
static int recorderd_file_idx;

static void codec_play(struct I2s *i2s, void *_buf, size_t len)
{
	played_size += kfile_read(&play_file.fd, _buf, len);
	if (played_size >= play_file.fat_file.fsize - sizeof(WavHdr))
	{
		kprintf("stop %d\n", played_size);
		i2s_dmaTxStop(i2s);
		is_playing = false;
		played_size = 0;
	}
}

static void codec_rec(struct I2s *i2s, void *buf, size_t len)
{
	(void)i2s;
	ASSERT(buf);
	ASSERT(len != 0);
	ASSERT(&rec_file.fd);

	recorded_size += kfile_write(&rec_file.fd, buf, len);
}

static void NORETURN play_proc(void)
{
	while (1)
	{
		event_wait(&proc_play_inPort.event);
		AudioMsg *play;
		play = (AudioMsg *)msg_get(&proc_play_inPort);
		if (play && SD_CARD_PRESENT())
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
					result = fatfile_open(&play_file, play->file_name,  FA_OPEN_EXISTING | FA_READ);
					if (result == FR_OK)
					{
						kprintf("Open file: %s size %ld\n", play->file_name, play_file.fat_file.fsize);
						WavHdr wav;
						kfile_read(&play_file.fd, &wav, sizeof(WavHdr));
						if (wav_checkHdr(&wav, 1, CONFIG_CHANNEL_NUM, CONFIG_SAMPLE_FREQ, CONFIG_WORD_BIT_SIZE) != -1)
						{
							kputs("Wav file play..\n");

							wm8731_setVolume(&wm8731_ctx, WM8731_HEADPHONE, headphone_volume);
							is_playing = true;
							i2s_dmaStartTxStreaming(&i2s, tmp, sizeof(tmp), sizeof(tmp) / 2, codec_play);

							wm8731_setVolume(&wm8731_ctx, WM8731_HEADPHONE, 0);
						}

						// Flush data and close the files.
						kfile_flush(&play_file.fd);
						kfile_close(&play_file.fd);
					}
					else
					{
						kprintf("Unable to open file: '%s' error[%d]\n", play->file_name, result);
					}
				}
				f_mount(0, NULL);
			}
		}
	}
}


static void NORETURN rec_proc(void)
{
	while (1)
	{
		event_wait(&proc_rec_inPort.event);
		AudioMsg *rec;
		rec = (AudioMsg *)msg_get(&proc_rec_inPort);
		if (rec && SD_CARD_PRESENT())
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
					result = fatfile_open(&rec_file, rec->file_name,  FA_CREATE_ALWAYS | FA_WRITE);
					if (result == FR_OK)
					{
						kprintf("Open file: %s size %ld\n", rec->file_name, rec_file.fat_file.fsize);
						kputs("Rec Wav file..\n");
						is_recording = true;
						// Leave place for wav header
						kfile_seek(&rec_file.fd, sizeof(WavHdr), KSM_SEEK_SET);

						i2s_dmaStartRxStreaming(&i2s, tmp, sizeof(tmp), sizeof(tmp) / 2, codec_rec);

						// write header
						WavHdr wav;
						wav_writeHdr(&wav, recorded_size, 1, CONFIG_CHANNEL_NUM, CONFIG_SAMPLE_FREQ, CONFIG_WORD_BIT_SIZE);
						kfile_seek(&rec_file.fd, 0, KSM_SEEK_SET);
						kfile_write(&rec_file.fd, &wav, sizeof(WavHdr));

						// Flush data and close the files.
						kfile_flush(&rec_file.fd);
						kfile_close(&rec_file.fd);
					}
					else
					{
						kprintf("Unable to open file: '%s' error[%d]\n", rec->file_name, result);
					}
				}
				f_mount(0, NULL);
			}
		}
	}
}


INLINE void start_play(char *file_name)
{
	played_size = 0;

	AudioMsg play_msg;
	memcpy(play_msg.file_name, file_name, sizeof(play_msg.file_name));
	msg_put(&proc_play_inPort, &play_msg.msg);
}

INLINE void stop_play(void)
{
	i2s_dmaTxStop(&i2s);
	is_playing = false;
}


INLINE void start_rec(char *file_name)
{
	recorded_size = 0;

	AudioMsg rec_msg;
	memcpy(rec_msg.file_name, file_name, sizeof(rec_msg.file_name));
	msg_put(&proc_rec_inPort, &rec_msg.msg);
}

INLINE void stop_rec(void)
{
	i2s_dmaRxStop(&i2s);
	is_recording = false;
}

INLINE FileItemNode *select_item(Bitmap *bm, List *file_list, int select_idx)
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
			{
				text_style(bm, STYLEF_INVERT, STYLEF_INVERT);
				select_node = item;
			}
		}
		else
		{
			if (row == (select_idx - MAX_ITEMS_ROW) && col == NEXT_ITEM_COL)
			{
				text_style(bm, STYLEF_INVERT, STYLEF_INVERT);
				select_node = item;
			}
		}

		text_xprintf(bm, row, col, TEXT_NORMAL, "%s", item->file_name);
		row++;
	}

	lcd_hx8347_blitBitmap(bm);

	return select_node;
}

static void play_menu(Bitmap *bm)
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
								kprintf("%s\n", item_nodes[i].file_name);
							}
							else
							{
								kputs("No enought space to store items in list\n");
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
	FileItemNode *selected_node = NULL;
	select_item(bm, &file_list, idx);
	while (1)
	{
		keymask_t key = kbd_peek();
		if ((key & K_LEFT) && !is_playing)
		{
			idx++;
			if (idx > file_list_size)
				idx = 0;

			selected_node = select_item(bm, &file_list, idx);
		}
		if (key & K_RIGHT)
		{
			if (idx == 0)
				break;

			if (!is_playing)
				start_play(selected_node->file_name);
			else
				stop_play();
		}

		cpu_relax();
	}
}


static void rec_menu(Bitmap *bm)
{
	gfx_bitmapClear(bm);
	text_style(bm, STYLEF_BOLD | STYLEF_UNDERLINE, STYLEF_BOLD | STYLEF_UNDERLINE);
	text_xprintf(bm, 0, 0, TEXT_CENTER | TEXT_FILL, "Microphone recorder.");
	text_style(bm, 0, STYLEF_MASK);
	text_xprintf(bm, 2, 0, TEXT_NORMAL, "Press RIGHT button to start recording");
	text_xprintf(bm, 3, 0, TEXT_NORMAL, "and to stop it re-press RIGHT button.");
	lcd_hx8347_blitBitmap(bm);

	ticks_t start= 0;
	while (1)
	{
		keymask_t key = kbd_peek();
		if ((key & K_LEFT) && (!is_recording))
		{
			break;
		}
		if (key & K_RIGHT)
		{
			char file_name[13];
			memset(file_name, 0, sizeof(file_name));
			sprintf(file_name, "REC%d.WAV", recorderd_file_idx);
			kprintf("rec %s\n", file_name);

			if (!is_recording)
			{
				start_rec(file_name);
				text_xprintf(bm, 5, 0, TEXT_CENTER | TEXT_FILL, "Start recording on file: %s", file_name);
				text_xprintf(bm, 6, 0, TEXT_CENTER | TEXT_FILL, " ");
				lcd_hx8347_blitBitmap(bm);
				start = timer_clock();
			}
			else
			{
				stop_rec();
				recorderd_file_idx++;
				text_xprintf(bm, 5, 0, TEXT_CENTER | TEXT_FILL, "Stop recording: %s", file_name);
				mtime_t elaps = ticks_to_ms(timer_clock() - start);
				text_xprintf(bm, 6, 0, TEXT_CENTER | TEXT_FILL, "Recorded: %ld.%ldsec", elaps / 1000, elaps % 1000);
				lcd_hx8347_blitBitmap(bm);
			}
		}

		cpu_relax();
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

static void setVolume(Bitmap *bm)
{
	gfx_bitmapClear(bm);
	text_style(bm, STYLEF_BOLD | STYLEF_UNDERLINE, STYLEF_BOLD | STYLEF_UNDERLINE);
	text_xprintf(bm, 0, 0, TEXT_CENTER, "Headphone Volume");
	text_style(bm, 0, STYLEF_MASK);
	text_xprintf(bm, 2, 0, TEXT_NORMAL, "Turn VR1 potentiometer to adjust it.");

	while (1)
	{
		headphone_volume = ADC_RANGECONV(adc_read(1), 0, 100);
		text_xprintf(bm, 5, 0, TEXT_FILL | TEXT_CENTER,	"Volume %d%%", headphone_volume);
		lcd_hx8347_blitBitmap(bm);

		timer_delay(400);
		if (kbd_peek() & KEY_MASK)
			break;
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


static struct MenuItem main_items[] =
{
	{ (const_iptr_t)"Play SD file",       0, (MenuHook)play_menu,     (iptr_t)&lcd_bitmap },
	{ (const_iptr_t)"Record file on SD",  0, (MenuHook)rec_menu,      (iptr_t)&lcd_bitmap },
	{ (const_iptr_t)"Set brightness",     0, (MenuHook)setBrightness, (iptr_t)&lcd_bitmap },
	{ (const_iptr_t)"Set volume",         0, (MenuHook)setVolume,     (iptr_t)&lcd_bitmap },
	{ (const_iptr_t)"Reboot",             0, (MenuHook)soft_reset,    (iptr_t)&lcd_bitmap },
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

	lcd_hx8347_init();
	lcd_setBacklight(lcd_brightness);

	gfx_bitmapInit(&lcd_bitmap, raster, LCD_WIDTH, LCD_HEIGHT);
	gfx_setFont(&lcd_bitmap, &font_luBS14);
	lcd_hx8347_blitBitmap(&lcd_bitmap);

	kbd_init();

	proc_new(play_proc, NULL, sizeof(play_proc_stack), play_proc_stack);
	proc_new(rec_proc, NULL, sizeof(rec_proc_stack), rec_proc_stack);
	msg_initPort(&proc_play_inPort, event_createGeneric());
	msg_initPort(&proc_rec_inPort, event_createGeneric());

	lcd_hx8347_blitBitmap24(10, 52, BMP_LOGO_WIDTH, BMP_LOGO_HEIGHT, bmp_logo);
	timer_delay(500);


	while (1)
	{
		menu_handle(&main_menu);
		cpu_relax();
	}

}
