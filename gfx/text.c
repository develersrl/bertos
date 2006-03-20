/*!
 * \file
 * <!--
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Text graphic routines
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
 *#* Revision 1.4  2006/02/15 09:10:15  bernie
 *#* Implement prop fonts; Fix algo styles.
 *#*
 *#* Revision 1.3  2006/02/10 12:31:55  bernie
 *#* Add multiple font support in bitmaps.
 *#*
 *#* Revision 1.2  2005/11/04 18:17:45  bernie
 *#* Fix header guards and includes for new location of gfx module.
 *#*
 *#* Revision 1.1  2005/11/04 18:11:35  bernie
 *#* Move graphics stuff from mware/ to gfx/.
 *#*
 *#* Revision 1.13  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.12  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.11  2005/01/20 18:46:31  aleph
 *#* Fix progmem includes.
 *#*
 *#* Revision 1.10  2005/01/08 09:20:12  bernie
 *#* Really make it work on both architectures.
 *#*
 *#* Revision 1.9  2004/12/31 16:44:29  bernie
 *#* Sanitize for non-Harvard processors.
 *#*
 *#* Revision 1.8  2004/11/16 21:16:28  bernie
 *#* Update to new naming scheme in mware/gfx.c.
 *#*
 *#* Revision 1.7  2004/09/20 03:28:28  bernie
 *#* Fix header.
 *#*
 *#* Revision 1.6  2004/09/14 20:57:15  bernie
 *#* Use debug.h instead of kdebug.h.
 *#*
 *#* Revision 1.5  2004/09/06 21:51:26  bernie
 *#* Extend interface to allow any algorithmic style.
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 15:43:16  bernie
 *#* Import mware modules.
 *#*
 *#* Revision 1.17  2004/05/15 16:57:01  aleph
 *#* Fixes for non-DEBUG build
 *#*
 *#* Revision 1.16  2004/04/03 20:42:49  aleph
 *#* Add text_clear()
 *#*
 *#* Revision 1.15  2004/03/24 15:03:45  bernie
 *#* Use explicit include paths; clean Doxygen comments
 *#*
 *#* Revision 1.14  2004/03/19 16:52:28  bernie
 *#* Move printf() like functions from text.c to text_format.c and add PROGMEM versions.
 *#*
 *#* Revision 1.13  2004/03/17 18:23:32  bernie
 *#* Oops.
 *#*
 *#* Revision 1.12  2004/03/17 18:03:22  bernie
 *#* Make diagnostic message shorter
 *#*
 *#* Revision 1.11  2004/03/13 22:52:54  aleph
 *#* documentation fixes
 *#*/

#include <gfx/gfx.h>
#include <gfx/font.h>
#include <gfx/text.h>
#include <gfx/text.h>

#include <gfx/gfx_p.h> // FIXME: BM_DRAWPIXEL

#include <cfg/debug.h>


/*! ANSI escape sequences flag: true for ESC state on */
static bool ansi_mode = false;


/*!
 * Move (imaginary) cursor to column and row specified.
 * Next text write will start a that row and col.
 */
void text_moveto(struct Bitmap *bm, int row, int col)
{
	ASSERT(col >= 0);
	ASSERT(col < bm->width / bm->font->width);
	ASSERT(row >= 0);
	ASSERT(row < bm->height / bm->font->height);

	bm->penX = col * bm->font->width;
	bm->penY = row * bm->font->height;
}


/*!
 * Move (imaginary) cursor to coordinates specified.
 */
void text_setcoord(struct Bitmap *bm, int x, int y)
{
	bm->penX = x;
	bm->penY = y;
}


/*!
 * Render char \a c on Bitmap \a bm.
 */
