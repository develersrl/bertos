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
 * -->
 *
 * \brief Function library for secure digital memory.
 *
 * \author Daniele Basile <asterix@develer.com>
 */


#include "hw/hw_sd.h"
#include "cfg/cfg_sd.h"

#include <drv/sd.h>
#include <drv/timer.h>
#include <drv/hsmci_sam3.h>

#include <io/kfile.h>
#include <io/kblock.h>

#include <fs/fat.h>

#define LOG_LEVEL  SD_LOG_LEVEL
#define LOG_FORMAT SD_LOG_FORMAT
#include <cfg/log.h>
#include <cpu/power.h>

#include <string.h> /* memset */


#define SD_SEND_ALL_CID        BV(0)
#define SD_STATUS_APP_CMD      BV(5)
#define SD_STATUS_READY        BV(8)
#define SD_CARD_IS_LOCKED      BV(25)

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


#define CMD8_V_RANGE_CHECK_PAT    0xAA
#define CMD8_V_RANGE_27V_36V      (0x100 | CMD8_V_RANGE_CHECK_PAT)
#define CMD8_V_RANGE_LOW          (0x1000 | CMD8_V_RANGE_CHECK_PAT)
#define CMD8_V_ECHO_REPLY         0xFF
#define CMD8_SUPP_V_RANGE_REPLY   0xFF00

#define SD_STATUS_ERROR          BV(19)

#define SD_GET_ERRORS(status)   ((status) & 0xFFF80000)
#define SD_ADDR_TO_RCA(addr)    (uint32_t)(((addr) << 16) & 0xFFFF0000)
#define SD_GET_STATE(status)    (uint8_t)(((status) & 0x1E00) >> 9)

static const uint32_t tran_exp[] =
{
	10000,      100000,     1000000,    10000000,
	0,      0,      0,      0
};

static const uint8_t tran_mant[] =
{
	0,  10, 12, 13, 15, 20, 25, 30,
	35, 40, 45, 50, 55, 60, 70, 80,
};


void sd_dumpCsd(SdCSD *csd);
void sd_dumpCid(SdCID *cid);
void sd_dumpSsr(SdSSR *ssr);

void sd_sendInit(void);
void sd_goIdle(void);
int sd_sendIfCond(Sd *sd);
int sd_sendAppOpCond(Sd *sd);

int sd_getCid(Sd *sd, SdCID *cid, uint32_t addr, uint8_t flag);
int sd_getCsd(Sd *sd, SdCSD *csd);
int sd_getSrc(Sd *sd);

int sd_appStatus(Sd *sd);
int sd_getRelativeAddr(Sd *sd);
int sd_getStatus(Sd *sd, SdSSR *ssr, uint32_t *buf, size_t words);

int sd_selectCard(Sd *sd);
int sd_deSelectCard(Sd *sd);
int sd_setBusWidth(Sd *sd, size_t len);
int sd_set_BlockLen(Sd *sd, size_t len);
void sd_setHightSpeed(Sd *sd);


INLINE int sd_setBus4bit(Sd *sd)
{
	return sd_setBusWidth(sd, 4);
}

INLINE int sd_setBus1bit(Sd *sd)
{
	return sd_setBusWidth(sd, 1);
}


LOG_INFOB(
static void dump(const char *label, uint32_t *r, size_t len)
{
	ASSERT(r);
	size_t i;
	int j = 0;
	kprintf("\n%s [\n", label);
	for (i = 0; i < len; i++)
	{
		if (j == 5)
		{
			kputs("\n");
			j = 0;
		}
		kprintf("%08lx ", r[i]);
		j++;
	}
	kprintf("\n] len=%d\n\n", i);
}
)

