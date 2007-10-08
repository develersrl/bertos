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
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Hardware dependent serial driver (interface)
 *
 * \version $Id$
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Giovanni Bajo <rasky@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.10  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.9  2006/02/17 22:23:06  bernie
 *#* Update POSIX serial emulator.
 *#*
 *#* Revision 1.8  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.7  2005/01/14 00:47:56  aleph
 *#* Rename callbacks; SerialHardwareVT.txSending: New callback.
 *#*
 *#* Revision 1.6  2004/12/08 08:56:58  bernie
 *#* Reformat.
 *#*
 *#* Revision 1.5  2004/09/06 21:40:50  bernie
 *#* Move buffer handling in chip-specific driver.
 *#*
 *#* Revision 1.4  2004/08/25 14:12:08  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.3  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.2  2004/05/23 18:21:53  bernie
 *#* Trim CVS logs and cleanup header info.
 *#*
 *#*/

#ifndef DRV_SER_P_H
#define DRV_SER_P_H

#include <cfg/compiler.h> /* size_t */

struct SerialHardware;
struct Serial;

struct SerialHardwareVT
{
	void (*init)(struct SerialHardware *ctx, struct Serial *ser);
	void (*cleanup)(struct SerialHardware *ctx);
	void (*setBaudrate)(struct SerialHardware *ctx, unsigned long rate);
	void (*setParity)(struct SerialHardware *ctx, int parity);
	void (*txStart)(struct SerialHardware *ctx);
	bool (*txSending)(struct SerialHardware *ctx);
};

struct SerialHardware
{
	const struct SerialHardwareVT *table;
	unsigned char *txbuffer;
	unsigned char *rxbuffer;
	size_t         txbuffer_size;
	size_t         rxbuffer_size;
};

struct SerialHardware *ser_hw_getdesc(int unit);

#endif /* DRV_SER_P_H */
