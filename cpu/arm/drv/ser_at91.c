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

#include <cpu/attr.h>
#include <drv/ser.h>
#include <drv/ser_p.h>

#include <hw/hw_ser.h>  /* Required for bus macros overrides */
#include <hw/hw_cpu.h>  /* CLOCK_FREQ */

#include <mware/fifobuf.h>
#include <cfg/debug.h>

#include <appconfig.h>

#define SERIRQ_PRIORITY 4 ///< default priority for serial irqs.

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

#ifndef SER_UART0_IRQ_INIT
	/**
	 * Default IRQ INIT macro - invoked in uart0_init()
	 *
	 * - Disable all interrupt
	 * - Register USART0 interrupt
	 * - Enable USART0 clock.
	 */
	#define SER_UART0_IRQ_INIT do { \
		US0_IDR = 0xFFFFFFFF; \
		/* Set the vector. */ \
		AIC_SVR(US0_ID) = uart0_irq_dispatcher; \
		/* Initialize to edge triggered with defined priority. */ \
		AIC_SMR(US0_ID) = AIC_SRCTYPE_INT_EDGE_TRIGGERED | SERIRQ_PRIORITY; \
		/* Enable the USART IRQ */ \
		AIC_IECR = BV(US0_ID); \
		PMC_PCER = BV(US0_ID); \
	} while (0)
#endif

#ifndef SER_UART0_BUS_TXINIT
	/**
	 * Default TXINIT macro - invoked in uart0_init()
	 *
	 * - Disable GPIO on USART0 tx/rx pins
	 * - Reset USART0
	 * - Set serial param: mode Normal, 8bit data, 1bit stop
	 * - Enable both the receiver and the transmitter
	 * - Enable only the RX complete interrupt
	 */
	#if !CPU_ARM_AT91SAM7S256
		#warning Check USART0 pins!
	#endif
	#define SER_UART0_BUS_TXINIT do { \
		PIOA_PDR = BV(RXD0) | BV(TXD0); \
		US0_CR = BV(US_RSTRX) | BV(US_RSTTX); \
		US0_MR = US_CHMODE_NORMAL | US_CHRL_8 | US_NBSTOP_1; \
		US0_CR = BV(US_RXEN) | BV(US_TXEN); \
		US0_IER = BV(US_RXRDY); \
	} while (0)

#endif

#ifndef SER_UART0_BUS_TXBEGIN
	/**
	 * Invoked before starting a transmission
	 *
	 * - Enable both the receiver and the transmitter
	 * - Enable both the RX complete and TX empty interrupts
	 */
	#define SER_UART0_BUS_TXBEGIN do { \
		US0_CR = BV(US_RXEN) | BV(US_TXEN); \
		US0_IER = BV(US_TXRDY) | BV(US_RXRDY); \
	} while (0)
#endif

#ifndef SER_UART0_BUS_TXCHAR
	/**
	 * Invoked to send one character.
	 */
	#define SER_UART0_BUS_TXCHAR(c) do { \
		US0_THR = (c); \
	} while (0)
#endif

#ifndef SER_UART0_BUS_TXEND
	/**
	 * Invoked as soon as the txfifo becomes empty
	 *
	 * - Keep both the receiver and the transmitter enabled
	 * - Keep the RX complete interrupt enabled
	 * - Disable the TX empty interrupts
	 */
	#define SER_UART0_BUS_TXEND do { \
		US0_CR = BV(US_RXEN) | BV(US_TXEN); \
		US0_IER = BV(US_RXRDY); \
		US0_IDR = BV(US_TXRDY); \
	} while (0)
#endif

/* End USART0 macros */

#ifndef SER_UART1_IRQ_INIT
	/** \sa SER_UART0_BUS_TXINIT */
	#define SER_UART1_IRQ_INIT do { \
		US1_IDR = 0xFFFFFFFF; \
		/* Set the vector. */ \
		AIC_SVR(US1_ID) = uart1_irq_dispatcher; \
		/* Initialize to edge triggered with defined priority. */ \
		AIC_SMR(US1_ID) = AIC_SRCTYPE_INT_EDGE_TRIGGERED | SERIRQ_PRIORITY; \
		/* Enable the USART IRQ */ \
		AIC_IECR = BV(US1_ID); \
		PMC_PCER = BV(US1_ID); \
	} while (0)
#endif

#ifndef SER_UART1_BUS_TXINIT
	/** \sa SER_UART1_BUS_TXINIT */
	#if !CPU_ARM_AT91SAM7S256
		#warning Check USART1 pins!
	#endif
	#define SER_UART1_BUS_TXINIT do { \
		PIOA_PDR = BV(RXD1) | BV(TXD1); \
		US1_CR = BV(US_RSTRX) | BV(US_RSTTX); \
		US1_MR = US_CHMODE_NORMAL | US_CHRL_8 | US_NBSTOP_1; \
		US1_CR = BV(US_RXEN) | BV(US_TXEN); \
		US1_IER = BV(US_RXRDY); \
	} while (0)
