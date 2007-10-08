/**
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief MCP41xxx digital potentiomenter driver (implementation)
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

#include <drv/timer.h>

#include <cfg/macros.h>
#include <cfg/cpu.h>
#include <cfg/compiler.h>

#include <hw_mcp41.h>
#include <drv/mcp41.h>
#include <drv/ser.h>

static Serial *spi_ser;

/**
 * Set resitance for digital pot \a dev
 */
void mcp41_setResistance(Mcp41Dev dev, mcp41_res_t res)
{
	MCP41_ON(dev);
	/* send command byte */
	ser_putchar(MCP41_WRITE_DATA, spi_ser);
	ser_putchar(((MCP41_HW_MAX * (uint32_t)res)  + (MCP41_MAX / 2)) / MCP41_MAX, spi_ser);
	ser_drain(spi_ser);

	MCP41_OFF(dev);
}


/**
 * Initialize mcp41 potentiometer driver
 */
void mcp41_init(Serial *spi_port)
{
	Mcp41Dev dev;

	/* Init all potentiometer ports */
	for (dev = 0; dev < MCP41_CNT; dev++)
	{
		SET_MCP41_DDR(dev);
		MCP41_OFF(dev);
	}

	spi_ser = spi_port;
}
