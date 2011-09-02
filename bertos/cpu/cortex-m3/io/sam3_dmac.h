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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 *  SAM3X DMAC register definitions.
 */

#ifndef SAM3_DMAC_H
#define SAM3_DMAC_H

/** DMAC registers base. */
#define DMAC_BASE                0x400C4000

#define DMAC_GCFG             (*((reg32_t *)(DMAC_BASE + 0x000))) ///< Global Configuration Register.
#define DMAC_EN               (*((reg32_t *)(DMAC_BASE + 0x004))) ///< Enable Register.
#define DMAC_SREQ             (*((reg32_t *)(DMAC_BASE + 0x008))) ///< Software Single Request Register.
#define DMAC_CREQ             (*((reg32_t *)(DMAC_BASE + 0x00C))) ///< Software Chunk Transfer Request Register.
#define DMAC_LAST             (*((reg32_t *)(DMAC_BASE + 0x010))) ///< Software Last Transfer Flag Register.
#define DMAC_EBCIER           (*((reg32_t *)(DMAC_BASE + 0x018))) ///< Error, Chained Buffer Transfer Completed Interrupt and Buffer Transfer Completed Interrupt Enable register.
#define DMAC_EBCIDR           (*((reg32_t *)(DMAC_BASE + 0x01C))) ///< Error, Chained Buffer Transfer Completed Interrupt and Buffer Transfer Completed Interrupt Disable register.
#define DMAC_EBCIMR           (*((reg32_t *)(DMAC_BASE + 0x020))) ///< Error, Chained Buffer Transfer Completed Interrupt and Buffer transfer completed Mask Register.
#define DMAC_EBCISR           (*((reg32_t *)(DMAC_BASE + 0x024))) ///< Error, Chained Buffer Transfer Completed Interrupt and Buffer transfer completed Status Register.
#define DMAC_CHER             (*((reg32_t *)(DMAC_BASE + 0x028))) ///< Channel Handler Enable Register.
#define DMAC_CHDR             (*((reg32_t *)(DMAC_BASE + 0x02C))) ///< Channel Handler Disable Register.
#define DMAC_CHSR             (*((reg32_t *)(DMAC_BASE + 0x030))) ///< Channel Handler Status Register.
#define DMAC_SADDR0           (*((reg32_t *)(DMAC_BASE + 0x03C))) ///< Channel Source Address Register (ch_num = 0).
#define DMAC_DADDR0           (*((reg32_t *)(DMAC_BASE + 0x040))) ///< Channel Destination Address Register (ch_num = 0).
#define DMAC_DSCR0            (*((reg32_t *)(DMAC_BASE + 0x044))) ///< Channel Descriptor Address Register (ch_num = 0).
#define DMAC_CTRLA0           (*((reg32_t *)(DMAC_BASE + 0x048))) ///< Channel Control A Register (ch_num = 0).
#define DMAC_CTRLB0           (*((reg32_t *)(DMAC_BASE + 0x04C))) ///< Channel Control B Register (ch_num = 0).
#define DMAC_CFG0             (*((reg32_t *)(DMAC_BASE + 0x050))) ///< Channel Configuration Register (ch_num = 0).
#define DMAC_SADDR1           (*((reg32_t *)(DMAC_BASE + 0x064))) ///< Channel Source Address Register (ch_num = 1).
#define DMAC_DADDR1           (*((reg32_t *)(DMAC_BASE + 0x068))) ///< Channel Destination Address Register (ch_num = 1).
#define DMAC_DSCR1            (*((reg32_t *)(DMAC_BASE + 0x06C))) ///< Channel Descriptor Address Register (ch_num = 1).
#define DMAC_CTRLA1           (*((reg32_t *)(DMAC_BASE + 0x070))) ///< Channel Control A Register (ch_num = 1).
#define DMAC_CTRLB1           (*((reg32_t *)(DMAC_BASE + 0x074))) ///< Channel Control B Register (ch_num = 1).
#define DMAC_CFG1             (*((reg32_t *)(DMAC_BASE + 0x078))) ///< Channel Configuration Register (ch_num = 1).
#define DMAC_SADDR2           (*((reg32_t *)(DMAC_BASE + 0x08C))) ///< Channel Source Address Register (ch_num = 2).
#define DMAC_DADDR2           (*((reg32_t *)(DMAC_BASE + 0x090))) ///< Channel Destination Address Register (ch_num = 2).
#define DMAC_DSCR2            (*((reg32_t *)(DMAC_BASE + 0x094))) ///< Channel Descriptor Address Register (ch_num = 2).
#define DMAC_CTRLA2           (*((reg32_t *)(DMAC_BASE + 0x098))) ///< Channel Control A Register (ch_num = 2).
#define DMAC_CTRLB2           (*((reg32_t *)(DMAC_BASE + 0x09C))) ///< Channel Control B Register (ch_num = 2).
#define DMAC_CFG2             (*((reg32_t *)(DMAC_BASE + 0x0A0))) ///< Channel Configuration Register (ch_num = 2).
#define DMAC_SADDR3           (*((reg32_t *)(DMAC_BASE + 0x0B4))) ///< Channel Source Address Register (ch_num = 3).
#define DMAC_DADDR3           (*((reg32_t *)(DMAC_BASE + 0x0B8))) ///< Channel Destination Address Register (ch_num = 3).
#define DMAC_DSCR3            (*((reg32_t *)(DMAC_BASE + 0x0BC))) ///< Channel Descriptor Address Register (ch_num = 3).
#define DMAC_CTRLA3           (*((reg32_t *)(DMAC_BASE + 0x0C0))) ///< Channel Control A Register (ch_num = 3).
#define DMAC_CTRLB3           (*((reg32_t *)(DMAC_BASE + 0x0C4))) ///< Channel Control B Register (ch_num = 3).
#define DMAC_CFG3             (*((reg32_t *)(DMAC_BASE + 0x0C8))) ///< Channel Configuration Register (ch_num = 3).
#define DMAC_SADDR4           (*((reg32_t *)(DMAC_BASE + 0x0DC))) ///< Channel Source Address Register (ch_num = 4).
#define DMAC_DADDR4           (*((reg32_t *)(DMAC_BASE + 0x0E0))) ///< Channel Destination Address Register (ch_num = 4).
#define DMAC_DSCR4            (*((reg32_t *)(DMAC_BASE + 0x0E4))) ///< Channel Descriptor Address Register (ch_num = 4).
#define DMAC_CTRLA4           (*((reg32_t *)(DMAC_BASE + 0x0E8))) ///< Channel Control A Register (ch_num = 4).
#define DMAC_CTRLB4           (*((reg32_t *)(DMAC_BASE + 0x0EC))) ///< Channel Control B Register (ch_num = 4).
#define DMAC_CFG4             (*((reg32_t *)(DMAC_BASE + 0x0F0))) ///< Channel Configuration Register (ch_num = 4).
#define DMAC_SADDR5           (*((reg32_t *)(DMAC_BASE + 0x104))) ///< Channel Source Address Register (ch_num = 5).
#define DMAC_DADDR5           (*((reg32_t *)(DMAC_BASE + 0x108))) ///< Channel Destination Address Register (ch_num = 5).
#define DMAC_DSCR5            (*((reg32_t *)(DMAC_BASE + 0x10C))) ///< Channel Descriptor Address Register (ch_num = 5).
#define DMAC_CTRLA5           (*((reg32_t *)(DMAC_BASE + 0x110))) ///< Channel Control A Register (ch_num = 5).
#define DMAC_CTRLB5           (*((reg32_t *)(DMAC_BASE + 0x114))) ///< Channel Control B Register (ch_num = 5).
#define DMAC_CFG5             (*((reg32_t *)(DMAC_BASE + 0x118))) ///< Channel Configuration Register (ch_num = 5).
#define DMAC_WPMR             (*((reg32_t *)(DMAC_BASE + 0x1E4))) ///< Write Protect Mode Register.
#define DMAC_WPSR             (*((reg32_t *)(DMAC_BASE + 0x1E8))) ///< Write Protect Status Register.




