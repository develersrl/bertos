/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000, 2001, 2002 Bernardo Innocenti <bernie@codewiz.org>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief General pourpose debug support for embedded systems (implementation).
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.16  2004/10/03 18:40:50  bernie
 *#* Use new CPU macros.
 *#*
 *#* Revision 1.15  2004/09/14 21:03:46  bernie
 *#* Use debug.h instead of kdebug.h.
 *#*
 *#* Revision 1.14  2004/09/06 21:39:56  bernie
 *#* Allow partial redefinition of BUS macros.
 *#*
 *#* Revision 1.13  2004/08/29 22:04:26  bernie
 *#* Convert 485 macros to generic BUS macros;
 *#* kputchar(): New public function.
 *#*
 *#* Revision 1.12  2004/08/25 14:12:08  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.11  2004/08/24 16:19:08  bernie
 *#* kputchar(): New public function; Add missing dummy inlines for \!_DEBUG.
 *#*
 *#* Revision 1.10  2004/08/04 15:57:50  rasky
 *#* Cambiata la putchar per kdebug per DSP56k: la nuova funzione e' quella piu' a basso livello (assembly)
 *#*
 *#* Revision 1.9  2004/08/02 20:20:29  aleph
 *#* Merge from project_ks
 *#*
 *#* Revision 1.8  2004/07/30 14:26:33  rasky
 *#* Semplificato l'output dell'ASSERT
 *#* Aggiunta ASSERT2 con stringa di help opzionalmente disattivabile
 *#*
 *#* Revision 1.7  2004/07/30 14:15:53  rasky
 *#* Nuovo supporto unificato per detect della CPU
 *#*
 *#* Revision 1.6  2004/07/18 21:49:28  bernie
 *#* Add ATmega8 support.
 *#*
 *#* Revision 1.5  2004/06/27 15:20:26  aleph
 *#* Change UNUSED() macro to accept two arguments: type and name;
 *#* Add macro GNUC_PREREQ to detect GCC version during build;
 *#* Some spacing cleanups and typo fix
 *#*
 *#* Revision 1.4  2004/06/06 18:09:51  bernie
 *#* Import DSP56800 changes; Print broken wall bricks in hex.
 *#*
 *#* Revision 1.3  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.2  2004/05/23 18:21:53  bernie
 *#* Trim CVS logs and cleanup header info.
 *#*
 *#*/

#include <debug.h>
#include <cpu.h>
#include "hw.h"
#include "config.h"

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
		 */
		#ifndef KDBG_UART0_BUS_INIT
		#define KDBG_UART0_BUS_INIT  do {} while (0)
		#endif
		#ifndef KDBG_UART0_BUS_RX
		#define KDBG_UART0_BUS_RX    do {} while (0)
		#endif
		#ifndef KDBG_UART0_BUS_TX
		#define KDBG_UART0_BUS_TX    do {} while (0)
		#endif

		#if CPU_AVR_ATMEGA64
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
		 */
		#ifndef KDBG_UART1_BUS_INIT
		#define KDBG_UART1_BUS_INIT  do {} while (0)
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

	#if CPU_AVR_ATMEGA64
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
static void __kputchar(char c, UNUSED(void *, unused))
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


void PGM_FUNC(kprintf)(const char * PGM_ATTR fmt, ...)
{
	va_list ap;

	/* Mask serial TX intr */
	kdbg_irqsave_t irqsave;
	KDBG_MASK_IRQ(irqsave);

	va_start(ap, fmt);
	PGM_FUNC(_formatted_write)(fmt, __kputchar, 0, ap);
	va_end(ap);

	/* Restore serial TX intr */
	KDBG_RESTORE_IRQ(irqsave);
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


int PGM_FUNC(__assert)(const char * PGM_ATTR cond, const char * PGM_ATTR file, int line)
{
	PGM_FUNC(kputs)(file);
	PGM_FUNC(kprintf)(PSTR(":%d: Assertion failed: "), line);
	PGM_FUNC(kputs)(cond);
	PGM_FUNC(kputs)(PSTR("\n"));
	return 1;
}


int PGM_FUNC(__invalid_ptr)(void *value, const char * PGM_ATTR name, const char * PGM_ATTR file, int line)
{
	PGM_FUNC(kputs)(file);
	PGM_FUNC(kprintf)(PSTR(":%d: Invalid pointer: "), line);
	PGM_FUNC(kputs)(name);
	PGM_FUNC(kprintf)(PSTR(" = 0x%x\n"), value);
	return 1;
}


void __init_wall(long *wall, int size)
{
	while(size--)
		*wall++ = WALL_VALUE;
}


int __check_wall(long *wall, int size, const char *name, const char *file, int line)
{
	int i, fail = 0;

	for (i = 0; i < size; i++)
	{
		if (wall[i] != WALL_VALUE)
		{
			kprintf("%s:%d: Wall broken: %s[%d] (0x%p) = 0x%lx\n",
				file, line, name, i, wall + i, wall[i]);
			fail = 1;
		}
	}

	return fail;
}


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

#endif /* _DEBUG */
