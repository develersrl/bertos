/**
 * \file
 * Copyright (C) 2000 Bernardo Innocenti <bernie@codewiz.org>
 * Copyright (C) 2003 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief CPU specific serial I/O driver
 */

/*
 * $Log$
 * Revision 1.1  2004/05/23 18:10:11  bernie
 * Import drv/ modules.
 *
 * Revision 1.1  2003/11/20 22:30:21  aleph
 * Add serial driver
 *
 */

#include "hw.h"
#include "serhw.h"

#define SER_HW_ENABLE_TX \
	DISABLE_INTS; \
	if (!ser_sending) \
	{ \
		ser_sending = true; \
		(INT_PEND1 |= INT1F_TI) \
	} \
	ENABLE_INTS;

static volatile bool ser_sending;

// Serial TX intr
INTERRUPT(0x30) void TI_interrupt(void)
{
	if (CANT_SEND)
	{
		ser_sending = false;
		return;
	}

	/* Can we send two bytes at the same time? */
	if (SP_STAT & SPSF_TX_EMPTY)
	{
		SBUF = fifo_pop(&ser_txfifo);

		if (CANT_SEND)
		{
			ser_sending = false;
			return;
		}
	}

	SBUF = fifo_pop(&ser_txfifo);
}

INTERRUPT(0x32) void RI_interrupt(void)
{
	ser_status |= SP_STAT &
		(SPSF_OVERRUN_ERROR | SPSF_PARITY_ERROR | SPSF_FRAMING_ERROR);
	if (fifo_isfull(&ser_rxfifo))
		ser_status |= SERRF_RXFIFOOVERRUN;
	else
		fifo_push(&ser_rxfifo, SBUF);
}

static void ser_setbaudrate(unsigned long rate)
{
	// Calcola il periodo per la generazione del baud rate richiesto
	uint16_t baud = (uint16_t)(((CLOCK_FREQ / 16) / rate) - 1) | 0x8000;
	BAUD_RATE = (uint8_t)baud;
	BAUD_RATE = (uint8_t)(baud >> 8);
}

static void ser_hw_init(void)
{
	// Inizializza la porta seriale
	SP_CON = SPCF_RECEIVE_ENABLE | SPCF_MODE1;
	ioc1_img |= IOC1F_TXD_SEL | IOC1F_EXTINT_SRC;
	IOC1 = ioc1_img;

	// Svuota il buffer di ricezione
	{
		uint8_t dummy = SBUF;
	}

	// Abilita gli interrupt
	INT_MASK1 |= INT1F_TI | INT1F_RI;
}

