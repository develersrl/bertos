/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/);
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief API function for to manage entropy pool.
 *
 *
 * \version $Id$
 * \author Daniele Basile <asterix@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.11  2007/02/15 13:54:26  asterix
 *#* Rename randpool_getN in randpool_get. Fix bug in randpool_get.
 *#*
 *#* Revision 1.10  2007/02/15 13:40:42  asterix
 *#* Fix bug in randpool_add and randpool_strir.
 *#*
 *#* Revision 1.9  2007/02/13 09:57:12  asterix
 *#* Add directive #if in struct EntropyPool, and remove #else in randpool_add.
 *#*
 *#* Revision 1.8  2007/02/12 09:47:39  asterix
 *#* Remove randpool_save. Add randpool_pool.
 *#*
 *#* Revision 1.6  2007/02/09 15:49:54  asterix
 *#* Fix bug in randpool_stir and randpool_add. Typos.
 *#*
 *#* Revision 1.5  2007/02/09 09:24:38  asterix
 *#* Typos. Add data_len in randpool_add and n_byte in randpool_push pototypes.
 *#*
 *#* Revision 1.4  2007/02/08 17:18:01  asterix
 *#* Write add_data and stir function. Typos
 *#*
 *#* Revision 1.3  2007/02/08 14:25:56  asterix
 *#* Typos.
 *#*
 *#* Revision 1.2  2007/02/08 11:53:03  asterix
 *#* Add EntropyPool struct. Typos.
 *#*
 *#* Revision 1.1  2007/02/08 11:13:41  asterix
 *#* Add function prototypes.
 *#*
 *#*/

#ifndef RANDPOOL_H 
#define RANDPOOL_H

#include <cfg/compiler.h>
#include <appconfig.h>


/**
 * Sturct data of entropy pool.
 */
typedef struct EntropyPool 
{
	size_t entropy;                                  ///< Actual value of entropy (byte).
	size_t pos_add;                                  ///< Number of byte  idd in entropy pool.
	size_t pos_get;                                  ///< Number of byte get in entropy pool.
	size_t counter;                                  ///< Counter.

#if CONFIG_RANDPOOL_TIMER
	size_t last_counter;                             ///< Last timer value.
#endif

	uint8_t pool_entropy[CONFIG_SIZE_ENTROPY_POOL];  ///< Entropy pool.

} EntropyPool;


void randpool_add(EntropyPool *pool, void *data, size_t entropy);
void randpool_init(EntropyPool *pool, void *_data, size_t len);
size_t randpool_size(EntropyPool *pool);
void randpool_get(EntropyPool *pool, void *data, size_t n_byte);
uint8_t *randpool_pool(EntropyPool *pool);

#endif /* RANDPOOL_H */
