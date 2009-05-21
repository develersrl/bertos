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
 * \brief FatFS: kfile interface for FatFS module by ChaN.
 *
 * \version $Id$
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 *
 */
#ifndef FS_FAT_H
#define FS_FAT_H

#include <kern/kfile.h>
#include "fatfs/src/ff.h"

typedef struct FatFile
{
	KFile fd;
	FIL fat_file;
	FRESULT error_code;       ///< error code for calls like kfile_read
} FatFile;

#define KFT_FATFILE MAKE_ID('F', 'A', 'T', 'F')

INLINE FatFile * FATFILE_CAST(KFile *fd)
{
	ASSERT(fd->_type == KFT_FATFILE);
	return (FatFile *)fd;
}

/**
 * Initialize \a file and open \a file_path for reading.
 */
FRESULT fatfile_open(FatFile *file, const char *file_path, BYTE mode);

#endif /* FS_FAT_H */

