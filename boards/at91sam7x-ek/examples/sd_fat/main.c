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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * \brief Simple application that implement a log device, running on the AT91SAM7X-EK board.
 *
 * The main scope of this application is to show you how use the SD memory card and read and write
 * settings and logs. Thanks the ini parser module we can read settings, stored on memory card,
 * and change the application behaviour.
 * Generally the application work in this way:
 * - Try to access to SD memory card, if it is present we read the inifile
 * - If we have read correctly from SD, we compute the value that we want log
 *   (temperature, pressure and supplay voltage)
 * - If we have enable (from ini file) to log on the file we write the log on it.
 * - If we have enable (from ini file) to log on the serial we write the log on it.
 * - wait the sample_time (from ini file)
 * - contine from beginning.
 *
 * Here we put the ini file that this example uses. To use it copy
 * the folling configutation line into the file called sd_fat.ini (or see INI_FILE_NAME
 * define if you want chande the name).
 * \code
 *
 *   # Bertos SD fat project example
 *   #
 *   # Basic configurarion
 *   #
 *
 *   [log]
 *   # Name of log file
 *   name = test.log
 *   # Enable the logging on serial device (enable = 1, disable = 0)
 *   log_on_serial = 1
 *   # Enable logging on sd file (enable = 1, disable = 0)
 *   log_on_file = 1
 *   # Period between two log in millisecond
 *   sample_time = 1000
 *
 *   [serial_log]
 *   # Select serial port where log
 *   port = 0
 *   # Serial port baudrate
 *   baud = 115200
 *
 *   [log_format]
 *   # Default text to insert on each log line
 *   line_header = BeRTOS Log: 
 *   # Use this char to separate each log field
 *   field_sep = ;
 *
 *   [temperature]
 *   unit_label = C
 *
 *   [pressure]
 *   unit_label = hPa
 *
 *   [voltage]
 *   unit_label = V
 *
 * \endcode
 */

#include <cfg/debug.h>

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL         3
#define LOG_VERBOSITY     0
#include <cfg/log.h>
#include <cfg/macros.h>

#include <cpu/irq.h>

#include <drv/timer.h>
#include <drv/sd.h>
#include <drv/ser.h>
#include <drv/tmp123.h>
#include <drv/spi_dma_at91.h>
#include <drv/mpxx6115a.h>
#include <drv/adc.h>

#include <fs/fat.h>

#include <mware/ini_reader.h>

#include <verstag.h>
#include <buildrev.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Setting structure define
 */
typedef struct INISetting
{
	// INI Log section
	char name[80];              ///< Name of log file
	bool log_on_serial;         ///< Enable the logging on serial device
	bool log_on_file;           ///< Enable logging on sd file
	mtime_t sample_time;        ///< Period between two log (millisecond)

	//INI serial log section
	int port;                   ///< Select serial port where log
	int baud;                   ///< Serial port baudrate

	// INI log format section
	char line_header[80];       ///< Default text to insert on each log line
	char field_sep[10];         ///< Use this char to separate each log field

	// INI temperature section
	char temp_unit_label[10];   ///< Default label for measure unit

	// INI pressure section
	char press_unit_label[10];  ///< Default label for measure unit

	// INI voltage section
	char voltage_unit_label[10];  ///< Default label for measure unit


} INISetting;

/*
 * Setting and various defines.
 */
#define INI_FILE_NAME     "sd_fat.ini"   ///< Default ini file name on SD card.

// INI Log section
#define LOG_SECTION        "log"           ///< Log section
#define LOG_NAME_KEY       "name"          ///< Name of log file
#define LOG_ON_SERIAL      "log_on_serial" ///< Enable the logging on serial device
#define LOG_ON_FILE        "log_on_file"   ///< Enable logging on sd file
#define LOG_SAMPLE_TIME    "sample_time"   ///< Period between two log

//INI serial log section
#define SERIAL_LOG         "serial_log"    ///< Serial log section
#define SERIAL_PORT        "port"          ///< Select serial port where log
#define SERIAL_BAUD        "baud"          ///< Serial port baudrate

// INI log format section
#define LOG_FORMAT_SEC     "log_format"    ///< log format section
#define FORMAT_LINE_HEADER "line_header"   ///< Default text to insert on each log line
#define FORMAT_FIELD_SEP   "field_sep"     ///< Use this char to separate each log field

// INI temperature section
#define TEMPERATURE        "temperature"   ///< temperarute section
#define TEMP_UNIT_LABEL    "unit_label"    ///< Default label for measure unit

// INI pressure section
#define PRESSURE           "pressure"      ///< pressure section
#define PRESS_UNIT_LABEL   "unit_label"    ///< Default label for measure unit

