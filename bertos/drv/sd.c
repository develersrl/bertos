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
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Function library for secure digital memory.
 *
 * \author Francesco Sacchi <batt@develer.com>
 */


#include "sd.h"
#include "hw/hw_sd.h"
#include <io/kfile.h>
#include <io/kblock.h>
#include <drv/timer.h>

#include <fs/fat.h>

#include "cfg/cfg_sd.h"

#define LOG_LEVEL  SD_LOG_LEVEL
#define LOG_FORMAT SD_LOG_FORMAT
#include <cfg/log.h>
#include <cpu/power.h>

#include <string.h> /* memset */

/**
 * Card Specific Data
 * read directly from the card.
 */
typedef struct CardCSD
{
	uint16_t block_len;  ///< Length of a block
	uint32_t block_num;  ///< Number of block on the card
	uint16_t capacity;   ///< Card capacity in MB
} CardCSD;

#define SD_IN_IDLE    0x01
#define SD_STARTTOKEN 0xFE

#define TIMEOUT_NAC   16384
#define SD_DEFAULT_BLOCKLEN 512

#define SD_BUSY_TIMEOUT ms_to_ticks(200)

static bool sd_select(Sd *sd, bool state)
{
	KFile *fd = sd->ch;

	if (state)
	{
		SD_CS_ON();

		ticks_t start = timer_clock();
		do
		{
			if (kfile_getc(fd) == 0xff)
				return true;

			cpu_relax();
		}
		while (timer_clock() - start < SD_BUSY_TIMEOUT);

		SD_CS_OFF();
		LOG_ERR("sd_select timeout\n");
		return false;
	}
	else
	{
		kfile_putc(0xff, fd);
		kfile_flush(fd);
		SD_CS_OFF();
		return true;
	}
}

static int16_t sd_waitR1(Sd *sd)
{
	uint8_t datain;

	for (int i = 0; i < TIMEOUT_NAC; i++)
	{
		datain = kfile_getc(sd->ch);
		if (datain != 0xff)
			return (int16_t)datain;
	}
	LOG_ERR("Timeout waiting R1\n");
	return EOF;
}

static int16_t sd_sendCommand(Sd *sd, uint8_t cmd, uint32_t param, uint8_t crc)
{
	KFile *fd = sd->ch;
	/* The 7th bit of command must be a 1 */
	kfile_putc(cmd | 0x40, fd);

	/* send parameter */
	kfile_putc((param >> 24) & 0xFF, fd);
	kfile_putc((param >> 16) & 0xFF, fd);
	kfile_putc((param >> 8) & 0xFF, fd);
	kfile_putc((param) & 0xFF, fd);

	kfile_putc(crc, fd);

	return sd_waitR1(sd);
}

static bool sd_getBlock(Sd *sd, void *buf, size_t len)
{
	uint8_t token;
	uint16_t crc;

	KFile *fd = sd->ch;

	for (int i = 0; i < TIMEOUT_NAC; i++)
	{
		token = kfile_getc(fd);
		if (token != 0xff)
		{
			if (token == SD_STARTTOKEN)
			{
				if (kfile_read(fd, buf, len) == len)
				{
					if (kfile_read(fd, &crc, sizeof(crc)) == sizeof(crc))
						/* check CRC here if needed */
						return true;
					else
						LOG_ERR("get_block error getting crc\n");
				}
				else
					LOG_ERR("get_block len error: %d\n", (int)len);
			}
			else
				LOG_ERR("get_block token error: %02X\n", token);

			return false;
		}
	}

	LOG_ERR("get_block timeout waiting token\n");
	return false;
}

#define SD_SELECT(sd) \
do \
{ \
	if (!sd_select((sd), true)) \
	{ \
		LOG_ERR("%s failed, card busy\n", __func__); \
		return EOF; \
	} \
} \
while (0)

#define SD_SETBLOCKLEN 0x50

static int16_t sd_setBlockLen(Sd *sd, uint32_t newlen)
{
	SD_SELECT(sd);

	sd->r1 = sd_sendCommand(sd, SD_SETBLOCKLEN, newlen, 0);

	sd_select(sd, false);
	return sd->r1;
}

#define SD_SEND_CSD 0x49

