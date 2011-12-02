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
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Serial port emulator for hosted environments.
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * Updated by Robin Gilham to include reading from serial port and setting port speed <Robin@inventech.co.za>
 */

#include "cfg/cfg_ser.h"

#include <cfg/debug.h>
#include <cfg/compiler.h>

#include <drv/ser.h>
#include <drv/ser_p.h>
#include <drv/timer.h>
#include <cpu/power.h>

#include <struct/fifobuf.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h> /* open() */
#include <unistd.h> /* read(), write() */
#include <stdlib.h>
#include <termios.h>

static unsigned long BaudRate[] = {300,600,1200,1800,2400,4800,9600,19200,38400,57600,115200};
static unsigned long BaudSetting[] = {B300,B600,B1200,B1800,B2400,B4800,B9600,B19200,B38400,B57600,B115200};


/* From the high-level serial driver */
extern struct Serial ser_handles[SER_CNT];

/* TX and RX buffers */
static unsigned char uart0_txbuffer[CONFIG_UART0_TXBUFSIZE];
static unsigned char uart0_rxbuffer[CONFIG_UART0_RXBUFSIZE];
static unsigned char uart1_txbuffer[CONFIG_UART1_TXBUFSIZE];
static unsigned char uart1_rxbuffer[CONFIG_UART1_RXBUFSIZE];
static unsigned char uart2_txbuffer[CONFIG_UART2_TXBUFSIZE];
static unsigned char uart2_rxbuffer[CONFIG_UART2_RXBUFSIZE];


//Change these to map to the Serial port I use USB connected serial ports
static char* devFile[SER_CNT] = {
		"/dev/ttyS0",
		"/dev/ttyUSB0",
		"/dev/ttyUSB1"
};

//Make this big enough not criticul as it is running in emulated enviroment
#define SERIAL_RX_STACK_SIZE (KERN_MINSTACKSIZE*3)
PROC_DEFINE_STACK(serial_rx_stack0, SERIAL_RX_STACK_SIZE);
PROC_DEFINE_STACK(serial_rx_stack1, SERIAL_RX_STACK_SIZE);
PROC_DEFINE_STACK(serial_rx_stack2, SERIAL_RX_STACK_SIZE);

cpu_stack_t *serail_rx_stack[] = {serial_rx_stack0,serial_rx_stack1,serial_rx_stack2};


/**
 * Internal state structure
 */
struct EmulSerial
{
	struct SerialHardware hw;
	struct Serial *ser;
	int fd;
	Process *ser_rcv_proc;
};

static void poll_serial_rcv(void);

static struct termios oldtio,newtio;


/*
 * Callbacks
 */
static void uart_init(struct SerialHardware *_hw, struct Serial *ser)
{
	struct EmulSerial *hw = (struct EmulSerial *)_hw;
	TRACEMSG("uart_init %d\n",ser->unit);
	hw->ser = ser;
	hw->fd = open(devFile[ser->unit], O_RDWR | O_NOCTTY | O_NDELAY);
	ASSERT(hw->fd);
    /* Make the file descriptor asynchronous (the manual page says only
       O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
	fcntl(hw->fd, F_SETFL, FNDELAY);
	tcflush(hw->fd, TCIFLUSH);
	tcgetattr(hw->fd,&oldtio); /* save current port settings */
	hw->ser_rcv_proc = proc_new(poll_serial_rcv,hw,SERIAL_RX_STACK_SIZE,serail_rx_stack[ser->unit]);
}

static void uart_cleanup(UNUSED_ARG(struct SerialHardware *, _hw))
{
	struct EmulSerial *hw = (struct EmulSerial *)_hw;
	tcsetattr(hw->fd,TCSANOW,&oldtio);
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


static void uart_setBaudrate(struct SerialHardware * _hw, unsigned long rate)
{
	int i;
	struct EmulSerial *hw = (struct EmulSerial *)_hw;
	TRACEMSG("rate=%d", rate);
	for (i=0;i<sizeof(BaudRate)/sizeof(unsigned long);i++)
		if (BaudRate[i]==rate)
			break;
	if (i<sizeof(BaudRate)/sizeof(unsigned long))
	{
        bzero(&newtio, sizeof(newtio));
        newtio.c_cflag = BaudSetting[i] | CS8 | CLOCAL | CREAD;
        newtio.c_iflag = IGNPAR ;
        newtio.c_oflag  &= ~OPOST;


        /* set input mode (non-canonical, no echo,...) */
        newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

        tcsetattr(hw->fd,TCSANOW,&newtio);
	}
	else
	{
		TRACEMSG("invalid rate %d", rate);
		ASSERT(i<sizeof(BaudRate)/sizeof(unsigned long));
	}


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
	{
		C99INIT(hw, /**/) {
			C99INIT(table, &uart_vtable),
			C99INIT(txbuffer, uart2_txbuffer),
			C99INIT(rxbuffer, uart2_rxbuffer),
			C99INIT(txbuffer_size, sizeof(uart2_txbuffer)),
			C99INIT(rxbuffer_size, sizeof(uart2_rxbuffer)),
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
static void poll_serial_rcv(void)
{
	int res;
	int p;
	struct EmulSerial *hw = (struct EmulSerial *)proc_currentUserData();
	//TRACEMSG("poll_serial_rcv dev %d\n",hw->ser->unit);
	for(;;)
	{
		while(!fifo_isfull(&hw->ser->rxfifo))
		{

			res = read(hw->fd,&p,1);
			//TRACEMSG("rcv %c res %d\n",(unsigned char)p,res);
			if (res>0)
			{
				//TRACEMSG("rcv %02x ",p);
				//printf("rcv %02x ", (unsigned char)p);
				fifo_push_locked(&hw->ser->rxfifo, (unsigned char)p);
			}
			else if (res==0)
				//Delay for 2 ticks if no characters are read
				timer_delay(2);
			else
				//exit if there is and error i.e. the port closes
				goto exit_rcv;
		}
		cpu_relax();
	}
exit_rcv:
	return;
}
