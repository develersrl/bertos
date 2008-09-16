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
 * \version $Id$
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
#include <struct/list.h>
#include <kern/kfile.h>

typedef uint16_t fill_t;    ///< Type for keeping trace of space filled inside a page
typedef fill_t   pgaddr_t;  ///< Type for addressing space inside a page
typedef uint16_t pgcnt_t;   ///< Type for counting pages on disk
typedef pgcnt_t  pgoff_t;   ///< Type for counting pages inside a file
typedef uint8_t  inode_t;   ///< Type for file inodes
typedef uint64_t  seq_t;    ///< Type for page seq number, at least 40bits wide.
typedef rotating_t fcs_t;   ///< Type for header FCS.


/**
 * BattFS page header, used to represent a page
 * header in memory.
 * To see how this is stored on disk:
 * \see battfs_to_disk
 * \see disk_to_battfs
 */
typedef struct BattFsPageHeader
{
	inode_t  inode; ///< File inode (file identifier).
	fill_t   fill;  ///< Filled bytes in page.
	pgoff_t  pgoff; ///< Page offset inside file.

	/**
	 * Page sequence number.
	 * Every time a page is rewritten the seq number is
	 * increased by 1. seq_t is wide enough to not to perform
	 * a wrap around before the memory death.
	 * So it can be kept as it would be
	 * monotonically increasing for our needs.
	 */
	seq_t    seq;

	/**
	 * FCS (Frame Check Sequence) of the page header.
	 */
	fcs_t fcs;
} BattFsPageHeader;

/**
 * Size of the header once saved on disk.
 * \see battfs_to_disk
 * \see disk_to_battfs
 */
#define BATTFS_HEADER_LEN 12

/**
 * Maximum page address.
 */
#define MAX_PAGE_ADDR ((1 << (CPU_BITS_PER_CHAR * sizeof(pgcnt_t))) - 1)

/**
 * Max number of files.
 */
#define BATTFS_MAX_FILES (1 << (CPU_BITS_PER_CHAR * sizeof(inode_t)))

/* Fwd decl */
struct BattFsSuper;

/**
 * Sentinel used to keep trace of unset pages in disk->page_array.
 */
#define PAGE_UNSET_SENTINEL ((1 << (CPU_BITS_PER_CHAR * sizeof(pgcnt_t))) - 1)

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
typedef size_t	(*disk_page_write_t) (struct BattFsSuper *d, pgcnt_t page, pgaddr_t addr, const void *buf, size_t);

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

	/**
	 * Lowest address, in page array, for free pages.
	 * Pages above this element are free for use.
	 */
	pgcnt_t free_page_start;

	disk_size_t disk_size;   ///< Size of the disk, in bytes (page_count * page_size).
	disk_size_t free_bytes;  ///< Free space on the disk.

	List file_opened_list;       ///< List used to keep trace of open files.
	/* TODO add other fields. */
} BattFsSuper;

typedef uint8_t filemode_t;  ///< Type for file open modes.
typedef int32_t file_size_t; ///< Type for file sizes.

/**
 * Modes for battfs_fileopen.
 * \{
 */
#define BATTFS_CREATE BV(0)  ///< Create file if does not exist
#define BATTFS_RD     BV(1)  ///< Open file for reading
#define BATTFS_WR     BV(2)  ///< Open file fir writing
/*/}*/


/**
 * Describe a BattFs file usign a KFile.
 */
typedef struct BattFs
{
	KFile fd;           ///< KFile context
	Node link;          ///< Link for inserting in opened file list
	inode_t inode;      ///< inode of the opened file
	BattFsSuper *disk;  ///< Disk context
	filemode_t mode;    ///< File open mode
	pgcnt_t *start;     ///< Pointer to page_array file start position.
} BattFs;

/**
 * Id for battfs file descriptors.
 */
#define KFT_BATTFS MAKE_ID('B', 'T', 'F', 'S')

/**
 * Macro used to cast a KFile to a BattFS.
 * Also perform dynamic type check.
 */
INLINE BattFs * BATTFS_CAST(KFile *fd)
{
	ASSERT(fd->_type == KFT_BATTFS);
	return (BattFs *)fd;
}

bool battfs_init(struct BattFsSuper *d);
bool battfs_close(struct BattFsSuper *disk);

bool battfs_fileExists(BattFsSuper *disk, inode_t inode);
bool battfs_fileopen(BattFsSuper *disk, BattFs *fd, inode_t inode, filemode_t mode);
bool battfs_writeTestBlock(struct BattFsSuper *disk, pgcnt_t page, inode_t inode, seq_t seq, fill_t fill, pgoff_t pgoff);
#endif /* FS_BATTFS_H */
