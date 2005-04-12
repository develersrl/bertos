/*!
 * \file
 * <!--
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2000 Bernardo Innocenti <bernie@codewiz.org>
 * -->
 *
 * \brief Simple serial I/O driver
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2005/04/12 01:37:50  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.5  2004/10/20 13:37:49  batt
 *#* Change testing of simple serial instead of ARCH_BOOT in sc driver.
 *#*
 *#* Revision 1.4  2004/10/15 12:22:04  batt
 *#* Readd ';' in setstatus macro.
 *#*
 *#* Revision 1.3  2004/10/15 12:13:57  batt
 *#* Correct \brief header.
 *#*
 *#* Revision 1.2  2004/10/15 11:54:21  batt
 *#* Reformat.
 *#*
 *#* Revision 1.1  2004/10/13 16:35:36  batt
 *#* New (simple) serial driver.
 *#*
 *#*
 */
#ifndef SER_SIMPLE_H
#define SER_SIMPLE_H

/* For checking which serial driver is linked */
#define SER_SIMPLE

#include <config.h>
#include <compiler.h>


#if 0
#if CPU_AVR
	typedef uint8_t serstatus_t;

	/* Software errors */
	#define SERRF_RXFIFOOVERRUN  BV(0)  /*!< Rx FIFO buffer overrun */
	#define SERRF_RXTIMEOUT      BV(5)  /*!< Receive timeout */
	#define SERRF_TXTIMEOUT      BV(6)  /*!< Transmit timeout */

	/* Hardware errors */
	#define SERRF_RXSROVERRUN    BV(3)  /*!< Rx shift register overrun */
	#define SERRF_FRAMEERROR     BV(4)  /*!< Stop bit missing */
	#define SERRF_PARITYERROR    BV(7)  /*!< Parity error */
#else
	#error unknown architecture
#endif
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
#else
	#error unknown architecture
#endif
	SER_CNT  /*!< Number of serial ports */
};
/*\}*/
#endif

/*! \name Parity settings for ser_setparity() */
/*\{*/
#define SER_PARITY_NONE  0
#define SER_PARITY_EVEN  2
#define SER_PARITY_ODD   3
/*\}*/


/*! Serial handle structure */
struct Serial;

/* Function prototypes */
extern int _ser_putchar(int c);
extern int _ser_getchar(void);
extern int _ser_getchar_nowait(void);
/*
extern int ser_write(struct Serial *port, const void *buf, size_t len);
extern int ser_read(struct Serial *port, void *buf, size_t size);

extern int ser_printf(struct Serial *port, const char *format, ...) FORMAT(__printf__, 2, 3);

extern int ser_gets(struct Serial *port, char *buf, int size);
extern int ser_gets_echo(struct Serial *port, char *buf, int size, bool echo);
*/
extern int _ser_print(const char *s);

extern void _ser_setbaudrate(unsigned long rate);
extern void _ser_setparity(int parity);
extern void _ser_settimeouts(void);
extern void _ser_setstatus(void);
/*
extern void ser_resync(struct Serial *port, time_t delay);
extern void ser_drain(struct Serial *port);
*/
extern void _ser_purge(void);
extern struct Serial *_ser_open(void);
extern void _ser_close(void);

/*!
 * \name Functions implemented as macros
 *
 * \{
 */
#define ser_putchar(c, port)        _ser_putchar(c)
#define ser_getchar(port)           _ser_getchar()
#define ser_getchar_nowait(port)    _ser_getchar_nowait()
#define ser_print(port, s)          _ser_print(s)
#define ser_setbaudrate(port, y)    _ser_setbaudrate(y)
#define ser_setparity(port, par)    _ser_setparity(par)
#define ser_settimeouts(port, y, z) _ser_settimeouts()
#define ser_purge(port)             _ser_purge()
#define ser_open(port)              _ser_open()
#define ser_getstatus(h)            0
#define ser_setstatus(h, x)         do {(void)(x);} while(0)
/* \} */

#endif /* SER_SIMPLE_H */
