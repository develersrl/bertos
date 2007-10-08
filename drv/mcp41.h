/**
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief MCP41xxx digital potentiomenter driver (interface)
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

#ifndef DRV_MCP41_H
#define DRV_MCP41_H

#include <hw/mcp41_map.h>
#include <cfg/compiler.h>
#include <drv/ser.h>

#define MCP41_WRITE_DATA 0x11
#define MCP41_HW_MAX     255
#define MCP41_MAX        999

/**
 * Type for digital potentiometer resistance.
 */
typedef uint16_t mcp41_res_t;

void mcp41_setResistance(Mcp41Dev dev, mcp41_res_t res);
void mcp41_init(Serial *spi_port);

#endif /* DRV_MCP41_H */
