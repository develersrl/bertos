/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti <bernie@codewiz.org>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief AVR UART and SPI I/O driver
 *
 * Rationale for project_ks hardware.
 *
 * The serial 0 on the board_kf board is used to communicate with the
 * smart card, which has the TX and RX lines connected together. To
 * allow the smart card to drive the RX line of the CPU the CPU TX has
 * to be in a high impedance state.
 * Whenever a transmission is done and there is nothing more to send
 * the transmitter is turn off. The output pin is held in input with
 * pull-up enabled, to avoid capturing noise from the nearby RX line.
 *
 * The line on the KBus port must keep sending data, even when
 * there is nothing to transmit, because a burst data transfer
 * generates noise on the audio channels.
 * This is accomplished using the multiprocessor mode of the
 * ATmega64/128 serial.
 *
 * The receiver keeps the MPCM bit always on. When useful data
 * is trasmitted the address bit is set. The receiver hardware
 * consider the frame as address info and receive it.
 * When useless fill bytes are sent the address bit is cleared
 * and the receiver will ignore them, avoiding useless triggering
 * of RXC interrupt.
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.15  2004/09/14 21:05:36  bernie
 *#* Use debug.h instead of kdebug.h; Use new AVR pin names; Spelling fixes.
 *#*
 *#* Revision 1.14  2004/09/06 21:50:00  bernie
 *#* Spelling fixes.
 *#*
 *#* Revision 1.13  2004/09/06 21:40:50  bernie
 *#* Move buffer handling in chip-specific driver.
 *#*
 *#* Revision 1.12  2004/08/29 22:06:10  bernie
 *#* Fix a bug in the (unused) RTS/CTS code; Clarify documentation.
 *#*
 *#* Revision 1.10  2004/08/10 06:30:41  bernie
 *#* Major redesign of serial bus policy handling.
 *#*
 *#* Revision 1.9  2004/08/02 20:20:29  aleph
 *#* Merge from project_ks
 *#*
 *#* Revision 1.8  2004/07/29 22:57:09  bernie
 *#* Several tweaks to reduce code size on ATmega8.
 *#*
 *#* Revision 1.7  2004/07/18 21:54:23  bernie
 *#* Add ATmega8 support.
 *#*
 *#* Revision 1.5  2004/06/27 15:25:40  aleph
 *#* Add missing callbacks for SPI;
 *#* Change UNUSED() macro to new version with two args;
 *#* Use TX line filling only on the correct KBUS serial port;
 *#* Fix nasty IRQ disabling bug in recv complete hander for port 1.
 *#*
 *#* Revision 1.4  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.3  2004/06/02 21:35:24  aleph
 *#* Serial enhancements: interruptible receive handler and 8 bit serial status for AVR; remove volatile attribute to FIFOBuffer, useless for new fifobuf routens
 *#*
 *#* Revision 1.2  2004/05/23 18:21:53  bernie
 *#* Trim CVS logs and cleanup header info.
 *#*
 *#*/

#include "ser.h"
#include "ser_p.h"
#include "config.h"
#include "hw.h"  /* Required for bus macros overrides */

#include <debug.h>
#include <drv/timer.h>
#include <mware/fifobuf.h>

#include <avr/signal.h>


/*!
 * \name Hardware handshake (RTS/CTS).
 * \{
 */
#ifndef RTS_ON
#define RTS_ON      do {} while (0)
#endif
#ifndef RTS_OFF
#define RTS_OFF     do {} while (0)
#endif
#ifndef IS_CTS_ON
#define IS_CTS_ON   true
#endif
#ifndef EIMSKB_CTS
#define EIMSKB_CTS  0 /*!< Dummy value, must be overridden */
#endif
/*\}*/


/*!
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
#ifndef SER_UART0_BUS_TXINIT
	/*!
	 * Default TXINIT macro - invoked in uart0_init()
	 *
	 * - Enable both the receiver and the transmitter
	 * - Enable only the RX complete interrupt
	 */
	#define SER_UART0_BUS_TXINIT do { \
		UCSR0B = BV(RXCIE) | BV(RXEN) | BV(TXEN); \
	} while (0)
#endif

