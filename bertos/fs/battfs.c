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

#define LOG_LEVEL       LOG_LVL_WARNING
#define LOG_FORMAT      LOG_FMT_VERBOSE
#include <cfg/log.h>

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
	 * Sequence number is 40 bits long.
	 * No need to take care of wraparonds: the memory will die first!
	 */
	buf[5] = hdr->seq;
	buf[6] = hdr->seq >> 8;
	buf[7] = hdr->seq >> 16;
	buf[8] = hdr->seq >> 24;
	buf[9] = hdr->seq >> 32;

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
	hdr->seq = (seq_t)buf[9] << 32 | (seq_t)buf[8] << 24 | (seq_t)buf[7] << 16 | buf[6] << 8 | buf[5];
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
		LOG_ERR("Error: page[%d]\n", page);
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
	if (!(disk->bufferWrite(disk, disk->page_size - BATTFS_HEADER_LEN, buf, BATTFS_HEADER_LEN)
	    == BATTFS_HEADER_LEN && disk->save(disk, page)))
	{
		LOG_ERR("Error: page[%d]\n", page);
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
	LOG_INFO("free_bytes:%d, free_page_start:%d\n", disk->free_bytes, disk->free_page_start);

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
 * Free blocks are allocated after the last file.
 *
 * \return true if ok, false on disk read errors.
 * \note The whole disk is scanned at max twice.
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
			pgcnt_t array_pos = countPages(filelen_table, hdr.inode);
			array_pos += hdr.pgoff;


			/* Check if position is already used by another page of the same file */
			if (disk->page_array[array_pos] == PAGE_UNSET_SENTINEL)
				disk->page_array[array_pos] = page;
			else
			{
				BattFsPageHeader hdr_prv;

				if (!battfs_readHeader(disk, disk->page_array[array_pos], &hdr_prv))
					return false;

				/* Check header FCS */
				ASSERT(hdr_prv.fcs == computeFcs(&hdr_prv));

				/* Only the very same page with a different seq number can be here */
				ASSERT(hdr.inode == hdr_prv.inode);
				ASSERT(hdr.pgoff == hdr_prv.pgoff);
				ASSERT(hdr.seq != hdr_prv.seq);

				pgcnt_t new_page, old_page;
				fill_t old_fill;

				/*
				 * Sequence number comparison: since
				 * seq is 40 bits wide, it wraps once
				 * every 1.1E12 times.
				 * The memory will not live enough to
				 * see a wraparound, so we can use a simple
				 * compare here.
				 */
				if (hdr.seq > hdr_prv.seq)
				{
					/* Current header is newer than the previuos one */
					old_page = disk->page_array[array_pos];
					new_page = page;
					old_fill = hdr_prv.fill;
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
				/* Move back all indexes */
				filelen_table[hdr.inode]--;
				disk->free_page_start--;
				curr_free_page--;
				/* Set old page as free */
				ASSERT(disk->page_array[curr_free_page] == PAGE_UNSET_SENTINEL);
				disk->page_array[curr_free_page++] = old_page;

			}
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
 * Flush the current \a disk buffer.
 * \return true if ok, false on errors.
 */
static bool battfs_flushBuffer(struct BattFsSuper *disk)
{
	if (disk->cache_dirty)
	{
		TRACE;
		if (!disk->save(disk, disk->curr_page))
			return false;
		disk->cache_dirty = false;
	}
	return true;
}

/**
 * Load \a new_page from \a disk in disk page buffer.
 * If a previuos page is still dirty in the buffer, will be
 * flushed first.
 * \return true if ok, false on errors.
 */
static bool battfs_loadPage(struct BattFsSuper *disk, pgcnt_t new_page)
{
	if (disk->curr_page == new_page)
		return true;

	if (battfs_flushBuffer(disk))
		return false;

	if (!disk->load(disk, new_page))
			return false;
	disk->curr_page = new_page;
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
		LOG_ERR("open error\n");
		return false;
	}

	/* Disk open must set all of these */
	ASSERT(disk->read);
	ASSERT(disk->load);
	ASSERT(disk->bufferWrite);
	ASSERT(disk->save);
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
		LOG_ERR("error counting file pages\n");
		return false;
	}

	/* Once here, we have filelen_table filled with file lengths */

	/* Fill page array with sentinel */
	for (pgcnt_t page = 0; page < disk->page_count; page++)
		disk->page_array[page] = PAGE_UNSET_SENTINEL;

	/* Fill page allocation array using filelen_table */
	if (!fillPageArray(disk, filelen_table))
	{
		LOG_ERR("error filling page array\n");
		return false;
	}
	#warning TODO: shuffle free blocks

	/* Initialize page buffer cache */
	disk->cache_dirty = false;
	disk->curr_page = 0;
	disk->load(disk, disk->curr_page);

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
	BattFs *fdb = BATTFS_CAST(fd);

	if (battfs_flushBuffer(fdb->disk))
		return 0;
	else
		return EOF;
}

/**
 * Close file \a fd.
 * \return 0 if ok, EOF on errors.
 */
