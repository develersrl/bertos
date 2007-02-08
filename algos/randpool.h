/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
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
 *#* Revision 1.4  2007/02/08 17:18:01  asterix
 *#* Write add_data and stir function. Typos
 *#*
 *#* Revision 1.3  2007/02/08 14:25:56  asterix
 *#* Typos.
 *#*
 *#* Revision 1.2  2007/02/08 11:53:03  asterix
 *#* Add EntrPool struct. Typos.
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
typedef struct EntrPool 
{
	size_t entropy;                                  ///< Actual value of entropy (In bit).
	size_t pool_pos_add;                             ///< Size of byte insert in entropy pool.
	size_t pool_pos_get;                             ///< Size of byte take in entropy pool.
	size_t counter;                                  ///< Counter.
	size_t last_counter;                             ///< Last timer value.
	uint8_t pool_entropy[CONFIG_SIZE_ENTROPY_POOL];  ///< Entropy pool.

} EntrPool;

void init_pool(EntrPool *pool);
void add_data(EntrPool *pool, void *data, size_t n_bit);
size_t pool_size(EntrPool *pool);
void get_bit(EntrPool *pool, void *data, size_t n_bit);
void get_bit_n(EntrPool *pool, void *data, size_t n_bit);
bool save_pool(void *data);
uint8_t *load_pool(void);

#endif /* RANDPOOL_H */
