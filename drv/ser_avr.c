/*!
 * \file
 * <!--
 * Copyright 2000 Bernardo Innocenti <bernie@codewiz.org>
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief AVR UART and SPI I/O driver
 */

/*
 * $Log$
 * Revision 1.3  2004/06/02 21:35:24  aleph
 * Serial enhancements: interruptible receive handler and 8 bit serial status for AVR; remove volatile attribute to FIFOBuffer, useless for new fifobuf routens
 *
 * Revision 1.2  2004/05/23 18:21:53  bernie
 * Trim CVS logs and cleanup header info.
 *
 */

#include "ser.h"
#include "ser_p.h"
#include "kdebug.h"
#include "config.h"
#include "hw.h"
#include <mware/fifobuf.h>

extern struct Serial ser_handles[SER_CNT];

struct AvrSerial
{
	struct SerialHardware hw;
	struct Serial* serial;
};


/* Hardware handshake */
#define RTS_ON
#define RTS_OFF
#define IS_CTS_ON   true
#define IS_CTS_OFF  false


/* SPI port and pin configuration */
#define SPI_PORT      PORTB
#define SPI_DDR       DDRB
#define SPI_SCK_BIT   PORTB1
#define SPI_MOSI_BIT  PORTB2
#define SPI_MISO_BIT  PORTB3


#ifdef __AVR_ATmega103__
	/* Macro for ATmega103 compatibility */
	#define UCSR0B UCR
	#define UDR0   UDR
	#define UCSR0A USR
	#define UBRR0L UBRR
#else
	#define UCR  UCSR0B
	#define UDR  UDR0
	#define USR  UCSR0A
#endif


/* Transmission fill byte */
#define SER_FILL_BYTE 0xAA


static void uart0_enabletxirq(UNUSED(struct SerialHardware *ctx))
{
#ifdef CONFIG_SER_TXFILL
	UCSR0B = BV(RXCIE) | BV(UDRIE) | BV(RXEN) | BV(TXEN) | BV(UCSZ2);
#else
	UCSR0B = BV(RXCIE) | BV(UDRIE) | BV(RXEN) | BV(TXEN);
#endif
}

static void uart0_init(struct SerialHardware *_hw, struct Serial *ser)
{
	struct AvrSerial *hw = (struct AvrSerial *)_hw;
	hw->serial = ser;

	/* Set TX port as input with pull-up enabled to avoid
	 * noise on the remote RX when TX is disabled */
	cpuflags_t flags;
	DISABLE_IRQSAVE(flags);
	DDRE &= ~BV(PORTE1);
	PORTE |= BV(PORTE1);
	ENABLE_IRQRESTORE(flags);

	/* TODO: explain why TX is disabled whenever possible */
#ifdef CONFIG_SER_TXFILL
	/*!
	 * Set multiprocessor mode and 9 bit data frame.
	 * The receiver keep MPCM bit always on. When useful data
	 * is trasmitted the ninth bit is set. Receiver consider the
	 * frame as address info and receive it.
	 * When useless fill bytes are sent the ninth bit is cleared
	 * and the receiver will ignore them, avoiding useless triggering
	 * of RXC interrupt.
	 */
	UCSR0A = BV(MPCM);
	UCSR0B = BV(RXCIE) | BV(RXEN) | BV(UCSZ2);
#else
	UCSR0B = BV(RXCIE) | BV(RXEN);
#endif

	RTS_ON;
}

static void uart0_cleanup(UNUSED(struct SerialHardware *ctx))
{
	UCSR0B = 0;
}

static void uart0_setbaudrate(UNUSED(struct SerialHardware *ctx), unsigned long rate)
{
	// Compute baud-rate period
	uint16_t period = (((CLOCK_FREQ / 16UL) + (rate / 2)) / rate) - 1;

#ifndef __AVR_ATmega103__
	UBRR0H = (period) >> 8;
#endif
	UBRR0L = (period);
}


#ifndef __AVR_ATmega103__

static void uart1_enabletxirq(UNUSED(struct SerialHardware *ctx))
{
	UCSR1B = BV(RXCIE) | BV(UDRIE) | BV(RXEN) | BV(TXEN);
}

static void uart1_init(struct SerialHardware *_hw, struct Serial *ser)
{
	struct AvrSerial *hw = (struct AvrSerial *)_hw;
	hw->serial = ser;

	/* Set TX port as input with pull-up enabled to avoid
	 * noise on the remote RX when TX is disabled */
	cpuflags_t flags;
	DISABLE_IRQSAVE(flags);
	DDRD &= ~BV(PORTD3);
	PORTD |= BV(PORTD3);
	ENABLE_IRQRESTORE(flags);

	/* TODO: explain why TX is disabled whenever possible */
	UCSR1B = BV(RXCIE) | BV(RXEN);

	RTS_ON;
}

static void uart1_cleanup(UNUSED(struct SerialHardware *ctx))
{
	UCSR1B = 0;
}

static void uart1_setbaudrate(UNUSED(struct SerialHardware *ctx), unsigned long rate)
{
	// Compute baud-rate period
	uint16_t period = (((CLOCK_FREQ / 16UL) + (rate / 2)) / rate) - 1;

	UBRR1H = (period) >> 8;
	UBRR1L = (period);
}

