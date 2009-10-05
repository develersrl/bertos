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
 * \brief NMEA Parser
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "nmea"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_nmea.h"
 * $WIZ$ module_depends = "kfile", "nmeap01"
 */

#ifndef NET_NMEA_H
#define NET_NMEA_H

#include "cfg/cfg_nmea.h"

#include <net/nmeap/inc/nmeap.h>

#include <kern/kfile.h>

/*
 * Implemented NMEA parser strings.
 */
#define NMEA_GPGGA 1   // GGA MESSAGE ID
#define NMEA_GPRMC 2   // RMC MESSAGE ID
#define NMEA_GPVTG 3   // VTG MESSAGE ID
#define NMEA_GPGSV 4   // GSV MESSAGE ID

// Standart type to rappresent fiels.
typedef uint32_t udegree_t;    // Micro degrees
typedef uint32_t mdegree_t;    // Milli degrees
typedef uint16_t degree_t;     // Degrees
typedef uint16_t cm_t;         // Centimeter
typedef uint32_t sec_t;        // Seconds
typedef uint32_t str_time_t;   // Time format HH:MM:SS
typedef uint32_t mknots_t;     // Milli knots
typedef uint32_t mkh_t;        // Milli kilometers/hour
typedef uint16_t number_t;      // Pure number


/**
 * Extracted data from a GGA message
 */
typedef struct NmeaGga
{
	udegree_t     latitude;
	udegree_t     longitude;
	cm_t          altitude;
	sec_t         time;
	number_t       satellites;
	number_t       quality;
	udegree_t     hdop;
	udegree_t     geoid;
} NmeaGga;

/**
 * Extracted data from an RMC message
 */
typedef struct NmeaRmc
{
	str_time_t    time;
	char          warn;
	udegree_t     latitude;
	udegree_t     longitude;
	mknots_t      speed;
	mdegree_t     course;
	str_time_t    date;
	mdegree_t     mag_var;
} NmeaRmc;

/**
 * Extracted data from an vtg message
 */
typedef struct NmeaVtg
{
	mdegree_t     track_good;
	mknots_t      knot_speed;
	mkh_t         km_speed;
} NmeaVtg;

/**
 * Extracted data from an gsv message
 */
typedef struct NmeaGsv
{
	number_t       tot_message;
	number_t       message_num;
	number_t       tot_svv;
	number_t       sv_prn;
	degree_t	  elevation;
	degree_t      azimut;
	number_t       snr;
	number_t       sv_prn2;
	degree_t	  elevation2;
	degree_t      azimut2;
	number_t       snr2;
	number_t       sv_prn3;
	degree_t	  elevation3;
	degree_t      azimut3;
	number_t       snr3;
	number_t       sv_prn4;
	degree_t	  elevation4;
	degree_t      azimut4;
	number_t       snr4;
} NmeaGsv;

void nmea_poll(nmeap_context_t *context, KFile *channel);

int nmea_gpgsv(nmeap_context_t *context, nmeap_sentence_t *sentence);
int nmea_gpvtg(nmeap_context_t *context, nmeap_sentence_t *sentence);
int nmea_gprmc(nmeap_context_t *context, nmeap_sentence_t *sentence);
int nmea_gpgga(nmeap_context_t *context, nmeap_sentence_t *sentence);

int nmea_testSetup(void);
int nmea_testTearDown(void);
int nmea_testRun(void);

#endif /* NET_NMEA_H */
