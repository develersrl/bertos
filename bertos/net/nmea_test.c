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
"$xyz,1234,asdfadfasdfasdfljsadfkjasdfk\r\n"                                  /* junk */
"$GPRMC,225446,A,4916.45,N,12311.120,W,000.5,054.7,191194,020.3,E*68\r\n"      /* good */
"$GPRMC,225446,A,4916.45,N,12311.120,W,000.5,054.7,191194,020.3,E*48\r\n"      /* checksum error */
"$GPGGA,091144.698,0000.0000,S,00000.0000,W,0,00,00.0,0.0,M,0.0,M,,*5C\r\n"   /* acquired */
"$GPRMC,091144.698,V,0000.0000,S,00000.0000,W,0.00,0.00,051009,,,A*75\r\n"    /* acquired */
"$GPVTG,0.00,T,,,0.00,N,0.00,K,A*70\r\n"                                      /* acquired */
"$GPGGA,091145.698,0000.0000,S,00000.0000,W,0,00,00.0,0.0,M,0.0,M,,*5D\r\n"   /* acquired */
"$GPGSV,1,1,02,1,,,41,12,,,35,,,,,,,,*4A\r\n"                                 /* acquired */
"$GPRMC,091145.698,V,0000.0000,S,00000.0000,W,0.00,0.00,051009,,,A*74\r\n"    /* acquired */
"$GPVTG,0.00,T,,,0.00,N,0.00,K,A*70\r\n"                                      /* acquired */
"$GPGGA,170529.948,4351.0841,N,01108.8685,E,1,05,02.6,57.4,M,45.2,M,,*50\r\n"
"$GPRMC,170525.949,A,4351.0843,N,01108.8687,E,0.00,237.67,051009,,,A*61\r\n"
"$GPVTG,237.67,T,,,0.00,N,0.00,K,A*77\r\n"
"$GPGSV,3,1,09,3,78,302,37,6,87,031,,7,05,292,37,14,05,135,*48\r\n"
"$GPGGA,170527.949,4351.0842,N,01108.8685,E,1,05,02.6,57.4,M,45.2,M,,*5C\r\n"
};


/**
 * do something with the GGA data
 */
static void gpgga_callout(nmeap_context_t *context, void *data, void *user_data)
{
	(void)context;
	(void)user_data;
	NmeaGga *gga = (NmeaGga *)data;

    kprintf("found GPGGA message %ld %ld %d %lu %d %d %d %d\n",
            gga->latitude,
            gga->longitude,
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

	kprintf("found GPRMC Message %lu %c %d %ld %ld %d %d\n",
            rmc->time,
            rmc->warn,
            rmc->latitude,
            rmc->longitude,
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

    kprintf("found GPGSV message %d %d %d %d %d %d %d\n",
			gsv->tot_message,
			gsv->message_num,
			gsv->tot_svv,
			gsv->sv_prn,
			gsv->elevation,
			gsv->azimut,
			gsv->snr
            );
}

/**
 * do something with the VTG data
 */
static void gpvtg_callout(nmeap_context_t *context, void *data, void *user_data)
{
	(void)context;
	(void)user_data;
	NmeaVtg *vtg = (NmeaVtg *)data;

    kprintf("found GPVTG message %d %d %d\n",
			vtg->track_good,
			vtg->knot_speed,
			vtg->km_speed
            );
}

int nmea_testSetup(void)
{
	kdbg_init();

	kfilemem_init(&mem, nmea_test_vector, sizeof(nmea_test_vector));
	kprintf("Init test buffer..done.\n");

    nmeap_init(&nmea, NULL);
	kprintf("Init NMEA context..done.\n");
    nmeap_addParser(&nmea, "GPGGA", nmea_gpgga, gpgga_callout, &gga);
	kprintf("Init NMEA GPGGA parser..done.\n");
    nmeap_addParser(&nmea, "GPRMC", nmea_gprmc, gprmc_callout, &rmc);
	kprintf("Init NMEA GPRMC parser..done.\n");
	nmeap_addParser(&nmea, "GPGSV", nmea_gpgsv, gpgsv_callout, &gsv);
	kprintf("Init NMEA GPGSV parser..done.\n");
	nmeap_addParser(&nmea, "GPVTG", nmea_gpvtg, gpvtg_callout, &vtg);
	kprintf("Init NMEA GPVTG parser..done.\n");

	return 0;
}

int nmea_testTearDown(void)
{
	return 0;
}

int nmea_testRun(void)
{
	nmea_poll(&nmea, &mem.fd);
	return  0;
}

TEST_MAIN(nmea);
