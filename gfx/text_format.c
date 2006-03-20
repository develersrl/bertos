/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief printf-family routines for text output
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.7  2006/03/20 17:51:55  bernie
 *#* Cleanups.
 *#*
 *#* Revision 1.6  2006/03/13 02:05:54  bernie
 *#* Mark slow paths as UNLIKELY.
 *#*
 *#* Revision 1.5  2006/03/07 22:18:04  bernie
 *#* Correctly compute text width for prop fonts; Make styles a per-bitmap attribute.
 *#*
 *#* Revision 1.4  2006/02/10 12:31:33  bernie
 *#* Add multiple font support in bitmaps.
 *#*
 *#* Revision 1.3  2005/11/27 23:31:58  bernie
 *#* Reorder includes.
 *#*
 *#* Revision 1.2  2005/11/04 18:17:45  bernie
 *#* Fix header guards and includes for new location of gfx module.
 *#*
 *#* Revision 1.1  2005/11/04 18:11:35  bernie
 *#* Move graphics stuff from mware/ to gfx/.
 *#*
 *#* Revision 1.10  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.9  2004/12/31 17:47:45  bernie
 *#* Rename UNUSED() to UNUSED_ARG().
 *#*
 *#* Revision 1.8  2004/11/16 21:16:56  bernie
 *#* Update to new naming scheme in mware/gfx.c.
 *#*
 *#* Revision 1.7  2004/10/03 19:05:04  bernie
 *#* text_widthf(), text_vwidthf(): New functions.
 *#*
 *#* Revision 1.6  2004/09/14 20:59:04  bernie
 *#* text_xprintf(): Support all styles; Pixel-wise text centering.
 *#*
 *#* Revision 1.5  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.4  2004/08/05 18:46:44  bernie
 *#* Documentation improvements.
 *#*
 *#* Revision 1.3  2004/08/03 15:57:18  aleph
 *#* Add include to fix warning for vsprintf()
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 15:43:16  bernie
 *#* Import mware modules.
 *#*
 *#* Revision 1.2  2004/03/26 18:50:50  bernie
 *#* Move _PROGMEM stuff to compiler.h
 *#*
 *#* Revision 1.1  2004/03/19 16:52:28  bernie
 *#* Move printf() like functions from text.c to text_format.c and add PROGMEM versions.
 *#*
 *#*/

#include "text.h"

#include <mware/formatwr.h> /* _formatted_write() */
#include <gfx/font.h>
#include <gfx/gfx.h>

#include <stdio.h> /* vsprintf() */
#include <stdarg.h>
#include <string.h> /* strlen() */

/*!
 * Render string \a str in Bitmap \a bm at current cursor position
 *
 * \note Text formatting functions are also available with an _P suffix
 *       accepting the source string from program memory.  This feature
 *       is only available (and useful) on Harvard microprocessors such
 *       as the AVR.
 *
 * \see text_putchar()
 */
int PGM_FUNC(text_puts)(const char * PGM_ATTR str, struct Bitmap *bm)
{
	char c;

	while ((c = PGM_READ_CHAR(str++)))
		text_putchar(c, bm);

	return 0;
}


/*!
 * vprintf()-like formatter to render text in a Bitmap.
 *
 * Perform vprintf()-like formatting on the \a fmt format string using the
 * variable-argument list \a ap.
 * Render the resulting string in Bitmap \a bm starting at the current
 * cursor position.
 *
 * \see text_puts() text_putchar() text_printf()
 */
int PGM_FUNC(text_vprintf)(struct Bitmap *bm, const char * PGM_ATTR fmt, va_list ap)
{
	return PGM_FUNC(_formatted_write)(fmt, (void (*)(char, void *))text_putchar, bm, ap);
}

/*!
 * printf()-like formatter to render text in a Bitmap.
 *
 * Perform printf()-like formatting on the \a fmt format string.
 * Render the resulting string in Bitmap \a bm starting at the
 * current cursor position.
 *
 * \see text_puts() text_putchar() text_vprintf()
 */
int PGM_FUNC(text_printf)(struct Bitmap *bm, const char * PGM_ATTR fmt, ...)
{
	int len;

	va_list ap;
	va_start(ap, fmt);
	len = PGM_FUNC(text_vprintf)(bm, fmt, ap);
	va_end(ap);

	return len;
}


