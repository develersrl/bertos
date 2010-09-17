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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \author Andrea Righi <arighi@develer.com>
 *
 * \brief Generic USB serial device driver.
 *
 */

#include "cfg/cfg_usb_serial.h"

#define LOG_LEVEL  USB_SERIAL_LOG_LEVEL
#define LOG_FORMAT USB_SERIAL_LOG_FORMAT

#include <cfg/log.h>
#include <cfg/debug.h>
#include <cfg/macros.h>

#include <cfg/compiler.h>
#include <cfg/module.h>

#include <cpu/irq.h> /* IRQ_DISABLE / IRQ_ENABLE */
#include <cpu/power.h> /* cpu_relax() */

#include <drv/usb.h>

#include <string.h> /* memcpy() */

#include "drv/usb_serial.h"

#define USB_SERIAL_VENDOR_ID	0x05f9
#define USB_SERIAL_PRODUCT_ID	0xffff

#define USB_SERIAL_INTERFACES	1
#define USB_SERIAL_ENDPOINTS	3

#define USB_STRING_MANUFACTURER 1
#define USB_STRING_PRODUCT	2
#define USB_STRING_SERIAL	3

static usb_device_descriptor_t usb_serial_device_descriptor =
{
	.bLength = sizeof(usb_serial_device_descriptor),
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x110,
	.bDeviceClass = USB_CLASS_COMM,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.idVendor = USB_SERIAL_VENDOR_ID,
	.idProduct = USB_SERIAL_PRODUCT_ID,
	.bcdDevice = 0,
	.iManufacturer = USB_STRING_MANUFACTURER,
	.iProduct = USB_STRING_PRODUCT,
	.iSerialNumber = USB_STRING_SERIAL,
	.bNumConfigurations = 1,
};

static usb_config_descriptor_t usb_serial_config_descriptor =
{
	.bLength = sizeof(usb_serial_config_descriptor),
	.bDescriptorType = USB_DT_CONFIG,
	.bNumInterfaces = USB_SERIAL_INTERFACES,
	.bConfigurationValue = 1,
	.iConfiguration = 0,
	.bmAttributes = USB_CONFIG_ATT_ONE,
	.bMaxPower = 50, /* 100 mA */
};

static usb_interface_descriptor_t usb_serial_interface_descriptor =
{
	.bLength = sizeof(usb_serial_interface_descriptor),
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 0,
	.bAlternateSetting = 0,
	.bNumEndpoints = USB_SERIAL_ENDPOINTS,
	.bInterfaceClass = 0xff,
	.bInterfaceSubClass = 0,
	.bInterfaceProtocol = 0,
	.iInterface = 0,
};

static usb_endpoint_descriptor_t usb_serial_ep_report_descriptor =
{
	.bLength = sizeof(usb_serial_ep_report_descriptor),
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = USB_DIR_IN | 1,
	.bmAttributes = USB_ENDPOINT_XFER_INT,
	.wMaxPacketSize = usb_cpu_to_le16(8),
	.bInterval = 1,
};

static usb_endpoint_descriptor_t usb_serial_ep_in_descriptor =
{
	.bLength = sizeof(usb_serial_ep_in_descriptor),
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = USB_DIR_IN | 3,
	.bmAttributes = USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize = usb_cpu_to_le16(64),
	.bInterval = 0,
};

static usb_endpoint_descriptor_t usb_serial_ep_out_descriptor =
{
	.bLength = sizeof(usb_serial_ep_in_descriptor),
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = USB_DIR_OUT | 2,
	.bmAttributes = USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize = usb_cpu_to_le16(64),
	.bInterval = 0,
};

static usb_descriptor_header_t *usb_serial_config[] =
{
	(usb_descriptor_header_t *)&usb_serial_config_descriptor,
	(usb_descriptor_header_t *)&usb_serial_interface_descriptor,
	(usb_descriptor_header_t *)&usb_serial_ep_report_descriptor,
	(usb_descriptor_header_t *)&usb_serial_ep_in_descriptor,
	(usb_descriptor_header_t *)&usb_serial_ep_out_descriptor,
	NULL,
};

static DEFINE_USB_STRING(language_str, "\x09\x04"); // Language ID: en_US
static DEFINE_USB_STRING(manufacturer_str,
			USB_STRING("B", "e", "R", "T", "O", "S"));
static DEFINE_USB_STRING(product_str,
			USB_STRING("U", "S", "B", "-", "s", "e", "r", "i", "a", "l"));
static DEFINE_USB_STRING(serial_str,
			USB_STRING("0", "0", "1"));

static usb_string_descriptor_t *usb_serial_strings[] =
{
	(usb_string_descriptor_t *)&language_str,
	(usb_string_descriptor_t *)&manufacturer_str,
	(usb_string_descriptor_t *)&product_str,
	(usb_string_descriptor_t *)&serial_str,
	NULL,
};

static struct usb_device usb_serial = {
	.device = &usb_serial_device_descriptor,
	.config = usb_serial_config,
	.strings = usb_serial_strings,
};

ssize_t usb_serial_read(void *buffer, ssize_t size)
{
	return usb_ep_read(usb_serial_ep_out_descriptor.bEndpointAddress,
				buffer, size);
}

ssize_t usb_serial_write(const void *buffer, ssize_t size)
{
	return usb_ep_write(usb_serial_ep_in_descriptor.bEndpointAddress,
				buffer, size);
}

int usb_serial_init(void)
{
#if CONFIG_KERN
	MOD_CHECK(proc);
#endif
	if (usb_device_register(&usb_serial) < 0)
		return -1;
	LOG_INFO("usb-serial: registered new USB interface driver\n");
	return 0;
}
