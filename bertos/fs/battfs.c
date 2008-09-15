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
 * \brief BattFS: a filesystem for embedded platforms (implementation).
 *
 * \version $Id:$
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 */

#include "battfs.h"

#include <cfg/debug.h>
#include <cfg/macros.h> /* MIN, MAX */
#include <cpu/byteorder.h> /* cpu_to_xx */


#include <string.h> /* memset, memmove */


/**
 * Convert from memory representation to disk structure.
 * \note filesystem is in little-endian format.
 */
INLINE void battfs_to_disk(struct BattFsPageHeader *hdr, uint8_t *buf)
{
	STATIC_ASSERT(BATTFS_HEADER_LEN == 10);
	buf[0] = hdr->inode;

	buf[1] = hdr->fill;
	buf[2] = hdr->fill >> 8;

	buf[3] = hdr->pgoff;
	buf[4] = hdr->pgoff >> 8;

	/*
	 * Sequence number is at least 1 bit longer than page address.
	 * Needed to take care of wraparonds.
	 */
	buf[5] = hdr->seq;
	buf[6] = hdr->seq >> 8;

	/*
	 * First bit used by seq.
	 * Unused bits are set to 1.
	 */
	buf[7] = (hdr->seq >> 16) ? 0xFF : 0xFE;

	/*
	 * This field must be the last one!
	 * This is needed because if the page is only partially
	 * written, we can use this to detect it.
	 */
	buf[8] = hdr->fcs;
	buf[9] = hdr->fcs >> 8;
}

/**
 * Convert from disk structure to memory representation.
 * \note filesystem is in little-endian format.
 */
INLINE void disk_to_battfs(uint8_t *buf, struct BattFsPageHeader *hdr)
{
	STATIC_ASSERT(BATTFS_HEADER_LEN == 10);
	hdr->inode = buf[0];
	hdr->fill = buf[2] << 8 | buf[1];
	hdr->pgoff = buf[4] << 8 | buf[3];
	hdr->seq = (seq_t)(buf[7] & 0x01) << 16 | buf[6] << 8 | buf[5];
	hdr->fcs = buf[9] << 8 | buf[8];
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
	rotating_update(buf, BATTFS_HEADER_LEN - sizeof(fcs_t), &cks);
	return cks;
}


/**
 * Read header of page \a page.
 * \return true on success, false otherwise.
 */
static bool battfs_readHeader(struct BattFsSuper *disk, pgcnt_t page, struct BattFsPageHeader *hdr)
{
	uint8_t buf[BATTFS_HEADER_LEN];
	/*
	 * Read header from disk.
	 * Header is actually a footer, and so
	 * resides at page end.
	 */
	if (disk->read(disk, page, disk->page_size - BATTFS_HEADER_LEN, buf, BATTFS_HEADER_LEN)
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
 * Write header of page \a page.
 * \return true on success, false otherwise.
 */
static bool battfs_writeHeader(struct BattFsSuper *disk, pgcnt_t page, struct BattFsPageHeader *hdr)
{
	uint8_t buf[BATTFS_HEADER_LEN];

	/* Fill buffer */
	battfs_to_disk(hdr, buf);

	/*
	 * write header to disk.
	 * Header is actually a footer, and so
	 * resides at page end.
	 */
	if (disk->write(disk, page, disk->page_size - BATTFS_HEADER_LEN, buf, BATTFS_HEADER_LEN)
	    != BATTFS_HEADER_LEN)
	{
		TRACEMSG("Error: page[%d]\n", page);
		return false;
	}
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
	memmove(&disk->page_array[dst], &disk->page_array[src], (disk->page_count - MAX(dst, src)) * sizeof(pgcnt_t));

	if (offset < 0)
	{
		/* Fill empty space in array with sentinel */
		for (pgcnt_t page = disk->page_count + offset; page < disk->page_count; page++)
			disk->page_array[page] = PAGE_UNSET_SENTINEL;
	}
}

#if 0

/**
 * Insert \a page at the bottom of page allocation array of \a disk.
 */
static void insertFreePage(struct BattFsSuper *disk, pgcnt_t page)
{
	pgcnt_t free_pos = disk->page_count - 1;
	ASSERT(disk->page_array[free_pos] == PAGE_UNSET_SENTINEL);
	ASSERT(page <= free_pos);

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

	if (!disk->write(disk, page, disk->page_size - BATTFS_HEADER_LEN, buf, BATTFS_HEADER_LEN))
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
 * Determine free_start and free_next blocks for \a disk
 * using \a minl, \a maxl, \a minh, \a maxh.
 *
 * Mark_t is a type that has at least 1 bit more than
 * pgaddr_t. So all free blocks can be numbered using
 * at most half numbers of a mark_t type.
 * The free blocks algorithm increments by 1 the disk->free_next
 * every time a page becomes free. So the free block sequence is
 * guaranteed to be countiguous.
 * Only wrap arounds may happen, but due to half size sequence limitation,
 * there are only 4 possible situations:
 *
 * \verbatim
 *    |------lower half------|-------upper half-------|
 *
 * 1) |------minl*****maxl---|------------------------|
 * 2) |------minl********maxl|minh******maxh----------|
 * 3) |----------------------|----minh*******maxh-----|
 * 4) |minl******maxl--------|------------minh****maxh|
 * \endverbatim
 *
 * Situations 1 and 3 are easy to detect, while 2 and 4 require more care.
 */
