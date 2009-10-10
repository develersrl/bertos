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
 *
 * -->
 *
 * \brief AFSK1200 modem.
 *
 * \version $Id$
 * \author Francesco Sacchi <asterix@develer.com>
 *
 * $WIZ$ module_name = "afsk"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_afsk.h"
 * $WIZ$ module_depends = "timer", "kfile"
 * $WIZ$ module_hw = "bertos/hw/hw_afsk.h"
 */

#ifndef DRV_AFSK_H
#define DRV_AFSK_H

#include "cfg/cfg_afsk.h"
#include "hw/hw_afsk.h"

#include <kern/kfile.h>
#include <cfg/compiler.h>
#include <struct/fifobuf.h>


// Demodulator constants
#define SAMPLERATE 9600
#define BITRATE    1200

#define SAMPLEPERBIT (SAMPLERATE / BITRATE)


typedef struct Afsk
{
	KFile fd;

	int adc_ch;
	int dac_ch;

	/** Current sample of bit for output data. */
	uint8_t sample_count;

	/** Current character to be modulated */
	uint8_t curr_out;

	/** Mask of current modulated bit */
	uint8_t tx_bit;

	/** True if bit stuff is allowed, false otherwise */
	bool bit_stuff;

	/** Counter for bit stuffing */
	uint8_t stuff_cnt;
	/**
	 * DDS phase accumulator for generating modulated data.
	 */
	uint16_t phase_acc;

	/** Current phase increment for current modulated bit */
	uint16_t phase_inc;

	/** Delay line used to delay samples by (SAMPLEPERBIT / 2) */
	FIFOBuffer delay_fifo;

	/**
	 * Buffer for delay FIFO.
	 * The 1 is added because the FIFO macros need
	 * 1 byte more to handle a buffer (SAMPLEPERBIT / 2) bytes long.
	 */
	int8_t delay_buf[SAMPLEPERBIT / 2 + 1];

	FIFOBuffer rx_fifo;
	uint8_t rx_buf[CONFIG_AFSK_RX_BUFLEN];

	FIFOBuffer tx_fifo;
	uint8_t tx_buf[CONFIG_AFSK_TX_BUFLEN];

	int16_t iir_x[2];
	int16_t iir_y[2];

	uint8_t sampled_bits;
	uint8_t found_bits;
	int8_t curr_phase;

	/* True while modem sends data */
	volatile bool sending;


	bool hdlc_rxstart;
	uint8_t hdlc_currchar;
	uint8_t hdlc_bit_idx;
	uint8_t hdlc_demod_bits;

	uint16_t preamble_len;
	uint16_t trailer_len;
} Afsk;

#define KFT_AFSK MAKE_ID('A', 'F', 'S', 'K')

INLINE Afsk *AFSK_CAST(KFile *fd)
{
  ASSERT(fd->_type == KFT_AFSK);
  return (Afsk *)fd;
}

void afsk_adc_isr(Afsk *af, int8_t curr_sample);
void afsk_dac_isr(Afsk *af);
void afsk_init(Afsk *af, int adc_ch, int dac_ch);


/**
 * \name afsk filter type
 * $WIZ$ afsk_filter_list = "AFSK_BUTTERWORTH", "AFSK_CHEBYSHEV"
 * \{
 */
#define AFSK_BUTTERWORTH  0
#define AFSK_CHEBYSHEV    1
/* \} */

int afsk_testSetup(void);
int afsk_testRun(void);
int afsk_testTearDown(void);

#endif
