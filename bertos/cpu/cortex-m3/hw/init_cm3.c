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
 * \brief Cortex-M3 architecture's entry point
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include <cfg/compiler.h>
#include <cfg/cfg_proc.h> /* CONFIG_KERN_PREEMPT */
#include <kern/proc_p.h>
#include <cfg/debug.h>
#include <cpu/attr.h> /* PAUSE */
#include <cpu/irq.h> /* IRQ_DISABLE */
#include <cpu/types.h>
#include <drv/irq_cm3.h>
#include "switch_ctx_cm3.h"

#if CPU_CM3_LM3S
#include <drv/clock_lm3s.h>
#include <io/lm3s.h>
#elif CPU_CM3_STM32
#include <drv/clock_stm32.h>
#include <io/stm32.h>
#endif

extern size_t __text_end, __data_start, __data_end, __bss_start, __bss_end;

extern void __init2(void);

/* Architecture's entry point */
void __init2(void)
{
	/*
	 * The main application expects IRQs disabled.
	 */
	IRQ_DISABLE;

#if CPU_CM3_LM3S
	/*
	 * PLL may not function properly at default LDO setting.
	 *
	 * Description:
	 *
	 * In designs that enable and use the PLL module, unstable device
	 * behavior may occur with the LDO set at its default of 2.5 volts or
	 * below (minimum of 2.25 volts). Designs that do not use the PLL
	 * module are not affected.
	 *
	 * Workaround: Prior to enabling the PLL module, it is recommended that
	 * the default LDO voltage setting of 2.5 V be adjusted to 2.75 V using
	 * the LDO Power Control (LDOPCTL) register.
	 *
	 * Silicon Revision Affected: A1, A2
	 *
	 * See also: Stellaris LM3S1968 A2 Errata documentation.
	 */
	if (REVISION_IS_A1 | REVISION_IS_A2)
		HWREG(SYSCTL_LDOPCTL) = SYSCTL_LDOPCTL_2_75V;
#endif
	/* Set the appropriate clocking configuration */
	clock_init();

	/* Initialize IRQ vector table in RAM */
	sysirq_init();

#if (CONFIG_KERN && CONFIG_KERN_PREEMPT)
	/*
	 * Voluntary context switch handler.
	 *
	 * This software interrupt can always be triggered and must be
	 * dispatched as soon as possible, thus we just disable IRQ priority
	 * for it.
	 */
	sysirq_setHandler(FAULT_SVCALL, svcall_handler);
	sysirq_setPriority(FAULT_SVCALL, IRQ_PRIO_MAX);
	/*
	 * Preemptible context switch handler
	 *
	 * The priority of this IRQ must be the lowest priority in the system
	 * in order to run last in the interrupt service routines' chain.
	 */
	sysirq_setHandler(FAULT_PENDSV, pendsv_handler);
	sysirq_setPriority(FAULT_PENDSV, IRQ_PRIO_MIN);
#endif
}
