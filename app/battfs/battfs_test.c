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

FILE *fp;
const char test_filename[]="battfs_disk.bin";
const char *filename;

#define PAGE_SIZE 128
static bool disk_open(struct BattFsSuper *d)
{
	fp = fopen(filename, "r+b");
	ASSERT(fp);
	fseek(fp, 0, SEEK_END);
	d->page_size = PAGE_SIZE;
	d->page_count = ftell(fp) / d->page_size;
	d->disk_size = d->page_size * d->page_count;
	d->page_array = malloc(d->page_count * sizeof(pgcnt_t));
	TRACEMSG("page_size:%d, page_count:%d, disk_size:%d\n", d->page_size, d->page_count, d->disk_size);
	return (fp && d->page_array);
}

static size_t disk_page_read(struct BattFsSuper *d, pgcnt_t page, pgaddr_t addr, void *buf, size_t size)
{
	TRACEMSG("page:%d, addr:%d, size:%d\n", page, addr, size);
	fseek(fp, page * d->page_size + addr, SEEK_SET);
	return fread(buf, 1, size, fp);
}

static size_t disk_page_write(struct BattFsSuper *d, pgcnt_t page, pgaddr_t addr, void *buf, size_t size)
{
	TRACEMSG("page:%d, addr:%d, size:%d\n", page, addr, size);
	fseek(fp, page * d->page_size + addr, SEEK_SET);
	return fwrite(buf, 1, size, fp);
}

static bool disk_page_erase(struct BattFsSuper *d, pgcnt_t page)
{
	TRACEMSG("page:%d\n", page);
	fseek(fp, page * d->page_size, SEEK_SET);

	for (int i = 0; i < d->page_size; i++)
		if (fputc(0xff, fp) == EOF)
			return false;
	return true;
}

static bool disk_close(struct BattFsSuper *d)
{
	TRACE;
	free(d->page_array);
	return (fclose(fp) != EOF);
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		FILE *fpt = fopen(test_filename, "w+");
		for (int i = 0; i < 32768; i++)
			fputc(0xff, fpt);
		fclose(fpt);
		filename = test_filename;
	}
	else
		filename = argv[1];

	BattFsSuper disk;
	disk.open = disk_open;
	disk.read = disk_page_read;
	disk.write = disk_page_write;
	disk.erase = disk_page_erase;
	disk.close = disk_close;

	if (battfs_init(&disk))
	{
		kprintf("page_array:\n");
		for (pgcnt_t i = 0; i < disk.page_count; i++)
		{
			if (!(i % 16))
				kputchar('\n');
			kprintf("%04d ", disk.page_array[i]);
		}
		kputchar('\n');
		return 0;
	}
	else
		return 1;
}
