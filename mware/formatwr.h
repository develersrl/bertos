/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \version $Id$
 *
 * \brief Basic "printf", "sprintf" and "fprintf" formatter.
 */

/*#*
 *#* $Log$
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
#include <config.h>
#include <cpu.h>    /* CPU_HARVARD */

/*!
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
	/*! Enable/disable _formatted_write return value */
	#define CONFIG_PRINTF_RETURN_COUNT 1
#endif

int
_formatted_write(
	const char *format,
	void put_char_func(char c, void *user_data),
	void *user_data,
	va_list ap);

#if CPU_HARVARD
	#include "pgm.h"
	int _formatted_write_P(
		const char * PROGMEM format,
		void put_char_func(char c, void *user_data),
		void *user_data,
		va_list ap);
#endif /* CPU_HARVARD */

#endif /* MWARE_FORMATWR_H */

