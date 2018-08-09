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
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief MMA8451 Freescale accelerometer sensor.
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "mma8451"
 * $WIZ$ module_depends = "timer", "i2c"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_mma8451.h"
 */

#ifndef DRV_MMA8451_H
#define DRV_MMA8451_H

#define MMA8451_REG_STATUS        0x00
#define MMA8451_REG_OUT_X_MSB     0x01
#define MMA8451_REG_SYSMOD        0x0B
#define MMA8451_REG_INT_SRC       0x0C
#define MMA8451_REG_WHOAMI        0x0D
#define MMA8451_REG_XYZ_DATA_CFG  0x0E


#define MMA8451_REG_PL_STATUS     0x10
#define MMA8451_REG_PL_CFG        0x11
#define MMA8451_REG_FF_MT_CFG     0x15
#define MMA8451_REG_FF_MT_SRC     0x16
#define MMA8451_REG_FF_MT_THS     0x17
#define MMA8451_REG_FF_MT_COUNT   0x18

#define MMA8451_REG_PULSE_CFG     0x21
#define MMA8451_REG_PULSE_SRC     0x22
#define MMA8451_REG_PULSE_THSX    0x23
#define MMA8451_REG_PULSE_THSY    0x24
#define MMA8451_REG_PULSE_THSZ    0x25
#define MMA8451_REG_PULSE_TMLT    0x26
#define MMA8451_REG_PULSE_LTCY    0x27
#define MMA8451_REG_PULSE_WIND    0x28

#define MMA8451_REG_CTRL_REG1     0x2A
#define MMA8451_REG_CTRL_REG2     0x2B
#define MMA8451_REG_CTRL_REG4     0x2D
#define MMA8451_REG_CTRL_REG5     0x2E

// CFG settings
#define MMA8451_ACTIVE              BV(0)
#define MMA8451_LNOISE              BV(2)

#define MMA8451_DATARATE_800_HZ     (0 << 3) //  400Hz
#define MMA8451_DATARATE_400_HZ     (1 << 3) //  200Hz
#define MMA8451_DATARATE_200_HZ     (2 << 3) //  100Hz
#define MMA8451_DATARATE_100_HZ     (3 << 3) //   50Hz
#define MMA8451_DATARATE_50_HZ      (4 << 3) //   25Hz
#define MMA8451_DATARATE_12_5_HZ    (5 << 3) // 6.25Hz
#define MMA8451_DATARATE_6_25HZ     (6 << 3) // 3.13Hz
#define MMA8451_DATARATE_1_56_HZ    (7 << 3) // 1.56Hz


// Status bit
#define MMA8451_ZYXDR             BV(3)
#define MMA8451_ZYXOW             BV(7)
#define MMA8451_ZOW               BV(6)
#define MMA8451_XOW               BV(5)
#define MMA8451_YOW               BV(4)

// Freefall / motion detection
#define MMA8451_ELE               BV(7)
#define MMA8451_OAE               BV(6)
#define MMA8451_ZEFE              BV(5)
#define MMA8451_YEFE              BV(4)
#define MMA8451_XEFE              BV(3)

#define MMA8451_FF_EA             BV(7)
#define MMA8451_ZHE               BV(5)
#define MMA8451_ZHP               BV(4)
#define MMA8451_YHE               BV(3)
#define MMA8451_YHP               BV(2)
#define MMA8451_XHE               BV(1)
#define MMA8451_XHP               BV(0)

// Single/Double tap detection
#define MMA8451_TAP_EA            BV(7)
#define MMA8451_AxZ               BV(6)
#define MMA8451_AxY               BV(5)
#define MMA8451_AxX               BV(4)
#define MMA8451_TAP_DPE           BV(3)
#define MMA8451_PolZ              BV(2)
#define MMA8451_PolY              BV(1)
#define MMA8451_PolX              BV(0)


/* Used with register (MMA8451_REG_XYZ_DATA_CFG) */
#define MMA8451_HPF              BV(4)

#define MMA8451_RANGE_8_G         0x2 // +/- 8g
#define MMA8451_RANGE_4_G         0x1 // +/- 4g
#define MMA8451_RANGE_2_G         0x0 // +/- 2g (default value)