#endif

#ifndef SER_UART1_BUS_TXBEGIN
	/** \sa SER_UART1_BUS_TXBEGIN */
	#define SER_UART1_BUS_TXBEGIN do { \
		US1_CR = BV(US_RXEN) | BV(US_TXEN); \
		US1_IER = BV(US_TXRDY) | BV(US_RXRDY); \
	} while (0)
#endif

#ifndef SER_UART1_BUS_TXCHAR
	/** \sa SER_UART1_BUS_TXCHAR */
	#define SER_UART1_BUS_TXCHAR(c) do { \
		US1_THR = (c); \
	} while (0)
#endif

#ifndef SER_UART1_BUS_TXEND
	/** \sa SER_UART1_BUS_TXEND */
	#define SER_UART1_BUS_TXEND do { \
		US1_CR = BV(US_RXEN) | BV(US_TXEN); \
		US1_IER = BV(US_RXRDY); \
		US1_IDR = BV(US_TXRDY); \
	} while (0)
#endif

/**
 * \name Overridable SPI hooks
 *
 * These can be redefined in hw.h to implement
 * special bus policies such as slave select pin handling, etc.
 *
 * \{
 */
#ifndef SER_SPI_BUS_TXINIT
	/**
	 * Default TXINIT macro - invoked in spi_init()
	 * The default is no action.
	 */
	#define SER_SPI_BUS_TXINIT
#endif

#ifndef SER_SPI_BUS_TXCLOSE
	/**
	 * Invoked after the last character has been transmitted.
	 * The default is no action.
	 */
	#define SER_SPI_BUS_TXCLOSE
#endif
/*\}*/


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

static unsigned char uart1_txbuffer[CONFIG_UART1_TXBUFSIZE];
static unsigned char uart1_rxbuffer[CONFIG_UART1_RXBUFSIZE];

static unsigned char spi_txbuffer[CONFIG_SPI_TXBUFSIZE];
static unsigned char spi_rxbuffer[CONFIG_SPI_RXBUFSIZE];

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
struct Serial *ser_uart1 = &ser_handles[SER_UART1];
struct Serial *ser_spi = &ser_handles[SER_SPI];

static void uart0_irq_dispatcher(void);
static void uart1_irq_dispatcher(void);
/*
 * Callbacks for USART0
 */
static void uart0_init(
	UNUSED_ARG(struct SerialHardware *, _hw),
	UNUSED_ARG(struct Serial *, ser))
{
	SER_UART0_IRQ_INIT;
	SER_UART0_BUS_TXINIT;
	SER_STROBE_INIT;
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
		SER_UART0_BUS_TXBEGIN;
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
	US0_MR &= ~US_PAR_MASK;
	/* Set UART parity */
	switch(parity)
	{
		case SER_PARITY_NONE:
		{
            /* Parity mode. */
			US0_MR |= US_PAR_NO;
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
		default:
			ASSERT(0);
	}

}
/*
 * Callbacks for USART1
 */
static void uart1_init(
	UNUSED_ARG(struct SerialHardware *, _hw),
	UNUSED_ARG(struct Serial *, ser))
{
	SER_UART1_IRQ_INIT;
	SER_UART1_BUS_TXINIT;
	SER_STROBE_INIT;
}

static void uart1_cleanup(UNUSED_ARG(struct SerialHardware *, _hw))
{
	US1_CR = BV(US_RSTRX) | BV(US_RSTTX) | BV(US_RXDIS) | BV(US_TXDIS) | BV(US_RSTSTA);
}

static void uart1_enabletxirq(struct SerialHardware *_hw)
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
		SER_UART1_BUS_TXBEGIN;
	}
}

static void uart1_setbaudrate(UNUSED_ARG(struct SerialHardware *, _hw), unsigned long rate)
{
	/* Compute baud-rate period */
	US1_BRGR = CLOCK_FREQ / (16 * rate);
	//DB(kprintf("uart0_setbaudrate(rate=%lu): period=%d\n", rate, period);)
}

static void uart1_setparity(UNUSED_ARG(struct SerialHardware *, _hw), int parity)
{
	US1_MR &= ~US_PAR_MASK;
	/* Set UART parity */
	switch(parity)
	{
		case SER_PARITY_NONE:
		{
            /* Parity mode. */
			US1_MR |= US_PAR_NO;
			break;
		}
		case SER_PARITY_EVEN:
		{
            /* Even parity.*/
			US1_MR |= US_PAR_EVEN;
			break;
		}
		case SER_PARITY_ODD:
		{
            /* Odd parity.*/
			US1_MR |= US_PAR_ODD;
			break;
		}
		default:
			ASSERT(0);
	}

}

