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
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief nRF4L01 Nordic radio chip
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include "nrf24l01.h"

#include "hw/hw_nrf24l01.h"
#include "cfg/cfg_spi.h"

#include <cfg/debug.h>
// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL   2
#define LOG_FORMAT  0
#include <cfg/log.h>

#include <cpu/types.h>
#include <cpu/power.h>

#include <drv/timer.h>
#include <drv/spi.h>

INLINE uint8_t nrf24l01_status(NRF24 *nrf24)
{
	spi_assertSS(nrf24->spi);
	nrf24->status = spi_sendRecv(nrf24->spi, CONFING_SPI_NOP);
	spi_deassertSS(nrf24->spi);

	return nrf24->status;
}



#define CMD_W(reg)  (NRF24L01_W_REGISTER << 5 | (reg))
#define CMD_R(reg)  (NRF24L01_R_REGISTER << 5 | (reg))

INLINE void nrf24l01_writeReg(NRF24 *nrf24, uint8_t reg, uint8_t data)
{
	ASSERT(nrf24);

	spi_assertSS(nrf24->spi);

	nrf24->status = spi_sendRecv(nrf24->spi, CMD_W(reg));
	spi_sendRecv(nrf24->spi, data);

	spi_deassertSS(nrf24->spi);
	LOG_INFO("wreg[%x] <- [%x] ST[%x]\n", reg, data, nrf24->status);
}

INLINE uint8_t nrf24l01_readReg(NRF24 *nrf24, uint8_t reg)
{
	ASSERT(nrf24);

	spi_assertSS(nrf24->spi);

	nrf24->status = spi_sendRecv(nrf24->spi, CMD_R(reg));
	uint8_t data = spi_sendRecv(nrf24->spi, CONFING_SPI_NOP);

	spi_deassertSS(nrf24->spi);
	LOG_INFO("rreg[%x] <- [%x] ST[%x]\n", reg, data, nrf24->status);

	return data;
}


INLINE void nrf24l01_activateFeature(NRF24 *nrf24)
{
	ASSERT(nrf24);

	spi_assertSS(nrf24->spi);

	nrf24->status = spi_sendRecv(nrf24->spi, NRF24L01_ACTIVATE);
	spi_sendRecv(nrf24->spi, 0x73);

	spi_deassertSS(nrf24->spi);
	spi_assertSS(nrf24->spi);

	LOG_INFO("Unlock/Lock Feature[%x] <- ST[%x]\n", NRF24L01_ACTIVATE, nrf24->status);
}

static void nrf24l01_setAddr(NRF24 *nrf24, uint8_t reg, uint8_t *addr, size_t len)
{
	spi_assertSS(nrf24->spi);

	nrf24->status = spi_sendRecv(nrf24->spi, CMD_W(reg));
	spi_write(nrf24->spi, addr, len);

	spi_deassertSS(nrf24->spi);
}

static size_t nrf24l01_write(struct KFile *_fd, const void *_buf, size_t size)
{
	ASSERT(_fd);
	ASSERT(_buf);

	const uint8_t *buff = (const uint8_t *)_buf;
	NRF24 *nrf24 = NRF24_CAST(_fd);

	nrf24l01_writeReg(nrf24, NRF24L01_CONFIG_REG_ADDR, \
			NRF24L01_PWR_UP | NRF24L01_EN_CRC);

	// fill tx fifo
	spi_assertSS(nrf24->spi);

	nrf24->status = spi_sendRecv(nrf24->spi, NRF24L01_W_TX_PAYLOAD);
	spi_write(nrf24->spi, buff, size);

	spi_deassertSS(nrf24->spi);

	// Pulse for CE -> starts the transmission.
	NRF24L01_CE_PULSE();

	// Read STATUS register
	nrf24l01_status(nrf24);

	// if exceed number of transmision packets
	if (nrf24->status & NRF24L01_MAX_RT)
	{

		// Clear MAX_RT bit in status register
		nrf24l01_writeReg(nrf24, NRF24L01_STATUS_ADDR, \
				nrf24->status | NRF24L01_MAX_RT);

		LOG_ERR("Max num of retrasmission.\n");
		nrf24->error |= NRF24L01_MAX_RE_TX;
		return size;
	}

	// If packet sent on TX
	if (nrf24->status & NRF24L01_TX_DS)
	{

		// Clear MAX_RT bit in status register
		nrf24l01_writeReg(nrf24, NRF24L01_STATUS_ADDR, \
				nrf24->status | NRF24L01_TX_DS);

		LOG_INFO("packet sent\n");
		return size;
	}

	// if exceed number of transmision packets
	if (nrf24->status & NRF24L01_TX_FULL)
	{
		spi_assertSS(nrf24->spi);
		nrf24->status = spi_sendRecv(nrf24->spi, NRF24L01_FLUSH_TX);
		spi_deassertSS(nrf24->spi);

		LOG_ERR("Fifo full.\n");
		nrf24->error |= NRF24L01_FIFO_FULL;
		return size;
	}

	LOG_ERR("Unmanaged status [%x]\n", nrf24->status);
	nrf24->error |= NRF24L01_TX_ERR;
	return size;
}

