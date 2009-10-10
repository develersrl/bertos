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
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief AFSK modem hardware-specific definitions.
 *
 * \version $Id$
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#ifndef HW_AFSK_H
#define HW_AFSK_H

#include "cfg/cfg_arch.h"

#if !(ARCH & ARCH_UNITTEST)
	#warning TODO:This is an example implementation, you must implement it!

	#define AFSK_ADC_INIT(ch, ctx) do { (void)ch, (void)ctx; } while (0)

	#define AFSK_STROBE_INIT() do { /* Implement me */ } while (0)
	#define AFSK_STROBE_ON()   do { /* Implement me */ } while (0)
	#define AFSK_STROBE_OFF()  do { /* Implement me */ } while (0)

	#define AFSK_DAC_INIT(ch, ctx)   do { (void)ch, (void)ctx; } while (0)
	#define AFSK_DAC_IRQ_START(ch)   do { (void)ch; /* Implement me */ } while (0)
	#define AFSK_DAC_IRQ_STOP(ch)    do { (void)ch; /* Implement me */ } while (0)
	#define AFSK_DAC_SET(ch, val)    do { (void)ch; (void)val; } while (0)
#else /* (ARCH & ARCH_UNITTEST) */

	#include <stdio.h>
	#include <cfg/compiler.h>

	/* For test */
	extern uint32_t data_written;
	extern FILE *fp_dac;
	extern bool afsk_tx_test;

	#define AFSK_ADC_INIT(ch, ctx)    do { (void)ch, (void)ctx; } while (0)

	#define AFSK_STROBE_INIT()  /* Implement me */
	#define AFSK_STROBE_ON()    /* Implement me */
	#define AFSK_STROBE_OFF()   /* Implement me */

	#define AFSK_DAC_INIT(ch, ctx)   do { (void)ch, (void)ctx; } while (0)
	#define AFSK_DAC_IRQ_START(ch)   do { (void)ch; afsk_tx_test = true; } while (0)
	#define AFSK_DAC_IRQ_STOP(ch)    do { (void)ch; afsk_tx_test = false; } while (0)
	#define AFSK_DAC_SET(ch, _val)     \
	do { \
		(void)ch; \
		int8_t val = (_val) - 128; \
		ASSERT(fwrite(&val, 1, sizeof(val), fp_dac) == sizeof(val)); \
		data_written++; \
	} while (0)

#endif /* !(ARCH & ARCH_UNITTEST) */

#endif /* HW_AFSK_H */