#define MMA8451_PL_PUF            0
#define MMA8451_PL_PUB            1
#define MMA8451_PL_PDF            2
#define MMA8451_PL_PDB            3
#define MMA8451_PL_LRF            4
#define MMA8451_PL_LRB            5
#define MMA8451_PL_LLF            6
#define MMA8451_PL_LLB            7
#define MMA8451_ID               0x1A

#define MMA8451_CFG_END         0xFF

#define MMA8451_ACTIVE_MODE      0
#define MMA8451_STANDBY_MODE     1

#include <drv/i2c.h>
#include <drv/timer.h>

#include <cpu/types.h>

typedef struct AccData
{
	uint32_t count;
	ticks_t timestamp;
	int16_t x;
	int16_t y;
	int16_t z;
	uint8_t data_status;
	uint8_t ff_status;
	uint8_t tap_status;
	uint8_t pl_status;
} AccData;

typedef struct AccCfg
{
	uint8_t reg;
	uint8_t value;
} AccCfg;

typedef struct AccMMA
{
	I2c *i2c;
	uint8_t dev_addr;
	uint8_t status;
} AccMMA;


#define MMA8451_PRINT_ST(axis, reg, bit, pol_bit) \
	do { \
		if ((reg) & (bit)) \
			kprintf("%s"axis",", ((reg) & (pol_bit)) ? "-":"+");\
		else \
			kputs("=,");\
	} while (0)

#define MMA8451_PRINT_DATA_ST(axis, reg, bit) \
	do { \
		if ((reg) & (bit)) \
			kprintf(axis":ow;");\
		else \
			kprintf(axis":ok;");\
	} while (0)

INLINE void mma8451_dump(AccData *data)
{
	kprintf("%ld;", data->count);
	kprintf("%ld;", ticks_to_ms(data->timestamp));
	kprintf("z:%05d;x:%05d;y:%05d;", data->z, data->x, data->y);

	/*
	if (data->data_status & MMA8451_ZYXOW)
	{
		MMA8451_PRINT_DATA_ST("z", data->data_status, MMA8451_ZOW);
		MMA8451_PRINT_DATA_ST("x", data->data_status, MMA8451_XOW);
		MMA8451_PRINT_DATA_ST("y", data->data_status, MMA8451_YOW);
	}
	else
	{
		kprintf("z:ok;y:ok;x:ok;");
	}
	*/

	kprintf("ff[%02x]:%s,", data->tap_status, \
			data->ff_status & MMA8451_FF_EA ? "y" : "n");
	MMA8451_PRINT_ST("z", data->ff_status, MMA8451_ZHE, MMA8451_ZHP);
	MMA8451_PRINT_ST("y", data->ff_status, MMA8451_YHE, MMA8451_YHP);
	MMA8451_PRINT_ST("x", data->ff_status, MMA8451_XHE, MMA8451_XHP);
	kputs(";");

	kprintf("tap[%02x]:%s,%s", data->ff_status, \
			data->tap_status & MMA8451_TAP_EA ? "y" : "n", \
			data->tap_status & MMA8451_TAP_DPE ? "d" : "s");
	MMA8451_PRINT_ST("z", data->ff_status, MMA8451_AxZ, MMA8451_PolZ);
	MMA8451_PRINT_ST("y", data->ff_status, MMA8451_AxY, MMA8451_PolY);
	MMA8451_PRINT_ST("x", data->ff_status, MMA8451_AxX, MMA8451_PolX);
	kputs(";");

}

INLINE bool mma8451_isEvent(AccData *data)
{
	return (data->tap_status & MMA8451_TAP_EA);
}

INLINE void mma8451_updateCount(AccData *data, uint32_t count)
{
	data->count = count;
}

INLINE void mma8451_updateTimestamp(AccData *data, ticks_t time)
{
	data->timestamp = time;
}

int mma8451_readData(AccMMA *mma, AccData *data);
int mma8451_setMode(AccMMA *mma, uint8_t mode);
void mma8451_updateCfg(AccMMA *mma, const AccCfg *cfg);
bool mma8451_init(AccMMA *mma, const AccCfg *cfg, uint8_t dev_addr, I2c *i2c);

#endif /* DRV_MMA8451_H */