void sd_dumpCsd(SdCSD *csd)
{
	ASSERT(csd);

	LOG_INFO("VERSION: %d.0\n", csd->structure ? 2 : 1);
	LOG_INFO("CARD COMMAND CLASS: %d\n", csd->ccc);
	LOG_INFO("MAX DATA RATE: %ld\n", csd->max_data_rate);
	LOG_INFO("WRITE BLK LEN BITS: %ld\n", csd->write_blk_bits);
	LOG_INFO("READ BLK LEN BITS: %ld\n", csd->read_blk_bits);
	LOG_INFO("ERASE SIZE: %ld\n", csd->erase_size);
	LOG_INFO("BLK NUM: %ld\n", csd->block_num);
	LOG_INFO("BLK LEN: %ld\n", csd->block_len);
	LOG_INFO("CAPACITY %ld\n", csd->capacity);
	LOG_INFO("FLAG Write: WP %d, W MISALIGN %d\n", csd->write_partial, csd->write_misalign);
	LOG_INFO("FLAG Read: RP %d, R MISALIGN %d\n", csd->read_partial, csd->read_misalign);

}

void sd_dumpCid(SdCID *cid)
{
	ASSERT(cid);

	LOG_INFO("MANFID: %d\n", cid->manfid);
	LOG_INFO("OEMID: %d\n", cid->oemid);
	LOG_INFO("SERIAL: %ld\n", cid->serial);
	LOG_INFO("PROD_NAME: %s\n", cid->prod_name);
	LOG_INFO("REV: %d.%d\n", cid->m_rev, cid->l_rev);
	LOG_INFO("OFF,Y,M: %lx, %ld %ld\n", cid->year_off, (BCD_TO_INT_32BIT(cid->year_off) / 12) + 2000,
												(BCD_TO_INT_32BIT(cid->year_off) % 12));
}

void sd_dumpSsr(SdSSR *ssr)
{
	ASSERT(ssr);

	LOG_INFO("BUS_WIDTH: %d\n", ssr->bus_width);
	LOG_INFO("TYPE: %d\n", ssr->card_type);
	LOG_INFO("AU_TYPE: %d\n", ssr->au_size);
	LOG_INFO("ERASE_SIZE: %d\n", ssr->erase_size);
	LOG_INFO("SPEED_CLASS: %d\n", ssr->speed_class);
}


static int sd_decodeCsd(SdCSD *csd, uint32_t *resp, size_t len)
{
	ASSERT(csd);
	ASSERT(resp);
	ASSERT(len >= 4);

	csd->structure = UNSTUFF_BITS(resp, 126, 2);
	csd->ccc = UNSTUFF_BITS(resp, 84, 12);

	csd->max_data_rate  = tran_exp[UNSTUFF_BITS(resp, 96, 3)] * tran_mant[UNSTUFF_BITS(resp, 99, 4)];

	/*
	 * CSD structure:
	 * - 0:
	 * 		- Version 1.01-1.10
	 * 		- Version 2.00/Standard Capacity
	 * - 1:
	 * 		- Version 2.00/High Capacity
	 * - >1: not defined.
	 */

	if (csd->structure == 0)
	{
		// (C_size + 1) x 2^(C_SIZE_MUL+2)
		csd->block_num = (1 + UNSTUFF_BITS(resp, 62, 12)) << (UNSTUFF_BITS(resp, 47, 3) + 2);

		csd->read_blk_bits = UNSTUFF_BITS(resp, 80, 4);
		csd->write_blk_bits = UNSTUFF_BITS(resp, 22, 4);

		csd->block_len = 1 << csd->read_blk_bits;
		csd->capacity  = csd->block_num * csd->block_len;

		csd->read_partial = UNSTUFF_BITS(resp, 79, 1);
		csd->read_misalign = UNSTUFF_BITS(resp, 77, 1);

		csd->write_misalign = UNSTUFF_BITS(resp, 78, 1);
		csd->write_partial = UNSTUFF_BITS(resp, 21, 1);

		if (UNSTUFF_BITS(resp, 46, 1))
		{
			csd->erase_size = 1;
		}
		else if(csd->write_blk_bits >= 9)
		{
			csd->erase_size = UNSTUFF_BITS(resp, 39, 7) + 1;
			csd->erase_size <<= csd->write_blk_bits - 9;
		}

		return 0;
	}
	else if (csd->structure == 1)
	{
		kprintf("csize %ld\n", UNSTUFF_BITS(resp, 48, 22));
		csd->capacity  = (1 + UNSTUFF_BITS(resp, 48, 22)) << 10;

		csd->write_blk_bits = 9;
		csd->write_partial = 0;
		csd->write_misalign = 0;

		csd->read_blk_bits = 9;
		csd->read_partial = 0;
		csd->read_misalign = 0;

		csd->erase_size = 1;
		// the block size if fixed to 512kb
		csd->block_len = (1 << csd->write_blk_bits) << 10;

		return 0;
	}
	else
	{
		kprintf("Unrecognised CSD structure version %d\n", csd->structure);
		return -1;
	}

	return 0;
}

