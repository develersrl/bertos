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
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief KBlock interface
 */

#ifndef IO_KBLOCK_H
#define IO_KBLOCK_H

#include <cfg/compiler.h>
#include <cfg/debug.h>
#include <cfg/macros.h>

/** Type for addressing blocks in the device. */
typedef uint32_t block_idx_t;

// Fwd Declaration
struct KBlock;

/**
 * \name Prototypes for KBlock access functions.
 *
 * A KBlock user can choose which function subset to implement,
 * but has to set to NULL unimplemented features.
 *
 *  \{
 */
typedef size_t (* kblock_read_direct_t) (struct KBlock *b, block_idx_t index, void *buf, size_t offset, size_t size);
typedef size_t (* kblock_read_t)        (struct KBlock *b, void *buf, size_t offset, size_t size);
typedef size_t (* kblock_write_t)       (struct KBlock *b, const void *buf, size_t offset, size_t size);
typedef int    (* kblock_load_t)        (struct KBlock *b, block_idx_t index);
typedef int    (* kblock_store_t)       (struct KBlock *b, block_idx_t index);

typedef int    (* kblock_write_block_t) (struct KBlock *b, block_idx_t index, const void *buf);
typedef int    (* kblock_read_block_t)  (struct KBlock *b, block_idx_t index, void *buf);

typedef int    (* kblock_error_t)       (struct KBlock *b);
typedef int    (* kblock_clearerr_t)    (struct KBlock *b);
typedef int    (* kblock_close_t)       (struct KBlock *b);
/* \} */

/**
 * Table of interface functions for a KBlock device.
 */
typedef struct KBlockVTable
{
	kblock_read_direct_t readDirect;

	kblock_read_t  readBuf;
	kblock_write_t writeBuf;
	kblock_load_t  load;
	kblock_store_t store;
	
	kblock_read_block_t readBlock;
	kblock_write_block_t writeBlock;
	
	kblock_error_t    error;    ///< \sa kblock_error()
	kblock_clearerr_t clearerr; ///< \sa kblock_clearerr()

	kblock_close_t  close; ///< \sa kblock_close()
} KBlockVTable;


#define KB_BUFFERED BV(0)

/**
 * KBlock private members.
 * These are the private members of the KBlock class, please do not
 * access these directly, use the KBlock API.
 */
typedef struct KBlockPriv
{
	DB(id_t type);         ///< Used to keep track, at runtime, of the class type.
	int flags;             ///< Status and error flags.
	void *buf;
	block_idx_t blk_start; ///< Start block number when the device is trimmed. \sa kblock_trim()
	block_idx_t curr_blk;
	bool cache_dirty;

	const struct KBlockVTable *vt; ///< Virtual table of interface functions.
} KBlockPriv;

/**
 * KBlock: interface for a generic block device.
 *
 * A block device is a device which can only be read/written
 * with data blocks of constant size: flash memories,
 * SD cards, hard disks, etc...
 *
 * This interface is designed to adapt to most block devices and
 * use peculiar features in order to save CPU time and memory space.
 *
 * You do not have to use this structure directly, specific implementations
 * will be supplied in the peripheral drivers.
 */
typedef struct KBlock
{
	KBlockPriv priv;         ///< Interface private data, do not use directly.

	/* Public access members/methods */
	size_t blk_size;         ///< Block size.
	block_idx_t blk_cnt;     ///< Number of blocks available in the device.
} KBlock;


/**
 * Use a subset of the blocks on the device.
 *
 * This function is useful for partitioning a device and use it for
 * different purposes at the same time.
 *
 * This function will limit the number of blocks used on the device by setting
 * a start index and a number of blocks to be used counting from that index.
 *
 * The blocks outside this range are no more accessible.
 *
 * Logical block indexes will be mapped to physical indexes inside this new
 * range automatically. Even following calls to kblock_trim() will use logical
 * indexes, so, once trimmed, access can only be limited further and never
 * expanded back.
 *
 * Example:
 * \code
 * //...init KBlock device dev
 * kblock_trim(dev, 200, 1500); // Restrict access to the 200-1700 physical block range.
 * kblock_load(dev, 0);  // Load the physical block #200.
 * kblock_trim(dev, 0, 300); // Restrict access to the 200-500 physical block range.
 * \endcode
 *
 * \param b KBlock device.
 * \param start The index of the start block for the limiting window in logical addressing units.
 * \param count The number of blocks to be used.
 *
 */
