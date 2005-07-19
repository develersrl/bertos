/*!
 * \file
 * <!--
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Hardware independent timer driver (interface)
 */

/*#*
 *#* $Log$
 *#* Revision 1.25  2005/07/19 07:26:37  bernie
 *#* Refactor to decouple timer ticks from milliseconds.
 *#*
 *#* Revision 1.24  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.23  2005/03/01 23:25:46  bernie
 *#* Move event.h to mware/.
 *#*
 *#* Revision 1.22  2004/12/13 12:07:06  bernie
 *#* DISABLE_IRQSAVE/ENABLE_IRQRESTORE: Convert to IRQ_SAVE_DISABLE/IRQ_RESTORE.
 *#*
 *#* Revision 1.21  2004/12/09 08:35:21  bernie
 *#* Replace IPTR with iptr_t.
 *#*
 *#* Revision 1.20  2004/12/08 08:56:41  bernie
 *#* Rename sigset_t to sigmask_t; Reformat.
 *#*
 *#* Revision 1.19  2004/12/08 08:30:37  bernie
 *#* Convert to mtime_t; timer_minutes(): Remove.
 *#*
 *#* Revision 1.18  2004/11/16 23:09:52  bernie
 *#* Disable timer_minutes() for targets with 16bit time_t.
 *#*
 *#* Revision 1.17  2004/11/16 22:37:14  bernie
 *#* Replace IPTR with iptr_t.
 *#*
 *#* Revision 1.16  2004/08/25 14:12:08  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.15  2004/08/10 06:59:09  bernie
 *#* timer_gettick(): Rename to timer_ticks() and add backwards compatibility inline.
 *#*
 *#* Revision 1.12  2004/07/30 14:34:10  rasky
 *#* Vari fix per documentazione e commenti
 *#* Aggiunte PP_CATn e STATIC_ASSERT
 *#*
 *#* Revision 1.11  2004/07/29 22:40:12  bernie
 *#* Spelling fix.
 *#*
 *#* Revision 1.10  2004/07/21 00:13:57  bernie
 *#* Put timer driver on diet.
 *#*
 *#* Revision 1.9  2004/07/20 23:45:01  bernie
 *#* Finally remove redundant protos.
 *#*
 *#* Revision 1.8  2004/07/18 21:57:32  bernie
 *#* timer_gettick(): Rename to timer_tick() and document better.
 *#*
 *#* Revision 1.7  2004/06/27 15:26:17  aleph
 *#* Declaration fix for build with GCC 3.4
 *#*
 *#* Revision 1.6  2004/06/07 18:10:06  aleph
 *#* Remove free pool of timers; use user-provided Timer structure instead
 *#*
 *#* Revision 1.5  2004/06/07 15:57:12  aleph
 *#* Add function prototypes
 *#*
 *#* Revision 1.4  2004/06/06 18:25:44  bernie
 *#* Rename event macros to look like regular functions.
 *#*
 *#* Revision 1.3  2004/06/06 16:57:18  bernie
 *#* Mark some functions INLINE instead of 'extern inline'.
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 18:23:30  bernie
 *#* Import drv/timer module.
 *#*
 *#*/
#ifndef DRV_TIMER_H
#define DRV_TIMER_H

#include <cfg/debug.h>

#include CPU_HEADER(timer)
#include <mware/list.h>
#include <cfg/cpu.h>
#include <cfg/compiler.h>
#include <appconfig.h>

/*! Number of timer ticks per second. */
#define TIMER_TICKS_PER_SEC  (TIMER_TICKS_PER_MSEC * 1000)

/*! Number of ticks per microsecond */
#define TIMER_TICKS_PER_USEC ((TIMER_TICKS_PER_MSEC + 500) / 1000)


extern volatile ticks_t _clock;

/*!
 * \brief Return the system tick counter (expressed in ticks)
 *
 * The result is guaranteed to increment monotonically,
 * but client code must be tolerant with respect to overflows.
 *
 * The following code is safe:
 *
 * \code
 *   ticks_t tea_start_time = timer_clock();
 *
 *   boil_water();
 *
 *   if (timer_clock() - tea_start_time > TEAPOT_DELAY)
 *       printf("Your tea, Sir.\n");
 * \endcode
 *
 * \note This function must disable interrupts on 8/16bit CPUs because the
 * clock variable is larger than the processor word size and can't
 * be copied atomically.
 */
INLINE ticks_t timer_clock(void)
{
	ticks_t result;

	ATOMIC(result = _clock);

	return result;
}

/*!
 * Faster version of timer_clock(), to be called only when the timer
 * interrupt is disabled (DISABLE_INTS) or overridden by a
 * higher-priority or non-nesting interrupt.
 *
 * \sa timer_ticks
 */
