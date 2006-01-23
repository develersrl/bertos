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
 *#* Revision 1.6  2006/01/23 23:13:39  bernie
 *#* gfx_blit(): New function, but dog slow for now.
 *#*
 *#* Revision 1.5  2006/01/17 22:59:23  bernie
 *#* Implement correct line clipping algorithm.
 *#*
 *#* Revision 1.4  2006/01/17 02:31:29  bernie
 *#* Add bitmap format support; Improve some comments.
 *#*
 *#* Revision 1.3  2005/11/27 23:33:40  bernie
 *#* Use appconfig.h instead of cfg/config.h.
 *#*
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

#include "gfx.h"
#include <appconfig.h>  /* CONFIG_GFX_CLIPPING */
#include <cfg/debug.h>
#include <cfg/cpu.h>     /* CPU_HARVARD */
#include <cfg/macros.h>  /* SWAP() */

#include <string.h>

/**
 * \name Known pixel formats for bitmap representation.
 * \{
 */
#define BITMAP_FMT_PLANAR_H_MSB  1  /**< Planar pixels, horizontal bytes, MSB left. */
#define BITMAP_FMT_PLANAR_V_LSB  2  /**< Planar pixels, vertical bytes, LSB top. */
/* \} */

#if CONFIG_BITMAP_FMT == BITMAP_FMT_PLANAR_H_MSB

	#define BM_ADDR(bm, x, y)  ((bm)->raster + (y) * (bm)->stride + ((x) / 8))
	#define BM_MASK(bm, x, y)  (1 << (7 - (x) % 8))

#elif CONFIG_BITMAP_FMT == BITMAP_FMT_PLANAR_V_LSB

	#define BM_ADDR(bm, x, y)  ((bm)->raster + ((y) / 8) * (bm)->stride + (x))
	#define BM_MASK(bm, x, y)  (1 << ((y) % 8))

#else
	#error Unknown value of CONFIG_BITMAP_FMT
#endif /* CONFIG_BITMAP_FMT */

/*!
 * Plot a pixel in bitmap \a bm.
 *
 * \note bm is evaluated twice.
 * \see BM_CLEAR BM_DRAWPIXEL
 */
#define BM_PLOT(bm, x, y) \
	( *BM_ADDR(bm, x, y) |= BM_MASK(bm, x, y) )

/*!
 * Clear a pixel in bitmap \a bm.
 *
 * \note bm is evaluated twice.
 * \see BM_PLOT BM_DRAWPIXEL
 */
#define BM_CLEAR(bm, x, y) \
	( *BM_ADDR(bm, x, y) &= ~BM_MASK(bm, x, y) )

/*!
 * Set a pixel in bitmap \a bm to the specified color.
 *
 * \note bm is evaluated twice.
 * \note This macro is somewhat slower than BM_PLOT and BM_CLEAR.
 * \see BM_PLOT BM_CLEAR
 */
#define BM_DRAWPIXEL(bm, x, y, fg_pen) \
	do { \
		uint8_t *p = BM_ADDR(bm, x, y); \
		uint8_t mask = BM_MASK(bm, x, y); \
		*p = (*p & ~mask) | ((fg_pen) ? mask : 0); \
	} while (0)

/*!
 * Get the value of the pixel in bitmap \a bm.
 *
 * \return The returned value is either 0 or 1.
 *
 * \note bm is evaluated twice.
 * \see BM_DRAWPIXEL
 */
#define BM_READPIXEL(bm, x, y) \
	( *BM_ADDR(bm, x, y) & BM_MASK(bm, x, y) ? 1 : 0 )

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
	#if (CONFIG_BITMAP_FMT == BITMAP_FMT_PLANAR_H_MSB)
		bm->stride = (w + 7) / 8;
	#elif CONFIG_BITMAP_FMT == BITMAP_FMT_PLANAR_V_LSB
		bm->stride = w;
	#else
		#error Unknown value of CONFIG_BITMAP_FMT
	#endif /* CONFIG_BITMAP_FMT */
	bm->penX = 0;
	bm->penY = 0;

