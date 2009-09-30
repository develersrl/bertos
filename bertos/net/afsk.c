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
 */

#include "afsk.h"
#include "cfg/cfg_afsk.h"
#include "hw/hw_afsk.h"

#include <drv/timer.h>

#include <cfg/module.h>

#include <cpu/power.h>
#include <struct/fifobuf.h>

#include <string.h> /* memset */

// Demodulator constants
#define SAMPLERATE 9600
#define BITRATE    1200

#define SAMPLEPERBIT (SAMPLERATE / BITRATE)
#define PHASE_BIT    8
#define PHASE_INC    1

#define PHASE_MAX    (SAMPLEPERBIT * PHASE_BIT)
#define PHASE_THRES  (PHASE_MAX / 2) // - PHASE_BIT / 2)

// Modulator constants
#define MARK_FREQ  1200
#define MARK_INC   (uint16_t)(DIV_ROUND(SIN_LEN * (uint32_t)MARK_FREQ, CONFIG_AFSK_DAC_SAMPLERATE))

#define SPACE_FREQ 2200
#define SPACE_INC  (uint16_t)(DIV_ROUND(SIN_LEN * (uint32_t)SPACE_FREQ, CONFIG_AFSK_DAC_SAMPLERATE))

//Ensure sample rate is a multiple of bit rate
STATIC_ASSERT(!(CONFIG_AFSK_DAC_SAMPLERATE % BITRATE));

#define DAC_SAMPLEPERBIT (CONFIG_AFSK_DAC_SAMPLERATE / BITRATE)


/** Current sample of bit for output data. */
static uint8_t sample_count;

/** Current character to be modulated */
static uint8_t curr_out;

/** Mask of current modulated bit */
static uint8_t tx_bit;

/** True if bit stuff is allowed, false otherwise */
static bool bit_stuff;

/** Counter for bit stuffing */
static uint8_t stuff_cnt;

/**
 * Sine table for the first quarter of wave.
 * The rest of the wave is computed from this first quarter.
 * This table is used to generate the modulated data.
 */
static const uint8_t sin_table[] =
{
	//TODO put in flash!
	128, 129, 131, 132, 134, 135, 137, 138, 140, 142, 143, 145, 146, 148, 149, 151,
	152, 154, 155, 157, 158, 160, 162, 163, 165, 166, 167, 169, 170, 172, 173, 175,
	176, 178, 179, 181, 182, 183, 185, 186, 188, 189, 190, 192, 193, 194, 196, 197,
	198, 200, 201, 202, 203, 205, 206, 207, 208, 210, 211, 212, 213, 214, 215, 217,
	218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233,
	234, 234, 235, 236, 237, 238, 238, 239, 240, 241, 241, 242, 243, 243, 244, 245,
	245, 246, 246, 247, 248, 248, 249, 249, 250, 250, 250, 251, 251, 252, 252, 252,
	253, 253, 253, 253, 254, 254, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255,
};

#define SIN_LEN 512 ///< Full wave length

STATIC_ASSERT(sizeof(sin_table) == SIN_LEN / 4);

/**
 * DDS phase accumulator for generating modulated data.
 */
static uint16_t phase_acc;

/** Current phase increment for current modulated bit */
static uint16_t phase_inc = MARK_INC;


/**
 * Given the index, this function computes the correct sine sample
 * based only on the first quarter of wave.
 */
INLINE uint8_t sin_sample(uint16_t idx)
{
	ASSERT(idx < SIN_LEN);
	uint16_t new_idx = idx % (SIN_LEN / 2);
	new_idx = (new_idx >= (SIN_LEN / 4)) ? (SIN_LEN / 2 - new_idx - 1) : new_idx;
	return (idx >= (SIN_LEN / 2)) ? (255 - sin_table[new_idx]) : sin_table[new_idx];
}


static FIFOBuffer delay_fifo;

/**
 * Buffer for delay FIFO.
 * The 1 is added because the FIFO macros need
 * 1 byte more to handle a buffer (SAMPLEPERBIT / 2) bytes long.
 */
static int8_t delay_buf[SAMPLEPERBIT / 2 + 1];

static FIFOBuffer rx_fifo;
static uint8_t rx_buf[CONFIG_AFSK_RX_BUFLEN];

static FIFOBuffer tx_fifo;
static uint8_t tx_buf[CONFIG_AFSK_TX_BUFLEN];

static int16_t iir_x[2];
static int16_t iir_y[2];
static uint8_t sampled_bits;
static uint8_t found_bits;
static uint8_t demod_bits;
static int8_t curr_phase;

static bool hdlc_rxstart;
static uint8_t hdlc_currchar;
static uint8_t hdlc_bit_idx;

#define BIT_DIFFER(bitline1, bitline2) (((bitline1) ^ (bitline2)) & 0x01)
#define EDGE_FOUND(bitline)            BIT_DIFFER((bitline), (bitline) >> 1)

