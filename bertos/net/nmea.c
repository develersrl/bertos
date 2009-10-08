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
 * \brief NMEA implementation.
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * notest:avr
 */

#include "nmea.h"

#include "cfg/cfg_nmea.h"

#include <cfg/debug.h>

#define LOG_LEVEL  NMEA_LOG_LEVEL
#define LOG_FORMAT NMEA_LOG_FORMAT
#include <cfg/log.h>

#include <net/nmeap/inc/nmeap.h>

#include <ctype.h>
#include <time.h>
#include <string.h>


static uint32_t tokenToInt(const char *s, int precision)
{
	uint32_t num = 0;
	bool sep_found = false;
	int i;

	if (!s)
		return 0;

	for(i = 0; i < NMEAP_MAX_SENTENCE_LENGTH; i++)
	{
		char c = *s++;

		if (c == '.')
		{
			sep_found = true;
			continue;
		}

		if (c == '\0' || !isdigit(c) || (precision == 0 && sep_found))
			break;

		if (sep_found)
			precision--;

		num *= 10;
		num += c - '0';
	}

	while (precision--)
		num *= 10;

	return num;
}

static udegree_t convertToDegree(const char *str)
{
	uint32_t dec;
	uint32_t deg;
	uint32_t min;

	if (*str == 0)
	{
        return 0;
    }

	dec = tokenToInt(str, 4);
	deg = dec / 1000000;
	min = dec - deg * 1000000;
	dec = deg * 1000000 + ((min * 5) + 1) / 3;

	return dec;
}

static udegree_t nmea_latitude(const char *plat, const char *phem)
{
	int ns;

    if (*phem == 0)
	{
        return 0;
    }


    /* north lat is +, south lat is - */
    if (*phem == 'N')
	{
        ns = 1;
    }
    else
	{
        ns = -1;
    }


	return ns * convertToDegree(plat);
}

static udegree_t nmea_longitude(const char *plot, const char *phem)
{
	int ew;

    /* west long is negative, east long is positive */
    if (*phem == 'E')
	{
        ew = 1;
    }
    else {
        ew = -1;
    }

    if (*phem == 0)
	{
        return 0;
    }

	return ew * convertToDegree(plot);
}

static uint16_t nmea_altitude(const char *palt, const char *punits)
{
	uint32_t alt;

	if (*palt == 0)
	{
        return 0;
    }

	alt = atoi(palt);

    if (*punits == 'F')
	{
        /* convert to feet */
        /* alt = alt * 3.2808399 */
		alt = alt * 3 +  /* 3.0 */
			  (alt >> 2) + /* 0.25 */
			  (alt >> 6) + /* 0.015625 */
			  (alt >> 7) + /* 0.0078125 */
			  (alt >> 8); /* 0,00390625 */

    }

	return alt;
}

static time_t timestampToSec(uint32_t time_stamp, uint32_t date_stamp)
{
	struct tm t;
	uint16_t msec;
	uint16_t tmr[3];
	uint16_t date[3];

	memset(&t, 0, sizeof(t));
	memset(&tmr, 0, sizeof(tmr));
	memset(&date, 0, sizeof(date));

	LOG_INFO("time_s[%lu],date[%lu]\n", (long)time_stamp, (long)date_stamp);
	uint32_t res = time_stamp / 1000;
	uint32_t all = time_stamp;
	msec = all - res * 1000;
	for (int i = 0; i < 3; i++)
	{
		all = res;
		res = all / 100;
		tmr[i]  = all - res * 100;
		LOG_INFO("t[%d]%d\n", tmr[i],i);
	}

	t.tm_sec = tmr[0] + (ROUND_UP(msec, 1000) / 1000);
	t.tm_min = tmr[1];
	t.tm_hour = tmr[2];
	//If we not have refence data, we set as default 1/1/1970.
	t.tm_mday = 1;
	t.tm_mon = 1;
	t.tm_year = 70;

	if (date_stamp)
	{
		res = all = date_stamp;
		for (int i = 0; i < 3; i++)
		{
			all = res;
			res = all / 100;
			date[i]  = all - res * 100;
			LOG_INFO("d[%d]%d\n", date[i],i);
		}
		t.tm_mday = date[2];
		t.tm_mon = date[1] - 1; // time struct count month from 0 to 11;
		// we should specific number of years from 1900, but the year field
		// is only two cipher, so we sum 100 (2000 - 1900)..
		t.tm_year = date[0] + 100;
	}
	LOG_INFO("times=%d,%d,%d,%d,%d,%d\n",t.tm_sec, t.tm_min, t.tm_hour, t.tm_year, t.tm_mon, t.tm_mday);

	return  mktime(&t);
}


/**
 * standard GPGGA sentence parser
 */
