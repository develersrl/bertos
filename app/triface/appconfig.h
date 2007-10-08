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
 * Copyright 2003, 2004, 2006 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief DevLib configuration options
 *
 * You should copy this header in your project and rename it to
 * "config.h" and delete the CONFIG_ macros for the modules
 * you're not using.
 *
 * <h2>Working with multiple applications</h2>
 *
 * If your project is made of multiple DevLib-based applications,
 * create a custom "config.h" file in each application subdirectory
 * and play with the compiler include path to get the desired result.
 * You can share common options by creationg a "config_common.h" header
 * and including it from all your "config.h" copies.
 *
 * <h2>Configuration style</h2>
 *
 * For improved compile-time checking of configuration options,
 * the preferred way to use a \c CONFIG_ symbol is keeping it
 * always defined with a value of either 0 or 1.  This lets
 * you write tests like this:
 *
 * \code
 *  #if CONFIG_FOO
 *  void foo(void)
 *  {
 *      if (CONFIG_BAR)
 *          bar();
 *  }
 *  #endif // CONFIG_FOO
 * \endcode
 *
 * In most cases, we rely on the optimizer to discard checks
 * on constant values and performing dead-code elimination.
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.5  2006/09/20 13:54:40  marco
 *#* Added new SPI definitions.
 *#*
 *#* Revision 1.4  2006/07/19 12:56:25  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.3  2006/06/12 21:37:01  marco
 *#* implemented some commands (ver and sleep)
 *#*
 *#* Revision 1.2  2006/06/01 12:29:21  marco
 *#* Add first simple protocol command (version request).
 *#*
 *#* Revision 1.1  2006/05/18 00:41:47  bernie
 *#* New triface devlib application.
 *#*
 *#*/

#ifndef APPCONFIG_TRIFACE_H
#define APPCONFIG_TRIFACE_H

/** Baud-rate for the kdebug console */
#define CONFIG_KDEBUG_BAUDRATE  38400

/** Serial port number for kdebug console */
#define CONFIG_KDEBUG_PORT  0


/**
 * printf()-style formatter configuration.
 *
 * \sa PRINTF_DISABLED
 * \sa PRINTF_NOMODIFIERS
 * \sa PRINTF_REDUCED
 * \sa PRINTF_NOFLOAT
 * \sa PRINTF_FULL
 */
#define CONFIG_PRINTF PRINTF_FULL

/**
 * Multithreading kernel.
 *
 * \sa config_kern.h
 */
#define CONFIG_KERNEL 0

/**
 * \name Serial driver parameters
 * \{
 */
	/** [bytes] Size of the outbound FIFO buffer for port 0. */
	#define CONFIG_UART0_TXBUFSIZE  32

	/** [bytes] Size of the inbound FIFO buffer for port 0. */
	#define CONFIG_UART0_RXBUFSIZE  64

	/** [bytes] Size of the outbound FIFO buffer for port 1. */
	#define CONFIG_UART1_TXBUFSIZE  32

	/** [bytes] Size of the inbound FIFO buffer for port 1. */
	#define CONFIG_UART1_RXBUFSIZE  64

	/** [bytes] Size of the outbound FIFO buffer for SPI port (AVR only). */
	#define CONFIG_SPI_TXBUFSIZE	16

	/** [bytes] Size of the inbound FIFO buffer for SPI port (AVR only). */
	#define CONFIG_SPI_RXBUFSIZE	32

	/** SPI data order (AVR only). */
	#define CONFIG_SPI_DATA_ORDER	SER_MSB_FIRST

	/** SPI clock division factor (AVR only). */
	#define CONFIG_SPI_CLOCK_DIV	16

	/** SPI clock polarity: 0 = normal low, 1 = normal high (AVR only). */
	#define CONFIG_SPI_CLOCK_POL	0

	/** SPI clock phase: 0 = sample on first edge, 1 = sample on second clock edge (AVR only). */
	#define CONFIG_SPI_CLOCK_PHASE	0

	/** Default transmit timeout (ms). Set to -1 to disable timeout support */
	#define CONFIG_SER_TXTIMEOUT    -1

	/** Default receive timeout (ms). Set to -1 to disable timeout support */
	#define CONFIG_SER_RXTIMEOUT    -1

	/** Use RTS/CTS handshake */
	#define CONFIG_SER_HWHANDSHAKE   0

	/** Default baud rate (set to 0 to disable) */
	#define CONFIG_SER_DEFBAUDRATE   0

	/** Enable ser_gets() and ser_gets_echo() */
	#define CONFIG_SER_GETS          1

	/** Enable second serial port in emulator. */
	#define CONFIG_EMUL_UART1        0

	/**
	 * Transmit always something on serial port 0 TX
	 * to avoid interference when sending burst of data,
	 * using AVR multiprocessor serial mode
	 */
	#define CONFIG_SER_TXFILL        0

	#define CONFIG_SER_STROBE        0
/*\}*/

/// Hardware timer selection for drv/timer.c
#define CONFIG_TIMER  TIMER_ON_OUTPUT_COMPARE0

/// Debug timer interrupt using a strobe pin.
#define CONFIG_TIMER_STROBE  0

/// Enable watchdog timer.
#define CONFIG_WATCHDOG  0

/// Enable internal parser commands.
#define CONFIG_INTERNAL_COMMANDS  0

#endif /* APPCONFIG_TRIFACE_H */