static int16_t sd_getCSD(Sd *sd, CardCSD *csd)
{
	SD_SELECT(sd);

	int16_t r1 = sd_sendCommand(sd, SD_SEND_CSD, 0, 0);

	if (r1)
	{
		LOG_ERR("send_csd failed: %04X\n", sd->r1);
		sd_select(sd, false);
		return r1;
	}

	uint8_t buf[16];
	bool res = sd_getBlock(sd, buf, sizeof(buf));
	sd_select(sd, false);

	if (res)
	{
		#if LOG_LEVEL >= LOG_LVL_INFO
			LOG_INFO("CSD: [");
			for (int i = 0; i < 16; i++)
				kprintf("%02X ", buf[i]);
			kprintf("]\n");
		#endif

		uint16_t mult = (1L << ((((buf[9] & 0x03) << 1) | ((buf[10] & 0x80) >> 7)) + 2));
		uint16_t c_size = (((uint16_t)(buf[6] & 0x03)) << 10) | (((uint16_t)buf[7]) << 2) |
				  (((uint16_t)(buf[8] & 0xC0)) >> 6);

		csd->block_len = (1L << (buf[5] & 0x0F));
		csd->block_num = (c_size + 1) * mult;
		csd->capacity = (csd->block_len * csd->block_num) >> 20; // in MB

		LOG_INFO("block_len %d bytes, block_num %ld, total capacity %dMB\n", csd->block_len, csd->block_num, csd->capacity);
		return 0;
	}
	else
		return EOF;
}


#define SD_READ_SINGLEBLOCK 0x51

static size_t sd_readDirect(struct KBlock *b, block_idx_t idx, void *buf, size_t offset, size_t size)
{
	Sd *sd = SD_CAST(b);
	LOG_INFO("reading from block %ld, offset %d, size %d\n", idx, offset, size);

	if (sd->tranfer_len != size)
	{
		if ((sd->r1 = sd_setBlockLen(sd, size)))
		{
			LOG_ERR("setBlockLen failed: %04X\n", sd->r1);
			return 0;
		}
		sd->tranfer_len = size;
	}

	SD_SELECT(sd);

	sd->r1 = sd_sendCommand(sd, SD_READ_SINGLEBLOCK, idx * SD_DEFAULT_BLOCKLEN + offset, 0);

	if (sd->r1)
	{
		LOG_ERR("read single block failed: %04X\n", sd->r1);
		sd_select(sd, false);
		return 0;
	}

	bool res = sd_getBlock(sd, buf, size);
	sd_select(sd, false);
	if (!res)
	{
		LOG_ERR("read single block failed reading data\n");
		return 0;
	}
	else
		return size;
}

#define SD_WRITE_SINGLEBLOCK 0x58
#define SD_DATA_ACCEPTED     0x05

static size_t sd_writeDirect(KBlock *b, block_idx_t idx, const void *buf, size_t offset, size_t size)
{
	Sd *sd = SD_CAST(b);
	KFile *fd = sd->ch;
	ASSERT(offset == 0);
	ASSERT(size == SD_DEFAULT_BLOCKLEN);

	LOG_INFO("writing block %ld\n", idx);
	if (sd->tranfer_len != SD_DEFAULT_BLOCKLEN)
	{
		if ((sd->r1 = sd_setBlockLen(sd, SD_DEFAULT_BLOCKLEN)))
		{
			LOG_ERR("setBlockLen failed: %04X\n", sd->r1);
			return 0;
		}
		sd->tranfer_len = SD_DEFAULT_BLOCKLEN;
	}

	SD_SELECT(sd);

	sd->r1 = sd_sendCommand(sd, SD_WRITE_SINGLEBLOCK, idx * SD_DEFAULT_BLOCKLEN, 0);

	if (sd->r1)
	{
		LOG_ERR("write single block failed: %04X\n", sd->r1);
		sd_select(sd, false);
		return 0;
	}

	kfile_putc(SD_STARTTOKEN, fd);
	kfile_write(fd, buf, SD_DEFAULT_BLOCKLEN);
	/* send fake crc */
	kfile_putc(0, fd);
	kfile_putc(0, fd);

	uint8_t dataresp = kfile_getc(fd);
	sd_select(sd, false);

	if ((dataresp & 0x1f) != SD_DATA_ACCEPTED)
	{
		LOG_ERR("write block %ld failed: %02X\n", idx, dataresp);
		return EOF;
	}

	return SD_DEFAULT_BLOCKLEN;
}

