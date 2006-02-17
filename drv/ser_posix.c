/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Serial port emulator for hosted environments.
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2006/02/17 22:28:00  bernie
 *#* Rename ser_emul.c to ser_posix.c.
 *#*
 *#* Revision 1.4  2006/02/17 22:23:06  bernie
 *#* Update POSIX serial emulator.
 *#*
 *#* Revision 1.3  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.2  2005/04/11 19:10:27  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.1  2004/12/31 17:40:00  bernie
 *#* Add a simple serial emulation driver.
 *#*
 *#*/

#include "ser.h"
#include "ser_p.h"

#include <cfg/debug.h>
#include <cfg/compiler.h>
#include <mware/fifobuf.h>

#include <appconfig.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> /* open() */
#include <unistd.h> /* read(), write() */


/* From the high-level serial driver */
extern struct Serial ser_handles[SER_CNT];

/* TX and RX buffers */
static unsigned char uart0_txbuffer[CONFIG_UART0_TXBUFSIZE];
static unsigned char uart0_rxbuffer[CONFIG_UART0_RXBUFSIZE];
static unsigned char uart1_txbuffer[CONFIG_UART1_TXBUFSIZE];
static unsigned char uart1_rxbuffer[CONFIG_UART1_RXBUFSIZE];


/*!
 * Internal state structure
 */
struct EmulSerial
{
	struct SerialHardware hw;
	struct Serial *ser;
	int fd;
};


/*
 * Callbacks
 */
static void uart_init(struct SerialHardware *_hw, struct Serial *ser)
{
	struct EmulSerial *hw = (struct EmulSerial *)_hw;

	hw->ser = ser;
	hw->fd = open("/dev/ttyS0", O_RDWR);
}

static void uart_cleanup(UNUSED_ARG(struct SerialHardware *, _hw))
{
	struct EmulSerial *hw = (struct EmulSerial *)_hw;

	close(hw->fd);
	hw->fd = -1;
}

static void uart_txStart(struct SerialHardware * _hw)
{
	struct EmulSerial *hw = (struct EmulSerial *)_hw;

	while(!fifo_isempty(&hw->ser->txfifo))
	{
		char c = fifo_pop(&hw->ser->txfifo);
		write(hw->fd, &c, 1);
	}
}

static bool uart_txSending(UNUSED_ARG(struct SerialHardware *, _hw))
{
	return false;
}


static void uart_setBaudrate(UNUSED_ARG(struct SerialHardware *, _hw), unsigned long rate)
{
	TRACEMSG("rate=%lu", rate);
	// TODO

}

static void uart_setParity(UNUSED_ARG(struct SerialHardware *, _hw), int parity)
{
	TRACEMSG("parity=%d", parity);
	// TODO
}

// FIXME: move into compiler.h?  Ditch?
#if COMPILER_C99
	#define	C99INIT(name,val) .name = val
#elif defined(__GNUC__)
	#define C99INIT(name,val) name: val
#else
	#warning No designated initializers, double check your code
	#define C99INIT(name,val) (val)
#endif

/*
 * High-level interface data structures.
 */
static const struct SerialHardwareVT uart_vtable =
{
	C99INIT(init, uart_init),
	C99INIT(cleanup, uart_cleanup),
	C99INIT(setBaudrate, uart_setBaudrate),
	C99INIT(setParity, uart_setParity),
	C99INIT(txStart, uart_txStart),
	C99INIT(txSending, uart_txSending),
};

static struct EmulSerial UARTDescs[SER_CNT] =
{
	{
		C99INIT(hw, /**/) {
			C99INIT(table, &uart_vtable),
			C99INIT(txbuffer, uart0_txbuffer),
			C99INIT(rxbuffer, uart0_rxbuffer),
			C99INIT(txbuffer_size, sizeof(uart0_txbuffer)),
			C99INIT(rxbuffer_size, sizeof(uart0_rxbuffer)),
		},
		C99INIT(ser, NULL),
		C99INIT(fd, -1),
	},
	{
		C99INIT(hw, /**/) {
			C99INIT(table, &uart_vtable),
			C99INIT(txbuffer, uart1_txbuffer),
			C99INIT(rxbuffer, uart1_rxbuffer),
			C99INIT(txbuffer_size, sizeof(uart1_txbuffer)),
			C99INIT(rxbuffer_size, sizeof(uart1_rxbuffer)),
		},
		C99INIT(ser, NULL),
		C99INIT(fd, -1),
	},
};

struct SerialHardware *ser_hw_getdesc(int unit)
{
	ASSERT(unit < SER_CNT);
	return &UARTDescs[unit].hw;
}
