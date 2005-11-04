/*!
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief Integer edit (interface).
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2005/11/04 18:26:38  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.3  2005/06/10 15:46:09  batt
 *#* Add EDIS_WRAP style that wrap around min and max.
 *#*
 *#* Revision 1.2  2005/05/26 14:44:10  batt
 *#* Abstract widget from layer: use context.
 *#*
 *#* Revision 1.1  2005/05/26 08:32:53  batt
 *#* Add new Develer widget system :)
 *#*
 *#*/

#ifndef DT_EDITINT_H
#define DT_EDITINT_H

#include <dt/dwidget.h>
#include <dt/dtag.h>
#include <drv/lcd_text.h>

#include <cfg/compiler.h>
#include <cfg/macros.h>


#define EDIS_DEFAULT        0
#define EDIS_CURSORLEFT     BV(0)
#define EDIS_WRAP           BV(1)

/*! Type for edit style */
typedef uint16_t dstyle_t;

typedef struct DEditInt
{
	DWidget widget;

	int *value;
	int min;
	int max;
	dstyle_t style;
	void (*draw)(struct DEditInt *);

} DEditInt;

void editint_init(DEditInt *e, dpos_t pos, dpos_t size, dcontext_t *context, int *value, int min, int max);
void editint_update(DEditInt *e, dtag_t tag, dval_t val);
void editint_draw(DEditInt *e);

#endif