#ifndef SER_UART0_BUS_TXBEGIN
	/*!
	 * Invoked before starting a transmission
	 *
	 * - Enable both the receiver and the transmitter
	 * - Enable both the RX complete and UDR empty interrupts
	 */
	#define SER_UART0_BUS_TXBEGIN do { \
		UCSR0B = BV(RXCIE) | BV(UDRIE) | BV(RXEN) | BV(TXEN); \
	} while (0)
#endif

#ifndef SER_UART0_BUS_TXCHAR
	/*!
	 * Invoked to send one character.
	 */
	#define SER_UART0_BUS_TXCHAR(c) do { \
		UDR0 = (c); \
	} while (0)
#endif

#ifndef SER_UART0_BUS_TXEND
	/*!
	 * Invoked as soon as the txfifo becomes empty
	 *
	 * - Keep both the receiver and the transmitter enabled
	 * - Keep the RX complete interrupt enabled
	 * - Disable the UDR empty interrupt
	 */
	#define SER_UART0_BUS_TXEND do { \
		UCSR0B = BV(RXCIE) | BV(RXEN) | BV(TXEN); \
	} while (0)
#endif

#ifndef SER_UART0_BUS_TXOFF
	/*!
	 * \def SER_UART0_BUS_TXOFF
	 *
	 * Invoked after the last character has been transmitted
	 *
	 * The default is no action.
	 */
#endif

#ifndef SER_UART1_BUS_TXINIT
	/*! \sa SER_UART0_BUS_TXINIT */
	#define SER_UART1_BUS_TXINIT do { \
		UCSR1B = BV(RXCIE) | BV(RXEN) | BV(TXEN); \
	} while (0)
#endif
#ifndef SER_UART1_BUS_TXBEGIN
	/*! \sa SER_UART0_BUS_TXBEGIN */
	#define SER_UART1_BUS_TXBEGIN do { \
		UCSR1B = BV(RXCIE) | BV(UDRIE) | BV(RXEN) | BV(TXEN); \
	} while (0)
#endif
#ifndef SER_UART1_BUS_TXCHAR
	/*! \sa SER_UART0_BUS_TXCHAR */
	#define SER_UART1_BUS_TXCHAR(c) do { \
		UDR1 = (c); \
	} while (0)
#endif
#ifndef SER_UART1_BUS_TXEND
	/*! \sa SER_UART0_BUS_TXEND */
	#define SER_UART1_BUS_TXEND do { \
		UCSR1B = BV(RXCIE) | BV(RXEN) | BV(TXEN); \
	} while (0)
#endif
#ifndef SER_UART1_BUS_TXOFF
	/*!
	 * \def SER_UART1_BUS_TXOFF
	 *
	 * \see SER_UART0_BUS_TXOFF
	 */
#endif
/*\}*/


/* SPI port and pin configuration */
#define SPI_PORT      PORTB
#define SPI_DDR       DDRB
#define SPI_SCK_BIT   PB1
#define SPI_MOSI_BIT  PB2
#define SPI_MISO_BIT  PB3

/* USART registers definitions */
#if defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__)
	#define AVR_HAS_UART1 1
#elif defined(__AVR_ATmega8__)
	#define AVR_HAS_UART1 0
	#define UCSR0A UCSRA
	#define UCSR0B UCSRB
	#define UCSR0C UCSRC
	#define UDR0   UDR
	#define UBRR0L UBRRL
	#define UBRR0H UBRRH
	#define SIG_UART0_DATA SIG_UART_DATA
	#define SIG_UART0_RECV SIG_UART_RECV
#elif defined(__AVR_ATmega103__)
	#define AVR_HAS_UART1 0
	#define UCSR0B UCR
	#define UDR0   UDR
	#define UCSR0A USR
	#define UBRR0L UBRR
	#define SIG_UART0_DATA SIG_UART_DATA
	#define SIG_UART0_RECV SIG_UART_RECV
#else
	#error Unknown architecture
#endif


