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
 * Copyright 2007, 2008 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief BattFS Test.
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

#include <fs/battfs.h>

#include <cfg/debug.h>
#include <cfg/test.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_SIZE 32768
#define PAGE_SIZE 128
#define PAGE_COUNT FILE_SIZE / PAGE_SIZE

#if UNIT_TEST

FILE *fp;
const char test_filename[]="battfs_disk.bin";

static uint8_t page_buffer[PAGE_SIZE];

static bool disk_open(struct BattFsSuper *d)
{
	fp = fopen(test_filename, "r+b");
	ASSERT(fp);
	fseek(fp, 0, SEEK_END);
	d->page_size = PAGE_SIZE;
	d->page_count = ftell(fp) / d->page_size;
	d->page_array = malloc(d->page_count * sizeof(pgcnt_t));
	//TRACEMSG("page_size:%d, page_count:%d\n", d->page_size, d->page_count);
	return (fp && d->page_array);
}

static size_t disk_page_read(struct BattFsSuper *d, pgcnt_t page, pgaddr_t addr, void *buf, size_t size)
{
	//TRACEMSG("page:%d, addr:%d, size:%d", page, addr, size);
	fseek(fp, page * d->page_size + addr, SEEK_SET);
	return fread(buf, 1, size, fp);
}

static size_t disk_buffer_write(struct BattFsSuper *d, pgaddr_t addr, const void *buf, size_t size)
{
	//TRACEMSG("addr:%d, size:%d", addr, size);
	ASSERT(addr + size <= d->page_size);
	memcpy(&page_buffer[addr], buf, size);

	return size;
}

static size_t disk_buffer_read(struct BattFsSuper *d, pgaddr_t addr, void *buf, size_t size)
{
	//TRACEMSG("addr:%d, size:%d", addr, size);
	ASSERT(addr + size <= d->page_size);
	memcpy(buf, &page_buffer[addr], size);

	return size;
}

static bool disk_page_load(struct BattFsSuper *d, pgcnt_t page)
{
	//TRACEMSG("page:%d", page);
	fseek(fp, page * d->page_size, SEEK_SET);
	return fread(page_buffer, 1, d->page_size, fp) == d->page_size;
}

static bool disk_page_save(struct BattFsSuper *d, pgcnt_t page)
{
	//TRACEMSG("page:%d", page);
	fseek(fp, page * d->page_size, SEEK_SET);
	return fwrite(page_buffer, 1, d->page_size, fp) == d->page_size;
}

static bool disk_page_erase(struct BattFsSuper *d, pgcnt_t page)
{
	//TRACEMSG("page:%d", page);
	fseek(fp, page * d->page_size, SEEK_SET);

	for (int i = 0; i < d->page_size; i++)
		if (fputc(0xff, fp) == EOF)
			return false;
	return true;
}

static bool disk_close(struct BattFsSuper *d)
{
	//TRACE;
	free(d->page_array);
	return (fclose(fp) != EOF);
}

static void testCheck(BattFsSuper *disk, pgcnt_t *reference)
{
	ASSERT(battfs_init(disk));

	for (int i = 0; i < disk->page_count; i++)
	{
		if (disk->page_array[i] != reference[i])
		{
			kprintf("Error at addr %d: page_array read", i);
			for (pgcnt_t i = 0; i < disk->page_count; i++)
			{
				if (!(i % 16))
					kputchar('\n');
				kprintf("%04d ", disk->page_array[i]);
			}
			kputchar('\n');
			kprintf("Expected:");
			for (pgcnt_t i = 0; i < disk->page_count; i++)
			{
				if (!(i % 16))
					kputchar('\n');
				kprintf("%04d ", reference[i]);
			}
			kputchar('\n');
			battfs_close(disk);
			exit(2);
		}
	}
	battfs_close(disk);
}

static void test1(BattFsSuper *disk)
{
	pgcnt_t ref[PAGE_COUNT];
	kprintf("Test1: disk new\n");

	FILE *fpt = fopen(test_filename, "w+");

	for (int i = 0; i < FILE_SIZE; i++)
		fputc(0xff, fpt);
	fclose(fpt);
	for (int i = 0; i < PAGE_COUNT; i++)
		ref[i] = i;

	testCheck(disk, ref);
	kprintf("Test1: passed\n");
}

