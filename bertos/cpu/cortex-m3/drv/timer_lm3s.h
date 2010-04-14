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
 * \brief Low-level timer driver (SysTick) for LM3S1968.
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#ifndef DRV_CORTEX_M3_TIMER_H
#define DRV_CORTEX_M3_TIMER_H

#include <io/lm3s.h>

/* Ticks frequency (HZ) */
#define TIMER_TICKS_PER_SEC	1000

/* Frequency of the hardware high-precision timer. */
#define TIMER_HW_HPTICKS_PER_SEC (CPU_FREQ)

/* Maximum value of the high-precision hardware counter register */
#define TIMER_HW_CNT (CPU_FREQ / TIMER_TICKS_PER_SEC)

/** Type of time expressed in ticks of the hardware high-precision timer */
typedef uint32_t hptime_t;
#define SIZEOF_HPTIME_T 4

/* Timer ISR prototype */
ISR_PROTO_CONTEXT_SWITCH(timer_handler);
#define DEFINE_TIMER_ISR DECLARE_ISR_CONTEXT_SWITCH(timer_handler)

INLINE void timer_hw_irq(void)
{
}

INLINE bool timer_hw_triggered(void)
{
	return true;
}

INLINE hptime_t timer_hw_hpread(void)
{
	return HWREG(NVIC_ST_CURRENT);
}

void timer_hw_init(void);
void timer_hw_exit(void);

#endif /* DRV_CORTEX_M3_TIMER_H */
