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
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief NMEA parser test.
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * notest:avr
 */

#include "nmea.h"

#include <struct/kfile_mem.h>

#include <cfg/debug.h>
#define LOG_LEVEL  NMEA_LOG_LEVEL
#define LOG_FORMAT NMEA_LOG_FORMAT
#include <cfg/log.h>

#include <cfg/test.h>

#include <string.h> //strncmp

static nmeap_context_t nmea;	   /* parser context */
static NmeaRmc rmc;
static NmeaGga gga;
static NmeaGsv gsv;
static NmeaVtg vtg;

static KFileMem mem;

static char nmea_test_vector[] =
{
"$GPGGA,123519.021,3929.946667,N,11946.086667,E,1,08,0.9,545.4,M,46.9,M,,*4A\r\n" /* good */
"$xyz,1234,asdfadfasdfasdfljsadfkjasdfk\r\n"                                      /* junk */
"$GPRMC,225446,A,4916.45,N,12311.120,W,000.5,054.7,191194,020.3,E*68\r\n"         /* good */
"$GPRMC,225446,A,4916.45,N,12311.120,W,000.5,054.7,191194,020.3,E*48\r\n"         /* checksum error */
"$GPGGA,091144.698,0000.0000,S,00000.0000,W,0,00,00.0,0.0,M,0.0,M,,*5C\r\n"       /* acquired */
"$GPRMC,091144.698,V,0000.0000,S,00000.0000,W,0.00,0.00,051009,,,A*75\r\n"        /* acquired */
"$GPVTG,0.00,T,,,0.00,N,0.00,K,A*70\r\n"                                          /* acquired */
"$GPGGA,091145.698,0000.0000,S,00000.0000,W,0,00,00.0,0.0,M,0.0,M,,*5D\r\n"       /* acquired */
"$GPGSV,1,1,02,1,,,41,12,,,35,,,,,,,,*4A\r\n"                                     /* acquired */
"$GPRMC,091145.698,V,0000.0000,S,00000.0000,W,0.00,0.00,051009,,,A*74\r\n"        /* acquired */
"$GPVTG,0.00,T,,,0.00,N,0.00,K,A*70\r\n"                                          /* acquired */
"$GPGGA,170529.948,4351.0841,N,01108.8685,E,1,05,02.6,57.4,M,45.2,M,,*50\r\n"     /* acquired */
"$GPRMC,170525.949,A,4351.0843,N,01108.8687,E,0.00,237.67,051009,,,A*61\r\n"      /* acquired */
"$GPVTG,237.67,T,,,0.00,N,0.00,K,A*77\r\n"                                        /* acquired */
"$GPGSV,3,1,09,3,78,302,37,6,87,031,,7,05,292,37,14,05,135,*48\r\n"               /* acquired */
"$GPGGA,170527.949,4351.0842,N,01108.8685,E,1,05,02.6,57.4,M,45.2,M,,*5C\r\n"     /* acquired */
};

NmeaGga gga_test =
{
	.latitude = 43851403,
    .longitude = 11147808,
    .altitude = 57,
    .time = 2736328,
    .satellites = 5,
    .quality = 1,
    .hdop = 26,
    .geoid = 45,
};

#define TOT_GOOD_SENTENCE_NUM    12

#define MAX_SENTENCE_POLL  20

static int tot_sentence_parsed = 0;

/**
 * do something with the GGA data
 */
static void gpgga_callout(nmeap_context_t *context, void *data, void *user_data)
{
	(void)context;
	(void)user_data;
	NmeaGga *gga = (NmeaGga *)data;

	tot_sentence_parsed++;

    LOG_INFO("[%d]found GPGGA message %ld %ld %d %lu %d %d %d %d\n",tot_sentence_parsed,
            (long)gga->latitude,
            (long)gga->longitude,
            gga->altitude,
            gga->time,
            gga->satellites,
            gga->quality,
            gga->hdop,
            gga->geoid
            );
}

/**
 * called when a gpgga message is received and parsed
 */
static void gprmc_callout(nmeap_context_t *context, void *data, void *user_data)
{
	(void)context;
	(void)user_data;
    NmeaRmc *rmc = (NmeaRmc *)data;

	tot_sentence_parsed++;

	LOG_INFO("[%d]found GPRMC Message %lu %c %ld %ld %d %d %d\n",tot_sentence_parsed,
            rmc->time,
            rmc->warn,
            (long)rmc->latitude,
            (long)rmc->longitude,
            rmc->speed,
            rmc->course,
            rmc->mag_var
            );
}

/**
 * do something with the GGA data
 */
static void gpgsv_callout(nmeap_context_t *context, void *data, void *user_data)
{
	(void)context;
	(void)user_data;
	NmeaGsv *gsv = (NmeaGsv *)data;

	tot_sentence_parsed++;

    LOG_INFO("[%d]found GPGSV message %d %d %d\n",tot_sentence_parsed,
			gsv->tot_message,
			gsv->message_num,
			gsv->tot_svv);

	for (int i = 0; i < 4; i++)
	    LOG_INFO("[%d]%d %d %d %d\n", i, gsv->info[i].sv_prn, gsv->info[i].elevation, gsv->info[i].azimut, gsv->info[i].snr);
}

/**
 * do something with the VTG data
 */
static void gpvtg_callout(nmeap_context_t *context, void *data, void *user_data)
{
	(void)context;
	(void)user_data;
	NmeaVtg *vtg = (NmeaVtg *)data;

	tot_sentence_parsed++;

    LOG_INFO("[%d]found GPVTG message %d %d %d\n",tot_sentence_parsed,
			vtg->track_good,
			vtg->knot_speed,
			vtg->km_speed
            );
}

int nmea_testSetup(void)
{
	kdbg_init();

	kfilemem_init(&mem, nmea_test_vector, sizeof(nmea_test_vector));
	LOG_INFO("Init test buffer..done.\n");

    nmeap_init(&nmea, NULL);
    nmeap_addParser(&nmea, "GPGGA", nmea_gpgga, gpgga_callout, &gga);
    nmeap_addParser(&nmea, "GPRMC", nmea_gprmc, gprmc_callout, &rmc);
	nmeap_addParser(&nmea, "GPGSV", nmea_gpgsv, gpgsv_callout, &gsv);
	nmeap_addParser(&nmea, "GPVTG", nmea_gpvtg, gpvtg_callout, &vtg);

	return 0;
}

int nmea_testTearDown(void)
{
	return 0;
}

int nmea_testRun(void)
{
	for (int i = 0; i < MAX_SENTENCE_POLL; i++)
	{
		nmea_poll(&nmea, &mem.fd);
	}

	if (tot_sentence_parsed != TOT_GOOD_SENTENCE_NUM)
	{
		LOG_ERR("No all test sentence have been parser.\n");
		return -1;
	}

	if (memcmp(&gga_test, &gga, sizeof(gga_test)))
	{
		LOG_ERR("Last gga test sentence had unexpected value\n");
		return -1;
	}
	return  0;
}

TEST_MAIN(nmea);