void sd_writeTest(Sd *sd)
{
	uint8_t buf[SD_DEFAULT_BLOCKLEN];
	memset(buf, 0, sizeof(buf));

	for (block_idx_t i = 0; i < sd->b.blk_cnt; i++)
	{
		LOG_INFO("writing block %ld: %s\n", i, (sd_writeDirect(&sd->b, i, buf, 0, SD_DEFAULT_BLOCKLEN) == SD_DEFAULT_BLOCKLEN) ? "OK" : "FAIL");
	}
}


bool sd_test(Sd *sd)
{
	uint8_t buf[SD_DEFAULT_BLOCKLEN];

	if (sd_readDirect(&sd->b, 0, buf, 0, sd->b.blk_size) != sd->b.blk_size)
		return false;

	kputchar('\n');
	for (int i = 0; i < SD_DEFAULT_BLOCKLEN; i++)
	{
		kprintf("%02X ", buf[i]);
		buf[i] = i;
		if (!((i+1) % 16))
			kputchar('\n');
	}

	if (sd_writeDirect(&sd->b, 0, buf, 0, SD_DEFAULT_BLOCKLEN) != SD_DEFAULT_BLOCKLEN)
		return false;

	memset(buf, 0, sizeof(buf));
	if (sd_readDirect(&sd->b, 0, buf, 0, sd->b.blk_size) != sd->b.blk_size)
		return false;

	kputchar('\n');
	for (block_idx_t i = 0; i < sd->b.blk_size; i++)
	{
		kprintf("%02X ", buf[i]);
		buf[i] = i;
		if (!((i+1) % 16))
			kputchar('\n');
	}

	return true;
}

static int sd_error(KBlock *b)
{
	Sd *sd = SD_CAST(b);
	return sd->r1;
}

static void sd_clearerr(KBlock *b)
{
	Sd *sd = SD_CAST(b);
	sd->r1 = 0;
}

static const KBlockVTable sd_unbuffered_vt =
{
	.readDirect = sd_readDirect,
	.writeDirect = sd_writeDirect,

	.error = sd_error,
	.clearerr = sd_clearerr,
};

static const KBlockVTable sd_buffered_vt =
{
	.readDirect = sd_readDirect,
	.writeDirect = sd_writeDirect,

	.readBuf = kblock_swReadBuf,
	.writeBuf = kblock_swWriteBuf,
	.load = kblock_swLoad,
	.store = kblock_swStore,

	.error = sd_error,
	.clearerr = sd_clearerr,
};

#define SD_GO_IDLE_STATE     0x40
#define SD_GO_IDLE_STATE_CRC 0x95
#define SD_SEND_OP_COND      0x41
#define SD_SEND_OP_COND_CRC  0xF9

#define SD_START_DELAY  10
#define SD_INIT_TIMEOUT ms_to_ticks(2000)
#define SD_IDLE_RETRIES 4