#if CONFIG_GFX_CLIPPING
	bm->cr.xmin = 0;
	bm->cr.ymin = 0;
	bm->cr.xmax = w;
	bm->cr.ymax = h;
#endif /* CONFIG_GFX_CLIPPING */
}


/*!
 * Clear the whole bitmap surface to the background color.
 *
 * \note This function does \b not update the current pen position.
 */
void gfx_bitmapClear(Bitmap *bm)
{
	memset(bm->raster, 0, RASTER_SIZE(bm->width, bm->height));
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

/**
 * Copy a rectangular area of a bitmap on another bitmap.
 *
 * Blitting is a common copy operation involving two bitmaps.
 * A rectangular area of the source bitmap is copied bit-wise
 * to a different position in the destination bitmap.
 *
 * \note Using the same bitmap for \a src and \a dst is unsupported.
 *
 * \param dst Bitmap where the operation writes
 *
 */
void gfx_blit(Bitmap *dst, Rect *rect, Bitmap *src, coord_t srcx, coord_t srcy)
{
	coord_t dxmin, dymin, dxmax, dymax;
	coord_t dx, dy, sx, sy;

	/*
	 * Clip coordinates inside dst->cr and src->width/height.
	 */
	dxmin = rect->xmin;
	if (dxmin < dst->cr.xmin)
	{
		srcx += dst->cr.xmin - dxmin;
		dxmin = dst->cr.xmin;
	}
	dymin = rect->ymin;
	if (dymin < dst->cr.ymin)
	{
		srcy += dst->cr.ymin - dymin;
		dymin = dst->cr.ymin;
	}
	dxmax = MIN(MIN(rect->xmax, rect->xmin + src->width), dst->cr.xmax);
	dymax = MIN(MIN(rect->ymax, rect->ymin + src->height), dst->cr.ymax);

	/* TODO: make it not as dog slow as this */
	for (dx = dxmin, sx = srcx; dx < dxmax; ++dx, ++sx)
		for (dy = dymin, sy = srcy; dy < dymax; ++dy, ++sy)
			BM_DRAWPIXEL(dst, dx, dy, BM_READPIXEL(src, sx, sy));
}

/*!
 * Draw a sloped line without performing clipping.
 *
 * Parameters are the same of gfx_line().
 * This routine is based on the Bresenham Line-Drawing Algorithm.
 *
 * \note Passing coordinates outside the bitmap boundaries will
 *       result in memory trashing.
 *
 * \todo Optimize for vertical and horiziontal lines.
 *
 * \sa gfx_line()
 */
static void gfx_lineUnclipped(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2)
{
	int x, y, e, len, adx, ady, signx, signy;

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
			ASSERT((x >= 0) && (x < bm->width) && (y >= 0) && (y < bm->height));
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
			ASSERT ((x >= 0) && (x < bm->width) && (y >= 0) && (y < bm->height));
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

//! Helper routine for gfx_line().
static int gfx_findRegion(int x, int y, Rect *cr)
{
	int code = 0;

	if (y >= cr->ymax)
		code |= 1; /* below */
	else if (y < cr->ymin)
		code |= 2; /* above */

	if (x >= cr->xmax)
		code |= 4; /* right */
	else if (x < cr->xmin)
		code |= 8; /* left */

	return code;
}

/**
 * Draw a sloped line segment.
 *
 * Draw a sloped line segment identified by the provided
 * start and end coordinates on the bitmap \a bm.
 *
 * The line endpoints are clipped inside the current bitmap
 * clipping rectangle using the Cohen-Sutherland algorithm,
 * which is very fast.
 *
 * \note The point at coordinates \a x2 \a y2 is not drawn.
 *
 * \note This function does \b not update the current pen position.
 *
 * \todo Compute updated Bresenham error term.
 */
void gfx_line(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2)
{
#if CONFIG_GFX_CLIPPING
	int clip1 = gfx_findRegion(x1, y1, &bm->cr);
	int clip2 = gfx_findRegion(x2, y2, &bm->cr);

	/* Loop while there is at least one point outside */
	while (clip1 | clip2)
	{
		/* Check for line totally outside */
		if (clip1 & clip2)
			return;

		int c = clip1 ? clip1 : clip2;
		int x, y;

		if (c & 1) /* Below */
		{
			x = x1 + (x2 - x1) * (bm->cr.ymax - y1) / (y2 - y1);
			y = bm->cr.ymax - 1;
		}
		else if (c & 2) /* Above */
		{
			x = x1 + (x2 - x1) * (bm->cr.ymin - y1) / (y2 - y1);
			y = bm->cr.ymin;
		}
		else if (c & 4) /* Right */
		{
			y = y1 + (y2 - y1) * (bm->cr.xmax - x1) / (x2 - x1);
			x = bm->cr.xmax - 1;
		}
		else /* Left */
		{
			y = y1 + (y2 - y1) * (bm->cr.xmin - x1) / (x2 - x1);
			x = bm->cr.xmin;
		}

		if (c == clip1) /* First endpoint was clipped */
		{
			// TODO: adjust Bresenham error term
			//coord_t clipdx = ABS(x - x1);
			//coord_t clipdy = ABS(y - y1);
			//e += (clipdy * e2) + ((clipdx - clipdy) * e1);

			x1 = x;
			y1 = y;
			clip1 = gfx_findRegion(x1, y1, &bm->cr);
		}
		else /* Second endpoint was clipped */
		{
			x2 = x;
			y2 = y;
			clip2 = gfx_findRegion(x2, y2, &bm->cr);
		}
	}
#endif /* CONFIG_GFX_CLIPPING */

	gfx_lineUnclipped(bm, x1, y1, x2, y2);
}

/*!
 * Move the current pen position to the specified coordinates.
 *
 * The pen position is used for drawing operations such as
 * gfx_lineTo(), which can be used to draw polygons.
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
 *
 * \sa gfx_line()
 */
void gfx_lineTo(Bitmap *bm, coord_t x, coord_t y)
{
	gfx_line(bm, bm->penX, bm->penY, x, y);
	gfx_moveTo(bm, x, y);
}


/*!
 * Draw the perimeter of an hollow rectangle.
 *
 * \note The bottom-right corner of the rectangle is drawn at (x2-1;y2-1).
 * \note This function does \b not update the current pen position.
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
 * \note This function does \b not update the current pen position.
 */
void gfx_rectFillC(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2, uint8_t color)
{
	coord_t x, y;

	/* Sort coords */
	if (x1 > x2) SWAP(x1, x2);
	if (y1 > y2) SWAP(y1, y2);

#if CONFIG_GFX_CLIPPING
	/* Clip rect to bitmap clip region */
	if (x1 < bm->cr.xmin)   x1 = bm->cr.xmin;
	if (x2 < bm->cr.xmin)   x2 = bm->cr.xmin;
	if (x1 > bm->cr.xmax)   x1 = bm->cr.xmax;
	if (x2 > bm->cr.xmax)   x2 = bm->cr.xmax;
	if (y1 < bm->cr.ymin)   y1 = bm->cr.ymin;
	if (y2 < bm->cr.ymin)   y2 = bm->cr.ymin;
	if (y1 > bm->cr.ymax)   y1 = bm->cr.ymax;
	if (y2 > bm->cr.ymax)   y2 = bm->cr.ymax;
#endif

	/* NOTE: Code paths are duplicated for efficiency */
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
 * \note This function does \b not update the current pen position.
 */
void gfx_rectFill(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2)
{
	gfx_rectFillC(bm, x1, y1, x2, y2, 0xFF);
}


/*!
 * Clear a rectangular area.
 *
 * \note The bottom-right border of the rectangle is not cleared.
 *
 * \note This function does \b not update the current pen position.
 */
void gfx_rectClear(Bitmap *bm, coord_t x1, coord_t y1, coord_t x2, coord_t y2)
{
	gfx_rectFillC(bm, x1, y1, x2, y2, 0x00);
}


/*!
 * Set the bitmap clipping rectangle to the specified coordinates.
 *
 * All drawing performed on the bitmap will be clipped inside this
 * rectangle.
 *
 * \note Following the convention used in all other operations, the
 *       top-left pixels of the rectangle are included, while the
 *       bottom-right pixels are considered outside the clipping region.
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
