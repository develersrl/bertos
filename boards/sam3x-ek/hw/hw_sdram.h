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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief MT48LC16M16A2 SDRAM initialization for Atmel SAM3X-EK board.
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * SDRAMC register settings and comments are from Atmel Softpack, see licence below:
 *
 * ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2010, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#ifndef HW_SAM3X_SDRAM_H
#define HW_SAM3X_SDRAM_H

#include "cfg/macros.h"
#include <cpu/types.h>
#include <io/sam3.h>
#include <drv/timer.h>

/*
 * SDRAM properties
 */
#define SDRAM_BASE      0x70000000
#define SDRAM_SIZE      (32*1024*1024)
#define SDRAM_BUSWIDTH  16
#define SDRAM_CLK       CPU_FREQ

/*
 * IO pins
 */
// port C, peripheral A
#define PIO_SDRAM_DATA     (0xFFFF << 2)  // Data bus, 2-17
#define PIO_SDRAM_SDA0_A7  (0xFF << 23)   // Address bus, 23-30
#define PIO_SDRAM_NBS0     BV(21)         // Data mask enable 0
// port D, peripheral A
#define PIO_SDRAM_SDCKE    BV(13)         // Clock enable
#define PIO_SDRAM_SDCS     BV(12)         // Chip select
#define PIO_SDRAM_RAS      BV(15)         // Row
#define PIO_SDRAM_CAS      BV(16)         // Column
#define PIO_SDRAM_BA0      BV(6)          // Bank select 0
#define PIO_SDRAM_BA1      BV(7)          // Bank select 1
#define PIO_SDRAM_SDWE     BV(14)         // Write enable
#define PIO_SDRAM_NBS1     BV(10)         // Data mask enable 1
#define PIO_SDRAM_SDA8     BV(22)
#define PIO_SDRAM_SDA9     BV(23)
#define PIO_SDRAM_SDA10    BV(11)
#define PIO_SDRAM_SDA11    BV(25)
#define PIO_SDRAM_SDA12    BV(4)
// port D, PIO output
#define PIO_SDRAM_EN       BV(18)         // Enable

#define SDRAM_PORTC_PERIPH  (PIO_SDRAM_DATA | PIO_SDRAM_SDA0_A7 | PIO_SDRAM_NBS0)
#define SDRAM_PORTD_PERIPH  (PIO_SDRAM_SDCKE | PIO_SDRAM_SDCS  | PIO_SDRAM_RAS   | \
                             PIO_SDRAM_CAS   | PIO_SDRAM_BA0   | PIO_SDRAM_BA1   | \
                             PIO_SDRAM_SDWE  | PIO_SDRAM_NBS1  | PIO_SDRAM_SDA8  | \
                             PIO_SDRAM_SDA9  | PIO_SDRAM_SDA10 | PIO_SDRAM_SDA11 | \
                             PIO_SDRAM_SDA12)
#define SDRAM_PORTD_OUTPUT  PIO_SDRAM_EN