static void hdlc_parse(bool bit)
{
	demod_bits <<= 1;
	demod_bits |= bit ? 1 : 0;

	/* HDLC Flag */
	if (demod_bits == HDLC_FLAG)
	{
		if (!fifo_isfull_locked(&rx_fifo))
		{
			fifo_push(&rx_fifo, HDLC_FLAG);
			hdlc_rxstart = true;
		}
		else
			hdlc_rxstart = false;
		hdlc_currchar = 0;
		hdlc_bit_idx = 0;
		return;
	}

	/* Reset */
	if ((demod_bits & HDLC_RESET) == HDLC_RESET)
	{
		hdlc_rxstart = false;
		return;
	}

	if (!hdlc_rxstart)
		return;

	/* Stuffed bit */
	if ((demod_bits & 0x3f) == 0x3e)
		return;

	if (demod_bits & 0x01)
		hdlc_currchar |= 0x80;

	if (++hdlc_bit_idx >= 8)
	{
		if ((hdlc_currchar == HDLC_FLAG
			|| hdlc_currchar == HDLC_RESET
			|| hdlc_currchar == AFSK_ESC)
			&& !fifo_isfull_locked(&rx_fifo))
			fifo_push(&rx_fifo, AFSK_ESC);
		else
			hdlc_rxstart = false;

		if (!fifo_isfull_locked(&rx_fifo))
			fifo_push(&rx_fifo, hdlc_currchar);
		else
			hdlc_rxstart = false;

		hdlc_currchar = 0;
		hdlc_bit_idx = 0;
		return;
	}

	hdlc_currchar >>= 1;
}

DEFINE_AFSK_ADC_ISR()
{
	AFSK_STROBE_ON();
	int8_t curr_sample = AFSK_READ_ADC();

	/*
	 * Frequency discriminator and LP IIR filter.
	 * This filter is designed to work
	 * at the given sample rate and bit rate.
	 */
	STATIC_ASSERT(SAMPLERATE == 9600);
	STATIC_ASSERT(BITRATE == 1200);

	/*
	 * Frequency discrimination is achieved by simply multiplying
	 * the sample with a delayed sample of (bits per sample) / 2.
	 * Then the signal is lowpass filtered with a first order, 600 Hz
	 * Butterworth filter.
	 */

	iir_x[0] = iir_x[1];
	iir_x[1] = ((int8_t)fifo_pop(&delay_fifo) * curr_sample) >> 2;
	iir_y[0] = iir_y[1];

	/*
	 * This strange sum + shift is an optimization for iir_y[0] * 0.668.
	 * iir * 0.668 ~= (iir * 21) / 32 =
	 * = (iir * 16) / 32 + (iir * 4) / 32 + iir / 32 =
	 * = iir / 2 + iir / 8 + iir / 32 =
	 * = iir >> 1 + iir >> 3 + iir >> 5
	 */
	iir_y[1] = iir_x[0] + iir_x[1] + (iir_y[0] >> 1) + (iir_y[0] >> 3) + (iir_y[0] >> 5);

	/* Save this sampled bit in a delay line */
	sampled_bits <<= 1;
	sampled_bits |= (iir_y[1] > 0) ? 1 : 0;

	/* Store current ADC sample in the delay_fifo */
	fifo_push(&delay_fifo, curr_sample);

	/* If there is an edge, adjust phase sampling */
	if (EDGE_FOUND(sampled_bits))
	{
		if (curr_phase < PHASE_THRES)
			curr_phase += PHASE_INC;
		else
			curr_phase -= PHASE_INC;
	}
	curr_phase += PHASE_BIT;

	/* sample the bit */
	if (curr_phase >= PHASE_MAX)
	{
		curr_phase %= PHASE_MAX;

		/* Shift 1 position in the shift register of the found bits */
		found_bits <<= 1;

		/*
		 * TODO: maybe a better algorithm to find the sample bit
		 * other than reading the last one.
		 */
		found_bits |= sampled_bits & 1;

		/*
		 * NRZI coding: if 2 consecutive bits have the same value
		 * a 1 is received, otherwise it's a 0.
		 */
		hdlc_parse(!EDGE_FOUND(found_bits));
	}

	AFSK_STROBE_OFF();
}

/* True while modem sends data */
static volatile bool sending;

static void afsk_txStart(void)
{
	if (!sending)
	{
		phase_inc = MARK_INC;
		phase_acc = 0;
		stuff_cnt = 0;
		sending = true;
		AFSK_DAC_IRQ_START();
	}
}

#define BIT_STUFF_LEN 5

#define SWITCH_TONE(inc)  (((inc) == MARK_INC) ? SPACE_INC : MARK_INC)

