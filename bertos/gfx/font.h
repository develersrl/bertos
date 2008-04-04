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
 *
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
 *#* Revision 1.7  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.6  2006/05/27 22:31:34  bernie
 *#* Switch to a smaller default fount.
 *#*
 *#* Revision 1.5  2006/03/13 02:06:04  bernie
 *#* FONT_HAS_GLYPH(): New macro.
 *#*
 *#* Revision 1.4  2006/02/15 09:10:15  bernie
 *#* Implement prop fonts; Fix algo styles.
 *#*
 *#* Revision 1.3  2006/02/10 12:29:05  bernie
 *#* Add multiple font support in bitmaps.
 *#*
 *#* Revision 1.2  2005/11/04 18:17:45  bernie
 *#* Fix header guards and includes for new location of gfx module.
 *#*
 *#* Revision 1.1  2005/11/04 18:11:35  bernie
 *#* Move graphics stuff from mware/ to gfx/.
 *#*
 *#* Revision 1.6  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
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
#ifndef GFX_FONT_H
#define GFX_FONT_H

#include <cfg/compiler.h> /* uint8_t */
#include <mware/pgm.h> /* PROGMEM */

typedef struct Font
{
	/**
	 * Pointer to glyph data.
	 *
	 * Data is an array of at most 256 glyphs packed together.
	 * Raster format must be the same of the bitmap.
	 */
	const PROGMEM uint8_t *glyph;

	uint8_t width;     /**< Pixel width of character cell. */
	uint8_t height;    /**< Pixel height of character cell. */

	uint8_t first;     /**< First encoded character in glyph array. */
	uint8_t last;      /**< Last encoded character in glyph array (inclusive). */

	/** Array of glyph offsets in bytes. NULL for fixed-width fonts. */
	const PROGMEM uint16_t *offset;
	const PROGMEM uint8_t *widths;

} Font;


/** Return true if glyph \a c is available in \a font. */
#define FONT_HAS_GLYPH(font, c) ((c) >= (font)->first && (c) <= (font)->last)


/** The default font. */
#define default_font font_luBS14
extern const struct Font default_font;

#endif /* GFX_FONT_H */
