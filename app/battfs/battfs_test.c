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
 * \version $Id: demo.c 18242 2007-10-08 17:35:23Z marco $
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * \brief BattFs Test.
 */

#include <cfg/debug.h>
#include <fs/battfs.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_SIZE 32768
#define PAGE_SIZE 128
#define PAGE_COUNT FILE_SIZE / PAGE_SIZE

FILE *fp;
const char test_filename[]="battfs_disk.bin";


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
	//TRACEMSG("page:%d, addr:%d, size:%d\n", page, addr, size);
	fseek(fp, page * d->page_size + addr, SEEK_SET);
	return fread(buf, 1, size, fp);
}

static size_t disk_page_write(struct BattFsSuper *d, pgcnt_t page, pgaddr_t addr, const void *buf, size_t size)
{
	//TRACEMSG("page:%d, addr:%d, size:%d\n", page, addr, size);
	fseek(fp, page * d->page_size + addr, SEEK_SET);
	return fwrite(buf, 1, size, fp);
}

static bool disk_page_erase(struct BattFsSuper *d, pgcnt_t page)
{
	//TRACEMSG("page:%d\n", page);
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
		ref[i] = PAGE_COUNT - i - 1;

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
		battfs_writeTestBlock(disk, i, 0, 0, 0, i, MARK_PAGE_VALID);
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
		battfs_writeTestBlock(disk, i, 0, 0, 0, i, MARK_PAGE_VALID);
		ref[i] = i;
	}
	fseek(fp, FILE_SIZE / 2, SEEK_SET);
	for (int i = FILE_SIZE / 2; i < FILE_SIZE; i++)
		fputc(0xff, fp);
	fclose(fp);

	for (int i = PAGE_COUNT / 2; i < PAGE_COUNT; i++)
	{
		ref[i] = PAGE_COUNT + PAGE_COUNT / 2 - i - 1;
	}


	testCheck(disk, ref);
	kprintf("Test3: passed\n");
}

static void test4(BattFsSuper *disk)
{
	pgcnt_t ref[PAGE_COUNT];
	kprintf("Test4: disk half full with 1 contiguos file, rest marked free\n");


	fp = fopen(test_filename, "w+");

	for (int i = 0; i < PAGE_COUNT / 2; i++)
	{
		battfs_writeTestBlock(disk, i, 0, 0, 0, i, MARK_PAGE_VALID);
		ref[i] = i;
	}
	for (int i = PAGE_COUNT / 2; i < PAGE_COUNT; i++)
	{
		battfs_writeTestBlock(disk, i, 0, 0, 0, i, i);
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
		battfs_writeTestBlock(disk, i, 0, 0, 0, i, MARK_PAGE_VALID);
		ref[i] = i;
	}
	for (int i = PAGE_COUNT / 3; i < 2 * (PAGE_COUNT / 3); i++)
	{
		battfs_writeTestBlock(disk, i, 0, 0, 0, i, i);
		ref[i + PAGE_COUNT / 3 + 1] = i;
	}
	fclose(fp);

	for (int i = PAGE_COUNT / 3; i < 2 * (PAGE_COUNT / 3) + 1; i++)
		ref[i] = PAGE_COUNT + PAGE_COUNT / 3 - i - 1;

	testCheck(disk, ref);
	kprintf("Test5: passed\n");
}

static void test6(BattFsSuper *disk)
{
	pgcnt_t ref[4];
	kprintf("Test6: 1 file with 1 old seq num, 1 free block\n");


	fp = fopen(test_filename, "w+");

	battfs_writeTestBlock(disk, 0, 0, 0, 0, 0, MARK_PAGE_VALID);
	battfs_writeTestBlock(disk, 1, 0, 0, 0, 1, MARK_PAGE_VALID);
	battfs_writeTestBlock(disk, 2, 0, 1, 0, 1, MARK_PAGE_VALID);
	battfs_writeTestBlock(disk, 3, 0, 0, 0, 0, 123);

	fclose(fp);
	ref[0] = 0;
	ref[1] = 2;
	ref[2] = 3;
	ref[3] = 1;

	testCheck(disk, ref);
	kprintf("Test6: passed\n");
}