/*!
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
#if AVR_HAS_UART1
	static unsigned char uart1_txbuffer[CONFIG_UART1_TXBUFSIZE];
	static unsigned char uart1_rxbuffer[CONFIG_UART1_RXBUFSIZE];
#endif
static unsigned char spi_txbuffer[CONFIG_SPI_TXBUFSIZE];
static unsigned char spi_rxbuffer[CONFIG_SPI_RXBUFSIZE];


/*!
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
struct AvrSerial
{
	struct SerialHardware hw;
	volatile bool sending;
};


/*
 * These are to trick GCC into *not* using
 * absolute addressing mode when accessing
 * ser_handles, which is very expensive.
 *
 * Accessing through these pointers generates
 * much shorter (and hopefully faster) code.
 */
struct Serial *ser_uart0 = &ser_handles[SER_UART0];
#if AVR_HAS_UART1
struct Serial *ser_uart1 = &ser_handles[SER_UART1];
#endif
struct Serial *ser_spi = &ser_handles[SER_SPI];



/*
 * Callbacks
 */
static void uart0_init(UNUSED(struct SerialHardware *, _hw), UNUSED(struct Serial *, ser))
{
	SER_UART0_BUS_TXINIT;
	RTS_ON;
}

static void uart0_cleanup(UNUSED(struct SerialHardware *, _hw))
{
	UCSR0B = 0;
}

static void uart0_enabletxirq(struct SerialHardware *_hw)
{
	struct AvrSerial *hw = (struct AvrSerial *)_hw;

	/*
	 * WARNING: racy code here!  The tx interrupt
	 * sets hw->sending to false when it runs with
	 * an empty fifo.  The order of the statements
	 * in the if-block matters.
	 */
	if (!hw->sending)
	{
		hw->sending = true;
		SER_UART0_BUS_TXBEGIN;
	}
}

static void uart0_setbaudrate(UNUSED(struct SerialHardware *, _hw), unsigned long rate)
{
	/* Compute baud-rate period */
	uint16_t period = (((CLOCK_FREQ / 16UL) + (rate / 2)) / rate) - 1;

#ifndef __AVR_ATmega103__
	UBRR0H = (period) >> 8;
#endif
	UBRR0L = (period);

	//DB(kprintf("uart0_setbaudrate(rate=%lu): period=%d\n", rate, period);)
}

static void uart0_setparity(UNUSED(struct SerialHardware *, _hw), int parity)
{
#ifndef __AVR_ATmega103__
	UCSR0C |= (parity) << UPM0;
#endif
}

#if AVR_HAS_UART1

static void uart1_init(UNUSED(struct SerialHardware *, _hw), UNUSED(struct Serial *, ser))
{
	SER_UART1_BUS_TXINIT;
	RTS_ON;
	SER_STROBE_INIT;
}

static void uart1_cleanup(UNUSED(struct SerialHardware *, _hw))
{
	UCSR1B = 0;
}

static void uart1_enabletxirq(struct SerialHardware *_hw)
{
	struct AvrSerial *hw = (struct AvrSerial *)_hw;

	/*
	 * WARNING: racy code here!  The tx interrupt
	 * sets hw->sending to false when it runs with
	 * an empty fifo.  The order of the statements
	 * in the if-block matters.
	 */
	if (!hw->sending)
	{
		hw->sending = true;
		SER_UART1_BUS_TXBEGIN;
	}
}

static void uart1_setbaudrate(UNUSED(struct SerialHardware *, _hw), unsigned long rate)
{
	/* Compute baud-rate period */
	uint16_t period = (((CLOCK_FREQ / 16UL) + (rate / 2)) / rate) - 1;

	UBRR1H = (period) >> 8;
	UBRR1L = (period);

	//DB(kprintf("uart1_setbaudrate(rate=%ld): period=%d\n", rate, period);)
}

static void uart1_setparity(UNUSED(struct SerialHardware *, _hw), int parity)
{
	UCSR1C |= (parity) << UPM0;
}

#endif // AVR_HAS_UART1

