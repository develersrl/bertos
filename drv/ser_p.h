/**
 * \file
 * Copyright (C) 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
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
 * Revision 1.1  2004/05/23 18:10:11  bernie
 * Import drv/ modules.
 *
 * Revision 1.1  2004/05/14 12:47:26  rasky
 * Importato nuovo supporto seriale per AVR da Stefano
 *
 * Revision 1.3  2004/05/08 13:59:08  aleph
 * Fix header guard
 *
 * Revision 1.2  2004/05/08 13:58:36  aleph
 * Add log comment
 *
 */

#ifndef _DRV_SER_P_H
#define _DRV_SER_P_H

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

#endif // _DRV_SER_P_H
