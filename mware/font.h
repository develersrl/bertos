/*!
 * \file
 * <!--
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief Font 8x6 IBM-PC 8bit
 */

/*#*
 *#* $Log$
 *#* Revision 1.5  2005/03/01 23:26:45  bernie
 *#* Use new CPU-neutral program-memory API.
 *#*
 *#* Revision 1.4  2004/12/31 16:42:55  bernie
 *#* Sanitize for non-Harvard processors.
 *#*
 *#* Revision 1.3  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 15:43:16  bernie
 *#* Import mware modules.
 *#*
 *#* Revision 1.2  2004/03/24 15:48:53  bernie
 *#* Remove Copyright messages from Doxygen output
 *#*
 *#* Revision 1.1  2004/01/13 12:15:28  aleph
 *#* Move font table in program memory; add font.h
 *#*
 *#*/
#ifndef MWARE_FONT_H
#define MWARE_FONT_H

#include <compiler.h> /* uint8_t */
#include <mware/pgm.h> /* PROGMEM */

/*!
 * \name Font size (in pixel)
 * \{
 */
#define FONT_WIDTH   6
#define FONT_HEIGHT  8
/* \} */

/*! Font table */
extern const PROGMEM uint8_t font[256 * FONT_WIDTH];

#endif /* MWARE_FONT_H */
