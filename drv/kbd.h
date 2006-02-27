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