static int battfs_fileclose(struct KFile *fd)
{
	BattFs *fdb = BATTFS_CAST(fd);

	battfs_flush(fd);
	REMOVE(&fdb->link);
	return 0;
}


/**
 * Write to file \a fd \a size bytes from \a buf.
 * \return The number of bytes written.
 */
static size_t battfs_write(struct KFile *fd, const void *_buf, size_t size)
{
	BattFs *fdb = BATTFS_CAST(fd);
	const uint8_t *buf = (const uint8_t *)_buf;

	size_t total_write = 0;
	pgoff_t pg_offset;
	pgaddr_t addr_offset;
	pgaddr_t wr_len;

	size = MIN((kfile_off_t)size, fd->size - fd->seek_pos);

	while (size)
	{
		#warning TODO: outside EOF?

		pg_offset = fd->seek_pos / (fdb->disk->page_size - BATTFS_HEADER_LEN);
		addr_offset = fd->seek_pos % (fdb->disk->page_size - BATTFS_HEADER_LEN);
		wr_len = MIN(size, (size_t)(fdb->disk->page_size - BATTFS_HEADER_LEN - addr_offset));


		if (fdb->start[pg_offset] != fdb->disk->curr_page)
		{
			if (!battfs_loadPage(fdb->disk, fdb->start[pg_offset]))
			{
				#warning TODO set error?
				return total_write;
			}

			/* Get a free page */
			fdb->disk->curr_page = fdb->disk->page_array[fdb->disk->free_page_start];
			movePages(fdb->disk, fdb->disk->free_page_start + 1, -1);

			/* Insert previous page in free blocks list */
			fdb->disk->page_array[fdb->disk->page_count - 1] = fdb->start[pg_offset];
			/* Assign new page */
			fdb->start[pg_offset] = fdb->disk->curr_page;
			#warning TODO: hdr have to be updated!
		}


		if (fdb->disk->bufferWrite(fdb->disk, addr_offset, buf, wr_len) != wr_len)
		{
			#warning TODO set error?
		}
		fdb->disk->cache_dirty = true;

		size -= wr_len;
		fd->seek_pos += wr_len;
		total_write += wr_len;
		buf += wr_len;
		#warning TODO: hdr have to be updated!
	}
	return total_write;

}


/**
 * Read from file \a fd \a size bytes in \a buf.
 * \return The number of bytes read.
 */
static size_t battfs_read(struct KFile *fd, void *_buf, size_t size)
{
	BattFs *fdb = BATTFS_CAST(fd);
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

		/* Flush current page if needed */
		if (fdb->start[pg_offset] == fdb->disk->curr_page)
			battfs_flushBuffer(fdb->disk);

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
	pgcnt_t first = 0, page, last = disk->free_page_start;
	fcs_t fcs;

	while (first < last)
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
static file_size_t countFileSize(BattFsSuper *disk, pgcnt_t *start, inode_t inode)
{
	file_size_t size = 0;
	BattFsPageHeader hdr;

	for (;;)
	{
		if (!battfs_readHeader(disk, *start++, &hdr))
			return EOF;
		if (hdr.fcs == computeFcs(&hdr) && hdr.inode == inode)
			size += hdr.fill;
		else
			return size;
	}
}

/**
 * Open file \a inode from \a disk in \a mode.
 * File context is stored in \a fd.
 * \return true if ok, false otherwise.
 */
bool battfs_fileopen(BattFsSuper *disk, BattFs *fd, inode_t inode, filemode_t mode)
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
		hdr.fcs = computeFcs(&hdr);
		#warning TODO: get a free block and write on disk!
	}

	/* Fill file size */
	if ((fd->fd.size = countFileSize(disk, fd->start, inode)) == EOF)
		return false;

	/* Reset seek position */
	fd->fd.seek_pos = 0;

	/* Insert file handle in list, ordered by inode, ascending. */
	FOREACH_NODE(n, &disk->file_opened_list)
	{
		BattFs *file = containerof(n, BattFs, link);
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
	fd->fd.write = battfs_write;

#warning TODO battfs_error, battfs_clearerr
#if 0
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
		BattFs *file = containerof(n, BattFs, link);
		res += battfs_fileclose(&file->fd);
	}

	/* Close disk */
	return disk->close(disk) && (res == 0);
}

#if UNIT_TEST
bool battfs_writeTestBlock(struct BattFsSuper *disk, pgcnt_t page, inode_t inode, seq_t seq, fill_t fill, pgoff_t pgoff)
{
	BattFsPageHeader hdr;

	/* Reset page to all 0xff */
	uint8_t buf[disk->page_size];
	memset(buf, 0xFF, disk->page_size);
	disk->bufferWrite(disk, 0, buf, disk->page_size);

	hdr.inode = inode;
	hdr.fill = fill;
	hdr.pgoff = pgoff;
	hdr.seq = seq;
	hdr.fcs = computeFcs(&hdr);

	if (!battfs_writeHeader(disk, page, &hdr))
	{
		LOG_ERR("error writing hdr\n");
		return false;
	}

	return true;
}
#endif
