/*!
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief Low-level timer module for Qt emulator (implementation).
 */

/*#*
 *#* $Log$
 *#* Revision 1.4  2006/05/28 12:17:57  bernie
 *#* Drop almost all the Qt3 cruft.
 *#*
 *#* Revision 1.3  2006/02/21 21:28:02  bernie
 *#* New time handling based on TIMER_TICKS_PER_SEC to support slow timers with ticks longer than 1ms.
 *#*
 *#* Revision 1.2  2006/02/20 02:01:35  bernie
 *#* Port to Qt 4.1.
 *#*
 *#* Revision 1.1  2005/11/27 03:06:36  bernie
 *#* Qt timer emulation.
 *#*
 *#*/

#include <cfg/compiler.h> /* hptime.t */

// Qt headers
#include <QtCore/QDateTime>
#include <QtCore/QTimer>


// The user interrupt server routine
void timer_isr(void);


/**
 * Singleton class for Qt-based hardware timer emulation.
 */
class EmulTimer : public QObject
{
private:
	Q_OBJECT;

	/// System timer (counts ms since application startup)
	QTime system_time;

	/// The 1ms "hardware" tick counter.
	QTimer timer;

	/**
	 * We deliberately don't use RAII because the real hardware
	 * we're simulating needs to be initialized manually.
	 */
	bool initialized;

	/// Private ctor (singleton)
	EmulTimer() : initialized(false) { }

public:
	/// Return singleton instance
	static EmulTimer &instance()
	{
		static EmulTimer et;
		return et;
	}

	/// Start timer emulator.
	void init()
	{
		// Timer initialized twice?
		ASSERT(!initialized);

		// Record initial time
		system_time.start();

		// Activate timer interrupt
		timer.connect(&timer, SIGNAL(timeout()), this, SLOT(timerInterrupt()));
		timer.start(1000 / TIMER_TICKS_PER_SEC);

		initialized = true;
	}

	/// Return current time in high-precision format.
	hptime_t hpread()
	{
		ASSERT(initialized);
		return system_time.elapsed();
	}

public slots:
	void timerInterrupt(void)
	{
		// Just call user interrupt server, timer restarts automatically.
		timer_isr();
	}

};

#include "timer_qt_moc.cpp"


/// HW dependent timer initialization.
extern "C" static void timer_hw_init(void)
{
	// Kick EmulTimer initialization
	EmulTimer::instance().init();
}

extern "C" INLINE hptime_t timer_hw_hpread(void)
{
	return EmulTimer::instance().hpread();
}

