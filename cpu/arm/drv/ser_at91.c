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
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief ARM UART and SPI I/O driver
 *
 *
 * \version $Id: ser_amr.c 18280 2007-10-11 15:14:20Z asterix $
 * \author Daniele Basile <asterix@develer.com>
 */

#include <io/arm.h>

//#include "ser_at91.h"
#include <drv/ser.h>
#include <drv/ser_p.h>

#include <hw/hw_ser.h>  /* Required for bus macros overrides */
#include <hw/hw_cpu.h>  /* CLOCK_FREQ */

#include <mware/fifobuf.h>
#include <cfg/debug.h>

#include <appconfig.h>


/**
 * \name Overridable serial bus hooks
 *
 * These can be redefined in hw.h to implement
 * special bus policies such as half-duplex, 485, etc.
 *
 *
 * \code
 *  TXBEGIN      TXCHAR      TXEND  TXOFF
 *    |   __________|__________ |     |
 *    |   |   |   |   |   |   | |     |
 *    v   v   v   v   v   v   v v     v
 * ______  __  __  __  __  __  __  ________________
 *       \/  \/  \/  \/  \/  \/  \/
 * ______/\__/\__/\__/\__/\__/\__/
 *
 * \endcode
 *
 * \{
 */



/**
 * \def CONFIG_SER_STROBE
 *
 * This is a debug facility that can be used to
 * monitor SER interrupt activity on an external pin.
 *
 * To use strobes, redefine the macros SER_STROBE_ON,
 * SER_STROBE_OFF and SER_STROBE_INIT and set
 * CONFIG_SER_STROBE to 1.
 */
#if !defined(CONFIG_SER_STROBE) || !CONFIG_SER_STROBE
	#define SER_STROBE_ON    do {/*nop*/} while(0)
	#define SER_STROBE_OFF   do {/*nop*/} while(0)
	#define SER_STROBE_INIT  do {/*nop*/} while(0)
#endif


/* From the high-level serial driver */
extern struct Serial ser_handles[SER_CNT];

/* TX and RX buffers */
static unsigned char uart0_txbuffer[CONFIG_UART0_TXBUFSIZE];
static unsigned char uart0_rxbuffer[CONFIG_UART0_RXBUFSIZE];

/**
 * Internal hardware state structure
 *
 * The \a sending variable is true while the transmission
 * interrupt is retriggering itself.
 *
 * For the USARTs the \a sending flag is useful for taking specific
 * actions before sending a burst of data, at the start of a trasmission
 * but not before every char sent.
 *
 * For the SPI, this flag is necessary because the SPI sends and receives
 * bytes at the same time and the SPI IRQ is unique for send/receive.
 * The only way to start transmission is to write data in SPDR (this
 * is done by spi_starttx()). We do this *only* if a transfer is
 * not already started.
 */
struct ArmSerial
{
	struct SerialHardware hw;
	volatile bool sending;
};


/*
 * These are to trick GCC into *not* using absolute addressing mode
 * when accessing ser_handles, which is very expensive.
 *
 * Accessing through these pointers generates much shorter
 * (and hopefully faster) code.
 */
struct Serial *ser_uart0 = &ser_handles[SER_UART0];

/**
 * Serial 0 TX interrupt handler
 */
static void serirq_tx(void)
{
	SER_STROBE_ON;

	struct FIFOBuffer * const txfifo = &ser_uart0->txfifo;

	if (fifo_isempty(txfifo))
	{
		/* Enable Tx and Rx */
		US0_CR = BV(US_RXEN) | BV(US_TXEN);
	}
	else
	{
		char c = fifo_pop(txfifo);
		/* Send one char */
		US0_THR = c;
	}

	SER_STROBE_OFF;
}

/**
 * Serial 0 RX complete interrupt handler.
 */
static void serirq_rx(void)
{
	SER_STROBE_ON;

	/* Should be read before UDR */
	ser_uart0->status |= US0_CSR & (SERRF_RXSROVERRUN | SERRF_FRAMEERROR);

	char c = US0_RHR;
	struct FIFOBuffer * const rxfifo = &ser_uart0->rxfifo;

	if (fifo_isfull(rxfifo))
		ser_uart0->status |= SERRF_RXFIFOOVERRUN;
	else
	{
		fifo_push(rxfifo, c);
	}

	SER_STROBE_OFF;
}

/**
 * Serial IRQ dispatcher.
 */
static void serirq_dispatcher(void) __attribute__ ((naked));
static void serirq_dispatcher(void)
{
	IRQ_ENTRY();

	if (US0_IMR & BV(US_RXRDY))
		serirq_rx();

	if (US0_IMR & BV(US_TXRDY))
		serirq_tx();

	IRQ_EXIT();
}

