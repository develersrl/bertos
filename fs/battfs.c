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
	STATIC_ASSERT(BATTFS_HEADER_LEN == 12);
	buf[0] = hdr->inode;

	buf[1] = hdr->fill;
	buf[2] = hdr->fill >> 8;

	buf[3] = hdr->pgoff;
	buf[4] = hdr->pgoff >> 8;

	/*
	 * Mark is at least 1 bit longer than page address.
	 * Needed to take care of wraparonds.
	 */
	buf[5] = hdr->mark;
	buf[6] = hdr->mark >> 8;

	/*
	 * First bit used by mark, last 2 bits used by seq.
	 * Since only 2 pages with the same inode and pgoff
	 * can exist at the same time, 2 bit for seq are enough.
	 * Unused bits are set to 1.
	 */
	buf[7] = ((hdr->mark >> 16) & 0x01) | (hdr->seq << 6) | ~(BV(7) | BV(6) | BV(0));

	/*
	 * This field must be the before the last one!
	 */
	buf[8] = hdr->fcs_free;
	buf[9] = hdr->fcs_free >> 8;

	/*
	 * This field must be the last one!
	 * This is needed because if the page is only partially
	 * written, we can use this to detect it.
	 */
	buf[10] = hdr->fcs;
	buf[11] = hdr->fcs >> 8;
}

/**
 * Convert from disk structure to memory representation.
 * \note filesystem is in little-endian format.
 */
INLINE void disk_to_battfs(uint8_t *buf, struct BattFsPageHeader *hdr)
{
	STATIC_ASSERT(BATTFS_HEADER_LEN == 12);
	hdr->inode = buf[0];
	hdr->fill = buf[2] << 8 | buf[1];
	hdr->pgoff = buf[4] << 8 | buf[3];
	hdr->mark = (mark_t)(buf[7] & 0x01) << 16 | buf[6] << 8 | buf[5];
	hdr->seq = buf[7] >> 6;
	hdr->fcs_free = buf[9] << 8 | buf[8];
	hdr->fcs = buf[11] << 8 | buf[10];
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
 * Compute the fcs of the header marked as free.
 */
static fcs_t computeFcsFree(struct BattFsPageHeader *hdr)
{
	uint8_t buf[BATTFS_HEADER_LEN];
	fcs_t cks;

	battfs_to_disk(hdr, buf);
	rotating_init(&cks);
	/* fcs_free is just before fcs of whole header */
	rotating_update(buf, BATTFS_HEADER_LEN - 2 * sizeof(fcs_t), &cks);
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
 * \note \a hdr is dirtyed even on errors.
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

/**
 * Insert \a page into page allocation array of \a disk,
 * using  \a mark to compute position.
 */
static void insertFreePage(struct BattFsSuper *disk, mark_t mark, pgcnt_t page)
{
	ASSERT(mark - disk->free_start < disk->free_next - disk->free_start);

	pgcnt_t free_pos = disk->page_count - disk->free_next + mark;
	ASSERT(free_pos < disk->page_count);

	TRACEMSG("mark:%u, page:%u, free_start:%u, free_next:%u, free_pos:%u\n",
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
	mark_t minl, maxl, minh, maxh;

	/* Initialize min and max counters to keep trace od free blocks */
	minl = MAX_PAGE_ADDR;
	maxl = 0;
	minh = MAX_PAGE_ADDR | MARK_HALF_SIZE;
	maxh = 0 | MARK_HALF_SIZE;


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
					maxh = MAX(maxh, hdr.mark);
				}
			}
			else
				TRACEMSG("page [%d] invalid, keeping as free\n", page);
		}
	}
	findFreeStartNext(disk, minl, maxl, minh, maxh);
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

				/* Fancy check to handle seq wraparound (2 bits only) */
				if (((hdr.seq - hdr_old.seq) & 0x03) < 2)
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

				insertFreePage(disk, hdr.mark, old_page);
			}
		}
		else
		{
			/* Check if page is free */
			if (hdr.fcs_free != computeFcsFree(&hdr))
				/* Page is not a valid marked page, insert at list beginning */
				hdr.mark = --disk->free_start;

			insertFreePage(disk, hdr.mark, page);
		}
	}
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
	KFileBattFs *fdb = KFILEBATTFS(fd);

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
	KFileBattFs *fdb = KFILEBATTFS(fd);
	uint8_t *buf = (uint8_t *)_buf;

	size_t total_read = 0;
	pgoff_t pg_offset;
	pgaddr_t addr_offset;
	pgaddr_t read_len;

	size = MIN(size, fd->size - fd->seek_pos);

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
bool battfs_fileopen(BattFsSuper *disk, KFileBattFs *fd, inode_t inode, filemode_t mode)
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
		KFileBattFs *file = containerof(n, KFileBattFs, link);
		if (file->inode >= inode)
			break;
	}
	INSERT_BEFORE(n, &fd->link);

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
		KFileBattFs *file = containerof(n, KFileBattFs, link);
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
