/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
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
#include <kern/kfile.h>

/**
 * Prototype of function for manage read/write on
 * flash memory.
 *\{
 */
size_t	prog_write(struct _KFile *fd, const void *buf, size_t size);
bool prog_open(struct _KFile *fd, UNUSED_ARG(const char *, name), UNUSED_ARG(int, mode));
bool prog_close(struct _KFile *fd);
bool prog_seek(struct _KFile *fd, int32_t offset);
size_t	prog_read(struct _KFile *fd, void *buf, size_t size);
bool prog_test(void);
/* \} */

#endif /* PROG_H */
