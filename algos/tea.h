/**
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief TEA Tiny Encription Algorith functions (interface).
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 *
 * Documentation for TEA is available at
 * http://www.cl.cam.ac.uk/ftp/users/djw3/tea.ps.
 */

#ifndef ALGOS_TEA_H
#define ALGOS_TEA_H

#include <cfg/compiler.h>

#define TEA_KEY_LEN     16	//!< TEA key size.
#define TEA_BLOCK_LEN   8	//!< TEA block length.

#define DELTA   0x9E3779B9	//!< Magic value. (Golden number * 2^31)
#define ROUNDS  32		//!< Number of rounds.

void tea_enc(void *_v, void *_k);
void tea_dec(void *_v, void *_k);

#endif /* ALGOS_TEA_H */