static void findFreeStartNext(struct BattFsSuper *disk, mark_t minl, mark_t maxl, mark_t minh, mark_t maxh)
{
	/* Determine free_start & free_next */
	if (maxl >= minl)
	{
		/* Valid interval found in lower half */
		if (maxh >= minh)
		{
			/* Valid interval also found in upper half */
			if (maxl == minh - 1)
			{
				/* Interval starts in lower half and ends in upper */
				disk->free_start = minl;
				disk->free_next = maxh;
			}
			else
			{
				/* Interval starts in upper half and ends in lower */
				ASSERT(minl == 0);
				ASSERT(maxh == (MAX_PAGE_ADDR | MARK_HALF_SIZE));

				disk->free_start = minh;
				disk->free_next = maxl;
			}
		}
		else
		{
			/*
			 * Upper interval is invalid.
			 * Use lower values.
			 */

			disk->free_start = minl;
			disk->free_next = maxl;
		}
	}
	else if (maxh >= minh)
	{
		/*
		 * Lower interval is invalid.
		 * Use upper values.
		 */
		disk->free_start = minh;
		disk->free_next = maxh;
	}
	else
	{
		/*
		 * No valid interval found.
		 * Hopefully the disk is brand new (or full).
		 */
		TRACEMSG("No valid marked free block found, new disk or disk full\n");
		disk->free_start = 0;
		disk->free_next = -1; //to be increased later
	}

	/* free_next should contain the first usable address */
	disk->free_next++;

	TRACEMSG("Free markers:\n minl %u\n maxl %u\n minh %u\n maxh %u\n free_start %u\n free_next %u\n",
		minl, maxl, minh, maxh, disk->free_start, disk->free_next);
}
#endif

/**
 * Count number of pages per file on \a disk.
 * This information is registered in \a filelen_table.
 * Array index represent file inode, while value contained
 * is the number of pages used by that file.
 *
 * \return true if ok, false on disk read errors.
 * \note The whole disk is scanned once.
 */
static bool countDiskFilePages(struct BattFsSuper *disk, pgoff_t *filelen_table)
{
	BattFsPageHeader hdr;
	disk->free_page_start = 0;

	/* Count the number of disk page per file */
	for (pgcnt_t page = 0; page < disk->page_count; page++)
	{
		if (!battfs_readHeader(disk, page, &hdr))
			return false;

		/* Increase free space */
		disk->free_bytes += disk->page_size - BATTFS_HEADER_LEN;

		/* Check header FCS */
		if (hdr.fcs == computeFcs(&hdr))
		{
			ASSERT(hdr.fill <= disk->page_size - BATTFS_HEADER_LEN);

			/* Page is valid and is owned by a file */
			filelen_table[hdr.inode]++;

			/* Keep trace of free space */
			disk->free_bytes -= hdr.fill;
			disk->free_page_start++;
		}
	}

	return true;
}