static bool sd_blockInit(Sd *sd, KFile *ch)
{
	ASSERT(sd);
	ASSERT(ch);
	memset(sd, 0, sizeof(*sd));
	DB(sd->b.priv.type = KBT_SD);
	sd->ch = ch;

	SD_CS_INIT();
	SD_CS_OFF();

	/* Wait a few moments for supply voltage to stabilize */
	timer_delay(SD_START_DELAY);

	/* Give 80 clk pulses to wake up the card */
	for (int i = 0; i < 10; i++)
		kfile_putc(0xff, ch);
	kfile_flush(ch);

	for (int i = 0; i < SD_IDLE_RETRIES; i++)
	{
		SD_SELECT(sd);
		sd->r1 = sd_sendCommand(sd, SD_GO_IDLE_STATE, 0, SD_GO_IDLE_STATE_CRC);
		sd_select(sd, false);

		if (sd->r1 == SD_IN_IDLE)
			break;
	}

	if (sd->r1 != SD_IN_IDLE)
	{
		LOG_ERR("go_idle_state failed: %04X\n", sd->r1);
		return false;
	}

	ticks_t start = timer_clock();

	/* Wait for card to start */
	do
	{
		SD_SELECT(sd);
		sd->r1 = sd_sendCommand(sd, SD_SEND_OP_COND, 0, SD_SEND_OP_COND_CRC);
		sd_select(sd, false);
		cpu_relax();
	}
	while (sd->r1 != 0 && timer_clock() - start < SD_INIT_TIMEOUT);

	if (sd->r1)
	{
		LOG_ERR("send_op_cond failed: %04X\n", sd->r1);
		return false;
	}

	sd->r1 = sd_setBlockLen(sd, SD_DEFAULT_BLOCKLEN);
	sd->tranfer_len = SD_DEFAULT_BLOCKLEN;

	if (sd->r1)
	{
		LOG_ERR("setBlockLen failed: %04X\n", sd->r1);
		return false;
	}

	/* Avoid warning for uninitialized csd use (gcc bug?) */
	CardCSD csd = csd;

	sd->r1 = sd_getCSD(sd, &csd);

	if (sd->r1)
	{
		LOG_ERR("getCSD failed: %04X\n", sd->r1);
		return false;
	}

	sd->b.blk_size = SD_DEFAULT_BLOCKLEN;
	sd->b.blk_cnt = csd.block_num * (csd.block_len / SD_DEFAULT_BLOCKLEN);
	LOG_INFO("blk_size %d, blk_cnt %ld\n", sd->b.blk_size, sd->b.blk_cnt);

#if CONFIG_SD_AUTOASSIGN_FAT
	disk_assignDrive(&sd->b, 0);
#endif

	return true;
}

#if CPU_CM3_SAM3X8

#include <drv/hsmci_sam3.h>

/* SD commands                           type  argument     response */
  /* class 0 */
/* This is basically the same command as for MMC with some quirks. */
#define SD_SEND_RELATIVE_ADDR     3   /* bcr                     R6  */
#define SD_SEND_IF_COND           8   /* bcr  [11:0] See below   R7  */
#define SD_SWITCH_VOLTAGE         11  /* ac                      R1  */

  /* class 10 */
#define SD_SWITCH                 6   /* adtc [31:0] See below   R1  */

  /* class 5 */
#define SD_ERASE_WR_BLK_START    32   /* ac   [31:0] data addr   R1  */
#define SD_ERASE_WR_BLK_END      33   /* ac   [31:0] data addr   R1  */

  /* Application commands */
#define SD_APP_SET_BUS_WIDTH      6   /* ac   [1:0] bus width    R1  */
#define SD_APP_SD_STATUS         13   /* adtc                    R1  */
#define SD_APP_SEND_NUM_WR_BLKS  22   /* adtc                    R1  */
#define SD_APP_OP_COND           41   /* bcr  [31:0] OCR         R3  */
#define SD_APP_SEND_SCR          51   /* adtc                    R1  */

/* OCR bit definitions */
#define SD_OCR_S18R     (1 << 24)    /* 1.8V switching request */
#define SD_ROCR_S18A        SD_OCR_S18R  /* 1.8V switching accepted by card */
#define SD_OCR_XPC      (1 << 28)    /* SDXC power control */

/*
 * SD_SWITCH argument format:
 *
 *      [31] Check (0) or switch (1)
 *      [30:24] Reserved (0)
 *      [23:20] Function group 6
 *      [19:16] Function group 5
 *      [15:12] Function group 4
 *      [11:8] Function group 3
 *      [7:4] Function group 2
 *      [3:0] Function group 1
 */

/*
 * SD_SEND_IF_COND argument format:
 *
 *  [31:12] Reserved (0)
 *  [11:8] Host Voltage Supply Flags
 *  [7:0] Check Pattern (0xAA)
 */

/*
 * SCR field definitions
 */

#define SCR_SPEC_VER_0      0   /* Implements system specification 1.0 - 1.01 */
#define SCR_SPEC_VER_1      1   /* Implements system specification 1.10 */
#define SCR_SPEC_VER_2      2   /* Implements system specification 2.00-3.0X */