static void uart0_setparity(UNUSED(struct SerialHardware *ctx), int parity)
{
	UCSR0C |= (parity) << UPM0;
}

static void uart1_setparity(UNUSED(struct SerialHardware *ctx), int parity)
{
	UCSR1C |= (parity) << UPM0;
}

#endif /* !__AVR_ATmega103__ */


static void spi_init(struct SerialHardware *_hw, struct Serial *ser)
{
	struct AvrSerial *hw = (struct AvrSerial *)_hw;
	hw->serial = ser;

	/* MOSI and SCK out, MISO in */
	SPI_DDR |= BV(SPI_MOSI_BIT) | BV(SPI_SCK_BIT);
	SPI_DDR &= ~BV(SPI_MISO_BIT);
	/* Enable SPI, IRQ on, Master, CPU_CLOCK/16 */
	SPCR = BV(SPE) | BV(SPIE) | BV(MSTR) | BV(SPR0);
}

static void spi_cleanup(UNUSED(struct SerialHardware *ctx))
{
	SPCR = 0;
	/* Set all pins as inputs */
	SPI_DDR &= ~(BV(SPI_MISO_BIT) | BV(SPI_MOSI_BIT) | BV(SPI_SCK_BIT));
}



#if defined(CONFIG_SER_HW_HANDSHAKE)

//! This interrupt is triggered when the CTS line goes high
SIGNAL(SIG_CTS)
{
	// Re-enable UDR empty interrupt and TX, then disable CTS interrupt
	UCR = BV(RXCIE) | BV(UDRIE) | BV(RXEN) | BV(TXEN);
	cbi(EIMSK, EIMSKB_CTS);
}

#endif // CONFIG_SER_HW_HANDSHAKE


/*!
 * Serial 0 TX interrupt handler
 */
#ifdef __AVR_ATmega103__
SIGNAL(SIG_UART_DATA)
#else
SIGNAL(SIG_UART0_DATA)
#endif
{
	if (fifo_isempty(&ser_handles[SER_UART0].txfifo))
	{
#ifdef CONFIG_SER_TXFILL
		/*
		 * To avoid audio interference: always transmit useless char.
		 * Send the byte with the ninth bit cleared, the receiver in MCPM mode
		 * will ignore it.
		 */
		UCSR0B &= ~BV(TXB8);
		UDR0 = SER_FILL_BYTE;
#else
		/* Disable UDR empty interrupt and transmitter */
		UCR = BV(RXCIE) | BV(RXEN);
#endif
	}
#if defined(CONFIG_SER_HWHANDSHAKE)
	else if (IS_CTS_OFF)
	{
		// disable rx interrupt and tx, enable CTS interrupt
		UCR = BV(RXCIE) | BV(RXEN);
		sbi(EIFR, EIMSKB_CTS);
		sbi(EIMSK, EIMSKB_CTS);
	}
#endif // CONFIG_SER_HWHANDSHAKE
	else
	{
#ifdef CONFIG_SER_TXFILL
		/* Send with ninth bit set. Receiver in MCPM mode will receive it */
		UCSR0B |= BV(TXB8);
#endif
		UDR = fifo_pop(&ser_handles[SER_UART0].txfifo);
	}
}

/*!
 * Serial 1 TX interrupt handler
 */
#ifndef __AVR_ATmega103__
SIGNAL(SIG_UART1_DATA)
{
	if (fifo_isempty(&ser_handles[SER_UART1].txfifo))
	{
		/* Disable UDR empty interrupt and transmitter */
		UCSR1B = BV(RXCIE) | BV(RXEN);
	}
#if defined(CONFIG_SER_HWHANDSHAKE)
	else if (IS_CTS_OFF)
	{
		// disable rx interrupt and tx, enable CTS interrupt
		UCSR1B = BV(RXCIE) | BV(RXEN);
		sbi(EIFR, EIMSKB_CTS);
		sbi(EIMSK, EIMSKB_CTS);
	}
#endif // CONFIG_SER_HWHANDSHAKE
	else
		UDR1 = fifo_pop(&ser_handles[SER_UART1].txfifo);
}
#endif /* !__AVR_ATmega103__ */


/*!
 * Serial 0 RX complete interrupt handler.
 *
 * This handler is interruptible.
 * Interrupt are reenabled as soon as recv complete interrupt is
 * disabled. Using INTERRUPT() is troublesome when the serial
 * is heavily loaded, because and interrupt could be retriggered
 * when executing the handler prologue before RXCIE is disabled.
 */
