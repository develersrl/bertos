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
 * Copyright 1999 Bernardo Innocenti <bernie@develer.com>
 *
 * -->
 *
 * \brief Text graphic routines (interface)
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 * \version $Id$
 */

/*#*
 *#* $Log$
 *#* Revision 1.8  2006/09/13 13:58:55  bernie
 *#* text_moveTo(): Swap parameters.
 *#*
 *#* Revision 1.7  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.6  2006/04/27 05:39:24  bernie
 *#* Enhance text rendering to arbitrary x,y coords.
 *#*
 *#* Revision 1.5  2006/04/11 00:08:24  bernie
 *#* text_offset(): New function, but I'm not quite confident with the design.
 *#*
 *#* Revision 1.4  2006/03/07 22:18:04  bernie
 *#* Correctly compute text width for prop fonts; Make styles a per-bitmap attribute.
 *#*
 *#* Revision 1.3  2006/02/10 12:26:19  bernie
 *#* Add STYLEF_TALL (unimplemented).
 *#*
 *#* Revision 1.2  2005/11/04 18:17:45  bernie
 *#* Fix header guards and includes for new location of gfx module.
 *#*
 *#* Revision 1.1  2005/11/04 18:11:35  bernie
 *#* Move graphics stuff from mware/ to gfx/.
 *#*
 *#* Revision 1.11  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.10  2005/03/01 23:26:46  bernie
 *#* Use new CPU-neutral program-memory API.
 *#*
 *#* Revision 1.9  2004/12/31 16:44:29  bernie
 *#* Sanitize for non-Harvard processors.
 *#*
 *#* Revision 1.8  2004/10/03 20:43:37  bernie
 *#* Import changes from project_ks.
 *#*
 *#* Revision 1.7  2004/09/20 03:28:49  bernie
 *#* Fix header; Conditionalize AVR-specific code.
 *#*
 *#* Revision 1.6  2004/09/14 20:57:30  bernie
 *#* Reformat.
 *#*
 *#* Revision 1.5  2004/09/06 21:51:26  bernie
 *#* Extend interface to allow any algorithmic style.
 *#*
 *#* Revision 1.4  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.3  2004/08/05 18:46:44  bernie
 *#* Documentation improvements.
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#*/

#ifndef GFX_TEXT_H
#define GFX_TEXT_H

#include <cfg/compiler.h>
#include <cfg/macros.h> /* BV() */
#include <cfg/cpu.h> /* CPU_HARVARD */
#include <gfx/gfx.h> /* coord_t */

#include <stdarg.h>

/**
 * \name Style flags
 * \see text_style()
 * \{
 */
#define STYLEF_BOLD        BV(0)
#define STYLEF_ITALIC      BV(1)
#define STYLEF_UNDERLINE   BV(2)
#define STYLEF_INVERT      BV(3)
#define STYLEF_EXPANDED    BV(4)
#define STYLEF_CONDENSED   BV(5)
#define STYLEF_STRIKEOUT   BV(6)  /*<! Not implemented */
#define STYLEF_TALL        BV(7)  /*<! Not implemented */

#define STYLEF_MASK \
	(STYLEF_BOLD | STYLEF_ITALIC | STYLEF_UNDERLINE \
	| STYLEF_INVERT | STYLEF_EXPANDED | STYLEF_CONDENSED \
	| STYLEF_STRIKEOUT | STYLEF_TALL)
/*\}*/

/**
 * \name Formatting flags for text rendering
 * \see text_xprintf()
 * \{
 */
#define TEXT_NORMAL   0       /**< Normal mode */
#define TEXT_FILL     BV(13)  /**< Fill rest of line with spaces */
#define TEXT_CENTER   BV(14)  /**< Center string in line */
#define TEXT_RIGHT    BV(15)  /**< Right aligned */
/*\}*/

/** Escape sequences codes */
#define ANSI_ESC_CLEARSCREEN 'c'


/* Fwd decl */
struct Bitmap;

/* Low-level text functions (mware/text.c) */
void text_moveTo(struct Bitmap *bm, int row, int col);
void text_setCoord(struct Bitmap *bm, int x, int y);
int text_putchar(char c, struct Bitmap *bm);
uint8_t text_style(struct Bitmap *bm, uint8_t flags, uint8_t mask);
void text_clear(struct Bitmap *bm);
void text_clearLine(struct Bitmap *bm, int line);

/* Text formatting functions (mware/text_format.c) */
int text_puts(const char *str, struct Bitmap *bm);
int text_vprintf(struct Bitmap *bm, const char *fmt, va_list ap);
int text_printf(struct Bitmap *bm, const char *fmt, ...) FORMAT(__printf__, 2, 3);
int text_xyvprintf(struct Bitmap *bm, coord_t x, coord_t y, uint16_t mode, const char *fmt, va_list ap);
int text_xyprintf(struct Bitmap *bm, coord_t x, coord_t col, uint16_t mode, const char *fmt, ...) FORMAT(__printf__, 5, 6);
int text_xprintf(struct Bitmap *bm, uint8_t row, uint8_t col, uint16_t mode, const char *fmt, ...) FORMAT(__printf__, 5, 6);
int text_vwidthf(struct Bitmap *bm, const char * fmt, va_list ap);
int text_widthf(struct Bitmap *bm, const char * fmt, ...) FORMAT(__printf__, 2, 3);

/* Text formatting functions for program-memory strings (mware/text_format.c) */
#if CPU_HARVARD
#include <mware/pgm.h>
int text_puts_P(const char * PROGMEM str, struct Bitmap *bm);
int text_vprintf_P(struct Bitmap *bm, const char * PROGMEM fmt, va_list ap);
int text_printf_P(struct Bitmap *bm, const char * PROGMEM fmt, ...) FORMAT(__printf__, 2, 3);
int text_xprintf_P(struct Bitmap *bm, uint8_t row, uint8_t col, uint16_t mode, const char * PROGMEM fmt, ...) FORMAT(__printf__, 5, 6);
int text_vwidthf_P(struct Bitmap *bm, const char * PROGMEM fmt, va_list ap);
int text_widthf_P(struct Bitmap *bm, const char * PROGMEM fmt, ...);
#endif /* CPU_HARVARD */

#endif /* GFX_TEXT_H */