static void test2(BattFsSuper *disk)
{
	pgcnt_t ref[PAGE_COUNT];
	kprintf("Test2: disk full with 1 contiguos file\n");


	fp = fopen(test_filename, "w+");

	for (int i = 0; i < PAGE_COUNT; i++)
	{
		battfs_writeTestBlock(disk, i, 0, 0, 0, i);
		ref[i] = i;
	}
	fclose(fp);

	testCheck(disk, ref);
	kprintf("Test2: passed\n");
}


static void test3(BattFsSuper *disk)
{
	pgcnt_t ref[PAGE_COUNT];
	kprintf("Test3: disk half full with 1 contiguos file, rest unformatted\n");


	fp = fopen(test_filename, "w+");

	for (int i = 0; i < PAGE_COUNT / 2; i++)
	{
		battfs_writeTestBlock(disk, i, 0, 0, 0, i);
		ref[i] = i;
	}
	fseek(fp, FILE_SIZE / 2, SEEK_SET);
	for (int i = FILE_SIZE / 2; i < FILE_SIZE; i++)
		fputc(0xff, fp);
	fclose(fp);

	for (int i = PAGE_COUNT / 2; i < PAGE_COUNT; i++)
	{
		ref[i] = i;
	}


	testCheck(disk, ref);
	kprintf("Test3: passed\n");
}

#if 0
static void test4(BattFsSuper *disk)
{
	pgcnt_t ref[PAGE_COUNT];
	kprintf("Test4: disk half full with 1 contiguos file, rest marked free\n");


	fp = fopen(test_filename, "w+");

	for (int i = 0; i < PAGE_COUNT / 2; i++)
	{
		battfs_writeTestBlock(disk, i, 0, 0, 0, i);
		ref[i] = i;
	}
	for (int i = PAGE_COUNT / 2; i < PAGE_COUNT; i++)
	{
		battfs_writeTestBlock(disk, i, 0, 0, 0, i);
		ref[i] = i;
	}
	fclose(fp);


	testCheck(disk, ref);
	kprintf("Test4: passed\n");
}

static void test5(BattFsSuper *disk)
{
	pgcnt_t ref[PAGE_COUNT];
	kprintf("Test5: disk 1/3 full with 1 contiguos file, 1/3 marked free, rest unformatted\n");


	fp = fopen(test_filename, "w+");

	for (int i = 0; i < FILE_SIZE; i++)
		fputc(0xff, fp);

	for (int i = 0; i < PAGE_COUNT / 3; i++)
	{
		battfs_writeTestBlock(disk, i, 0, 0, 0, i);
		ref[i] = i;
	}
	for (int i = PAGE_COUNT / 3; i < 2 * (PAGE_COUNT / 3); i++)
	{
		battfs_writeTestBlock(disk, i, 0, 0, 0, i);
		ref[i + PAGE_COUNT / 3 + 1] = i;
	}
	fclose(fp);

	for (int i = PAGE_COUNT / 3; i < 2 * (PAGE_COUNT / 3) + 1; i++)
		ref[i] = PAGE_COUNT + PAGE_COUNT / 3 - i - 1;

	testCheck(disk, ref);
	kprintf("Test5: passed\n");
}
#endif

static void test6(BattFsSuper *disk)
{
	pgcnt_t ref[4];
	kprintf("Test6: 1 file with 1 old seq num, 1 free block\n");


	fp = fopen(test_filename, "w+");
	// page, inode, seq, fill, pgoff
	battfs_writeTestBlock(disk, 0, 0, 0, 0, 0);
	battfs_writeTestBlock(disk, 1, 0, 0, 0, 1);
	battfs_writeTestBlock(disk, 2, 0, 1, 0, 1);
	disk->erase(disk, 3);


	fclose(fp);
	ref[0] = 0;
	ref[1] = 2;
	ref[2] = 1;
	ref[3] = 3;

	testCheck(disk, ref);
	kprintf("Test6: passed\n");
}

