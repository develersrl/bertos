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
#include "hw/hw_boot.h"

// Define log settings for cfg/log.h
#define LOG_LEVEL    3
#define LOG_FORMAT   0
#include <cfg/log.h>
#include <cfg/debug.h>

#include <cpu/irq.h>

#include <drv/timer.h>
#include <drv/lcd_hx8347.h>
#include <drv/flash.h>
#include <drv/sd.h>
#include <drv/dmac_sam3.h>

#include <io/kblock.h>

#include <fs/fat.h>

#include <string.h>


static Flash internal_flash;
static KFileBlock flash;
static Sd sd;

FATFS fs;
FatFile fw_file;

uint8_t fw_buf[4096];
uint8_t fw_buf1[4096];

/*
 * To jump to the application init, we should add this offset plus one.
 */
#define FLASH_INIT_OFFSET  0x13
#define FLASH_TRIM_START   FLASH_BOOT_SIZE / FLASH_PAGE_SIZE_BYTES
/*
 * Define pointer function to main program.
 */
void (*rom_start)(void) NORETURN = (void *)(FLASH_BOOT_SIZE + FLASH_INIT_OFFSET);

#define START_APP() rom_start()

static void init(void)
{
	kdbg_init();

	IRQ_ENABLE;

	timer_init();
	dmac_init();
	LED_INIT();

	flash_init(&internal_flash, 0);

	kprintf("Trim start: %d, blocks: %ld\n", FLASH_TRIM_START, internal_flash.blk.blk_cnt - FLASH_TRIM_START);
	kblock_trim(&internal_flash.blk, FLASH_TRIM_START, internal_flash.blk.blk_cnt - FLASH_TRIM_START);
	kfileblock_init(&flash, &internal_flash.blk);
}


int main(void)
{
	init();

	LED_ON(LED_RED);

	if (SD_CARD_PRESENT())
	{
		// There is an hardware bug, so to flash correctly we access to flash more slowing.
		EEFC0_FMR = EEFC_FMR_FWS(7);
		EEFC1_FMR = EEFC_FMR_FWS(7);

		FRESULT result;
		bool sd_ok = sd_init(&sd, NULL, 0);
		if (sd_ok)
		{
			kprintf("Mount FAT filesystem.\n");
			result = f_mount(0, &fs);
			if (result != FR_OK)
			{
				kprintf("Mounting FAT volumes error[%d]\n", result);
				sd_ok = false;
				f_mount(0, NULL);
				goto end;
			}

			if (sd_ok)
			{
				result = fatfile_open(&fw_file, "firmware.bin",  FA_READ);
				if (result == FR_OK)
				{

					LOG_INFO("Firmware file found, checking for update...\n");
					size_t  fw_len = fw_file.fat_file.fsize;
					if (fw_len > (kfile_off_t)(FLASH_MEM_SIZE - FLASH_BOOT_SIZE))
					{
						LOG_ERR("Fw file too large\n");
						kfile_close(&fw_file.fd);
						f_mount(0, NULL);
						goto end;
					}

					size_t len;
					while (fw_len)
					{
						len = MIN(sizeof(fw_buf), fw_len);

						if (kfile_read(&fw_file.fd, fw_buf, len) != len)
						{
							LOG_ERR("Error reading fw file\n");
							kfile_close(&fw_file.fd);
							f_mount(0, NULL);
							goto end;
						}

						if (kfile_read(&flash.fd, fw_buf1, len) != len)
						{
							LOG_ERR("Error reading from flash\n");
							kfile_close(&fw_file.fd);
							f_mount(0, NULL);
							goto end;
						}
						if (memcmp(fw_buf, fw_buf1, len))
							break;

						fw_len -= len;
					}

					if (fw_len == 0)
					{
						LOG_INFO("Already up-to date\n");
						kfile_close(&fw_file.fd);
						f_mount(0, NULL);
						goto end;
					}

					LOG_INFO("Firmware file differs from memory, reprogramming...\n");
					fw_len = fw_file.fat_file.fsize;
					kfile_seek(&fw_file.fd, 0, KSM_SEEK_SET);
					kfile_seek(&flash.fd, 0, KSM_SEEK_SET);

					while (fw_len)
					{
						len = MIN(sizeof(fw_buf), fw_len);
						if (kfile_read(&fw_file.fd, fw_buf, len) != len)
						{
							LOG_ERR("Error reading fw file[%d]\n", len);
							kfile_close(&fw_file.fd);
							f_mount(0, NULL);
							goto end;
						}
						if (kfile_write(&flash.fd, fw_buf, len) != len)
						{
							LOG_ERR("Error writing flash!\n");
							kfile_close(&fw_file.fd);
							f_mount(0, NULL);
							goto end;
						}

						fw_len -= len;
					}
					kfile_flush(&flash.fd);
					kfile_close(&fw_file.fd);

					f_mount(0, NULL);
					LOG_INFO("Done!\n");

				}
				else
				{
					LOG_INFO("firmware file not found\n");
					f_mount(0, NULL);
					goto end;
				}
			}
		}
	}


end:
	//Shut down all peripheral before to jump to application.
	timer_hw_exit();
	IRQ_DISABLE;

	EEFC0_FMR = EEFC_FMR_FWS(3);
	EEFC1_FMR = EEFC_FMR_FWS(3);

	LOG_INFO("Jump to main application.\n");
	START_APP();

	return 0;
}
