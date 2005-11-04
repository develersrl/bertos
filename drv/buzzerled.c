/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2004 Giovanni Bajo
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Generic library to handle buzzers and leds
 *
 * This library is divided into three different layers:
 *
 *  - The topmost portable layer is buzzerled.[ch] which exposes a common API
 *    enable/disable the devices. Basically, it handles the asynchronism to
 *    implement bld_beep and bld_repeat.
 *  - The middle layer is CPU-specific and exposes a single main function which
 *    turns on/off each device.
 *  - The lower layer is board-specific and communicates with the middle layer
 *    with any required API. The idea is that devices can be tied to the CPU in
 *    many different ways (many different pins), so this part should describe
 *    which devices are present, and how they are connected.
 *
 * \version $Id$
 *
 * \author Giovanni Bajo <rasky@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.6  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.5  2004/12/08 09:43:41  bernie
 *#* Add a todo item.
 *#*
 *#* Revision 1.4  2004/08/25 14:12:08  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.3  2004/07/14 14:04:29  rasky
 *#* Merge da SC: spostata bld_set inline perché si ottimizza parecchio tramite propagazione di costanti
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 18:36:05  bernie
 *#* Import buzzerled driver.
 *#*
 *#*/

#include "buzzerled.h"
#include "timer.h"

static struct Timer timers[NUM_BLDS];
static bool timer_go[NUM_BLDS];

INLINE enum BLD_DEVICE hook_parm_to_device(void* parm)
{
	struct Timer* t = (struct Timer*)parm;
	int num_bld = t - &timers[0];

	ASSERT(num_bld >= 0);
	ASSERT(num_bld < NUM_BLDS);

	return (enum BLD_DEVICE)num_bld;
}

static void hook_turn_off(void* parm)
{
	enum BLD_DEVICE num_bld = hook_parm_to_device(parm);
	bld_set(num_bld, false);
}

void bld_init(void)
{
	bld_hw_init();
}

void bld_beep(enum BLD_DEVICE device, uint16_t duration)
{
	// \todo This is not reentrant for the same device. FIXME!
	struct Timer *t = &timers[device];
	timer_set_delay(t, duration);
	timer_set_event_softint(t, hook_turn_off, t);
	timer_add(t);

	bld_set(device, true);
}

void bld_beep_and_wait(enum BLD_DEVICE device, uint16_t duration)
{
	bld_set(device, true);
	timer_delay(duration);
	bld_set(device, false);
}

