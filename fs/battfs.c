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
 * Convert from memory representation to disk structure.
 * \note filesystem is in little-endian format.
 */
INLINE void battfs_to_disk(struct BattFsPageHeader *hdr, uint8_t *buf)
{
	STATIC_ASSERT(BATTFS_HEADER_LEN == 13);
	buf[0] = hdr->inode;

	buf[1] = hdr->seq;

	buf[2] = hdr->fill;
	buf[3] = hdr->fill >> 8;

	buf[4] = hdr->pgoff;
	buf[5] = hdr->pgoff >> 8;

	/*
	 * Mark is at least 1 bit longer than page address.
	 * Needed to take care of wraparonds.
	 */
	buf[6] = hdr->mark;
	buf[7] = hdr->mark >> 8;
	buf[8] = hdr->mark >> 16;

	/*
	 * This field must be the before the last one!
	 */
	buf[9] = hdr->fcs_free;
	buf[10] = hdr->fcs_free >> 8;

	/*
	 * This field must be the last one!
	 * This is needed because if the page is only partially
	 * written, we can use this to detect it.
	 */
	buf[11] = hdr->fcs;
	buf[12] = hdr->fcs >> 8;
}

/**
 * Convert from disk structure to memory representation.
 * \note filesystem is in little-endian format.
 */
INLINE void disk_to_battfs(uint8_t *buf, struct BattFsPageHeader *hdr)
{
	STATIC_ASSERT(BATTFS_HEADER_LEN == 13);
	hdr->inode = buf[0];
	hdr->seq = buf[1];
	hdr->fill = buf[3] << 8 | buf[2];
	hdr->pgoff = buf[5] << 8 | buf[4];
	hdr->mark = (mark_t)buf[8] << 16 | buf[7] << 8 | buf[6];
	hdr->fcs_free = buf[10] << 8 | buf[9];
	hdr->fcs = buf[12] << 8 | buf[11];
}

/**
 * Compute the fcs of the header.
 */
static fcs_t computeFcs(struct BattFsPageHeader *hdr)
{
	uint8_t buf[BATTFS_HEADER_LEN];
	fcs_t cks;

	battfs_to_disk(hdr, buf);
	rotating_init(&cks);
	/* fcs is at the end of whole header */
	rotating_update(buf,BATTFS_HEADER_LEN - sizeof(fcs_t), &cks);
	return cks;
}

/**
 * Compute the fcs of the header marked as free.
 */
static fcs_t computeFcsFree(struct BattFsPageHeader *hdr)
{
	uint8_t buf[BATTFS_HEADER_LEN];
	fcs_t cks;

	battfs_to_disk(hdr, buf);
	rotating_init(&cks);
	/* fcs_free is just before fcs of whole header */
	rotating_update(buf,BATTFS_HEADER_LEN - 2 * sizeof(fcs_t), &cks);
	return cks;
}


/**
 * Read header of page \a page.
 * \return true on success, false otherwise.
 * \note \a hdr is dirtyed even on errors.
 */
