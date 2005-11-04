/*!
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief Edit bool widget (implementation).
 * This widget handles boolean editing.
 * The boolean value will be displayed using two strings:
 * one when the bool is false and one when it's true.
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@ðeveler.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2005/11/04 18:26:38  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.3  2005/06/08 17:32:33  batt
 *#* Switch to new messaging system.
 *#*
 *#* Revision 1.2  2005/06/06 11:04:12  batt
 *#* Add some comments.
 *#*
 *#* Revision 1.1  2005/05/31 11:11:37  batt
 *#* Edit bool: first release.
 *#*
 *#*/

#include <mware/editbool.h>
#include <dt/dtag.h>

#include <drv/lcd_text.h>

/*!
 * Init widget.
 */
void editbool_init(DEditBool *e, dpos_t pos, dpos_t size, dcontext_t *context, bool *value, const char *true_string, const char *false_string)
{
	// Initialize superclass
	widget_init(&e->widget, pos, size, context);

	// Override superclass methods
	e->widget.notifier.update = (update_func_ptr)editbool_update;

	// Init instance
	e->value = value;
	e->true_string = true_string;
	e->false_string = false_string;
	e->draw = editbool_draw;
}

/*!
 * Handle the messages (edit the bool).
 */
void editbool_update(DEditBool *e, dtag_t tag, dval_t _val)
{
	bool changed = false;

	switch (tag)
	{
	case TAG_SETVALUE:
		*e->value = (bool)_val;
		changed = true;
		break;

	case TAG_TOGGLE:
		*e->value = !*e->value;
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

/*!
 * Draw the string on the context.
 */
void editbool_draw(DEditBool *e)
{
	lcd_printf((Layer *)e->widget.context, (lcdpos_t)e->widget.pos, LCD_NORMAL, "%*s", (int)e->widget.size , *e->value? e->true_string: e->false_string);
}
