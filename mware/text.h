/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
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

#ifndef MWARE_TEXT_H
#define MWARE_TEXT_H

#include <compiler.h>
#include <macros.h> /* BV() */
#include <cpu.h> /* CPU_HARVARD */

#include <stdarg.h>

/*!
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

#define STYLEF_MASK \
	(STYLEF_BOLD | STYLEF_ITALIC | STYLEF_UNDERLINE | \
	STYLEF_EXPANDED | STYLEF_CONDENSED | STYLEF_INVERT)
/*\}*/

/*!
 * \name Formatting flags for text rendering
 * \see text_xprintf()
 * \{
 */
#define TEXT_NORMAL   0       /*!< Normal mode */
#define TEXT_FILL     BV(7)   /*!< Fill rest of line with spaces */
#define TEXT_CENTER   BV(8)   /*!< Center string in line */
#define TEXT_RIGHT    BV(9)   /*!< Right aligned */
/*\}*/

/*! Escape sequences codes */
#define ANSI_ESC_CLEARSCREEN 'c'


/* Fwd decl */
struct Bitmap;

/* Low-level text functions (mware/text.c) */
void text_moveto(struct Bitmap *bm, int row, int col);
void text_setcoord(struct Bitmap *bm, int x, int y);
int text_putchar(char c, struct Bitmap *bm);
uint8_t text_style(uint8_t flags, uint8_t mask);
void text_clear(struct Bitmap *bm);
void text_clearLine(struct Bitmap *bm, int line);

/* Text formatting functions (mware/text_format.c) */
int text_puts(const char *str, struct Bitmap *bm);
int text_vprintf(struct Bitmap *bm, const char *fmt, va_list ap);
int text_printf(struct Bitmap *bm, const char *fmt, ...) FORMAT(__printf__, 2, 3);
int text_xprintf(struct Bitmap *bm, uint8_t row, uint8_t col, uint16_t mode, const char *fmt, ...) FORMAT(__printf__, 5, 6);
int text_vwidthf(struct Bitmap *bm, const char * fmt, va_list ap);
int text_widthf(struct Bitmap *bm, const char * fmt, ...) FORMAT(__printf__, 2, 3);

/* Text formatting functions for program-memory strings (mware/text_format.c) */
#if CPU_HARVARD
int text_puts_P(const char * PROGMEM str, struct Bitmap *bm);
int text_vprintf_P(struct Bitmap *bm, const char * PROGMEM fmt, va_list ap);
int text_printf_P(struct Bitmap *bm, const char * PROGMEM fmt, ...) FORMAT(__printf__, 2, 3);
int text_xprintf_P(struct Bitmap *bm, uint8_t row, uint8_t col, uint16_t mode, const char * PROGMEM fmt, ...) FORMAT(__printf__, 5, 6);
int text_vwidthf_P(struct Bitmap *bm, const char * PROGMEM fmt, va_list ap);
int text_widthf_P(struct Bitmap *bm, const char * PROGMEM fmt, ...);
#endif /* CPU_HARVARD */

#endif /* MWARE_TEXT_H */
