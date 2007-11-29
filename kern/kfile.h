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
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001, 2003 Bernardo Innocenti <bernie@develer.com>
 *
 * -->
 *
 * \brief Virtual KFile I/O interface.
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

#ifndef MWARE_KFILE_H
#define MWARE_KFILE_H

#include <cfg/compiler.h>

/* fwd decl */
struct _KFile;

typedef int32_t kfile_off_t;

/**
 * Costants for repositioning read/write file offset.
 * These are needed because on some embedded platforms
 * ANSI I/O library may not be present.
 */
typedef enum KSeekMode
{
	KSM_SEEK_SET, ///< Seek from file beginning.
	KSM_SEEK_CUR, ///< Seek from file current position.
	KSM_SEEK_END, ///< Seek from file end.
} KSeekMode;


typedef size_t	(*ReadFunc_t)  (struct _KFile *fd, void *buf, size_t size);
typedef size_t	(*WriteFunc_t) (struct _KFile *fd, const void *buf, size_t size);
typedef	int32_t (*SeekFunc_t)  (struct _KFile *fd, kfile_off_t offset, KSeekMode whence);
typedef bool	(*OpenFunc_t)  (struct _KFile *fd, const char *name, int mode);
typedef bool	(*CloseFunc_t) (struct _KFile *fd);

/**
 * Context data for callback functions which operate on
 * pseudo files.
 */
typedef struct _KFile
{
	ReadFunc_t		read;
	WriteFunc_t		write;
	OpenFunc_t		open;
	CloseFunc_t		close;
	SeekFunc_t		seek;

	/* NOTE: these must _NOT_ be size_t on 16bit CPUs! */
	uint32_t		seek_pos;
	uint32_t		size;
} KFile;


/*
 * Generic implementation of seek function.
 */
int32_t kfile_seek(struct _KFile *fd, kfile_off_t offset, KSeekMode whence);

/*
 * Kfile test function
 */
bool kfile_test(uint8_t *buf, size_t _size , uint8_t *save_buf, size_t save_buf_size);

#endif /* MWARE_KFILE_H */
