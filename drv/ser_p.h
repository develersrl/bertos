/*!
 * \file
 * <!--
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Hardware dependent serial driver (interface)
 *
 * \version $Id$
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Giovanni Bajo <rasky@develer.com>
 */

/*
 * $Log$
 * Revision 1.3  2004/06/03 11:27:09  bernie
 * Add dual-license information.
 *
 * Revision 1.2  2004/05/23 18:21:53  bernie
 * Trim CVS logs and cleanup header info.
 *
 */

#ifndef DRV_SER_P_H
#define DRV_SER_P_H

struct SerialHardware;
struct Serial;

struct SerialHardwareVT
{
	void (*init)(struct SerialHardware* ctx, struct Serial* ser);
	void (*cleanup)(struct SerialHardware* ctx);
	void (*setbaudrate)(struct SerialHardware* ctx, unsigned long rate);
	void (*setparity)(struct SerialHardware* ctx, int parity);
	void (*enabletxirq)(struct SerialHardware* ctx);
};

struct SerialHardware
{
	const struct SerialHardwareVT* table;
};

struct SerialHardware* ser_hw_getdesc(int unit);

#endif /* DRV_SER_P_H */
