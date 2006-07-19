/**
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief Integer edit widget (implementation).
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.1  2005/11/04 18:26:38  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.7  2005/06/10 15:46:09  batt
 *#* Add EDIS_WRAP style that wrap around min and max.
 *#*
 *#* Revision 1.6  2005/06/08 17:32:33  batt
 *#* Switch to new messaging system.
 *#*
 *#* Revision 1.5  2005/06/06 11:04:12  batt
 *#* Add some comments.
 *#*
 *#* Revision 1.4  2005/05/31 11:09:34  batt
 *#* Fix sending pointer instead of value bug.
 *#*
 *#* Revision 1.3  2005/05/26 14:46:20  batt
 *#* Use correct tag; remove warning.
 *#*
 *#* Revision 1.2  2005/05/26 14:44:10  batt
 *#* Abstract widget from layer: use context.
 *#*
 *#* Revision 1.1  2005/05/26 08:32:53  batt
 *#* Add new Develer widget system :)
 *#*
 *#*/

#include <mware/editint.h>
#include <dt/dwidget.h>
#include <dt/dtag.h>
#include <dt/dnotifier.h>

#include <cfg/macros.h>

#include <drv/lcd_text.h>

/**
 * Init.
 */
void editint_init(DEditInt *e, dpos_t pos, dpos_t size, dcontext_t *context, int *value, int min, int max)
{
	// Initialize superclass
	widget_init(&e->widget, pos, size, context);

	// Override superclass methods
	e->widget.notifier.update = (update_func_ptr)editint_update;

	// Init instance
	e->value = value;
	e->min = min;
	e->max = max;
	e->style = EDIS_DEFAULT;
	e->draw = editint_draw;
}

/**
 * Handle the messages (edit the int).
 */
void editint_update(DEditInt *e, dtag_t tag, dval_t _val)
{
	bool changed = false;
	int val = (int)_val;

	switch (tag)
	{
	case TAG_SETVALUE:
		*e->value = MINMAX(e->min, val, e->max);
		changed = true;
		break;

	/* Increments the integer by val */
	case TAG_UP:
		if (e->style & EDIS_WRAP)
		{
			if (*e->value + val > e->max)
				*e->value = (*e->value + val - e->min) % (e->max - e->min + 1) + e->min;
			else
				*e->value += val;
		}
		else
			*e->value = MIN(*e->value + val, e->max);
		changed = true;
		break;
	/* Decrements the integer by val */
	case TAG_DOWN:
		if (e->style & EDIS_WRAP)
		{
			if (*e->value - val < e->min)
				*e->value = e->max - (e->max - (*e->value - val)) % (e->max - e->min + 1);
			else
				*e->value -= val;
		}
		else
			*e->value = MAX(*e->value - val, e->min);
		changed = true;
		break;

	default:
		break;
	}

	if (changed)
	{
		e->draw(e);
		dnotify_targets(&e->widget.notifier, TAG_SETVALUE, (dval_t)*e->value);
	}
}

/**
 * Draw the integer on the context.
 */
void editint_draw(DEditInt *e)
{
	lcd_printf((Layer *)e->widget.context, (lcdpos_t)e->widget.pos, LCD_NORMAL,"%*d", (int)e->widget.size, *e->value);
}