void sd_sendInit(void)
{
	hsmci_init(NULL); //TODO: REMOVE IT!

	if (hsmci_sendCmd(0, 0, HSMCI_CMDR_SPCMD_INIT | HSMCI_CMDR_RSPTYP_NORESP))
		LOG_ERR("INIT: %lx\n", HSMCI_SR);
}


void sd_goIdle(void)
{
	hsmci_setSpeed(HSMCI_INIT_SPEED, false);
	if (hsmci_sendCmd(0, 0, HSMCI_CMDR_RSPTYP_NORESP))
		LOG_ERR("GO_IDLE: %lx\n", HSMCI_SR);
}

int sd_sendIfCond(Sd *sd)
{
	if (hsmci_sendCmd(8, CMD8_V_RANGE_27V_36V, HSMCI_CMDR_RSPTYP_48_BIT))
	{
		LOG_ERR("IF_COND %lx\n", HSMCI_SR);
		return -1;
	}
	hsmci_readResp(&(sd->status), 1);
	if (((sd->status) & 0xFFF) == CMD8_V_RANGE_27V_36V)
	{
		LOG_INFO("IF_COND: %lx\n", (sd->status));
		return 0;
	}
	LOG_ERR("IF_COND: %lx\n", (sd->status));
	return -1;
}

int sd_sendAppOpCond(Sd *sd)
{
	if (hsmci_sendCmd(55, 0, HSMCI_CMDR_RSPTYP_48_BIT))
	{
		LOG_ERR("APP_CMD %lx\n", HSMCI_SR);
		return -1;
	}

	hsmci_readResp(&(sd->status), 1);
	if ((sd->status) & (SD_STATUS_APP_CMD | SD_STATUS_READY))
	{
		if (hsmci_sendCmd(41, SD_HOST_VOLTAGE_RANGE | SD_OCR_CCS, HSMCI_CMDR_RSPTYP_48_BIT))// se cmd 8 va ok.
		{
			LOG_ERR("APP_OP_COND %lx\n", HSMCI_SR);
			return -1;
		}
		else
		{
			hsmci_readResp(&(sd->status), 1);
			if ((sd->status) & SD_OCR_BUSY)
			{
				LOG_INFO("SD power up! Hight Capability [%d]\n", (bool)((sd->status) & SD_OCR_CCS));
				return 0;
			}
		}
	}

	return -1;
}