/**
 * Fill page allocation array of \a disk
 * using file lenghts in \a filelen_table.
 *
 * The page allocation array is an array containings all file infos.
 * Is ordered by file, and within each file is ordered by page offset
 * inside file.
 * e.g. : at page array[0] you will find page address of the first page
 * of the first file (if present).
 * Free blocks are allocated after the last file, starting from invalid ones
 * and continuing with the marked free ones.
 *
 * \return true if ok, false on disk read errors.
 * \note The whole disk is scanned once.
 */
static bool fillPageArray(struct BattFsSuper *disk, pgoff_t *filelen_table)
{
	BattFsPageHeader hdr;
	pgcnt_t curr_free_page = disk->free_page_start;
	/* Fill page allocation array */
	for (pgcnt_t page = 0; page < disk->page_count; page++)
	{
		if (!battfs_readHeader(disk, page, &hdr))
			return false;

		/* Check header FCS */
		if (hdr.fcs == computeFcs(&hdr))
		{
			/* Compute array position */
			pgcnt_t array_pos_start = countPages(filelen_table, hdr.inode);
			pgcnt_t array_pos = array_pos_start + hdr.pgoff;

			/* Find the first free position */
			while (disk->page_array[array_pos] != PAGE_UNSET_SENTINEL)
			{
				ASSERT(array_pos < array_pos_start + filelen_table[hdr.inode + 1]);
				array_pos++;
			}

			disk->page_array[array_pos] = page;
		}
		else
		{
			/* Invalid page, keep as free */
			ASSERT(disk->page_array[curr_free_page] == PAGE_UNSET_SENTINEL);
			LOG_INFO("Page %d invalid, keeping as free\n", page);
			disk->page_array[curr_free_page++] = page;
		}
	}
	return true;
}

/**
 * Find the latest version of a page, starting from the
 * page supplied by \a page_array.
 * The pages are read from the disk until a different
 * inode or page offset is found.
 * The lastest version of the page is moved in the first
 * position of \a page_array.
 * \return the number of old versions of the page or PAGE_ERROR
 *         on disk read errors.
 */
static pgcnt_t findLastVersion(pgcnt_t *page_array)
{
	pgcnt_t *array_start = page_array;
	BattFsPageHeader hdr;
	if (!battfs_readHeader(disk, *page_array++, &hdr))
		return PAGE_ERROR;

	/* Free space: early bailout */
	if (hdr.fcs != computeFcs(&hdr))
		return 0;

	/*
	 * If the first page is valid,
	 * inode and pg_off in the array are taken
	 * as the current page markers.
	 */
	inode_t curr_inode = hdr.inode;
	pgoff_t curr_pgoff = hdr.pgoff;

	/* Temps used to find the sequence number range */
	seq_t minl = HALF_SEQ - 1;
	seq_t maxl = 0;
	seq_t minh = FULL_SEQ;
	seq_t maxh = HALF_SEQ;
	pgcnt_t lpos = 0, hpos = 0, dup_cnt = 0;

	/*
	 * Find min and max values for the two
	 * half of seq_num range.
	 * With this we can find seqnum wraparounds.
	 * seq_t is a type that has at least 1 bit more than
	 * pgaddr_t. So all version of a page blocks can be numbered using
	 * at most half numbers of a seq_t type.
	 * The sequence number algorithm increments by 1 the previous seq_num
	 * every time a page is rewritten. So the sequence is
	 * guaranteed to be countiguous.
	 * Only wrap arounds may happen, but due to half size sequence limitation,
	 * there are only 4 possible situations:
	 *
	 * \verbatim
	 *    |------lower half------|-------upper half-------|
	 *
	 * 1) |------minl*****maxl---|------------------------|
	 * 2) |------minl********maxl|minh******maxh----------|
	 * 3) |----------------------|----minh*******maxh-----|
	 * 4) |minl******maxl--------|------------minh****maxh|
	 * \endverbatim
	 *
	 * Situations 1 and 3 are easy to detect, while 2 and 4 require more care.
	 */
	do
	{
		if (hdr.seq < SEQ_HALF_SIZE)
		{
			minl = MIN(minl, hdr.seq);
			if (hdr.seq > maxl)
			{
				maxl = hdr.seq;
				lpos = dup_cnt;
			}
		}
		else
		{
			minh = MIN(minh, hdr.seq);
			if (hdr.seq > maxh)
			{
				maxh = hdr.seq;
				hpos = dup_cnt;
			}
		}

		if (!battfs_readHeader(disk, *page_array++, &hdr))
			return PAGE_ERROR;
		dup_cnt++;
	}
	while (curr_inode == hdr.inode && curr_pgoff == hdr.pgoff && hdr.fcs == computeFcs(&hdr))


	/* Return early if there is only one version of the current page */
	if (dup_cnt == 1)
		return 0;

	/* Find the position in the array of the last version of the page */
	pgcnt_t last_ver = hpos;
	if (maxl >= minl)
	{
		/* Valid interval found in lower half */
		if (maxh >= minh)
		{
			/* Valid interval also found in upper half */
			if (maxl != minh - 1)
			{
				/* Interval starts in upper half and ends in lower */
				ASSERT(minl == 0);
				ASSERT(maxh == FULL_SEQ);

				last_ver = lpos;
			}
		}
		else
			/*
			 * Upper interval is invalid.
			 * Use lower values.
			 */
			last_ver = lpos;
	}

	/* Put last page version at array start position */
	SWAP(array_start[0], array_start[last_ver]);

	return dup_cnt - 1;
}

