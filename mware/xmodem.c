#error This module has not been revised for the API changes in several DevLib modules
/**
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 * \brief X-Modem serial transmission protocol (implementation)
 *
 * Suppots the CRC-16 and 1K-blocks variants of the standard.
 * \see ymodem.txt for the protocol description.
 *
 * \todo Decouple this code from the LCD, buzzer and timer drivers
 *       introducing user hooks or macros like CHECK_ABORT.
 *
 * \todo Break xmodem_send() and xmodem_recv() in smaller functions.
 *
 * \todo Add CONFIG_* vars to exclude either the receiver or the sender,
 *       to reduce the footprint for applications that don't need both.
 *
 * \todo Maybe convert drv/ser.c to the KFile interface for symmetry and
 *       flexibility.
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.10  2006/07/19 12:56:28  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.9  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.8  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.7  2004/08/15 06:30:06  bernie
 *#* Make the buffer a local variable, as documented.
 *#*
 *#* Revision 1.6  2004/08/15 05:31:46  bernie
 *#* Add an #error to spread some FUD about the quality of this module;
 *#* Add a few TODOs from Rasky's review;
 *#* Update to the new drv/ser.c API;
 *#* Move FlushSerial() to drv/ser.c and generalize.
 *#*
 *#* Revision 1.5  2004/08/12 23:46:21  bernie
 *#* Remove extra indentation level in switch statements.
 *#*
 *#* Revision 1.4  2004/08/12 23:35:50  bernie
 *#* Replace a handmade loop with memset().
 *#*
 *#* Revision 1.3  2004/08/12 23:34:36  bernie
 *#* Replace if/else with continue to reduce indentation level.
 *#*
 *#* Revision 1.2  2004/08/12 23:24:07  bernie
 *#* Rename UPDCRC() to UPDCRC16().
 *#*
 *#* Revision 1.1  2004/08/11 19:54:22  bernie
 *#* Import XModem protocol into DevLib.
 *#*
 *#*/

#include "xmodem.h"

#include <drv/ser.h>
#include <drv/lcd.h>
#include <drv/buzzer.h>
#include <mware/crc.h>
#include <mware/kfile.h>

#include <string.h> /* for memset() */


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
#define XM_BUFSIZE      1024  /**< Size of block buffer */


#if (ARCH & ARCH_BOOT)
	#include "kbdhw.h"
	#if (ARCH & ARCH_SLIM)
		#define CHECK_ABORT		KEYPRESSED_STOP
	#elif (ARCH & ARCH_SARF)
		#define CHECK_ABORT		KEYPRESSED_ESC
	#endif
#else
	#include "kbd.h"
	#if (ARCH & ARCH_SLIM)
		#define CHECK_ABORT		(kbd_getchar() == K_STOP)
	#elif (ARCH & ARCH_SARF)
		#define CHECK_ABORT		(kbd_getchar() == K_ESC)
	#endif
#endif /* ARCH_BOOT */


/**
 * Decode serial driver errors and print them on the display.
 */
static void print_serial_error(struct Serial *port, int retries)
{
	serstatus_t err, status;

	/* Get serial error code and reset it */
	status = ser_getstatus(port);
	ser_setstatus(port, 0);

	/* Mostra tutti gli errori in sequenza */
	for (err = 0; status != 0; status >>= 1, err++)
	{
		/* Se il bit dell'errore e' settato */
		if (status & 1)
		{
			lcd_printf(0, 3, LCD_FILL, "%s %d", serial_errors[err], retries);
			buz_beep(200);
			timer_delay(500);
		}
	}
}


/**
 * \brief Receive a file using the XModem protocol.
 *
 * \param port Serial port to use for transfer
 * \param fd Destination file
 *
 * \note This function allocates a large amount of stack (>1KB).
 */