DEFINE_AFSK_DAC_ISR()
{
	/* Check if we are at a start of a sample cycle */
	if (sample_count == 0)
	{
		if (tx_bit == 0)
		{
			/* We have just finished transimitting a char, get a new one. */
			if (fifo_isempty(&tx_fifo))
			{
				AFSK_DAC_IRQ_STOP();
				sending = false;
				return;
			}
			else
			{
				/*
				 * If we have just finished sending an unstuffed byte,
				 * reset bitstuff counter.
				 */
				if (!bit_stuff)
					stuff_cnt = 0;

				bit_stuff = true;
				curr_out = fifo_pop(&tx_fifo);

				/* Handle char escape */
				if (curr_out == AFSK_ESC)
				{
					if (fifo_isempty(&tx_fifo))
					{
						AFSK_DAC_IRQ_STOP();
						sending = false;
						return;
					}
					else
						curr_out = fifo_pop(&tx_fifo);
				}
				else if (curr_out == HDLC_FLAG || curr_out == HDLC_RESET)
					/* If these chars are not escaped disable bit stuffing */
					bit_stuff = false;
			}
			/* Start with LSB mask */
			tx_bit = 0x01;
		}

		/* check for bit stuffing */
		if (bit_stuff && stuff_cnt >= BIT_STUFF_LEN)
		{
			/* If there are more than 5 ones in a row insert a 0 */
			stuff_cnt = 0;
			/* switch tone */
			phase_inc = SWITCH_TONE(phase_inc);
		}
		else
		{
			/*
			 * NRZI: if we want to transmit a 1 the modulated frequency will stay
			 * unchanged; with a 0, there will be a change in the tone.
			 */
			if (curr_out & tx_bit)
			{
				/*
				 * Transmit a 1:
				 * - Stay on the previous tone
				 * - Increace bit stuff count
				 */
				stuff_cnt++;
			}
			else
			{
				/*
				 * Transmit a 0:
				 * - Reset bit stuff count
				 * - Switch tone
				 */
				stuff_cnt = 0;
				phase_inc = SWITCH_TONE(phase_inc);
			}

			/* Go to the next bit */
			tx_bit <<= 1;
		}
		sample_count = DAC_SAMPLEPERBIT;
	}

	/* Get new sample and put it out on the DAC */
	phase_acc += phase_inc;
	phase_acc %= SIN_LEN;

	AFSK_SET_DAC(sin_sample(phase_acc));
	sample_count--;
}


static size_t afsk_read(UNUSED_ARG(KFile *, fd), void *_buf, size_t size)
{
	uint8_t *buf = (uint8_t *)_buf;

	#if CONFIG_AFSK_RXTIMEOUT == 0
	while (size-- && !fifo_isempty_locked(&rx_fifo))
	#else
	while (size--)
	#endif
	{
		#if CONFIG_AFSK_RXTIMEOUT != -1
		ticks_t start = timer_clock();
		#endif

		do
		{
			cpu_relax();
			#if CONFIG_AFSK_RXTIMEOUT != -1
			if (timer_clock() - start > ms_to_ticks(CONFIG_AFSK_RXTIMEOUT))
				return buf - (uint8_t *)_buf;
			#endif
		}
		while (fifo_isempty_locked(&rx_fifo));

		*buf++ = fifo_pop_locked(&rx_fifo);
	}

	return buf - (uint8_t *)_buf;
}

static size_t afsk_write(UNUSED_ARG(KFile *, fd), const void *_buf, size_t size)
{
	const uint8_t *buf = (const uint8_t *)_buf;

	while (size--)
	{
		while (fifo_isfull_locked(&tx_fifo))
			cpu_relax();

		fifo_push_locked(&tx_fifo, *buf++);
		afsk_txStart();
	}

	return buf - (const uint8_t *)_buf;
}

static int afsk_flush(UNUSED_ARG(KFile *, fd))
{
	while (sending)
		cpu_relax();
	return 0;
}


void afsk_init(Afsk *af)
{
	#if CONFIG_AFSK_RXTIMEOUT != -1
	MOD_CHECK(timer);
	#endif

	fifo_init(&delay_fifo, (uint8_t *)delay_buf, sizeof(delay_buf));
	fifo_init(&rx_fifo, rx_buf, sizeof(rx_buf));

	/* Fill sample FIFO with 0 */
	for (int i = 0; i < SAMPLEPERBIT / 2; i++)
		fifo_push(&delay_fifo, 0);

	fifo_init(&tx_fifo, tx_buf, sizeof(tx_buf));

	AFSK_ADC_INIT();
	kprintf("MARK_INC %d, SPACE_INC %d\n", MARK_INC, SPACE_INC);

	memset(af, 0, sizeof(*af));
	DB(af->fd._type = KFT_AFSK);
	af->fd.write = afsk_write;
	af->fd.read = afsk_read;
	af->fd.flush = afsk_flush;
}