static void spi_init(UNUSED(struct SerialHardware *, _hw), UNUSED(struct Serial *, ser))
{
	/*
	 * Set MOSI and SCK ports out, MISO in.
	 *
	 * The ATmega64/128 datasheet explicitly states that the input/output
	 * state of the SPI pins is not significant, as when the SPI is
	 * active the I/O port are overrided.
	 * This is *blatantly FALSE*.
	 *
	 * Moreover, the MISO pin on the board_kc *must* be in high impedance
	 * state even when the SPI is off, because the line is wired together
	 * with the KBus serial RX, and the transmitter of the slave boards
	 * would be unable to drive the line.
	 */
	SPI_DDR |= BV(SPI_MOSI_BIT) | BV(SPI_SCK_BIT);
	SPI_DDR &= ~BV(SPI_MISO_BIT);
	/* Enable SPI, IRQ on, Master, CPU_CLOCK/16 */
	SPCR = BV(SPE) | BV(SPIE) | BV(MSTR) | BV(SPR0);
}

static void spi_cleanup(UNUSED(struct SerialHardware *, _hw))
{
	SPCR = 0;
	/* Set all pins as inputs */
	SPI_DDR &= ~(BV(SPI_MISO_BIT) | BV(SPI_MOSI_BIT) | BV(SPI_SCK_BIT));
}

static void spi_starttx(struct SerialHardware *_hw)
{
	struct AvrSerial *hw = (struct AvrSerial *)_hw;

	cpuflags_t flags;
	DISABLE_IRQSAVE(flags);

	/* Send data only if the SPI is not already transmitting */
	if (!hw->sending && !fifo_isempty(&ser_spi->txfifo))
	{
		hw->sending = true;
		SPDR = fifo_pop(&ser_spi->txfifo);
	}

	ENABLE_IRQRESTORE(flags);
}

static void spi_setbaudrate(UNUSED(struct SerialHardware *, _hw), UNUSED(unsigned long, rate))
{
	// nop
}

static void spi_setparity(UNUSED(struct SerialHardware *, _hw), UNUSED(int, parity))
{
	// nop
}



/*
 * High-level interface data structures
 */
static const struct SerialHardwareVT UART0_VT =
{
	.init = uart0_init,
	.cleanup = uart0_cleanup,
	.setbaudrate = uart0_setbaudrate,
	.setparity = uart0_setparity,
	.enabletxirq = uart0_enabletxirq,
};

#if AVR_HAS_UART1
static const struct SerialHardwareVT UART1_VT =
{
	.init = uart1_init,
	.cleanup = uart1_cleanup,
	.setbaudrate = uart1_setbaudrate,
	.setparity = uart1_setparity,
	.enabletxirq = uart1_enabletxirq,
};
#endif // AVR_HAS_UART1

static const struct SerialHardwareVT SPI_VT =
{
	.init = spi_init,
	.cleanup = spi_cleanup,
	.setbaudrate = spi_setbaudrate,
	.setparity = spi_setparity,
	.enabletxirq = spi_starttx,
};

static struct AvrSerial UARTDescs[SER_CNT] =
{
	{
		.hw = {
			.table = &UART0_VT,
			.txbuffer = uart0_txbuffer,
			.rxbuffer = uart0_rxbuffer,
			.txbuffer_size = CONFIG_UART0_TXBUFSIZE,
			.rxbuffer_size = CONFIG_UART0_RXBUFSIZE,
		},
		.sending = false,
	},
#if AVR_HAS_UART1
	{
		.hw = {
			.table = &UART1_VT,
			.txbuffer = uart1_txbuffer,
			.rxbuffer = uart1_rxbuffer,
			.txbuffer_size = CONFIG_UART1_TXBUFSIZE,
			.rxbuffer_size = CONFIG_UART1_RXBUFSIZE,
		},
		.sending = false,
	},
#endif
	{
		.hw = {
			.table = &SPI_VT,
			.txbuffer = spi_txbuffer,
			.rxbuffer = spi_rxbuffer,
			.txbuffer_size = CONFIG_SPI_TXBUFSIZE,
			.rxbuffer_size = CONFIG_SPI_RXBUFSIZE,
		},
		.sending = false,
	}
};

struct SerialHardware* ser_hw_getdesc(int unit)
{
	ASSERT(unit < SER_CNT);
	return &UARTDescs[unit].hw;
}



/*
 * Interrupt handlers
 */

#if CONFIG_SER_HWHANDSHAKE

