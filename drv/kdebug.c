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
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000, 2001, 2002 Bernardo Innocenti <bernie@codewiz.org>
 *
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
 *#* Revision 1.31  2007/06/07 14:25:30  batt
 *#* Merge from project_ks
 *#*
 *#* Revision 1.30  2007/01/28 09:18:06  batt
 *#* Merge from project_ks.
 *#*
 *#* Revision 1.29  2006/11/23 13:19:39  batt
 *#* Add BitBanged serial debug console.
 *#*
 *#* Revision 1.28  2006/07/19 12:56:25  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.27  2006/06/01 12:32:06  marco
 *#* Updated include reference.
 *#*
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
#include <cpu/cpu.h>
#include <cfg/macros.h> /* for BV() */
#include <appconfig.h>
#include <hw_cpu.h>     /* for CLOCK_FREQ */
#include <hw_ser.h>     /* Required for bus macros overrides */

#include <mware/formatwr.h> /* for _formatted_write() */

#ifdef _DEBUG

#if CPU_HARVARD && !defined(_PROGMEM)
	#error This module build correctly only in program memory!
#endif

#if defined(_EMUL)
	#include <stdio.h>
	#define KDBG_WAIT_READY()      do { /*nop*/ } while(0)
	#define KDBG_WRITE_CHAR(c)     putchar((c))
	#define KDBG_MASK_IRQ(old)     do { (void)(old); } while(0)
	#define KDBG_RESTORE_IRQ(old)  do { /*nop*/ } while(0)
	typedef char kdbg_irqsave_t; /* unused */

	#if CONFIG_KDEBUG_PORT == 666
		#error BITBANG debug console missing for this platform
	#endif
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

	#if CONFIG_KDEBUG_PORT == 666
		#error BITBANG debug console missing for this platform
	#endif
#elif CPU_AVR
	#include <avr/io.h>

	#if CONFIG_KDEBUG_PORT == 0

		/*
		 * Support for special bus policies or external transceivers
		 * on UART0 (to be overridden in "hw_ser.h").
		 *
		 * HACK: if we don't set TXEN, kdbg disables the transmitter
		 * after each output statement until the serial driver
		 * is initialized.  These glitches confuse the debug
		 * terminal that ends up printing some trash.
		 */
		#ifndef KDBG_UART0_BUS_INIT
		#define KDBG_UART0_BUS_INIT  do { \
				UCSR0B = BV(TXEN0); \
			} while (0)
		#endif
		#ifndef KDBG_UART0_BUS_RX
		#define KDBG_UART0_BUS_RX    do {} while (0)
		#endif
		#ifndef KDBG_UART0_BUS_TX
		#define KDBG_UART0_BUS_TX    do {} while (0)
		#endif

		#if CPU_AVR_ATMEGA64 || CPU_AVR_ATMEGA128 || CPU_AVR_ATMEGA168
			#define UCR UCSR0B
			#define UDR UDR0
			#define USR UCSR0A
		#elif CPU_AVR_ATMEGA8
			#define UCR UCSRB
			#define USR UCSRA
		#else
			#error Unknown CPU
		#endif

		#define KDBG_WAIT_READY()     do { loop_until_bit_is_set(USR, UDRE0); } while(0)
		#define KDBG_WAIT_TXDONE()    do { loop_until_bit_is_set(USR, TXC0); } while(0)

		/*
		 * We must clear the TXC flag before sending a new character to allow
		 * KDBG_WAIT_TXDONE() to work properly.
		 *
		 * BUG: if KDBG_WRITE_CHAR() is called after the TXC flag is set by hardware,
		 * a new TXC could be generated after we've cleared it and before the new
		 * character is written to UDR.  On a 485 bus, the transceiver will be put
		 * in RX mode while still transmitting the last char.
		 */
		#define KDBG_WRITE_CHAR(c)    do { USR |= BV(TXC0); UDR = (c); } while(0)

		#define KDBG_MASK_IRQ(old)    do { \
			(old) = UCR; \
			UCR |= BV(TXEN0); \
			UCR &= ~(BV(TXCIE0) | BV(UDRIE0)); \
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
		 * on UART1 (to be overridden in "hw_ser.h").
		 *
		 * HACK: if we don't set TXEN, kdbg disables the transmitter
		 * after each output statement until the serial driver
		 * is initialized.  These glitches confuse the debug
		 * terminal that ends up printing some trash.
		 */
		#ifndef KDBG_UART1_BUS_INIT
		#define KDBG_UART1_BUS_INIT  do { \
				UCSR1B = BV(TXEN1); \
			} while (0)
		#endif
		#ifndef KDBG_UART1_BUS_RX
		#define KDBG_UART1_BUS_RX    do {} while (0)
		#endif
		#ifndef KDBG_UART1_BUS_TX
		#define KDBG_UART1_BUS_TX    do {} while (0)
		#endif

		#define KDBG_WAIT_READY()     do { loop_until_bit_is_set(UCSR1A, UDRE1); } while(0)
		#define KDBG_WAIT_TXDONE()    do { loop_until_bit_is_set(UCSR1A, TXC1); } while(0)
		#define KDBG_WRITE_CHAR(c)    do { UCSR1A |= BV(TXC1); UDR1 = (c); } while(0)

		#define KDBG_MASK_IRQ(old)    do { \
			(old) = UCSR1B; \
			UCSR1B |= BV(TXEN1); \
			UCSR1B &= ~(BV(TXCIE1) | BV(UDRIE1)); \
			KDBG_UART1_BUS_TX; \
		} while(0)

		#define KDBG_RESTORE_IRQ(old) do { \
			KDBG_WAIT_TXDONE(); \
			KDBG_UART1_BUS_RX; \
			UCSR1B = (old); \
		} while(0)

		typedef uint8_t kdbg_irqsave_t;

	/*
	 * Special debug port for BitBanged Serial see below for details...
 	 */
	#elif CONFIG_KDEBUG_PORT == 666
		#include "hw_ser.h"
		#define KDBG_WAIT_READY()      do { /*nop*/ } while(0)
		#define KDBG_WRITE_CHAR(c)     _kdebug_bitbang_putchar((c))
		#define KDBG_MASK_IRQ(old)     do { IRQ_SAVE_DISABLE((old)); } while(0)
		#define KDBG_RESTORE_IRQ(old)  do { IRQ_RESTORE((old)); } while(0)
		typedef cpuflags_t kdbg_irqsave_t;

		#define KDBG_DELAY (((CLOCK_FREQ + CONFIG_KDEBUG_BAUDRATE / 2) / CONFIG_KDEBUG_BAUDRATE) + 7) / 14

		static void _kdebug_bitbang_delay(void)
		{
			unsigned long i;

			for (i = 0; i < KDBG_DELAY; i++)
			{
				NOP;
				NOP;
				NOP;
				NOP;
				NOP;
			}
		}
	#else
		#error CONFIG_KDEBUG_PORT should be either 0, 1 or 666
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
	#if CONFIG_KDEBUG_PORT == 666
		#error BITBANG debug console missing for this platform
	#endif