#define UNSTUFF_BITS(resp, start, size)                   \
    ({                              \
        const uint32_t __size = size;                \
        const uint32_t __mask = (__size < 32 ? 1 << __size : 0) - 1; \
        const uint32_t __off = 3 - ((start) / 32);           \
        const uint32_t __shft = (start) & 31;            \
        uint32_t __res;                      \
                                    \
        __res = resp[__off] >> __shft;              \
        if (__size + __shft > 32)               \
            __res |= resp[__off-1] << ((32 - __shft) % 32); \
        __res & __mask;                     \
    })


#define SD_ADDR_TO_RCA(addr)    (uint32_t)(((addr) << 16) & 0xFFFF0000)

#define BCD_TO_INT_32BIT(bcd)  ((uint32_t )((bcd) & 0xf) * 1 +  \
								(((bcd) >> 4) & 0xf)  * 10 +      \
								(((bcd) >> 8) & 0xf)  * 100 +     \
								(((bcd) >> 12) & 0xf) * 1000 +   \
								(((bcd) >> 16) & 0xf) * 10000 +   \
								(((bcd) >> 20) & 0xf) * 100000 +  \
								(((bcd) >> 24) & 0xf) * 1000000 + \
								(((bcd) >> 28) & 0xf) * 10000000) \

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
		kprintf("%08lx", r[i]);
		j++;
	}
	kprintf("\n] len=%d\n\n", i);
}
)

static const uint32_t tran_exp[] = {
    10000,      100000,     1000000,    10000000,
    0,      0,      0,      0
};

static const uint8_t tran_mant[] = {
    0,  10, 12, 13, 15, 20, 25, 30,
    35, 40, 45, 50, 55, 60, 70, 80,
};

static int sd_decodeCsd(SDcsd *csd, uint32_t *resp, size_t len)
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
		csd->blk_num = (1 + UNSTUFF_BITS(resp, 62, 12)) << (UNSTUFF_BITS(resp, 47, 3) + 2);

		csd->read_blk_bits = UNSTUFF_BITS(resp, 80, 4);
		csd->write_blk_bits = UNSTUFF_BITS(resp, 22, 4);

		csd->blk_len = 1 << csd->read_blk_bits;
        csd->capacity  = csd->blk_num * csd->blk_len;

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
		csd->blk_len = (1 << csd->write_blk_bits) << 10;

        return 0;
	}
    else
	{
        kprintf("Unrecognised CSD structure version %d\n", csd->structure);
        return -1;
    }

    return 0;
}


void sd_dumpCsd(Sd *sd)
{
	ASSERT(sd);

	LOG_INFO("VERSION: %d.0\n", sd->csd.structure ? 2 : 1);
    LOG_INFO("CARD COMMAND CLASS: %d\n", sd->csd.ccc);
	LOG_INFO("MAX DATA RATE: %ld\n", sd->csd.max_data_rate);
	LOG_INFO("WRITE BLK LEN BITS: %ld\n", sd->csd.write_blk_bits);
	LOG_INFO("READ BLK LEN BITS: %ld\n", sd->csd.read_blk_bits);
	LOG_INFO("ERASE SIZE: %ld\n", sd->csd.erase_size);
	LOG_INFO("BLK NUM: %ld\n", sd->csd.blk_num);
	LOG_INFO("BLK LEN: %ld\n", sd->csd.blk_len);
	LOG_INFO("CAPACITY %ld\n", sd->csd.capacity);
	LOG_INFO("FLAG Write: WP %d, W MISALIGN %d\n", sd->csd.write_partial, sd->csd.write_misalign);
	LOG_INFO("FLAG Read: RP %d, R MISALIGN %d\n", sd->csd.read_partial, sd->csd.read_misalign);

}

void sd_dumpCid(Sd *sd)
{
	ASSERT(sd);

	LOG_INFO("MANFID: %d\n", sd->cid.manfid);
    LOG_INFO("OEMID: %d\n", sd->cid.oemid);
	LOG_INFO("SERIAL: %ld\n", sd->cid.serial);
    LOG_INFO("PROD_NAME: %s\n", sd->cid.prod_name);
    LOG_INFO("REV: %d.%d\n", sd->cid.m_rev, sd->cid.l_rev);
    LOG_INFO("OFF,Y,M: %lx, %ld %ld\n", sd->cid.year_off, (BCD_TO_INT_32BIT(sd->cid.year_off) / 12) + 2000,
												(BCD_TO_INT_32BIT(sd->cid.year_off) % 12));
}

