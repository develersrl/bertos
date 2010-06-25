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
typedef size_t (* kblock_read_t)    (struct KBlock *b, void *buf, size_t offset, size_t size);
typedef size_t (* kblock_write_t)   (struct KBlock *b, const void *buf, size_t offset, size_t size);
typedef int    (* kblock_load_t)    (struct KBlock *b, block_idx_t index);
typedef int    (* kblock_store_t)   (struct KBlock *b, block_idx_t index);
typedef void * (* kblock_map_t)     (struct KBlock *b, size_t offset, size_t size);
typedef int    (* kblock_unmap_t)   (struct KBlock *b, size_t offset, size_t size);
typedef int    (* kblock_error_t)   (struct KBlock *b);
typedef int    (* kblock_clearerr_t)(struct KBlock *b);
typedef int    (* kblock_close_t)   (struct KBlock *b);
/* \} */

/**
 * Table of interface functions for a KBlock device.
 */
typedef struct KBlockVTable
{
    kblock_read_t  readBuf;  ///< \sa kblock_readBuf()
	kblock_write_t writeBuf; ///< \sa kblock_writeBuf()
	kblock_load_t  load;     ///< \sa kblock_load()
	kblock_store_t store;    ///< \sa kblock_store()

	kblock_map_t   map;   ///< \sa kblock_map()
	kblock_unmap_t unmap; ///< \sa kblock_unmap()
	
	kblock_error_t    error;    ///< \sa kblock_error()
	kblock_clearerr_t clearerr; ///< \sa kblock_clearerr()
	
	kblock_close_t  close; ///< \sa kblock_close()
} KBlockVTable;


/**
 * KBlock status and error codes.
 */
typedef enum KBlockStatus
{
	/* Status flags */
	KBS_MAPPED,  ///< Status: The current loaded block from the device is memory mapped.
	
	/* Errors */
	KBS_ERR_ALREADY_MAPPED,    ///< Error: trying to memory map a block already mapped.
	KBS_ERR_NOT_MAPPED,        ///< Error: trying to memory unmap a block not yet mapped.
	KBS_ERR_MAP_NOT_AVAILABLE, ///< Error: mapping methods not implemented.

	#define KBS_STATUS_MASK (BV(KBS_MAPPER) | 0 /* Add status flags here */)
	
	#define KBS_ERROR_MASK (BV(KBS_ERR_ALREADY_MAPPED) | BV(KBS_ERR_ALREADY_MAPPED) \
		| BV(KBS_ERR_MAP_NOT_AVAILABLE) | 0 /* Add error flags here */)
}  KBlockStatus;


/**
 * KBlock private members.
 * These are the private members of the KBlock class, please do not
 * access these directly, use the KBlock API.
 */ 
typedef struct KBlockPriv
{
	DB(id_t type);         ///< Used to keep track, at runtime, of the class type.
	void *pagebuf;         ///< Pointer to a buffer used as page buffer when memory mapping is active. \sa kblock_map(), kblock_unmap()
	size_t pagebuf_size;   ///< Size of the page buffer used for memory mapping. \sa kblock_map(), kblock_unmap()
	KBlockStatus flags;    ///< Status and error flags.
	block_idx_t blk_start; ///< Start block number when the device is trimmed. \sa kblock_trim()
	DB(size_t map_off);    ///< When mapping is active, this is the mapped data offset inside the block. \sa kblock_map(), kblock_unmap()
	DB(size_t map_size);   ///< When mapping is active, this is the mapped data size inside the block. \sa kblock_map(), kblock_unmap()
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
	struct KBlockVTable *vt; ///< Virtual table of interface functions.
} KBlock;


/**
 * Add generic memory mapping functionality to a block device.
 * 
 * If the device has an hardware page buffer mechanism, the map/unmap
 * functions are unimplemented.
 * If you need to use the mapping functions of such device, this function
 * will add generic software mapping features wrapping the KBlock methods.
 * 
 * \param dev the block device.
 * \param buf the buffer to be used as page buffer for memory mapping functions.
 * \param size the size of the buffer. This is the maximum size that can be
 *        memory mapped. If you want to map a full block, a size of at least
 *        dev->blk_size have to be supplied.
 * 
 * \sa kblock_map(), kblock_unmap(), kblock_readBuf(), kblock_writeBuf()
 */
void kblock_addMapping(struct KBlock *dev, void *buf, size_t size);

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

/**
 * Transfer data from the internal page buffer to user memory.
 * 
 * This function accesses the internal page buffer of the block device and copy
 * the data to \a buf. The content is copied from the current cached block.
 * 
 * \param b KBlock device.
 * \param buf User buffer to copy the data to.
 * \param offset Address offset within the block, from which to copy data.
 * \param size Size, in bytes, of the data to be copied.
 * 
 * \return The number of bytes copied. Can be less than \a size on errors.
 * 
 * \sa kblock_writeBuf()
 */
INLINE size_t kblock_readBuf(struct KBlock *b, void *buf, size_t offset, size_t size)
{
	ASSERT(b->vt);
	ASSERT(b->vt->readBuf);
	ASSERT(offset + size <= b->blk_size);
	
	return b->vt->readBuf(b, buf, offset, size);
}

