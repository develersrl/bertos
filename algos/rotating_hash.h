/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Rotating Hash algorithm (interface).
 *
 * This is a simple yet powerfull checksum algorithm.
 * Instead of just xor-ing the data, rotating hash
 * circular shift the checksum 4 place left before xoring.
 * This is a bit more stronger than simply sum the data.
 *
 * \version $Id$
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2007/06/07 09:11:17  batt
 *#* Add rotating hash algorithm.
 *#*
 *#* Revision 1.1  2007/01/12 20:30:49  batt
 *#* Add right Rotating hash file.
 *#*
 *#*/

#ifndef ALGOS_ROTATING_H
#define ALGOS_ROTATING_H

#include <cfg/compiler.h>

typedef uint16_t rotating_t;

/**
 * Update checksum pointed by \c rot with \c c data.
 */
INLINE void rotating_update1(uint8_t c, rotating_t *rot)
{
	*rot = (*rot << 4) ^ (*rot >> 12) ^ c;
}

/**
 * Update checksum pointed by \c rot with data supplied in \c buf.
 */
INLINE void rotating_update(const void *_buf, size_t len, rotating_t *rot)
{
	const uint8_t *buf = (const uint8_t *)_buf;

	while (len--)
		rotating_update1(*buf++, rot);
}


#endif // ALGOS_ROTATING_H