/*
 * DMAC_GCFG : (DMAC Offset: 0x000) DMAC Global Configuration Register
 */
#define    DMAC_GCFG_ARB_CFG                       4 ///< (DMAC_GCFG) Arbiter Configuration.

/*
 * DMAC_EN : (DMAC Offset: 0x004) DMAC Enable Register
 */
#define    DMAC_EN_ENABLE           0 ///< (DMAC_EN).

/*
 * DMAC_SREQ : (DMAC Offset: 0x008) DMAC Software Single Request Register
 */
#define    DMAC_SREQ_SSREQ0           0 ///< (DMAC_SREQ) Source Request.
#define    DMAC_SREQ_DSREQ0           1 ///< (DMAC_SREQ) Destination Request.
#define    DMAC_SREQ_SSREQ1           2 ///< (DMAC_SREQ) Source Request.
#define    DMAC_SREQ_DSREQ1           3 ///< (DMAC_SREQ) Destination Request.
#define    DMAC_SREQ_SSREQ2           4 ///< (DMAC_SREQ) Source Request.
#define    DMAC_SREQ_DSREQ2           5 ///< (DMAC_SREQ) Destination Request.
#define    DMAC_SREQ_SSREQ3           6 ///< (DMAC_SREQ) Source Request.
#define    DMAC_SREQ_DSREQ3           7 ///< (DMAC_SREQ) Destination Request.
#define    DMAC_SREQ_SSREQ4           8 ///< (DMAC_SREQ) Source Request.
#define    DMAC_SREQ_DSREQ4           9 ///< (DMAC_SREQ) Destination Request.
#define    DMAC_SREQ_SSREQ5           10 ///< (DMAC_SREQ) Source Request.
#define    DMAC_SREQ_DSREQ5           11 ///< (DMAC_SREQ) Destination Request.

