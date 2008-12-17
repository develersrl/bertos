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
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 *
 * \brief Test kernel process.
 *
 * \version $Id$
 * \author Daniele Basile <asterix@develer.com>
 */

#include <kern/proc.h>
#include <kern/irq.h>
#include <kern/monitor.h>

#include <drv/timer.h>
#include <cfg/test.h>

/*
 * Proc scheduling test subthread 1
 */
static void proc_test1(void)
{
	for (int i = 0; i < 30; ++i)
	{
		kputs("> test1\n");
		timer_delay(50);
		proc_yield();
	}
}

/*
 * Proc scheduling test subthread 2
 */
static void proc_test2(void)
{
	for (int i = 0; i < 30; ++i)
	{
		kputs("> test2\n");
		timer_delay(75);
	}
}

static cpu_stack_t proc_test1_stack[CONFIG_KERN_MINSTACKSIZE / sizeof(cpu_stack_t)];
static cpu_stack_t proc_test2_stack[CONFIG_KERN_MINSTACKSIZE / sizeof(cpu_stack_t)];


/**
 * Process scheduling test
 */
int proc_testRun(void)
{
	proc_new(proc_test1, NULL, sizeof(proc_test1_stack), proc_test1_stack);
	proc_new(proc_test2, NULL, sizeof(proc_test2_stack), proc_test2_stack);
	kputs("Processes created\n");

	for (int i = 0; i < 30; ++i)
	{
		kputs("> main\n");
		timer_delay(93);
		monitor_report();
	}
	return 0;
}


int proc_testSetup(void)
{
	kdbg_init();

	#if CONFIG_KERN_PREEMPT
		irq_init();
	#endif

	timer_init();

	proc_init();
	return 0;
}

int proc_testTearDown(void)
{
	return 0;
}

TEST_MAIN(proc);