INLINE void sdram_init(void)
{
	HWREG(PIOC_BASE + PIO_PDR_OFF) = SDRAM_PORTC_PERIPH;
	HWREG(PIOC_BASE + PIO_PUER_OFF) = SDRAM_PORTC_PERIPH;
	PIO_PERIPH_SEL(PIOC_BASE, SDRAM_PORTC_PERIPH, PIO_PERIPH_A);

	HWREG(PIOD_BASE + PIO_PDR_OFF) = SDRAM_PORTD_PERIPH;
	HWREG(PIOD_BASE + PIO_PUER_OFF) = SDRAM_PORTD_PERIPH;
	PIO_PERIPH_SEL(PIOD_BASE, SDRAM_PORTD_PERIPH, PIO_PERIPH_A);

	HWREG(PIOD_BASE + PIO_PER_OFF) = SDRAM_PORTD_OUTPUT;
	HWREG(PIOD_BASE + PIO_OER_OFF) = SDRAM_PORTD_OUTPUT;
	HWREG(PIOD_BASE + PIO_SODR_OFF) = SDRAM_PORTD_OUTPUT;

	pmc_periphEnable(SMC_SDRAMC_ID);

	// SDRAM device configuration
	SDRAMC_CR =
		SDRAMC_CR_NC_COL9 | SDRAMC_CR_NR_ROW13 | SDRAMC_CR_NB_BANK4 |
		SDRAMC_CR_CAS_LATENCY2 | SDRAMC_CR_DBW |
		SDRAMC_CR_TWR(2) | SDRAMC_CR_TRC_TRFC(9) | SDRAMC_CR_TRP(3) |
		SDRAMC_CR_TRCD(3) | SDRAMC_CR_TRAS(6) | SDRAMC_CR_TXSR(10);

	SDRAMC_LPR = 0;
	SDRAMC_MDR = SDRAMC_MDR_MD_SDRAM;

	/*
	 * A minimum pause of 200 us needed before any signal toggle
	 * (6 core cycles per iteration).
	 */
	timer_delay(1);

	/*
	 * A NOP command is issued to the SDR-SDRAM. Program NOP command into
	 * Mode Register, the application must set Mode to 1 in the Mode Register.
	 * Perform a write access to any SDR-SDRAM address to acknowledge this command.
	 * Now the clock which drives SDR-SDRAM device is enabled.
	 */
	SDRAMC_MR = SDRAMC_MR_MODE_NOP;
	*(uint32_t *)SDRAM_BASE = 0;

	/*
	 * An all banks precharge command is issued to the SDR-SDRAM. Program all
	 * banks precharge command into Mode Register, the application must set Mode to
	 * 2 in the Mode Register . Perform a write access to any SDRSDRAM address to
	 * acknowledge this command.
	 */
	SDRAMC_MR = SDRAMC_MR_MODE_ALLBANKS_PRECHARGE;
	*(uint32_t *)SDRAM_BASE = 0;

	/*
	 * Eight auto-refresh (CBR) cycles are provided. Program the auto refresh
	 * command (CBR) into Mode Register, the application must set Mode to 4 in
	 * the Mode Register. Once in the idle state, two AUTO REFRESH cycles must
	 * be performed.
	 */
	SDRAMC_MR = SDRAMC_MR_MODE_AUTO_REFRESH;
	*(uint32_t *)SDRAM_BASE = 0;

	SDRAMC_MR = SDRAMC_MR_MODE_AUTO_REFRESH;
	*(uint32_t *)SDRAM_BASE = 0;

	/*
	 * A Mode Register set (MRS) cycle is issued to program the parameters of
	 * the SDRAM devices, in particular CAS latency and burst length.
	 */
	SDRAMC_MR = SDRAMC_MR_MODE_LOAD_MODEREG;
	*(uint32_t *)(SDRAM_BASE + 9) = 0xc001babe;

	/*
	 * For low-power SDR-SDRAM initialization, an Extended Mode Register set
	 * (EMRS) cycle is issued to program the SDR-SDRAM parameters (TCSR, PASR, DS).
	 * The write address must be chosen so that BA[1] is set to 1 and BA[0] is set
	 * to 0: BK1 is at bit 24, 1+9+13+1.
	 */
	SDRAMC_MR = SDRAMC_MR_MODE_EXT_LOAD_MODEREG;
	*(uint32_t *)(SDRAM_BASE + (1 << 24)) = 0;

	/*
	 * The application must go into Normal Mode, setting Mode to 0 in the Mode
	 * Register and perform a write access at any location in the SDRAM to
	 * acknowledge this command.
	 */
	SDRAMC_MR = SDRAMC_MR_MODE_NORMAL;
	*(uint32_t *)SDRAM_BASE = 0;

	/*
	 * Write the refresh rate into the count field in the SDRAMC Refresh
	 * Timer register. Set Refresh timer 15.625 us
	 */
	SDRAMC_TR = SDRAMC_TR_COUNT(SDRAM_CLK / 1000 * 15625 / 1000000) ;
}

#endif /* HW_SAM3X_SDRAM_H */
