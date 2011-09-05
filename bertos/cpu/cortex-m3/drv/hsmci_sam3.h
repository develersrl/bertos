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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief HSMCI driver functions.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef DRV_HSMCI_SAM3_H
#define DRV_HSMCI_SAM3_H

#include <cfg/macros.h>
#include <cfg/debug.h>

#include <io/cm3.h>

#define CMD8_V_RANGE_CHECK_PAT    0xAA
#define CMD8_V_RANGE_27V_36V      (0x100 | CMD8_V_RANGE_CHECK_PAT)
#define CMD8_V_RANGE_LOW          (0x1000 | CMD8_V_RANGE_CHECK_PAT)
#define CMD8_V_ECHO_REPLY         0xFF
#define CMD8_SUPP_V_RANGE_REPLY   0xFF00


#define SD_OCR_CCS              BV(30)     /**< SD Card Capacity Status (CCS) */
#define SD_OCR_BUSY             BV(31)     /**< SD/MMC Card power up status bit (busy) */

#define SD_OCR_VDD_27_28        BV(15)
#define SD_OCR_VDD_28_29        BV(16)
#define SD_OCR_VDD_29_30        BV(17)
#define SD_OCR_VDD_30_31        BV(18)
#define SD_OCR_VDD_31_32        BV(19)
#define SD_OCR_VDD_32_33        BV(20)


#define SD_HOST_VOLTAGE_RANGE     (SD_OCR_VDD_27_28 | \
                                   SD_OCR_VDD_28_29 | \
                                   SD_OCR_VDD_29_30 | \
                                   SD_OCR_VDD_30_31 | \
                                   SD_OCR_VDD_31_32 | \
                                   SD_OCR_VDD_32_33)

#define HSMCI_CHECK_BUSY() \
	do { \
		cpu_relax(); \
	} while (!(HSMCI_SR & BV(HSMCI_SR_NOTBUSY)))


#define HSMCI_INIT_SPEED  400000

typedef struct Hsmci
{
} Hsmci;

INLINE void hsmci_enableIrq(void)
{
	HSMCI_IER = BV(HSMCI_IER_RTOE);
}

INLINE void hsmci_disableIrq(void)
{
	HSMCI_IDR = BV(HSMCI_IER_RTOE);
}

INLINE void hsmci_setBusWidth(size_t len)
{
	int sdcsel= 0;
	if (len == 4)
		sdcsel = 2;
	if (len == 8)
		sdcsel = 3;

	HSMCI_SDCR = (sdcsel << HSMCI_SDCR_SDCBUS_SHIFT) & HSMCI_SDCR_SDCBUS_MASK;
}

void hsmci_readResp(uint32_t *resp, size_t len);
bool hsmci_sendCmd(uint8_t index, uint32_t argument, uint32_t reply_type);

void hsmci_prgRxDMA(uint32_t *buf, size_t word_num, size_t blk_size);
void hsmci_prgTxDMA(const uint32_t *buf, size_t word_num, size_t blk_size);
void hsmci_waitTransfer(void);

void hsmci_setSpeed(uint32_t data_rate, int flag);




void hsmci_init(Hsmci *hsmci);

#endif /* DRV_HSMCI_SAM3_H */