// INI voltsge section
#define VOLTAGE            "voltage"      ///< voltage section
#define VOLTAGE_UNIT_LABEL "unit_label"   ///< Default label for measure unit

/**
 * Voltage reference in volts for ADC.
 * \note This value should be complied to jumper J13 on AT91SAM7X-EK board, and
 * the supply voltade for sensor.
 * See schamatics for more info.
 */
#define ADC_VOLTAGE_REF             3300
/**
 * Channel where the sensor we connect it (AD6)
 * See schamatics for more info.
 */
#define PRESSURE_SENSOR_CH          6

/**
 * ADC channle to read voltage supplay
 * \note we connect board power supplay (+3.3V) to AD3 port with wire.
 */
#define SUPPLAY_VOLTAGE_CH          3
#define ADC_SUPPLAY_VOLTAGE         3300

/*
 * Static definition for application devices.
 */
static SpiDmaAt91 spi_dma;
static Serial temp_sensor_bus;
static Serial log_port;

static void init(void)
{
	IRQ_ENABLE;
	kdbg_init();

	timer_init();
	LOG_INFO("TIMER init..ok\n");

	/*
	 * Init temperature sensor device.
	 * - init the temperature driver
	 * - init the spi communication channel
	 */
	tmp123_init();
	// Init SPI connected to sensor temperature
	spimaster_init(&temp_sensor_bus, SER_SPI1);
	ser_setbaudrate(&temp_sensor_bus, 1000000L);
	LOG_INFO("TMP123 sensor init..ok\n");

	// Init SPI bus to communicate to SD card
	spi_dma_init(&spi_dma);
	spi_dma_setclock(20000000L);
	LOG_INFO("SD SPI init..ok\n");

	adc_init();
	LOG_INFO("ADC init..ok\n");

}