#else
	#error Unknown architecture
#endif

#if CONFIG_KDEBUG_PORT == 666
	/**
	 * Putchar for BITBANG serial debug console.
	 * Sometimes, we can't permit to use a whole serial for debugging purpose.
	 * Since debug console is in output only it is usefull to use a single generic I/O pin for debug.
	 * This is achieved by this simple function, that shift out the data like a UART, but
	 * in software :)
	 * The only requirement is that SER_BITBANG_* macros will be defined somewhere (usually hw_ser.h)
	 * \note All interrupts are disabled during debug prints!
	 */
	static void _kdebug_bitbang_putchar(char c)
	{
		int i;
		uint16_t data = c;

		/* Add stop bit */
		data |= 0x0100;

		/* Add start bit*/
		data <<= 1;

		/* Shift out data */
		uint16_t shift = 1;
		for (i = 0; i < 10; i++)
		{
			if (data & shift)
				SER_BITBANG_HIGH;
			else
				SER_BITBANG_LOW;
			_kdebug_bitbang_delay();
			shift <<= 1;
		}
	}
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
	#if CONFIG_KDEBUG_PORT == 666
		SER_BITBANG_INIT;
	#else /* CONFIG_KDEBUG_PORT != 666 */
		/* Compute the baud rate */
		uint16_t period = (((CLOCK_FREQ / 16UL) + (CONFIG_KDEBUG_BAUDRATE / 2)) / CONFIG_KDEBUG_BAUDRATE) - 1;

		#if (CPU_AVR_ATMEGA64 || CPU_AVR_ATMEGA128)
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

		#elif CPU_AVR_ATMEGA168
			UBRR0H = (uint8_t)(period>>8);
			UBRR0L = (uint8_t)period;
			KDBG_UART0_BUS_INIT;
		#elif CPU_AVR_ATMEGA8
			UBRRH = (uint8_t)(period>>8);
			UBRRL = (uint8_t)period;
		#elif CPU_AVR_ATMEGA103
			UBRR = (uint8_t)period;
			KDBG_UART0_BUS_INIT;
		#else
			#error Unknown CPU
		#endif
	#endif /* CONFIG_KDEBUG_PORT == 666 */

#endif /* !CPU_I196 && !CPU_AVR */

	kputs("\n\n*** DBG START ***\n");
}


/**
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


/**
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

/**
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
