/*!
 * \file
 * <!--
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief General pourpose graphics routines
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2005/11/04 18:17:45  bernie
 *#* Fix header guards and includes for new location of gfx module.
 *#*
 *#* Revision 1.1  2005/11/04 18:11:35  bernie
 *#* Move graphics stuff from mware/ to gfx/.
 *#*
 *#* Revision 1.14  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.13  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.12  2005/03/01 23:26:45  bernie
 *#* Use new CPU-neutral program-memory API.
 *#*/

#include <gfx/gfx.h>
#include <cfg/config.h>  /* CONFIG_GFX_CLIPPING */
#include <cfg/debug.h>
#include <cfg/cpu.h>     /* CPU_HARVARD */
#include <cfg/macros.h>  /* SWAP() */

#include <string.h>


/*!
 * Plot a point in bitmap \a bm.
 *
 * \note bm is evaluated twice
 */
#define BM_PLOT(bm, x, y) \
	( *((bm)->raster + ((y) / 8) * (bm)->width + (x)) |= 1 << ((y) % 8) )

/*!
 * Clear a point in bitmap \a bm.
 *
 * \note bm is evaluated twice
 */
#define BM_CLEAR(bm, x, y) \
	( *((bm)->raster + ((y) / 8) * (bm)->width + (x)) &= ~(1 << ((y) % 8)) )

/*!
 * Set a point in bitmap \a bm to the specified color.
 *
 * \note bm is evaluated twice
 * \note This macro is somewhat slower than BM_PLOT and BM_CLEAR.
 * \see BM_PLOT BM_CLEAR
 */
#define BM_DRAWPIXEL(bm, x, y, fg_pen) \
	do { \
		uint8_t *p = (bm)->raster + ((y) / 8) * (bm)->width + (x); \
		uint8_t mask = 1 << ((y) % 8); \
		*p = (*p & ~mask) | ((fg_pen) ? mask : 0); \
	} while (0)

/*!
 * Initialize a Bitmap structure with the provided parameters.
 *
 * \note The pen position is reset to the origin.
 */
void gfx_bitmapInit(Bitmap *bm, uint8_t *raster, coord_t w, coord_t h)
{
	bm->raster = raster;
	bm->width = w;
	bm->height = h;
	bm->penX = 0;
	bm->penY = 0;
}


/*!
 * Clear the whole bitmap surface to the background color.
 *
 * \note This function does \b not update the current pen position
 */
void gfx_bitmapClear(Bitmap *bm)
{
	memset(bm->raster, 0, (bm->width * bm->height) / 8);
}


#if CPU_HARVARD

#include <avr/pgmspace.h> /* FIXME: memcpy_P() */

/*!
 * Copy a raster picture located in program memory in the bitmap.
 * The size of the raster to copy *must* be the same of the raster bitmap.
 *
 * \note This function does \b not update the current pen position
 */
void gfx_blit_P(Bitmap *bm, const pgm_uint8_t *raster)
{
	memcpy_P(bm->raster, raster, (bm->height / 8) * bm->width);
}
#endif /* CPU_HARVARD */


/*!
 * Draw a line on the bitmap \a bm using the specified start and end
 * coordinates.
 *
 * \note This function does \b not update the current pen position.
 *
 * \todo Optimize for vertical and horiziontal lines.
 */
