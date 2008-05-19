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
 * Copyright 2004, 2005, 2006, 2007 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2001 Bernardo Innocenti <bernie@develer.com>
 *
 * -->
 *
 * \brief X-Modem serial transmission protocol (implementation)
 *
 * Supports the CRC-16 and 1K-blocks variants of the standard.
 * \see ymodem.txt for the protocol description.
 *
 * \todo Break xmodem_send() and xmodem_recv() in smaller functions.
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 */


#include "xmodem.h"

#include <appconfig.h>
#include <string.h> /* for memset() */
#include <drv/ser.h>
#include <algo/crc.h>
#include <cfg/debug.h>




/**
 * \name Protocol control codes
 * \{
 */
#define XM_SOH  0x01  /**< Start Of Header (128-byte block) */
#define XM_STX  0x02  /**< Start Of Header (1024-byte block) */
#define XM_EOT  0x04  /**< End Of Transmission */
#define XM_ACK  0x06  /**< Acknowledge block */
#define XM_NAK  0x15  /**< Negative Acknowledge */
#define XM_C    0x43  /**< Request CRC-16 transmission */
#define XM_CAN  0x18  /**< CANcel transmission */
/*\}*/

#define XM_MAXRETRIES     15  /**< Max retries before giving up */
#define XM_MAXCRCRETRIES   7  /**< Max retries before switching to BCC */

#if CONFIG_XMODEM_1KCRC == 1
	#define XM_BUFSIZE       1024  /**< 1024 bytes of block buffer */
#else
	#define XM_BUFSIZE       128   /**< 128 bytes of block buffer */
#endif


#if CONFIG_XMODEM_RECV
/**
 * \brief Receive a file using the XModem protocol.
 *
 * \param port Serial port to use for transfer
 * \param fd Destination file
 *
 * \note This function allocates a large amount of stack (\see XM_BUFSIZE).
 */
bool xmodem_recv(struct KFileSerial *port, KFile *fd)
{
	char block_buffer[XM_BUFSIZE]; /* Buffer to hold a block of data */
	int c, i, blocksize;
	int blocknr = 0, last_block_done = 0, retries = 0;
	char *buf;
	uint8_t checksum;
	uint16_t crc;
	bool purge = false;
	bool usecrc = true;


	XMODEM_PROGRESS("Starting Transfer...\n");
	purge = true;
	kfile_clearerr(&port->fd);

	/* Send initial NAK to start transmission */
	for(;;)
	{
		if (XMODEM_CHECK_ABORT)
		{
			kfile_putc(XM_CAN, &port->fd);
			kfile_putc(XM_CAN, &port->fd);
			XMODEM_PROGRESS("Transfer aborted\n");
			return false;
		}

		/*
		 * Discard incoming input until a timeout occurs, then send
		 * a NAK to the transmitter.
		 */
		if (purge)
		{
			purge = false;

			if (kfile_error(&port->fd))
				XMODEM_PROGRESS("Retries %d\n", retries);

			ser_resync(port, 200);
			retries++;

			if (retries >= XM_MAXRETRIES)
			{
				kfile_putc(XM_CAN, &port->fd);
				kfile_putc(XM_CAN, &port->fd);
				XMODEM_PROGRESS("Transfer aborted\n");
				return false;
			}

			/* Transmission start? */
			if (blocknr == 0)
			{
				if (retries < XM_MAXCRCRETRIES)
				{
					XMODEM_PROGRESS("Request Tx (CRC)\n");
					kfile_putc(XM_C, &port->fd);
				}
				else
				{
					/* Give up with CRC and fall back to checksum */
					usecrc = false;
					XMODEM_PROGRESS("Request Tx (BCC)\n");
					kfile_putc(XM_NAK, &port->fd);
				}
			}
			else
				kfile_putc(XM_NAK, &port->fd);
		}

		switch (kfile_getc(&port->fd))
		{
		#if XM_BUFSIZE >= 1024
		case XM_STX:  /* Start of header (1024-byte block) */
			blocksize = 1024;
			goto getblock;
		#endif

		case XM_SOH:  /* Start of header (128-byte block) */
			blocksize = 128;
			/* Needed to avoid warning if XM_BUFSIZE < 1024 */

		getblock:
			/* Get block number */
			c = kfile_getc(&port->fd);

			/* Check complemented block number */
			if ((~c & 0xff) != kfile_getc(&port->fd))
			{
				XMODEM_PROGRESS("Bad blk (%d)\n", c);
				purge = true;
				break;
			}

			/* Determine which block is being sent */
			if (c == (blocknr & 0xff))
				/* Last block repeated */
				XMODEM_PROGRESS("Repeat blk %d\n", blocknr);
			else if (c == ((blocknr + 1) & 0xff))
				/* Next block */
				XMODEM_PROGRESS("Recv blk %d\n", ++blocknr);
			else
			{
				/* Sync lost */
				XMODEM_PROGRESS("Sync lost (%d/%d)\n", c, blocknr);
				purge = true;
				break;
			}

			buf = block_buffer;	/* Reset pointer to start of buffer */
			checksum = 0;
			crc = 0;
			for (i = 0; i < blocksize; i++)
			{
				if ((c = kfile_getc(&port->fd)) == EOF)
				{
					purge = true;
					break;
				}

				/* Store in buffer */
				*buf++ = (char)c;

				/* Calculate block checksum or CRC */
				if (usecrc)
					crc = UPDCRC16(c, crc);
				else
					checksum += (char)c;
			}

			if (purge)
				break;

			/* Get the checksum byte or the CRC-16 MSB */
			if ((c = kfile_getc(&port->fd)) == EOF)
			{
				purge = true;
				break;
			}

			if (usecrc)
			{
				crc = UPDCRC16(c, crc);

				/* Get CRC-16 LSB */
				if ((c = kfile_getc(&port->fd)) == EOF)
				{
					purge = true;
					break;
				}

				crc = UPDCRC16(c, crc);

				if (crc)
				{
					XMODEM_PROGRESS("Bad CRC: %04x\n", crc);
					purge = true;
					break;
				}
			}
			/* Compare the checksum */
			else if (c != checksum)
			{
				XMODEM_PROGRESS("Bad sum: %04x/%04x\n", checksum, c);
				purge = true;
				break;
			}

			/*
			 * Avoid flushing the same block twice.
			 * This could happen when the sender does not receive our
			 * acknowledge and resends the same block.
			 */
			if (last_block_done < blocknr)
			{
				/* Call user function to flush the buffer */
				if (kfile_write(fd, block_buffer, blocksize))
				{
					/* Acknowledge block and clear error counter */
					kfile_putc(XM_ACK, &port->fd);
					retries = 0;
					last_block_done = blocknr;
				}
				else
				{
					/* User callback failed: abort transfer immediately */
					retries = XM_MAXRETRIES;
					purge = true;
				}
			}
			break;

		case XM_EOT:	/* End of transmission */
			kfile_putc(XM_ACK, &port->fd);
			XMODEM_PROGRESS("Transfer completed\n");
			return true;

		case EOF: /* Timeout or serial error */
			purge = true;
			break;

		default:
			XMODEM_PROGRESS("Skipping garbage\n");
			purge = true;
			break;
		}
	} /* End forever */
}
#endif


