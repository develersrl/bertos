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
 * \brief Cortex-M3 IRQ management.
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include <cfg/debug.h> /* ASSERT() */
#include <cfg/log.h> /* LOG_ERR() */
#include <cpu/irq.h>
#include "io/lm3s.h"
#include "irq_lm3s.h"

static void (*irq_table[NUM_INTERRUPTS])(void)
			__attribute__((section("vtable")));

/* Unhandled IRQ */
static NORETURN NAKED void unhandled_isr(void)
{
	reg32_t reg;

	asm volatile ("mrs %0, ipsr" : "=r"(reg));
	LOG_ERR("unhandled IRQ %lu\n", reg);
	PAUSE;
	UNREACHABLE();
	ASSERT(0);
}

void sysirq_setHandler(sysirq_t irq, sysirq_handler_t handler)
{
	cpu_flags_t flags;

	ASSERT(irq < NUM_INTERRUPTS);

	IRQ_SAVE_DISABLE(flags);
	irq_table[irq] = handler;
	IRQ_RESTORE(flags);
}

void sysirq_freeHandler(sysirq_t irq)
{
	cpu_flags_t flags;

	ASSERT(irq < NUM_INTERRUPTS);

	IRQ_SAVE_DISABLE(flags);
	irq_table[irq] = unhandled_isr;
	IRQ_RESTORE(flags);
}

void sysirq_init(void)
{
	cpu_flags_t flags;
	int i;

	IRQ_SAVE_DISABLE(flags);
	for (i = 0; i < NUM_INTERRUPTS; i++)
		irq_table[i] = unhandled_isr;

	/* Update NVIC to point to the new vector table */
	HWREG(NVIC_VTABLE) = (size_t)irq_table;
	IRQ_RESTORE(flags);
}
