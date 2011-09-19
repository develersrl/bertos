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
 * \brief WM8731 Audio codec 2 wire driver.
 *
 * $WIZ$ module_name = "wm8731"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_wm8731.h"
 * $WIZ$ module_hw = "bertos/hw/hw_wm8731.h"
 * $WIZ$ module_depends = "i2c"
 *
 */

#ifndef DRV_WM8731_H
#define DRV_WM8731_H

#include <cpu/types.h>

#include <drv/i2c.h>

/* Reset register*/
#define WM8731_REG_RESET                           0x0F

/* Left Line in register */
#define WM8731_REG_LEFT_LINEIN                     0x0
#define WM8731_LINVOL_BITS_MASK                   0x1F // Left line input volume control
#define WM8731_LINMUTE_BIT                           7 // Left line input mute to ADC
#define WM8731_LRINBOTH_BIT                          8 // Left to right channel line input volume and mute data load control

/* Right Line in register */
#define WM8731_REG_RIGHT_LINEIN                    0x1
#define WM8731_RINVOL_BITS_MASK                   0x1F // Right line input volume control
#define WM8731_RINMUTE_BIT                           7 // Right line input mute to ADC
#define WM8731_RLINBOTH_BIT                          8 // Right to right channel line input volume and mute data load control

/* Left Headphone out register*/
#define WM8731_REG_LEFT_HPOUT                      0x2
#define WM8731_LHPVOL_BITS_MASK                   0x7F // Left channel headphone output volume control
#define WM8731_LZCEN_BIT                             7 // Left channel zero cross detect enable
#define WM8731_LRHPBOTH_BIT                          8 // Left to right channel headphone volume, mute and zero cross data load control

/* Right Headphone out register*/
#define WM8731_REG_RIGHT_HPOUT                      0x3
#define WM8731_RHPVOL_BITS_MASK                    0x7F // Right channel headphone output volume control
#define WM8731_RZCEN_BIT                              7 // Right channel zero cross detect enable
#define WM8731_RLHPBOTH_BIT                           8 // Right to right channel headphone volume, mute and zero cross data load control

/* Analogue audio path control register*/
#define WM8731_REG_ANALOGUE_PATH_CTRL              0x4
#define WM8731_MICBOOST                              0 // Microphone Input Level Boost
#define WM8731_MUTEMIC                               1 // Line input Mute to ADC
/**
 * Input selector
 * $WIZ$ wm8731_insel = "WM8731_INSEL_MIC", "WM8731_INSEL_LINEIN"
 */
#define WM8731_INSEL                                 2 // Microphone/Line Select to ADC
#define WM8731_INSEL_MIC              BV(WM8731_INSEL) // Microphone Select to ADC
#define WM8731_INSEL_LINEIN          ~BV(WM8731_INSEL) // Line in Select to ADC
/**
 * Input to output
 * $WIZ$ wm8731_bypass = "WM8731_BYPASS", "WM8731_DACSEL", "WM8731_SIDETONE"
 */
#define WM8731_BYPASS                            BV(3) // Bypass switch
#define WM8731_DACSEL                            BV(4) // DAC select
#define WM8731_SIDETONE                          BV(5) // Side tone switch
/**
 * Side tone attenuation
 * $WIZ$ wm8731_sideatt = "WM8731_SIDEATT_15dB", "WM8731_SIDEATT_12dB", "WM8731_SIDEATT_9dB", "WM8731_SIDEATT_6dB"
 */
#define WM8731_SIDEATT_MASK                       0xC0 // Side tone attenuation
#define WM8731_SIDEATT_15dB                       0xC0 // -15dB
#define WM8731_SIDEATT_12dB                       0x80 // -12dB
#define WM8731_SIDEATT_9dB                        0x40 // -9dB
#define WM8731_SIDEATT_6dB                        0x00 // -6dB


/* Digital audio path control register*/
#define WM8731_REG_DIGITAL_PATH_CTRL               0x5
#define WM8731_ADCHPD                                0 // ADC high pass filter enable
/**
 * De-emphasis control
 * $WIZ$ wm8731_deemp = "WM8731_DEEMP_48kHz", "WM8731_DEEMP_44k1Hz", "WM8731_DEEMP_32kHz", "WM8731_DEEMP_DISABLE"
 */
#define WM8731_DEEMP_MASK                          0x6 // De-emphasis control
#define WM8731_DEEMP_48kHz                         0x6 // 48kHz
#define WM8731_DEEMP_44k1Hz                        0x4 // 44.1kHz
#define WM8731_DEEMP_32kHz                         0x2 // 32kHz
#define WM8731_DEEMP_DISABLE                       0x0 // Disable
#define WM8731_DACMU                                 3 // DAC Soft Mute control

