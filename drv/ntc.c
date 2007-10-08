/**
 * \file
 * <!--
 * Copyright 2004, 2005 Develer S.r.l. (http://www.de+veler.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Driver for NTC (reads a temperature through an ADC)
 *
 * \version $Id$
 *
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 *
 *
 * This module handles an external NTC bound to an AD converter. As usual,
 * it relies on a low-level API (ntc_hw_*) (see below):
 *
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.1  2005/11/04 17:59:47  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.1  2005/05/24 09:17:58  batt
 *#* Move drivers to top-level.
 *#*/

#include <drv/ntc.h>
#include <hw_ntc.h>
#include <ntc_map.h>

#include <cfg/debug.h>

DB(bool ntc_initialized;)

/**
 * Find in a table of values \a orig_table of size \a size, the index which
 * value is less or equal to \a val.
 *
 * \retval 0 When \a val is higher than the first table entry.
 * \retval size When \a val is lower than the last table entry.
 * \retval 1..size-1 When \a val is within the table.
 */
static size_t upper_bound(const res_t *orig_table, size_t size, res_t val)
{
	const res_t *table = orig_table;

	while (size)
	{
		size_t pos = size / 2;
		if (val > table[pos])
			size = pos;
		else
		{
			table += pos+1;
			size -= pos+1;
		}
	}

	return table - orig_table;
}


/**
 * Read the temperature for the NTC channel \a dev.
 * First read the resistence of the NTC through ntc_hw_read(), then,
 * for the conversion from resistance to temperature, since the formula
 * varies from device to device, we implemented a generic system using
 * a table of data which maps temperature (index) to resistance (data).
 * The range of the table (min/max temperature) and the step
 * (temperature difference between two consecutive elements of the table)
 * is variable and can be specified. Notice that values inbetween the
 * table elements are still possible as the library does a linear
 * interpolation using the actual calculated resistance to find out
 * the exact temperature.
 *
 * The low-level API provides a function to get access to a description
 * of the NTC (ntc_hw_getInfo()), including the resistance table.
 *
 */
deg_t ntc_read(NtcDev dev)
{
	const NtcHwInfo* hw = ntc_hw_getInfo(dev);
	const res_t* r = hw->resistances;

	float rx;
	size_t i;
	deg_t degrees;

	rx = ntc_hw_read(dev);


	i = upper_bound(r, hw->num_resistances, rx);
	ASSERT(i <= hw->num_resistances);

	if (i >= hw->num_resistances)
		return NTC_SHORT_CIRCUIT;
	else if (i == 0)
		return NTC_OPEN_CIRCUIT;

	/*
	 * Interpolated value in 0.1 degrees multiplied by 10:
	 *   delta t          step t
	 * ----------  = ----------------
	 * (rx - r[i])   (r[i-1] - r [i])
	 */
	float tmp;
	tmp = 10 * hw->degrees_step * (rx - r[i]) / (r[i - 1] - r[i]);

	/*
	 * degrees = integer part corresponding to the superior index
	 *           in the table multiplied by 10
	 *           - decimal part interpolated (already multiplied by 10)
	 */
	degrees = (i * hw->degrees_step + hw->degrees_min) * 10 - (int)(tmp);

	//kprintf("dev= %d, I=%d, degrees = %d\n", dev, i , degrees);

	return degrees;
}


/**
 * Init NTC hardware.
 */
void ntc_init(void)
{
	NTC_HW_INIT;
	DB(ntc_initialized = true;)
}