/*
 * DMAC_CREQ : (DMAC Offset: 0x00C) DMAC Software Chunk Transfer Request Register
 */
#define    DMAC_CREQ_SCREQ0           0 ///< (DMAC_CREQ) Source Chunk Request.
#define    DMAC_CREQ_DCREQ0           1 ///< (DMAC_CREQ) Destination Chunk Request.
#define    DMAC_CREQ_SCREQ1           2 ///< (DMAC_CREQ) Source Chunk Request.
#define    DMAC_CREQ_DCREQ1           3 ///< (DMAC_CREQ) Destination Chunk Request.
#define    DMAC_CREQ_SCREQ2           4 ///< (DMAC_CREQ) Source Chunk Request.
#define    DMAC_CREQ_DCREQ2           5 ///< (DMAC_CREQ) Destination Chunk Request.
#define    DMAC_CREQ_SCREQ3           6 ///< (DMAC_CREQ) Source Chunk Request.
#define    DMAC_CREQ_DCREQ3           7 ///< (DMAC_CREQ) Destination Chunk Request.
#define    DMAC_CREQ_SCREQ4           8 ///< (DMAC_CREQ) Source Chunk Request.
#define    DMAC_CREQ_DCREQ4           9 ///< (DMAC_CREQ) Destination Chunk Request.
#define    DMAC_CREQ_SCREQ5           10 ///< (DMAC_CREQ) Source Chunk Request.
#define    DMAC_CREQ_DCREQ5           11 ///< (DMAC_CREQ) Destination Chunk Request.

/*
 * DMAC_LAST : (DMAC Offset: 0x010) DMAC Software Last Transfer Flag Register
 */
#define    DMAC_LAST_SLAST0           0 ///< (DMAC_LAST) Source Last.
#define    DMAC_LAST_DLAST0           1 ///< (DMAC_LAST) Destination Last.
#define    DMAC_LAST_SLAST1           2 ///< (DMAC_LAST) Source Last.
#define    DMAC_LAST_DLAST1           3 ///< (DMAC_LAST) Destination Last.
#define    DMAC_LAST_SLAST2           4 ///< (DMAC_LAST) Source Last.
#define    DMAC_LAST_DLAST2           5 ///< (DMAC_LAST) Destination Last.
#define    DMAC_LAST_SLAST3           6 ///< (DMAC_LAST) Source Last.
#define    DMAC_LAST_DLAST3           7 ///< (DMAC_LAST) Destination Last.
#define    DMAC_LAST_SLAST4           8 ///< (DMAC_LAST) Source Last.
#define    DMAC_LAST_DLAST4           9 ///< (DMAC_LAST) Destination Last.
#define    DMAC_LAST_SLAST5           10 ///< (DMAC_LAST) Source Last.
#define    DMAC_LAST_DLAST5           11 ///< (DMAC_LAST) Destination Last.

/*
 * DMAC_EBCIER : (DMAC Offset: 0x018) DMAC Error,
 * Chained Buffer Transfer Completed Interrupt and Buffer Transfer Completed Interrupt Enable register.
 */
#define    DMAC_EBCIER_BTC0           0 ///< (DMAC_EBCIER) Buffer Transfer Completed [5:0].
#define    DMAC_EBCIER_BTC1           1 ///< (DMAC_EBCIER) Buffer Transfer Completed [5:0].
#define    DMAC_EBCIER_BTC2           2 ///< (DMAC_EBCIER) Buffer Transfer Completed [5:0].
#define    DMAC_EBCIER_BTC3           3 ///< (DMAC_EBCIER) Buffer Transfer Completed [5:0].
#define    DMAC_EBCIER_BTC4           4 ///< (DMAC_EBCIER) Buffer Transfer Completed [5:0].
#define    DMAC_EBCIER_BTC5           5 ///< (DMAC_EBCIER) Buffer Transfer Completed [5:0].
#define    DMAC_EBCIER_CBTC0          8 ///< (DMAC_EBCIER) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCIER_CBTC1          9 ///< (DMAC_EBCIER) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCIER_CBTC2          10 ///< (DMAC_EBCIER) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCIER_CBTC3          11 ///< (DMAC_EBCIER) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCIER_CBTC4          12 ///< (DMAC_EBCIER) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCIER_CBTC5          13 ///< (DMAC_EBCIER) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCIER_ERR0           16 ///< (DMAC_EBCIER) Access Error [5:0].
#define    DMAC_EBCIER_ERR1           17 ///< (DMAC_EBCIER) Access Error [5:0].
#define    DMAC_EBCIER_ERR2           18 ///< (DMAC_EBCIER) Access Error [5:0].
#define    DMAC_EBCIER_ERR3           19 ///< (DMAC_EBCIER) Access Error [5:0].
#define    DMAC_EBCIER_ERR4           20 ///< (DMAC_EBCIER) Access Error [5:0].
#define    DMAC_EBCIER_ERR5           21 ///< (DMAC_EBCIER) Access Error [5:0].

