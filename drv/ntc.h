/**
 * \file
 * <!--
 * Copyright 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Driver for NTC (reads a temperature through an ADC)
 *
 * \version $Id$
 *
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.1  2005/11/04 17:59:47  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.3  2005/06/10 08:56:47  batt
 *#* Avoid calling DEG_T_TO_DEG().
 *#*
 *#* Revision 1.2  2005/06/10 08:54:58  batt
 *#* Rename deg_t conversion macros to accomplish coding standard.
 *#*
 *#* Revision 1.1  2005/05/24 09:17:58  batt
 *#* Move drivers to top-level.
 *#*/

#ifndef DRV_NTC_H
#define DRV_NTC_H

#include <ntc_map.h>
#include <cfg/debug.h>
#include <cfg/compiler.h>

#define NTC_OPEN_CIRCUIT  -32768
#define NTC_SHORT_CIRCUIT 32767

typedef int16_t  deg_t; /** type for celsius degrees deg_t = °C * 10 */

/** Macro for converting from deg to deg_t type */
#define DEG_TO_DEG_T(x)         ((deg_t)((x) * 10))

/** Macro for converting from deg_t to celsius degrees (returns only the integer part) */
#define DEG_T_TO_INTDEG(x)      ((x) / 10)

/** Macro for converting from deg_t to celsius degrees (returns only the decimal part) */
#define DEG_T_TO_DECIMALDEG(x)  ((x) % 10)

/** Macro for converting from deg_t to celsius degrees (returns type is float) */
#define DEG_T_TO_FLOATDEG(x)    ((x) / 10.0)


typedef uint32_t res_t; /** type for resistor res_t = Ohm * 100 */
typedef float    amp_t; /** type for defining amplifications  amp_t = A, where A is a pure number */

DB(extern bool ntc_initialized;)


/** Describe a NTC chip */
typedef struct NtcHwInfo
{
	const res_t *resistances; ///< resistances of the NTC (ohms * 100)
	size_t num_resistances;   ///< number of resistances
	deg_t degrees_min;        ///< degrees corresponding to the first entry in the table (celsius * 10)
	deg_t degrees_step;       ///< difference in degrees between two consecutive elements in the table (celsius * 10)
} NtcHwInfo;

/** Initialize the NTC module */
void ntc_init(void);

/** Read a single temperature value from the NTC */
deg_t ntc_read(NtcDev dev);

#endif /* DRV_NTC_H */
