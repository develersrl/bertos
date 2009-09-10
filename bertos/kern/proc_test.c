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
 *
 * $test$: cp bertos/cfg/cfg_proc.h $cfgdir/
 * $test$: echo  "#undef CONFIG_KERN" >> $cfgdir/cfg_proc.h
 * $test$: echo "#define CONFIG_KERN 1" >> $cfgdir/cfg_proc.h
 * $test$: echo  "#undef CONFIG_KERN_PRI" >> $cfgdir/cfg_proc.h
 * $test$: echo "#define CONFIG_KERN_PRI 1" >> $cfgdir/cfg_proc.h
 * $test$: cp bertos/cfg/cfg_monitor.h $cfgdir/
 * $test$: echo  "#undef CONFIG_KERN_MONITOR" >> $cfgdir/cfg_monitor.h
 * $test$: echo "#define CONFIG_KERN_MONITOR 1" >> $cfgdir/cfg_monitor.h
 * $test$: cp bertos/cfg/cfg_signal.h $cfgdir/
 * $test$: echo  "#undef CONFIG_KERN_SIGNALS" >> $cfgdir/cfg_signal.h
 * $test$: echo "#define CONFIG_KERN_SIGNALS 1" >> $cfgdir/cfg_signal.h
 */

#include <kern/proc.h>
#include <kern/irq.h>
#include <kern/monitor.h>

#include <drv/timer.h>
#include <cfg/test.h>


// Global settings for the test.
#define DELAY                           5

// Settings for the test process.
//Process 1
#define INC_PROC_T1                     1
#define DELAY_PROC_T1   INC_PROC_T1*DELAY
//Process 2
#define INC_PROC_T2                     3
#define DELAY_PROC_T2   INC_PROC_T2*DELAY
//Process 3
#define INC_PROC_T3                     5
#define DELAY_PROC_T3   INC_PROC_T3*DELAY
//Process 4
#define INC_PROC_T4                     7
#define DELAY_PROC_T4   INC_PROC_T4*DELAY
//Process 5
#define INC_PROC_T5                    11
#define DELAY_PROC_T5   INC_PROC_T5*DELAY
//Process 6
#define INC_PROC_T6                    13
#define DELAY_PROC_T6   INC_PROC_T6*DELAY
//Process 7
#define INC_PROC_T7                    17
#define DELAY_PROC_T7   INC_PROC_T7*DELAY
//Process 8
#define INC_PROC_T8                    19
#define DELAY_PROC_T8   INC_PROC_T8*DELAY

//Global count for each process.
unsigned int t1_count = 0;
unsigned int t2_count = 0;
unsigned int t3_count = 0;
unsigned int t4_count = 0;
unsigned int t5_count = 0;
unsigned int t6_count = 0;
unsigned int t7_count = 0;
unsigned int t8_count = 0;

/*
 * These macros generate the code needed to create the test process functions.
 */