/* DMAC_EBCIDR : (DMAC Offset: 0x01C) DMAC Error, Chained Buffer Transfer Completed Interrupt and Buffer Transfer Completed Interrupt Disable register.*/
#define    DMAC_EBCIDR_BTC0           0 ///< (DMAC_EBCIDR) Buffer Transfer Completed [5:0].
#define    DMAC_EBCIDR_BTC1           1 ///< (DMAC_EBCIDR) Buffer Transfer Completed [5:0].
#define    DMAC_EBCIDR_BTC2           2 ///< (DMAC_EBCIDR) Buffer Transfer Completed [5:0].
#define    DMAC_EBCIDR_BTC3           3 ///< (DMAC_EBCIDR) Buffer Transfer Completed [5:0].
#define    DMAC_EBCIDR_BTC4           4 ///< (DMAC_EBCIDR) Buffer Transfer Completed [5:0].
#define    DMAC_EBCIDR_BTC5           5 ///< (DMAC_EBCIDR) Buffer Transfer Completed [5:0].
#define    DMAC_EBCIDR_CBTC0          8 ///< (DMAC_EBCIDR) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCIDR_CBTC1          9 ///< (DMAC_EBCIDR) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCIDR_CBTC2          10 ///< (DMAC_EBCIDR) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCIDR_CBTC3          11 ///< (DMAC_EBCIDR) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCIDR_CBTC4          12 ///< (DMAC_EBCIDR) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCIDR_CBTC5          13 ///< (DMAC_EBCIDR) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCIDR_ERR0           16 ///< (DMAC_EBCIDR) Access Error [5:0].
#define    DMAC_EBCIDR_ERR1           17 ///< (DMAC_EBCIDR) Access Error [5:0].
#define    DMAC_EBCIDR_ERR2           18 ///< (DMAC_EBCIDR) Access Error [5:0].
#define    DMAC_EBCIDR_ERR3           19 ///< (DMAC_EBCIDR) Access Error [5:0].
#define    DMAC_EBCIDR_ERR4           20 ///< (DMAC_EBCIDR) Access Error [5:0].
#define    DMAC_EBCIDR_ERR5           21 ///< (DMAC_EBCIDR) Access Error [5:0].

/* DMAC_EBCIMR : (DMAC Offset: 0x020) DMAC Error, Chained Buffer Transfer Completed Interrupt and Buffer transfer completed Mask Register.*/
#define    DMAC_EBCIMR_BTC0           0 ///< (DMAC_EBCIMR) Buffer Transfer Completed [5:0].
#define    DMAC_EBCIMR_BTC1           1 ///< (DMAC_EBCIMR) Buffer Transfer Completed [5:0].
#define    DMAC_EBCIMR_BTC2           2 ///< (DMAC_EBCIMR) Buffer Transfer Completed [5:0].
#define    DMAC_EBCIMR_BTC3           3 ///< (DMAC_EBCIMR) Buffer Transfer Completed [5:0].
#define    DMAC_EBCIMR_BTC4           4 ///< (DMAC_EBCIMR) Buffer Transfer Completed [5:0].
#define    DMAC_EBCIMR_BTC5           5 ///< (DMAC_EBCIMR) Buffer Transfer Completed [5:0].
#define    DMAC_EBCIMR_CBTC0          8 ///< (DMAC_EBCIMR) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCIMR_CBTC1          9 ///< (DMAC_EBCIMR) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCIMR_CBTC2          10 ///< (DMAC_EBCIMR) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCIMR_CBTC3          11 ///< (DMAC_EBCIMR) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCIMR_CBTC4          12 ///< (DMAC_EBCIMR) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCIMR_CBTC5          13 ///< (DMAC_EBCIMR) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCIMR_ERR0           16 ///< (DMAC_EBCIMR) Access Error [5:0].
#define    DMAC_EBCIMR_ERR1           17 ///< (DMAC_EBCIMR) Access Error [5:0].
#define    DMAC_EBCIMR_ERR2           18 ///< (DMAC_EBCIMR) Access Error [5:0].
#define    DMAC_EBCIMR_ERR3           19 ///< (DMAC_EBCIMR) Access Error [5:0].
#define    DMAC_EBCIMR_ERR4           20 ///< (DMAC_EBCIMR) Access Error [5:0].
#define    DMAC_EBCIMR_ERR5           21 ///< (DMAC_EBCIMR) Access Error [5:0].