//! This interrupt is triggered when the CTS line goes high
SIGNAL(SIG_CTS)
{
	// Re-enable UDR empty interrupt and TX, then disable CTS interrupt
	UCSR0B = BV(RXCIE) | BV(UDRIE) | BV(RXEN) | BV(TXEN);
	cbi(EIMSK, EIMSKB_CTS);
}

#endif // CONFIG_SER_HWHANDSHAKE


/*!
 * Serial 0 TX interrupt handler
 */
SIGNAL(SIG_UART0_DATA)
{
	SER_STROBE_ON;

	struct FIFOBuffer * const txfifo = &ser_uart0->txfifo;

	if (fifo_isempty(txfifo))
	{
		SER_UART0_BUS_TXEND;
#ifndef SER_UART0_BUS_TXOFF
		UARTDescs[SER_UART0].sending = false;
#endif
	}
#if CPU_AVR_ATMEGA64 || CPU_AVR_ATMEGA128 || CPU_AVR_ATMEGA103
	else if (!IS_CTS_ON)
	{
		// Disable rx interrupt and tx, enable CTS interrupt
		// UNTESTED
		UCSR0B = BV(RXCIE) | BV(RXEN) | BV(TXEN);
		sbi(EIFR, EIMSKB_CTS);
		sbi(EIMSK, EIMSKB_CTS);
	}
#endif
	else
	{
		char c = fifo_pop(txfifo);
		SER_UART0_BUS_TXCHAR(c);
	}

	SER_STROBE_OFF;
}

#ifdef SER_UART0_BUS_TXOFF
/*!
 * Serial port 0 TX complete interrupt handler.
 *
 * This IRQ is usually disabled.  The UDR-empty interrupt
 * enables it when there's no more data to transmit.
 * We need to wait until the last character has been
 * transmitted before switching the 485 transceiver to
 * receive mode.
 *
 * The txfifo might have been refilled by putchar() while
 * we were waiting for the transmission complete interrupt.
 * In this case, we must restart the UDR empty interrupt,
 * otherwise we'd stop the serial port with some data
 * still pending in the buffer.
 */
SIGNAL(SIG_UART0_TRANS)
{
	SER_STROBE_ON;

	struct FIFOBuffer * const txfifo = &ser_uart0->txfifo;
	if (fifo_isempty(txfifo))
	{
		SER_UART0_BUS_TXOFF;
		UARTDescs[SER_UART0].sending = false;
	}
	else
		UCSR0B = BV(RXCIE) | BV(UDRIE) | BV(RXEN) | BV(TXEN);

	SER_STROBE_OFF;
}
#endif /* SER_UART0_BUS_TXOFF */


#if AVR_HAS_UART1

/*!
 * Serial 1 TX interrupt handler
 */
SIGNAL(SIG_UART1_DATA)
{
	SER_STROBE_ON;

	struct FIFOBuffer * const txfifo = &ser_uart1->txfifo;

	if (fifo_isempty(txfifo))
	{
		SER_UART1_BUS_TXEND;
#ifndef SER_UART1_BUS_TXOFF
		UARTDescs[SER_UART1].sending = false;
#endif
	}
#if CPU_AVR_ATMEGA64 || CPU_AVR_ATMEGA128 || CPU_AVR_ATMEGA103
	else if (!IS_CTS_ON)
	{
		// Disable rx interrupt and tx, enable CTS interrupt
		// UNTESTED
		UCSR1B = BV(RXCIE) | BV(RXEN) | BV(TXEN);
		sbi(EIFR, EIMSKB_CTS);
		sbi(EIMSK, EIMSKB_CTS);
	}
#endif
	else
	{
		char c = fifo_pop(txfifo);
		SER_UART1_BUS_TXCHAR(c);
	}

	SER_STROBE_OFF;
}

#ifdef SER_UART1_BUS_TXOFF
/*!
 * Serial port 1 TX complete interrupt handler.
 *
 * \sa port 0 TX complete handler.
 */
SIGNAL(SIG_UART1_TRANS)
{
	SER_STROBE_ON;

	struct FIFOBuffer * const txfifo = &ser_uart1->txfifo;
	if (fifo_isempty(txfifo))
	{
		SER_UART1_BUS_TXOFF;
		UARTDescs[SER_UART1].sending = false;
	}
	else
		UCSR1B = BV(RXCIE) | BV(UDRIE) | BV(RXEN) | BV(TXEN);

	SER_STROBE_OFF;
}
#endif /* SER_UART1_BUS_TXOFF */

