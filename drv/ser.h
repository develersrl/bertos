/*!
 * \file
 * <!--
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti <bernie@codewiz.org>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief High level serial I/O API
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.16  2004/10/19 11:48:05  bernie
 *#* Reformat.
 *#*
 *#* Revision 1.15  2004/10/19 08:11:53  bernie
 *#* SERRF_TX, SERRF_RX: New macros; Enhance documentation.
 *#*
 *#* Revision 1.14  2004/10/03 18:43:18  bernie
 *#* Fix a nasty bug caused by confusion between old-style and new-style configuration macros.
 *#*
 *#* Revision 1.13  2004/09/14 21:04:57  bernie
 *#* Don't vanely call kdebug.h.
 *#*
 *#* Revision 1.12  2004/09/06 21:40:50  bernie
 *#* Move buffer handling in chip-specific driver.
 *#*
 *#* Revision 1.11  2004/08/25 14:12:08  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.10  2004/08/24 16:20:48  bernie
 *#* ser_read(): Make buffer argument void *#* for consistency with ANSI C and ser_write()
 *#*
 *#* Revision 1.9  2004/08/15 05:32:22  bernie
 *#* ser_resync(): New function.
 *#*
 *#* Revision 1.8  2004/08/02 20:20:29  aleph
 *#* Merge from project_ks
 *#*
 *#* Revision 1.7  2004/07/30 14:15:53  rasky
 *#* Nuovo supporto unificato per detect della CPU
 *#*
 *#* Revision 1.6  2004/07/29 22:57:09  bernie
 *#* ser_drain(): New function; Make Serial::is_open a debug-only feature; Switch to new-style CONFIG_* macros.
 *#*
 *#* Revision 1.5  2004/07/18 21:54:23  bernie
 *#* Add ATmega8 support.
 *#*
 *#* Revision 1.4  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.3  2004/06/02 21:35:24  aleph
 *#* Serial enhancements: interruptible receive handler and 8 bit serial status for AVR; remove volatile attribute to FIFOBuffer, useless for new fifobuf routens
 *#*
 *#* Revision 1.2  2004/05/23 18:21:53  bernie
 *#* Trim CVS logs and cleanup header info.
 *#*
 *#*/
#ifndef DRV_SER_H
#define DRV_SER_H

#include <mware/fifobuf.h>
#include <compiler.h>
#include <config.h>

/*! \name Serial Error/status flags. */
/*\{*/
#if CPU_AVR
	typedef uint8_t serstatus_t;

	/* Software errors */
	#define SERRF_RXFIFOOVERRUN  BV(0)  /*!< Rx FIFO buffer overrun */
	#define SERRF_RXTIMEOUT      BV(5)  /*!< Receive timeout */
	#define SERRF_TXTIMEOUT      BV(6)  /*!< Transmit timeout */

	/*
	 * Hardware errors.
         * These flags map directly to the AVR UART Status Register (USR).
	 */
	#define SERRF_RXSROVERRUN    BV(3)  /*!< Rx shift register overrun */
	#define SERRF_FRAMEERROR     BV(4)  /*!< Stop bit missing */
	#define SERRF_PARITYERROR    BV(7)  /*!< Parity error */
	#define SERRF_NOISEERROR     0      /*!< Unsupported */
#elif CPU_DSP56K
	typedef uint16_t serstatus_t;

	/* Software errors */
	#define SERRF_RXFIFOOVERRUN  BV(0)  /*!< Rx FIFO buffer overrun */
	#define SERRF_RXTIMEOUT      BV(1)  /*!< Receive timeout */
	#define SERRF_TXTIMEOUT      BV(2)  /*!< Transmit timeout */

	/*
	 * Hardware errors.
	 * These flags map directly to the SCI Control Register.
	 */
	#define SERRF_PARITYERROR    BV(8)  /*!< Parity error */
	#define SERRF_FRAMEERROR     BV(9)  /*!< Stop bit missing */
	#define SERRF_NOISEERROR     BV(10) /*!< Noise error */
	#define SERRF_RXSROVERRUN    BV(11) /*!< Rx shift register overrun */
