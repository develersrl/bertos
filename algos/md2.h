/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief MD2 Message-Digest algorithm.
 *
 * \version $Id$
 * \author Daniele Basile <asterix@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2007/01/30 15:53:26  batt
 *#* Add first md2 skel.
 *#*
 *#*/

#ifndef ALGOS_MD2_H
#define ALGOS_MD2_H

#include <cfg/compiler.h>
#include <appconfig.h>

/**
 * Context for MD2 computation.
 */
typedef struct Md2Context
{
	uint8_t buffer[CONFIG_MD2_BLOBK_LEN];   ///< Input buffer.
	uint8_t state[CONFIG_MD2_BLOBK_LEN];    ///< Current state buffer.
	uint8_t checksum[CONFIG_MD2_BLOBK_LEN]; ///< Checksum.
	size_t counter;                         ///< Counter of remaining bytes.
} Md2Context;


#endif /* ALGOS_MD2_H */
