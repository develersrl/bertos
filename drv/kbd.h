/*!
 * \file
 * <!--
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2003 Bernardo Innocenti
 * This file is part of DevLib - See README.devlib for information.
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
 *#* Revision 1.1  2005/06/27 21:28:45  bernie
 *#* Import generic keyboard driver.
 *#*
 *#*/
#ifndef DRV_KBD_H
#define DRV_KBD_H

#include <kbd_map.h>
#include <cfg/compiler.h>
#include <arch_config.h>
#include <mware/list.h>


/*!
 * Keyboard handler descriptor
 */
typedef struct KbdHandler
{
	Node link;
	keymask_t (*hook)(keymask_t);   /*!< Hook function */
	int8_t pri;                     /*!< Priority in input queue */
	uint8_t flags;                  /*!< See below for definitions */
} KbdHandler;

#define KHF_RAWKEYS	BV(0)           /*!< Handler gets raw key events */


extern void kbd_init(void);
extern keymask_t kbd_peek(void);
extern keymask_t kbd_get(void);
extern keymask_t kbd_get_timeout(mtime_t timeout);
extern void kbd_addHandler(struct KbdHandler *handler);
extern void kbd_remHandler(struct KbdHandler *handler);

#endif /* DRV_KBD_H */