static void test7(BattFsSuper *disk)
{
	pgcnt_t ref[4];
	kprintf("Test7: 1 file with 1 old seq num, 1 free block\n");


	fp = fopen(test_filename, "w+");

	battfs_writeTestBlock(disk, 0, 0, 0, 0, 0, MARK_PAGE_VALID);
	battfs_writeTestBlock(disk, 1, 0, 1, 0, 1, MARK_PAGE_VALID);
	battfs_writeTestBlock(disk, 2, 0, 0, 0, 1, MARK_PAGE_VALID);
	battfs_writeTestBlock(disk, 3, 0, 0, 0, 0, 123);

	fclose(fp);
	ref[0] = 0;
	ref[1] = 1;
	ref[2] = 3;
	ref[3] = 2;

	testCheck(disk, ref);
	kprintf("Test7: passed\n");
}

static void test8(BattFsSuper *disk)
{
	pgcnt_t ref[4];
	kprintf("Test8: 1 file with 1 old seq num, 1 free block\n");


	fp = fopen(test_filename, "w+");

	battfs_writeTestBlock(disk, 0, 0, 0, 0, 0, 1235);
	battfs_writeTestBlock(disk, 1, 0, 0, 0, 0, MARK_PAGE_VALID);
	battfs_writeTestBlock(disk, 2, 0, 1, 0, 1, MARK_PAGE_VALID);
	battfs_writeTestBlock(disk, 3, 0, 0, 0, 1, MARK_PAGE_VALID);


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

	battfs_writeTestBlock(disk, 0, 0, 0, 0, 0, 1235);
	battfs_writeTestBlock(disk, 1, 0, 0, 0, 0, MARK_PAGE_VALID);
	battfs_writeTestBlock(disk, 2, 0, 3, 0, 1, MARK_PAGE_VALID);
	battfs_writeTestBlock(disk, 3, 0, 0, 0, 1, MARK_PAGE_VALID);
	battfs_writeTestBlock(disk, 4, 0, 0, 0, 0, 1236);
	battfs_writeTestBlock(disk, 5, 4, 0, 0, 0, MARK_PAGE_VALID);
	battfs_writeTestBlock(disk, 6, 4, 1, 0, 1, MARK_PAGE_VALID);
	battfs_writeTestBlock(disk, 7, 4, 0, 0, 1, MARK_PAGE_VALID);


	fclose(fp);
	ref[0] = 1;
	ref[1] = 3;
	ref[2] = 5;
	ref[3] = 6;
	ref[4] = 0;
	ref[5] = 4;
	ref[6] = 2;
	ref[7] = 7;

	testCheck(disk, ref);
	kprintf("Test9: passed\n");
}

static void test10(BattFsSuper *disk)
{
	KFileBattFs fd1;
	KFileBattFs fd2;
	kprintf("Test10: open file test, inode 0 and inode 4\n");

	fp = fopen(test_filename, "w+");

	unsigned int PAGE_FILL = 116;
	unsigned int INODE = 0;
	unsigned int INODE2 = 4;
	unsigned int INEXISTENT_INODE = 123;
	unsigned int MODE = 0;

	battfs_writeTestBlock(disk, 0, 123, 0, PAGE_FILL, 0, 1235);
	battfs_writeTestBlock(disk, 1, INODE, 0, PAGE_FILL, 0, MARK_PAGE_VALID);
	battfs_writeTestBlock(disk, 2, INODE, 3, PAGE_FILL, 1, MARK_PAGE_VALID);
	battfs_writeTestBlock(disk, 3, INODE, 0, PAGE_FILL, 1, MARK_PAGE_VALID);
	battfs_writeTestBlock(disk, 4, INODE2, 0, PAGE_FILL, 0, 1236);
	battfs_writeTestBlock(disk, 5, INODE2, 0, PAGE_FILL, 0, MARK_PAGE_VALID);
	battfs_writeTestBlock(disk, 6, INODE2, 1, PAGE_FILL, 1, MARK_PAGE_VALID);
	battfs_writeTestBlock(disk, 7, INODE2, 0, PAGE_FILL, 1, MARK_PAGE_VALID);

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





int main(void)
{
	BattFsSuper disk;

	disk.open = disk_open;
	disk.read = disk_page_read;
	disk.write = disk_page_write;
	disk.erase = disk_page_erase;
	disk.close = disk_close;
	test1(&disk);
	test2(&disk);
	test3(&disk);
	test4(&disk);
	test5(&disk);
	test6(&disk);
	test7(&disk);
	test8(&disk);
	test9(&disk);
	test10(&disk);
	kprintf("All test passed!\n");

	return 0;
}
