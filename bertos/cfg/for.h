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
 * Copyright 2012 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Preprocessor FOR macro.
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#ifndef CFG_FOR_H
#define CFG_FOR_H

#define IDENTITY(x) x

#define FOR(body, ...) \
		PP_CAT(FOR_, COUNT_PARMS(__VA_ARGS__)) (body, __VA_ARGS__)

#define FOR_1(body, x) IDENTITY(body x)

#define FOR_2(body, x, ...) \
		IDENTITY(body x) \
		FOR_1(body, __VA_ARGS__)

#define FOR_3(body, x, ...) \
		IDENTITY(body x) \
		FOR_2(body, __VA_ARGS__)

#define FOR_4(body, x, ...) \
		IDENTITY(body x) \
		FOR_3(body, __VA_ARGS__)

#define FOR_5(body, x, ...) \
		IDENTITY(body x) \
		FOR_4(body, __VA_ARGS__)

#define FOR_6(body, x, ...) \
		IDENTITY(body x) \
		FOR_5(body, __VA_ARGS__)

#define FOR_7(body, x, ...) \
		IDENTITY(body x) \
		FOR_6(body, __VA_ARGS__)

#define FOR_8(body, x, ...) \
		IDENTITY(body x) \
		FOR_7(body, __VA_ARGS__)

#define FOR_9(body, x, ...) \
		IDENTITY(body x) \
		FOR_8(body, __VA_ARGS__)

#define FOR_10(body, x, ...) \
		IDENTITY(body x) \
		FOR_9(body, __VA_ARGS__)

#define FOR_11(body, x, ...) \
		IDENTITY(body x) \
		FOR_10(body, __VA_ARGS__)

#define FOR_12(body, x, ...) \
		IDENTITY(body x) \
		FOR_11(body, __VA_ARGS__)

#define FOR_13(body, x, ...) \
		IDENTITY(body x) \
		FOR_12(body, __VA_ARGS__)

#define FOR_14(body, x, ...) \
		IDENTITY(body x) \
		FOR_13(body, __VA_ARGS__)

#define FOR_15(body, x, ...) \
		IDENTITY(body x) \
		FOR_14(body, __VA_ARGS__)

#define FOR_16(body, x, ...) \
		IDENTITY(body x) \
		FOR_15(body, __VA_ARGS__)

#define FOR_17(body, x, ...) \
		IDENTITY(body x) \
		FOR_16(body, __VA_ARGS__)

#define FOR_18(body, x, ...) \
		IDENTITY(body x) \
		FOR_17(body, __VA_ARGS__)

#define FOR_19(body, x, ...) \
		IDENTITY(body x) \
		FOR_18(body, __VA_ARGS__)

#define FOR_20(body, x, ...) \
		IDENTITY(body x) \
		FOR_19(body, __VA_ARGS__)

#define FOR_21(body, x, ...) \
		IDENTITY(body x) \
		FOR_20(body, __VA_ARGS__)

#define FOR_22(body, x, ...) \
		IDENTITY(body x) \
		FOR_21(body, __VA_ARGS__)

#define FOR_23(body, x, ...) \
		IDENTITY(body x) \
		FOR_22(body, __VA_ARGS__)

#define FOR_24(body, x, ...) \
		IDENTITY(body x) \
		FOR_23(body, __VA_ARGS__)

#define FOR_25(body, x, ...) \
		IDENTITY(body x) \
		FOR_24(body, __VA_ARGS__)

#define FOR_26(body, x, ...) \
		IDENTITY(body x) \
		FOR_25(body, __VA_ARGS__)

#define FOR_27(body, x, ...) \
		IDENTITY(body x) \
		FOR_26(body, __VA_ARGS__)

#define FOR_28(body, x, ...) \
		IDENTITY(body x) \
		FOR_27(body, __VA_ARGS__)

#define FOR_29(body, x, ...) \
		IDENTITY(body x) \
		FOR_28(body, __VA_ARGS__)

#define FOR_30(body, x, ...) \
		IDENTITY(body x) \
		FOR_29(body, __VA_ARGS__)

#define FOR_31(body, x, ...) \
		IDENTITY(body x) \
		FOR_30(body, __VA_ARGS__)

#define FOR_32(body, x, ...) \
		IDENTITY(body x) \
		FOR_31(body, __VA_ARGS__)

#endif /* CFG_FOR_H */