int nmea_gpgga(nmeap_context_t *context, nmeap_sentence_t *sentence)
{
	/*
	 * get pointer to sentence data
	 */
	NmeaGga *gga = (NmeaGga *)sentence->data;

	ASSERT(gga);

	/*
	 * if there is a data element, extract data from the tokens
	 */
	gga->latitude   = nmea_latitude(context->token[2],context->token[3]);
	gga->longitude  = nmea_longitude(context->token[4],context->token[5]);
	gga->altitude   = nmea_altitude(context->token[9],context->token[10]);
	gga->time       = timestampToSec(tokenToInt(context->token[1], 3), 0);
	gga->satellites = atoi(context->token[7]);
	gga->quality    = atoi(context->token[6]);
	gga->hdop       = tokenToInt(context->token[8], 1);
	gga->geoid      = nmea_altitude(context->token[11],context->token[12]);

	/*
	 * if the sentence has a callout, call it
	 */

	if (sentence->callout != 0)
		(*sentence->callout)(context, gga, context->user_data);

	return NMEA_GPGGA;
}

/**
 * standard GPRMCntence parser
 */
int nmea_gprmc(nmeap_context_t *context, nmeap_sentence_t *sentence)
{

    /*
	 * get pointer to sentence data
	 */
    NmeaRmc *rmc = (NmeaRmc *)sentence->data;

	/*
	 * if there is a data element, use it
	 */
	if (rmc != 0)
	{
		/*
		 * extract data from the tokens
		 */
		rmc->time       = timestampToSec(tokenToInt(context->token[1], 3), atoi(context->token[9]));
		rmc->warn       = *context->token[2];
		rmc->latitude   = nmea_latitude(context->token[3],context->token[4]);
		rmc->longitude  = nmea_longitude(context->token[5],context->token[6]);
		rmc->speed      = atoi(context->token[7]);
		rmc->course     = atoi(context->token[8]);
		rmc->mag_var    = atoi(context->token[10]);
	}

    /*
	 * if the sentence has a callout, call it
	 */
    if (sentence->callout != 0)
        (*sentence->callout)(context, rmc, context->user_data);

    return NMEA_GPRMC;
}


/**
 * standard GPVTG sentence parser
 */
int nmea_gpvtg(nmeap_context_t *context, nmeap_sentence_t *sentence)
{

    /*
	 * get pointer to sentence data
	 */
    NmeaVtg *vtg = (NmeaVtg *)sentence->data;

	/*
	 * if there is a data element, use it
	 */
	if (vtg != 0)
	{
		/*
		 * extract data from the tokens
		 */
		vtg->track_good  = atoi(context->token[1]);
		vtg->knot_speed  = atoi(context->token[5]);
		vtg->km_speed    = atoi(context->token[7]);
	}

    /*
	 * if the sentence has a callout, call it
	 */
    if (sentence->callout != 0)
        (*sentence->callout)(context, vtg, context->user_data);

    return NMEA_GPVTG;
}

/**
 * standard GPGDSV sentence parser
 */
int nmea_gpgsv(nmeap_context_t *context, nmeap_sentence_t *sentence)
{

    /*
	 * get pointer to sentence data
	 */
    NmeaGsv *gsv = (NmeaGsv *)sentence->data;

	/*
	 * if there is a data element, use it
	 */
	if (gsv != 0)
	{
		/*
		 * extract data from the tokens
		 */
		gsv->tot_message     = atoi(context->token[1]);
		gsv->message_num     = atoi(context->token[2]);
		gsv->tot_svv         = atoi(context->token[3]);
		gsv->sv_prn          = atoi(context->token[4]);
		gsv->elevation       = atoi(context->token[5]);
		gsv->azimut          = atoi(context->token[6]);
		gsv->snr             = atoi(context->token[7]);
		gsv->sv_prn2         = atoi(context->token[8]);
		gsv->elevation2      = atoi(context->token[9]);
		gsv->azimut2         = atoi(context->token[10]);
		gsv->snr2            = atoi(context->token[11]);
		gsv->sv_prn3         = atoi(context->token[12]);
		gsv->elevation3      = atoi(context->token[13]);
		gsv->azimut3         = atoi(context->token[14]);
		gsv->snr3            = atoi(context->token[15]);
		gsv->sv_prn4         = atoi(context->token[16]);
		gsv->elevation4      = atoi(context->token[17]);
		gsv->azimut4         = atoi(context->token[18]);
		gsv->snr4            = atoi(context->token[19]);
	}


    /*
	 * if the sentence has a callout, call it
	 */
    if (sentence->callout != 0)
        (*sentence->callout)(context, gsv, context->user_data);

    return NMEA_GPGSV;
}

void nmea_poll(nmeap_context_t *context, KFile *channel)
{
	int c;
	while ((c = kfile_getc(channel)) != EOF)
	{
		nmeap_parse(context, c);
	}
}

