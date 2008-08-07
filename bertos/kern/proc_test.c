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
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 *
 * \brief Test kernel process.
 *
 * \version $Id$
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include <kern/proc.h>
#include <drv/timer.h>

#warning FIXME: Review this test and refactor for all target.

/**
 * Proc scheduling test subthread 1
 */
static void proc_test_thread1(void)
{
	for (int i = 0; i < 30; ++i)
	{
		kputs(">task 1\n");
		timer_delay(50);
		proc_switch();
	}
}

/**
 * Proc scheduling test subthread 2
 */
static void proc_test_thread2(void)
{
	for (int i = 0; i < 30; ++i)
	{
		kputs(">task 2\n");
		timer_delay(75);
		proc_switch();
	}
}

static cpustack_t proc_test_stack1[CONFIG_PROC_DEFSTACKSIZE / sizeof(cpustack_t)];
static cpustack_t proc_test_stack2[CONFIG_PROC_DEFSTACKSIZE / sizeof(cpustack_t)];

/**
 * Process scheduling test
 */
void proc_test(void)
{
	proc_new(proc_test_thread1, NULL, sizeof(proc_test_stack1), proc_test_stack1);
	proc_new(proc_test_thread2, NULL, sizeof(proc_test_stack2), proc_test_stack2);
	kputs("Created tasks\n");

	kputs("stack1:\n");
//	#warning FIXME
	kdump(proc_test_stack1+sizeof(proc_test_stack1)-64, 64);
	kputs("stack2:\n");
//	#warning FIXME
	kdump(proc_test_stack2+sizeof(proc_test_stack1)-64, 64);

	for (int i = 0; i < 30; ++i)
	{
		kputs(">main task\n");
		timer_delay(93);
		proc_switch();
	}
}
