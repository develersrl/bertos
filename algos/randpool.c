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
 *#* Revision 1.4  2007/02/08 17:18:00  asterix
 *#* Write add_data and stir function. Typos
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

#include <stdio.h>   //sprintf();



/*
 * Insert bytes in entropy pool, making a XOR of bytes present
 * in entropy pool.
 */
static void push_byte(EntrPool *pool, void *_byte)
{
	size_t i = pool->pool_pos_add; // Current number of byte insert in entropy pool.
	size_t len_byte;
	uint8_t *byte;

		
	byte = (uint8_t *)_byte;
	len_byte = strlen(byte);

	/*
	 * Insert a bytes in entropy pool.
	 */
	for(int j = 0; j < len_byte; j++)
	{
		pool->pool_entropy[i] = pool->pool_entropy[i] ^ byte[j];
		i = (i++) % CONFIG_SIZE_ENTROPY_POOL;
	}

	pool->pool_pos_add  =  i; // Update a insert bytes.
}

/* 
 * This function stir entropy pool with MD2 function hash.
 *
 */
static void stir(EntrPool *pool)
{
	size_t entropy = pool->entropy; //Save current calue of entropy.
	Md2Context context;
	uint8_t tmp_buf[(sizeof(size_t) * 2) + sizeof(int)];  //Temporary buffer.

	md2_init(&context);
	
	add_data(pool, "", 0);
		
	for (int i = 0; i < (CONFIG_SIZE_ENTROPY_POOL / CONFIG_MD2_BLOCK_LEN); i++)
	{
		sprintf(tmp_buf, "%x%x%x",pool->counter, i, pool->pool_pos_add); 
		
		/*
		 * Hash with MD2 algorithm the entropy pool.
		 */
		md2_update(&context, pool->pool_entropy, CONFIG_SIZE_ENTROPY_POOL);
		
		md2_update(&context, tmp_buf, CONFIG_SIZE_ENTROPY_POOL);

		push_byte(pool, md2_end(&context)); //Insert a message digest in entropy pool.

		pool->counter = (pool->counter + 1) & 0xFFFFFFFF; //Update a counter modulo 4.
	
	}
	
	/*Insert in pool the difference between a two call of this function (see above).*/
	add_data(pool, "", 0); 
	
	pool->entropy = entropy; //Restore old value of entropy. We haven't add entropy.
}


void init_pool(EntrPool *pool)
{
	
	memset(pool, 0, sizeof(EntrPool));

	//TODO: inizializzazione del timer di sistema.

}

/**
 * Add n_bit of  entropy in entropy pool.
 */
void add_data(EntrPool *pool, void *data, size_t n_bit)
{
	uint32_t event = timer_clock();
	uint32_t delta;

	push_byte(pool, data); //Insert data to entropy pool. 
	
	push_byte(pool, "\xaa\xaa\xaa\xaa"); // ??

	/*Difference of time between a two accese to entropy pool.*/
	delta = event - pool->last_counter;
	
	push_byte(pool, &delta);
	
	delta = delta & 0xff;
	
	push_byte(pool, &delta);

	/*
	 * Count of number entropy bit add with delta.
	 */
	while(delta) 
	{
		delta >>= 1;
		n_bit++;
	}
	
	pool->entropy = n_bit;      //Update a entropy of the pool.
	pool->last_counter = event;
}

size_t pool_size(EntrPool *pool)
{
}

void get_bit(EntrPool *pool, void *data, size_t n_bit)
{
}

void get_bit_n(EntrPool *pool, void *data, size_t n_bit)
{
}

bool save_pool(void *data)
{
}

uint8_t *load_pool(void)
{
}

