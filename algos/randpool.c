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
 *#* Revision 1.1  2007/02/08 11:13:40  asterix
 *#* Add function prototypes.
 *#*
 *#*/

#include "randpool.h"

#include <cfg/compiler.h>
#include <cfg/debug.h>        //ASSERT()


void add_entropy(void *data, size_t n_bit)
{
}

size_t pool_size(void)
{
}

void get_bit(void *data, size_t n_bit)
{
}

void get_bit_n(void *data, size_t n_bit)
{
}

bool save_pool(void)
{
}

uint8_t load_pool(void)
{
}

