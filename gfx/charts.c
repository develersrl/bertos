/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001, 2003 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Simple charts on top of mware/gfx routines (implementation).
 *
 * Sample usage:
 *
 * \code
 *	bm = chart_init(0, ymax, N_POINTS_CURVE, ymin);
 *
 *	chart_drawCurve(bm, curve_y, curve_points + 1);
 *	gfx_setViewRect(bm, xmin, ymax, xmax, ymin);
 *	chart_drawDots(bm, samples_x, samples_y, samples_cnt);
 *
 *	print_bitmap(bm);
 * \endcode
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.3  2005/11/27 23:33:29  bernie
 *#* Reorder includes.
 *#*
 *#* Revision 1.2  2005/11/04 18:17:45  bernie
 *#* Fix header guards and includes for new location of gfx module.
 *#*
 *#* Revision 1.1  2005/11/04 18:11:35  bernie
 *#* Move graphics stuff from mware/ to gfx/.
 *#*
 *#* Revision 1.7  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.6  2004/11/16 21:04:23  bernie
 *#* Update to new naming scheme in mware/gfx.c.
 *#*
 *#* Revision 1.5  2004/09/14 20:56:39  bernie
 *#* Make more generic and adapt to new gfx functions.
 *#*
 *#* Revision 1.3  2004/08/11 19:39:12  bernie
 *#* Use chart_x_t and chart_y_t for the input dataset.
 *#*
 *#* Revision 1.1  2004/08/04 03:16:30  bernie
 *#* Import simple chart drawing code.
 *#*
 *#*/

#include "charts.h"
#include <gfx/gfx.h>


#ifndef CONFIG_CHART_ARROWS
#define CONFIG_CHART_ARROWS 0
#endif


void chart_init(Bitmap *bm, coord_t xmin, coord_t ymin, coord_t xmax, coord_t ymax)
{
	/* Clear the chart area */
	gfx_rectClear(bm, xmin, ymin, xmax, ymax);

	gfx_setClipRect(bm, xmin + CHART_BORDERLEFT, ymin + CHART_BORDERTOP,
		xmax - CHART_BORDERRIGHT, ymax - CHART_BORDERBOTTOM);

	chart_drawAxis(bm);
}


void chart_setScale(Bitmap *bm, chart_x_t xmin, chart_y_t ymin, chart_x_t xmax, chart_y_t ymax)
{
	gfx_setViewRect(bm, xmin, ymin, xmax, ymax);
}


/*!
 * Draw the chart axes
 */
void chart_drawAxis(Bitmap *bm)
{
#if CONFIG_CHART_ARROWS

	/* Draw axis */
	gfx_moveTo(bm, bm->cr.xmin, bm->cr.ymin + 4);
	gfx_lineTo(bm, bm->cr.xmin, bm->cr.ymax - 1);
	gfx_lineTo(bm, bm->cr.xmax - 4 - 1, bm->cr.ymax - 1);

	/* Draw up arrow */
	gfx_moveTo(bm, bm->cr.xmin - 2, bm->cr.ymin + 3);
	gfx_lineTo(bm, bm->cr.xmin + 2, bm->cr.ymin + 3);
	gfx_lineTo(bm, bm->cr.xmin, bm->cr.ymin);
	gfx_lineTo(bm, bm->cr.xmin - 2, bm->cr.ymin + 3);

	/* Draw right arrow */
	gfx_moveTo(bm, bm->cr.xmax - 4, bm->cr.ymax - 3);
	gfx_lineTo(bm, bm->cr.xmax - 4, bm->cr.ymax + 1);
	gfx_lineTo(bm, bm->cr.xmax - 1, bm->cr.ymax - 1);
	gfx_lineTo(bm, bm->cr.xmax - 4, bm->cr.ymax - 3);

#else /* CONFIG_CHART_ARROWS */

	/* Draw a box around the chart */
	gfx_rectDraw(bm, bm->cr.xmin, bm->cr.ymin, bm->cr.xmax, bm->cr.ymax);

#endif /* CONFIG_CHART_ARROWS */

	//CHECK_WALL(wall_before_raster, WALL_SIZE);
	//CHECK_WALL(wall_after_raster, WALL_SIZE);
}


/*!
 * Draw a set of \a curve_cnt connected segments, whose Y coordinates
 * are identified by the \a curve_y array and X-coordinates are
 * are evenly spaced by one virtual unit.
 */
void chart_drawCurve(Bitmap *bm, const chart_y_t *curve_y, int curve_cnt)
{
	int i;

	gfx_moveTo(bm, gfx_transformX(bm, 0), gfx_transformY(bm, curve_y[0]));

	for (i = 1; i < curve_cnt; i++)
		gfx_lineTo(bm, gfx_transformX(bm, i), gfx_transformY(bm, curve_y[i]));

	//CHECK_WALL(wall_before_raster, WALL_SIZE);
	//CHECK_WALL(wall_after_raster, WALL_SIZE);
}


/*!
 * Disegna dei dot in corrispondenza delle coppie (dotsx[i];dotsy[i])
 * Se dotsx e' NULL, i punti vengono disegnati ad intervalli regolari.
 */
void chart_drawDots(Bitmap *bm, const chart_x_t *dots_x, const chart_y_t *dots_y, int cnt)
{
	int i;
	coord_t x, y;

	for (i = 0; i < cnt; i++)
	{
		if (dots_x)
			x = gfx_transformX(bm, dots_x[i]);
		else
			x = gfx_transformX(bm, i);

		y = gfx_transformY(bm, dots_y[i]);

		/* Draw tick over the curve */
		gfx_rectFill(bm,
			x - TICKS_WIDTH / 2, y - TICKS_HEIGHT / 2,
			x + (TICKS_WIDTH + 1) / 2, y + (TICKS_HEIGHT + 1) / 2);

		/* Draw vertical line from the curve to the X-axis */
		//gfx_drawLine(bm, x, y, x, bm->cr.ymax - 1);
	}

	//CHECK_WALL(wall_before_raster, WALL_SIZE);
	//CHECK_WALL(wall_after_raster, WALL_SIZE);
}

