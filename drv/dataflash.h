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
 *
 * -->
 *
 * \brief Function library for AT45DB081D Flash memory.
 *
 *
 * \version $Id: dflash.h 15402 2007-04-10 09:10:56Z asterix $
 * \author Daniele Basile <asterix@develer.com>
 */


#ifndef DATAFLASH_H
#define DATAFLASH_H

#include <kern/kfile.h>
#include <cfg/compiler.h>
#include <drv/spi.h>
#include "hw_spi.h"

#include <appconfig.h>

/**
 * Type definition for dflash memory.
 */
typedef uint32_t dflashAddr_t;
typedef uint32_t dflashSize_t;

#define RESET_PULSE_WIDTH     10 // Width of reset pulse in usec.
#define BUSY_BIT            0x80 // Select a busy bit in status register.
#define CMP_BIT             0x40 // Select a compare bit in status register.

/**
 * Select 2,3,4,5 bits of status register, those
 * bits indicate a id of density device (see datasheet for
 * more detail).
 */
#define GET_ID_DESITY_DEVICE(reg_stat)\
  do {\
		reg_stat &= 0x3C;\
		reg_stat >>= 2;\
  } while (0)

/**
 * Pin definition.
 *
 * \note RESET and WP are asserted when logic
 * level is low.
 * \{
 */
#define RESET               PC0   ///<  Connect to RESET pin of flash memory
#define WP                  PC1   ///<  Connect to WP pin of flash memory
#define DFLASH_PORT         PORTC ///<  Micro pin PORT register.
#define DFLASH_PIN          PINC  ///<  Micro pin PIN register.
#define DFLASH_DDR          DDRC  ///<  Micro pin DDR register.
/* \} */

/**
 * Pin logic level.
 *
 * \{
 */
#define RESET_LOW()      do { DFLASH_PORT &= ~BV(RESET); } while(0)
#define RESET_HIGH()     do { DFLASH_PORT |= BV(RESET); } while(0)
#define WP_LOW()         do { DFLASH_PORT &= ~BV(WP); } while(0)
#define WP_HIGH()        do { DFLASH_PORT |= BV(WP); } while(0)
/* \} */

/**
 * Commands pin.
 *
 * \note To reset flash memory it needs a pulse
 * long about 10 usec. To do this we insert a
 * for cycle.
 *
 * \{
 */
#define RESET_OUT()       do { DFLASH_DDR |= BV(RESET); } while(0)
#define WP_OUT()          do { DFLASH_DDR |= BV(WP); } while(0)
#define WRITE_ENABLE()    WP_HIGH()
#define WRITE_DISABLE()   WP_LOW()
#define RESET_ENABLE()    RESET_LOW()
#define RESET_DISABLE()   RESET_HIGH()
/* \} */


/**
 * Memory definition.
 *
 * \note Below are defined valid flash memory support to
 * this drive. Every time we call dflash_init() function we check
 * if memory defined are right (see dflash.c form more detail).
 * \{
 */
#define DFLASH_AT45DB041B         1
#define DFLASH_AT45DB081D         2
#define DFLASH_AT45DB161D         3

#if CONFIG_DATA_FLASH == DFLASH_AT45DB161D
	#define DFLASH_ID_DEVICE_DENSITY      0xb ///< This indicate AT45DB161D data flah memory.
	#define DFLASH_PAGE_SIZE              528 ///< Number of byte in one page.
	#define DFLASH_PAGE_ADDRESS_BIT        10 ///< Number bit for addressing one page.
	#define DFLASH_NUM_PAGE              4096 ///< Number page in data flash memory.
#elif CONFIG_DATA_FLASH == DFLASH_AT45DB081D
	#define DFLASH_ID_DEVICE_DENSITY      0x9  ///< This indicate AT45DB081D data flah memory.
	#define DFLASH_PAGE_SIZE              264  ///< Number of byte in one page.
	#define DFLASH_PAGE_ADDRESS_BIT         9  ///< Number bit for addressing one page.
	#define DFLASH_NUM_PAGE              4096  ///< Number page in data flash memory.