/* SPI driver */

static void spi_init(UNUSED_ARG(struct SerialHardware *, _hw), UNUSED_ARG(struct Serial *, ser))
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
	ATOMIC(SPI_DDR |= (BV(SPI_MOSI_BIT) | BV(SPI_SCK_BIT)));

	/*
	 * If the SPI master mode is activated and the SS pin is in input and tied low,
	 * the SPI hardware will automatically switch to slave mode!
	 * For proper communication this pins should therefore be:
	 * - as output
	 * - as input but tied high forever!
	 * This driver set the pin as output.
	 */
	#warning SPI SS pin set as output for proper operation, check schematics for possible conflicts.
	ATOMIC(SPI_DDR |= BV(SPI_SS_BIT));

	ATOMIC(SPI_DDR &= ~BV(SPI_MISO_BIT));
	/* Enable SPI, IRQ on, Master */
	SPCR = BV(SPE) | BV(SPIE) | BV(MSTR);

	/* Set data order */
	#if CONFIG_SPI_DATA_ORDER == SER_LSB_FIRST
		SPCR |= BV(DORD);
	#endif

	/* Set SPI clock rate */
	#if CONFIG_SPI_CLOCK_DIV == 128
		SPCR |= (BV(SPR1) | BV(SPR0));
	#elif (CONFIG_SPI_CLOCK_DIV == 64 || CONFIG_SPI_CLOCK_DIV == 32)
		SPCR |= BV(SPR1);
	#elif (CONFIG_SPI_CLOCK_DIV == 16 || CONFIG_SPI_CLOCK_DIV == 8)
		SPCR |= BV(SPR0);
	#elif (CONFIG_SPI_CLOCK_DIV == 4 || CONFIG_SPI_CLOCK_DIV == 2)
		// SPR0 & SDPR1 both at 0
	#else
		#error Unsupported SPI clock division factor.
	#endif

	/* Set SPI2X bit (spi double frequency) */
	#if (CONFIG_SPI_CLOCK_DIV == 128 || CONFIG_SPI_CLOCK_DIV == 64 \
	  || CONFIG_SPI_CLOCK_DIV == 16 || CONFIG_SPI_CLOCK_DIV == 4)
		SPSR &= ~BV(SPI2X);
	#elif (CONFIG_SPI_CLOCK_DIV == 32 || CONFIG_SPI_CLOCK_DIV == 8 || CONFIG_SPI_CLOCK_DIV == 2)
		SPSR |= BV(SPI2X);
	#else
		#error Unsupported SPI clock division factor.
	#endif

	/* Set clock polarity */
	#if CONFIG_SPI_CLOCK_POL == 1
		SPCR |= BV(CPOL);
	#endif

	/* Set clock phase */
	#if CONFIG_SPI_CLOCK_PHASE == 1
		SPCR |= BV(CPHA);
	#endif
	SER_SPI_BUS_TXINIT;

	SER_STROBE_INIT;
}

static void spi_cleanup(UNUSED_ARG(struct SerialHardware *, _hw))
{
	SPCR = 0;

	SER_SPI_BUS_TXCLOSE;

	/* Set all pins as inputs */
	ATOMIC(SPI_DDR &= ~(BV(SPI_MISO_BIT) | BV(SPI_MOSI_BIT) | BV(SPI_SCK_BIT) | BV(SPI_SS_BIT)));
}

static void spi_starttx(struct SerialHardware *_hw)
{
	struct AvrSerial *hw = (struct AvrSerial *)_hw;

	cpuflags_t flags;
	IRQ_SAVE_DISABLE(flags);

	/* Send data only if the SPI is not already transmitting */
	if (!hw->sending && !fifo_isempty(&ser_spi->txfifo))
	{
		hw->sending = true;
		SPDR = fifo_pop(&ser_spi->txfifo);
	}

	IRQ_RESTORE(flags);
}

static void spi_setbaudrate(
	UNUSED_ARG(struct SerialHardware *, _hw),
	UNUSED_ARG(unsigned long, rate))
{
	// nop
}