void gfx_line(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2)
{
	int x, y, e, len, adx, ady, signx, signy;


#if CONFIG_GFX_CLIPPING
	/* FIXME: broken */

	#define XMIN 0
	#define YMIN 0
	#define XMAX (bm->width - 1)
	#define YMAX (bm->height - 1)

	/* Clipping */
	if (x1 < XMIN)
	{
		y1 = y2 - ((x2 - XMIN) * (y2 - y1)) / (x2 - x1);
		x1 = XMIN;
	}
	if (y1 < YMIN)
	{
		x1 = x2 - ((y2 - YMIN) * (x2 - x1)) / (y2 - y1);
		y1 = YMIN;
	}
	if (x2 < XMIN)
	{
		y2 = y2 - ((XMIN - x1) * (y2 - y1)) / (x2 - x1);
		x2 = XMIN;
	}
	if (y2 < YMIN)
	{
		x2 = x2 - ((YMIN - y1) * (x2 - x1)) / (y2 - y1);
		y2 = YMIN;
	}

	if (x1 > XMAX)
	{
		y1 = ((x2 - XMAX) * (y2 - y1)) / (x2 - x1);
		x1 = XMAX;
	}
	if (y1 > YMAX)
	{
		x1 = ((y2 - YMAX) * (x2 - x1)) / (y2 - y1);
		y1 = YMAX;
	}
	if (x2 > XMAX)
	{
		y2 = ((XMAX - x1) * (y2 - y1)) / (x2 - x1);
		x2 = XMAX;
	}
	if (y2 > YMAX)
	{
		x2 = ((YMAX - y1) * (x2 - x1)) / (y2 - y1);
		y2 = YMAX;
	}

	#undef XMIN
	#undef YMIN
	#undef XMAX
	#undef YMAX

#endif /* CONFIG_GFX_CLIPPING */


	if (x2 > x1)
	{
		/* left to right */
		signx = +1;
		adx = x2 - x1;
	}
	else
	{
		/* right to left */
		signx = -1;
		adx = x1 - x2;
	}

	if (y2 > y1)
	{
		/* top to bottom */
		signy = +1;
		ady = y2 - y1;
	}
	else
	{
		/* bottom to top */
		signy = -1;
		ady = y1 - y2;
	}

	x = x1;
	y = y1;

	if (adx > ady)
	{
		/* X-major line (octants 1/4/5/8) */

		len = adx;
		e = -adx;
		while (len--)
		{
			/* Sanity check */
			if ((x >= 0) && (x < bm->width) && (y >= 0) && (y < bm->height))
				BM_PLOT(bm, x, y);
			x += signx;
			e += ady;
			if (e >= 0)
			{
				y += signy;
				e -= adx;
			}
		}
	}
	else
	{
		/* Y-major line (octants 2/3/6/7) */

		len = ady;
		e = -ady;
		while (len--)
		{
			/* Sanity check */
			if ((x >= 0) && (x < bm->width) && (y >= 0) && (y < bm->height))
				BM_PLOT(bm, x, y);
			y += signy;
			e += adx;
			if (e >= 0)
			{
				x += signx;
				e -= ady;
			}
		}
	}
}


/*!
 * Move the current pen position to the specified coordinates.
 */
void gfx_moveTo(Bitmap *bm, coord_t x, coord_t y)
{
	bm->penX = x;
	bm->penY = y;
}

/*!
 * Draw a line from the current pen position to the new coordinates.
 *
 * \note This function moves the current pen position to the
 *       new coordinates.
 */
void gfx_lineTo(Bitmap *bm, coord_t x, coord_t y)
{
	gfx_line(bm, bm->penX, bm->penY, x, y);
	gfx_moveTo(bm, x, y);
}


/*!
 * Draw an the outline of an hollow rectangle.
 *
 * \note The bottom-right border of the rectangle is not drawn.
 * \note This function does \b not update the current pen position
 */
void gfx_rectDraw(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2)
{
	/* Sort coords (needed for correct bottom-right semantics) */
	if (x1 > x2) SWAP(x1, x2);
	if (y1 > y2) SWAP(y1, y2);

	/* Draw rectangle */
	gfx_line(bm, x1,   y1,   x2-1, y1);
	gfx_line(bm, x2-1, y1,   x2-1, y2-1);
	gfx_line(bm, x2-1, y2-1, x1,   y2-1);
	gfx_line(bm, x1,   y2-1, x1,   y1);
}


/*!
 * Fill a rectangular area with \a color.
 *
 * \note The bottom-right border of the rectangle is not drawn.
 *
 * \note This function does \b not update the current pen position
 */