int sd_getCid(Sd *sd, SdCID *cid, uint32_t addr, uint8_t flag)
{
	ASSERT(sd);
	ASSERT(cid);
	memset(cid, 0, sizeof(SdCID));

	uint8_t idx = 9; // CMD9 get cid from gived sd address (RCA)
	if (flag & SD_SEND_ALL_CID)
		idx = 2;


	if (hsmci_sendCmd(idx, SD_ADDR_TO_RCA(addr), HSMCI_CMDR_RSPTYP_136_BIT))
	{
		LOG_ERR("GET_CID %lx\n", HSMCI_SR);
		return -1;
	}
	else
	{
		uint32_t resp[4];
		hsmci_readResp(resp, 4);
		LOG_INFOB(dump("CID", resp, 4););

		cid->manfid        = UNSTUFF_BITS(resp, 120, 8);
		cid->oemid         = UNSTUFF_BITS(resp, 104, 16);
		cid->prod_name[0]      = UNSTUFF_BITS(resp, 96, 8);
		cid->prod_name[1]      = UNSTUFF_BITS(resp, 88, 8);
		cid->prod_name[2]      = UNSTUFF_BITS(resp, 80, 8);
		cid->prod_name[3]      = UNSTUFF_BITS(resp, 72, 8);
		cid->prod_name[4]      = UNSTUFF_BITS(resp, 64, 8);
		cid->m_rev         = UNSTUFF_BITS(resp, 60, 4);
		cid->l_rev         = UNSTUFF_BITS(resp, 56, 4);
		cid->serial        = (uint32_t)UNSTUFF_BITS(resp, 24, 32);
		cid->year_off      = UNSTUFF_BITS(resp, 8, 12);
	}

	return 0;
}

int sd_getCsd(Sd *sd, SdCSD *csd)
{
	ASSERT(sd);
	ASSERT(csd);
	memset(csd, 0, sizeof(SdCSD));

	LOG_INFO("Send to RCA: %lx\n", SD_ADDR_TO_RCA(sd->addr));
	if (hsmci_sendCmd(9, SD_ADDR_TO_RCA(sd->addr), HSMCI_CMDR_RSPTYP_136_BIT))
	{
		LOG_ERR("GET_CSD %lx\n", HSMCI_SR);
		return -1;
	}
	else
	{
		uint32_t resp[4];
		hsmci_readResp(resp, 4);
		LOG_INFOB(dump("CSD", resp, 4););
		sd_decodeCsd(csd, resp, 4);
	}

	return 0;
}

int sd_getRelativeAddr(Sd *sd)
{
	ASSERT(sd);
	if (hsmci_sendCmd(3, 0, HSMCI_CMDR_RSPTYP_48_BIT))
	{
		LOG_ERR("RCA: %lx\n", HSMCI_SR);
		return -1;
	}

	hsmci_readResp(&sd->addr, 1);
	sd->addr = sd->addr >> 16;

	LOG_INFOB(dump("RCA", &sd->addr, 1););

	return 0;
}

int sd_appStatus(Sd *sd)
{
	ASSERT(sd);
	LOG_INFO("Send to RCA: %lx\n", SD_ADDR_TO_RCA(sd->addr));
	if (hsmci_sendCmd(13, SD_ADDR_TO_RCA(sd->addr), HSMCI_CMDR_RSPTYP_48_BIT))
	{
		LOG_ERR("STATUS: %lx\n", HSMCI_SR);
		sd->status |= SD_STATUS_ERROR;
		return -1;
	}

	hsmci_readResp(&(sd->status), 1);
	LOG_INFOB(dump("STATUS", &(sd->status), 1););

	LOG_INFO("State[%d]\n", SD_GET_STATE(sd->status));

	if (sd->status & SD_STATUS_READY)
		return 0;

	return -1;
}


INLINE int sd_cardSelection(Sd *sd, uint32_t rca)
{
	ASSERT(sd);
	LOG_INFO("Select RCA: %lx\n", rca);
	if (hsmci_sendCmd(7, rca, HSMCI_CMDR_RSPTYP_R1B))
	{
		LOG_ERR("SELECT_SD: %lx\n", HSMCI_SR);
		sd->status |= SD_STATUS_ERROR;
		return -1;
	}

	HSMCI_CHECK_BUSY();
	hsmci_readResp(&(sd->status), 1);
	LOG_INFOB(dump("SELECT_SD", &(sd->status), 1););

	LOG_INFO("State[%d]\n", SD_GET_STATE(sd->status));

	if (sd->status & SD_STATUS_READY)
		return 0;

	return -1;
}