#if CONFIG_XMODEM_SEND
/**
 * \brief Transmit some data using the XModem protocol.
 *
 * \param port Serial port to use for transfer
 * \param fd Source file
 *
 * \note This function allocates a large amount of stack for
 *       the XModem transfer buffer (\see XM_BUFSIZE).
 */
bool xmodem_send(struct KFileSerial *port, KFile *fd)
{
	char block_buffer[XM_BUFSIZE]; /* Buffer to hold a block of data */
	size_t size = -1;
	int blocknr = 1, retries = 0, c, i;
	bool proceed, usecrc = false;
	uint16_t crc;
	uint8_t sum;

	/*
	 * Reading a block can be very slow, so we read the first block early
	 * to avoid receiving double XM_C char.
	 * This could happen if we check for XM_C and then read the block, giving
	 * the receiving device time to send another XM_C char misinterpretating
	 * the blocks sent.
	 */
	size = kfile_read(fd, block_buffer, XM_BUFSIZE);

	kfile_clearerr(&port->fd);
	ser_purge(port);
	XMODEM_PROGRESS("Wait remote host\n");

	for(;;)
	{
		proceed = false;
		do
		{
			if (XMODEM_CHECK_ABORT)
				return false;

			switch (c = kfile_getc(&port->fd))
			{
			case XM_NAK:
				XMODEM_PROGRESS("Resend blk %d\n", blocknr);
				proceed = true;
				break;

			case XM_C:
				if (c == XM_C)
				{
					XMODEM_PROGRESS("Tx start (CRC)\n");
					usecrc = true;
				}
				else
					XMODEM_PROGRESS("Tx start (BCC)\n");

				proceed = true;
				break;

			case XM_ACK:
				/* End of transfer? */
				if (!size)
					return true;

				/* Call user function to read in one block */
				size = kfile_read(fd, block_buffer, XM_BUFSIZE);
				XMODEM_PROGRESS("Send blk %d\n", blocknr);
				blocknr++;
				retries = 0;
				proceed = true;
				break;

			case EOF:
				kfile_clearerr(&port->fd);
				retries++;
				XMODEM_PROGRESS("Retries %d\n", retries);
				if (retries <= XM_MAXRETRIES)
					break;
				/* falling through! */

			case XM_CAN:
				XMODEM_PROGRESS("Transfer aborted\n");
				return false;

			default:
				XMODEM_PROGRESS("Skipping garbage\n");
				break;
			}
		}
		while (!proceed);

		if (!size)
		{
			kfile_putc(XM_EOT, &port->fd);
			continue;
		}

		/* Pad block with 0xFF if it's partially full */
		memset(block_buffer + size, 0xFF, XM_BUFSIZE - size);

		/* Send block header (STX, blocknr, ~blocknr) */
		#if XM_BUFSIZE == 128
			kfile_putc(XM_SOH, &port->fd);
		#else
			kfile_putc(XM_STX, &port->fd);
		#endif
		kfile_putc(blocknr & 0xFF, &port->fd);
		kfile_putc(~blocknr & 0xFF, &port->fd);

		/* Send block and compute its CRC/checksum */
		sum = 0;
		crc = 0;
		for (i = 0; i < XM_BUFSIZE; i++)
		{
			kfile_putc(block_buffer[i], &port->fd);
			crc = UPDCRC16(block_buffer[i], crc);
			sum += block_buffer[i];
		}

		/* Send CRC/Checksum */
		if (usecrc)
		{
			crc = UPDCRC16(0, crc);
			crc = UPDCRC16(0, crc);
			kfile_putc(crc >> 8, &port->fd);
			kfile_putc(crc & 0xFF, &port->fd);
		}
		else
			kfile_putc(sum, &port->fd);
	}
}
#endif
