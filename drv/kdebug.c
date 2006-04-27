/*!
 * \file
 * <!--
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000, 2001, 2002 Bernardo Innocenti <bernie@codewiz.org>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief General pourpose debug support for embedded systems (implementation).
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.26  2006/04/27 05:40:27  bernie
 *#* Break on assertion failures.
 *#*
 *#* Revision 1.25  2005/06/27 21:26:24  bernie
 *#* Misc PGM fixes.
 *#*
 *#* Revision 1.24  2005/04/12 01:36:37  bernie
 *#* Add hack to enable TX line at module initialization.
 *#*
 *#* Revision 1.23  2005/04/11 19:10:27  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.22  2005/02/18 11:18:33  bernie
 *#* Fixes for Harvard processors from project_ks.
 *#*
 *#* Revision 1.21  2005/02/16 20:29:48  bernie
 *#* TRACE(), TRACEMSG(): Reduce code and data footprint.
 *#*
 *#* Revision 1.20  2005/01/25 08:36:40  bernie
 *#* kputnum(): Export.
 *#*
 *#* Revision 1.19  2004/12/31 17:47:45  bernie
 *#* Rename UNUSED() to UNUSED_ARG().
 *#*/

#include <cfg/debug.h>
#include <cfg/cpu.h>
#include <cfg/macros.h> /* for BV() */
#include <appconfig.h>
#include <hw.h>

#include <mware/formatwr.h> /* for _formatted_write() */

#ifdef _DEBUG

#if defined(_EMUL)
	#include <stdio.h>
	#define KDBG_WAIT_READY()      do { /*nop*/ } while(0)
	#define KDBG_WRITE_CHAR(c)     putchar((c))
	#define KDBG_MASK_IRQ(old)     do { (void)(old); } while(0)
	#define KDBG_RESTORE_IRQ(old)  do { /*nop*/ } while(0)
	typedef char kdbg_irqsave_t; /* unused */
#elif CPU_I196
	#include "Util196.h"
	#define KDBG_WAIT_READY()      do {} while (!(SP_STAT & (SPSF_TX_EMPTY | SPSF_TX_INT)))
	#define KDBG_WRITE_CHAR(c)     do { SBUF = (c); } while(0)
	#define KDBG_MASK_IRQ(old) \
		do { \
			(old) = INT_MASK1 & INT1F_TI; \
			INT_MASK1 &= ~INT1F_TI; \
		} while(0)
	#define KDBG_RESTORE_IRQ(old)  do { INT_MASK1 |= (old); }
	typedef uint16_t kdbg_irqsave_t; /* FIXME: unconfirmed */
#elif CPU_AVR
	#include <avr/io.h>

	#if CONFIG_KDEBUG_PORT == 0

		/*
		 * Support for special bus policies or external transceivers
		 * on UART0 (to be overridden in "hw.h").
		 *
		 * HACK: if we don't set TXEN, kdbg disables the transmitter
		 * after each output statement until the serial driver
		 * is initialized.  These glitches confuse the debug
		 * terminal that ends up printing some trash.
		 */
		#ifndef KDBG_UART0_BUS_INIT
		#define KDBG_UART0_BUS_INIT  do { \
				UCSR0B = BV(TXEN); \
			} while (0)
		#endif
		#ifndef KDBG_UART0_BUS_RX
		#define KDBG_UART0_BUS_RX    do {} while (0)
		#endif
		#ifndef KDBG_UART0_BUS_TX
		#define KDBG_UART0_BUS_TX    do {} while (0)
		#endif

		#if CPU_AVR_ATMEGA64 || CPU_AVR_ATMEGA128
			#define UCR UCSR0B
			#define UDR UDR0
			#define USR UCSR0A
		#elif CPU_AVR_ATMEGA8
			#define UCR UCSRB
			#define USR UCSRA
		#else
			#error Unknown CPU
		#endif

		#define KDBG_WAIT_READY()     do { loop_until_bit_is_set(USR, UDRE); } while(0)
		#define KDBG_WAIT_TXDONE()    do { loop_until_bit_is_set(USR, TXC); } while(0)

		/*
		 * We must clear the TXC flag before sending a new character to allow
		 * KDBG_WAIT_TXDONE() to work properly.
		 *
		 * BUG: if KDBG_WRITE_CHAR() is called after the TXC flag is set by hardware,
		 * a new TXC could be generated after we've cleared it and before the new
		 * character is written to UDR.  On a 485 bus, the transceiver will be put
		 * in RX mode while still transmitting the last char.
		 */
		#define KDBG_WRITE_CHAR(c)    do { USR |= BV(TXC); UDR = (c); } while(0)

		#define KDBG_MASK_IRQ(old)    do { \
			(old) = UCR; \
			UCR |= BV(TXEN); \
			UCR &= ~(BV(TXCIE) | BV(UDRIE)); \
			KDBG_UART0_BUS_TX; \
		} while(0)

		#define KDBG_RESTORE_IRQ(old) do { \
			KDBG_WAIT_TXDONE(); \
			KDBG_UART0_BUS_RX; \
			UCR = (old); \
		} while(0)

		typedef uint8_t kdbg_irqsave_t;

	#elif CONFIG_KDEBUG_PORT == 1

		/*
		 * Support for special bus policies or external transceivers
		 * on UART1 (to be overridden in "hw.h").
		 *
		 * HACK: if we don't set TXEN, kdbg disables the transmitter
		 * after each output statement until the serial driver
		 * is initialized.  These glitches confuse the debug
		 * terminal that ends up printing some trash.
		 */
		#ifndef KDBG_UART1_BUS_INIT
		#define KDBG_UART1_BUS_INIT  do { \
				UCSR1B = BV(TXEN); \
			} while (0)
		#endif
		#ifndef KDBG_UART1_BUS_RX
		#define KDBG_UART1_BUS_RX    do {} while (0)
		#endif
		#ifndef KDBG_UART1_BUS_TX
		#define KDBG_UART1_BUS_TX    do {} while (0)
		#endif

		#define KDBG_WAIT_READY()     do { loop_until_bit_is_set(UCSR1A, UDRE); } while(0)
		#define KDBG_WAIT_TXDONE()    do { loop_until_bit_is_set(UCSR1A, TXC); } while(0)
		#define KDBG_WRITE_CHAR(c)    do { UCSR1A |= BV(TXC); UDR1 = (c); } while(0)

		#define KDBG_MASK_IRQ(old)    do { \
			(old) = UCSR1B; \
			UCSR1B |= BV(TXEN); \
			UCSR1B &= ~(BV(TXCIE) | BV(UDRIE)); \
			KDBG_UART1_BUS_TX; \
		} while(0)

		#define KDBG_RESTORE_IRQ(old) do { \
			KDBG_WAIT_TXDONE(); \
			KDBG_UART1_BUS_RX; \
			UCSR1B = (old); \
		} while(0)

		typedef uint8_t kdbg_irqsave_t;
	#else
		#error CONFIG_KDEBUG_PORT should be either 0 or 1
	#endif
