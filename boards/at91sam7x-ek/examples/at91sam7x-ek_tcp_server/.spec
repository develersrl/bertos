name = 'TCP/IP echo server'
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
The application then starts a TCP server listening on port 80. The server acts
as a TCP echo service, sending back to the clients all the submitted requests.
</p>
<p>
On the debugging console are reported the measured total requests and total
requests per second (reqs/s) served by the TCP service, together with the stack
usage of each process.
</p>
"""
