/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
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
 *#* Revision 1.8  2004/09/20 03:29:22  bernie
 *#* Relax assertion.
 *#*
 *#* Revision 1.7  2004/09/14 21:01:08  bernie
 *#* Rename rectangle drawing functions; Unify filled/cleared implementations.
 *#*
 *#* Revision 1.4  2004/08/24 16:53:10  bernie
 *#* Use new-style config macros.
 *#*
 *#* Revision 1.3  2004/08/04 03:16:59  bernie
 *#* Switch to new DevLib CONFIG_ convention.
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#*/

#include "gfx.h"
#include "config.h"  /* CONFIG_GFX_CLIPPING */
#include <debug.h>
#include <cpu.h>     /* CPU_AVR */
#include <macros.h>  /* SWAP() */

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
void gfx_InitBitmap(Bitmap *bm, uint8_t *raster, coord_t w, coord_t h)
{
	bm->raster = raster;
	bm->width = w;
	bm->height = h;
	bm->penX = 0;
	bm->penY = 0;
}


/*!
 * Clear the whole bitmap surface to all zeros
 *
 * \note This function does \b not update the current pen position
 */
void gfx_ClearBitmap(Bitmap *bm)
{
	memset(bm->raster, 0, (bm->width * bm->height) / 8);
}


#if CPU_AVR
/*!
 * Copy a raster picture located in program memory in the bitmap.
 * The size of the raster to copy *must* be the same of the raster bitmap.
 *
 * \note This function does \b not update the current pen position
 */
void gfx_blitBitmap_P(Bitmap *bm, const prog_uchar *raster)
{
	memcpy_P(bm->raster, raster, bm->height/8 * bm->width);
}
#endif /* CPU_AVR */


/*!
 * Draw a line on the bitmap \a bm using the specified start and end
 * coordinates.
 *
 * \note This function does \b not update the current pen position
 */
void gfx_DrawLine(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2)
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
void gfx_MoveTo(Bitmap *bm, coord_t x, coord_t y)
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
void gfx_LineTo(Bitmap *bm, coord_t x, coord_t y)
{
	gfx_DrawLine(bm, bm->penX, bm->penY, x, y);
	gfx_MoveTo(bm, x, y);
}


/*!
 * Draw an the outline of an hollow rectangle.
 *
 * \note The bottom-right border of the rectangle is not drawn.
 * \note This function does \b not update the current pen position
 */
void gfx_RectDraw(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2)
{
	/* Sort coords (needed for correct bottom-right semantics) */
	if (x1 > x2) SWAP(x1, x2);
	if (y1 > y2) SWAP(y1, y2);

	/* Draw rectangle */
	gfx_DrawLine(bm, x1,   y1,   x2-1, y1);
	gfx_DrawLine(bm, x2-1, y1,   x2-1, y2-1);
	gfx_DrawLine(bm, x2-1, y2-1, x1,   y2-1);
	gfx_DrawLine(bm, x1,   y2-1, x1,   y1);
}


/*!
 * Fill a rectangular area with \a color.
 *
 * \note The bottom-right border of the rectangle is not drawn.
 *
 * \note This function does \b not update the current pen position
 */
void gfx_RectFillC(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2, uint8_t color)
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
void gfx_RectFill(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2)
{
	gfx_RectFillC(bm, x1, y1, x2, y2, 0xFF);
}


/*!
 * Clear a rectangular area.
 *
 * \note The bottom-right border of the rectangle is not drawn.
 *
 * \note This function does \b not update the current pen position
 */
void gfx_RectClear(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2)
{
	gfx_RectFillC(bm, x1, y1, x2, y2, 0x00);
}


/*!
 * Imposta un rettangolo di clipping per il disegno nella bitmap
 */
void gfx_SetClipRect(Bitmap *bm, coord_t minx, coord_t miny, coord_t maxx, coord_t maxy)
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
void gfx_SetViewRect(Bitmap *bm, vcoord_t x1, vcoord_t y1, vcoord_t x2, vcoord_t y2)
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
coord_t gfx_TransformX(Bitmap *bm, vcoord_t x)
{
	return bm->cr.xmin + (coord_t)((x - bm->orgX) * bm->scaleX);
}

/*!
 * Transform a coordinate from the current reference system to a
 * pixel offset within the bitmap.
 */
coord_t gfx_TransformY(Bitmap *bm, vcoord_t y)
{
	return bm->cr.ymin + (coord_t)((y - bm->orgY) * bm->scaleY);
}


/*!
 * Draw a line from (x1;y1) to (x2;y2)
 */
void gfx_VDrawLine(Bitmap *bm, vcoord_t x1, vcoord_t y1, vcoord_t x2, vcoord_t y2)
{
	gfx_DrawLine(bm,
		gfx_TransformX(bm, x1), gfx_TransformY(bm, y1),
		gfx_TransformY(bm, x2), gfx_TransformY(bm, y2));
}
#endif /* CONFIG_GFX_VCOORDS */
