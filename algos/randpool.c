/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief API function for to manage entropy pool.
 *
 * \version $Id$
 * \author Daniele Basile <asterix@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.7  2007/02/09 15:49:54  asterix
 *#* Fix bug in randpool_stir and randpool_add. Typos.
 *#*
 *#* Revision 1.6  2007/02/09 09:24:38  asterix
 *#* Typos. Add data_len in randpool_add and n_byte in randpool_push pototypes.
 *#*
 *#* Revision 1.3  2007/02/08 14:25:29  asterix
 *#* Write static funcion push_byte.
 *#*
 *#*/

#include "randpool.h"
#include "md2.h"

#include <string.h>            //memset(), memcpy();
#include <cfg/compiler.h>
#include <cfg/debug.h>        //ASSERT()
#include <drv/timer.h>        //timer_clock();

#include <stdio.h>            //sprintf();



/*
 * Insert bytes in entropy pool, making a XOR of bytes present
 * in entropy pool.
 */
static void randpool_push(EntropyPool *pool, void *_byte, size_t n_byte)
{
	size_t i = pool->pos_add; // Current number of byte insert in entropy pool.
	uint8_t *byte;

	byte = (uint8_t *)_byte;

	/*
	 * Insert a bytes in entropy pool.
	 */
	for(int j = 0; j < n_byte; j++)
	{
		pool->pool_entropy[i] = pool->pool_entropy[i] ^ byte[j];
		i = (i++) % CONFIG_SIZE_ENTROPY_POOL;
	}

	pool->pos_add  =  i; // Update a insert bytes.
}


/**
 * Add n_bit of  entropy in entropy pool.
 */
void randpool_add(EntropyPool *pool, void *data, size_t data_len, size_t entropy)
{
	ticks_t event = timer_clock();
	uint32_t delta;
	uint8_t sep[] = "\xaa\xaa\xaa\xaa";  // ??

	randpool_push(pool, data, data_len); //Insert data to entropy pool.

	randpool_push(pool, sep, strlen(sep)); // ??

	/*Difference of time between a two accese to entropy pool.*/
	delta = event - pool->last_counter;

	randpool_push(pool, &delta, sizeof(delta));

	delta = delta & 0xff;

	randpool_push(pool, &delta, sizeof(delta));

	/*
	 * Count of number entropy bit add with delta.
	 */
	while(delta)
	{
		delta >>= 1;
		entropy++;
	}

	pool->entropy += entropy;      //Update a entropy of the pool.
	pool->last_counter = event;
}

/* \
 * This function stir entropy pool with MD2 function hash.
 *
 */
static void randpool_stir(EntropyPool *pool)
{
	size_t entropy = pool->entropy; //Save current calue of entropy.
	Md2Context context;
	uint8_t tmp_buf[((sizeof(size_t) * 2) + sizeof(int)) * 2]; //Temporary buffer.

	md2_init(&context); //Init MD2 algorithm.

	randpool_add(pool, "", 0, 0);

	for (int i = 0; i < NUM_STIR_LOOP; i++)
	{
		sprintf(tmp_buf, "%0x%0x%0x",pool->counter, i, pool->pos_add);

		/*
		 * Hash with MD2 algorithm the entropy pool.
		 */
		md2_update(&context, pool->pool_entropy, CONFIG_SIZE_ENTROPY_POOL);

		md2_update(&context, tmp_buf, strlen(tmp_buf));

		/*Insert a message digest in entropy pool.*/
		randpool_push(pool, md2_end(&context), CONFIG_MD2_BLOCK_LEN);

		pool->counter = (pool->counter + 1) & 0xFFFFFFFF; //Clamp a counter to 4 byte.

	}

	/*Insert in pool the difference between a two call of this function (see above).*/
	randpool_add(pool, "", 0, 0);

	pool->entropy = entropy; //Restore old value of entropy. We haven't add entropy.
}


void randpool_init(EntropyPool *pool)
{

	memset(pool, 0, sizeof(EntropyPool));
	pool->pos_get = CONFIG_MD2_BLOCK_LEN;
	pool->last_counter = timer_clock();

	//TODO: inizializzazione del timer di sistema.

}

/**
 * Get the actual value of entropy.
 */
size_t randpool_size(EntropyPool *pool)
{
	return pool->entropy;
}

void randpool_get(EntropyPool *pool, void *data, size_t n_byte)
{

}

void randpool_getN(EntropyPool *pool, void *data, size_t n_byte)
{
}

bool randpool_save(void *data)
{
}

uint8_t *randpool_load(void)
{
}