/* Power down control register*/
#define WM8731_REG_PWDOWN_CTRL                     0x6
#define WM8731_LINEINPD                           BV(0) ///< LineIn power down
#define WM8731_MICPD                              BV(1) ///< Mic power down
#define WM8731_ADCPD                              BV(2) ///< ADC power down
#define WM8731_DACPD                              BV(3) ///< DAC power down
#define WM8731_OUTPD                              BV(4) ///< OUT power down
#define WM8731_OSCBIT                             BV(5) ///< OSC power down
#define WM8731_CLKOUTPD                           BV(6) ///< CLKOUT powerdown
#define WM8731_POWEROFF_BIT                          7 // Power off device

/* Interface format register*/
#define WM8731_REG_DA_INTERFACE_FORMAT             0x7
/**
 * Audio format
 * $WIZ$ wm8731_fmt = "WM8731_FORMAT_MSB_LEFT_JUSTIFIED", "WM8731_FORMAT_MSB_RIGHT_JUSTIFIED", "WM8731_FORMAT_I2S", "WM8731_FORMAT_DSP"
 */
#define WM8731_FORMAT_BITS_MASK                    0x3 // Format
#define WM8731_FORMAT_MSB_LEFT_JUSTIFIED           0x0 // MSB-First, left justified
#define WM8731_FORMAT_MSB_RIGHT_JUSTIFIED          0x1 // MSB-First, right justified
#define WM8731_FORMAT_I2S                          0x2 //I2S Format, MSB-First left-1 justified
#define WM8731_FORMAT_DSP                          0x3 //DSP Mode, frame sync + 2 data packed words
/**
 * Audio format
 * $WIZ$ wm8731_databit = "WM8731_IWL_16_BIT", "WM8731_IWL_20_BIT", "WM8731_IWL_24_BIT", "WM8731_IWL_32_BIT"
 */
#define WM8731_IWL_BITS                            0xC // Input audio data bit length select
#define WM8731_IWL_16_BIT                          0x0 // 16 bit
#define WM8731_IWL_20_BIT                          0x4 // 20 bit
#define WM8731_IWL_24_BIT                          0x8 // 24 bit
#define WM8731_IWL_32_BIT                          0xC // 32 bit
#define WM8731_IRP_BITS                              4 //  DACLRC phase control
#define WM8731_IRSWAP_BIT                            5 //  DAC Left right clock swap
#define WM8731_MS_BIT                                6 //  Master slave mode control
#define WM8731_BCLKINV_BIT                           7 // Bit clock invert

/* Sampling control*/
#define WM8731_REG_SAMPLECTRL                      0x8
#define WM8731_USBNORMAL_BIT                         0 // Mode select, usb mode, normal mode
#define WM8731_BOSR_BIT                              1 // Base over-sampling rate
#define WM8731_SR_BITS_MASK                       0x3C // Sample rate control
#define WM8731_CLKIDIV2_BIT                          6 // Core clock divider select
#define WM8731_CLKODIV2_BIT                          7 // CLKOUT divider select
/**
 * Sampling rate control
 * $WIZ$ wm8731_sample_rate = "WM8731_NORMAL_256FS_48KHZ", "WM8731_NORMAL_256FS_44_1KHZ", "WM8731_NORMAL_384FS_44_1KHZ"
 */
#define WM8731_NORMAL_256FS_48KHZ      0
#define WM8731_NORMAL_256FS_44_1KHZ    0x20
#define WM8731_NORMAL_384FS_44_1KHZ    0x22
#define WM8731_NORMAL_128FS_44_1KHZ    0x3E

/* Active control register*/
#define WM8731_REG_ACTIVE_CTRL                     0x9
#define WM8731_ACTIVE_BIT                            0 // Activate interface


typedef struct Wm8731
{
	I2c *i2c;
	uint8_t addr;
} Wm8731;


/**
 * Devices which is possible to set volume
 * \}
 */
#define 	WM8731_LINE_IN        BV(0)
#define 	WM8731_HEADPHONE      BV(1)
/* \} */

/**
 * Set the volume of select device line in or Headphone.
 * The volume is express in %, and the volume range is 0-100%.
 */
void wm8731_setVolume(Wm8731 *ctx, uint16_t device, uint8_t volume);
void wm8731_powerOn(Wm8731 *ctx);
void wm8731_powerOff(Wm8731 *ctx);
void wm8731_powerOnDevices(Wm8731 *ctx, uint16_t device);
void wm8731_powerOffDevices(Wm8731 *ctx, uint16_t device);

void wm8731_init(Wm8731 *ctx, I2c *i2c, uint8_t codec_addr);

#endif /* DRV_WM8731_H */