#else
	#error unknown architecture
#endif
/*\}*/

/*! \name Masks to group TX/RX errors. */
/*\{*/
#define SERRF_RX  (SERRF_RXFIFOOVERRUN \
	| SERRF_RXTIMEOUT \
	| SERRF_RXSROVERRUN \
	| SERRF_PARITYERROR \
	| SERRF_FRAMEERROR \
	| SERRF_NOISEERROR)
#define SERRF_TX  (SERRF_TXTIMEOUT)
/*\}*/


/*!
 * \name Parity settings for ser_setparity().
 *
 * \note Values are AVR-specific for performance reasons.
 *       Other processors should either decode them or
 *       redefine these macros.
 */
/*\{*/
#define SER_PARITY_NONE  0
#define SER_PARITY_EVEN  2
#define SER_PARITY_ODD   3
/*\}*/

/*!
 * \name Serial hw numbers
 *
 * \{
 */
enum
{
#if CPU_AVR_ATMEGA64 || CPU_AVR_ATMEGA128
	SER_UART0,
	SER_UART1,
	SER_SPI,
#elif CPU_AVR_ATMEGA103 || CPU_AVR_ATMEGA8
	SER_UART0,
	SER_SPI,
#elif CPU_DSP56K
	SER_UART0,
	SER_UART1,
#else
	#error unknown architecture
#endif
	SER_CNT  /*!< Number of serial ports */
};
/*\}*/


struct SerialHardware;

/*! Human-readable serial error descriptions */
extern const char * const serial_errors[8];

/*! Serial handle structure */
struct Serial
{
	/*! Physical port number */
	unsigned int unit;

#ifdef _DEBUG
	bool is_open;
#endif

	/*!
	 * \name Transmit and receive FIFOs.
	 *
	 * Declared volatile because handled asinchronously by interrupts.
	 *
	 * \{
	 */
	FIFOBuffer txfifo;
	FIFOBuffer rxfifo;
	/* \} */

#if CONFIG_SER_RXTIMEOUT != -1
	time_t rxtimeout;
#endif
#if CONFIG_SER_TXTIMEOUT != -1
	time_t txtimeout;
#endif

	/*! Holds the flags defined above.  Will be 0 when no errors have occurred. */
	serstatus_t status;

	/*! Low-level interface to hardware. */
	struct SerialHardware* hw;
};


/* Function prototypes */
extern int ser_putchar(int c, struct Serial *port);
extern int ser_getchar(struct Serial *port);
extern int ser_getchar_nowait(struct Serial *port);

extern int ser_write(struct Serial *port, const void *buf, size_t len);
extern int ser_read(struct Serial *port, void *buf, size_t size);

extern int ser_print(struct Serial *port, const char *s);
extern int ser_printf(struct Serial *port, const char *format, ...) FORMAT(__printf__, 2, 3);

extern int ser_gets(struct Serial *port, char *buf, int size);
extern int ser_gets_echo(struct Serial *port, char *buf, int size, bool echo);

extern void ser_setbaudrate(struct Serial *port, unsigned long rate);
extern void ser_setparity(struct Serial *port, int parity);
extern void ser_settimeouts(struct Serial *port, time_t rxtimeout, time_t txtimeout);
extern void ser_resync(struct Serial *port, time_t delay);
extern void ser_purge(struct Serial *port);
extern void ser_drain(struct Serial *port);

extern struct Serial *ser_open(unsigned int unit);
extern void ser_close(struct Serial *port);

/*!
 * \name Additional functions implemented as macros
 *
 * \{
 */
#define ser_getstatus(h)    ((h)->status)
#define ser_setstatus(h, x) ((h)->status = (x))
/* \} */

#endif /* DRV_SER_H */