static int text_putglyph(char c, struct Bitmap *bm)
{
	const uint8_t * PROGMEM glyph;  /* font is in progmem */
	uint8_t glyph_width, glyph_height, glyph_height_bytes;
	unsigned char index = (unsigned char)c;

	/* Check for out of range char and replace with '?' or first char in font. */
	if (UNLIKELY(!FONT_HAS_GLYPH(bm->font, index)))
	{
		kprintf("Illegal char '%c' (0x%02x)\n", index, index);
		if (FONT_HAS_GLYPH(bm->font, '?'))
			index = '?';
		else
			index = bm->font->first;
	}

	/* Make character relative to font start */
	index -= bm->font->first;

	glyph_height = bm->font->height;
	// FIXME: for vertical fonts only
	glyph_height_bytes = ROUND_UP2(glyph_height, 8);

	if (bm->font->offset)
	{
		/* Proportional font */
		glyph_width = bm->font->widths[index]; /* TODO: optimize away */
		glyph = bm->font->glyph + bm->font->offset[index];
	}
	else
	{
		/*
		 * Fixed-width font: compute the first column of pixels
		 * of the selected glyph using the character code to index
		 * the glyph array.
		 */
		glyph_width = bm->font->width;

		//For horizontal fonts
		//glyph = bm->font->glyph + index * (((glyph_width + 7) / 8) * glyph_height);
		glyph = bm->font->glyph + index * glyph_height_bytes * glyph_width;
	}

	/* Slow path for styled glyphs */
	if (UNLIKELY(bm->styles))
	{
		uint8_t styles = bm->styles;
		uint8_t prev_dots = 0, italic_prev_dots = 0;
		uint8_t dots;
		uint8_t row, col;

		/* This style alone could be handled by the fast path too */
		if (bm->styles & STYLEF_CONDENSED)
			--glyph_width;

		if (bm->styles & STYLEF_EXPANDED)
			glyph_width *= 2;

		/* Check if glyph fits in the bitmap. */
		if ((bm->penX < 0) || (bm->penX + glyph_width > bm->width)
			|| (bm->penY < 0) || (bm->penY + glyph_height > bm->height))
		{
			kprintf("bad coords x=%d y=%d\n", bm->penX, bm->penY);
			return 0;
		}

		for (row = 0; row < glyph_height_bytes; ++row)
		{
			/* For each dot column in the glyph... */
			for (col = 0; col < glyph_width; ++col)
			{
				uint8_t src_col = col;
				uint8_t i;

				/* Expanded style: advances only once every two columns. */
				if (styles & STYLEF_EXPANDED)
					src_col /= 2;

				/* Fetch a column of dots from glyph. */
				dots = PGM_READ_CHAR(glyph + src_col * glyph_height_bytes + row);

				/* Italic: get lower 4 dots from previous column */
				if (styles & STYLEF_ITALIC)
				{
					uint8_t new_dots = dots;
					dots = (dots & 0xF0) | italic_prev_dots;
					italic_prev_dots = new_dots & 0x0F;
				}

				/* Bold: "or" pixels with the previous column */
				if (styles & STYLEF_BOLD)
				{
					uint8_t new_dots = dots;
					dots |= prev_dots;
					prev_dots = new_dots;
				}

				/* Underlined: turn on base pixel */
				if ((styles & STYLEF_UNDERLINE)
					&& (row == glyph_height_bytes - 1))
					dots |= (1 << (glyph_height - row * 8 - 1));

				/* Inverted: invert pixels */
				if (styles & STYLEF_INVERT)
					dots = ~dots;

				/* Output dots */
				for (i = 0; i < 8 && (row * 8) + i < glyph_height; ++i)
					BM_DRAWPIXEL(bm, bm->penX + col, bm->penY + row * 8 + i, dots & (1<<i));
			}
		}
	}
	else
	{
		/* No style: fast vanilla copy of glyph to bitmap */
		gfx_blitRaster(bm, bm->penX, bm->penY, glyph, glyph_width, glyph_height, glyph_height_bytes);
	}

	/* Update current pen position */
	bm->penX += glyph_width;

	return c;
}


/*!
 * Render char \c c, with (currently) limited ANSI escapes
 * emulation support and '\n' for newline.
 */
int text_putchar(char c, struct Bitmap *bm)
{
	/* Handle ANSI escape sequences */
	if (UNLIKELY(ansi_mode))
	{
		switch (c)
		{
		case ANSI_ESC_CLEARSCREEN:
			gfx_bitmapClear(bm);
			bm->penX = 0;
			bm->penY = 0;
			text_style(bm, 0, STYLEF_MASK);
			break;
		DB(default:
			kprintf("Unknown ANSI esc code: %x\n", c);)
		}
		ansi_mode = false;
	}
	else if (c == '\033')  /* Enter ANSI ESC mode */
	{
		ansi_mode = true;
	}
	else if (c == '\n')  /* Go one line down on a line-feed */
	{
		if (bm->penY + bm->font->height < bm->height)
		{
			bm->penY += bm->font->height;
			bm->penX = 0;
		}
	}
	else
	{
		text_putglyph(c, bm);
	}
	return c;
}


/*!
 * Clear the screen and reset cursor position
 */
void text_clear(struct Bitmap *bmp)
{
	text_putchar('\x1b', bmp);
	text_putchar('c', bmp);
}


void text_clearLine(struct Bitmap *bm, int line)
{
	gfx_rectClear(bm, 0, line * bm->font->height, bm->width, (line + 1) * bm->font->height);
}


/**
 * Set/clear algorithmic font style bits.
 *
 * \param bm     Pointer to Bitmap to affect.
 * \param flags  Style flags to set
 * \param mask   Mask of flags to modify
 * \return       Old style flags
 *
 * Examples:
 * Turn on bold, leave other styles alone
 *   \code text_style(bm, STYLEF_BOLD, STYLEF_BOLD); \endcode
 *
 * Turn off bold and turn on italic, leave others as they are
 *   \code text_style(bm, STYLEF_ITALIC, STYLEF_BOLD | STYLEF_ITALIC); \endcode
 *
 * Query current style without chaning it
 *   \code style = text_style(bm, 0, 0); \endcode
 *
 * Reset all styles (plain text)
 *   \code text_style(bm, 0, STYLE_MASK); \endcode
 */
uint8_t text_style(struct Bitmap *bm, uint8_t flags, uint8_t mask)
{
	uint8_t old = bm->styles;
	bm->styles = (bm->styles & ~mask) | flags;
	return old;
}
