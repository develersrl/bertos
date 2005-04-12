/*!
 * \file
 * <!--
 * Copyright 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Simple serial I/O driver
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2005/04/12 01:37:50  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.7  2005/01/23 12:24:27  bernie
 *#* Include macros.h for BV().
 *#*
 *#* Revision 1.6  2004/10/20 13:40:54  batt
 *#* Put {} instead of ; after while loop.
 *#*
 *#* Revision 1.5  2004/10/20 13:39:40  batt
 *#* Reformat.
 *#*
 *#* Revision 1.4  2004/10/20 13:30:02  batt
 *#* Optimization of UCSR0C writing
 *#*
 *#* Revision 1.3  2004/10/14 15:55:32  batt
 *#* Add ser_purge.
 *#*
 *#* Revision 1.2  2004/10/14 14:46:59  batt
 *#* Change baudrate calculation.
 *#*
 *#* Revision 1.1  2004/10/13 16:35:36  batt
 *#* New (simple) serial driver.
 *#*/
#include "ser_simple.h"

#include <compiler.h>
#include <config.h>
#include <macros.h> /* BV() */
#include <hw.h>

#include <avr/io.h>

/*!
 * Send a character over the serial line.
 *
 * \return the character sent.
 */
int _ser_putchar(int c)
{
	/* Disable Rx to avoid echo*/
	UCSR0B &= ~BV(RXEN);
	/* Enable tx*/
	UCSR0B |= BV(TXEN);
	/* Prepare transmission */
	UDR0 = c;
	/* Wait until byte sent */
	while (!(UCSR0A & BV(TXC))) {}
	/* Disable tx to avoid short circuit when tx and rx share the same wire. */
	UCSR0B &= ~BV(TXEN);
	/* Enable Rx */
	UCSR0B |= BV(RXEN);
	/* Delete TRANSMIT_COMPLETE_BIT flag */
	UCSR0A |= BV(TXC);
	return c;
}


/*!
 * Get a character from the serial line.
 * If ther is no character in the buffer this function wait until
 * one is received (no timeout).
 *
 * \return the character received.
 */
int _ser_getchar(void)
{
	/* Wait for data */
	while (!(UCSR0A & BV(RXC))) {}
	return UDR0;

}


/*!
 * Get a character from the receiver buffer
 * If the buffer is empty, ser_getchar_nowait() returns
 * immediatly EOF.
 */
int _ser_getchar_nowait(void)
{
	if (!(UCSR0A & BV(RXC))) return EOF;
	else return UDR0;
}

void _ser_settimeouts(void)
{
}

/*!
 * Set the baudrate.
 */
void _ser_setbaudrate(unsigned long rate)
{
	/* Compute baud-rate period */
	uint16_t period = (((CLOCK_FREQ / 16UL) + (rate / 2)) / rate) - 1;

	UBRR0H = (period) >> 8;
	UBRR0L = (period);
}

/*!
 * Send a string.
 */
int _ser_print(const char *s)
{
	while(*s) _ser_putchar(*s++);
	return 0;
}


void _ser_setparity(int parity)
{
	/* Set the new parity */
	UCSR0C |= (UCSR0C & ~(BV(UPM1) | BV(UPM0))) | (parity << UPM0);	
}

/*!
 * Dummy functions.
 */
void _ser_purge(void)
{
	while (_ser_getchar_nowait() != EOF) {}
}

/*!
 * Initialize serial.
 */
struct Serial * _ser_open(void)
{
	/*
	 * Set Rx and Tx pins as input to avoid short
	 * circuit when serial is disabled.
	 */
	DDRE  &= ~(BV(PE0)|BV(PE1));
	PORTE &= ~BV(PE0);
	PORTE |=  BV(PE1);
	/* Enable only Rx section */
	UCSR0B = BV(RXEN);
	return NULL;
}


/*!
 * Clean up serial port, disabling the associated hardware.
 */
void _ser_close(void)
{
	/* Disable Rx & Tx. */
	UCSR0B &= ~(BV(RXEN) | BV(TXEN));
}
