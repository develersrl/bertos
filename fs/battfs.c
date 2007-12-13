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


#include <string.h> /* memset, memmove */

/**
 * Convert mark_t from cpu endianess to filesystem endianness.
 * \note filesystem is in little-endian format.
 */
INLINE mark_t cpu_to_mark_t(mark_t mark)
{
	STATIC_ASSERT(sizeof(mark_t) == 2);
	return cpu_to_le16(mark);
}

/**
 * Convert mark_t from filesystem endianness to cpu endianess.
 * \note filesystem is in little-endian format.
 */
INLINE mark_t mark_t_to_cpu(mark_t mark)
{
	STATIC_ASSERT(sizeof(mark_t) == 2);
	return le16_to_cpu(mark);
}

/**
 * Convert from cpu endianess to filesystem endianness.
 * \note filesystem is in little-endian format.
 */
INLINE void cpu_to_battfs(struct BattFsPageHeader *hdr)
{
	STATIC_ASSERT(sizeof(hdr->inode) == 1);
	STATIC_ASSERT(sizeof(hdr->seq) == 1);

	hdr->mark = cpu_to_mark_t(hdr->mark);

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

	hdr->mark = mark_t_to_cpu(hdr->mark);

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

	return true;
}

/**
 * Count the number of pages from
 * inode 0 to \a inode in \a filelen_table.
 */
static pgcnt_t countPages(pgoff_t *filelen_table, inode_t inode)
{
	pgcnt_t cnt = 0;

	for (inode_t i = 0; i < inode; i++)
		cnt += filelen_table[i];

	return cnt;
}

/**
 * Move all pages in page allocation array from \a src to \a src + \a offset.
 * The number of pages moved is page_count - MAX(dst, src).
 */
static void movePages(struct BattFsSuper *disk, pgcnt_t src, int offset)
{
	pgcnt_t dst = src + offset;
	memmove(&disk->page_array[dst], &disk->page_array[src], disk->page_count - MAX(dst, src) * sizeof(pgcnt_t));
	
	if (offset < 0)
	{
		/* Fill empty space in array with sentinel */
		for (pgcnt_t page = disk->page_count + offset; page < disk->page_count; page++)
			disk->page_array[page] = PAGE_UNSET_SENTINEL;
	}
}

/**
 * Insert \a page into page allocation array of \a disk, using \a filelen_table and
 * \a free_number to compure position.
 */
static void insertFreePage(struct BattFsSuper *disk, pgoff_t *filelen_table, mark_t free_number, pgcnt_t page)
{
	ASSERT(mark >= disk->min_free);
	ASSERT(mark <= disk->max_free);

	pgcnt_t free_pos = countPages(filelen_table, BATTFS_MAX_FILES - 1);
	free_pos += free_number - disk->min_free;
	ASSERT(disk->page_array[free_pos] == PAGE_UNSET_SENTINEL);
	disk->page_array[free_pos] = page;
}

/**
 * Mark \a page of \a disk as free.
 * \note max_free of \a disk is increased by 1 and is used as
 *       \a page free marker.
 */
