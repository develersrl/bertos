/*!
 * \file
 * <!--
 * Copyright 2000 Bernardo Innocenti <bernie@codewiz.org>
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief High level serial I/O API
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*
 * $Log$
 * Revision 1.1  2004/05/23 18:10:11  bernie
 * Import drv/ modules.
 *
 * Revision 1.2  2004/04/21 17:38:24  rasky
 * New application
 *
 * Revision 1.16  2004/04/03 18:30:49  aleph
 * Move timeout defines in config, private define in .c
 *
 * Revision 1.15  2004/03/29 17:01:02  aleph
 * Add function to set serial parity, fix it when ser_open is used
 *
 * Revision 1.14  2004/03/29 16:19:33  aleph
 * Add ser_cleanup function; Various code improvements
 *
 * Revision 1.13  2004/03/24 15:22:27  aleph
 * Removed subdirs -I, fix header inclusion, move config.h in board_kf, kctrl
 *
 * Revision 1.12  2004/03/17 17:30:30  bernie
 * Add GCC format checks to printf()-like functions.
 *
 * Revision 1.11  2004/03/16 23:06:42  aleph
 * Doc fix
 *
 * Revision 1.10  2004/03/12 18:46:53  bernie
 * ser_read(): New function.
 *
 * Revision 1.9  2004/03/11 18:11:51  bernie
 * Cosmetic fixes
 */
#ifndef SER_H
#define SER_H

#include "compiler.h"
#include <mware/fifobuf.h>
#include "config.h"

/*!
 * \name Serial Error/status flags
 *
 * Some of these flags map directly to the flags
 * in AVR UART Status Register(USR).
 * \todo  flags of DSP56k aren't mapped to these flags. Luckily
 *        these flags doesn't collide with the DSP56k ones,
 *        which are from 0x0100 to 0x8000
 */
/*\{*/
#if defined(__AVR__)
	/* Software errors */
	#define SERRF_RXFIFOOVERRUN  BV(0)  /*!< Rx FIFO buffer overrun */
	#define SERRF_RXTIMEOUT      BV(5)  /*!< Receive timeout */
	#define SERRF_TXTIMEOUT      BV(6)  /*!< Transmit timeout */

	/* Hardware errors */
	#define SERRF_RXSROVERRUN    BV(3)  /*!< Rx shift register overrun */
	#define SERRF_FRAMEERROR     BV(4)  /*!< Stop bit missing */
	#define SERRF_PARITYERROR    BV(7)  /*!< Parity error */
#elif defined(__m56800__)
	/* Software errors */
	#define SERRF_RXFIFOOVERRUN  BV(0)  /*!< Rx FIFO buffer overrun */
	#define SERRF_RXTIMEOUT      BV(1)  /*!< Receive timeout */
	#define SERRF_TXTIMEOUT      BV(2)  /*!< Transmit timeout */

	/* Hardware errors */
	#define SERRF_PARITYERROR    BV(8)  /*!< Parity error */
	#define SERRF_FRAMEERROR     BV(9)  /*!< Stop bit missing */
	#define SERRF_NOISEERROR     BV(10) /*!< Noise error */
	#define SERRF_RXSROVERRUN    BV(11) /*!< Rx shift register overrun */
#else
	#error unknown architecture
#endif
/*\}*/

/*! \name Parity settings for ser_setparity() */
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
	#if defined(__AVR_ATmega64__)
	SER_UART0,
	SER_UART1,
	SER_SPI,

	#elif defined(__AVR_ATmega103__)
	SER_UART0,
	SER_SPI,
	
	#elif defined (__m56800__)
	SER_UART0,
	SER_UART1,	
	
	#else
		#error unknown architecture
	#endif

	SER_CNT       /**< Number of serial ports */
};
/* @} */


struct SerialHardware;

/*! Human-readable serial error descriptions */
extern const char * const serial_errors[8];

/*! Serial handle structure */
struct Serial
{
	/*! Physical port number */
	unsigned int unit;

	bool is_open;

	/*!
	 * \name FIFO transmit and receive buffers.
	 *
	 * Declared volatile because handled asinchronously by interrupts.
	 *
	 * \{
	 */
	volatile FIFOBuffer txfifo;
	volatile FIFOBuffer	rxfifo;
	unsigned char txbuffer[CONFIG_SER_TXBUFSIZE];
	unsigned char rxbuffer[CONFIG_SER_RXBUFSIZE];
	/* \} */

#ifdef CONFIG_SER_RXTIMEOUT
	time_t rxtimeout;
#endif
#ifdef CONFIG_SER_TXTIMEOUT
	time_t txtimeout;
#endif

	/*! Holds the flags defined above.  Will be 0 when no errors have occurred. */
	REGISTER uint16_t status;
	
	/*! Low-level interface to hardware. */
	struct SerialHardware* hw;
	
};


/* Function prototypes */
extern int ser_putchar(int c, struct Serial *port);
extern int ser_getchar(struct Serial *port);
extern int ser_getchar_nowait(struct Serial *port);
extern int ser_write(struct Serial *port, const void *buf, size_t len);
extern int ser_read(struct Serial *port, char *buf, size_t size);
extern int ser_print(struct Serial *port, const char *s);
extern int ser_gets(struct Serial *port, char *buf, int size);
extern int ser_gets_echo(struct Serial *port, char *buf, int size, bool echo);
extern int ser_printf(struct Serial *port, const char *format, ...) FORMAT(__printf__, 2, 3);
extern void ser_setbaudrate(struct Serial *port, unsigned long rate);
extern void ser_setparity(struct Serial *port, int parity);
extern void ser_purge(struct Serial *port);
extern struct Serial *ser_open(unsigned int unit);
extern void ser_close(struct Serial *port);
#if defined(CONFIG_SER_RXTIMEOUT) || defined(CONFIG_SER_TXTIMEOUT)
	extern void ser_settimeouts(struct Serial *port, time_t rxtimeout, time_t txtimeout);
#endif

/**
 * @name Additional functions implemented as macros
 *
 * @{
 */
#define ser_getstatus(h)    ((h)->status)
#define ser_setstatus(h, x) ((h)->status = (x))
/* @} */

#endif /* SER_H */
