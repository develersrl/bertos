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
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \version $Id$
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Driver for the AVR ATMega TWI (interface)
 */

/*#*
 *#* $Log$
 *#* Revision 1.5  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.4  2006/03/20 17:49:49  bernie
 *#* Make the TWI driver more generic to work with devices other than EEPROMS.
 *#*
 *#* Revision 1.3  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.2  2005/02/18 11:19:52  bernie
 *#* Update copyright info.
 *#*
 *#*/
#ifndef DRV_TWI_H
#define DRV_TWI_H

#include <cfg/compiler.h>

bool twi_start_w(uint8_t id);
bool twi_start_r(uint8_t id);
void twi_stop(void);
bool twi_put(const uint8_t data);
bool twi_send(const void *_buf, size_t count);
bool twi_recv(void *_buf, size_t count);
void twi_init(void);

#endif /* DRV_EEPROM_H */