static bool battfs_markFree(struct BattFsSuper *disk, pgcnt_t page)
{
	pgaddr_t addr = disk->page_size - sizeof(BattFsPageHeader) + offsetof(BattFsPageHeader, mark);
	mark_t mark = cpu_to_mark_t(++disk->max_free);
	if (!disk->write(disk, page, addr, &mark, sizeof(mark)))
	{
		TRACEMSG("error marking page [%d]\n", page);
		return false;
	}
	else
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

	/* Sanity checks */
	ASSERT(disk->open);
	ASSERT(disk->read);
	ASSERT(disk->write);
	ASSERT(disk->erase);
	ASSERT(disk->close);
	ASSERT(disk->page_size);
	ASSERT(disk->page_count);
	ASSERT(disk->page_count < PAGE_UNSET_SENTINEL - 1);
	ASSERT(disk->page_array);
	
	/* Init disk device */
	if (!disk->open(disk))
	{
		TRACEMSG("open error\n");
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
			/* Page is valid and is owned by a file */
			filelen_table[hdr.inode]++;

			ASSERT(hdr.mark == MARK_PAGE_VALID);
			ASSERT(hdr.fill <= disk->page_size - sizeof(BattFsPageHeader));
			/* Keep trace of free space */
			disk->free_bytes += disk->page_size - sizeof(BattFsPageHeader) - hdr.fill;
		}
		else
		{
			/* Increase free space */
			disk->free_bytes += disk->page_size - sizeof(BattFsPageHeader);
			
			/* Check if putting mark to MARK_PAGE_VALID makes fcs correct */
			mark_t old_mark = hdr.mark;
			hdr.mark = MARK_PAGE_VALID;
			rotating_init(&cks);
			rotating_update(&hdr, sizeof(BattFsPageHeader) - sizeof(rotating_t), &cks);
			if (cks == hdr.fcs)
			{
				/*
				 * This page is a valid and marked free page.
				 * Update min and max free page sequence numbers.
				 */
				disk->min_free = MIN(disk->min_free, old_mark);
				disk->max_free = MAX(disk->max_free, old_mark);
			}
			else
				TRACEMSG("page [%d] invalid, keeping as free\n", page);
		}
	}

	/* Once here, we have filelen_table filled with file lengths */

	/* Fill page array with sentinel */
	for (pgcnt_t page = 0; page < disk->page_count; page++)
		disk->page_array[page] = PAGE_UNSET_SENTINEL;

	/* Fill page allocation array */
	for (pgcnt_t page = 0; page < disk->page_count; page++)
	{
		if (!battfs_readHeader(disk, page, &hdr))
			return false;

		/* Check header FCS */
		rotating_init(&cks);
		rotating_update(&hdr, sizeof(BattFsPageHeader) - sizeof(rotating_t), &cks);
		if (cks == hdr.fcs)
		{
			/* Page is valid and is owned by a file */
			ASSERT(hdr.mark == MARK_PAGE_VALID);

			/* Compute array position */
			pgcnt_t array_pos = countPages(filelen_table, hdr.inode);
			array_pos += hdr.pgoff;

			/* Check if position is already used by another page of the same file */
			if (disk->page_array[array_pos] == PAGE_UNSET_SENTINEL)
				disk->page_array[array_pos] = page;
			else
			{
				BattFsPageHeader hdr_old;
				
				if (!battfs_readHeader(disk, page, &hdr_old))
					return false;

				#ifdef _DEBUG
				/* Check header FCS */
				rotating_t cks_old;
				rotating_init(&cks_old);
				rotating_update(&hdr_old, sizeof(BattFsPageHeader) - sizeof(rotating_t), &cks_old);
				ASSERT(cks_old == hdr_old.fcs);
				#endif

				/* Only the very same page with a different seq number can be here */
				ASSERT(hdr.inode == hdr_old.inode);
				ASSERT(hdr.pgoff == hdr_old.pgoff);
				ASSERT(hdr.mark == hdr_old.mark);
				ASSERT(hdr.seq != hdr_old.seq);

				pgcnt_t new_page, old_page;
				fill_t old_fill;

				if (hdr.seq > hdr_old.seq)
				{
					/* Actual header is newer than the previuos one */
					old_page = disk->page_array[array_pos];
					new_page = page;
					old_fill = hdr_old.fill;
				}
				else
				{
					/* Previous header is newer than the current one */
					old_page = page;
					new_page = disk->page_array[array_pos];
					old_fill = hdr.fill;
				}

				/* Set new page */
				disk->page_array[array_pos] = new_page;

				/* Add free space */
				disk->free_bytes -= disk->page_size - sizeof(BattFsPageHeader) - old_fill;

				/* Shift all array one position to the left, overwriting duplicate page */
				array_pos -= hdr.pgoff;
				array_pos += filelen_table[hdr.inode];
				movePages(disk, array_pos, -1);
				
				/* Decrease file page count */
				filelen_table[hdr.inode]--;

				/* Add old page to free pages pool */
				if (!battfs_markFree(disk, old_page))
					return false;

				insertFreePage(disk, filelen_table, disk->max_free, old_page);
			}
		}
		else
		{
			/* Check if putting mark to MARK_PAGE_VALID makes fcs correct */
			mark_t old_mark = hdr.mark;
			hdr.mark = MARK_PAGE_VALID;
			rotating_init(&cks);
			rotating_update(&hdr, sizeof(BattFsPageHeader) - sizeof(rotating_t), &cks);
			if (cks == hdr.fcs)
				/* Page is a valid marked page, insert in free list in correct order */
				insertFreePage(disk, filelen_table, old_mark, page);
			else
				/* Page is not a valid marked page, insert at the end of list */
				insertFreePage(disk, filelen_table, ++disk->max_free, page);
		}
	}

	#warning Test me!	
	return true;	
}


