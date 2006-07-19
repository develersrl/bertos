/**
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001, 2003 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Simple charts on top of mware/gfx routines (interface).
 *
 * Configuration:
 *  - \c CONFIG_CHART_TYPE_X: type for the input dataset of X-coordinates
 *  - \c CONFIG_CHART_TYPE_Y: type for the input dataset of Y-coordinates
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.4  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.3  2005/11/27 23:36:18  bernie
 *#* Use appconfig.h instead of cfg/config.h.
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
 *#* Revision 1.6  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
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
#ifndef GFX_CHARTS_H
#define GFX_CHARTS_H

#include <gfx/gfx.h>    /* vcoord_t */
#include <appconfig.h> /* CONFIG_ stuff */

/**
 * \name Width/height of the small ticks drawn over the axes
 * \{
 */
#define TICKS_HEIGHT     2
#define TICKS_WIDTH      2
/*\}*/

/**
 * \name Chart frame dimensions
 * \{
 */
#define CHART_BORDERTOP       0
#define CHART_BORDERBOTTOM    0
#define CHART_BORDERLEFT      0
#define CHART_BORDERRIGHT     0
/*\}*/

#ifndef CONFIG_CHART_TYPE_X
#define CONFIG_CHART_TYPE_X vcoord_t
#endif
#ifndef CONFIG_CHART_TYPE_Y
#define CONFIG_CHART_TYPE_Y vcoord_t
#endif


typedef CONFIG_CHART_TYPE_X chart_x_t;
typedef CONFIG_CHART_TYPE_Y chart_y_t;


/* Public function protos */
void chart_init(Bitmap *bm, coord_t xmin, coord_t ymin, coord_t xmax, coord_t ymax);
void chart_setScale(Bitmap *bm, chart_x_t xmin, chart_y_t ymin, chart_x_t xmax, chart_y_t ymax);
void chart_drawAxis(Bitmap *bm);
void chart_drawCurve(Bitmap *bm, const chart_y_t *curve_y, int curve_cnt);
void chart_drawDots(Bitmap *bm, const chart_x_t *dots_x, const chart_y_t *dots_y, int cnt);

#endif /* GFX_CHARTS_H */
