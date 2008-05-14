/**
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Dataflash HW control routines.
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

#include "hw_dataflash.h"

#include <cfg/compiler.h>
#include <cfg/module.h>
#include <cfg/macros.h>

#include <io/arm.h>


MOD_DEFINE(hw_dataflash);

/**
 * Data flash init function.
 *
 * This function provide to initialize all that
 * needs to drive a dataflash memory.
 * Generaly needs to init pins to drive a CS line
 * and reset line.
 */
void dataflash_hw_init(void)
{
	#warning The data flash init pins function is not implemented!

    //Disable CS line (remove if not needed)
	dataflash_hw_setCS(false);

	/*
	 * Put here your code!
	 *
	 * Note:
	 * - if you drive manualy CS line, here init a CS pin
	 * - if you use a dedicated reset line, here init a reset pin
	 */

	MOD_INIT(hw_dataflash);
}

/**
 * Chip Select drive.
 *
 * This function enable or disable a CS line.
 * You must implement this function comply to a dataflash
 * memory datasheet to allow the drive to enable a memory
 * when \p enable flag is true, and disable it when is false.
 */
void dataflash_hw_setCS(bool enable)
{
	#warning The data flash setCS function is not implemented!
	if (enable)
	{
		/*
		 * Put here your code to enable
		 * dataflash memory
		 */
	}
	else
	{
		/*
		 * Put here your code to disable
		 * dataflash memory
		 */
	}
}

/**
 * Reset data flash memory.
 *
 * This function provide to send reset signal to
 * dataflash memory. You must impement it comly to a dataflash
 * memory datasheet to allow the drive to set a reset pin
 * when \p enable flag is true, and disable it when is false.
 *
 */
void dataflash_hw_setReset(bool enable)
{
	#warning The data flash setReset function is not implemented!
	if (enable)
	{
		/*
		 * Put here your code to set reset of
		 * dataflash memory
		 */
	}
	else
	{
		/*
		 * Put here your code to clear reset of
		 * dataflash memory
		 */
	}
}