/*!
 * Render the result of printf()-like formatting in a specified position
 * of a Bitmap.
 *
 * \param bm Bitmap where to render the text
 * \param row   Starting row in character units (zero based)
 * \param col   Starting column in character units (zero based)
 * \param style Formatting style to use.  In addition to any STYLEF_
 *        flag, it can be TEXT_NORMAL, TEXT_FILL, TEXT_INVERT or
 *        TEXT_RIGHT, or a combination of these flags ORed together.
 * \param fmt  String possibly containing printf() formatting commands.
 *
 * \see text_puts() text_putchar() text_printf() text_vprintf()
 * \see text_moveto() text_style()
 */
int PGM_FUNC(text_xprintf)(struct Bitmap *bm,
		uint8_t row, uint8_t col, uint16_t style, const char * PGM_ATTR fmt, ...)
{
	int len;
	uint8_t oldstyle = 0;
	va_list ap;

	va_start(ap, fmt);

	text_moveto(bm, row, col);

	if (style & STYLEF_MASK)
		oldstyle = text_style(bm, style, STYLEF_MASK);

	if (style & (TEXT_CENTER | TEXT_RIGHT))
	{
		uint8_t pad = bm->width - PGM_FUNC(text_vwidthf)(bm, fmt, ap);

		if (style & TEXT_CENTER)
			pad /= 2;

		if (style & TEXT_FILL)
			gfx_rectFillC(bm, 0, row * bm->font->height, pad, (row + 1) * bm->font->height,
				(style & STYLEF_INVERT) ? 0xFF : 0x00);

		text_setcoord(bm, pad, row * bm->font->height);
	}

	len = PGM_FUNC(text_vprintf)(bm, fmt, ap);
	va_end(ap);

	if (style & TEXT_FILL)
		gfx_rectFillC(bm, bm->penX, row * bm->font->height, bm->width, (row + 1) * bm->font->height,
			(style & STYLEF_INVERT) ? 0xFF : 0x00);

	/* Restore old style */
	if (style & STYLEF_MASK)
		text_style(bm, oldstyle, STYLEF_MASK);

	return len;
}


struct TextWidthData
{
	Bitmap *bitmap;
	coord_t width;
};

/**
 * Compute width in pixels of a character.
 *
 * Compute the on screen width of a character, taking the
 * current style and font into account.
 *
 * The width is accumulated in the WidthData structure
 * passed as second argument.
 *
 * This is a formatted_write() callback used by text_vwidthf()
 * to compute the length of a formatted string.
 */
static int text_charWidth(int c, struct TextWidthData *twd)
{
	unsigned char index = (unsigned char)c;
	Bitmap *bm = twd->bitmap;
	coord_t glyph_width;


	if (UNLIKELY(!FONT_HAS_GLYPH(bm->font, index)))
	{
		if (!FONT_HAS_GLYPH(bm->font, '?'))
			index = '?';
		else
			index = bm->font->first;
	}

	/* Make character relative to font start */
	index -= bm->font->first;

	if (bm->font->offset)
		/* Proportional font */
		glyph_width = bm->font->widths[index]; /* TODO: optimize away */
	else
		/* Fixed width font */
		glyph_width = bm->font->width;

	if (bm->styles & STYLEF_CONDENSED)
		--glyph_width;

	if (bm->styles & STYLEF_EXPANDED)
		glyph_width *= 2;

	twd->width += glyph_width;

	return c;
}

/*!
 * Return the width in pixels of a vprintf()-formatted string.
 */
int PGM_FUNC(text_vwidthf)(
	UNUSED_ARG(struct Bitmap *, bm),
	const char * PGM_ATTR fmt,
	va_list ap)
{
	/* Fixed font with no styles affecting the width? */
	if (!bm->font->offset && !(bm->styles & (STYLEF_CONDENSED | STYLEF_EXPANDED)))
		return PGM_FUNC(vsprintf)(NULL, fmt, ap) * bm->font->width;
	else
	{
		struct TextWidthData twd = { bm, 0 };
		_formatted_write(fmt, (void (*)(char, void *))text_charWidth, &twd, ap);
		return twd.width;
	}
}


/*!
 * Return the width in pixels of a printf()-formatted string.
 */
int PGM_FUNC(text_widthf)(struct Bitmap *bm, const char * PGM_ATTR fmt, ...)
{
	int width;

	va_list ap;
	va_start(ap, fmt);
	width = PGM_FUNC(text_vwidthf)(bm, fmt, ap);
	va_end(ap);

	return width;
}
