/**
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief MCP41 hardware-specific definitions
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

#ifndef HW_MCP41_H
#define HW_MCP41_H

#include <mcp41_map.h>
#include <cfg/compiler.h>
#include <cfg/cpu.h>
#include <avr/io.h>

extern const uint16_t mcp41_ports[MCP41_CNT];
extern const uint8_t  mcp41_pins[MCP41_CNT];

/**
 * Set MCP41 port associated with \a dev to output.
 */
INLINE void SET_MCP41_DDR(Mcp41Dev dev)
{
	/* DDR port is 1 address before PORT */
	ATOMIC(_SFR_IO8(mcp41_ports[dev] - 1) |= mcp41_pins[dev]);
}

INLINE void MCP41_ON(Mcp41Dev i)
{
	ATOMIC(_SFR_IO8(mcp41_ports[i]) &= ~mcp41_pins[i]);
}

INLINE void MCP41_OFF(Mcp41Dev i)
{
	ATOMIC(_SFR_IO8(mcp41_ports[i]) |= mcp41_pins[i]);
}


#endif /* HW_MCP41_H */
