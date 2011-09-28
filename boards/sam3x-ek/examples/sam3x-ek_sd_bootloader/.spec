name = 'SD bootloader'
description="""
<h2>Overview</h2>
<p>
This example application demonstrates how to make simple bootloader with BeRTOS.
<\p>
<p>
The bootloader start every time we reset the board, and check if SD card if present.
If the SD is present the bootloader find on file system (FAT) the file named <b>firmware.bin</b>,
and make simple check if the board need to be flashed. After that the bootloader jump to flashed application.
</p>
"""
