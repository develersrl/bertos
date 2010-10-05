name = 'Virtual USB Mouse'
description="""
<h1>Virtual USB Mouse</h1>

<h2>Overview</h2>

<p>This project implements a HID USB device using the Olimex STM32-P103
evaluation board. The virtual mouse can be connected to the USB port of a
normal PC.</p>

<p>The WAKE_UP button on the board can be pressed to send to the host the mouse
movement to draw a rectangle shape (taking the left button pressed).</p>

<pExample (mouse detection on Linux):</p>
<pre>
$ dmesg | tail
[25361.710435] usb 1-8.1.3: new full speed USB device using ehci_hcd and address 33
[25361.943901] usb 1-8.1.3: configuration #1 chosen from 1 choice
[25362.004002] input: BeRTOS USB Mouse as /devices/pci0000:00/0000:00:1d.7/usb1/1-8/1-8.1/1-8.1.3/1-8.1.3:1.0/input/input19
[25362.004738] generic-usb 0003:FFFF:0000.0008: input,hidraw3: USB HID v1.10 Mouse [BeRTOS USB Mouse] on usb-0000:00:1d.7-8.1.3/input0
</pre>
"""