/* DMAC_EBCISR : (DMAC Offset: 0x024) DMAC Error, Chained Buffer Transfer Completed Interrupt and Buffer transfer completed Status Register.*/
#define    DMAC_EBCISR_BTC0            0 ///< (DMAC_EBCISR) Buffer Transfer Completed [5:0].
#define    DMAC_EBCISR_BTC1            1 ///< (DMAC_EBCISR) Buffer Transfer Completed [5:0].
#define    DMAC_EBCISR_BTC2            2 ///< (DMAC_EBCISR) Buffer Transfer Completed [5:0].
#define    DMAC_EBCISR_BTC3            3 ///< (DMAC_EBCISR) Buffer Transfer Completed [5:0].
#define    DMAC_EBCISR_BTC4            4 ///< (DMAC_EBCISR) Buffer Transfer Completed [5:0].
#define    DMAC_EBCISR_BTC5            5 ///< (DMAC_EBCISR) Buffer Transfer Completed [5:0].
#define    DMAC_EBCISR_CBTC0           8 ///< (DMAC_EBCISR) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCISR_CBTC1           9 ///< (DMAC_EBCISR) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCISR_CBTC2          10 ///< (DMAC_EBCISR) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCISR_CBTC3          11 ///< (DMAC_EBCISR) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCISR_CBTC4          12 ///< (DMAC_EBCISR) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCISR_CBTC5          13 ///< (DMAC_EBCISR) Chained Buffer Transfer Completed [5:0].
#define    DMAC_EBCISR_ERR0           16 ///< (DMAC_EBCISR) Access Error [5:0].
#define    DMAC_EBCISR_ERR1           17 ///< (DMAC_EBCISR) Access Error [5:0].
#define    DMAC_EBCISR_ERR2           18 ///< (DMAC_EBCISR) Access Error [5:0].
#define    DMAC_EBCISR_ERR3           19 ///< (DMAC_EBCISR) Access Error [5:0].
#define    DMAC_EBCISR_ERR4           20 ///< (DMAC_EBCISR) Access Error [5:0].
#define    DMAC_EBCISR_ERR5           21 ///< (DMAC_EBCISR) Access Error [5:0].

/* DMAC_CHER : (DMAC Offset: 0x028) DMAC Channel Handler Enable Register*/
#define    DMAC_CHER_ENA0            0 ///< (DMAC_CHER) Enable [5:0].
#define    DMAC_CHER_ENA1            1 ///< (DMAC_CHER) Enable [5:0].
#define    DMAC_CHER_ENA2            2 ///< (DMAC_CHER) Enable [5:0].
#define    DMAC_CHER_ENA3            3 ///< (DMAC_CHER) Enable [5:0].
#define    DMAC_CHER_ENA4            4 ///< (DMAC_CHER) Enable [5:0].
#define    DMAC_CHER_ENA5            5 ///< (DMAC_CHER) Enable [5:0].
#define    DMAC_CHER_SUSP0           8 ///< (DMAC_CHER) Suspend [5:0].
#define    DMAC_CHER_SUSP1           9 ///< (DMAC_CHER) Suspend [5:0].
#define    DMAC_CHER_SUSP2           10 ///< (DMAC_CHER) Suspend [5:0].
#define    DMAC_CHER_SUSP3           11 ///< (DMAC_CHER) Suspend [5:0].
#define    DMAC_CHER_SUSP4           12 ///< (DMAC_CHER) Suspend [5:0].
#define    DMAC_CHER_SUSP5           13 ///< (DMAC_CHER) Suspend [5:0].
#define    DMAC_CHER_KEEP0           24 ///< (DMAC_CHER) Keep on [5:0].
#define    DMAC_CHER_KEEP1           25 ///< (DMAC_CHER) Keep on [5:0].
#define    DMAC_CHER_KEEP2           26 ///< (DMAC_CHER) Keep on [5:0].
#define    DMAC_CHER_KEEP3           27 ///< (DMAC_CHER) Keep on [5:0].
#define    DMAC_CHER_KEEP4           28 ///< (DMAC_CHER) Keep on [5:0].
#define    DMAC_CHER_KEEP5           29 ///< (DMAC_CHER) Keep on [5:0].