#endif // AVR_HAS_UART1


/*!
 * Serial 0 RX complete interrupt handler.
 *
 * This handler is interruptible.
 * Interrupt are reenabled as soon as recv complete interrupt is
 * disabled. Using INTERRUPT() is troublesome when the serial
 * is heavily loaded, because an interrupt could be retriggered
 * when executing the handler prologue before RXCIE is disabled.
 *
 * \note The code that re-enables interrupts is commented out
 *       because in some nasty cases the interrupt is retriggered.
 *       This is probably due to the RXC flag being set before
 *       RXCIE is cleared.  Unfortunately the RXC flag is read-only
 *       and can't be cleared by code.
 */
SIGNAL(SIG_UART0_RECV)
{
	SER_STROBE_ON;

	/* Disable Recv complete IRQ */
	//UCSR0B &= ~BV(RXCIE);
	//ENABLE_INTS;

	/* Should be read before UDR */
	ser_uart0->status |= UCSR0A & (SERRF_RXSROVERRUN | SERRF_FRAMEERROR);

	/* To clear the RXC flag we must _always_ read the UDR even when we're
	 * not going to accept the incoming data, otherwise a new interrupt
	 * will occur once the handler terminates.
	 */
	char c = UDR0;
	struct FIFOBuffer * const rxfifo = &ser_uart0->rxfifo;

	if (fifo_isfull(rxfifo))
		ser_uart0->status |= SERRF_RXFIFOOVERRUN;
	else
	{
		fifo_push(rxfifo, c);
#if CONFIG_SER_HWHANDSHAKE
		if (fifo_isfull(rxfifo))
			RTS_OFF;
#endif
	}

	/* Reenable receive complete int */
	//DISABLE_INTS;
	//UCSR0B |= BV(RXCIE);

	SER_STROBE_OFF;
}


#if AVR_HAS_UART1

/*!
 * Serial 1 RX complete interrupt handler.
 *
 * This handler is interruptible.
 * Interrupt are reenabled as soon as recv complete interrupt is
 * disabled. Using INTERRUPT() is troublesome when the serial
 * is heavily loaded, because an interrupt could be retriggered
 * when executing the handler prologue before RXCIE is disabled.
 *
 * \see SIGNAL(SIG_UART0_RECV)
 */
SIGNAL(SIG_UART1_RECV)
{
	SER_STROBE_ON;

	/* Disable Recv complete IRQ */
	//UCSR1B &= ~BV(RXCIE);
	//ENABLE_INTS;

	/* Should be read before UDR */
	ser_uart1->status |= UCSR1A & (SERRF_RXSROVERRUN | SERRF_FRAMEERROR);

	/* To avoid an IRQ storm, we must _always_ read the UDR even when we're
	 * not going to accept the incoming data
	 */
	char c = UDR1;
	struct FIFOBuffer * const rxfifo = &ser_uart1->rxfifo;
	//ASSERT_VALID_FIFO(rxfifo);

	if (UNLIKELY(fifo_isfull(rxfifo)))
		ser_uart1->status |= SERRF_RXFIFOOVERRUN;
	else
	{
		fifo_push(rxfifo, c);
#if CONFIG_SER_HWHANDSHAKE
		if (fifo_isfull(rxfifo))
			RTS_OFF;
#endif
	}
	/* Re-enable receive complete int */
	//UCSR1B |= BV(RXCIE);

	SER_STROBE_OFF;
}

#endif // AVR_HAS_UART1


/*!
 * SPI interrupt handler
 */
SIGNAL(SIG_SPI)
{
	/* Read incoming byte. */
	if (!fifo_isfull(&ser_spi->rxfifo))
		fifo_push(&ser_spi->rxfifo, SPDR);
	/*
	 * FIXME
	else
		ser_spi->status |= SERRF_RXFIFOOVERRUN;
	*/

	/* Send */
	if (!fifo_isempty(&ser_spi->txfifo))
		SPDR = fifo_pop(&ser_spi->txfifo);
	else
		UARTDescs[SER_SPI].sending = false;
}
