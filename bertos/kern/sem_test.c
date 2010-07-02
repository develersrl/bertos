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
 *
 * \author Daniele Basile <asterix@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * $test$: cp bertos/cfg/cfg_proc.h $cfgdir/
 * $test$: echo  "#undef CONFIG_KERN" >> $cfgdir/cfg_proc.h
 * $test$: echo "#define CONFIG_KERN 1" >> $cfgdir/cfg_proc.h
 * $test$: cp bertos/cfg/cfg_sem.h $cfgdir/
 * $test$: echo  "#undef CONFIG_KERN_SEMAPHORES" >> $cfgdir/cfg_sem.h
 * $test$: echo "#define CONFIG_KERN_SEMAPHORES 1" >> $cfgdir/cfg_sem.h
 */

#include <cfg/debug.h>
#include <cfg/test.h>

#include <kern/sem.h>
#include <kern/proc.h>
#include <kern/irq.h>

#include <drv/timer.h>

// Global settings for the test.
#define MAX_GLOBAL_COUNT             1024
#define TEST_TIME_OUT_MS             6000
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

Semaphore sem;
unsigned int global_count = 0;

/*
 * These macros generate the code needed to create the test process functions.
 */
#define PROC_TEST(num) static void proc_semTest##num(void) \
{ \
	unsigned int local_count = 0; \
	\
	for (int i = 0; i < INC_PROC_T##num; ++i) \
	{ \
		sem_obtain(&sem); \
		kprintf("> test%d: Obtain semaphore.\n", num); \
		local_count = global_count; \
		kprintf("> test%d: Read global count [%d]\n", num, local_count); \
		timer_delay(DELAY_PROC_T##num); \
		local_count += INC_PROC_T##num; \
		global_count = local_count; \
		kprintf("> test%d: Update count g[%d] l[%d]\n", num, global_count, local_count); \
		sem_release(&sem); \
		kprintf("> test%d: Relase semaphore.\n", num); \
	} \
} \

#define PROC_TEST_STACK(num)  PROC_DEFINE_STACK(proc_sem_test##num##_stack, KERN_MINSTACKSIZE * 2)
#define PROC_TEST_INIT(num)   proc_new(proc_semTest##num, NULL, sizeof(proc_sem_test##num##_stack), proc_sem_test##num##_stack);

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

/**
 * Run semaphore test
 */
int sem_testRun(void)
{
	ticks_t start_time = timer_clock();

	kprintf("Run semaphore test..\n");

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

	/*
	 * Wait until all processes exit, if something goes wrong we return an
	 * error after timeout_ms.
	 */
	while((timer_clock() - start_time) < ms_to_ticks(TEST_TIME_OUT_MS))
	{
		if (sem_attempt(&sem))
		{
			kputs("> Main: Check if test has finished..\n");
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
