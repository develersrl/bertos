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
 * \brief KFile interface over a FIFO buffer.
 *
 * \version $Id: cfg_adc.h 2348 2009-02-16 13:43:44Z duplo $
 * \author Francesco Sacchi <asterix@develer.com>
 *
 * $WIZ$ module_name = "kfilefifo"
 * $WIZ$ module_depends = "kfile"
 */

#ifndef STRUCT_KFILE_FIFO
#define STRUCT_KFILE_FIFO

#include "fifobuf.h"
#include <kern/kfile.h>

typedef struct KFileFifo
{
		KFile fd;
		FIFOBuffer *fifo;
} KFileFifo;

/**
 * ID for KFile FIFO.
 */
#define KFT_KFILEFIFO MAKE_ID('F', 'I', 'F', '0')

/**
 * Convert + ASSERT from generic KFile to KFileFifo.
 */
INLINE KFileFifo * KFILEFIFO_CAST(KFile *fd)
{
	ASSERT(fd->_type == KFT_KFILEFIFO);
	return (KFileFifo *)fd;
}

void kfilefifo_init(KFileFifo *kf, FIFOBuffer *fifo);

#endif /* STRUCT_KFILE_FIFO */
