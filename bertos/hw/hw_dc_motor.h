/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief DC motor hardware-specific definitions
 *
 * \version $Id$
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef HW_DC_MOTOR_H
#define HW_DC_MOTOR_H

#warning TODO:This is an example implementation, you must implement it!

//DC motor 0
#define DC_MOTOR0_EN_PIN      /* Implement me! */
#define DC_MOTOR0_DIR_PIN     /* Implement me! */
#define DC_MOTOR0_CLEAR       /* Implement me! */
#define DC_MOTOR0_SET         /* Implement me! */

/* Add other motor here */


#define DC_MOTOR_PIN_INIT_MACRO(port, index) \
    do { \
        /* put here code to init pins */ \
	} while (0)


#define DC_MOTOR_SET_ENABLE(index)     do { /* Implement me! */ } while (0)
#define DC_MOTOR_SET_DISABLE(index)    do { /* Implement me! */ } while (0)
#define DC_MOTOR_DIR_CW_MACRO(index)   do { /* Implement me! */ } while (0)
#define DC_MOTOR_DIR_CCW_MACRO(index)  do { /* Implement me! */ } while (0)


#define DC_MOTOR_ENABLE(index) \
   do {  \
        switch (index) \
        { \
        case 0:\
            DC_MOTOR_SET_ENABLE(0); \
            break; \
		/* Add other motor here */ \
        } \
    } while(0)

#define DC_MOTOR_DISABLE(index) \
   do {  \
        switch (index) \
        { \
        case 0:\
            DC_MOTOR_SET_DISABLE(0); \
            break; \
		/* Add other motor here */ \
        } \
    } while(0)


#define DC_MOTOR_DIR_CW(index) \
   do {  \
        switch (index) \
        { \
        case 0:\
            DC_MOTOR_DIR_CW_MACRO(0); \
            break; \
		/* Add other motor here */ \
        } \
    } while(0)

 #define DC_MOTOR_DIR_CCW(index) \
   do {  \
        switch (index) \
        { \
        case 0:\
            DC_MOTOR_DIR_CCW_MACRO(0); \
            break; \
		/* Add other motor here */ \
        } \
    } while(0)



#define DC_MOTOR_SET_DIR(index, dir) \
    do { \
        if (dir) \
            DC_MOTOR_DIR_CW(index); \
        else \
            DC_MOTOR_DIR_CCW(index); \
    } while (0)

#define DC_MOTOR_INIT(index) \
    do { \
        switch (index) \
        { \
        case 0: \
            DC_MOTOR_PIN_INIT_MACRO(A, 0); \
            break; \
        case 1: \
            DC_MOTOR_PIN_INIT_MACRO(A, 1); \
            break; \
        } \
    } while (0)

#endif /* HW_DC_MOTOR_H */