#elif CONFIG_DATA_FLASH == DFLASH_AT45DB041B
	#define DFLASH_ID_DEVICE_DENSITY      0x7  ///< This indicate AT45DB041B data flah memory.
	#define DFLASH_PAGE_SIZE              264  ///< Number of byte in one page.
	#define DFLASH_PAGE_ADDRESS_BIT         9  ///< Number bit for addressing one page.
	#define DFLASH_NUM_PAGE              2048  ///< Number page in data flash memory.
#else
	#error Nothing memory defined in CONFIG_DATA_FLASH are support.
#endif
/* \} */


/**
 * Data flash opcode commands.
 */
typedef enum {
	/**
	* Read commands data flash.
	* \{
	*/

#if CONFIG_DATA_FLASH == DFLASH_AT45DB081D || CONFIG_DATA_FLASH == AT45DB161D
	DFO_READ_FLASH_MEM_BYTE  = 0x0B, ///< Continuos array read.
#elif CONFIG_DATA_FLASH == DFLASH_AT45DB041B
	DFO_READ_FLASH_MEM_BYTE  = 0xE8, ///< Continuos array read.
#else
	#error No supported memory defined in CONFIG_DATA_FLASH.
#endif
	DFO_READ_FLASH_MEM       = 0xD2, ///< Main memory page read.
	DFO_READ_BUFF1           = 0xD4, ///< SRAM buffer 1 read.
	DFO_READ_BUFF2           = 0xD6, ///< SRAM buffer 2 read.
	/* \}*/

	/**
	* Program and erase commands data flash.
	* \{
	*/
	DFO_WRITE_BUFF1          =  0x84, ///< SRAM buffer 1 write.
	DFO_WRITE_BUFF2          =  0x87, ///< SRAM buffer 2 write.
	DFO_WRITE_BUFF1_TO_MEM_E =  0x83, ///< Buffer 1 to main memory page program with build-in erase.
	DFO_WRITE_BUFF2_TO_MEM_E =  0x86, ///< Buffer 2 to main memory page program with build-in erase.
	DFO_WRITE_BUFF1_TO_MEM   =  0x88, ///< Buffer 1 to main memory page program without build-in erase.
	DFO_WRITE_BUFF2_TO_MEM   =  0x89, ///< Buffer 2 to main memory page program without build-in erase.
	DFO_ERASE_PAGE           =  0x81, ///< Erase page.
	DFO_ERASE_BLOCK          =  0x50, ///< Erase block.
	DFO_ERASE_SECTOR         =  0x7C, ///< Erase sector.
	DFO_WRITE_MEM_TR_BUFF1   =  0x82, ///< Write main memory page program through buffer 1.
	DFO_WRITE_MEM_TR_BUFF2   =  0x85, ///< Write main memory page program through buffer 2.
	/* \}*/

	/**
	* Additional commands data flash.
	* \{
	*/
	DFO_MOV_MEM_TO_BUFF1     =  0x53, ///< Main mmemory to buffer 1 transfer.
	DFO_MOV_MEM_TO_BUFF2     =  0x55, ///< Main mmemory to buffer 2 transfer.
	DFO_CMP_MEM_TO_BUFF1     =  0x60, ///< Main mmemory to buffer 1 compare.
	DFO_CMP_MEM_TO_BUFF2     =  0x61, ///< Main mmemory to buffer 2 compare.
	DFO_ARW_MEM_TR_BUFF1     =  0x58, ///< Auto page rewrite through buffer 1.
	DFO_ARW_MEM_TR_BUFF2     =  0x59, ///< Auto page rewrite through buffer 2
	DFO_PWR_DOWN             =  0xB9, ///< Deep power-down.
	DFO_RESUME_PWR_DOWN      =  0xAB, ///< Resume from deep power-down.
	DFO_READ_STATUS          =  0xD7, ///< Status register read.
	DFO_ID_DEV               =  0x9F  ///< Manufacturer and device ID read.
	/* \}*/
} DFlashOpcode;

void dflash_init(struct _KFile *fd)

#endif /* DATAFLASH_H */