static void test7(BattFsSuper *disk)
{
	pgcnt_t ref[4];
	kprintf("Test7: 1 file with 1 old seq num, 1 free block\n");


	fp = fopen(test_filename, "w+");
	// page, inode, seq, fill, pgoff
	battfs_writeTestBlock(disk, 0, 0, 0, 0, 0);
	battfs_writeTestBlock(disk, 1, 0, 1, 0, 1);
	battfs_writeTestBlock(disk, 2, 0, 0, 0, 1);
	disk->erase(disk, 3);

	fclose(fp);
	ref[0] = 0;
	ref[1] = 1;
	ref[2] = 2;
	ref[3] = 3;

	testCheck(disk, ref);
	kprintf("Test7: passed\n");
}

static void test8(BattFsSuper *disk)
{
	pgcnt_t ref[4];
	kprintf("Test8: 1 file with 1 old seq num, 1 free block\n");


	fp = fopen(test_filename, "w+");

	// page, inode, seq, fill, pgoff
	disk->erase(disk, 0);
	battfs_writeTestBlock(disk, 1, 0, 0, 0, 0);
	battfs_writeTestBlock(disk, 2, 0, 1, 0, 1);
	battfs_writeTestBlock(disk, 3, 0, 0, 0, 1);


	fclose(fp);
	ref[0] = 1;
	ref[1] = 2;
	ref[2] = 0;
	ref[3] = 3;

	testCheck(disk, ref);
	kprintf("Test8: passed\n");
}

static void test9(BattFsSuper *disk)
{
	pgcnt_t ref[8];
	kprintf("Test9: 2 file with old seq num, 2 free block\n");


	fp = fopen(test_filename, "w+");

	// page, inode, seq, fill, pgoff
	disk->erase(disk, 0);
	battfs_writeTestBlock(disk, 1, 0, 0, 0, 0);
	battfs_writeTestBlock(disk, 2, 0, 3, 0, 1);
	battfs_writeTestBlock(disk, 3, 0, 0, 0, 1);
	disk->erase(disk, 4);
	battfs_writeTestBlock(disk, 5, 4, 0, 0, 0);
	battfs_writeTestBlock(disk, 6, 4, 1, 0, 1);
	battfs_writeTestBlock(disk, 7, 4, 0, 0, 1);


	fclose(fp);
	ref[0] = 1;
	ref[1] = 2;
	ref[2] = 5;
	ref[3] = 6;
	ref[4] = 0;
	ref[5] = 3;
	ref[6] = 4;
	ref[7] = 7;

	testCheck(disk, ref);
	kprintf("Test9: passed\n");
}

static void test10(BattFsSuper *disk)
{
	BattFs fd1;
	BattFs fd2;
	kprintf("Test10: open file test, inode 0 and inode 4\n");

	fp = fopen(test_filename, "w+");

	int PAGE_FILL = PAGE_SIZE - BATTFS_HEADER_LEN;
	unsigned int INODE = 0;
	unsigned int INODE2 = 4;
	unsigned int INEXISTENT_INODE = 123;
	unsigned int MODE = 0;

	// page, inode, seq, fill, pgoff
	disk->erase(disk, 0);
	battfs_writeTestBlock(disk, 1, INODE, 0, PAGE_FILL, 0);
	battfs_writeTestBlock(disk, 2, INODE, 3, PAGE_FILL, 1);
	battfs_writeTestBlock(disk, 3, INODE, 0, PAGE_FILL, 1);
	disk->erase(disk, 4);
	battfs_writeTestBlock(disk, 5, INODE2, 0, PAGE_FILL, 0);
	battfs_writeTestBlock(disk, 6, INODE2, 1, PAGE_FILL, 1);
	battfs_writeTestBlock(disk, 7, INODE2, 0, PAGE_FILL, 1);

	fclose(fp);

	ASSERT(battfs_init(disk));
	ASSERT(!battfs_fileExists(disk, INEXISTENT_INODE));

	ASSERT(battfs_fileExists(disk, INODE));
	ASSERT(battfs_fileopen(disk, &fd1, INODE, MODE));
	ASSERT(fd1.fd.size == PAGE_FILL * 2);
	ASSERT(fd1.fd.seek_pos == 0);
	ASSERT(fd1.mode == MODE);
	ASSERT(fd1.inode == INODE);
	ASSERT(fd1.start == &disk->page_array[0]);
	ASSERT(fd1.disk == disk);
	ASSERT(LIST_HEAD(&disk->file_opened_list) == &fd1.link);

	ASSERT(kfile_reopen(&fd1.fd) == &fd1.fd);
	ASSERT(fd1.fd.size == PAGE_FILL * 2);
	ASSERT(fd1.fd.seek_pos == 0);
	ASSERT(fd1.mode == MODE);
	ASSERT(fd1.inode == INODE);
	ASSERT(fd1.start == &disk->page_array[0]);
	ASSERT(fd1.disk == disk);
	ASSERT(LIST_HEAD(&disk->file_opened_list) == &fd1.link);

	ASSERT(battfs_fileExists(disk, INODE2));
	ASSERT(battfs_fileopen(disk, &fd2, INODE2, MODE));
	ASSERT(fd2.fd.size == PAGE_FILL * 2);
	ASSERT(fd2.fd.seek_pos == 0);
	ASSERT(fd2.mode == MODE);
	ASSERT(fd2.inode == INODE2);
	ASSERT(fd2.start == &disk->page_array[2]);
	ASSERT(fd2.disk == disk);
	ASSERT(LIST_HEAD(&disk->file_opened_list)->succ == &fd2.link);

	ASSERT(kfile_close(&fd1.fd) == 0);
	ASSERT(kfile_close(&fd2.fd) == 0);
	ASSERT(LIST_EMPTY(&disk->file_opened_list));
	ASSERT(battfs_close(disk));

	kprintf("Test10: passed\n");
}