#elif defined(__MWERKS__) && CPU_DSP56K
	/* Debugging go through the JTAG interface. The MSL library already
	   implements the console I/O correctly. */
	#include <stdio.h>
	#define KDBG_WAIT_READY()         do { } while (0)
	#define KDBG_WRITE_CHAR(c)        __put_char(c, stdout)
	#define KDBG_MASK_IRQ(old)        do { (void)(old); } while (0)
	#define KDBG_RESTORE_IRQ(old)     do { (void)(old); } while (0)
	typedef uint8_t kdbg_irqsave_t; /* unused */
#else
	#error Unknown architecture
#endif


void kdbg_init(void)
{
#if CPU_I196

	/* Set serial port for 19200bps 8N1 */
	INT_MASK1 &= ~(INT1F_TI | INT1F_RI);
	SP_CON = SPCF_RECEIVE_ENABLE | SPCF_MODE1;
	ioc1_img |= IOC1F_TXD_SEL | IOC1F_EXTINT_SRC;
	IOC1 = ioc1_img;
	BAUD_RATE = 0x33;
	BAUD_RATE = 0x80;

#elif CPU_AVR

	/* Compute the baud rate */
	uint16_t period = (((CLOCK_FREQ / 16UL) + (CONFIG_KDEBUG_BAUDRATE / 2)) / CONFIG_KDEBUG_BAUDRATE) - 1;

	#if CPU_AVR_ATMEGA64 || CPU_AVR_ATMEGA128
		#if CONFIG_KDEBUG_PORT == 0
			UBRR0H = (uint8_t)(period>>8);
			UBRR0L = (uint8_t)period;
			KDBG_UART0_BUS_INIT;
		#elif CONFIG_KDEBUG_PORT == 1
			UBRR1H = (uint8_t)(period>>8);
			UBRR1L = (uint8_t)period;
			KDBG_UART1_BUS_INIT;
		#else
			#error CONFIG_KDEBUG_PORT must be either 0 or 1
		#endif
	#elif CPU_AVR_ATMEGA8
		UBRRH = (uint8_t)(period>>8);
		UBRRL = (uint8_t)period;
	#elif CPU_AVR_ATMEGA103
		UBRR = (uint8_t)period;
		KDBG_UART0_BUS_INIT;
	#else
		#error Unknown CPU
	#endif

#endif /* !CPU_I196 && !CPU_AVR */

	kputs("\n\n*** DBG START ***\n");
}


/*!
 * Output one character to the debug console
 */
static void __kputchar(char c, UNUSED_ARG(void *, unused))
{
	/* Poll while serial buffer is still busy */
	KDBG_WAIT_READY();

	/* Send '\n' as '\r\n' for dumb terminals */
	if (c == '\n')
	{
		KDBG_WRITE_CHAR('\r');
		KDBG_WAIT_READY();
	}

	KDBG_WRITE_CHAR(c);
}


