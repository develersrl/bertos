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
 * Copyright 2003, 2004, 2005, 2006, 2007 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000, 2001, 2002 Bernie Innocenti <bernie@codewiz.org>
 * -->
 *
 * \brief General pourpose debug support for embedded systems (implementation).
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

#include "cfg/cfg_debug.h"
#include "cfg/cfg_arch.h"
#include "cfg/cfg_netlog.h"

#define LOG_LEVEL  NETLOG_LOG_LEVEL
#define LOG_FORMAT NETLOG_LOG_FORMAT
#include <cfg/log.h>

#include <cfg/macros.h> /* for BV() */
#include <cfg/debug.h>
#include <cfg/os.h>

#include <cpu/attr.h>
#include <cpu/types.h>
#include <cpu/power.h>
#include <cpu/irq.h>
#include <cpu/pgm.h>

#include <mware/formatwr.h> /* for _formatted_write() */

#include <drv/timer.h>

#ifdef _DEBUG

#if CPU_HARVARD && !defined(_PROGMEM)
	#error This module build correctly only in program memory!
#endif


#if OS_HOSTED
	#include <unistd.h> // write()

	#define KDBG_WAIT_READY()      do { /*nop*/ } while(0)
	#define KDBG_WRITE_CHAR(c)     do { char __c = (c); write(STDERR_FILENO, &__c, sizeof(__c)); } while(0)
	#define KDBG_MASK_IRQ(old)     do { (void)(old); } while(0)
	#define KDBG_RESTORE_IRQ(old)  do { /*nop*/ } while(0)
	typedef char kdbg_irqsave_t; /* unused */

	#define	kdbg_hw_init() do {} while (0) ///< Not needed

	#if CONFIG_KDEBUG_PORT == 666
		#error BITBANG debug console missing for this platform
	#endif
#else
	#include CPU_CSOURCE(kdebug)
#endif

void kdbg_init(void)
{
	/* Init debug hw */
	kdbg_hw_init();
	kputs("\n\n*** BeRTOS DBG START ***\n");
}

volatile bool lwip_kdebug = false;

void PGM_FUNC(kvprintf)(const char * PGM_ATTR fmt, va_list ap);

/**
 * Output one character to the debug console
 */
static void __raw_putchar(char c, UNUSED_ARG(void *, unused))
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
#if !CONFIG_KERN_LOGGER && !(ARCH & ARCH_BOOT)
	if (!IRQ_RUNNING() && !lwip_kdebug)
		cpu_relax();
#endif
}

#include <kern/signal.h>

#if CONFIG_KERN && CONFIG_KERN_SIGNALS && CONFIG_KERN_LOGGER
#include <cfg/module.h>
#include <cpu/power.h>
#include <kern/proc.h>
#include <struct/fifobuf.h>

#define KLOGGER_PRIO		INT_MIN
#define KLOGGER_STACK_SIZE	KERN_MINSTACKSIZE

#if CONFIG_KERN_HEAP
#define klogger_stack	NULL
#else /* !CONFIG_KERN_HEAP */
PROC_DEFINE_STACK(klogger_stack, KLOGGER_STACK_SIZE);
#endif

static unsigned char log_buffer[CONFIG_KERN_LOGGER_BUFSIZE];
static DECLARE_FIFO(log_ring, log_buffer, sizeof(log_buffer));

static Process *klogger_proc;
static bool klogger_should_stop;


static void klogger(void)
{
	proc_setPri(proc_current(), KLOGGER_PRIO);

	while (!klogger_should_stop)
	{
		static uint8_t ch;

		sig_wait(SIG_SINGLE);

		while (!fifo_isempty(&log_ring))
		{
			ch = fifo_pop(&log_ring);
			__raw_putchar(ch, 0);
			cpu_relax();
		}
	}
	klogger_proc = NULL;
}

static void __kputchar(char c, UNUSED_ARG(void *, unused))
{
	/* Be sure circular buffer writers are serialized */
	ATOMIC(
		if (!fifo_isfull(&log_ring))
			fifo_push(&log_ring, c);
	);
	if (klogger_proc)
		sig_post(klogger_proc, SIG_SINGLE);
}

bool klogger_init(void)
{
	MOD_CHECK(proc);

	kprintf("**** KLOGGER INIT ****\n");

	klogger_should_stop = false;
	klogger_proc = proc_new(klogger, NULL, KLOGGER_STACK_SIZE, klogger_stack);
	/*
	 * Wake-up the klogger immediately to flush all the messages from the
	 * ring buffer.
	 */
	if (LIKELY(klogger_proc))
		sig_post(klogger_proc, SIG_SINGLE);

	return klogger_proc != NULL;
}

void klogger_exit(void)
{
	klogger_should_stop = true;
	while (klogger_proc)
		cpu_relax();
	fifo_flush(&log_ring);
}

void kputchar(char c)
{
	__kputchar(c, 0);
}

void PGM_FUNC(kvprintf)(const char * PGM_ATTR fmt, va_list ap)
{
#if CONFIG_PRINTF
	PROC_ATOMIC(PGM_FUNC(_formatted_write)(fmt, __kputchar, 0, ap));
#else
	/* A better than nothing printf() surrogate. */
	PROC_ATOMIC(PGM_FUNC(kputs)(fmt));
#endif /* CONFIG_PRINTF */
}

#else /* !CONFIG_KERN_LOGGER */

static void __kputchar(char c, UNUSED_ARG(void *, unused))
{
	__raw_putchar(c, 0);
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


void PGM_FUNC(kvprintf)(const char * PGM_ATTR fmt, va_list ap)
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

#endif /* CONFIG_KERN_LOGGER */

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

int PGM_FUNC(__bassert)(const char * PGM_ATTR cond, const char * PGM_ATTR file, int line)
{
	#if !CONFIG_LOG_NET
		klocation(file, line);
		PGM_FUNC(kputs)(PGM_STR("Assertion failed: "));
		PGM_FUNC(kputs)(cond);
		kputchar('\n');
	#else
		LOG_ERR("%s:%d: Assertion failed: %s\n", file, line, cond);
		if (!IRQ_RUNNING())
			timer_delay(500);
	#endif
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
		PGM_FUNC(kprintf)(PGM_STR(" = 0x%p\n"), value);
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

	kprintf("Dumping buffer at addr [%p], %zu bytes", buf, len);
	size_t i=0;
	while (len--)
	{
		if ((i++ % 16) == 0)
			kputs("\n");
		kprintf("%02X ", *buf++);
	}
	kputchar('\n');
}

#endif /* CONFIG_PRINTF */

#endif /* _DEBUG */