static void test11(BattFsSuper *disk)
{
	BattFs fd1;
	uint8_t buf[16];

	kprintf("Test11: read file test\n");

	fp = fopen(test_filename, "w+");

	unsigned int PAGE_FILL = PAGE_SIZE - BATTFS_HEADER_LEN;
	unsigned int INODE = 0;
	unsigned int INODE2 = 4;
	unsigned int MODE = 0;

	disk->erase(disk, 0);
	battfs_writeTestBlock(disk, 1, INODE, 0, PAGE_FILL, 0);
	battfs_writeTestBlock(disk, 2, INODE, 3, PAGE_FILL, 1);
	battfs_writeTestBlock(disk, 3, INODE, 0, PAGE_FILL, 1);
	disk->erase(disk, 4);
	battfs_writeTestBlock(disk, 5, INODE2, 0, PAGE_FILL, 0);
	battfs_writeTestBlock(disk, 6, INODE2, 1, PAGE_FILL, 1);
	battfs_writeTestBlock(disk, 7, INODE2, 0, PAGE_FILL, 1);

	fclose(fp);

	ASSERT(battfs_init(disk));
	ASSERT(battfs_fileopen(disk, &fd1, INODE, MODE));
	ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	ASSERT(fd1.fd.seek_pos == sizeof(buf));
	for (size_t i = 0; i < sizeof(buf); i++)
		ASSERT(buf[i] == 0xff);

	ASSERT(kfile_close(&fd1.fd) == 0);
	ASSERT(battfs_close(disk));

	kprintf("Test11: passed\n");
}

