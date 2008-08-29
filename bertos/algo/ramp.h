
/**
 * \file
 * <!--
 * Copyright 2004, 2008 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Compute, save and load ramps for stepper motors (interace)
 *
 * \version $Id$
 *
 * \author Simone Zinanni <s.zinanni@develer.com>
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 *
 * The acceleration ramp is used to properly accelerate a stepper motor. The main
 * entry point is the function ramp_evaluate(), which must be called at every step
 * of the motor: it gets as input the time elapsed since the stepper started
 * accelerating, and returns the time to wait before sending the next step. A pseudo
 * usage pattern is as follows:
 *
 * <pre>
 *  float time = 0;
 *  while (1)
 *  {
 *      float delta = ramp_evaluate(&my_ramp, time);
 *      sleep(delta);
 *      do_motor_step();
 *      time += delta;
 *  }
 * </pre>
 *
 * A similar pattern can be used to decelerate (it is sufficient to move the total
 * time backward, such as "time -= delta").
 *
 * The ramp can be configured with ramp_setup(), providing it with the minimum and
 * maximum operating frequency of the motor, and the total acceleration time in
 * milliseconds (that is, the time that will be needed to accelerate from the
 * minimum frequency to the maximum frequency).
 *
 * Both a very precise floating point and a very fast fixed point implementation
 * of the ramp evaluation are provided. The fixed point is hand-optimized assembly
 * for DSP56000 (but a portable C version of it can be easily written, see the
 * comments in the code).
 *
 */

#ifndef ALGO_RAMP_H
#define ALGO_RAMP_H

#include "hw/hw_stepper.h"

#include <cfg/compiler.h>

/**
 * Define whether the ramp will use floating point calculation within ramp_evaluate().
 * Otherwise, a less precise fixed point version will be used, which is faster on
 * platforms which do no support floating point operations.
 *
 * \note Floating point operations will be always done within ramp_compute() to
 * precalculate values, so there has to be at least a floating point emulation support.
 */
#define RAMP_USE_FLOATING_POINT   0


#if !RAMP_USE_FLOATING_POINT

	/**
	 * Number of least-significant bits which are stripped away during ramp evaluation.
	 * This setting allows to specify larger ramps at the price of less precision.
	 *
	 * The maximum ramp size allowed is 2^(24 + RAMP_CLOCK_SHIFT_PRECISION), in clocks.
	 * For instance, using RAMP_CLOCK_SHIFT_PRECISION 1, and a 8x prescaler, the maximum
	 * length of a ramp is about 6.7 secs. Raising RAMP_CLOCK_SHIFT_PRECISION to 2
	 * brings the maximum length to 13.4 secs, at the price of less precision.
	 *
	 * ramp_compute() will check that the length is below the maximum allowed through
	 * a runtime assertion.
	 *
	 * \note This macro is used only for the fixed-point version of the ramp.
	 */
	#define RAMP_CLOCK_SHIFT_PRECISION 2
#endif


///< Negative pulse width for ramp
#define RAMP_PULSE_WIDTH    50

///< Default ramp
#define RAMP_DEF_TIME       6000000 ///< microsecs
#define RAMP_DEF_MAXFREQ       5000 ///< Hz
#define RAMP_DEF_MINFREQ        200 ///< Hz
#define RAMP_DEF_POWERRUN        10 ///< 10 deciampere (1 ampere)
#define RAMP_DEF_POWERIDLE        1 ///< 1 deciampere


/**
 * Convert microseconds to timer clock ticks
 */
#define TIME2CLOCKS(micros) ((uint32_t)(micros) * (STEPPER_CLOCK / 1000000))

/**
 * Convert timer clock ticks back to microseconds
 */
#define CLOCKS2TIME(clocks) ((uint32_t)(clocks) / (STEPPER_CLOCK / 1000000))

/**
 * Convert microseconds to Hz
 */
#define MICROS2FREQ(micros) (1000000UL / ((uint32_t)(micros)))

/**
 * Convert frequency (in Hz) to time (in microseconds)
 */
#define FREQ2MICROS(hz) (1000000UL / ((uint32_t)(hz)))

/**
 * Multiply \p a and \p b two integer at 32 bit and extract the high 16 bit word.
 */
#define FIX_MULT32(a,b)  (((uint64_t)(a)*(uint32_t)(b)) >> 16)

/**
 * Structure holding pre-calculated data for speeding up real-time evaluation
 * of the ramp. This structure is totally different between the fixed and the
 * floating point version of the code.
 *
 * Consult the file-level documentation of ramp.c for more information about
 * the values of this structure.
 */
struct RampPrecalc
{
#if RAMP_USE_FLOATING_POINT
	float beta;
	float alpha;
	float gamma;
#else
	uint16_t max_div_min;
	uint32_t inv_total_time;
#endif
};


/**
 * Ramp structure
 */
struct Ramp
{
	uint32_t clocksRamp;
	uint16_t clocksMinWL;
	uint16_t clocksMaxWL;

	struct RampPrecalc precalc; ///< pre-calculated values for speed
};


/*
 * Function prototypes
 */
void ramp_compute(
	struct Ramp * ramp,
	uint32_t clocksInRamp,
	uint16_t clocksInMinWavelength,
	uint16_t clocksInMaxWavelength);


/** Setup an acceleration ramp for a stepper motor
 *
 *  \param ramp Ramp to fill
 *  \param length Length of the ramp (milliseconds)
 *  \param minFreq Minimum operating frequency of the motor (hertz)
 *  \param maxFreq Maximum operating frequency of the motor (hertz)
 *
 */
void ramp_setup(struct Ramp* ramp, uint32_t length, uint32_t minFreq, uint32_t maxFreq);


/**
 * Initialize a new ramp with default values
 */
void ramp_default(struct Ramp *ramp);


/**
 * Evaluate the ramp at the given point. Given a \a ramp, and the current \a clock since
 * the start of the acceleration, compute the next step, that is the interval at which
 * send the signal to the motor.
 *
 * \note The fixed point version does not work when curClock is zero. Anyway,
 * the first step is always clocksMaxWL, as stored within the ramp structure.
 */
#if RAMP_USE_FLOATING_POINT
	float ramp_evaluate(const struct Ramp* ramp, float curClock);
#else
	uint16_t ramp_evaluate(const struct Ramp* ramp, uint32_t curClock);
#endif


/** Self test */
int ramp_testSetup(void);
int ramp_testRun(void);
int ramp_testTearDown(void);

#endif /* ALGO_RAMP_H */

