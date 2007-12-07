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
 * \brief BattFS: a filesystem for embedded platforms (implementation).
 */

#include "battfs.h"

#include <cfg/debug.h>
#include <cfg/macros.h> /* MIN, MAX */
#include <mware/byteorder.h> /* cpu_to_xx */


#include <string.h> /* memset */

/**
 * Convert from cpu endianess to filesystem endianness.
 * \note filesystem is in little-endian format.
 */
INLINE void cpu_to_battfs(struct BattFsPageHeader *hdr)
{
	STATIC_ASSERT(sizeof(hdr->inode) == 1);
	STATIC_ASSERT(sizeof(hdr->seq) == 1);

	STATIC_ASSERT(sizeof(hdr->mark) == 2);
	hdr->mark = cpu_to_le16(hdr->mark);

	STATIC_ASSERT(sizeof(hdr->fill) == 2);
	hdr->fill = cpu_to_le16(hdr->fill);

	STATIC_ASSERT(sizeof(hdr->pgoff) == 2);
	hdr->pgoff = cpu_to_le16(hdr->pgoff);

	STATIC_ASSERT(sizeof(hdr->fcs) == 2);
	hdr->fcs = cpu_to_le16(hdr->fcs);

	STATIC_ASSERT(sizeof(hdr->rfu) == 2);
	hdr->rfu = cpu_to_le16(hdr->rfu);
}


/**
 * Convert from filesystem endianness to cpu endianess.
 * \note filesystem is in little-endian format.
 */
INLINE void battfs_to_cpu(struct BattFsPageHeader *hdr)
{
	STATIC_ASSERT(sizeof(hdr->inode) == 1);
	STATIC_ASSERT(sizeof(hdr->seq) == 1);

	STATIC_ASSERT(sizeof(hdr->mark) == 2);
	hdr->mark = le16_to_cpu(hdr->mark);

	STATIC_ASSERT(sizeof(hdr->fill) == 2);
	hdr->fill = le16_to_cpu(hdr->fill);

	STATIC_ASSERT(sizeof(hdr->pgoff) == 2);
	hdr->pgoff = le16_to_cpu(hdr->pgoff);

	STATIC_ASSERT(sizeof(hdr->fcs) == 2);
	hdr->fcs = le16_to_cpu(hdr->fcs);

	STATIC_ASSERT(sizeof(hdr->rfu) == 2);
	hdr->rfu = le16_to_cpu(hdr->rfu);
}


/**
 * Read header of page \a page.
 * \return true on success, false otherwise.
 * \note \a hdr is dirtyed even on errors.
 */
static bool battfs_readHeader(struct BattFsSuper *disk, pgcnt_t page, struct BattFsPageHeader *hdr)
{
	/*
	 * Read header from disk.
	 * header is actually a footer, and so
	 * resides at page end.
	 */
	if (disk->read(disk, page, disk->page_size - sizeof(BattFsPageHeader), hdr, sizeof(BattFsPageHeader))
	    != sizeof(BattFsPageHeader))
	{
		TRACEMSG("Error: page[%d]\n", page);
		return false;
	}

	/* Fix endianess */
	battfs_to_cpu(hdr);

	ASSERT(hdr->fill <= disk->page_size - sizeof(BattFsPageHeader));
	return true;
}

/**
 * Initialize and mount disk described by
 * \a d.
 * \return false on errors, true otherwise.
 */
bool battfs_init(struct BattFsSuper *disk)
{
	BattFsPageHeader hdr;
	rotating_t cks;
	pgoff_t filelen_table[BATTFS_MAX_FILES];

	/* Init disk device */
	if (!disk->open(disk))
	{
		TRACEMSG("Open error\n");
		return false;
	}

	memset(filelen_table, 0, BATTFS_MAX_FILES * sizeof(pgoff_t));

	/* Initialize min free sequence number to max value */
	disk->min_free = MARK_PAGE_VALID;
	/* Initialize max free sequence number to min value */
	disk->max_free = 0;

	disk->free_bytes = 0;
	disk->disk_size = (disk_size_t)(disk->page_size - sizeof(BattFsPageHeader)) * disk->page_count;

	/* Count the number of disk page per files */
	for (pgcnt_t page = 0; page < disk->page_count; page++)
	{
		if (!battfs_readHeader(disk, page, &hdr))
			return false;

		/* Check header FCS */
		rotating_init(&cks);
		rotating_update(&hdr, sizeof(BattFsPageHeader) - sizeof(rotating_t), &cks);
		if (cks == hdr.fcs)
		{
			/* Page is valid */
			filelen_table[hdr.inode]++;

			/* Keep trace of free space */
			disk->free_bytes += disk->page_size - sizeof(BattFsPageHeader) - hdr.fill;
		}
		else
		{
			/* Check if putting mark to MARK_PAGE_VALID makes fcs correct */
			mark_t old_mark = hdr.mark;
			hdr.mark = MARK_PAGE_VALID;
			rotating_init(&cks);
			rotating_update(&hdr, sizeof(BattFsPageHeader) - sizeof(rotating_t), &cks);
			if (cks == hdr.fcs)
			{
				/*
				 * This page is a valid free page.
				 * Update min and max free page sequence numbers.
				 */
				disk->min_free = MIN(disk->min_free, old_mark);
				disk->max_free = MAX(disk->max_free, old_mark);
			}
			else
				TRACEMSG("Page [%d] invalid, keeping as free\n", page);

			/* Increase free space */
			filelen_table[BATTFS_FREE_INODE]++;
			disk->free_bytes += disk->page_size - sizeof(BattFsPageHeader);
		}
	}

	/* Once here, we have filelen_table filled with file lengths */
	#warning Complete me!

	
	return true;	
}


