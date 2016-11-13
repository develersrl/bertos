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

#ifndef DRV_NRF24L01_H
#define DRV_NRF24L01_H

#include <cpu/types.h>

#include <drv/spi.h>

#include <cfg/macros.h>

#include <io/kfile.h>

#define   NRF24L01_NOP             0xff
#define   NRF24L01_R_REGISTER      0x00
#define   NRF24L01_W_REGISTER      0x01
#define   NRF24L01_R_RX_PAYLOAD    0x61
#define   NRF24L01_W_TX_PAYLOAD    0xA0
#define   NRF24L01_FLUSH_TX        0xE1
#define   NRF24L01_FLUSH_RX        0xE2
#define   NRF24L01_REUSE_TX_PL     0xE3
#define   NRF24L01_ACTIVATE        0x50
#define   NRF24L01_R_RX_PL_WID     0x60

#define   NRF24L01_RX_ADDR_P0      0x0A
#define   NRF24L01_RX_ADDR_P1      0x0B
#define   NRF24L01_TX_ADDR         0x10
#define   NRF24L01_RX_PW_P0        0x11
#define   NRF24L01_RX_PW_P1        0x12
#define   NRF24L01_FIFO_STATUS     0x17
#define   NRF24L01_MAX_RT          0x10


#define   NRF24L01_CONFIG_REG_ADDR 0x00

#define   NRF24L01_PWR_PRIM_RX     BV(0)
#define   NRF24L01_PWR_UP          BV(1)
#define   NRF24L01_CRCO            BV(2)
#define   NRF24L01_EN_CRC          BV(3)

#define   NRF24L01_EN_AA           0x01

#define   NRF24L01_ENAA_P0         BV(0)
#define   NRF24L01_ENAA_P1         BV(1)
#define   NRF24L01_ENAA_P2         BV(2)
#define   NRF24L01_ENAA_P3         BV(3)
#define   NRF24L01_ENAA_P4         BV(4)
#define   NRF24L01_ENAA_P5         BV(5)

#define   NRF24L01_STATUS_ADDR     0x07

#define   NRF24L01_FLUSH_TX        0xE1
#define   NRF24L01_TX_FULL         0x01
#define   NRF24L01_RX_DR           0x40
#define   NRF24L01_TX_DS           0x20

#define   NRF24L01_FEATURE         0x1d

#define   NRF24L01_EN_DPL          BV(2)

#define   NRF24L01_DYNPD           0x1c

#define   NRF24L01_DPL_P0         BV(0)
#define   NRF24L01_DPL_P1         BV(1)
#define   NRF24L01_DPL_P2         BV(2)
#define   NRF24L01_DPL_P3         BV(3)
#define   NRF24L01_DPL_P4         BV(4)
#define   NRF24L01_DPL_P5         BV(5)

#define   NRF24L01_RX_PW_P0       0x11
#define   NRF24L01_RX_PW_P1       0x12
#define   NRF24L01_RX_PW_P2       0x13
#define   NRF24L01_RX_PW_P3       0x14
#define   NRF24L01_RX_PW_P4       0x15
#define   NRF24L01_RX_PW_P5       0x16

typedef struct NRF24
{
	KFile fd;
	Spi *spi;
	uint8_t status;
	uint8_t error;
	mtime_t recv_timeout;
} NRF24;

/**
 * ID for NRF24.
 */
#define KFT_NRF24 MAKE_ID('N', 'R', 'F', '2')


/**
 * Convert + ASSERT from generic KFile to Radio.
 */
INLINE NRF24 * NRF24_CAST(KFile *fd)
{
	ASSERT(fd->_type == KFT_NRF24);
	return (NRF24 *)fd;
}

/*
 * NRF24L01 errors
 */
#define NRF24L01_TX_ERR        BV(0)
#define NRF24L01_RX_ERR        BV(1)
#define NRF24L01_RX_TIMEOUT    BV(2)
#define NRF24L01_FIFO_FULL     BV(3)
#define NRF24L01_MAX_RE_TX     BV(4)

#define NRF24L01_MAXPAYLOAD_LEN  32


static const char *error_msg[8] = {
	"Trasmission Error",
	"Receiver Error",
	"Receiver Timeout",
	"Fifo full",
	"Max retry",
};

INLINE const char *nrf24l01_decodeErrors(NRF24 *fd, int err)
{
	(void)fd;
	for (int i = 0; i < 8; i++)
	{
		if (err & BV(i))
			return error_msg[i];
	}
	return "";
}

void nfr24l01_sleep(void);
INLINE void nfr24l01_timeout(NRF24 *fd, mtime_t timeout)
{
	fd->recv_timeout = timeout;
}

void nrf24l01_init(NRF24 *nrf24, Spi *spi);

#endif /* DRV_NRF24L01_H */
