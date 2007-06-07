/**
 * \file
 * <!--
 * Copyright 2003, 2004 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Generic program read/write routines interface
 * 
 * This module implements an interface function for programming
 * a CPU flash memory. To guarantee correct memory writing, we
 * use a rotating hash algorithm.
 * 
 * 
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef PROG_H
#define PROG_H

#include <cfg/compiler.h>
#include <mware/kfile.h>
#include <algos/rotating_hash.h>

/**
 * Generic interface of progress status function.
 * \param value is current progress value
 * \param max is max progress value
 * \return True if programming should continue
 * \return False to stop programming
 */
typedef bool (*progress_func_t) (int value, int max);

/**
 * Generic interface for programming a CPU flash memory.
 * \param *file is the Kfile pointer to write
 * \param progress is progress fuction pointer, can be NULL if not used
 * \return rotating hash of file written
 */
rotating_t prog_write(struct _KFile *file, progress_func_t progress);

#endif /* PROG_H */