void sd_dumpSsr(Sd *sd)
{
	ASSERT(sd);

	LOG_INFO("BUS_WIDTH: %d\n", sd->ssr.bus_width);
    LOG_INFO("TYPE: %d\n", sd->ssr.card_type);
	LOG_INFO("AU_TYPE: %d\n", sd->ssr.au_size);
	LOG_INFO("ERASE_SIZE: %d\n", sd->ssr.erase_size);
	LOG_INFO("SPEED_CLASS: %d\n", sd->ssr.speed_class);
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

int sd_sendIfCond(void)
{
	if (hsmci_sendCmd(8, CMD8_V_RANGE_27V_36V, HSMCI_CMDR_RSPTYP_48_BIT))
	{
		LOG_ERR("IF_COND %lx\n", HSMCI_SR);
		return -1;
	}
	else
	{
		uint32_t r = HSMCI_RSPR;
		if ((r & 0xFFF) == CMD8_V_RANGE_27V_36V)
		{
			LOG_INFO("IF_COND: %lx\n", r);
			return 0;
		}
		LOG_ERR("IF_COND: %lx\n", r);
	}
	return -1;
}

int sd_sendAppOpCond(void)
{
	if (hsmci_sendCmd(55, 0, HSMCI_CMDR_RSPTYP_48_BIT))
	{
		LOG_ERR("APP_CMD %lx\n", HSMCI_SR);
		return -1;
	}
	else
	{
		LOG_INFO("APP_CMD %lx\n", HSMCI_RSPR);
	}

	if (hsmci_sendCmd(41, SD_HOST_VOLTAGE_RANGE | SD_OCR_CCS, HSMCI_CMDR_RSPTYP_48_BIT))// se cmd 8 va ok.
	{
		LOG_ERR("APP_OP_COND %lx\n", HSMCI_SR);
		return -1;
	}
	else
	{
		uint32_t status = HSMCI_RSPR;
		if (status & SD_OCR_BUSY)
		{
			LOG_INFO("SD power up! Hight Capability [%d]\n", (bool)(status & SD_OCR_CCS));
			return 0;
		}

		LOG_ERR("sd not ready.\n");
	}

	return -1;
}


int sd_getCid(Sd *sd, uint32_t addr, uint8_t flag)
{
	ASSERT(sd);
	memset(&(sd->cid), 0, sizeof(SDcid));

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

		sd->cid.manfid        = UNSTUFF_BITS(resp, 120, 8);
		sd->cid.oemid         = UNSTUFF_BITS(resp, 104, 16);
		sd->cid.prod_name[0]      = UNSTUFF_BITS(resp, 96, 8);
		sd->cid.prod_name[1]      = UNSTUFF_BITS(resp, 88, 8);
		sd->cid.prod_name[2]      = UNSTUFF_BITS(resp, 80, 8);
		sd->cid.prod_name[3]      = UNSTUFF_BITS(resp, 72, 8);
		sd->cid.prod_name[4]      = UNSTUFF_BITS(resp, 64, 8);
		sd->cid.m_rev         = UNSTUFF_BITS(resp, 60, 4);
		sd->cid.l_rev         = UNSTUFF_BITS(resp, 56, 4);
		sd->cid.serial        = (uint32_t)UNSTUFF_BITS(resp, 24, 32);
		sd->cid.year_off      = UNSTUFF_BITS(resp, 8, 12);
	}

	return 0;
}

