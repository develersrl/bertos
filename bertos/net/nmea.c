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
 */

#include "nmea.h"

#include "cfg/cfg_nmea.h"

#include <cfg/debug.h>

#define LOG_LEVEL  NMEA_LOG_LEVEL
#define LOG_FORMAT NMEA_LOG_FORMAT
#include <cfg/log.h>

#include <net/nmeap/inc/nmeap.h>

#include <ctype.h>


static uint32_t tokenToInt(const char *s)
{
	uint32_t num = 0;
	int i;

	ASSERT(s);

	for(i = 0; i < NMEAP_MAX_TOKENS; i++)
	{
		char c = *s++;

		if (c == '.')
			continue;

		if (c == '\0' || !isdigit(c))
			break;

		num *= 10;
		num += c - '0';
	}

	return num;
}


static udegree_t nmea_latitude(const char *plat, const char *phem)
{
	int ns;
	uint32_t lat;
	uint32_t deg;
	uint32_t min;

	if (*plat == 0)
	{
        return 0;
    }
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

	lat = tokenToInt(plat);
	deg = lat / 1000000;
	min = lat - deg * 1000000;
	lat = deg * 1000000 + ((min * 5) + 1) / 3;

	return ns * lat;
}

static udegree_t nmea_longitude(const char *plot, const char *phem)
{
	int ew;
	uint32_t lot;
	uint32_t deg;
	uint32_t min;

	if (*plot == 0)
	{
        return 0;
    }
    if (*phem == 0)
	{
        return 0;
    }

    /* west long is negative, east long is positive */
    if (*phem == 'E')
	{
        ew = 1;
    }
    else {
        ew = -1;
    }

	lot = tokenToInt(plot);
	deg = lot / 1000000;
	min = lot - deg * 1000000;
	lot = deg * 1000000 + ((min * 5) + 1) / 3;

	return ew  * lot;
}

static cm_t nmea_altitude(const char *palt, const char *punits)
{
	uint32_t alt;

	if (*palt == 0)
	{
        return 0;
    }

	alt = tokenToInt(palt);

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

/**
 * standard GPGGA sentence parser
 */
int nmea_gpgga(nmeap_context_t *context, nmeap_sentence_t *sentence)
{
	/*
	 * get pointer to sentence data
	 */
	NmeaGga *gga = (NmeaGga *)sentence->data;

	/*
	 * if there is a data element, extract data from the tokens
	 */
	if (gga != 0)
	{
		gga->latitude   = nmea_latitude(context->token[2],context->token[3]);
		gga->longitude  = nmea_longitude(context->token[4],context->token[5]);
		gga->altitude   = nmea_altitude(context->token[9],context->token[10]);
		gga->time       = tokenToInt(context->token[1]);
		gga->satellites = tokenToInt(context->token[7]);
		gga->quality    = tokenToInt(context->token[6]);
		gga->hdop       = tokenToInt(context->token[8]);
		gga->geoid      = nmea_altitude(context->token[11],context->token[12]);

	}


	/*
	 * if the sentence has a callout, call it
	 */
	if (sentence->callout != 0)
		(*sentence->callout)(context,gga,context->user_data);

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
		rmc->time       = tokenToInt(context->token[1]);
		rmc->warn       = *context->token[2];
		rmc->latitude   = nmea_latitude(context->token[3],context->token[4]);
		rmc->longitude  = nmea_longitude(context->token[5],context->token[6]);
		rmc->speed      = tokenToInt(context->token[7]) * 100;
		rmc->course     = tokenToInt(context->token[8]) * 100;
		rmc->date       = tokenToInt(context->token[9]);
		rmc->mag_var    = tokenToInt(context->token[10]);
	}

    /*
	 * if the sentence has a callout, call it
	 */
    if (sentence->callout != 0)
        (*sentence->callout)(context,rmc,context->user_data);

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
		vtg->track_good  = tokenToInt(context->token[1]) * 100;
		vtg->knot_speed  = tokenToInt(context->token[5]) * 100;
		vtg->km_speed    = tokenToInt(context->token[7]) * 100;
	}

    /*
	 * if the sentence has a callout, call it
	 */
    if (sentence->callout != 0)
        (*sentence->callout)(context,vtg,context->user_data);

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
		gsv->tot_message     = tokenToInt(context->token[1]);
		gsv->message_num     = tokenToInt(context->token[2]);
		gsv->tot_svv         = tokenToInt(context->token[3]);
		gsv->sv_prn          = tokenToInt(context->token[4]);
		gsv->elevation       = tokenToInt(context->token[5]);
		gsv->azimut          = tokenToInt(context->token[6]);
		gsv->snr             = tokenToInt(context->token[7]);
		gsv->sv_prn2         = tokenToInt(context->token[8]);
		gsv->elevation2      = tokenToInt(context->token[9]);
		gsv->azimut2         = tokenToInt(context->token[10]);
		gsv->snr2            = tokenToInt(context->token[11]);
		gsv->sv_prn3         = tokenToInt(context->token[12]);
		gsv->elevation3      = tokenToInt(context->token[13]);
		gsv->azimut3         = tokenToInt(context->token[14]);
		gsv->snr3            = tokenToInt(context->token[15]);
		gsv->sv_prn4         = tokenToInt(context->token[16]);
		gsv->elevation4      = tokenToInt(context->token[17]);
		gsv->azimut4         = tokenToInt(context->token[18]);
		gsv->snr4            = tokenToInt(context->token[19]);
	}


    /*
	 * if the sentence has a callout, call it
	 */
    if (sentence->callout != 0)
        (*sentence->callout)(context,gsv,context->user_data);

    return NMEA_GPGSV;
}

void nmea_poll(nmeap_context_t *context, KFile *channel)
{
	int c;
	while ((c = kfile_getc(channel)) != EOF)
	{
		if (nmeap_parse(context, c) == -1)
			break;
	}
}