static void spi_setparity(UNUSED_ARG(struct SerialHardware *, _hw), UNUSED_ARG(int, parity))
{
	// nop
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

static const struct SerialHardwareVT UART1_VT =
{
	C99INIT(init, uart1_init),
	C99INIT(cleanup, uart1_cleanup),
	C99INIT(setBaudrate, uart1_setbaudrate),
	C99INIT(setParity, uart1_setparity),
	C99INIT(txStart, uart1_enabletxirq),
	C99INIT(txSending, tx_sending),
};

static const struct SerialHardwareVT SPI_VT =
{
	C99INIT(init, spi_init),
	C99INIT(cleanup, spi_cleanup),
	C99INIT(setBaudrate, spi_setbaudrate),
	C99INIT(setParity, spi_setparity),
	C99INIT(txStart, spi_starttx),
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
	},
	{
		C99INIT(hw, /**/) {
			C99INIT(table, &UART1_VT),
			C99INIT(txbuffer, uart1_txbuffer),
			C99INIT(rxbuffer, uart1_rxbuffer),
			C99INIT(txbuffer_size, sizeof(uart1_txbuffer)),
			C99INIT(rxbuffer_size, sizeof(uart1_rxbuffer)),
		},
		C99INIT(sending, false),
	},
	{
		C99INIT(hw, /**/) {
			C99INIT(table, &SPI_VT),
			C99INIT(txbuffer, spi_txbuffer),
			C99INIT(rxbuffer, spi_rxbuffer),
			C99INIT(txbuffer_size, sizeof(spi_txbuffer)),
			C99INIT(rxbuffer_size, sizeof(spi_rxbuffer)),
		},
		C99INIT(sending, false),
	}
};

struct SerialHardware *ser_hw_getdesc(int unit)
{
	ASSERT(unit < SER_CNT);
	return &UARTDescs[unit].hw;
}

/**
 * Serial 0 TX interrupt handler
 */
static void uart0_irq_tx(void)
{
	SER_STROBE_ON;

	struct FIFOBuffer * const txfifo = &ser_uart0->txfifo;

	if (fifo_isempty(txfifo))
	{
		SER_UART0_BUS_TXEND;
		UARTDescs[SER_UART0].sending = false;
	}
	else
	{
		char c = fifo_pop(txfifo);
		SER_UART0_BUS_TXCHAR(c);
	}

	SER_STROBE_OFF;
}

/**
 * Serial 0 RX complete interrupt handler.
 */
static void uart0_irq_rx(void)
{
	SER_STROBE_ON;

	/* Should be read before US_CRS */
	ser_uart0->status |= US0_CSR & (SERRF_RXSROVERRUN | SERRF_FRAMEERROR);

	char c = US0_RHR;
	struct FIFOBuffer * const rxfifo = &ser_uart0->rxfifo;

	if (fifo_isfull(rxfifo))
		ser_uart0->status |= SERRF_RXFIFOOVERRUN;
	else
		fifo_push(rxfifo, c);

	SER_STROBE_OFF;
}

/**
 * Serial IRQ dispatcher for USART0.
 */
static void uart0_irq_dispatcher(void) __attribute__ ((naked));
static void uart0_irq_dispatcher(void)
{
	IRQ_ENTRY();

	if (US0_IMR & BV(US_RXRDY))
		uart0_irq_rx();

	if (US0_IMR & BV(US_TXRDY))
		uart0_irq_tx();

	IRQ_EXIT();
}

/**
 * Serial 1 TX interrupt handler
 */
static void uart1_irq_tx(void)
{
	SER_STROBE_ON;

	struct FIFOBuffer * const txfifo = &ser_uart1->txfifo;

	if (fifo_isempty(txfifo))
	{
		SER_UART1_BUS_TXEND;
		UARTDescs[SER_UART1].sending = false;
	}
	else
	{
		char c = fifo_pop(txfifo);
		SER_UART1_BUS_TXCHAR(c);
	}

	SER_STROBE_OFF;
}

/**
 * Serial 1 RX complete interrupt handler.
 */
static void uart1_irq_rx(void)
{
	SER_STROBE_ON;

	/* Should be read before US_CRS */
	ser_uart1->status |= US1_CSR & (SERRF_RXSROVERRUN | SERRF_FRAMEERROR);

	char c = US1_RHR;
	struct FIFOBuffer * const rxfifo = &ser_uart1->rxfifo;

	if (fifo_isfull(rxfifo))
		ser_uart1->status |= SERRF_RXFIFOOVERRUN;
	else
		fifo_push(rxfifo, c);

	SER_STROBE_OFF;
}

/**
 * Serial IRQ dispatcher for USART1.
 */
static void uart1_irq_dispatcher(void) __attribute__ ((naked));
static void uart1_irq_dispatcher(void)
{
	IRQ_ENTRY();

	if (US1_IMR & BV(US_RXRDY))
		uart1_irq_rx();

	if (US1_IMR & BV(US_TXRDY))
		uart1_irq_tx();

	IRQ_EXIT();
}
