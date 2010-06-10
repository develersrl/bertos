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
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Function library for secure digital memory.
 *
 * Right now, the interface for these function is the one defined in diskio.h from
 * the FatFS module.
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "sd"
 * $WIZ$ module_depends = "kfile", "timer"
 * $WIZ$ module_hw = "bertos/hw/hw_sd.h"
 */


#ifndef DRV_SD_H
#define DRV_SD_H

#include "cfg/cfg_fat.h"

#include <kern/kfile.h>

#include <fs/fatfs/diskio.h>

bool sd_test(void);

/**
 * Initializes the SD driver.
 *
 * \param _fd A pointer to a kfile where the SD will read/write to.
 * \return true if initialization succeds, false otherwise.
 */
bool sd_init(KFile *_fd);

#if CONFIG_FAT_DRIVES == 1

	/**
	 * Same as sd_disk_status.
	 *
	 * Card initialization must be done with sd_init.
	 */
	#define sd_disk_initialize disk_initialize

	/**
	 * Return the status of the disk.
	 *
	 * \param drv The number of the drive to initialize. Currently only drive 0 is allowed.
	 * \return RES_OK if the sd card was correctly initialized by a previous call to sd_init(), STA_NOINIT otherwise.
	 */
	#define sd_disk_status     disk_status
	/**
	 * Read \a count sectors from SD card.
	 *
	 * \param drv The drive number to read from. Only 0 is supported.
	 * \param buf A buffer to store read data. You can get sector size using sd_disk_ioctl.
	 * \param sector Start sector number.
	 * \param count The number of sectors to read.
	 * \return RES_OK if the function succeded, RES_ERROR if any error occurred, RES_NOTRDY if the disk is not initialized.
	 *
	 * \sa diskio.h
	 */
	#define sd_disk_read       disk_read

	#if	CONFIG_FAT_FS_READONLY == 0

		/**
		 * Write \a count sectors to SD card.
		 *
		 * \param drv The drive number to read from. Only 0 is supported.
		 * \param buf The data to be written.
		 * \param sector Start sector number.
		 * \param count The number of sectors to write.
		 * \return RES_OK if the function succeded, RES_ERROR if any error occurred, RES_NOTRDY if the disk is not initialized.
		 *
		 * \sa diskio.h
		 */
		#define sd_disk_write      disk_write
	#endif

	/**
	 * Interface to send device independant commands to the device.
	 *
	 * \sa diskio.h and related documentation for further explanations.
	 */
	#define sd_disk_ioctl      disk_ioctl

#endif /* CONFIG_FAT_DRIVES == 1 */

#endif /* DRV_SD_H */