INLINE ticks_t timer_clock_unlocked(void)
{
	return _clock;
}



//TODO: take care of slow timers so add convertions for seconds to ticks and viceversa.

/*! Convert \a ms [ms] to ticks */
INLINE ticks_t ms_to_ticks(mtime_t ms)
{
	return ms * TIMER_TICKS_PER_MSEC;
}

/*! Convert \a us [us] to ticks */
INLINE ticks_t us_to_ticks(utime_t us)
{
#if TIMER_TICKS_PER_MSEC < 10000
	return (us * TIMER_TICKS_PER_MSEC + 500) / 1000;
#else
	return (us * TIMER_TICKS_PER_USEC);
#endif
}

/*! Convert \a ticks [ticks] to ms */
INLINE mtime_t ticks_to_ms(ticks_t ticks)
{
	return (ticks + TIMER_TICKS_PER_MSEC / 2) / TIMER_TICKS_PER_MSEC;
}

/*! Convert \a ticks [ticks] to us */
INLINE utime_t ticks_to_us(ticks_t ticks)
{
#if TIMER_TICKS_PER_USEC > 10
	return (ticks / TIMER_TICKS_PER_USEC);
#else
	return (ticks * 1000 + TIMER_TICKS_PER_MSEC / 2) / TIMER_TICKS_PER_MSEC;
#endif
}

/*! Convert \a us [us] to hpticks */
INLINE hptime_t us_to_hptime(utime_t us)
{
	#if TIMER_HW_HPTICKS_PER_SEC > 10000000UL
		return(us * ((TIMER_HW_HPTICKS_PER_SEC + 500000UL) / 1000000UL));
	#else
		return((us * TIMER_HW_HPTICKS_PER_SEC + 500000UL) / 1000000UL));
	#endif /* TIMER_HW_HPTICKS_PER_SEC > 10000000UL */
}

/*! Convert \a hpticks [hptime] to usec */
INLINE utime_t hptime_to_us(hptime_t hpticks)
{
	#if TIMER_HW_HPTICKS_PER_SEC < 100000UL
		return(hpticks * (1000000UL / TIMER_HW_HPTICKS_PER_SEC));
	#else
		return((hpticks * 1000000UL) / TIMER_HW_HPTICKS_PER_SEC);
	#endif /* TIMER_HW_HPTICKS_PER_SEC < 100000UL */
}


void timer_init(void);
void timer_delayTicks(ticks_t delay);
INLINE void timer_delay(mtime_t delay)
{
	timer_delayTicks(ms_to_ticks(delay));
}

#if !defined(CONFIG_TIMER_DISABLE_UDELAY)
void timer_busyWait(hptime_t delay);
void timer_delayHp(hptime_t delay);
INLINE void timer_udelay(utime_t delay)
{
	timer_delayHp(us_to_hptime(delay));
}
#endif

#if CONFIG_TEST
void timer_test(void);
#endif /* CONFIG_TEST */

#ifndef CONFIG_TIMER_DISABLE_EVENTS

#include <mware/event.h>

/*!
 * The timer driver supports multiple synchronous timers
 * that can trigger an event when they expire.
 *
 * \sa timer_add()
 * \sa timer_abort()
 */
typedef struct Timer
{
	Node    link;     /*!< Link into timers queue */
	ticks_t _delay;   /*!< Timer delay in ms */
	ticks_t tick;     /*!< Timer will expire at this tick */
	Event   expire;   /*!< Event to execute when the timer expires */
	DB(uint16_t magic;)
} Timer;

/*! Timer is active when Timer.magic contains this value (for debugging purposes). */
#define TIMER_MAGIC_ACTIVE    0xABBA
#define TIMER_MAGIC_INACTIVE  0xBAAB

extern void timer_add(Timer *timer);
extern Timer *timer_abort(Timer *timer);

/*! Set the timer so that it calls an user hook when it expires */
INLINE void timer_set_event_softint(Timer *timer, Hook func, iptr_t user_data)
{
	event_initSoftInt(&timer->expire, func, user_data);
}

/*! Set the timer delay (the time before the event will be triggered) */
INLINE void timer_setDelay(Timer *timer, ticks_t delay)
{
	timer->_delay = delay;
}

#endif /* CONFIG_TIMER_DISABLE_EVENTS */

#if defined(CONFIG_KERN_SIGNALS) && CONFIG_KERN_SIGNALS

/*! Set the timer so that it sends a signal when it expires */
INLINE void timer_set_event_signal(Timer *timer, struct Process *proc, sigmask_t sigs)
{
	event_initSignal(&timer->expire, proc, sigs);
}

#endif /* CONFIG_KERN_SIGNALS */


#endif /* DRV_TIMER_H */