static void test12(BattFsSuper *disk)
{
	BattFs fd1;

	kprintf("Test12: read file test across page boundary and seek test\n");

	fp = fopen(test_filename, "w+");

	const unsigned int PAGE_FILL = PAGE_SIZE - BATTFS_HEADER_LEN;
	unsigned int INODE = 0;
	unsigned int MODE = 0;
	uint8_t buf[PAGE_FILL + BATTFS_HEADER_LEN / 2];

	disk->erase(disk, 0);
	battfs_writeTestBlock(disk, 1, INODE, 0, PAGE_FILL, 0);
	battfs_writeTestBlock(disk, 2, INODE, 3, PAGE_FILL, 1);
	battfs_writeTestBlock(disk, 3, INODE, 0, PAGE_FILL, 1);
	disk->erase(disk, 4);
	battfs_writeTestBlock(disk, 5, INODE, 0, PAGE_FILL, 2);
	battfs_writeTestBlock(disk, 6, INODE, 1, PAGE_FILL, 3);
	battfs_writeTestBlock(disk, 7, INODE, 0, PAGE_FILL, 3);

	fclose(fp);

	ASSERT(battfs_init(disk));
	ASSERT(battfs_fileopen(disk, &fd1, INODE, MODE));

	ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	ASSERT(fd1.fd.seek_pos == (kfile_off_t)sizeof(buf));
	for (size_t i = 0; i < sizeof(buf); i++)
		ASSERT(buf[i] == 0xff);

	ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	ASSERT(fd1.fd.seek_pos == (kfile_off_t)sizeof(buf) * 2);
	for (size_t i = 0; i < sizeof(buf); i++)
		ASSERT(buf[i] == 0xff);

	ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	ASSERT(fd1.fd.seek_pos == (kfile_off_t)sizeof(buf) * 3);
	for (size_t i = 0; i < sizeof(buf); i++)
		ASSERT(buf[i] == 0xff);

	ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == PAGE_FILL * 4 - sizeof(buf) * 3);
	ASSERT(fd1.fd.seek_pos == (kfile_off_t)fd1.fd.size);
	for (size_t i = 0; i < PAGE_FILL * 4 - sizeof(buf) * 3; i++)
		ASSERT(buf[i] == 0xff);

	ASSERT(kfile_seek(&fd1.fd, 0, KSM_SEEK_SET) == 0);
	ASSERT(fd1.fd.seek_pos == 0);

	ASSERT(kfile_seek(&fd1.fd, 0, KSM_SEEK_END) == (kfile_off_t)fd1.fd.size);
	ASSERT(fd1.fd.seek_pos = (kfile_off_t)fd1.fd.size);

	ASSERT(kfile_close(&fd1.fd) == 0);
	ASSERT(battfs_close(disk));

	kprintf("Test12: passed\n");
}


