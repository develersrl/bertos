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
#include <cpu/types.h> // CPU_BITS_PER_CHAR
#include <algo/rotating_hash.h>

typedef uint16_t fill_t;
typedef fill_t   pgaddr_t;
typedef uint16_t pgoff_t;
typedef pgoff_t  mark_t;
typedef uint8_t  inode_t;
typedef uint8_t  seq_t;
typedef rotating_t fcs_t;

/**
 * BattFS page header.
 * \note fields order is chosen to comply
 * with CPU alignment.
 */
typedef struct BattFsPageHeader
{
	inode_t  inode; ///< File inode (file identifier).
	seq_t    seq;   ///< Page sequence number.
	mark_t   mark;  ///< Marker used to keep trace of free/used pages.
	pgoff_t  pgoff; ///< Page offset inside file.
	fill_t   fill;  ///< Filled bytes in page.
	uint16_t rfu;   ///< Reserved for future use, 0xFFFF for now.

	/**
	 * FCS (Frame Check Sequence) of the page header.
	 * \note This field must be the last one!
	 *       This is needed because if the page is only partially
	 *       written, we can use this to detect it.
	 */
	fcs_t fcs;
} BattFsPageHeader;
/* Ensure structure has no padding added */
STATIC_ASSERT(sizeof(BattFsPageHeader) == 12);


/**
 * Mark for valid pages.
 * Simply set to 1 all field bits.
 */
#define MARK_PAGE_VALID ((1 << (CPU_BITS_PER_CHAR * sizeof(mark_t))) - 1)

/**
 * Max number of files.
 */
#define BATTFS_MAX_FILES (1 << (CPU_BITS_PER_CHAR * sizeof(inode_t)))

/**
 * Special inode used to identify free pages.
 */
#define BATTFS_FREE_INODE (BATTFS_MAX_FILES - 1)

/* Fwd decl */
struct BattFsSuper;

/**
 * Type for disk page addressing.
 */
typedef uint16_t pgcnt_t;

/**
 * Type interface for disk init function.
 * \return true if all is ok, false otherwise.
 */
typedef bool (*disk_open_t) (struct BattFsSuper *d);

/**
 * Type interface for disk page read function.
 * \a page is the page address, \a addr the address inside the page,
 * \a size the lenght to be read.
 * \return the number of bytes read.
 */
typedef size_t (*disk_page_read_t) (struct BattFsSuper *d, pgcnt_t page, pgaddr_t addr, void *buf, size_t);

/**
 * Type interface for disk page write function.
 * \a page is the page address, \a addr the address inside the page,
 * \a size the lenght to be written.
 * \return the number of bytes written.
 */
typedef size_t	(*disk_page_write_t) (struct BattFsSuper *d, pgcnt_t page, pgaddr_t addr, void *buf, size_t);

/**
 * Type interface for disk page erase function.
 * \a page is the page address.
 * \return true if all is ok, false otherwise.
 */
typedef bool (*disk_page_erase_t) (struct BattFsSuper *d, pgcnt_t page);

/**
 * Type interface for disk deinit function.
 * \return true if all is ok, false otherwise.
 */
typedef bool (*disk_close_t) (struct BattFsSuper *d);


typedef uint32_t disk_size_t; ///< Type for disk sizes.

/**
 * Context used to describe a disk.
 * This context structure will be used to access disk.
 * Must be initialized by hw memory driver.
 */
typedef struct BattFsSuper
{
	disk_open_t open;        ///< Disk init.
	disk_page_read_t  read;  ///< Page read.
	disk_page_write_t write; ///< Page write.
	disk_page_erase_t erase; ///< Page erase.
	disk_close_t close;      ///< Disk deinit.

	pgaddr_t page_size;      ///< Size of a disk page, in bytes.
	pgcnt_t page_count;      ///< Number of pages on disk.

	/**
	 * Page allocation array.
	 * This array must be allocated somewhere and
	 * must have enough space for page_count elements.
	 * Is used by the filesystem to represent
	 * the entire disk in memory.
	 */
	pgcnt_t *page_array;

	mark_t min_free; ///< Lowest sequence number of free page
	mark_t max_free; ///< Highest sequence number of free page

	disk_size_t disk_size;   ///< Size of the disk, in bytes (page_count * page_size).
	disk_size_t free_bytes;  ///< Free space on the disk.
	
	/* TODO add other fields. */
} BattFsSuper;

bool battfs_init(struct BattFsSuper *d);

#endif /* FS_BATTFS_H */