void gfx_rectFillC(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2, uint8_t color)
{
	coord_t x, y;

	/* Sort coords */
	if (x1 > x2) SWAP(x1, x2);
	if (y1 > y2) SWAP(y1, y2);

	/* Clip rect to bitmap bounds */
	if (x1 < 0)             x1 = 0;
	if (x2 < 0)             x2 = 0;
	if (x1 > bm->width)     x1 = bm->width;
	if (x2 > bm->width)     x2 = bm->width;
	if (y1 < 0)             y1 = 0;
	if (y2 < 0)             y2 = 0;
	if (y1 > bm->width)     y1 = bm->width;
	if (y2 > bm->width)     y2 = bm->width;

	/*
	 * Draw rectangle
	 * NOTE: Code paths are duplicated for efficiency
	 */
	if (color) /* fill */
	{
		for (x = x1; x < x2; x++)
			for (y = y1; y < y2; y++)
				BM_PLOT(bm, x, y);
	}
	else /* clear */
	{
		for (x = x1; x < x2; x++)
			for (y = y1; y < y2; y++)
				BM_CLEAR(bm, x, y);
	}
}


/*!
 * Draw a filled rectangle.
 *
 * \note The bottom-right border of the rectangle is not drawn.
 *
 * \note This function does \b not update the current pen position
 */
void gfx_rectFill(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2)
{
	gfx_rectFillC(bm, x1, y1, x2, y2, 0xFF);
}


/*!
 * Clear a rectangular area.
 *
 * \note The bottom-right border of the rectangle is not drawn.
 *
 * \note This function does \b not update the current pen position
 */
void gfx_rectClear(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2)
{
	gfx_rectFillC(bm, x1, y1, x2, y2, 0x00);
}


/*!
 * Imposta un rettangolo di clipping per il disegno nella bitmap.
 */
void gfx_setClipRect(Bitmap *bm, coord_t minx, coord_t miny, coord_t maxx, coord_t maxy)
{
	ASSERT(minx < maxx);
	ASSERT(miny < maxy);
	ASSERT(miny >= 0);
	ASSERT(minx >= 0);
	ASSERT(maxx <= bm->width);
	ASSERT(maxy <= bm->height);

	bm->cr.xmin = minx;
	bm->cr.ymin = miny;
	bm->cr.xmax = maxx;
	bm->cr.ymax = maxy;

/*	DB(kprintf("cr.xmin = %d, cr.ymin = %d, cr.xmax = %d, cr.ymax = %d\n",
		bm->cr.xMin, bm->cr.ymin, bm->cr.xmax, bm->cr.ymax);)
*/
}


#if CONFIG_GFX_VCOORDS
/*!
 * Imposta gli estremi del sistema di coordinate cartesiane rispetto
 * al rettangolo di clipping della bitmap.
 */
void gfx_setViewRect(Bitmap *bm, vcoord_t x1, vcoord_t y1, vcoord_t x2, vcoord_t y2)
{
	ASSERT(x1 != x2);
	ASSERT(y1 != y2);

	bm->orgX    = x1;
	bm->orgY    = y1;
	bm->scaleX  = (vcoord_t)(bm->cr.xmax - bm->cr.xmin - 1) / (vcoord_t)(x2 - x1);
	bm->scaleY  = (vcoord_t)(bm->cr.ymax - bm->cr.ymin - 1) / (vcoord_t)(y2 - y1);

/*	DB(kprintf("orgX = %f, orgY = %f, scaleX = %f, scaleY = %f\n",
		bm->orgX, bm->orgY, bm->scaleX, bm->scaleY);)
*/
}


/*!
 * Transform a coordinate from the current reference system to a
 * pixel offset within the bitmap.
 */
coord_t gfx_transformX(Bitmap *bm, vcoord_t x)
{
	return bm->cr.xmin + (coord_t)((x - bm->orgX) * bm->scaleX);
}

/*!
 * Transform a coordinate from the current reference system to a
 * pixel offset within the bitmap.
 */
coord_t gfx_transformY(Bitmap *bm, vcoord_t y)
{
	return bm->cr.ymin + (coord_t)((y - bm->orgY) * bm->scaleY);
}


/*!
 * Draw a line from (x1;y1) to (x2;y2).
 */
void gfx_vline(Bitmap *bm, vcoord_t x1, vcoord_t y1, vcoord_t x2, vcoord_t y2)
{
	gfx_line(bm,
		gfx_transformX(bm, x1), gfx_transformY(bm, y1),
		gfx_transformY(bm, x2), gfx_transformY(bm, y2));
}
#endif /* CONFIG_GFX_VCOORDS */