int sd_selectCard(Sd *sd)
{
	ASSERT(sd);
	uint32_t rca = SD_ADDR_TO_RCA(sd->addr);
	LOG_INFO("Select RCA: %lx\n", rca);
	if (hsmci_sendCmd(7, rca, HSMCI_CMDR_RSPTYP_R1B))
	{
		LOG_ERR("SELECT_SD: %lx\n", HSMCI_SR);
		return -1;
	}

	HSMCI_CHECK_BUSY();
	hsmci_readResp(&(sd->status), 1);

	LOG_INFOB(dump("SELECT_SD", &(sd->status), 1););
	LOG_INFO("State[%d]\n", SD_GET_STATE(sd->status));

	if (sd->status & SD_STATUS_READY)
		return 0;

	return -1;
}

int sd_deSelectCard(Sd *sd)
{
	ASSERT(sd);

	uint32_t rca = 0;
	if (!sd->addr)
		rca = SD_ADDR_TO_RCA(sd->addr + 1);

	LOG_INFO("Select RCA: %lx\n", rca);

	if (hsmci_sendCmd(7, rca, HSMCI_CMDR_RSPTYP_NORESP))
	{
		LOG_ERR("DESELECT_SD: %lx\n", HSMCI_SR);
		return -1;
	}

	return 0;
}

int sd_setBusWidth(Sd *sd, size_t len)
{
	ASSERT(sd);

	if (hsmci_sendCmd(55, SD_ADDR_TO_RCA(sd->addr), HSMCI_CMDR_RSPTYP_48_BIT))
	{
		LOG_ERR("APP_CMD %lx\n", HSMCI_SR);
		return -1;
	}

	hsmci_readResp(&(sd->status), 1);
	if ((sd->status) & (SD_STATUS_APP_CMD | SD_STATUS_READY))
	{

		uint8_t arg = 0;
		if (len == 4)
			arg = 2;

		if (hsmci_sendCmd(6, arg, HSMCI_CMDR_RSPTYP_48_BIT))
		{
			LOG_ERR("SET_BUS_WIDTH CMD: %lx\n", HSMCI_SR);
			return -1;
		}

		hsmci_readResp(&(sd->status), 1);

		LOG_INFOB(dump("SET_BUS_WIDTH", &(sd->status), 1););
		LOG_INFO("State[%d]\n", SD_GET_STATE(sd->status));

		if (sd->status & SD_STATUS_READY)
		{
			hsmci_setBusWidth(len);
			return 0;
		}
	}

	LOG_ERR("SET_BUS_WIDTH REP %lx\n", (sd->status));
	return -1;
}


int sd_set_BlockLen(Sd *sd, size_t len)
{
	ASSERT(sd);

	if (hsmci_sendCmd(16, len, HSMCI_CMDR_RSPTYP_48_BIT))
	{
		LOG_ERR("SET_BLK_LEN: %lx\n", HSMCI_SR);
		return -1;
	}

	hsmci_readResp(&(sd->status), 1);

	LOG_INFOB(dump("SET_BLK_LEN", &(sd->status), 1););
	LOG_INFO("State[%d]\n", SD_GET_STATE(sd->status));

	sd->b.blk_size = len;

	if (sd->status & SD_STATUS_READY)
		return 0;

	return -1;
}