/* DMAC_CHDR : (DMAC Offset: 0x02C) DMAC Channel Handler Disable Register*/
#define    DMAC_CHDR_DIS0           0 ///< (DMAC_CHDR) Disable [5:0].
#define    DMAC_CHDR_DIS1           1 ///< (DMAC_CHDR) Disable [5:0].
#define    DMAC_CHDR_DIS2           2 ///< (DMAC_CHDR) Disable [5:0].
#define    DMAC_CHDR_DIS3           3 ///< (DMAC_CHDR) Disable [5:0].
#define    DMAC_CHDR_DIS4           4 ///< (DMAC_CHDR) Disable [5:0].
#define    DMAC_CHDR_DIS5           5 ///< (DMAC_CHDR) Disable [5:0].
#define    DMAC_CHDR_RES0           8 ///< (DMAC_CHDR) Resume [5:0].
#define    DMAC_CHDR_RES1           9 ///< (DMAC_CHDR) Resume [5:0].
#define    DMAC_CHDR_RES2           10 ///< (DMAC_CHDR) Resume [5:0].
#define    DMAC_CHDR_RES3           11 ///< (DMAC_CHDR) Resume [5:0].
#define    DMAC_CHDR_RES4           12 ///< (DMAC_CHDR) Resume [5:0].
#define    DMAC_CHDR_RES5           13 ///< (DMAC_CHDR) Resume [5:0].

/* DMAC_CHSR : (DMAC Offset: 0x030) DMAC Channel Handler Status Register*/
#define    DMAC_CHSR_EN_MASK      0x31 ///<
#define    DMAC_CHSR_ENA0            0 ///< (DMAC_CHSR) Enable [5:0].
#define    DMAC_CHSR_ENA1            1 ///< (DMAC_CHSR) Enable [5:0].
#define    DMAC_CHSR_ENA2            2 ///< (DMAC_CHSR) Enable [5:0].
#define    DMAC_CHSR_ENA3            3 ///< (DMAC_CHSR) Enable [5:0].
#define    DMAC_CHSR_ENA4            4 ///< (DMAC_CHSR) Enable [5:0].
#define    DMAC_CHSR_ENA5            5 ///< (DMAC_CHSR) Enable [5:0].
#define    DMAC_CHSR_SUSP0           8 ///< (DMAC_CHSR) Suspend [5:0].
#define    DMAC_CHSR_SUSP1           9 ///< (DMAC_CHSR) Suspend [5:0].
#define    DMAC_CHSR_SUSP2           10 ///< (DMAC_CHSR) Suspend [5:0].
#define    DMAC_CHSR_SUSP3           11 ///< (DMAC_CHSR) Suspend [5:0].
#define    DMAC_CHSR_SUSP4           12 ///< (DMAC_CHSR) Suspend [5:0].
#define    DMAC_CHSR_SUSP5           13 ///< (DMAC_CHSR) Suspend [5:0].
#define    DMAC_CHSR_EMPT0           16 ///< (DMAC_CHSR) Empty [5:0].
#define    DMAC_CHSR_EMPT1           17 ///< (DMAC_CHSR) Empty [5:0].
#define    DMAC_CHSR_EMPT2           18 ///< (DMAC_CHSR) Empty [5:0].
#define    DMAC_CHSR_EMPT3           19 ///< (DMAC_CHSR) Empty [5:0].
#define    DMAC_CHSR_EMPT4           20 ///< (DMAC_CHSR) Empty [5:0].
#define    DMAC_CHSR_EMPT5           21 ///< (DMAC_CHSR) Empty [5:0].
#define    DMAC_CHSR_STAL0           24 ///< (DMAC_CHSR) Stalled [5:0].
#define    DMAC_CHSR_STAL1           25 ///< (DMAC_CHSR) Stalled [5:0].
#define    DMAC_CHSR_STAL2           26 ///< (DMAC_CHSR) Stalled [5:0].
#define    DMAC_CHSR_STAL3           27 ///< (DMAC_CHSR) Stalled [5:0].
#define    DMAC_CHSR_STAL4           28 ///< (DMAC_CHSR) Stalled [5:0].
#define    DMAC_CHSR_STAL5           29 ///< (DMAC_CHSR) Stalled [5:0].

