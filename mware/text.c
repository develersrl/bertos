/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
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

#include "gfx.h"
#include "font.h"
#include "text.h"
#include <debug.h>

/*!
 * Flags degli stili algoritmici
 *
 * La routine di rendering del testo e' in grado di applicare
 * delle semplici trasformazioni al font interno per generare
 * automaticamente degli stili predefiniti (bold, italic,
 * underline) a partire dal set di caratteri plain.
 */
static uint8_t text_styles;

/*! ANSI escape sequences flag: true for ESC state on */
static bool ansi_mode = false;


/*!
 * Move (imaginary) cursor to column and row specified.
 * Next text write will start a that row and col.
 */
void text_moveto(struct Bitmap *bm, int row, int col)
{
	ASSERT(col >= 0);
	ASSERT(col < bm->width / FONT_WIDTH);
	ASSERT(row >= 0);
	ASSERT(row < bm->height / FONT_HEIGHT);

	bm->penX = col * FONT_WIDTH;
	bm->penY = row * FONT_HEIGHT;
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
 * Render char \a c on Bitmap \a bm
 */
static int text_putglyph(char c, struct Bitmap *bm)
{
	const uint8_t * PROGMEM glyph;  /* font is in progmem */
	uint8_t glyph_width;
	uint8_t i;
	uint8_t *buf;

	/*
	 * Il carattere da stampare viene usato come indice per prelevare
	 * la prima colonna di dots del glyph all'interno del font.
	 */
	glyph = font + (((unsigned char)c) * FONT_WIDTH);
	glyph_width = FONT_WIDTH;

	if (text_styles & STYLEF_CONDENSED)
		--glyph_width;

	if (text_styles & STYLEF_EXPANDED)
		glyph_width *= 2;

	/* The y coord is rounded at multiples of 8 for simplicity */
	bm->penY &= ~((coord_t)7);

	/* Check if glyph to write fits in the bitmap */
	if ((bm->penX < 0) || (bm->penX + glyph_width > bm->width) ||
		(bm->penY < 0) || (bm->penY + FONT_HEIGHT > bm->height))
	{
		DB(kprintf("bad coords x=%d y=%d\n", bm->penX, bm->penY);)
		return 0;
	}

	/* Locate position where to write in the raster */
	buf = bm->raster + bm->penY / 8 * bm->width + bm->penX;

	bm->penX += glyph_width;

	/* If some styles are set */
	if (text_styles)
	{
		uint8_t prev_dots = 0, italic_prev_dots = 0, new_dots;
		uint8_t dots;

		/* Per ogni colonna di dot del glyph... */
		for (i = 0; i < glyph_width; ++i)
		{
			dots = pgm_read_byte(glyph);

			/* Advance to next column in glyph.
			 * Expand: advances only once every two columns
			 */
			if (!(text_styles & STYLEF_EXPANDED) || (i & 1))
				glyph++;

			/* Italic: get lower 4 dots from previous column */
			if (text_styles & STYLEF_ITALIC)
			{
				new_dots = dots;
				dots = (dots & 0xF0) | italic_prev_dots;
				italic_prev_dots = new_dots & 0x0F;
			}

			/* Bold: "or" pixels with the previous column */
			if (text_styles & STYLEF_BOLD)
			{
				new_dots = dots;
				dots |= prev_dots;
				prev_dots = new_dots;
			}

			/* Underlined: turn on base pixel */
			if (text_styles & STYLEF_UNDERLINE)
				dots |= 0x80;

			/* Inverted: invert pixels */
			if (text_styles & STYLEF_INVERT)
				dots = ~dots;

			/* Output dots */
			*buf++ = dots;
		}
	}
	else /* No style: fast vanilla copy of glyph to line buffer */
		while (glyph_width--)
			*buf++ = pgm_read_byte(glyph++);

	return c;
}


/*!
 * Render char \c c, with (currently) limited ANSI escapes
 * emulation support and '\n' for newline.
 */
int text_putchar(char c, struct Bitmap *bm)
{
	/* Handle ANSI escape sequences */
	if (ansi_mode)
	{
		switch (c)
		{
		case ANSI_ESC_CLEARSCREEN:
			gfx_ClearBitmap(bm);
			bm->penX = 0;
			bm->penY = 0;
			text_style(0, STYLEF_MASK);
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
		if (bm->penY + FONT_HEIGHT < bm->height)
		{
			bm->penY += FONT_HEIGHT;
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


void text_clearLine(struct Bitmap *bmp, int line)
{
	gfx_ClearRect(bmp, 0, line * FONT_HEIGHT, bmp->width, (line + 1) * FONT_HEIGHT);
}


/*!
 * Set/clear algorithmic font style bits.
 *
 * \param flags  Style flags to set
 * \param mask   Mask of flags to modify
 * \return       Old style flags
 *
 * Examples:
 * Turn on bold, leave other styles alone
 *   \code prt_style(STYLEF_BOLD, STYLEF_BOLD); \endcode
 *
 * Turn off bold and turn on italic, leave others as they are
 *   \code prt_style(STYLEF_ITALIC, STYLEF_BOLD | STYLEF_ITALIC); \endcode
 *
 * Query current style without chaning it
 *   \code style = prt_style(0, 0); \endcode
 *
 * Reset all styles (plain text)
 *   \code prt_style(0, STYLE_MASK); \endcode
 */
uint8_t text_style(uint8_t flags, uint8_t mask)
{
	uint8_t old = text_styles;
	text_styles = (text_styles & ~mask) | flags;
	return old;
}
