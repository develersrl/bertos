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
 * \brief Semaphore test.
 *
 * \version $Id$
 * 
 * \author Daniele Basile <asterix@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com> 
 * 
 */

#include <cfg/test.h>

#include <kern/sem.h>
#include <kern/proc.h>
#include <kern/irq.h>

#include <drv/timer.h>

// Global settings for the test.
#define MAX_GLOBAL_COUNT               10
#define TEST_TIME_OUT_MS             1000
#define DELAY                          10
#define INC_PROC_T1                     1
#define DELAY_PROC_T1   INC_PROC_T1*DELAY
#define INC_PROC_T2                     3
#define DELAY_PROC_T2   INC_PROC_T2*DELAY

Semaphore sem;
unsigned int global_count = 0;

/*
 * Proc scheduling test subthread 1
 */
static void proc_test1(void)
{
	unsigned int local_count = 0;
	
	for (int i = 0; i < INC_PROC_T1; ++i)
	{
		kputs("> test1\n");
		sem_obtain(&sem);
		kputs("> test1: Obtain semaphore.\n");
		local_count = global_count;
		kprintf("> test1: Read global count [%d]\n", local_count);
		timer_delay(DELAY_PROC_T1);
		local_count += INC_PROC_T1;
		global_count = local_count;
		kprintf("> test1: Update count g[%d] l[%d]\n", global_count, local_count);
		sem_release(&sem);
		kputs("> test1: Relase semaphore.\n");
	}
}

/*
 * Proc scheduling test subthread 2
 */
static void proc_test2(void)
{
	unsigned int local_count = 0;
	
	for (int i = 0; i < INC_PROC_T2; ++i)
	{
		kputs("> test2\n");
		sem_obtain(&sem);
		kputs("> test2: Obtain semaphore.\n");
		local_count = global_count;
		kprintf("> test2: Read global count [%d]\n", local_count);
		timer_delay(DELAY_PROC_T2);
		local_count += INC_PROC_T2;
		global_count = local_count;
		kprintf("> test2: Update count g[%d] l[%d]\n", global_count, local_count);
		sem_release(&sem);
		kputs("> test2: Relase semaphore.\n");
	}
}

// Define process stacks for test.
static cpu_stack_t proc_test1_stack[CONFIG_KERN_MINSTACKSIZE / sizeof(cpu_stack_t)];
static cpu_stack_t proc_test2_stack[CONFIG_KERN_MINSTACKSIZE / sizeof(cpu_stack_t)];


/**
 * Run semaphore test
 */
int sem_testRun(void)
{
	ticks_t start_time = timer_clock();

	kprintf("Run semaphore test..\n");
	
	proc_new(proc_test1, NULL, sizeof(proc_test1_stack), proc_test1_stack);
	proc_new(proc_test2, NULL, sizeof(proc_test2_stack), proc_test2_stack);
	kputs("> Main: Processes created\n");
	
	/*
	 * Wait until all process finishing, if some going wrong we return 
	 * error after time_out_ms ms.
	 */ 
	while((timer_clock() - start_time) < ms_to_ticks(TEST_TIME_OUT_MS))
	{
		if (sem_attempt(&sem))
		{
			kputs("> Main: Check if test is finish..\n");
			if(global_count == MAX_GLOBAL_COUNT)
			{
				kputs("> Main: Test Finished..Ok!\n");
				return 0;
			}
			sem_release(&sem);
			kputs("> Main: Test is still running..\n");
		}
		proc_yield();
	}
	
	kputs("Semaphore Test fail..\n");
	return -1;
}

int sem_testSetup(void)
{
	kdbg_init();

	kprintf("Init Semaphore..");
	sem_init(&sem);
	kprintf("Done.\n");
	
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

int sem_testTearDown(void)
{
	kputs("TearDown Semaphore test.\n");
	return 0;
}

TEST_MAIN(sem);