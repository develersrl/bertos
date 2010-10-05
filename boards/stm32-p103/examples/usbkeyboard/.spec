name = 'Virtual USB Keyboard'
description="""
<h1>Virtual USB keyboard</h1>

<h2>Overview</h2>

<p>This project implements a HID USB device using the Olimex STM32-P103
evaluation board. The virtual keyboard can be connected to the USB port of a
normal PC.</p>

<p>The WAKE_UP button on the board can be pressed to send to the host the
keyboard scancodes that simulate the typing of <a
href="http://www.bertos.org">http://www.bertos.org</a>.

<p><i>NOTE: the keyboard layout on the host PC must be set to US
(QWERTY).</i></p>

<p>Example (keyboard detection on Linux):</p>
<pre>
[24388.400573] usb 1-8.1.3: new full speed USB device using ehci_hcd and address 32
[24388.623957] usb 1-8.1.3: configuration #1 chosen from 1 choice
[24388.685827] input: BeRTOS USB Keyboard as /devices/pci0000:00/0000:00:1d.7/usb1/1-8/1-8.1/1-8.1.3/1-8.1.3:1.0/input/input18
[24388.688953] generic-usb 0003:FFFF:0000.0007: input,hidraw3: USB HID v1.10 Keyboard [BeRTOS USB Keyboard] on usb-0000:00:1d.7-8.1.3/input0
</pre>
"""
