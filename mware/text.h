/*!
 * \file
 * Copyright 1999 Bernardo Innocenti <bernie@develer.com>
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See devlib/README for information.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief Text graphic routines
 */

/*#*
 *#* $Log$
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

#ifndef TEXT_H
#define TEXT_H

#include "compiler.h"
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
#define TEXT_FILL     BV(0)   /*!< Fill rest of line with spaces */
#define TEXT_CENTER   BV(1)   /*!< Center string in line */
#define TEXT_INVERT   BV(2)   /*!< Inverted mode */
#define TEXT_RIGHT    BV(3)   /*!< Right aligned */
/*\}*/

/*! Escape sequences codes */
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
