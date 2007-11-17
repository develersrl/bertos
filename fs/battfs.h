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
 * \version $Id:$
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief BattFS: a filesystem for embedded platforms (interface).
 * TODO: Add detailed filesystem description.
 */

#ifndef FS_BATTFS_H
#define FS_BATTFS_H

#include <cfg/compiler.h> // uintXX_t; STATIC_ASSERT
#include <algos/rotating_hash.h>

typedef uint16_t filled_t;
typedef uint16_t page_off_t;
typedef uint8_t  inode_t;
typedef uint8_t  seqnum_t;
typedef rotating_t battfs_crc_t;

typedef uint16_t battfs_page_t;

/**
 * BattFS page header.
 * \note fields order is chosen to comply
 * with CPU alignment.
 */
typedef struct BattFsPageHeader
{
	inode_t      inode;    ///< File inode (file identifier).
	seqnum_t     seqnum;   ///< bit[1:0]: Page sequence number; bit[7:2] unused for now, must be set to 1.
	filled_t     filled;   ///< Filled bytes in page.
	page_off_t   page_off; ///< Page offset inside file.
	battfs_crc_t crc;      ///< CRC of the page header.
} BattFsPageHeader;

/* Ensure structure has no padding added */
STATIC_ASSERT(sizeof(BattFsPageHeader) == sizeof(filled_t) + sizeof(page_off_t)
              + sizeof(inode_t) + sizeof(seqnum_t) + siezof(BattFsDisk));

/**
 * Reset page sequence number of struct \a m to default value (0xFF).
 */
#define RESET_SEQ(m) ((m).seqnum = 0xFF)

/**
 * Get page sequence number from struct \a m.
 */
#define SEQ(m) ((m).seqnum & 0x03)

/**
 * Set sequence number of struct \a m to \a d.
 */
#define SET_SEQ(m, d) ((m).seqnum = ((m).seqnum & 0xFC) | ((d) & 0x03))

/**
 * Increment sequence number of struct \a m.
 */
#define INC_SEQ(m) SET_SEQ((m), SEQ(m) + 1)

/* Fwd decl */
struct BattFsDisk;

/**
 * Type interface for disk init function.
 * \return true if all is ok, false otherwise.
 */
typedef bool (*disk_init_t) (struct BattFsDisk *d);

/**
 * Type interface for disk page read function.
 * \a page is the page address, \a size the lenght to be read.
 * \return the number of bytes read.
 */
typedef size_t (*disk_page_read_t) (struct BattFsDisk *d, void *buf, battfs_page_t page, size_t size);

/**
 * Type interface for disk page write function.
 * \a page is the page address, \a size the lenght to be written.
 * \return the number of bytes written.
 */
typedef size_t	(*disk_page_write_t) (struct BattFsDisk *d, const void *buf, battfs_page_t page, size_t size);

/**
 * Type interface for disk page erase function.
 * \a page is the page address.
 * \return true if all is ok, false otherwise.
 */
typedef bool (*disk_page_erase_t) (struct BattFsDisk *d, battfs_page_t page);

typedef uint32_t disk_size_t ///< Type for disk sizes.

/**
 * Context used to describe a disk.
 * This context structure will be used to access disk.
 * Must be initialized by hw memory driver.
 */
typedef struct BattFsDisk
{
	disk_init_t init;        ///< Disk init.
	disk_page_read_t  read;  ///< Page read.
	disk_page_write_t write; ///< Page write.
	disk_page_erase_t erase; ///< Page erase.

	disk_size_t disk_size;   ///< Size of the disk, in bytes.
	disk_size_t free_space;  ///< Free space on the disk, in bytes.
	/* TODO add other fields. */
} BattFsDisk;

#endif /* FS_BATTFS_H */
