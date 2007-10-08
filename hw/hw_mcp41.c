/**
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief MCP41 hardware-specific definitions
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

#include <hw_mcp41.h>

#include <cfg/macros.h>
#include <cfg/compiler.h>

#include <avr/io.h>


const uint16_t mcp41_ports[MCP41_CNT] = {
                                        	_SFR_IO_ADDR(PORTB), /** Led pot */
                                        	_SFR_IO_ADDR(PORTB), /** Photometer pot */
					};
const uint8_t  mcp41_pins [MCP41_CNT] = {
                                        	BV(PB7),
						BV(PB6),
					};
