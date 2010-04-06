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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief LM3S1968 Cortex-M3 testcase
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include <cpu/irq.h>
#include <drv/timer.h>
#include "io/lm3s.h"

static Process *hp_proc, *lp_proc;

static hptime_t start, end;

static void led_init(void)
{
	/* Enable the GPIO port that is used for the on-board LED */
	SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOG;
	/*
	 * Perform a dummy read to insert a few cycles delay before enabling
	 * the peripheral.
	 */
	(void)SYSCTL_RCGC2_R;
	/* Enable the GPIO pin for the LED */
	GPIO_PORTG_DIR_R = 0x04;
	GPIO_PORTG_DEN_R = 0x04;
}

INLINE void led_on(void)
{
	GPIO_PORTG_DATA_R |= 0x04;
}

INLINE void led_off(void)
{
	GPIO_PORTG_DATA_R &= ~0x04;
}

INLINE hptime_t get_hp_ticks(void)
{
	return (TIMER_HW_CNT - timer_hw_hpread()) +
			timer_clock_unlocked() * TIMER_HW_CNT;
}

#if CONFIG_KERN_HEAP
#define hp_stack NULL
#define HP_STACK_SIZE	KERN_MINSTACKSIZE * 2
#else
static PROC_DEFINE_STACK(hp_stack, KERN_MINSTACKSIZE * 2);
#define HP_STACK_SIZE	sizeof(hp_stack)
#endif

static void NORETURN hp_process(void)
{
	char spinner[] = {'/', '-', '\\', '|'};
	int i;

	for(i = 0; ; i++)
	{
		sig_wait(SIG_USER0);
		end = get_hp_ticks();
		kprintf("%c context switch in %lu clock cycles (~%lu us)    \r",
				spinner[i % countof(spinner)],
				end - start,
				((end - start) * 1000000 / CPU_FREQ));
		led_off();
		timer_delay(50);
		sig_send(lp_proc, SIG_USER0);
	}
}

static void NORETURN lp_process(void)
{
	while (1)
	{
		led_on();
		timer_delay(50);
		start = get_hp_ticks();
		sig_send(hp_proc, SIG_USER0);
		sig_wait(SIG_USER0);
	}
}

int main(void)
{
	IRQ_ENABLE;
	kdbg_init();

	kputs("Init LED..");
	led_init();
	kputs("Done.\n");
	kputs("Init Timer..");
	timer_init();
	kputs("Done.\n");
	kputs("Init Process..");
	proc_init();
	kputs("Done.\n");

	kputs("Check scheduling functionality\n");
	proc_testRun();

	kputs("BeRTOS is up & running\n\n");
	hp_proc = proc_new(hp_process, NULL, HP_STACK_SIZE, hp_stack);
	lp_proc = proc_current();

	proc_setPri(hp_proc, 2);
	proc_setPri(lp_proc, 1);

	lp_process();
}
