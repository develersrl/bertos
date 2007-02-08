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
 *#* Revision 1.2  2007/02/08 11:53:03  asterix
 *#* Add EntrPool struct. Typos.
 *#*
 *#*/

#include "randpool.h"
#include "md2.h"

#include <cfg/compiler.h>
#include <cfg/debug.h>        //ASSERT()

static void stir(EntrPool *pool)
{
}

void init_pool(EntrPool *pool);
{
}

void add_data(EntrPool *pool, void *data, size_t n_bit);
{
}

size_t pool_size(EntrPool *pool);
{
}

void get_bit(EntrPool *pool, void *data, size_t n_bit);
{
}

void get_bit_n(EntrPool *pool, void *data, size_t n_bit);
{
}

bool save_pool(void *data);
{
}

uint8_t *load_pool(void);
{
}