int sd_getCsd(Sd *sd)
{
	ASSERT(sd);
	memset(&(sd->csd), 0, sizeof(SDcsd));

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
		sd_decodeCsd(&(sd->csd), resp, 4);
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

#define SD_STATUS_APP_CMD      BV(5)
#define SD_STATUS_READY        BV(8)
#define SD_STATUS_CURR_MASK    0x1E00
#define SD_STATUS_CURR_SHIFT   9

#define SD_GET_STATE(status)    (uint8_t)(((status) & SD_STATUS_CURR_MASK) >> SD_STATUS_CURR_SHIFT)

int sd_appStatus(Sd *sd)
{
	ASSERT(sd);
	LOG_INFO("Send to RCA: %lx\n", SD_ADDR_TO_RCA(sd->addr));
	if (hsmci_sendCmd(13, SD_ADDR_TO_RCA(sd->addr), HSMCI_CMDR_RSPTYP_48_BIT))
	{
		LOG_ERR("STATUS: %lx\n", HSMCI_SR);
		return -1;
	}

	hsmci_readResp(&(sd->status), 1);
	LOG_INFOB(dump("STATUS", &(sd->status), 1););

	LOG_INFO("State[%d]\n", SD_GET_STATE(sd->status));

	if (sd->status & SD_STATUS_READY)
		return 0;

	return -1;
}


INLINE int sd_cardSelection(Sd *sd, size_t rca)
{
	ASSERT(sd);
	LOG_INFO("Select RCA: %lx\n", SD_ADDR_TO_RCA(sd->addr));
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

int sd_selectCard(Sd *sd)
{
	return sd_cardSelection(sd, SD_ADDR_TO_RCA(sd->addr));
}

int sd_deSelectCard(Sd *sd)
{
	uint32_t rca = 0;
	if (!sd->addr)
		rca = SD_ADDR_TO_RCA(sd->addr + 1);

	return sd_cardSelection(sd, rca);
}


int sd_setBusWidth(Sd *sd, size_t len)
{
	ASSERT(sd);

	if (hsmci_sendCmd(55, SD_ADDR_TO_RCA(sd->addr), HSMCI_CMDR_RSPTYP_48_BIT))
	{
		LOG_ERR("APP_CMD %lx\n", HSMCI_SR);
		return -1;
	}

	uint32_t status = HSMCI_RSPR;
	if (status & (SD_STATUS_APP_CMD | SD_STATUS_READY))
	{
		hsmci_setBusWidth(len);

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
			return 0;
	}

	LOG_ERR("SET_BUS_WIDTH REP %lx\n", status);
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

	sd->csd.blk_len = len;

	if (sd->status & SD_STATUS_READY)
		return 0;

	return -1;
}

int sd_getStatus(Sd *sd, uint32_t *buf, size_t words)
{
	ASSERT(sd);

	// Status reply with 512bit data, so the block size in byte is 64
	hsmci_prgRxDMA(buf, words, 64);

	if (hsmci_sendCmd(55, SD_ADDR_TO_RCA(sd->addr), HSMCI_CMDR_RSPTYP_48_BIT))
	{
		LOG_ERR("APP_CMD %lx\n", HSMCI_SR);
		return -1;
	}

	uint32_t status = HSMCI_RSPR;
	if (status & (SD_STATUS_APP_CMD | SD_STATUS_READY))
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
			memset(&(sd->ssr), 0, sizeof(SDssr));

			sd->ssr.bus_width  = UNSTUFF_BITS(buf, 510, 2);
			sd->ssr.card_type  = UNSTUFF_BITS(buf, 480, 16);
			sd->ssr.au_size  = UNSTUFF_BITS(buf, 432, 8);
			sd->ssr.speed_class  = UNSTUFF_BITS(buf, 440, 8);
			sd->ssr.erase_size = UNSTUFF_BITS(buf, 408, 24);

			return 0;
		}
	}

	return -1;
}



int sd_readSingleBlock(Sd *sd, size_t index, uint32_t *buf, size_t words)
{
	ASSERT(sd);
	ASSERT(buf);

	hsmci_prgRxDMA(buf, words, sd->csd.blk_len);

	if (hsmci_sendCmd(17, index * sd->csd.blk_len, HSMCI_CMDR_RSPTYP_48_BIT |
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
		LOG_INFOB(dump("BLK", buf, words););

		return words;
	}

	return -1;
}



void sd_setHightSpeed(Sd *sd)
{
	hsmci_setSpeed(2100000, true);
}


#endif


bool sd_initUnbuf(Sd *sd, KFile *ch)
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

bool sd_initBuf(Sd *sd, KFile *ch)
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