/*
 * Callbacks
 */
static void uart0_init(
	UNUSED_ARG(struct SerialHardware *, _hw),
	UNUSED_ARG(struct Serial *, ser))
{
	/* Disable all interrupt */
	US0_IDR = 0xFFFFFFFF;

	/* Set the vector. */
	AIC_SVR(US0_ID) = serirq_dispatcher;
	/* Initialize to edge triggered with defined priority. */
	AIC_SMR(US0_ID) = AIC_SRCTYPE_INT_EDGE_TRIGGERED;
	/* Enable the USART IRQ */
	AIC_IECR = BV(US0_ID);

    /* Enable UART clock. */
	PMC_PCER = BV(US0_ID);

    /* Disable GPIO on UART tx/rx pins. */
	PIOA_PDR = BV(5) | BV(6);

	/* Set serial param: mode Normal, 8bit data, 1bit stop */
	US0_MR = US_CHMODE_NORMAL | US_CHRL_8 | US_NBSTOP_1;

	/* Reset UART. */
	US0_CR = BV(US_RSTRX) | BV(US_RSTTX);

	/* Enable Tx and Rx */
	US0_CR = BV(US_RXEN) | BV(US_TXEN);

	/* Enable Tx and Rx interrupt*/
	US0_IER = BV(US_RXRDY) | BV(US_TXRDY);


}

static void uart0_cleanup(UNUSED_ARG(struct SerialHardware *, _hw))
{
	US0_CR = BV(US_RSTRX) | BV(US_RSTTX) | BV(US_RXDIS) | BV(US_TXDIS) | BV(US_RSTSTA);
}

static void uart0_enabletxirq(struct SerialHardware *_hw)
{
	struct ArmSerial *hw = (struct ArmSerial *)_hw;

	/*
	 * WARNING: racy code here!  The tx interrupt sets hw->sending to false
	 * when it runs with an empty fifo.  The order of statements in the
	 * if-block matters.
	 */
	if (!hw->sending)
	{
		hw->sending = true;
		/* Enable Tx and Rx */
		US0_CR = BV(US_RXEN) | BV(US_TXEN);
	}
}

static void uart0_setbaudrate(UNUSED_ARG(struct SerialHardware *, _hw), unsigned long rate)
{
	/* Compute baud-rate period */
	US0_BRGR = CLOCK_FREQ / (16 * rate);
	//DB(kprintf("uart0_setbaudrate(rate=%lu): period=%d\n", rate, period);)
}

static void uart0_setparity(UNUSED_ARG(struct SerialHardware *, _hw), int parity)
{
	/* Set UART parity */
	switch(parity)
	{
		case SER_PARITY_NONE:
		{
            /* Parity mode. */
			US0_MR |= US_PAR_MASK;
			break;
		}
		case SER_PARITY_EVEN:
		{
            /* Even parity.*/
			US0_MR |= US_PAR_EVEN;
			break;
		}
		case SER_PARITY_ODD:
		{
            /* Odd parity.*/
			US0_MR |= US_PAR_ODD;
			break;
		}
	}

}

static bool tx_sending(struct SerialHardware* _hw)
{
	struct ArmSerial *hw = (struct ArmSerial *)_hw;
	return hw->sending;
}

// FIXME: move into compiler.h?  Ditch?
#if COMPILER_C99
	#define	C99INIT(name,val) .name = val
#elif defined(__GNUC__)
	#define C99INIT(name,val) name: val
#else
	#warning No designated initializers, double check your code
	#define C99INIT(name,val) (val)
#endif

/*
 * High-level interface data structures
 */
static const struct SerialHardwareVT UART0_VT =
{
	C99INIT(init, uart0_init),
	C99INIT(cleanup, uart0_cleanup),
	C99INIT(setBaudrate, uart0_setbaudrate),
	C99INIT(setParity, uart0_setparity),
	C99INIT(txStart, uart0_enabletxirq),
	C99INIT(txSending, tx_sending),
};

static struct ArmSerial UARTDescs[SER_CNT] =
{
	{
		C99INIT(hw, /**/) {
			C99INIT(table, &UART0_VT),
			C99INIT(txbuffer, uart0_txbuffer),
			C99INIT(rxbuffer, uart0_rxbuffer),
			C99INIT(txbuffer_size, sizeof(uart0_txbuffer)),
			C99INIT(rxbuffer_size, sizeof(uart0_rxbuffer)),
		},
		C99INIT(sending, false),
	}
};

struct SerialHardware *ser_hw_getdesc(int unit)
{
	ASSERT(unit < SER_CNT);
	return &UARTDescs[unit].hw;
}
