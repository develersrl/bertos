/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Flash memory programmar interface.
 *
 * \version $Id$
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 */

#include "prog.h"

#include <string.h>

#include <cfg/os.h>
#include <cfg/cpu.h>
#include <cfg/compiler.h>

#include <cfg/debug.h>

/*
 * Include platform-specific binding code if we're hosted.
 * Try the CPU specific one for bare-metal environments.
 */
#if OS_HOSTED
	#include OS_CSOURCE(prog)
#else
	#include CPU_CSOURCE(prog)
#endif


#define TEST_SIZE 683
#define ONE_BYTE_TEST_ADDRESS 347

uint8_t test_buf[TEST_SIZE];
uint8_t save_buf[TEST_SIZE];

/**
 * Program memory read/write subtest.
 * Try to write/read in the same \param f file location \param _size bytes.
 * \return true if all is ok, false otherwise
 * \note Restore file position at exit (if no error)
 * \note Test buffer \param buf must be filled with
 * the following statement:
 * <pre>
 * buf[i] = i & 0xff
 * </pre>
 */
static bool prog_rwTest(KFile *f, uint8_t *buf, size_t _size)
{
	int32_t size = _size;
	// Write test buffer
	if (f->write(f, buf, size) != size)
		return false;
	f->seek(f, -size);

	// Reset test buffer
	memset(buf, 0, size);

	// Read flash in test buffer
	if (f->read(f, buf, size) != size)
		return false;
	f->seek(f, -size);

	// Check test result
 	for (size_t i = 0; i < size; i++)
 		if (buf[i] != (i & 0xff))
			return false;

	return true;
}

/**
 * Test for program memory read/write.
 */
bool prog_test(void)
{
	KFile fd;

	// Set up flash programming functions.
	fd.open = prog_open;
	fd.close = prog_close;
	fd.read = prog_read;
	fd.write = prog_write;
	fd.seek = prog_seek;

	// Fill in test buffer
	for (int i = 0; i < TEST_SIZE; i++)
		test_buf[i] = (i & 0xff);

	// Open flash
	fd.open(&fd, NULL, 0);
	// Save flash content (for later restore).
	fd.read(&fd, save_buf, sizeof(save_buf));
	fd.seek(&fd, -TEST_SIZE);

	// Test flash read/write to address 0..TEST_SIZE
	if (!prog_rwTest(&fd, test_buf, TEST_SIZE))
		goto prog_test_end;

	// One byte read/write test
	fd.seek(&fd, ONE_BYTE_TEST_ADDRESS); // Random address
	if (!prog_rwTest(&fd, test_buf, 1))
		goto prog_test_end;
	fd.seek(&fd, -(int32_t)ONE_BYTE_TEST_ADDRESS);

	// Restore old flash data
	if (fd.write(&fd, save_buf, sizeof(test_buf)) != TEST_SIZE)
		goto prog_test_end;
	fd.seek(&fd, -TEST_SIZE);

	// Go to the Flash end
	fd.seek(&fd, fd.size - TEST_SIZE);
	// Save flash content (for later restore).
	fd.read(&fd, save_buf, sizeof(save_buf));
	fd.seek(&fd, -TEST_SIZE);

	// Test flash read/write to address (FLASHEND - TEST_SIZE) ... FLASHEND
	if (!prog_rwTest(&fd, test_buf, TEST_SIZE))
		goto prog_test_end;

	// Go to half test size.
	fd.seek(&fd, (TEST_SIZE / 2));

	// This test should FAIL, cause we try to write over file end.
	if (prog_rwTest(&fd, test_buf, TEST_SIZE))
		goto prog_test_end;

	fd.seek(&fd, -TEST_SIZE);
	// Restore old flash data
	fd.write(&fd, save_buf, TEST_SIZE);

	fd.close(&fd);
	return true;

prog_test_end:
	fd.close(&fd);
	return false;
}
