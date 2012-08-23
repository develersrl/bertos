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
 * Copyright 2012 Jack Bradach <jack@bradach.net>
 *
 * -->
 *
 * \brief Maxim MAX31855 Cold-Junction Compensated Thermocouple-to-Digital Converter
 *
 * \author Jack Bradach <jack@bradach.net>
 *
 */
#include <cfg/module.h>
#include <cpu/byteorder.h>
#include <io/kfile.h>
#include "max31855.h"
#include "hw/hw_max31855.h"
#include "cfg/debug.h"

/** Multipliers for the internal and external temperature readings */
#define MAX31855_INT_SCALE 0.0625
#define MAX31855_EXT_SCALE 0.25

/**
 * Read the external and, optionally, internal temperatures from
 * the MAX31855 chip in Celsius.  The external temperature is from the
 * thermocouple.  The internal temperature is the temperature at the
 * cold junction.  If tmp_int is NULL, only the external temperature
 * will be read (which is slightly faster than grabbing both).
 *
 * \param fd a valid KFile handle to a SPI controller.
 * \param tmp_ext pointer where external temperature will be stored.
 * \param tmp_int pointer where internal temperature will be stored, else NULL.
 * \returns status of read operation.
 */
int max31855_rd_cel(KFile *fd, float *tmp_ext, float *tmp_int)
{
    uint32_t sensor_data;
    uint16_t raw_ext = 0;
    uint16_t raw_int = 0;
    bool fault = false;
    int sign;
    int read_size;

    /* tmp_int is optional but tmp_ext must be provided. */
    ASSERT_VALID_PTR(tmp_ext);
    ASSERT_VALID_PTR_OR_NULL(tmp_int);

    /* If we're just reading the external temperature, only
     * 16 bits get read from the MAX31855.  Otherwise, we
     * can pull the full 32.
     */
    read_size = (tmp_int) ? MAX31855_SIZE_BOTH : MAX31855_SIZE_EXT;

    /* Assert the chip select, read the data, and deselect it to
     * complete the read operation.  Conversions are free running
     * inside the MAX31855, so you'll always get the most up-to-date
     * data when you read.
     */
    MAX31855_HW_CS_EN();
    kfile_read(fd, &sensor_data, read_size);
    MAX31855_HW_CS_DIS();

    if (MAX31855_SIZE_BOTH == read_size)
    {
        sensor_data = be32_to_cpu(sensor_data);
        raw_ext = (uint16_t) (sensor_data >> MAX31855_EXT_BIT_OFFSET);
        raw_int = (uint16_t) (sensor_data & MAX31855_INT_MASK);
    }
    else
    {
        sensor_data = be16_to_cpu((uint16_t)sensor_data);
        raw_ext = (uint16_t) sensor_data;
    }

    /* Error checking.  If any of the three errors that the MAX31855 can
     * report (open circuit, shorts to ground or Vcc) have occured, bit
     * 16 (or bit 0 if you're only reading the external temperature) will
     * be set.  If both internal and external temperatures are being
     * read, the error being signaled is the lower three bits of
     * the internal temperature.
     */
    if (MAX31855_SIZE_BOTH == read_size)
    {
       if (sensor_data & (1UL << MAX31855_FAULT_BIT)) { fault = true; }
    }
    else
    {
       if (sensor_data & BV(MAX31855_FAULT_BIT_EXT)) { fault = true; }
    }

    /* Wugh oh, a fault!  Either mask off the fault bits and return
     * them, if we're reading both internal and external temperatures,
     * or simply set bit zero to indicate a failure, if in external-only mode.
     * Note that the faults the MAX31855 reports are typically non-transient
     * and usually indicate a problem with the hardware.
     */
    if (fault)
    {
        if (MAX31855_SIZE_BOTH == read_size)
            return sensor_data & MAX31855_FAULT_MASK;
	else
            return BV(MAX31855_FAULT_BIT_EXT);
    }

    /* Apply the sign (high bit of the value) to the temperature and
     * copy the values over to the pointers passed in. */
    sign = (raw_ext & BV(MAX31855_SIGN_BIT))?-1:1;
    *tmp_ext = (float) sign * (((raw_ext & ~BV(MAX31855_SIGN_BIT)) >> MAX31855_EXT_DATA_SHIFT) * MAX31855_EXT_SCALE);
    if (NULL != tmp_int)
    {
        sign = (raw_int & BV(MAX31855_SIGN_BIT))?-1:1;
        *tmp_int = (float) sign * (((raw_int & ~BV(MAX31855_SIGN_BIT)) >> MAX31855_INT_DATA_SHIFT) * MAX31855_INT_SCALE);
    }

    return MAX31855_OK;
}

/**
 * Read the external and, optionally, internal temperatures from the MAX31855
 * and return them in Fahrenheit.  This is essentially a wrapper to the Celsius
 * reading call does the "9/5ths plus 32" conversion that we all learned in
 * grade school, at least you did if you grew up in a country that decided
 * that the metric system was the tool of the devil.  *sigh* Ah well.  My
 * car gets forty rods to the hogshead, and that's the way I likes it!
 */
int max31855_rd_fah(KFile *fd, float *tmp_ext, float *tmp_int)
{
    int rc;

    rc = max31855_rd_cel(fd, tmp_ext, tmp_int);
    if (rc != MAX31855_OK)
        return rc;

    *tmp_ext = *tmp_ext * (9.0/5.0) + 32.0;
    if(NULL != tmp_int) { *tmp_int = *tmp_int * (9.0/5.0) + 32.0; }

    return MAX31855_OK;
}

/**
 * Read the external and, optionally, internal temperatures from the MAX31855
 * and return them in Kelvin.  Also a wrapper to the Celsius routine.  It
 * just adds 273.15 to the results.  But you knew that.
 */
int max31855_rd_kel(KFile *fd, float *tmp_ext, float *tmp_int)
{
    int rc;

    rc = max31855_rd_cel(fd, tmp_ext, tmp_int);
    if (rc != MAX31855_OK)
        return rc;

    *tmp_ext = *tmp_ext + 273.15;
    if(NULL != tmp_int)
        *tmp_int = *tmp_int + 273.15;

    return MAX31855_OK;
}

/**
 * Init the MAX31855 interface.  This function wraps the hardware-
 * specific macro which you must define for your specific architecture.
 */
void max31855_init(void)
{
	MAX31855_HW_INIT();
}