#define PROC_TEST(num) static void proc_test##num(void) \
{ \
	for (int i = 0; i < INC_PROC_T##num; ++i) \
	{ \
		t##num##_count++; \
		kprintf("> Process[%d]: count[%d]\n", num, t##num##_count); \
		timer_delay(DELAY_PROC_T##num); \
	} \
} \

#define PROC_TEST_STACK(num)  static cpu_stack_t proc_test##num##_stack[700 / sizeof(cpu_stack_t)];
#define PROC_TEST_INIT(num)   proc_new(proc_test##num, NULL, sizeof(proc_test##num##_stack), proc_test##num##_stack);

// Define process
PROC_TEST(1)
PROC_TEST(2)
PROC_TEST(3)
PROC_TEST(4)
PROC_TEST(5)
PROC_TEST(6)
PROC_TEST(7)
PROC_TEST(8)

// Define process stacks for test.
PROC_TEST_STACK(1)
PROC_TEST_STACK(2)
PROC_TEST_STACK(3)
PROC_TEST_STACK(4)
PROC_TEST_STACK(5)
PROC_TEST_STACK(6)
PROC_TEST_STACK(7)
PROC_TEST_STACK(8)

// Define params to test priority
#define PROC_PRI_TEST(num) static void proc_pri_test##num(void) \
{ \
	struct Process *main_proc = (struct Process *) proc_currentUserData(); \
	sig_signal(main_proc, SIG_USER##num); \
} \

// Default priority is 0
#define PROC_PRI_TEST_INIT(num, proc)  \
do { \
	struct Process *p = proc_new(proc_pri_test##num, (proc), sizeof(proc_test##num##_stack), proc_test##num##_stack); \
	proc_setPri(p, num + 1); \
} while (0)

PROC_TEST_STACK(0)
PROC_PRI_TEST(0)
PROC_PRI_TEST(1)
PROC_PRI_TEST(2)


/**
 * Process scheduling test
 */
int proc_testRun(void)
{
	int ret_value = 0;
	kprintf("Run Process test..\n");

	//Init the process tests
	PROC_TEST_INIT(1)
	PROC_TEST_INIT(2)
	PROC_TEST_INIT(3)
	PROC_TEST_INIT(4)
	PROC_TEST_INIT(5)
	PROC_TEST_INIT(6)
	PROC_TEST_INIT(7)
	PROC_TEST_INIT(8)
	kputs("> Main: Processes created\n");

	for (int i = 0; i < 30; ++i)
	{
		kputs("> Main\n");
		timer_delay(93);
		monitor_report();
	}

	if( t1_count == INC_PROC_T1 &&
		t2_count == INC_PROC_T2 &&
		t3_count == INC_PROC_T3 &&
		t4_count == INC_PROC_T4 &&
		t5_count == INC_PROC_T5 &&
		t6_count == INC_PROC_T6 &&
		t7_count == INC_PROC_T7 &&
		t8_count == INC_PROC_T8)
	{
		kputs("> Main: process test finished..ok!\n");
		ret_value = 0;
	}
	else
	{
		kputs("> Main: process test..fail!\n");
		ret_value = -1;
	}

#if CONFIG_KERN_SIGNALS
	// test process priority
	// main process must have the higher priority to check signals received
	proc_setPri(proc_current(), 10);

	struct Process *curr = proc_current();
	// the order in which the processes are created is important!
	PROC_PRI_TEST_INIT(0, curr);
	PROC_PRI_TEST_INIT(1, curr);
	PROC_PRI_TEST_INIT(2, curr);

	// signals must be: USER2, 1, 0 in order
	sigmask_t signals = sig_wait(SIG_USER0 | SIG_USER1 | SIG_USER2);
	if (!(signals & SIG_USER2))
		goto priority_fail;

	signals = sig_wait(SIG_USER0 | SIG_USER1 | SIG_USER2);
	if (!(signals & SIG_USER1))
		goto priority_fail;

	signals = sig_wait(SIG_USER0 | SIG_USER1 | SIG_USER2);
	if (!(signals & SIG_USER0))
		goto priority_fail;

	// All processes must have quit by now, but just in case...
	signals = sig_waitTimeout(SIG_USER0 | SIG_USER1 | SIG_USER2, 200);
	if (signals & (SIG_USER0 | SIG_USER1 | SIG_USER2))
		goto priority_fail;

	if (signals & SIG_TIMEOUT)
	{
		kputs("Priority test successfull.\n");
	}

priority_fail:
	kputs("Priority test failed.\n");
	ret_value = -1;

#endif

	return ret_value

}


int proc_testSetup(void)
{
	kdbg_init();

	#if CONFIG_KERN_PREEMPT
		kprintf("Init Interrupt (preempt mode)..");
		irq_init();
		kprintf("Done.\n");
	#endif

	kprintf("Init Timer..");
	timer_init();
	kprintf("Done.\n");

	kprintf("Init Process..");
	proc_init();
	kprintf("Done.\n");

	return 0;
}

int proc_testTearDown(void)
{
	kputs("TearDown Process test.\n");
	return 0;
}

TEST_MAIN(proc);