static void test13(BattFsSuper *disk)
{
	BattFs fd1;
	uint8_t buf[PAGE_SIZE - BATTFS_HEADER_LEN];

	kprintf("Test13: write file test\n");

	fp = fopen(test_filename, "w+");

	unsigned int PAGE_FILL = PAGE_SIZE - BATTFS_HEADER_LEN;
	unsigned int INODE = 0;
	unsigned int INODE2 = 4;
	unsigned int MODE = 0;

	disk->erase(disk, 0);
	battfs_writeTestBlock(disk, 1, INODE, 0, PAGE_FILL, 0);
	battfs_writeTestBlock(disk, 2, INODE, 3, PAGE_FILL, 1);
	battfs_writeTestBlock(disk, 3, INODE, 0, PAGE_FILL, 1);
	disk->erase(disk, 4);
	battfs_writeTestBlock(disk, 5, INODE2, 0, PAGE_FILL, 0);
	battfs_writeTestBlock(disk, 6, INODE2, 1, PAGE_FILL, 1);
	battfs_writeTestBlock(disk, 7, INODE2, 0, PAGE_FILL, 1);

	fclose(fp);

	for (size_t i = 0; i < sizeof(buf); i++)
		buf[i] = i;

	ASSERT(battfs_init(disk));
	ASSERT(battfs_fileopen(disk, &fd1, INODE, MODE));
	ASSERT(kfile_write(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	ASSERT(fd1.fd.seek_pos == sizeof(buf));
	ASSERT(kfile_seek(&fd1.fd, 0, KSM_SEEK_SET) == 0);
	ASSERT(fd1.fd.seek_pos == 0);

	memset(buf, 0, sizeof(buf));
	ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	for (size_t i = 0; i < sizeof(buf); i++)
		ASSERT(buf[i] == i);

	ASSERT(kfile_close(&fd1.fd) == 0);
	ASSERT(battfs_close(disk));

	kprintf("Test13: passed\n");
}

static void test14(BattFsSuper *disk)
{
	BattFs fd1;

	kprintf("Test14: write file test across page boundary and seek test\n");

	fp = fopen(test_filename, "w+");

	const unsigned int PAGE_FILL = PAGE_SIZE - BATTFS_HEADER_LEN;
	unsigned int INODE = 0;
	unsigned int MODE = 0;
	uint8_t buf[PAGE_FILL + BATTFS_HEADER_LEN / 2];

	disk->erase(disk, 0);
	battfs_writeTestBlock(disk, 1, INODE, 0, PAGE_FILL, 0);
	battfs_writeTestBlock(disk, 2, INODE, 3, PAGE_FILL, 1);
	battfs_writeTestBlock(disk, 3, INODE, 0, PAGE_FILL, 1);
	disk->erase(disk, 4);
	battfs_writeTestBlock(disk, 5, INODE, 0, PAGE_FILL, 2);
	battfs_writeTestBlock(disk, 6, INODE, 1, PAGE_FILL, 3);
	battfs_writeTestBlock(disk, 7, INODE, 0, PAGE_FILL, 3);

	fclose(fp);

	ASSERT(battfs_init(disk));
	ASSERT(battfs_fileopen(disk, &fd1, INODE, MODE));

	uint8_t val = 0;
	for (size_t i = 0; i < sizeof(buf); i++)
		buf[i] = val++;
	ASSERT(kfile_write(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	ASSERT(fd1.fd.seek_pos == (kfile_off_t)sizeof(buf));

	for (size_t i = 0; i < sizeof(buf); i++)
		buf[i] = val++;
	ASSERT(kfile_write(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	ASSERT(fd1.fd.seek_pos == (kfile_off_t)sizeof(buf) * 2);

	for (size_t i = 0; i < sizeof(buf); i++)
		buf[i] = val++;
	ASSERT(kfile_write(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	ASSERT(fd1.fd.seek_pos == (kfile_off_t)sizeof(buf) * 3);

	ASSERT(kfile_seek(&fd1.fd, 0, KSM_SEEK_SET) == 0);
	ASSERT(fd1.fd.seek_pos == 0);
	val = 0;

	memset(buf, 0, sizeof(buf));
	ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	for (size_t i = 0; i < sizeof(buf); i++)
		ASSERT(buf[i] == val++);

	memset(buf, 0, sizeof(buf));
	ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	for (size_t i = 0; i < sizeof(buf); i++)
		ASSERT(buf[i] == val++);

	memset(buf, 0, sizeof(buf));
	ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	for (size_t i = 0; i < sizeof(buf); i++)
		ASSERT(buf[i] == val++);

	ASSERT(fd1.fd.seek_pos == (kfile_off_t)sizeof(buf) * 3);

	ASSERT(kfile_close(&fd1.fd) == 0);
	ASSERT(battfs_close(disk));

	kprintf("Test14: passed\n");
}

static void test15(BattFsSuper *disk)
{
	kprintf("Test15: file creation on new disk\n");

	FILE *fpt = fopen(test_filename, "w+");

	for (int i = 0; i < FILE_SIZE; i++)
		fputc(0xff, fpt);
	fclose(fpt);

	BattFs fd1;
	unsigned int INODE = 0;
	unsigned int MODE = BATTFS_CREATE;

	ASSERT(battfs_init(disk));
	ASSERT(battfs_fileopen(disk, &fd1, INODE, MODE));
	for (int i = 0; i < FILE_SIZE / 2; i++)
		ASSERT(kfile_putc(i, &fd1.fd) != EOF);

	ASSERT(kfile_close(&fd1.fd) == 0);
	ASSERT(battfs_close(disk));

	kprintf("Test15: passed\n");
}

int battfs_testRun(void)
{
	BattFsSuper disk;

	disk.open = disk_open;
	disk.read = disk_page_read;
	disk.load = disk_page_load;
	disk.bufferWrite = disk_buffer_write;
	disk.bufferRead = disk_buffer_read;
	disk.save = disk_page_save;
	disk.erase = disk_page_erase;
	disk.close = disk_close;
	test1(&disk);
	test2(&disk);
	test3(&disk);
	//test4(&disk);
	//test5(&disk);
	test6(&disk);
	test7(&disk);
	test8(&disk);
	test9(&disk);
	test10(&disk);
	test11(&disk);
	test12(&disk);
	test13(&disk);
	test14(&disk);
	test15(&disk);
	kprintf("All tests passed!\n");

	return 0;
}

int battfs_testSetup(void)
{
	return 0;
}

int battfs_testTearDown(void)
{
	return 0;
}

TEST_MAIN(battfs)

#include <fs/battfs.c>
#include <kern/kfile.c>
#include <drv/kdebug.c>
#include <mware/formatwr.c>
#include <mware/hex.c>

#endif // _TEST