/* DMAC_CTRLA : (DMAC Offset: N/A) DMAC Channel Control A Register*/
#define    DMAC_CTRLA_BTSIZE_MASK 0xffff  ///< (DMAC_CTRLA) Buffer Transfer Size.
#define    DMAC_CTRLA_SCSIZE_MASK    0x70000  ///< (DMAC_CTRLA) Source Chunk Transfer Size..
#define    DMAC_CTRLA_SCSIZE_CHK_1   0x00000 ///< (DMAC_CTRLA) 1 data transferred.
#define    DMAC_CTRLA_SCSIZE_CHK_4   0x10000 ///< (DMAC_CTRLA) 4 data transferred.
#define    DMAC_CTRLA_SCSIZE_CHK_8   0x20000 ///< (DMAC_CTRLA) 8 data transferred.
#define    DMAC_CTRLA_SCSIZE_CHK_16  0x30000 ///< (DMAC_CTRLA) 16 data transferred.
#define    DMAC_CTRLA_SCSIZE_CHK_32  0x40000 ///< (DMAC_CTRLA) 32 data transferred.
#define    DMAC_CTRLA_SCSIZE_CHK_64  0x50000 ///< (DMAC_CTRLA) 64 data transferred.
#define    DMAC_CTRLA_SCSIZE_CHK_128 0x60000 ///< (DMAC_CTRLA) 128 data transferred.
#define    DMAC_CTRLA_SCSIZE_CHK_256 0x70000 ///< (DMAC_CTRLA) 256 data transferred.
#define    DMAC_CTRLA_DCSIZE_MASK    0x700000 ///< (DMAC_CTRLA) Destination Chunk Transfer Size.
#define    DMAC_CTRLA_DCSIZE_CHK_1   0x000000 ///< (DMAC_CTRLA) 1 data transferred.
#define    DMAC_CTRLA_DCSIZE_CHK_4   0x100000 ///< (DMAC_CTRLA) 4 data transferred.
#define    DMAC_CTRLA_DCSIZE_CHK_8   0x200000 ///< (DMAC_CTRLA) 8 data transferred.
#define    DMAC_CTRLA_DCSIZE_CHK_16  0x300000 ///< (DMAC_CTRLA) 16 data transferred.
#define    DMAC_CTRLA_DCSIZE_CHK_32  0x400000 ///< (DMAC_CTRLA) 32 data transferred.
#define    DMAC_CTRLA_DCSIZE_CHK_64  0x500000 ///< (DMAC_CTRLA) 64 data transferred.
#define    DMAC_CTRLA_DCSIZE_CHK_128 0x600000 ///< (DMAC_CTRLA) 128 data transferred.
#define    DMAC_CTRLA_DCSIZE_CHK_256 0x700000 ///< (DMAC_CTRLA) 256 data transferred.
#define    DMAC_CTRLA_SRC_WIDTH_MASK        0x3000000 ///< (DMAC_CTRLA) Transfer Width for the Source.
#define    DMAC_CTRLA_SRC_WIDTH_BYTE        0x0000000 ///< (DMAC_CTRLA) the transfer size is set to 8-bit width.
#define    DMAC_CTRLA_SRC_WIDTH_HALF_WORD   0x1000000///< (DMAC_CTRLA) the transfer size is set to 16-bit width.
#define    DMAC_CTRLA_SRC_WIDTH_WORD        0x2000000 ///< (DMAC_CTRLA) the transfer size is set to 32-bit width.
#define    DMAC_CTRLA_DST_WIDTH_MASK       0x30000000 ///< (DMAC_CTRLA) Transfer Width for the Destination.
#define    DMAC_CTRLA_DST_WIDTH_BYTE       0x00000000 ///< (DMAC_CTRLA) the transfer size is set to 8-bit width.
#define    DMAC_CTRLA_DST_WIDTH_HALF_WORD  0x10000000///< (DMAC_CTRLA) the transfer size is set to 16-bit width.
#define    DMAC_CTRLA_DST_WIDTH_WORD       0x20000000 ///< (DMAC_CTRLA) the transfer size is set to 32-bit width.
#define    DMAC_CTRLA_DONE                31 ///< (DMAC_CTRLA) .

