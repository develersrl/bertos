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
 * Copyright 1999, 2003 Bernardo Innocenti
 *
 * -->
 *
 * \brief Keyboard driver (interface)
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.6  2006/07/19 12:56:25  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.5  2006/06/03 13:57:36  bernie
 *#* Make keyboard repeat mask run-time configurable.
 *#*
 *#* Revision 1.4  2006/03/20 17:50:17  bernie
 *#* Add FreeRTOS and Observers support.
 *#*
 *#* Revision 1.3  2006/02/27 22:39:45  bernie
 *#* Misc build and doc fixes from project_grl.
 *#*
 *#* Revision 1.2  2006/02/10 12:38:16  bernie
 *#* Add preliminary FreeRTOS support; Enforce CONFIG_* definitions.
 *#*
 *#* Revision 1.1  2005/06/27 21:28:45  bernie
 *#* Import generic keyboard driver.
 *#*
 *#*/
#ifndef DRV_KBD_H
#define DRV_KBD_H

#include <kbd_map.h>
#include <cfg/compiler.h>
#include <mware/list.h>
#include <appconfig.h> // CONFIG_KBD_OBSERVER

/**
 * \name Keyboard polling modes.
 *
 * Define CONFIG_KBD_POLL to one of these.
 *
 * \{
 */
#define KBD_POLL_SOFTINT  1
#define KBD_POLL_FREERTOS 2
/* \} */

/**
 * Keyboard handler descriptor
 */
typedef struct KbdHandler
{
	Node link;
	keymask_t (*hook)(keymask_t);   /**< Hook function */
	int8_t pri;                     /**< Priority in input queue */
	uint8_t flags;                  /**< See below for definitions */
} KbdHandler;

#define KHF_RAWKEYS	BV(0)           /**< Handler gets raw key events */


void kbd_init(void);
keymask_t kbd_peek(void);
keymask_t kbd_get(void);
keymask_t kbd_get_timeout(mtime_t timeout);
void kbd_addHandler(struct KbdHandler *handler);
void kbd_remHandler(struct KbdHandler *handler);
keymask_t kbd_setRepeatMask(keymask_t mask);

#if CONFIG_KBD_OBSERVER
	struct Subject;

	/** Subject structure for keyboard observers. */
	extern struct Subject kbd_subject;

	enum
	{
		/* Event for key presses. */
		KBD_EVENT_KEY = 0x100
	};
#endif

#endif /* DRV_KBD_H */
