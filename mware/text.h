/*!
 * \file
 * Copyright (C) 1999 Bernardo Innocenti <bernie@develer.com>
 * Copyright (C) 2003 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief Text graphic routines
 */

/*
 * $Log$
 * Revision 1.1  2004/05/23 15:43:16  bernie
 * Import mware modules.
 *
 * Revision 1.9  2004/04/03 20:42:49  aleph
 * Add text_clear()
 *
 * Revision 1.8  2004/03/19 16:52:28  bernie
 * Move printf() like functions from text.c to text_format.c and add PROGMEM versions.
 *
 * Revision 1.7  2004/03/17 17:30:30  bernie
 * Add GCC format checks to printf()-like functions.
 *
 * Revision 1.6  2004/02/21 21:40:20  aleph
 * Various small fixes
 *
 * Revision 1.5  2004/02/18 11:50:10  aleph
 * Add right alignment to xprintf
 *
 * Revision 1.4  2004/01/13 12:15:28  aleph
 * Move font table in program memory; add font.h
 *
 * Revision 1.3  2004/01/13 00:06:47  aleph
 * Fix clipping and ANSI esc bug, add text_xprintf()
 *
 * Revision 1.2  2004/01/08 18:03:12  aleph
 * Add text functions to set coordinates
 *
 * Revision 1.1  2004/01/07 23:31:54  aleph
 * Add text routines
 *
 */

#ifndef TEXT_H
#define TEXT_H

#include "compiler.h"
#include <stdarg.h>

/* Style flags */
#define STYLEF_BOLD        BV(0)
#define STYLEF_ITALIC      BV(1)
#define STYLEF_UNDERLINE   BV(2)
#define STYLEF_INVERT      BV(3)
#define STYLEF_EXPANDED    BV(4)
#define STYLEF_CONDENSED   BV(5)
#define STYLEF_STRIKEOUT   BV(6)  /* Not implemented */

#define STYLEF_MASK \
	(STYLEF_BOLD | STYLEF_ITALIC | STYLEF_UNDERLINE | \
	STYLEF_EXPANDED | STYLEF_CONDENSED | STYLEF_INVERT)

/* Flags for text_xprintf() */
#define TEXT_NORMAL   0       /*!< Normal mode */
#define TEXT_FILL     BV(0)   /*!< Fill rest of line with spaces */
#define TEXT_CENTER   BV(1)   /*!< Center string in line */
#define TEXT_INVERT   BV(2)   /*!< Inverted mode */
#define TEXT_RIGHT    BV(3)   /*!< Right aligned */

/* Escape sequences codes */
#define ANSI_ESC_CLEARSCREEN 'c'



/* Fwd decl */
struct Bitmap;

void text_moveto(struct Bitmap *bm, int row, int col);
void text_setcoord(struct Bitmap *bm, int x, int y);
int text_putchar(char c, struct Bitmap *bm);
int text_puts(const char *str, struct Bitmap *bm);
int text_vprintf(struct Bitmap *bm, const char *fmt, va_list ap);
int text_printf(struct Bitmap *bm, const char *fmt, ...) FORMAT(__printf__, 2, 3);
int text_xprintf(struct Bitmap *bm, uint8_t row, uint8_t col, uint8_t mode, const char *fmt, ...) FORMAT(__printf__, 5, 6);
uint8_t text_style(uint8_t flags, uint8_t mask);
void text_clear(struct Bitmap *bm);

int text_puts_P(const char * PROGMEM str, struct Bitmap *bm);
int text_vprintf_P(struct Bitmap *bm, const char * PROGMEM fmt, va_list ap);
int text_printf_P(struct Bitmap *bm, const char * PROGMEM fmt, ...) FORMAT(__printf__, 2, 3);
int text_xprintf_P(struct Bitmap *bm, uint8_t row, uint8_t col, uint8_t mode, const char * PROGMEM fmt, ...) FORMAT(__printf__, 5, 6);

#endif /* TEXT_H */