/**
 * Write to the page buffer.
 * 
 * Copies data from user memory to the device page buffer. The data is written
 * in the current cached block buffer.
 * 
 * \param b KBlock device.
 * \param buf User buffer to copy the data from.
 * \param offset Address offset within the block, from which data has to be written.
 * \param size Size, in bytes, of the data to be written.
 * 
 * \return The number of bytes written. Can be less than \a size on errors.
 * 
 * \sa kblock_readBuf()
 */
INLINE size_t kblock_writeBuf(struct KBlock *b, const void *buf, size_t offset, size_t size)
{
	ASSERT(b->vt);
	ASSERT(b->vt->writeBuf);
	ASSERT(offset + size <= b->blk_size);
	return b->vt->writeBuf(b, buf, offset, size);
}

/**
 * Load a block from the device to the page buffer.
 * 
 * The block \a index will be loaded in the internal page buffer.
 * 
 * \param b KBlock device.
 * \param index Logical index of the block to be loaded.
 * 
 * \return 0 on success, EOF on errors.
 */
INLINE int kblock_load(struct KBlock *b, block_idx_t index)
{
	ASSERT(b->vt);
	ASSERT(b->vt->load);
	ASSERT(index < b->blk_cnt);
	
	return b->vt->load(b, b->priv.blk_start + index);
}

/**
 * Store a block from the page buffer to the device.
 * 
 * The current content of the page buffer will be flushed to the block \a index.
 * 
 * \param b KBlock device.
 * \param index Logical index of the block to be stored.
 * 
 * \return 0 on success, EOF on errors.
 */
INLINE int kblock_store(struct KBlock *b, block_idx_t index)
{
	ASSERT(b->vt);
	ASSERT(b->vt->store);
	ASSERT(index < b->blk_cnt);
	
	return b->vt->store(b, b->priv.blk_start + index);
}


/**
 * Memory map the current page buffer.
 * 
 * To speed up access, instead of using kblock_readBuf() and kblock_writeBuf(),
 * you can memory map the page buffer and access it directly through the
 * returned pointer. You can freely access the pointer in any way you
 * like. Once done, call kblock_unmap() to release the lock on the page_buffer.
 * 
 * \note This function may be not available on all drivers, since the page
 *       buffer can be in the hardware and not directly accessible through memory.
 *       For this devices you can still add generic software mapping features
 *       thanks to kblock_addMapping().
 * 
 * \note Only one mapping is available at a time, trying to map the page buffer
 *       again before releasing it is an error.
 * 
 * \param b KBlock device.
 * \param offset Address offset within the page buffer, from which data has to 
 *               be memory mapped.
 * \param size Size of the memory to be mapped.
 * 
 * \return A pointer to the mapped region of the page buffer or NULL on errors.
 * 
 * \sa kblock_addMapping(), kblock_unmap()
 */
INLINE void * kblock_map(struct KBlock *b, size_t offset, size_t size)
{
	ASSERT(b->vt);
	ASSERT(b->vt->map);
	
	if (b->priv.flags & BV(KBS_MAPPED))
	{
		b->priv.flags |= BV(KBS_ERR_ALREADY_MAPPED);
		return NULL;
	}
	
	ASSERT(size < b->priv.pagebuf_size);
	ASSERT(offset + size <= b->blk_size);
	DB(b->priv.map_off = offset);
	DB(b->priv.map_size = size);

	void *ret = b->vt->map(b, offset, size);
	
	if (ret)
		b->priv.flags |= BV(KBS_MAPPED);

	return ret;
}


/**
 * Release the memory map on the page buffer.
 * 
 * This function has to be called when memory mapped access has finished.
 * This is needed because only one mapping is allowed at a time.
 * The \a offset and \a size passed should be the same passed to 
 * kblock_map() when the page buffer has been mapped.
 * 
 * \note Trying to unmap the page buffer when there is no mapping ongoing is
 *       an error.
 * 
 * \param b KBlock device.
 * \param offset Address offset within the page buffer, from which data has been
 *               memory mapped. Must be the same value passed to kblock_map()
 *               when the memory was mapped.
 * \param size Size of the memory mapped. Must be the same value passed to
 *             kblock_map() when the memory was mapped.
 * 
 * \return 0 on success, EOF on errors.
 * 
 * \sa kblock_addMapping(), kblock_map()
 */ 
INLINE int kblock_unmap(struct KBlock *b, size_t offset, size_t size)
{
	ASSERT(b->vt);
	ASSERT(b->vt->unmap);
	
	if (!(b->priv.flags & BV(KBS_MAPPED)))
	{
		b->priv.flags |= BV(KBS_ERR_NOT_MAPPED);
		return EOF;
	}
	
	ASSERT(b->priv.map_off == offset);
	ASSERT(b->priv.map_size == size);
	int ret = b->vt->unmap(b, offset, size);
	
	if (ret == 0)
		b->priv.flags &= ~BV(KBS_MAPPED);
	return ret;
}

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
	ASSERT(b->vt);
	ASSERT(b->vt->error);
	/* Automatically mask status flags */
	return b->vt->error(b) & ~KBS_STATUS_MASK;
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
	ASSERT(b->vt);
	ASSERT(b->vt->clearerr);
	/* Automatically clear error flags */
	b->priv.flags &= ~KBS_ERROR_MASK;
	return b->vt->clearerr(b);
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
	ASSERT(b->vt);
	ASSERT(b->vt->close);
	return b->vt->close(b);
}

#endif /* IO_KBLOCK_H */
