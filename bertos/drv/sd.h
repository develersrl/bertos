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
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "sd"
 * $WIZ$ module_depends = "kfile", "timer", "kblock", "sd_spi"
 * $WIZ$ module_hw = "bertos/hw/hw_sd.h"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_sd.h"
 */


#ifndef DRV_SD_H
#define DRV_SD_H

#include "cfg/cfg_sd.h"

#include <io/kfile.h>
#include <io/kblock.h>

#include <fs/fatfs/diskio.h>

#if CPU_CM3_SAM3X8

typedef struct SDcid
{
    uint8_t        manfid;
    uint8_t        prod_name[8];
    uint32_t       serial;
    uint16_t       oemid;
    uint32_t       year_off;
    uint8_t        m_rev;
    uint8_t        l_rev;
}SDcid;

typedef struct SDcsd
{
	uint8_t     structure;
    uint8_t     ccc;          ///< Card command classes
    uint32_t    erase_size;  ///< The size of an erasable sector, in write block len
	uint32_t    capacity;     ///< Card size in byte
	uint32_t    max_data_rate; ///< Step rate, usec
    uint32_t    blk_len;      ///< Block data size len in byte
	uint32_t    blk_num;      ///< Number of block in card
	uint32_t  	write_blk_bits; ///< Max write block length in bits
	uint32_t  	read_blk_bits;  ///< Max read block length in bits
    uint8_t     read_partial:1,
                read_misalign:1,
                write_partial:1,
                write_misalign:1;
} SDcsd;

typedef struct SDssr
{
	uint8_t    bus_width;
	uint8_t    card_type;
	uint8_t    speed_class;
	uint8_t    au_size;
	uint8_t    erase_size;
} SDssr;


#define SD_READY_FOR_DATA         BV(8)
#define SD_CARD_IS_LOCKED         BV(25)

#define SD_SEND_CID_RCA     0
#define SD_SEND_ALL_CID   BV(0)

#endif

#define SD_START_DELAY  10
#define SD_INIT_TIMEOUT ms_to_ticks(2000)
#define SD_IDLE_RETRIES 4
#define SD_DEFAULT_BLOCKLEN 512

/**
 * $WIZ$ sd_mode = "SD_SDMMC_MODE", "SD_SPI_MODE"
 * \{
 */
#define SD_SDMMC_MODE  0
#define SD_SPI_MODE    1
/** \} */

#define SD_UNBUFFERED     BV(0) ///< Open SD memory disabling page caching, no modification and partial write are allowed.

/**
 * SD Card context structure.
 */
typedef struct Sd
{

	KBlock b;   ///< KBlock base class
	KFile *ch;  ///< SPI communication channel
	uint16_t r1;  ///< Last status data received from SD
	uint16_t tranfer_len; ///< Lenght for the read/write commands, cached in order to increase speed.

	#if CPU_CM3_SAM3X8
	SDcid cid;
	SDcsd csd;
	SDssr ssr;
	uint32_t addr;
	uint32_t status;
	#endif

} Sd;

bool sd_hw_initUnbuf(Sd *sd, KFile *ch);
bool sd_hw_initBuf(Sd *sd, KFile *ch);

bool sd_spi_initUnbuf(Sd *sd, KFile *ch);
bool sd_spi_initBuf(Sd *sd, KFile *ch);

#if CPU_CM3_SAM3X8

void sd_dumpCsd(Sd *sd);
void sd_dumpCid(Sd *sd);
void sd_dumpSsr(Sd *sd);

void sd_sendInit(void);
void sd_goIdle(void);
int sd_sendIfCond(Sd *sd);
int sd_sendAppOpCond(Sd *sd);

int sd_getCid(Sd *sd, uint32_t addr, uint8_t flag);
int sd_getCsd(Sd *sd);
int sd_getSrc(Sd *sd);

int sd_appStatus(Sd *sd);
int sd_getRelativeAddr(Sd *sd);

int sd_getStatus(Sd *sd, uint32_t *buf, size_t words);

int sd_selectCard(Sd *sd);
int sd_deSelectCard(Sd *sd);
int sd_setBusWidth(Sd *sd, size_t len);
int sd_set_BlockLen(Sd *sd, size_t len);
void sd_setHightSpeed(Sd *sd);
int sd_readSingleBlock(Sd *sd, size_t index, uint32_t *buf, size_t words);
int sd_writeSingleBlock(Sd *sd, size_t index, uint32_t *buf, size_t words);


INLINE int sd_setBus4bit(Sd *sd)
{
	return sd_setBusWidth(sd, 4);
}

INLINE int sd_setBus1bit(Sd *sd)
{
	return sd_setBusWidth(sd, 1);
}

#endif

// For old compatibility.
#ifndef CONFIG_SD_MODE
	#define CONFIG_SD_MODE  SD_SPI_MODE
	#define SD_INCLUDE_SPI_SOURCE
#endif

#if CONFIG_SD_OLD_INIT

	#if !(ARCH & ARCH_NIGHTTEST)
		#warning "Deprecated: this API will be removed in the next major release,"
		#warning "please disable CONFIG_SD_OLD_INIT and pass explicitly the SD context to sd_init()."
	#endif

	/**
	 * Initializes the SD driver.
	 *
	 * \param ch A pointer to a SPI channel where the SD will read/write to.
	 *
	 * \return true if initialization succeds, false otherwise.
	 *
	 * \note This API is deprecated, disable CONFIG_SD_OLD_INIT and
	 *       use the new one instead.
	 *
	 * \see CONFIG_SD_OLD_INIT.
	 */
	#if CONFIG_SD_MODE == SD_SPI_MODE
		#define sd_init(ch) {static struct Sd sd; sd_spi_initUnbuf(&sd, (ch));}
	#else
		#define sd_init(ch) {static struct Sd sd; sd_hw_initUnbuf(&sd, (ch));}
	#endif

#else

	/**
	 * Initializes the SD driver.
	 *
	 * \param sd The SD KBlock context.
	 * \param ch A pointer to a SPI channel where the SD will read/write to.
	 * \param buffered Set to true if you want the KBlock to be buffered,
	 *        to false otherwise. The FatFs module does not require the device
	 *        to be buffered because it has an internal cache. This will save
	 *        512 bytes of RAM in this case.
	 *
	 * \return true if initialization succeds, false otherwise.
	 */
	#if CONFIG_SD_MODE == SD_SPI_MODE
		#define sd_init(sd, ch, buffered) ((buffered & SD_UNBUFFERED) ? sd_spi_initUnbuf((sd), (ch)) : sd_spi_initBuf((sd), (ch)))
	#else
		#define sd_init(sd, ch, buffered) ((buffered & SD_UNBUFFERED) ? sd_hw_initUnbuf((sd), (ch)) : sd_hw_initBuf((sd), (ch)))
	#endif

#endif


#define KBT_SD MAKE_ID('S', 'D', 'B', 'K')

bool sd_test(Sd *sd);
void sd_writeTest(Sd *sd);

INLINE Sd *SD_CAST(KBlock *b)
{
	ASSERT(b->priv.type == KBT_SD);
	return (Sd *)b;
}

#endif /* DRV_SD_H */