int sd_getStatus(Sd *sd, SdSSR *ssr, uint32_t *buf, size_t words)
{
	ASSERT(sd);
	ASSERT(ssr);

	// Status reply with 512bit data, so the block size in byte is 64
	hsmci_read(buf, words, 64);

	if (hsmci_sendCmd(55, SD_ADDR_TO_RCA(sd->addr), HSMCI_CMDR_RSPTYP_48_BIT))
	{
		LOG_ERR("APP_CMD %lx\n", HSMCI_SR);
		return -1;
	}

	hsmci_readResp(&(sd->status), 1);
	if (sd->status & (SD_STATUS_APP_CMD | SD_STATUS_READY))
	{
		if (hsmci_sendCmd(13, 0, HSMCI_CMDR_RSPTYP_48_BIT |
				BV(HSMCI_CMDR_TRDIR) | HSMCI_CMDR_TRCMD_START_DATA | HSMCI_CMDR_TRTYP_SINGLE))
		{
			LOG_ERR("STATUS CMD: %lx\n", HSMCI_SR);
			return -1;
		}

		hsmci_readResp(&(sd->status), 1);
		LOG_INFOB(dump("STATUS", &(sd->status), 1););
		LOG_INFO("State[%d]\n", SD_GET_STATE(sd->status));

		if (sd->status & SD_STATUS_READY)
		{
			hsmci_waitTransfer();

			LOG_INFOB(dump("STATUS", buf, words););
			memset(ssr, 0, sizeof(SdSSR));
			ssr->bus_width  = UNSTUFF_BITS(buf, 510, 2);
			ssr->card_type  = UNSTUFF_BITS(buf, 480, 16);
			ssr->au_size  = UNSTUFF_BITS(buf, 432, 8);
			ssr->speed_class  = UNSTUFF_BITS(buf, 440, 8);
			ssr->erase_size = UNSTUFF_BITS(buf, 408, 24);

			return 0;
		}
	}

	return -1;
}


void sd_setHightSpeed(Sd *sd)
{
	(void)sd;
	hsmci_setSpeed(HSMCI_HIGH_SPEED, HSMCI_HS_MODE);
}


static size_t sd_SdReadDirect(struct KBlock *b, block_idx_t idx, void *buf, size_t offset, size_t size)
{
	ASSERT(buf);
	ASSERT(!((uint32_t)buf & 0x3));

	Sd *sd = SD_CAST(b);
	LOG_INFO("reading from block %ld, offset %d, size %d\n", idx, offset, size);

	hsmci_waitTransfer();
	hsmci_read(buf, size / 4, sd->b.blk_size);

	if (hsmci_sendCmd(17, idx * sd->b.blk_size + offset, HSMCI_CMDR_RSPTYP_48_BIT |
			BV(HSMCI_CMDR_TRDIR) | HSMCI_CMDR_TRCMD_START_DATA | HSMCI_CMDR_TRTYP_SINGLE))
	{
		LOG_ERR("SIGLE_BLK_READ: %lx\n", HSMCI_SR);
		return -1;
	}

	hsmci_readResp(&(sd->status), 1);

	LOG_INFOB(dump("SIGLE_BLK_READ", &(sd->status), 1););
	LOG_INFO("State[%d]\n", SD_GET_STATE(sd->status));

	if (sd->status & SD_STATUS_READY)
	{
		hsmci_waitTransfer();
		return size;
	}
	return -1;
}

static size_t sd_SdWriteDirect(KBlock *b, block_idx_t idx, const void *buf, size_t offset, size_t size)
{
	ASSERT(buf);
	ASSERT(!((uint32_t)buf & 0x3));

	Sd *sd = SD_CAST(b);
	const uint32_t *_buf = (const uint32_t *)buf;
	LOG_INFO("writing block %ld, offset %d, size %d\n", idx, offset, size);

	hsmci_waitTransfer();
	hsmci_write(_buf, size / 4, sd->b.blk_size);

	if (hsmci_sendCmd(24, idx * sd->b.blk_size + offset, HSMCI_CMDR_RSPTYP_48_BIT |
						HSMCI_CMDR_TRCMD_START_DATA | HSMCI_CMDR_TRTYP_SINGLE))
	{
		LOG_ERR("SIGLE_BLK_WRITE: %lx\n", HSMCI_SR);
		return -1;
	}

	hsmci_readResp(&(sd->status), 1);

	LOG_INFOB(dump("SIGLE_BLK_WR", &(sd->status), 1););
	LOG_INFO("State[%d]\n", SD_GET_STATE(sd->status));

	if (sd->status & SD_STATUS_READY)
	{
		return size;
	}

	return -1;
}


