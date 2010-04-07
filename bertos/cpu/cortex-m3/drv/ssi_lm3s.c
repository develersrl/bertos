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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief LM3S1968 Synchronous Serial Interface (SSI) driver.
 *
 * \author Andrea Righi <arighi@develer.com>
 */

#include <cfg/compiler.h>
#include <cfg/debug.h>
#include "io/lm3s.h"
#include "drv/ssi_lm3s.h"

/* SSI clocking informations (CPSDVSR + SCR) */
struct ssi_clock
{
	unsigned int cpsdvsr;
	unsigned int scr;
};

/*
 * Evaluate the SSI clock prescale (SSICPSR) and SSI serial clock rate (SCR).
 */
INLINE struct ssi_clock
lm3s_ssi_prescale(unsigned int bitrate)
{
	struct ssi_clock ret;

	for (ret.cpsdvsr = 2, ret.scr = CPU_FREQ / bitrate / ret.cpsdvsr - 1;
			ret.scr > 255; ret.cpsdvsr += 2);
	ASSERT(ret.cpsdvsr < 255);

	return ret;
}

/*
 * Initialize the SSI interface.
 *
 * @base: the SSI port base address.
 * @frame: the data transfer protocol (SSI_FRF_MOTO_MODE_0,
 * SSI_FRF_MOTO_MODE_1, SSI_FRF_MOTO_MODE_2, SSI_FRF_MOTO_MODE_3, SSI_FRF_TI or
 * SSI_FRF_NMW)
 * @mode: the mode of operation (SSI_MODE_MASTER, SSI_MODE_SLAVE,
 * SSI_MODE_SLAVE_OD)
 * @bitrate: the SSI clock rate
 * @data_width: number of bits per frame
 *
 * Return 0 in case of success, a negative value otherwise.
 */
int lm3s_ssi_init(uint32_t base, uint32_t frame, int mode,
                   unsigned int bitrate, unsigned int data_width)
{
	struct ssi_clock ssi_clock;

	ASSERT(base == SSI0_BASE || base == SSI1_BASE);
	/* Configure the SSI operating mode */
	switch (mode)
	{
		/* SSI Slave Mode Output Disable */
		case SSI_MODE_SLAVE_OD:
			HWREG(base + SSI_O_CR1) = SSI_CR1_SOD;
			break;
		/* SSI Slave */
		case SSI_MODE_SLAVE:
			HWREG(base + SSI_O_CR1) = SSI_CR1_MS;
			break;
		/* SSI Master */
		case SSI_MODE_MASTER:
			HWREG(base + SSI_O_CR1) = 0;
			break;
		default:
			ASSERT(0);
			return -1;
	}
	/* Configure the peripheral clock and frame format */
	ssi_clock = lm3s_ssi_prescale(bitrate);
	HWREG(base + SSI_O_CPSR) = ssi_clock.cpsdvsr;
	HWREG(base + SSI_O_CR0) =
			(ssi_clock.scr << 8)	|
			((frame & 3) << 6)	|
			(frame & SSI_CR0_FRF_M) |
			(data_width - 1);
	return 0;
}

/* Enable the SSI interface */
void lm3s_ssi_enable(uint32_t base)
{
	HWREG(base + SSI_O_CR1) |= SSI_CR1_SSE;
}

/* Disable the SSI interface */
void lm3s_ssi_disable(uint32_t base)
{
	HWREG(base + SSI_O_CR1) &= ~SSI_CR1_SSE;
}

/*
 * Put a frame into the SSI transmit FIFO.
 *
 * NOTE: the upper bits of the frame will be automatically discarded by the
 * hardware according to the frame data width, configured by lm3s_ssi_init().
 */
void lm3s_ssi_write_frame(uint32_t base, uint32_t val)
{
	/* Wait for available space in the TX FIFO */
	while (!(HWREG(base + SSI_O_SR) & SSI_SR_TNF))
		cpu_relax();
	/* Enqueue data to the TX FIFO */
	HWREG(base + SSI_O_DR) = val;
}

/*
 * Put a frame into the SSI transmit FIFO without blocking.
 *
 * NOTE: the upper bits of the frame will be automatically discarded by the
 * hardware according to the frame data width, configured by lm3s_ssi_init().
 *
 * Return the number of frames written to the TX FIFO.
 */
int lm3s_ssi_write_frame_nonblocking(uint32_t base, uint32_t val)
{
	/* Check for available space in the TX FIFO */
	if (!(HWREG(base + SSI_O_SR) & SSI_SR_TNF))
		return 0;
	/* Enqueue data to the TX FIFO */
	HWREG(base + SSI_O_DR) = val;
	return 1;
}

/*
 * Get a frame from the SSI receive FIFO.
 */
void lm3s_ssi_read_frame(uint32_t base, uint32_t *val)
{
	/* Wait for data available in the RX FIFO */
	while (!(HWREG(base + SSI_O_SR) & SSI_SR_RNE))
		cpu_relax();
	/* Read data from SSI RX FIFO */
	*val = HWREG(base + SSI_O_DR);
}

/*
 * Get a frame into the SSI receive FIFO without blocking.
 *
 * Return the number of frames read from the RX FIFO.
 */
int lm3s_ssi_read_frame_nonblocking(uint32_t base, uint32_t *val)
{
	/* Check for data available in the RX FIFO */
	if (!(HWREG(base + SSI_O_SR) & SSI_SR_RNE))
		return 0;
	/* Read data from SSI RX FIFO */
	*val = HWREG(base + SSI_O_DR);
	return 1;
}

/*
 * Check if the SSI transmitter is busy or not
 *
 * This allows to determine whether the TX FIFO have been cleared by the
 * hardware, so the transmission can be safely considered completed.
 */
bool lm3s_ssi_txdone(uint32_t base)
{
	/* Check if the SSI is busy */
	return (HWREG(base + SSI_O_SR) & SSI_SR_BSY) ? true : false;
}
