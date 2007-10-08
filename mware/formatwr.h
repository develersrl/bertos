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
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \brief Basic "printf", "sprintf" and "fprintf" formatter.
 */

/*#*
 *#* $Log$
 *#* Revision 1.11  2006/07/19 12:56:27  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.10  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.9  2005/07/19 07:25:46  bernie
 *#* Use appconfig.h instead of cfg/config.h.
 *#*
 *#* Revision 1.8  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.7  2005/02/16 20:28:03  bernie
 *#* Add %S formatter.
 *#*
 *#* Revision 1.6  2005/01/08 08:50:06  bernie
 *#* Make more portable.
 *#*
 *#* Revision 1.5  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.4  2004/08/04 15:53:47  rasky
 *#* Nuove opzioni di configurazione per formatted_write e ridotto maggiormente l'utilizzo dellos tack
 *#*
 *#* Revision 1.3  2004/07/29 22:57:09  bernie
 *#* Add values for new-style CONFIG_PRINTF option.
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 15:43:16  bernie
 *#* Import mware modules.
 *#*
 *#*/
#ifndef MWARE_FORMATWR_H
#define MWARE_FORMATWR_H

#include <stdarg.h> /* va_list */
#include <appconfig.h>
#include <cfg/cpu.h>    /* CPU_HARVARD */

/**
 * \name _formatted_write() configuration
 * \{
 */
#define PRINTF_DISABLED    0
#define PRINTF_NOMODIFIERS 1
#define PRINTF_REDUCED     2
#define PRINTF_NOFLOAT     3
#define PRINTF_FULL        4
/* \} */

#ifndef CONFIG_PRINTF_RETURN_COUNT
	/** Enable/disable _formatted_write return value */
	#define CONFIG_PRINTF_RETURN_COUNT 1
#endif

int
_formatted_write(
	const char *format,
	void put_char_func(char c, void *user_data),
	void *user_data,
	va_list ap);

#if CPU_HARVARD
	#include <mware/pgm.h>
	int _formatted_write_P(
		const char * PROGMEM format,
		void put_char_func(char c, void *user_data),
		void *user_data,
		va_list ap);
#endif /* CPU_HARVARD */

#endif /* MWARE_FORMATWR_H */