static size_t nrf24l01_read(struct KFile *_fd, void *_buf, size_t size)
{
	ASSERT(_fd);
	ASSERT(_buf);

	uint8_t *buff = (uint8_t *)_buf;
	NRF24 *nrf24 = NRF24_CAST(_fd);

	(void)size;

	spi_assertSS(nrf24->spi);
	nrf24->status = spi_sendRecv(nrf24->spi, NRF24L01_R_RX_PL_WID);
	size_t rx_len = spi_sendRecv(nrf24->spi, NRF24L01_RX_PW_P0);
	spi_read(nrf24->spi, buff, rx_len);

	LOG_INFO("Rxlen[%d]\n", rx_len);

	spi_deassertSS(nrf24->spi);

	// Put chip in RX mode
	nrf24l01_writeReg(nrf24, NRF24L01_CONFIG_REG_ADDR, \
			NRF24L01_PWR_UP | NRF24L01_EN_CRC | NRF24L01_PWR_PRIM_RX);

	ticks_t start = timer_clock();
	while (1)
	{
		if ((timer_clock() - start) > ms_to_ticks(nrf24->recv_timeout)
				&& (nrf24->recv_timeout != -1))
		{
			nrf24->error |= NRF24L01_RX_TIMEOUT;
			return 0;
		}

		if (!NRF24L01_IRQ_STATE())
		{

			// Read STATUS register
			nrf24l01_status(nrf24);

			// if exceed number of transmision packets
			if (nrf24->status & NRF24L01_RX_DR)
			{
				//read data
				spi_assertSS(nrf24->spi);

				nrf24->status = spi_sendRecv(nrf24->spi, NRF24L01_R_RX_PAYLOAD);
				spi_read(nrf24->spi, buff, rx_len);

				spi_deassertSS(nrf24->spi);

				// Clear RX bit in status register
				nrf24l01_writeReg(nrf24, NRF24L01_STATUS_ADDR, \
						nrf24->status | NRF24L01_RX_DR);

				LOG_INFO("rx payload\n");
				return rx_len;
			}
		}
		cpu_relax();
	}
	return rx_len;
}

static int nrf24l01_error(struct KFile *_fd)
{
	NRF24 *fd = NRF24_CAST(_fd);
	return fd->error;
}

static void nrf24l01_clearerr(struct KFile *_fd)
{
	NRF24 *nrf24 = NRF24_CAST(_fd);

	spi_assertSS(nrf24->spi);
	nrf24->status = spi_sendRecv(nrf24->spi, CONFING_SPI_NOP);

	nrf24l01_writeReg(nrf24, NRF24L01_STATUS_ADDR, 0xE);
	spi_deassertSS(nrf24->spi);

	nrf24->error = 0;
}

static uint8_t DEVICE_ADDR[5]  = {5,6,7,8,9};

/**
 * Reset the chip and apply the settings.
 */

void nrf24l01_init(NRF24 *nrf24, Spi *spi)
{
	ASSERT(nrf24);
	ASSERT(spi);

	//Set kfile struct type as a generic kfile structure.
	DB(nrf24->fd._type = KFT_NRF24);

	// Set up data flash programming functions.
	nrf24->fd.read = nrf24l01_read;
	nrf24->fd.write = nrf24l01_write;
	nrf24->fd.error = nrf24l01_error;
	nrf24->fd.clearerr = nrf24l01_clearerr;
	nrf24->error = 0;
	nrf24->recv_timeout = -1;
	nrf24->spi = spi;

	NRF24L01_HW_INIT();

	// Discard transmision
	spi_deassertSS(spi);
	NRF24L01_CE_ON();

	/* Enable dinamic payload packet */
	// Put in power-down mode to activate feature reg
	nrf24l01_writeReg(nrf24, NRF24L01_CONFIG_REG_ADDR, 0);

	// Enable feature reg: Activare cmd follewed by 0x73
	nrf24l01_activateFeature(nrf24);

	// Enable Dinamic payload packet
	nrf24l01_writeReg(nrf24, NRF24L01_FEATURE, NRF24L01_EN_DPL);
	// Enable on pipe 0 dinamic rx and tx
	nrf24l01_writeReg(nrf24, NRF24L01_EN_AA, NRF24L01_ENAA_P0);
	nrf24l01_writeReg(nrf24, NRF24L01_DYNPD, NRF24L01_DPL_P0);

	nrf24l01_setAddr(nrf24, NRF24L01_TX_ADDR, DEVICE_ADDR, sizeof(DEVICE_ADDR));
	nrf24l01_setAddr(nrf24, NRF24L01_RX_ADDR_P0, DEVICE_ADDR, sizeof(DEVICE_ADDR));

}

