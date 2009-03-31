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
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \version $Id$
 *
 * \brief SIPO Module
 *
 * The SIPO module trasform a serial input in
 * a parallel output. Please check hw_sipo.h
 * file to customize hardware relative parameters.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "sipo.h"

#include "hw/hw_sipo.h"

#include <cfg/compiler.h>

#include <kern/kfile.h>

#include <string.h>

/**
 * Write a char in sipo shift register
 */
INLINE void sipo_putchar(uint8_t c)
{
	for(int i = 0; i < 8; i++)
	{
		if((c & BV(i)) == 0)
			SIPO_SI_LOW();
		else
			SIPO_SI_HIGH();

		SIPO_SI_CLOCK();
	}
}

/**
 * Write a buffer into sipo register and when finish to
 * we load it.
 */
 static size_t sipo_write(UNUSED_ARG(struct KFile *, fd), const void *_buf, size_t size)
{
	const uint8_t *buf = (const uint8_t *)_buf;
	size_t write_len = size;
	ASSERT(buf);

	// Load into shift register all byte in buffer
	while(size--)
		sipo_putchar(*buf++);

	// We finsh to load bytes into shift register, load it.
	SIPO_LOAD();

	return write_len;
}

/**
 * Initialize the SIPO
 */
void sipo_init(Sipo *fd)
{
	ASSERT(fd);

	memset(fd, 0, sizeof(Sipo));

	//Set kfile struct type as a generic kfile structure.
	DB(fd->fd._type = KFT_SIPO);

	// Set up data flash programming functions.
	fd->fd.write = sipo_write;

	SIPO_INIT_PIN();

}
