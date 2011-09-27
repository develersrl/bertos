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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief CHIP ID SAM3 definitions.
 */

#include "sam3_chipid.h"

#include <cpu/types.h>

static const char _unknown[] = "unknown";
static const char * const chip_id_eproc_names[] =
{
	_unknown,					// 0
	"arm946es",					// 1
	"arm7tdmi",					// 2
	"cortex-m3",				// 3
	"arm920t",					// 4
	"arm926ejs",				// 5
	"cortex-a5",				// 6
};

const char *chipid_eproc_name(int idx)
{
	if (idx <= 6)
		return chip_id_eproc_names[idx];

	return _unknown;
}

#define chip_id_nvpsize2 chip_id_nvpsize		// these two tables are identical
static const char * const chip_id_nvpsize[] =
{
	"none",						//  0
	"8K bytes",					//  1
	"16K bytes",				//  2
	"32K bytes",				//  3
	_unknown,					//  4
	"64K bytes",				//  5
	_unknown,					//  6
	"128K bytes",				//  7
	_unknown,					//  8
	"256K bytes",				//  9
	"512K bytes",				// 10
	_unknown,					// 11
	"1024K bytes",				// 12
	_unknown,					// 13
	"2048K bytes",				// 14
	_unknown,					// 15
};

const char *chipid_nvpsize(int idx)
{
	if (idx <= 15)
		return chip_id_nvpsize[idx];

	return _unknown;
}


static const char * const chip_id_sramsize[] =
{
	"48K Bytes",				//  0
	"1K Bytes",					//  1
	"2K Bytes",					//  2
	"6K Bytes",					//  3
	"112K Bytes",				//  4
	"4K Bytes",					//  5
	"80K Bytes",				//  6
	"160K Bytes",				//  7
	"8K Bytes",					//  8
	"16K Bytes",				//  9
	"32K Bytes",				// 10
	"64K Bytes",				// 11
	"128K Bytes",				// 12
	"256K Bytes",				// 13
	"96K Bytes",				// 14
	"512K Bytes",				// 15

};

const char *chipid_sramsize(int idx)
{
	if (idx <= 15)
		return chip_id_sramsize[idx];

	return _unknown;
}


static const struct archnames { unsigned value; const char *name; } chip_id_archnames[] =
{
	{ 0x19,  "AT91SAM9xx Series"						},
	{ 0x29,  "AT91SAM9XExx Series"						},
	{ 0x34,  "AT91x34 Series"							},
	{ 0x37,  "CAP7 Series"								},
	{ 0x39,  "CAP9 Series"								},
	{ 0x3B,  "CAP11 Series"								},
	{ 0x40,  "AT91x40 Series"							},
	{ 0x42,  "AT91x42 Series"							},
	{ 0x55,  "AT91x55 Series"							},
	{ 0x60,  "AT91SAM7Axx Series"						},
	{ 0x61,  "AT91SAM7AQxx Series"						},
	{ 0x63,  "AT91x63 Series"							},
	{ 0x70,  "AT91SAM7Sxx Series"						},
	{ 0x71,  "AT91SAM7XCxx Series"						},
	{ 0x72,  "AT91SAM7SExx Series"						},
	{ 0x73,  "AT91SAM7Lxx Series"						},
	{ 0x75,  "AT91SAM7Xxx Series"						},
	{ 0x76,  "AT91SAM7SLxx Series"						},
	{ 0x80,  "ATSAM3UxC Series (100-pin version)"		},
	{ 0x81,  "ATSAM3UxE Series (144-pin version)"		},
	{ 0x83,  "ATSAM3AxC Series (100-pin version)"		},
	{ 0x84,  "ATSAM3XxC Series (100-pin version)"		},
	{ 0x85,  "ATSAM3XxE Series (144-pin version)"		},
	{ 0x86,  "ATSAM3XxG Series (208/217-pin version)"	},
	{ 0x88,  "ATSAM3SxA Series (48-pin version)"		},
	{ 0x89,  "ATSAM3SxB Series (64-pin version)"		},
	{ 0x8A,  "ATSAM3SxC Series (100-pin version)"		},
	{ 0x92,  "AT91x92 Series"							},
	{ 0x95,  "ATSAM3NxC Series (100-pin version)"		},
	{ 0xF0,  "AT75Cxx Series"							},
	{ -1, NULL },

};

const char *chipid_archnames(unsigned value)
{
	for (int i = 0; chip_id_archnames[i].name; i++)
	{
		if (chip_id_archnames[i].value == value)
			return chip_id_archnames[i].name;
	}

	return _unknown;
}

static const char * const chip_id_nvptype[] = {
	"rom", // 0
	"romless or onchip flash", // 1
	"embedded flash memory", // 2
	"rom(nvpsiz) + embedded flash (nvpsiz2)", //3
	"sram emulating flash", // 4
};

const char *chipid_nvptype(int idx)
{
	if (idx <= 4)
		return chip_id_nvptype[idx];

	return _unknown;
}