static bool battfs_readHeader(struct BattFsSuper *disk, pgcnt_t page, struct BattFsPageHeader *hdr)
{
	uint8_t buf[BATTFS_HEADER_LEN];
	/*
	 * Read header from disk.
	 * Header is actually a footer, and so
	 * resides at page end.
	 */
	if (disk->read(disk, page, disk->page_size - BATTFS_HEADER_LEN - 1, buf, BATTFS_HEADER_LEN)
	    != BATTFS_HEADER_LEN)
	{
		TRACEMSG("Error: page[%d]\n", page);
		return false;
	}

	/* Fill header */
	disk_to_battfs(buf, hdr);

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
 * \a free_number to compute position.
 */
static void insertFreePage(struct BattFsSuper *disk, pgoff_t *filelen_table, mark_t mark, pgcnt_t page)
{
	ASSERT(mark >= disk->free_start);
	ASSERT(mark < disk->free_next);

	pgcnt_t free_pos = countPages(filelen_table, BATTFS_MAX_FILES - 1);
	free_pos += mark - disk->free_start;
	TRACEMSG("mark:%d, page:%d, free_start:%d, free_next:%d, free_pos:%d\n",
		mark, page, disk->free_start, disk->free_next, free_pos);

	ASSERT(disk->page_array[free_pos] == PAGE_UNSET_SENTINEL);
	disk->page_array[free_pos] = page;
}

/**
 * Mark \a page of \a disk as free.
 * \note free_next of \a disk is used as \a page free marker
 * and is increased by 1.
 */
static bool battfs_markFree(struct BattFsSuper *disk, struct BattFsPageHeader *hdr, pgcnt_t page)
{
	uint8_t buf[BATTFS_HEADER_LEN];

	hdr->mark = disk->free_next;
	hdr->fcs_free = computeFcsFree(hdr);
	battfs_to_disk(hdr, buf);

	if (!disk->write(disk, page, disk->page_size - BATTFS_HEADER_LEN - 1, buf, BATTFS_HEADER_LEN))
	{
		TRACEMSG("error marking page [%d]\n", page);
		return false;
	}
	else
	{
		disk->free_next++;
		return true;
	}
}


/**
 * Initialize and mount disk described by
 * \a d.
 * \return false on errors, true otherwise.
 */
bool battfs_init(struct BattFsSuper *disk)
{
	BattFsPageHeader hdr;
	pgoff_t filelen_table[BATTFS_MAX_FILES];
	mark_t minl, maxl, minh, maxh;

	/* Sanity check */
	ASSERT(disk->open);

	/* Init disk device */
	if (!disk->open(disk))
	{
		TRACEMSG("open error\n");
		return false;
	}

	/* Disk open must set all of these */
	ASSERT(disk->read);
	ASSERT(disk->write);
	ASSERT(disk->erase);
	ASSERT(disk->close);
	ASSERT(disk->page_size);
	ASSERT(disk->page_count);
	ASSERT(disk->page_count < PAGE_UNSET_SENTINEL - 1);
	ASSERT(disk->page_array);

	memset(filelen_table, 0, BATTFS_MAX_FILES * sizeof(pgoff_t));

	/* Initialize min and max counters to keep trace od free blocks */
	minl = MAX_PAGE_ADDR;
	maxl = 0;
	minh = MAX_PAGE_ADDR | MARK_HALF_SIZE;
	maxh = 0 | MARK_HALF_SIZE;

	disk->free_bytes = 0;
	disk->disk_size = (disk_size_t)(disk->page_size - BATTFS_HEADER_LEN) * disk->page_count;

	/* Count the number of disk page per file */
	for (pgcnt_t page = 0; page < disk->page_count; page++)
	{
		if (!battfs_readHeader(disk, page, &hdr))
			return false;

		/* Check header FCS */
		if (hdr.fcs == computeFcs(&hdr))
		{
			ASSERT(hdr.mark == MARK_PAGE_VALID);
			ASSERT(hdr.fcs_free == FCS_FREE_VALID);
			ASSERT(hdr.fill <= disk->page_size - BATTFS_HEADER_LEN);

			/* Page is valid and is owned by a file */
			filelen_table[hdr.inode]++;

			/* Keep trace of free space */
			disk->free_bytes += disk->page_size - BATTFS_HEADER_LEN - hdr.fill;
		}
		else
		{
			/* Increase free space */
			disk->free_bytes += disk->page_size - BATTFS_HEADER_LEN;
			
			/* Check if page is marked free */
			if (hdr.fcs_free == computeFcsFree(&hdr))
			{
				/*
				 * This page is a valid and marked free page.
				 * Update min and max free page markers.
				 */
				if (hdr.mark < MARK_HALF_SIZE)
				{
					minl = MIN(minl, hdr.mark);
					maxl = MAX(maxl, hdr.mark);
				}
				else
				{
					minh = MIN(minh, hdr.mark);
					maxl = MAX(maxl, hdr.mark);
				}
			}
			else
				TRACEMSG("page [%d] invalid, keeping as free\n", page);
		}
	}

	/* Once here, we have filelen_table filled with file lengths */

	/* Fill page array with sentinel */
	for (pgcnt_t page = 0; page < disk->page_count; page++)
		disk->page_array[page] = PAGE_UNSET_SENTINEL;

	/* Determine free_start & free_next */
	if (maxl >= minl)
	{
		if (maxh >= minh)
		{
			if (maxl == minh - 1)
			{
				disk->free_start = minl;
				disk->free_next = maxh;
			}
			else
			{
				ASSERT(minl == 0);
				ASSERT(maxh == (MAX_PAGE_ADDR | MARK_HALF_SIZE));

				disk->free_start = minh;
				disk->free_next = maxl;
			}
		}
		else
		{
			disk->free_start = minl;
			disk->free_next = maxl;
		}
	}
	else if (maxh >= minh)
	{
		disk->free_start = minh;
		disk->free_next = maxh;
	}
	else
	{
		TRACEMSG("No valid marked free block found\n");
		disk->free_start = 0;
		disk->free_next = -1;
	}

	/* free_next should contain the first usable address */
	disk->free_next++;

	TRACEMSG("Free markers:\n minl %u\n maxl %u\n minh %u\n maxh %u\n free_start %u\n free_next %u\n",
		minl, maxl, minh, maxh, disk->free_start, disk->free_next);


	/* Fill page allocation array */
	for (pgcnt_t page = 0; page < disk->page_count; page++)
	{
		if (!battfs_readHeader(disk, page, &hdr))
			return false;

		/* Check header FCS */
		if (hdr.fcs == computeFcs(&hdr))
		{
			/* Page is valid and is owned by a file */
			ASSERT(hdr.mark == MARK_PAGE_VALID);
			ASSERT(hdr.fcs_free == FCS_FREE_VALID);

			/* Compute array position */
			pgcnt_t array_pos = countPages(filelen_table, hdr.inode);
			array_pos += hdr.pgoff;

			/* Check if position is already used by another page of the same file */
			if (LIKELY(disk->page_array[array_pos] == PAGE_UNSET_SENTINEL))
				disk->page_array[array_pos] = page;
			else
			{
				BattFsPageHeader hdr_old;
				
				if (!battfs_readHeader(disk, disk->page_array[array_pos], &hdr_old))
					return false;

				/* Check header FCS */
				ASSERT(hdr_old.fcs == computeFcs(&hdr_old));

				/* Only the very same page with a different seq number can be here */
				ASSERT(hdr.inode == hdr_old.inode);
				ASSERT(hdr.pgoff == hdr_old.pgoff);
				ASSERT(hdr.mark == hdr_old.mark);
				ASSERT(hdr.fcs_free == hdr_old.fcs_free);
				ASSERT(hdr.seq != hdr_old.seq);

				pgcnt_t new_page, old_page;
				fill_t old_fill;

				/* Fancy check to handle seq wraparound */
				#define HALF_SEQ (1 << ((sizeof(seq_t) * CPU_BITS_PER_CHAR) - 1))
				if ((hdr.seq - hdr_old.seq) < HALF_SEQ)
				{
					/* Current header is newer than the previuos one */
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
				disk->free_bytes += old_fill;

				/* Shift all array one position to the left, overwriting duplicate page */
				array_pos -= hdr.pgoff;
				array_pos += filelen_table[hdr.inode];
				movePages(disk, array_pos, -1);
				
				/* Decrease file page count */
				filelen_table[hdr.inode]--;

				/* Add old page to free pages pool */
				if (!battfs_markFree(disk, &hdr, old_page))
					return false;

				insertFreePage(disk, filelen_table, hdr.mark, old_page);
			}
		}
		else
		{
			/* Check if page is free */
			if (hdr.fcs_free != computeFcsFree(&hdr))
				/* Page is not a valid marked page, insert at the end of list */
				hdr.mark = disk->free_next++;

			insertFreePage(disk, filelen_table, hdr.mark, page);
		}
	}

	#warning Test me!	
	return true;	
}


