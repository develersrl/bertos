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
 * \brief Virtual KFile I/O interface.
 * This module implement a standard fd.seek and a kfile
 * test function.
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 *
 */

#include <kern/kfile.h>
#include <cfg/debug.h>

#include <string.h>

#include <appconfig.h>

/**
 * Move \a fd file seek position of \a offset bytes
 * from current position.
 * This is a generic implementation of seek function, you should redefine
 * it in your local module.
 */
kfile_off_t kfile_seek(struct _KFile *fd, kfile_off_t offset, KSeekMode whence)
{
	uint32_t seek_pos;

	switch(whence)
	{

	case KSM_SEEK_SET:
		seek_pos = 0;
		break;
	case KSM_SEEK_END:
		seek_pos = fd->size - 1;
		break;
	case KSM_SEEK_CUR:
		seek_pos = fd->seek_pos;
		break;
	default:
		ASSERT(0);
		return -1;
		break;

	}

	/* Bound check */
	if (seek_pos + offset > fd->size)
	{
		ASSERT(0);
		return -1;
	}

	fd->seek_pos = seek_pos + offset;
	kprintf("Flash seek to [%lu]\n", fd->seek_pos);

	return fd->seek_pos;
}

#if CONFIG_TEST

/**
 * Program memory read/write subtest.
 * Try to write/read in the same \a f file location \a _size bytes.
 * \return true if all is ok, false otherwise
 * \note Restore file position at exit (if no error)
 * \note Test buffer \a buf must be filled with
 * the following statement:
 * <pre>
 * buf[i] = i & 0xff
 * </pre>
 */
static bool kfile_rwTest(KFile *f, uint8_t *buf, size_t _size)
{
	int32_t size = _size;

	// Write test buffer
	if (f->write(f, buf, size) != size)
		return false;
	f->seek(f, -size, KSM_SEEK_CUR);

	// Reset test buffer
	memset(buf, 0, size);

	// Read flash in test buffer
	if (f->read(f, buf, size) != size)
		return false;
	f->seek(f, -size, KSM_SEEK_CUR);

	// Check test result
 	for (size_t i = 0; i < size; i++)
 		if (buf[i] != (i & 0xff))
			return false;

	return true;
}

/**
 * Test for program memory read/write.
 * This function write and read \p test_buf long \p _size
 * on \p fd handler. If you want not overwrite exist data
 * you should pass an \p save_buf where test store exist data,
 * otherwise su must pass NULL.
 */
bool kfile_test(KFile *fd, uint8_t *test_buf, size_t _size , uint8_t *save_buf, size_t save_buf_size)
{
	int32_t size = _size;

	/*
	 * Part of test buf size that you would write.
	 * This var is useded in test 3 to check fd.write
	 * when write outside size limit. Normaly we want
	 * perform a write until is space to write, otherwise
	 * we return.
	 */
	int32_t len = size/2;


	/* Fill test buffer */
 	for (size_t i = 0; i < size; i++)
 		test_buf[i] = (i & 0xff);

	/*
	 * Open fd handler
	 */
	fd->open(fd, NULL, 0);
	kprintf("Opened fd handler..\n");

	/*
	 * If necessary, user could save content,
	 * for later restore.
	 */
	if (save_buf != NULL)
	{
		fd->read(fd, save_buf, save_buf_size);
		kprintf("Saved content..form [%lu] to [%lu]\n", fd->seek_pos, fd->seek_pos + save_buf_size);
	}

	/* TEST 1 BEGIN. */
	kprintf("Test 1: write from pos 0 to [%lu]\n", size);

	/*
	 * Seek to addr 0
	 */
	if (fd->seek(fd, 0, KSM_SEEK_SET) != 0)
		goto kfile_test_end;

	/*
	 * Test flash read/write to address 0..size
	 */
	if (!kfile_rwTest(fd, test_buf, size))
		goto kfile_test_end;

	kprintf("Test 1: ok!\n");

	/*
	 * Restore previous read content
	 */
	if (save_buf != NULL)
	{
		fd->seek(fd, 0, KSM_SEEK_SET);

		if (fd->write(fd, save_buf, save_buf_size) != size)
			goto kfile_test_end;

		kprintf("Restore content..form [%lu] to [%lu]\n", fd->seek_pos, fd->seek_pos + save_buf_size);
	}
	/* TEST 1 END. */

	/* TEST 2 BEGIN. */
	kprintf("Test 2: write from pos [%lu] to [%lu]\n", fd->size/2 , size);

	/*
	 * Go to half test size.
	 */
	fd->seek(fd, (fd->size/ 2), KSM_SEEK_SET);

	/*
	 * If necessary, user could save content,
	 * for later restore.
	 */
	if (save_buf != NULL)
	{
		fd->read(fd, save_buf, save_buf_size);
		fd->seek(fd, -size, KSM_SEEK_CUR);
		kprintf("Saved content..form [%lu] to [%lu]\n", fd->seek_pos, fd->seek_pos + save_buf_size);
	}

	/*
	 * Test flash read/write to address FLASHEND/2 ... FLASHEND/2 + size
	 */
	if (!kfile_rwTest(fd, test_buf, size))
		goto kfile_test_end;

	kprintf("Test 2: ok!\n");

	/*
	 * Restore previous read content
	 */
	if (save_buf != NULL)
	{
		fd->seek(fd, -size, KSM_SEEK_CUR);

		if (fd->write(fd, save_buf, save_buf_size) != size)
			goto kfile_test_end;

		kprintf("Restore content..form [%lu] to [%lu]\n", fd->seek_pos, fd->seek_pos + save_buf_size);
	}

	/* TEST 2 END. */

	/* TEST 3 BEGIN. */
	kprintf("Test 3: write outside of fd->size limit [%lu]\n", fd->size);

	/*
	 * Go to the Flash end
	 */
	fd->seek(fd, -len, KSM_SEEK_END);

	/*
	 * If necessary, user could save content,
	 * for later restore.
	 */
	if (save_buf != NULL)
	{
		ASSERT(len > save_buf_size);

		fd->read(fd, save_buf, len);
		fd->seek(fd, -len, KSM_SEEK_CUR);
		kprintf("Saved content..form [%lu] to [%lu]\n", fd->seek_pos, fd->seek_pos + len);
	}

	/*
	 * Test flash read/write to address (FLASHEND - size) ... FLASHEND
	 */
	if (kfile_rwTest(fd, test_buf, size))
		goto kfile_test_end;

	kprintf("Test 3: ok !\n");

	/*
	 * Restore previous read content
	 */
	if (save_buf != NULL)
	{
		fd->seek(fd, -len, KSM_SEEK_END);

		if (fd->write(fd, save_buf, len) != len)
			goto kfile_test_end;

		kprintf("Restore content..form [%lu] to [%lu]\n", fd->seek_pos, fd->seek_pos + len);
	}

	/* TEST 3 END. */

	fd->close(fd);
	return true;

kfile_test_end:
	fd->close(fd);
	return false;
}

#endif /* CONFIG_TEST */
