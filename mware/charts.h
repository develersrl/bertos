/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001, 2003 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
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

/*
 * $Log$
 * Revision 1.3  2004/08/11 19:39:12  bernie
 * Use chart_x_t and chart_y_t for the input dataset.
 *
 * Revision 1.1  2004/08/04 03:16:30  bernie
 * Import simple chart drawing code.
 *
 */
#ifndef MWARE_CHARTS_H
#define MWARE_CHARTS_H

#include <mware/gfx.h> /* vcoord_t */
#include <config.h> /* CONFIG_ stuff */

/*!
 * \name Width/height of the small ticks drawn over the axes
 * \{
 */
#define TICKS_HEIGHT     2
#define TICKS_WIDTH      3
/*\}*/

/*!
 * \name Chart frame dimensions
 * \{
 */
#define CHART_BORDERTOP       0
#define CHART_BORDERBOTTOM    TICKS_HEIGHT
#define CHART_BORDERLEFT      TICKS_WIDTH
#define CHART_BORDERRIGHT     0
/*\}*/

/*!
 * \name Chart size in pixels
 * \{
 */
#define CHART_WIDTH     (bm->width - CHART_BORDERLEFT - CHART_BORDERRIGHT)
#define CHART_HEIGHT    (bm->height  - CHART_BORDERTOP - CHART_BORDERBOTTOM)
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
extern void chart_init(Bitmap *bm, vcoord_t xmin, vcoord_t ymin, vcoord_t xmax, vcoord_t ymax);
extern void chart_drawAxis(Bitmap *bm);
extern void chart_drawCurve(Bitmap *bm, const chart_y_t *curve_y, int curve_cnt);
extern void chart_drawDots(Bitmap *bm, const chart_x_t *dots_x, const chart_y_t *dots_y, int cnt);

#endif /* MWARE_CHARTS_H */
