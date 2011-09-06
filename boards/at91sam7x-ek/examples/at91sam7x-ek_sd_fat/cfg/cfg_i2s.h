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
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Configuration file for I2S module.
 *
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef CFG_I2S_H
#define CFG_I2S_H

/**
 * Module logging level.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_level"
 */
#define I2S_LOG_LEVEL      LOG_LVL_INFO

/**
 * Module logging format.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_format"
 */
#define I2S_LOG_FORMAT     LOG_FMT_TERSE

/**
 * Length of each play buffer.
 *
 * $WIZ$ type = "int"
 */
#define CONFIG_PLAY_BUF_LEN    8192

/**
 * Sampling frequency of the audio file.
 *
 * $WIZ$ type = "int"
 * $WIZ$ min =  32000
 * $WIZ$ max = 192000
 */
#define CONFIG_SAMPLE_FREQ     44100UL

/**
 * Sample bits per channel.
 *
 * $WIZ$ type = "int"
 * * $WIZ$ min = 8
 * $WIZ$ max = 32
 */
#define CONFIG_WORD_BIT_SIZE               16

/**
 * Number of channel.
 *
 * $WIZ$ type = "int"
 */
#define CONFIG_CHANNEL_NUM                  2

/**
 * Size of trasmit start delay
 *
 * $WIZ$ type = "int"
 * $WIZ$ min = 0
 * $WIZ$ max = 255
 */
#define CONFIG_DELAY                        0

/**
 * Generate frame sync every 2 x CONFIG_PERIOD bits (zero based)
 *
 * $WIZ$ type = "int"
 * $WIZ$ min = 0
 * $WIZ$ max = 512
 */
#define CONFIG_PERIOD                      15

/**
 * Number of words transmitted in frame
 *
 * $WIZ$ type = "int"
 * $WIZ$ min = 0
 * $WIZ$ max = 16
 */
#define CONFIG_WORD_PER_FRAME               1

/**
 * Size of Synchro data register (zero based)
 *
 * $WIZ$ type = "int"
 * $WIZ$ min = 0
 * $WIZ$ max = 15
 */
#define CONFIG_FRAME_SYNC_SIZE             15


/**
 * Extra Size of Synchro data register (CONFIG_FRAME_SYNC_SIZE + CONFIG_EXTRA_FRAME_SYNC_SIZE * 16 + 1) (zero based)
 *
 * $WIZ$ type = "int"
 * $WIZ$ min = 0
 * $WIZ$ max = 15
 */
#define CONFIG_EXTRA_FRAME_SYNC_SIZE       0

#endif /* CFG_I2S_H */