static int sd_SdError(KBlock *b)
{
	Sd *sd = SD_CAST(b);
	return SD_GET_ERRORS(sd->status);
}

static void sd_SdClearerr(KBlock *b)
{
	Sd *sd = SD_CAST(b);
	sd->status = 0;
}

static bool sd_blockInit(Sd *sd, KFile *ch)
{
	(void)ch;
	ASSERT(sd);
	memset(sd, 0, sizeof(*sd));
	DB(sd->b.priv.type = KBT_SD);

	/* Wait a few moments for supply voltage to stabilize */
	timer_delay(SD_START_DELAY);

	sd_sendInit();
	sd_goIdle();

	sd_sendIfCond(sd);

	ticks_t start = timer_clock();
	bool sd_power_on = false;
	do
	{
		if (!sd_sendAppOpCond(sd))
		{
			sd_power_on = true;
			break;
		}
		cpu_relax();
	}
	while (timer_clock() - start < SD_INIT_TIMEOUT);


	if (sd_power_on)
	{
		SdCID cid;
		if(sd_getCid(sd, &cid, 0, SD_SEND_ALL_CID) < 0)
			return false;
		else
		{
			sd_dumpCid(&cid);
		}

		if (sd_getRelativeAddr(sd) < 0)
			return false;
		else
		{
			LOG_INFO("RCA: %0lx\n", sd->addr);
		}

		SdCSD csd;
		if (sd_getCsd(sd, &csd) < 0)
			return false;
		else
		{
			sd->b.blk_cnt = csd.block_num * (csd.block_len / SD_DEFAULT_BLOCKLEN);
			LOG_INFO("blk_size %d, blk_cnt %ld\n", sd->b.blk_size, sd->b.blk_cnt);
			sd_dumpCsd(&csd);
		}

		if (sd_appStatus(sd) < 0)
		{
			LOG_INFO("STATUS: %ld\n", sd->status);
			return false;
		}

		if (sd->status & SD_CARD_IS_LOCKED)
		{
			LOG_INFO("SD is locked!\n");
			return false;
		}

		if (sd->status & SD_STATUS_READY)
		{
			sd_selectCard(sd);
			sd_set_BlockLen(sd, SD_DEFAULT_BLOCKLEN);
			sd_setBus4bit(sd);
			sd_setHightSpeed(sd);

			#if CONFIG_SD_AUTOASSIGN_FAT
				disk_assignDrive(&sd->b, 0);
			#endif

			return true;
		}
	}
	LOG_ERR("SD not ready.\n");
	return false;
}

static const KBlockVTable sd_unbuffered_vt =
{
	.readDirect = sd_SdReadDirect,
	.writeDirect = sd_SdWriteDirect,

	.error = sd_SdError,
	.clearerr = sd_SdClearerr,
};

static const KBlockVTable sd_buffered_vt =
{
	.readDirect = sd_SdReadDirect,
	.writeDirect = sd_SdWriteDirect,

	.readBuf = kblock_swReadBuf,
	.writeBuf = kblock_swWriteBuf,
	.load = kblock_swLoad,
	.store = kblock_swStore,

	.error = sd_SdError,
	.clearerr = sd_SdClearerr,
};

bool sd_hw_initUnbuf(Sd *sd, KFile *ch)
{
	if (sd_blockInit(sd, ch))
	{
		sd->b.priv.vt = &sd_unbuffered_vt;
		return true;
	}
	else
		return false;
}

static uint8_t sd_buf[SD_DEFAULT_BLOCKLEN];

bool sd_hw_initBuf(Sd *sd, KFile *ch)
{
	if (sd_blockInit(sd, ch))
	{
		sd->b.priv.buf = sd_buf;
		sd->b.priv.flags |= KB_BUFFERED | KB_PARTIAL_WRITE;
		sd->b.priv.vt = &sd_buffered_vt;
		sd->b.priv.vt->load(&sd->b, 0);
		return true;
	}
	else
		return false;
}