/**
 * Collect old pages, removing empty spaces from \a pg_array, for a maximum len of \a pg_len.
 * Once the collect task is completed, copy \a old_cnt pages from \a old_pages at the
 * end of free space in pg_array.
 */
void collectOldPages(pgcnt_t *pg_array, pgcnt_t pg_len, pgcnt_t *old_pages, pgcnt_t old_cnt)
{
	bool copy = false;
	pgcnt_t gap = 0;

	for (pgcnt_t curr_page = 0; curr_page < pg_len; pg_len++)
	{
		if (!copy)
		{
			if (pg_array[curr_page] == PAGE_UNSET_SENTINEL)
				gap++;
			else
			{
				pg_array[curr_page - gap] = pg_array[curr_page];
				copy = true;
			}
		}
		else
		{
			if (pg_array[curr_page] != PAGE_UNSET_SENTINEL)
				pg_array[curr_page - gap] = pg_array[curr_page];
			else
			{
				gap++;
				copy = false;
			}
		}
	}
	ASSERT(gap == old_cnt);
	pg_array += pg_len - old_cnt;

	memcpy(pg_array, old_pages, old_cnt * sizeof(pgcnt_t));
}

/**
 * This function scan the page array of \a disk looking for
 * old versions of the same page.
 *
 * Only the last version is kept as valid, the old ones are inserted
 * in the free blocks heap.
 * \return true if ok, false on disk read errors.
 * \note The whole disk is scanned once.
 */
static bool dropOldPages(struct BattFsSuper *disk)
{
	#define OLD_PAGE_BUFLEN 64
	pgcnt_t old_pages[OLD_PAGE_BUFLEN];
	pgcnt_t old_cnt = 0;

	pgcnt_t *curr_page = disk->page_array;
	pgcnt_t *collect_start = disk->page_array;
	pgcnt_t collect_len = disk->page_count;
	pgcnt_t dup_pages;

	do
	{
		dup_pages = findLastVersion(curr_page);
		if (dup_pages == PAGE_ERROR)
			return false;
		/* The first page is the last version */
		curr_page++;
		while (dup_pages--)
		{
			if (old_cnt >= OLD_PAGE_BUFLEN)
			{
				collectOldPages(collect_start, collect_len, old_pages, old_cnt);
				collect_len -= old_cnt;
				disk->free_bytes += old_cnt * (disk->page_size - BATTFS_HEADER_LEN);
				disk->free_page_start -= old_cnt;
				curr_page -= old_cnt;
				collect_start = curr_page;
				old_cnt = 0;
			}

			old_pages[old_cnt++] = *curr_page;
			*curr_page++ = PAGE_UNSET_SENTINEL;
		}
	}
	while (curr_page < disk->page_array + disk->free_page_start);

	collectOldPages(collect_start, collect_len, old_pages, old_cnt);
	disk->free_bytes += old_cnt * (disk->page_size - BATTFS_HEADER_LEN);
	disk->free_page_start -= old_cnt;

	return true;
}


