/*!
 * \file
 * <!--
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000,2001,2002 Bernardo Innocenti <bernie@codewiz.org>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief General pourpose debug functions.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

/*
 * $Log$
 * Revision 1.6  2004/07/18 21:49:28  bernie
 * Add ATmega8 support.
 *
 * Revision 1.5  2004/06/27 15:20:26  aleph
 * Change UNUSED() macro to accept two arguments: type and name;
 * Add macro GNUC_PREREQ to detect GCC version during build;
 * Some spacing cleanups and typo fix
 *
 * Revision 1.4  2004/06/06 18:09:51  bernie
 * Import DSP56800 changes; Print broken wall bricks in hex.
 *
 * Revision 1.3  2004/06/03 11:27:09  bernie
 * Add dual-license information.
 *
 * Revision 1.2  2004/05/23 18:21:53  bernie
 * Trim CVS logs and cleanup header info.
 *
 */

#include "kdebug.h"
#include "hw.h"
#include "config.h"

#include <mware/formatwr.h> /* for _formatted_write() */

#ifdef _DEBUG

#if defined(_EMUL)
	#include <stdio.h>
	#define KDBG_WAIT_READY()      do {/*nop*/} while(0)
	#define KDBG_WRITE_CHAR(c)     putchar((c))
	#define KDBG_MASK_IRQ(old)     do {/*nop*/} while(0)
	#define KDBG_RESTORE_IRQ()     do {/*nop*/} while(0)
#elif defined(__I196__)
	#include "Util196.h"
	#define KDBG_WAIT_READY()      do {} while (!(SP_STAT & (SPSF_TX_EMPTY | SPSF_TX_INT)))
	#define KDBG_WRITE_CHAR(c)     do { SBUF = (c); } while(0)
	#define KDBG_MASK_IRQ(old) \
		do { \
			(old) = INT_MASK1 & INT1F_TI; \
			INT_MASK1 &= ~INT1F_TI; \
		} while(0)
	#define KDBG_RESTORE_IRQ(old)  do { INT_MASK1 |= (old); }
#elif defined(__AVR__)
	#include <avr/io.h>
	#if CONFIG_KDEBUG_PORT == 0
		#if defined(__AVR_ATmega64__)
			#define UCR UCSR0B
			#define UDR UDR0
			#define USR UCSR0A
		#elif defined(__AVR_ATmega8__)
			#define UCR UCSRB
			#define USR UCSRA
		#endif
		#define KDBG_WAIT_READY()     do { loop_until_bit_is_set(USR, UDRE); } while(0)
		#define KDBG_WRITE_CHAR(c)    do { UCR |= BV(TXEN); UDR = (c); } while(0)
		#define KDBG_MASK_IRQ(old)    do { (old) = UCR & BV(TXCIE); cbi(UCR, TXCIE); } while(0)
		#define KDBG_RESTORE_IRQ(old) do { UCR |= (old); } while(0)
	#elif CONFIG_KDEBUG_PORT == 1
		#define KDBG_WAIT_READY()     do { loop_until_bit_is_set(UCSR1A, UDRE); } while(0)
		#define KDBG_WRITE_CHAR(c)    do { UCSR1B |= BV(TXEN); UDR1 = (c); } while(0)
		#define KDBG_MASK_IRQ(old)    do { (old) = UCSR1B & BV(TXCIE); cbi(UCSR1B, TXCIE); } while(0)
		#define KDBG_RESTORE_IRQ(old) do { UCSR1B |= (old); } while(0)
	#else
		#error CONFIG_KDEBUG_PORT should be either 0 or 1
	#endif
#elif defined(__MWERKS__) && (defined(__m56800E__) || defined(__m56800__))
	/* Debugging go through the JTAG interface. The MSL library already
	   implements the console I/O correctly. */
	#include <stdio.h>
	#define KDBG_WAIT_READY()
	#define KDBG_WRITE_CHAR(c)        do { char ch=c; fwrite(&ch,1,1,stdout); } while (0)
	#define KDBG_MASK_IRQ(old)
	#define KDBG_RESTORE_IRQ(old)
#else
	#error Unknown architecture
#endif


void kdbg_init(void)
{
#if defined(__I196__)

	/* Set serial port for 19200bps 8N1 */
	INT_MASK1 &= ~(INT1F_TI | INT1F_RI);
	SP_CON = SPCF_RECEIVE_ENABLE | SPCF_MODE1;
	ioc1_img |= IOC1F_TXD_SEL | IOC1F_EXTINT_SRC;
	IOC1 = ioc1_img;
	BAUD_RATE = 0x33;
	BAUD_RATE = 0x80;

#elif defined(__AVR__)

	/* Compute the baud rate */
	uint16_t period = (((CLOCK_FREQ / 16UL) + (CONFIG_KDEBUG_BAUDRATE / 2)) / CONFIG_KDEBUG_BAUDRATE) - 1;

	#if defined(__AVR_ATmega64__)
		#if CONFIG_KDEBUG_PORT == 0
			UBRR0H = (uint8_t)(period>>8);
			UBRR0L = (uint8_t)period;
		#elif CONFIG_KDEBUG_PORT == 1
			UBRR1H = (uint8_t)(period>>8);
			UBRR1L = (uint8_t)period;
		#else
			#error CONFIG_KDEBUG_PORT must be either 0 or 1
		#endif
	#elif defined(__AVR_ATmega8__)
		UBRRH = (uint8_t)(period>>8);
		UBRRL = (uint8_t)period;
	#elif defined(__AVR_ATmega103__)
		UBRR = (uint8_t)period;
	#else
		#error Unknown arch
	#endif

#endif /* !__I196__ && !__AVR__ */

	kputs("\n\n*** DBG START ***\n");
}


/*!
 * Output one character to the debug console
 */
static void kputchar(char c, UNUSED(void *, unused))
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


void PGM_FUNC(kprintf)(const char * PGM_ATTR fmt, ...)
{
	va_list ap;

	/* Mask serial TX intr */
	unsigned char irqsave;
	KDBG_MASK_IRQ(irqsave);

	va_start(ap, fmt);
	PGM_FUNC(_formatted_write)(fmt, kputchar, 0, ap);
	va_end(ap);

	/* Restore serial TX intr */
	KDBG_RESTORE_IRQ(irqsave);
}


void PGM_FUNC(kputs)(const char * PGM_ATTR str)
{
	char c;

	/* Mask serial TX intr */
	unsigned char irqsave;
	KDBG_MASK_IRQ(irqsave);

	while ((c = PGM_READ_CHAR(str++)))
		kputchar(c, 0);

	KDBG_RESTORE_IRQ(irqsave);
}


int PGM_FUNC(__assert)(const char * PGM_ATTR cond, const char *file, int line)
{
	PGM_FUNC(kputs)(file);
	PGM_FUNC(kprintf)(PSTR(":%d: Assertion failed: \""), line);
	PGM_FUNC(kputs)(cond);
	PGM_FUNC(kputs)(PSTR("\"\n"));
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
	kputs("\n");
}

#endif /* _DEBUG */