bool xmodem_recv(struct Serial *port, KFile *fd)
{
	char block_buffer[XM_BUFSIZE]; /* Buffer to hold a block of data */
	int c, i, blocksize;
	int blocknr = 0, last_block_done = 0, retries = 0;
	char *buf;
	uint8_t checksum;
	uint16_t crc;
	bool purge = false;
	bool usecrc = true;


	lcd_printf(0, 2, LCD_FILL, "Starting Transfer...");
	lcd_clear();
	purge = true;
	ser_settimeouts(port, SER_DEFRXTIMEOUT, SER_DEFTXTIMEOUT);
	ser_setstatus(port, 0);

	/* Send initial NAK to start transmission */
	for(;;)
	{
		if (CHECK_ABORT)
		{
			ser_putchar(XM_CAN, port);
			ser_putchar(XM_CAN, port);
			lcd_printf(0, 2, LCD_FILL, "Transfer aborted");
			return false;
		}

		/*
		 * Discard incoming input until a timeout occurs, then send
		 * a NAK to the transmitter.
		 */
		if (purge)
		{
			purge = false;

			if (ser_getstatus())
				SerialError(retries);

			ser_resync(port, 200);
			retries++;

			if (retries >= XM_MAXRETRIES)
			{
				ser_putchar(XM_CAN, port);
				ser_putchar(XM_CAN, port);
				lcd_printf(0, 2, LCD_FILL, "Transfer aborted");
				return false;
			}

			/* Transmission start? */
			if (blocknr == 0)
			{
				if (retries < XM_MAXCRCRETRIES)
				{
					lcd_printf(0, 2, LCD_FILL, "Request Tx (CRC)");
					ser_putchar(XM_C, port);
				}
				else
				{
					/* Give up with CRC and fall back to checksum */
					usecrc = false;
					lcd_printf(0, 2, LCD_FILL, "Request Tx (BCC)");
					ser_putchar(XM_NAK, port);
				}
			}
			else
				ser_putchar(XM_NAK, port);
		}

		switch (ser_getchar(port))
		{
		case XM_STX:  /* Start of header (1024-byte block) */
			blocksize = 1024;
			goto getblock;

		case XM_SOH:  /* Start of header (128-byte block) */
			blocksize = 128;

		getblock:
			/* Get block number */
			c = ser_getchar(port);

			/* Check complemented block number */
			if ((~c & 0xff) != ser_getchar(port))
			{
				lcd_printf(0, 3, LCD_FILL, "Bad blk (%d)", c);
				purge = true;
				break;
			}

			/* Determine which block is being sent */
			if (c == (blocknr & 0xff))
				/* Last block repeated */
				lcd_printf(0, 2, LCD_FILL, "Repeat blk %d", blocknr);
			else if (c == ((blocknr + 1) & 0xff))
				/* Next block */
				lcd_printf(0, 2, LCD_FILL, "Recv blk %d", ++blocknr);
			else
			{
				/* Sync lost */
				lcd_printf(0, 3, LCD_FILL, "Sync lost (%d/%d)", c, blocknr);
				purge = true;
				break;
			}

			buf = block_buffer;	/* Reset pointer to start of buffer */
			checksum = 0;
			crc = 0;
			for (i = 0; i < blocksize; i++)
			{
				if ((c = ser_getchar(port)) == EOF)
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
			if ((c = ser_getchar(port)) == EOF)
			{
				purge = true;
				break;
			}

			if (usecrc)
			{
				crc = UPDCRC16(c, crc);

				/* Get CRC-16 LSB */
				if ((c = ser_getchar(port)) == EOF)
				{
					purge = true;
					break;
				}

				crc = UPDCRC16(c, crc);

				if (crc)
				{
					lcd_printf(0, 3, LCD_FILL, "Bad CRC: %04x", crc);
					purge = true;
					break;
				}
			}
			/* Compare the checksum */
			else if (c != checksum)
			{
				lcd_printf(0, 3, LCD_FILL, "Bad sum: %04x/%04x", checksum, c);
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
				if (fd->write(fd, block_buffer, blocksize))
				{
					/* Acknowledge block and clear error counter */
					ser_putchar(XM_ACK, port);
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
			ser_putchar(XM_ACK, port);
			lcd_printf(0, 2, LCD_FILL, "Transfer completed");
			return true;

		case EOF: /* Timeout or serial error */
			purge = true;
			break;

		default:
			lcd_printf(0, 3, LCD_FILL, "Skipping garbage");
			purge = true;
			break;
		}
	} /* End forever */
}


/**
 * \brief Transmit a file using the XModem protocol.
 *
 * \param port Serial port to use for transfer
 * \param fd Source file
 *
 * \note This function allocates a large amount of stack for
 *       the XModem transfer buffer (1KB).
 */
bool xmodem_send(struct Serial *port, KFile *fd)
{
	char block_buffer[XM_BUFSIZE]; /* Buffer to hold a block of data */
	size_t size = -1;
	int blocknr = 1, retries = 0, c, i;
	bool proceed, usecrc = false;
	uint16_t crc;
	uint8_t sum;


	ser_settimeouts(port, SER_DEFRXTIMEOUT, SER_DEFTXTIMEOUT);
	ser_setstatus(port, 0);
	ser_purge(port);
	lcd_printf(0, 2, LCD_FILL, "Wait remote host");

	for(;;)
	{
		proceed = false;
		do
		{
			if (CHECK_ABORT)
				return false;

			switch (c = ser_getchar(port))
			{
			case XM_NAK:
			case XM_C:
				if (blocknr == 1)
				{
					if (c == XM_C)
					{
						lcd_printf(0, 2, LCD_FILL, "Tx start (CRC)");
						usecrc = true;
					}
					else
						lcd_printf(0, 2, LCD_FILL, "Tx start (BCC)");

					/* Call user function to read in one block */
					size = fd->read(fd, block_buffer, XM_BUFSIZE);
				}
				else
					lcd_printf(0, 2, LCD_FILL, "Resend blk %d", blocknr);
				proceed = true;
				break;

			case XM_ACK:
				/* End of transfer? */
				if (!size)
					return true;

				/* Call user function to read in one block */
				size = fd->read(fd, block_buffer, XM_BUFSIZE);
				blocknr++;
				retries = 0;
				proceed = true;
				lcd_printf(0, 2, LCD_FILL, "Send blk %d", blocknr);
				break;

			case EOF:
				retries++;
				SerialError(retries);
				if (retries <= XM_MAXRETRIES)
					break;
				/* falling through! */

			case XM_CAN:
				lcd_printf(0, 2, LCD_FILL, "Transfer aborted");
				return false;

			default:
				lcd_printf(0, 3, LCD_FILL, "Skipping garbage");
				break;
			}
		}
		while (!proceed);

		if (!size)
		{
			ser_putchar(XM_EOT, port);
			continue;
		}

		/* Pad block with 0xFF if it's partially full */
		memset(block_buffer + size, 0xFF, XM_BUFSIZE - size);

		/* Send block header (STX, blocknr, ~blocknr) */
		ser_putchar(XM_STX, port);
		ser_putchar(blocknr & 0xFF, port);
		ser_putchar(~blocknr & 0xFF, port);

		/* Send block and compute its CRC/checksum */
		sum = 0;
		crc = 0;
		for (i = 0; i < XM_BUFSIZE; i++)
		{
			ser_putchar(block_buffer[i], port);
			crc = UPDCRC16(block_buffer[i], crc);
			sum += block_buffer[i];
		}

		/* Send CRC/Checksum */
		if (usecrc)
		{
			crc = UPDCRC16(0, crc);
			crc = UPDCRC16(0, crc);
			ser_putchar(crc >> 8, port);
			ser_putchar(crc & 0xFF, port);
		}
		else
			ser_putchar(sum, port);
	}
}