/**
 * Initialize and mount disk described by
 * \a disk.
 * \return false on errors, true otherwise.
 */
bool battfs_init(struct BattFsSuper *disk)
{
	pgoff_t filelen_table[BATTFS_MAX_FILES];

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

	disk->free_bytes = 0;
	disk->disk_size = (disk_size_t)(disk->page_size - BATTFS_HEADER_LEN) * disk->page_count;

	/* Count pages per file */
	if (!countDiskFilePages(disk, filelen_table))
	{
		TRACEMSG("error counting file pages\n");
		return false;
	}

	/* Once here, we have filelen_table filled with file lengths */

	/* Fill page array with sentinel */
	for (pgcnt_t page = 0; page < disk->page_count; page++)
		disk->page_array[page] = PAGE_UNSET_SENTINEL;

	/* Fill page allocation array using filelen_table */
	if (!fillPageArray(disk, filelen_table))
	{
		TRACEMSG("error filling page array\n");
		return false;
	}

	if (!dropOldPages(disk))
	{
		LOG_ERR("error dropping old pages\n");
		return false;
	}

	/* Init list for opened files. */
	LIST_INIT(&disk->file_opened_list);
	return true;
}

/**
 * Flush file \a fd.
 * \return 0 if ok, EOF on errors.
 */
static int battfs_flush(struct KFile *fd)
{
	(void)fd;
	#warning TODO
	return 0;
}

/**
 * Close file \a fd.
 * \return 0 if ok, EOF on errors.
 */
static int battfs_fileclose(struct KFile *fd)
{
	BattFS *fdb = BATTFSKFILE(fd);

	battfs_flush(fd);
	REMOVE(&fdb->link);
	return 0;
}

/**
 * Read from file \a fd \a size bytes in \a buf.
 * \return The number of bytes read.
 */
static size_t battfs_read(struct KFile *fd, void *_buf, size_t size)
{
	BattFS *fdb = BATTFSKFILE(fd);
	uint8_t *buf = (uint8_t *)_buf;

	size_t total_read = 0;
	pgoff_t pg_offset;
	pgaddr_t addr_offset;
	pgaddr_t read_len;

	size = MIN((kfile_off_t)size, fd->size - fd->seek_pos);

	while (size)
	{
		pg_offset = fd->seek_pos / (fdb->disk->page_size - BATTFS_HEADER_LEN);
		addr_offset = fd->seek_pos % (fdb->disk->page_size - BATTFS_HEADER_LEN);
		read_len = MIN(size, (size_t)(fdb->disk->page_size - BATTFS_HEADER_LEN - addr_offset));

		/* Read from disk */
		if (fdb->disk->read(fdb->disk, fdb->start[pg_offset], addr_offset, buf, read_len) != read_len)
		{
			#warning TODO set error?
		}

		size -= read_len;
		fd->seek_pos += read_len;
		total_read += read_len;
		buf += read_len;
	}
	return total_read;
}


/**
 * Search file \a inode in \a disk using a binary search.
 * \return pointer to file start in disk->page_array
 * if file exists, NULL otherwise.
 */
static pgcnt_t *findFile(BattFsSuper *disk, inode_t inode)
{
	BattFsPageHeader hdr;
	pgcnt_t first = 0, page, last = disk->page_count -1;
	fcs_t fcs;

	while (first <= last)
	{
		page = (first + last) / 2;

		if (!battfs_readHeader(disk, disk->page_array[page], &hdr))
			return NULL;

		fcs = computeFcs(&hdr);
		if (hdr.fcs == fcs && hdr.inode == inode)
			return (&disk->page_array[page]) - hdr.pgoff;
		else if (hdr.fcs == fcs && hdr.inode < inode)
			first = page + 1;
		else
			last = page - 1;
	}

	return NULL;
}

