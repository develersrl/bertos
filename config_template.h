/*!
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
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

/*
 * $Log$
 * Revision 1.2  2004/08/05 18:46:52  bernie
 * Documentation improvements.
 *
 * Revision 1.1  2004/07/29 23:34:32  bernie
 * Add template configuration file.
 *
 */

#ifndef CONFIG_COMMON_H
#define CONFIG_COMMON_H

/*! Baud-rate for the kdebug console */
#define CONFIG_KDEBUG_BAUDRATE  19200

/*!
 * printf()-style formatter configuration.
 *
 * \sa PRINTF_DISABLED
 * \sa PRINTF_NOMODIFIERS
 * \sa PRINTF_REDUCED
 * \sa PRINTF_NOFLOAT
 * \sa PRINTF_FULL
 */
#define CONFIG_PRINTF PRINTF_FULL

/*!
 * Multithreading kernel
 *
 * /sa config_kernel.h
 */
#define CONFIG_KERNEL 0

/*!
 * \name Serial driver parameters
 * \{
 */
	/*! Size of the outbound FIFO buffer for all ports (bytes) */
	#define CONFIG_SER_TXBUFSIZE  32
	/*! Size of the inbound FIFO buffer for all ports (bytes) */
	#define CONFIG_SER_RXBUFSIZE  64

	/*! Default transmit timeout (ms). Set to -1 to disable timeout support */
	#define CONFIG_SER_TXTIMEOUT    -1
	/*! Default receive timeout (ms). Set to -1 to disable timeout support */
	#define CONFIG_SER_RXTIMEOUT    -1

	/*! Use RTS/CTS handshake */
	#define CONFIG_SER_HWHANDSHAKE   0

	/*! Default baud rate (set to 0 to disable) */
	#define CONFIG_SER_DEFBAUDRATE   0

	/*! Enable ser_gets() and ser_gets_echo() */
	#define CONFIG_SER_GETS          0

	/*!
	 * Transmit always something on serial port 0 TX
	 * to avoid interference when sending burst of data,
	 * using AVR multiprocessor serial mode
	 */
	#define CONFIG_SER_TXFILL        0
/*\}*/

/*!
 * \name KBus configuration
 * \{
 */
	/*! Board address for KBus */
	#define CONFIG_KBUS_ADDR KBUS_ADDR_FOOBAR

	/*! Disable KBUS escaping support */
	#define CONFIG_KBUS_ESCAPE  0

	/*! Serial port for internal KBUS communication */
	#define CONFIG_KBUS_PORT  0

	/*! Serial port speed for KBus communication */
	#define CONFIG_KBUS_BAUDRATE  19200
/*\}*/

//! EEPROM type for drv/eeprom.c
#define CONFIG_EEPROM_TYPE EEPROM_24XX256

#endif /* CONFIG_COMMON_H */
