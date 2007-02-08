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
 *#* Revision 1.3  2007/02/08 14:25:29  asterix
 *#* Write static funcion push_byte.
 *#*
 *#*/

#include "randpool.h"
#include "md2.h"

#include <string.h>            //memset(), memcpy();
#include <cfg/compiler.h>
#include <cfg/debug.h>        //ASSERT()

static void stir(EntrPool *pool)
{
	
}

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

void init_pool(EntrPool *pool)
{
	
	memset(pool, 0, sizeof(EntrPool));

}

void add_data(EntrPool *pool, void *data, size_t n_bit)
{

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