#ifdef __AVR_ATmega103__
SIGNAL(SIG_UART_RECV)
#else
SIGNAL(SIG_UART0_RECV)
#endif
{
	/* Disable Recv complete IRQ */
	UCR &= ~BV(RXCIE);
	ENABLE_INTS;

	/* Should be read before UDR */
	ser_handles[SER_UART0].status |= USR & (SERRF_RXSROVERRUN | SERRF_FRAMEERROR);

	/* To clear the RXC flag we must _always_ read the UDR even when we're
	 * not going to accept the incoming data, otherwise a new interrupt
	 * will occur once the handler terminates.
	 */
	char c = UDR;

	if (fifo_isfull(&ser_handles[SER_UART0].rxfifo))
		ser_handles[SER_UART0].status |= SERRF_RXFIFOOVERRUN;
	else
	{
		fifo_push(&ser_handles[SER_UART0].rxfifo, c);
#if defined(CONFIG_SER_HW_HANDSHAKE)
		if (fifo_isfull(&ser_handles[SER_UART0].rxfifo))
			RTS_OFF;
#endif
	}
	/* Reenable receive complete int */
	UCR |= BV(RXCIE);
}

/*!
 * Serial 1 RX complete interrupt handler.
 *
 * This handler is interruptible.
 * Interrupt are reenabled as soon as recv complete interrupt is
 * disabled. Using INTERRUPT() is troublesome when the serial
 * is heavily loaded, because and interrupt could be retriggered
 * when executing the handler prologue before RXCIE is disabled.
 */
#ifndef __AVR_ATmega103__
SIGNAL(SIG_UART1_RECV)
{
	/* Disable Recv complete IRQ */
	UCSR0B &= ~BV(RXCIE);
	ENABLE_INTS;

	/* Should be read before UDR */
	ser_handles[SER_UART1].status |= UCSR1A & (SERRF_RXSROVERRUN | SERRF_FRAMEERROR);

	/* To avoid an IRQ storm, we must _always_ read the UDR even when we're
	 * not going to accept the incoming data
	 */
	char c = UDR1;

	if (fifo_isfull(&ser_handles[SER_UART1].rxfifo))
		ser_handles[SER_UART1].status |= SERRF_RXFIFOOVERRUN;
	else
	{
		fifo_push(&ser_handles[SER_UART1].rxfifo, c);
#if defined(CONFIG_SER_HW_HANDSHAKE)
		if (fifo_isfull(&ser_handles[SER_UART1].rxfifo))
			RTS_OFF;
#endif
	}
	/* Reenable receive complete int */
	UCSR0B |= BV(RXCIE);
}
#endif /* !__AVR_ATmega103__ */


/*
 * SPI Flag: true if we are transmitting/receiving with the SPI.
 *
 * This kludge is necessary because the SPI sends and receives bytes
 * at the same time and the SPI IRQ is unique for send/receive.
 * The only way to start transmission is to write data in SPDR (this
 * is done by ser_spi_starttx()). We do this *only* if a transfer is
 * not already started.
 */
static volatile bool spi_sending = false;

static void spi_starttx(UNUSED(struct SerialHardware *ctx))
{
	cpuflags_t flags;

	DISABLE_IRQSAVE(flags);

	/* Send data only if the SPI is not already transmitting */
	if (!spi_sending && !fifo_isempty(&ser_handles[SER_SPI].txfifo))
	{
		SPDR = fifo_pop(&ser_handles[SER_SPI].txfifo);
		spi_sending = true;
	}

	ENABLE_IRQRESTORE(flags);
}

/*!
 * SPI interrupt handler
 */
SIGNAL(SIG_SPI)
{
	/* Read incoming byte. */
	if (!fifo_isfull(&ser_handles[SER_SPI].rxfifo))
		fifo_push(&ser_handles[SER_SPI].rxfifo, SPDR);
	/*
	 * FIXME
	else
		ser_handles[SER_SPI].status |= SERRF_RXFIFOOVERRUN;
	*/

	/* Send */
	if (!fifo_isempty(&ser_handles[SER_SPI].txfifo))
		SPDR = fifo_pop(&ser_handles[SER_SPI].txfifo);
	else
		spi_sending = false;
}


/*

#pragma vector = UART_TXC_vect
__interrupt void UART_TXC_interrupt(void)
{
  UCSRB &= ~TXCIE;
  ReceiveMode();
  UCSRB = RXCIE | RXEN | TXEN;  //Abilito l'Interrupt in ricezione e RX e TX
}
*/


static const struct SerialHardwareVT UART0_VT =
{
	.init = uart0_init,
	.cleanup = uart0_cleanup,
	.setbaudrate = uart0_setbaudrate,
	.setparity = uart0_setparity,
	.enabletxirq = uart0_enabletxirq,
};

static const struct SerialHardwareVT UART1_VT =
{
	.init = uart1_init,
	.cleanup = uart1_cleanup,
	.setbaudrate = uart1_setbaudrate,
	.setparity = uart1_setparity,
	.enabletxirq = uart1_enabletxirq,
};

static const struct SerialHardwareVT SPI_VT =
{
	.init = spi_init,
	.cleanup = spi_cleanup,
	.enabletxirq = spi_starttx,
};

static struct AvrSerial UARTDescs[SER_CNT] =
{
	{
		.hw = { .table = &UART0_VT },
	},

	{
		.hw = { .table = &UART1_VT },
	},

	{
		.hw = { .table = &SPI_VT },
	},
};

struct SerialHardware* ser_hw_getdesc(int unit)
{
	ASSERT(unit < SER_CNT);
	return &UARTDescs[unit].hw;
}