/**
 * \return true if file \a inode exists on \a disk, false otherwise.
 */
bool battfs_fileExists(BattFsSuper *disk, inode_t inode)
{
	return findFile(disk, inode) != NULL;
}

/**
 * Count size of file \a inode on \a disk, starting at pointer \a start
 * in disk->page_array. Size is written in \a size.
 * \return true if all s ok, false on disk read errors.
 */
static bool countFileSize(BattFsSuper *disk, pgcnt_t *start, inode_t inode, file_size_t *size)
{
	*size = 0;
	BattFsPageHeader hdr;

	for (;;)
	{
		if (!battfs_readHeader(disk, *start++, &hdr))
			return false;
		if (hdr.fcs == computeFcs(&hdr) && hdr.inode == inode)
			*size += hdr.fill;
		else
			return true;
	}
}

/**
 * Open file \a inode from \a disk in \a mode.
 * File context is stored in \a fd.
 * \return true if ok, false otherwise.
 */
bool battfs_fileopen(BattFsSuper *disk, BattFS *fd, inode_t inode, filemode_t mode)
{
	Node *n;

	memset(fd, 0, sizeof(*fd));

	/* Search file start point in disk page array */
	fd->start = findFile(disk, inode);
	if (fd->start == NULL)
	{
		if (!(mode & BATTFS_CREATE))
			return false;

		/* File does not exist, create it */
		BattFsPageHeader hdr;
		hdr.inode = inode;
		hdr.seq = 0;
		hdr.fill = 0;
		hdr.pgoff = 0;
		hdr.mark = MARK_PAGE_VALID;
		hdr.fcs_free = FCS_FREE_VALID;
		hdr.fcs = computeFcs(&hdr);
		#warning TODO: get a free block and write on disk!
	}

	/* Fill file size */
	if (!countFileSize(disk, fd->start, inode, &fd->fd.size))
		return false;

	/* Reset seek position */
	fd->fd.seek_pos = 0;

	/* Insert file handle in list, ordered by inode, ascending. */
	FOREACH_NODE(n, &disk->file_opened_list)
	{
		BattFS *file = containerof(n, BattFS, link);
		if (file->inode >= inode)
			break;
	}
	INSERT_BEFORE(&fd->link, n);

	/* Fill in data */
	fd->inode = inode;
	fd->mode = mode;
	fd->disk = disk;

	fd->fd.close = battfs_fileclose;
	fd->fd.flush = battfs_flush;
	fd->fd.read = battfs_read;
	fd->fd.reopen = kfile_genericReopen;
	fd->fd.seek = kfile_genericSeek;

#warning TODO battfs_write, battfs_error, battfs_clearerr
#if 0
	fd->fd.write = battfs_write;
	fd->fd.error = battfs_error;
	fd->fd.clearerr = battfs_clearerr;
#endif

	DB(fd->fd._type = KFT_BATTFS);

	return true;
}

/**
 * Close \a disk.
 */
bool battfs_close(struct BattFsSuper *disk)
{
	Node *n;
	int res = 0;

	/* Close all open files */
	FOREACH_NODE(n, &disk->file_opened_list)
	{
		BattFS *file = containerof(n, BattFS, link);
		res += battfs_fileclose(&file->fd);
	}

	/* Close disk */
	return disk->close(disk) && (res == 0);
}


bool battfs_writeTestBlock(struct BattFsSuper *disk, pgcnt_t page, inode_t inode, seq_t seq, fill_t fill, pgoff_t pgoff, mark_t mark)
{
	BattFsPageHeader hdr;

	hdr.inode = inode;
	hdr.seq = seq;
	hdr.fill = fill;
	hdr.pgoff = pgoff;
	hdr.mark = MARK_PAGE_VALID;
	hdr.fcs_free = FCS_FREE_VALID;
	hdr.fcs = computeFcs(&hdr);
	if (mark != MARK_PAGE_VALID)
	{
		hdr.mark = mark;
		hdr.fcs_free = computeFcsFree(&hdr);
	}

	if (!battfs_writeHeader(disk, page, &hdr))
	{
		TRACEMSG("error writing hdr\n");
		return false;
	}

	return true;
}