int main(void)
{
	// SD fat filesystem context
	FATFS fs;

	// Context files that we would manage
	FatFile ini_file;
	FatFile log_file;

	init();

	LOG_INFO("SD fat example project %s: %d times\n", VERS_HOST, VERS_BUILD);

	while (1)
	{
		bool sd_ok = true;
		FRESULT result;

		// Setting info
		INISetting ini_set;
		memset(&ini_set, 0, sizeof(ini_set));

		sd_ok = sd_init(&spi_dma.fd);

		if (sd_ok)
		{
			LOG_INFO("Mount FAT filesystem.\n");
			if ((result = f_mount(0, &fs)) != FR_OK)
			{
				LOG_ERR("Mounting FAT volumes error[%d]\n", result);
				sd_ok = false;
			}

			LOG_INFO("Read setting from ini file: %s\n", INI_FILE_NAME);
			if (sd_ok && ((result = fatfile_open(&ini_file, INI_FILE_NAME, FA_READ)) != FR_OK))
			{
				LOG_ERR("Could not open ini file: %s error[%d,]\n", INI_FILE_NAME, result);
				sd_ok = false;
			}

			if (sd_ok)
			{
				/*
				 * If sd is ok, we read all setting from INI file.
				 * NOTE: if one ini key or section was not found into INI
				 * file, the iniparser return the defaul value.
				 */
				ini_getString(&ini_file.fd, LOG_SECTION, LOG_NAME_KEY, "default.log", ini_set.name, sizeof(ini_set.name));
				LOG_INFO("Log file name [%s]\n", ini_set.name);

				char tmp[25];
				ini_getString(&ini_file.fd, LOG_SECTION, LOG_ON_SERIAL,   "1", tmp, sizeof(tmp));
				ini_set.log_on_serial = atoi(tmp);
				LOG_INFO("Log serial [%d]\n", ini_set.log_on_serial);
				ini_getString(&ini_file.fd, LOG_SECTION, LOG_ON_FILE,     "1", tmp, sizeof(tmp));
				ini_set.log_on_file = atoi(tmp);
				LOG_INFO("Log sd [%d]\n", ini_set.log_on_file);
				ini_getString(&ini_file.fd, LOG_SECTION, LOG_SAMPLE_TIME, "500", tmp, sizeof(tmp));
				ini_set.sample_time = atoi(tmp);
				LOG_INFO("Sample time [%ld]\n", ini_set.sample_time);

				ini_getString(&ini_file.fd, SERIAL_LOG, SERIAL_PORT, "0", tmp, sizeof(tmp));
				ini_set.port = atoi(tmp);
				LOG_INFO("Serial port [%d]\n", ini_set.port);
				ini_getString(&ini_file.fd, SERIAL_LOG, SERIAL_BAUD, "115200", tmp, sizeof(tmp));
				ini_set.baud = atoi(tmp);
				LOG_INFO("Serial buad [%d]\n", ini_set.baud);

				ini_getString(&ini_file.fd, LOG_FORMAT_SEC, FORMAT_LINE_HEADER, "BeRTOS: ", ini_set.line_header, sizeof(ini_set.line_header));
				LOG_INFO("Serial line header[%s]\n", ini_set.line_header);

				ini_getString(&ini_file.fd, LOG_FORMAT_SEC, FORMAT_FIELD_SEP, ",", ini_set.field_sep, sizeof(ini_set.field_sep));
				LOG_INFO("Serial char sep[%s]\n", ini_set.field_sep);

				ini_getString(&ini_file.fd, TEMPERATURE, TEMP_UNIT_LABEL, "C", ini_set.temp_unit_label, sizeof(ini_set.temp_unit_label));
				LOG_INFO("Temp unit label[%s]\n", ini_set.temp_unit_label);

				ini_getString(&ini_file.fd, PRESSURE, PRESS_UNIT_LABEL, "hPa", ini_set.press_unit_label, sizeof(ini_set.press_unit_label));
				LOG_INFO("Press unit label[%s]\n", ini_set.press_unit_label);

				ini_getString(&ini_file.fd, VOLTAGE, VOLTAGE_UNIT_LABEL, "V", ini_set.voltage_unit_label, sizeof(ini_set.voltage_unit_label));
				LOG_INFO("Press unit label[%s]\n", ini_set.voltage_unit_label);

			}
		}

		if (ini_set.log_on_serial)
		{
			// Init serial log port
			ser_init(&log_port, ini_set.port);
			ser_setbaudrate(&log_port, ini_set.baud);
			LOG_INFO("SERIAL init..port[%d] buad[%d]\n", ini_set.port, ini_set.baud);
		}

		char log_string[160];
		memset(log_string, 0, sizeof(log_string));

		// Supplay voltage
		uint16_t vdd = ADC_RANGECONV(adc_read(SUPPLAY_VOLTAGE_CH), 0, ADC_SUPPLAY_VOLTAGE);

		// Read temperature
		int16_t tmp = tmp123_read(&temp_sensor_bus.fd);

		// Read pressure
		uint16_t vout = ADC_RANGECONV(adc_read(PRESSURE_SENSOR_CH), 0, vdd);
		int16_t press = mpxx6115a_press(vout,  vdd);

		/*
		 * Format string whit read data
		 * line_header + temp + temp_unit_label + field_sep + press + press_unit_label + field_sep + vdd + voltage_unit_label
		 */
		int wr_len = sprintf(log_string, "%s%d.%01d%s%s%d%s%s%d.%d%s\r\n", ini_set.line_header,
																tmp / 10, ABS(tmp % 10),
																ini_set.temp_unit_label,
																ini_set.field_sep,
																press,
																ini_set.press_unit_label,
																ini_set.field_sep,
																vdd / 1000, ABS(vdd % 1000),
																ini_set.voltage_unit_label);


		/*
		 * if SD is ok, try to open log file and write our data and, only
		 * if by configuration we have enable the log on file
		 */
		if (sd_ok && ini_set.log_on_file)
		{
			// Open log file and do not overwrite the previous log file
			result = fatfile_open(&log_file, ini_set.name,  FA_OPEN_EXISTING | FA_WRITE);

			// If the log file there isn't we create the new one
			if (result == FR_NO_FILE)
			{
				result = fatfile_open(&log_file, ini_set.name,  FA_CREATE_NEW | FA_WRITE);
				LOG_INFO("Create the log file: %s\n", ini_set.name);
			}

			if ( result == FR_OK)
			{
				LOG_INFO("Opened log file '%s' size %ld\n", ini_set.name, log_file.fat_file.fsize);

				// To append data we should go to end of file before to start to write
				kfile_seek(&log_file.fd, 0, KSM_SEEK_END);

				int len = kfile_write(&log_file.fd, log_string, wr_len);

				// Flush data and close the files.
				kfile_flush(&log_file.fd);
				kfile_close(&log_file.fd);

				// Unmount always to prevent accidental sd remove.
				f_mount(0, NULL);
				LOG_INFO("Wrote [%d]\n", len);
			}
			else
			{
				LOG_ERR("Unable to open file: '%s' error[%d]\n", ini_set.name, result);
			}
		}

		// If by configuration we have enable the log on serial, we log it
		if (ini_set.log_on_serial)
			kfile_write(&log_port.fd, log_string, wr_len);

		timer_delay(ini_set.sample_time);
	}

	return 0;
}

