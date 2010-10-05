name = 'BeRTOS USB/Serial converter'
description="""
<h1>BeRTOS USB/Serial converter</h1>

<h2>Overview</h2>

<p>This example implements a real USB/serial converter using the <b><i>Olimex
STM32-P103</i></b> evaluation board. Two independent processes are created
exploiting the preemptive kernel. One process reads from the USB port and
writes to the UART port, the other does the opposite.</p>

<p>The project can be compiled and flashed to the STM32-P103. Then, the board
can be connected to any USB port of a standard PC and it will be recognized as
a generic USB/serial converter.</p>

<p>For example, on a Linux PC:</p>
<pre>
$ dmesg | tail
[18058.280545] usb 1-8.1.3: new full speed USB device using ehci_hcd and address 25
[18058.392775] usb 1-8.1.3: configuration #1 chosen from 1 choice
[18058.393118] usbserial_generic 1-8.1.3:1.0: generic converter detected
[18058.393332] usb 1-8.1.3: generic converter now attached to ttyUSB1
</pre>
"""