void kputchar(char c)
{
	/* Mask serial TX intr */
	kdbg_irqsave_t irqsave;
	KDBG_MASK_IRQ(irqsave);

	__kputchar(c, 0);

	/* Restore serial TX intr */
	KDBG_RESTORE_IRQ(irqsave);
}


static void PGM_FUNC(kvprintf)(const char * PGM_ATTR fmt, va_list ap)
{
#if CONFIG_PRINTF
	/* Mask serial TX intr */
	kdbg_irqsave_t irqsave;
	KDBG_MASK_IRQ(irqsave);

	PGM_FUNC(_formatted_write)(fmt, __kputchar, 0, ap);

	/* Restore serial TX intr */
	KDBG_RESTORE_IRQ(irqsave);
#else
	/* A better than nothing printf() surrogate. */
	PGM_FUNC(kputs)(fmt);
#endif /* CONFIG_PRINTF */
}

void PGM_FUNC(kprintf)(const char * PGM_ATTR fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	PGM_FUNC(kvprintf)(fmt, ap);
	va_end(ap);
}

void PGM_FUNC(kputs)(const char * PGM_ATTR str)
{
	char c;

	/* Mask serial TX intr */
	kdbg_irqsave_t irqsave;
	KDBG_MASK_IRQ(irqsave);

	while ((c = PGM_READ_CHAR(str++)))
		__kputchar(c, 0);

	KDBG_RESTORE_IRQ(irqsave);
}


/*!
 * Cheap function to print small integers without using printf().
 */
int kputnum(int num)
{
	int output_len = 0;
	int divisor = 10000;
	int digit;

	do
	{
		digit = num / divisor;
		num %= divisor;

		if (digit || output_len || divisor == 1)
		{
			kputchar(digit + '0');
			++output_len;
		}
	}
	while (divisor /= 10);

	return output_len;
}


static void klocation(const char * PGM_ATTR file, int line)
{
	PGM_FUNC(kputs)(file);
	kputchar(':');
	kputnum(line);
	PGM_FUNC(kputs)(PGM_STR(": "));
}

int PGM_FUNC(__assert)(const char * PGM_ATTR cond, const char * PGM_ATTR file, int line)
{
	klocation(file, line);
	PGM_FUNC(kputs)(PGM_STR("Assertion failed: "));
	PGM_FUNC(kputs)(cond);
	kputchar('\n');
	BREAKPOINT;
	return 1;
}

/*
 * Unfortunately, there's no way to get __func__ in
 * program memory, so we waste quite a lot of RAM in
 * AVR and other Harvard processors.
 */
void PGM_FUNC(__trace)(const char *name)
{
	PGM_FUNC(kprintf)(PGM_STR("%s()\n"), name);
}

void PGM_FUNC(__tracemsg)(const char *name, const char * PGM_ATTR fmt, ...)
{
	va_list ap;

	PGM_FUNC(kprintf)(PGM_STR("%s(): "), name);
	va_start(ap, fmt);
	PGM_FUNC(kvprintf)(fmt, ap);
	va_end(ap);
	kputchar('\n');
}

int PGM_FUNC(__invalid_ptr)(void *value, const char * PGM_ATTR name, const char * PGM_ATTR file, int line)
{
	klocation(file, line);
	PGM_FUNC(kputs)(PGM_STR("Invalid ptr: "));
	PGM_FUNC(kputs)(name);
	#if CONFIG_PRINTF
		PGM_FUNC(kprintf)(PGM_STR(" = 0x%x\n"), (unsigned int)value);
	#else
		(void)value;
		kputchar('\n');
	#endif
	return 1;
}


void __init_wall(long *wall, int size)
{
	while(size--)
		*wall++ = WALL_VALUE;
}


int PGM_FUNC(__check_wall)(long *wall, int size, const char * PGM_ATTR name, const char * PGM_ATTR file, int line)
{
	int i, fail = 0;

	for (i = 0; i < size; i++)
	{
		if (wall[i] != WALL_VALUE)
		{
			klocation(file, line);
			PGM_FUNC(kputs)(PGM_STR("Wall broken: "));
			PGM_FUNC(kputs)(name);
			#if CONFIG_PRINTF
				PGM_FUNC(kprintf)(PGM_STR("[%d] (0x%p) = 0x%lx\n"), i, wall + i, wall[i]);
			#else
				kputchar('\n');
			#endif
			fail = 1;
		}
	}

	return fail;
}


#if CONFIG_PRINTF

/*!
 * Dump binary data in hex
 */
void kdump(const void *_buf, size_t len)
{
	const unsigned char *buf = (const unsigned char *)_buf;

	while (len--)
		kprintf("%02X", *buf++);
	kputchar('\n');
}

#endif /* CONFIG_PRINTF */

#endif /* _DEBUG */
