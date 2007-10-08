/**
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief TC520 ADC driver (implementation)
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 * \author Marco Benelli <marco@develer.com>
 */

#include <drv/tc520.h>
#include <drv/timer.h>

#include <hw_tc520.h>

#include <cfg/macros.h>
#include <cfg/cpu.h>
#include <cfg/compiler.h>

#include <drv/ser.h>

static Serial *spi_ser;

#define TC520_CONVERSION_TIMEOUT ms_to_ticks(1000)
#define INIT_LOAD_VALUE 0x00

/**
 * Start an AD conversion and return result.
 * To start a conversion first we must pull down CE pin.
 * The ADC starts a convertion and keeps the DV pin high until the end.
 * At this point, we can read the conversion value by SPI.
 * The convertion result is yield in 3 bytes.
 * First byte:
 * bit | Value
 * ----|-------
 *  7  | Overrange
 *  6  | Polarity
 * 5:0 | data bits 15:10
 *
 * Second byte: data 9:2
 *
 * Third byte:
 * bit | Value
 * ----|-------
 *  7  | data bit 1
 *  6  | data bit 0
 * 5:0 | '0'
 *
 * So, to get the result we must shift and recompose the bits.
 * \Note Ovverrange bit is handled as the 17th data bit.
 */
tc520_data_t tc520_read(void)
{
	/* Start convertion and wait */
	CE_LOW();
	ticks_t start = timer_clock();
	do
	{
		/* timeout check */
		if (timer_clock() - start >= TC520_CONVERSION_TIMEOUT)
		{
			ASSERT(0);
			CE_HIGH();
			return TC520_MAX_VALUE;
		}
	}
	while(DV_LOW());

	start = timer_clock();
	do
	{
		/* timeout check */
		if (timer_clock() - start >= TC520_CONVERSION_TIMEOUT)
		{
			ASSERT(0);
			CE_HIGH();
			return TC520_MAX_VALUE;
		}
	}
	while(DV_HIGH());

	/* Ok, convertion finished, read result */
	CE_HIGH();
	READ_LOW();

	/* RX buffer could be dirty...*/
	ser_purge(spi_ser);

	/* I/O buffer */
	uint8_t buf[3] = "\x0\x0\x0";

	/* Dummy write to activate recv */
	ser_write(spi_ser, buf, sizeof(buf));
	ser_drain(spi_ser);
	READ_HIGH();

	/* recv */
	ASSERT(ser_read(spi_ser, buf, sizeof(buf)) == sizeof(buf));

	tc520_data_t res;

	/* Recompose data */
	res = (((tc520_data_t)(buf[0] & 0x3F)) << 10) | (((tc520_data_t)buf[1]) << 2) | (((tc520_data_t)buf[2]) >> 6);

	#define OVERRANGE_BIT BV(7)
	/* Handle overrange bit as 17th bit */
	if (buf[0] & OVERRANGE_BIT)
		res |= BV32(16);

	return res;
}


/**
 * Initialize tc520 A/D converter driver
 */
void tc520_init(Serial *spi_port)
{
	spi_ser = spi_port;
	/* init io ports */
	TC520_HW_INIT;
	/* Send initial load value */
	LOAD_LOW();
	ser_putchar(INIT_LOAD_VALUE, spi_ser);
	ser_drain(spi_ser);
	LOAD_HIGH();
}
