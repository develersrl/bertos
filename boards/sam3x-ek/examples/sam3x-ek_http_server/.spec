name = 'Simple HTTP server'
description="""
<h2>Overview</h2>
<p>
This example application demonstrates the usage of the lwIP TCP/IP stack with
BeRTOS.
<\p>
<p>
The DHCP protocol is used to obtain an IP address from an external DHCP server
connected via ethernet.
<\p>
<p>
The server reply to the client request sending the page that match the file stored on SD
Card. If the SD or file is not preset, the server reply with header 404 FILE NOT found,
and send harcoded html page that report the occured error.
</p>
<p>
The BeRTOS simple HTTP server can reply the internal status or other things that the user wants, like: temperature,
local ip, etc, to do this the client can load special pages that match an harcoded callbacks, where it return to client
sending JSON string.
</p>
"""