/* DMAC_CTRLB : (DMAC Offset: N/A) DMAC Channel Control B Register*/
#define    DMAC_CTRLB_SRC_DSCR                          16 ///< (DMAC_CTRLB) Source Address Descriptor.
#define    DMAC_CTRLB_DST_DSCR                          20 ///< (DMAC_CTRLB) Destination Address Descriptor.
#define    DMAC_CTRLB_FC_MASK                     0xE00000 ///< (DMAC_CTRLB) Flow Control.
#define    DMAC_CTRLB_FC_MEM2MEM_DMA_FC                  0 ///< (DMAC_CTRLB) Memory-to-Memory Transfer DMAC is flow controller.
#define    DMAC_CTRLB_FC_MEM2PER_DMA_FC           0x200000 ///< (DMAC_CTRLB) Memory-to-Peripheral Transfer DMAC is flow controller.
#define    DMAC_CTRLB_FC_PER2MEM_DMA_FC           0x400000 ///< (DMAC_CTRLB) Peripheral-to-Memory Transfer DMAC is flow controller.
#define    DMAC_CTRLB_FC_PER2PER_DMA_FC           0x600000 ///< (DMAC_CTRLB) Peripheral-to-Peripheral Transfer DMAC is flow controller.
#define    DMAC_CTRLB_SRC_INCR_MASK              0x3000000 ///< (DMAC_CTRLB) Incrementing, Decrementing or Fixed Address for the Source.
#define    DMAC_CTRLB_SRC_INCR_INCREMENTING              0 ///< (DMAC_CTRLB) The source address is incremented.
#define    DMAC_CTRLB_SRC_INCR_DECREMENTING      0x1000000 ///< (DMAC_CTRLB) The source address is decremented.
#define    DMAC_CTRLB_SRC_INCR_FIXED             0x2000000 ///< (DMAC_CTRLB) The source address remains unchanged.
#define    DMAC_CTRLB_DST_INCR_MASK             0x30000000 ///< (DMAC_CTRLB) Incrementing, Decrementing or Fixed Address for the Destination.
#define    DMAC_CTRLB_DST_INCR_INCREMENTING              0 ///< (DMAC_CTRLB) The destination address is incremented.
#define    DMAC_CTRLB_DST_INCR_DECREMENTING     0x10000000 ///< (DMAC_CTRLB) The destination address is decremented.
#define    DMAC_CTRLB_DST_INCR_FIXED            0x20000000 ///< (DMAC_CTRLB) The destination address remains unchanged.
#define    DMAC_CTRLB_IEN                               30 ///< (DMAC_CTRLB).

/* DMAC_CFG : (DMAC Offset: N/A) DMAC Channel Configuration Register*/
#define    DMAC_CFG_SRC_PER_MASK       0xf ///< (DMAC_CFG) Source with Peripheral identifier.
#define    DMAC_CFG_DST_PER_MASK      0xf0 ///< (DMAC_CFG) Destination with Peripheral identifier.
#define    DMAC_CFG_DST_PER_SHIFT        4 ///< (DMAC_CFG) Destination with Peripheral identifier.
#define    DMAC_CFG_SRC_H2SEL            9 ///< (DMAC_CFG) Software or Hardware Selection for the Source.
#define    DMAC_CFG_DST_H2SEL           13 ///< (DMAC_CFG) Software or Hardware Selection for the Destination.
#define    DMAC_CFG_SOD                 16 ///< (DMAC_CFG) Stop On Done.
#define    DMAC_CFG_LOCK_IF             20 ///< (DMAC_CFG) Interface Lock.
#define    DMAC_CFG_LOCK_B              21 ///< (DMAC_CFG) Bus Lock.
#define    DMAC_CFG_LOCK_IF_L           22 ///< (DMAC_CFG) Master Interface Arbiter Lock.
#define    DMAC_CFG_AHB_PROT_SHIFT      24 ///< (DMAC_CFG) AHB Protection.
#define    DMAC_CFG_AHB_PROT_MASK      0x7000000 ///< (DMAC_CFG) AHB Protection.
#define    DMAC_CFG_FIFOCFG_MASK      0x70000000 ///< (DMAC_CFG) FIFO Configuration.
#define    DMAC_CFG_FIFOCFG_ALAP_CFG  0x00000000 ///< (DMAC_CFG) The largest defined length AHB burst is performed on the destination AHB interface..
#define    DMAC_CFG_FIFOCFG_HALF_CFG  0x10000000 ///< (DMAC_CFG) When half FIFO size is available/filled, a source/destination request is serviced..
#define    DMAC_CFG_FIFOCFG_ASAP_CFG  0x20000000 ///< (DMAC_CFG) When there is enough space/data available to perform a single AHB access, then the request is serviced..
/* DMAC_WPMR : (DMAC Offset: 0x1E4) DMAC Write Protect Mode Register*/
#define    DMAC_WPMR_WPEN                   0 ///< (DMAC_WPMR) Write Protect Enable.
#define    DMAC_WPMR_WPKEY_MASK    0xFFFFFF00 ///< (DMAC_WPMR) Write Protect KEY.

/* DMAC_WPSR : (DMAC Offset: 0x1E8) DMAC Write Protect Status Register*/
#define    DMAC_WPSR_WPVS                   0 ///< (DMAC_WPSR) Write Protect Violation Status.
#define    DMAC_WPSR_WPVSRC_MASK   0x00FFFF00 ///< (DMAC_WPSR) Write Protect Violation Source.


#endif /* SAM3_DMAC_H */

