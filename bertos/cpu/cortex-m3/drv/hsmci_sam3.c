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
 * -->
 *
 * \brief HSMCI driver implementation.
 *
 * \author Daniele Basile <asterix@develer.com>
 */


#include "hsmci_sam3.h"

#include <drv/timer.h>
#include <cpu/irq.h>
#include <drv/irq_cm3.h>

#include <io/cm3.h>



#define HSMCI_INIT_SPEED  400000
#define HSMCI_CLK_DIV     ((CPU_FREQ / (HSMCI_INIT_SPEED << 1)) - 1)

#define HSMCI_ERROR_MASK   (BV(HSMCI_SR_RINDE)    | \
							BV(HSMCI_SR_RDIRE)    | \
							BV(HSMCI_SR_RCRCE)    | \
							BV(HSMCI_SR_RENDE)    | \
							BV(HSMCI_SR_RTOE)     | \
							BV(HSMCI_SR_DCRCE)    | \
							BV(HSMCI_SR_DTOE)     | \
							BV(HSMCI_SR_CSTOE)    | \
							BV(HSMCI_SR_BLKOVRE)  | \
							BV(HSMCI_SR_ACKRCVE))


#define HSMCI_RESP_ERROR_MASK   (BV(HSMCI_SR_RINDE) | BV(HSMCI_SR_RDIRE) \
	| BV(HSMCI_SR_RCRCE)| BV(HSMCI_SR_RENDE)| BV(HSMCI_SR_RTOE))


#define HSMCI_WAIT()\
	do { \
		cpu_relax(); \
	} while (!(HSMCI_SR & (BV(HSMCI_SR_CMDRDY) | BV(HSMCI_SR_NOTBUSY) | BV(HSMCI_SR_XFRDONE))))

#define HSMCI_ERROR()   (HSMCI_SR & HSMCI_ERROR_MASK)

#define HSMCI_HW_INIT()  \
do { \
	PIOA_PDR = BV(19) | BV(20) | BV(21) | BV(22) | BV(23) | BV(24); \
	PIO_PERIPH_SEL(PIOA_BASE, BV(19) | BV(20) | BV(21) | BV(22) | BV(23) | BV(24), PIO_PERIPH_A); \
} while (0)


#define STROBE_ON()   PIOB_SODR = BV(13)
#define STROBE_OFF()  PIOB_CODR = BV(13)
#define STROBE_INIT() \
	do { \
		PIOB_OER = BV(13); \
		PIOB_PER = BV(13); \
	} while(0)

static DECLARE_ISR(hsmci_irq)
{
	kprintf("irq stato %lx\n", HSMCI_SR);
}

void hsmci_readResp(void *resp, size_t len)
{
	ASSERT(resp);
	uint32_t *r = (uint32_t *)resp;

	kprintf("size %d %d\n",(len / sizeof(HSMCI_RSPR)),  sizeof(HSMCI_RSPR));
	for (size_t i = 0; i < (len / sizeof(HSMCI_RSPR)); i++)
		r[i] = HSMCI_RSPR;
}

bool hsmci_sendCmd(uint8_t index, uint32_t argument, uint32_t reply_type)
{
	STROBE_ON();
	HSMCI_WAIT();

	HSMCI_ARGR = argument;
	HSMCI_CMDR = index | reply_type | BV(HSMCI_CMDR_OPDCMD);

	uint32_t status;
	do {
		status = HSMCI_SR;
		if (status & HSMCI_RESP_ERROR_MASK)
			return status;

		cpu_relax();
	} while (!(status & (BV(HSMCI_SR_CMDRDY) | BV(HSMCI_SR_NOTBUSY) | BV(HSMCI_SR_XFRDONE))));

	timer_delay(1);
	STROBE_OFF();
	return 0;
}

void hsmci_init(Hsmci *hsmci)
{
	(void)hsmci;

	HSMCI_HW_INIT();
	STROBE_INIT();

	pmc_periphEnable(HSMCI_ID);
	HSMCI_CR = BV(HSMCI_CR_SWRST);
	HSMCI_CR = BV(HSMCI_CR_PWSDIS) | BV(HSMCI_CR_MCIDIS);
	HSMCI_IDR = 0xFFFFFFFF;

	HSMCI_DTOR = 0xFF | HSMCI_DTOR_DTOMUL_1048576;
	HSMCI_CSTOR = 0xFF | HSMCI_CSTOR_CSTOMUL_1048576;
	HSMCI_MR = HSMCI_CLK_DIV | ((0x7u << HSMCI_MR_PWSDIV_SHIFT) & HSMCI_MR_PWSDIV_MASK);
	HSMCI_SDCR = 0;
	HSMCI_CFG = BV(HSMCI_CFG_FIFOMODE) | BV(HSMCI_CFG_FERRCTRL);

	sysirq_setHandler(INT_HSMCI, hsmci_irq);
	HSMCI_CR = BV(HSMCI_CR_MCIEN);

}
