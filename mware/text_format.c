/*!
 * \file
 *
 * <!--
 * Copyright 1999 Bernardo Innocenti <bernie@develer.com>
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief printf-family routines for text output
 */

/*
 * $Log$
 * Revision 1.2  2004/06/03 11:27:09  bernie
 * Add dual-license information.
 *
 * Revision 1.1  2004/05/23 15:43:16  bernie
 * Import mware modules.
 *
 * Revision 1.2  2004/03/26 18:50:50  bernie
 * Move _PROGMEM stuff to compiler.h
 *
 * Revision 1.1  2004/03/19 16:52:28  bernie
 * Move printf() like functions from text.c to text_format.c and add PROGMEM versions.
 *
 */

#include "text.h"
#include "formatwr.h" /* _formatted_write() */
#include "font.h"
#include "gfx.h"
#include <stdarg.h>
#include <string.h> /* strlen */

/*!
 * Render string <code>str</code> in bitmap
 */
int PGM_FUNC(text_puts)(const char * PGM_ATTR str, struct Bitmap *bm)
{
	char c;

	while ((c = PGM_READ_CHAR(str++)))
		text_putchar(c, bm);

	return 0;
}


int PGM_FUNC(text_vprintf)(struct Bitmap *bm, const char * PGM_ATTR fmt, va_list ap)
{
	return PGM_FUNC(_formatted_write)(fmt, (void (*)(char, void *))text_putchar, bm, ap);
}


int PGM_FUNC(text_printf)(struct Bitmap *bm, const char * PGM_ATTR fmt, ...)
{
	int len;

	va_list ap;
	va_start(ap, fmt);
	len = PGM_FUNC(text_vprintf)(bm, fmt, ap);
	va_end(ap);

	return len;
}


int PGM_FUNC(text_xprintf)(struct Bitmap *bm,
		uint8_t row, uint8_t col, uint8_t mode, const char * PGM_ATTR fmt, ...) 
{
	int len;
	uint8_t oldstyle = 0;
	va_list ap;

	va_start(ap, fmt);

	text_moveto(bm, row, col);

	if (mode & TEXT_INVERT)
		oldstyle = text_style(STYLEF_INVERT, STYLEF_INVERT);

	if (mode & (TEXT_CENTER | TEXT_RIGHT))
	{
		uint8_t pad;

		pad = bm->width/FONT_WIDTH - PGM_FUNC(vsprintf)(NULL, fmt, ap);

		if (mode & TEXT_CENTER)
			pad /= 2;

		while (pad--)
			text_putchar(' ', bm);
	}

	len = PGM_FUNC(text_vprintf)(bm, fmt, ap);
	va_end(ap);

	if (mode & (TEXT_FILL | TEXT_CENTER))
		while (bm->penX + FONT_WIDTH < bm->width)
			text_putchar(' ', bm);

	if (mode & TEXT_INVERT)
		text_style(oldstyle, STYLEF_MASK);

	return len;
}