INLINE void kblock_trim(struct KBlock *b, block_idx_t start, block_idx_t count)
{
	ASSERT(start + count <= b->blk_cnt);
	b->priv.blk_start += start;
	b->blk_cnt = count;
}


#define KB_ASSERT_METHOD(b, method) \
	do \
	{ \
		ASSERT(b); \
		ASSERT((b)->priv.vt); \
		ASSERT((b)->priv.vt->method); \
	} \
	while (0)


/**
 * Get the current errors for the device.
 *
 * \note Calling this function will not clear the errors.
 *
 * \param b KBlock device.
 *
 * \return 0 if no error is present, a driver specific mask of errors otherwise.
 *
 * \sa kblock_clearerr()
 */
INLINE int kblock_error(struct KBlock *b)
{
	KB_ASSERT_METHOD(b, error);
	return b->priv.vt->error(b);
}

/**
 * Clear the errors of the device.
 *
 * \param b KBlock device.
 *
 * \return 0 on success, EOF on errors.
 *
 * \sa kblock_error()
 */
INLINE int kblock_clearerr(struct KBlock *b)
{
	KB_ASSERT_METHOD(b, clearerr);
	return b->priv.vt->clearerr(b);
}

/**
 * Close the device.
 *
 * \param b KBlock device.
 *
 * \return 0 on success, EOF on errors.
 */
INLINE int kblock_close(struct KBlock *b)
{
	KB_ASSERT_METHOD(b, close);
	return b->priv.vt->close(b);
}

INLINE int kblock_writeBlock(struct KBlock *b, block_idx_t index, const void *buf)
{
	KB_ASSERT_METHOD(b, writeBlock);
	ASSERT(index < b->blk_cnt);
	return b->priv.vt->writeBlock(b, b->priv.blk_start + index, buf);
}

INLINE int kblock_readBlock(struct KBlock *b, block_idx_t index, void *buf)
{
	KB_ASSERT_METHOD(b, readDirect);
	ASSERT(index < b->blk_cnt);
	return b->priv.vt->readBlock(b, b->priv.blk_start + index, buf);
}

INLINE block_idx_t kblock_cachedBlock(struct KBlock *b)
{
	return b->priv.curr_blk;
}

INLINE bool kblock_buffered(struct KBlock *b)
{
	ASSERT(b);
	return (b->priv.flags & KB_BUFFERED);
}

size_t kblock_read(struct KBlock *b, block_idx_t idx, void *buf, size_t offset, size_t size);

int kblock_flush(struct KBlock *b);

size_t kblock_write(struct KBlock *b, block_idx_t idx, const void *buf, size_t offset, size_t size);

int kblock_copy(struct KBlock *b, block_idx_t idx1, block_idx_t idx2);


int kblock_swWriteBlock(struct KBlock *b, block_idx_t index, const void *buf);
int kblock_swReadBlock(struct KBlock *b, block_idx_t index, void *buf);

size_t kblock_swReadDirect(struct KBlock *b, block_idx_t index, void *buf, size_t offset, size_t size);
int kblock_swLoad(struct KBlock *b, block_idx_t index);
int kblock_swStore(struct KBlock *b, block_idx_t index);
size_t kblock_swReadBuf(struct KBlock *b, void *buf, size_t offset, size_t size);
size_t kblock_swWriteBuf(struct KBlock *b, const void *buf, size_t offset, size_t size);

#endif /* IO_KBLOCK_H */
