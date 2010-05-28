name = 'BeRTOS Context Switch'
description="""
<p>
This benchmark measures the overhead (in us) required to deliver a
signal and switch from a low-priority process to a high-priority process.
</p>
<p>
The measure is made by the high-precision timer if supported by the
architecture. Alternatively, the time can also be measured using an external
oscilloscope connected to the board's status LED.
</p>
<p>
For more information have a look at the <a href="http://www.bertos.org/discover/benchmarks">BeRTOS benchmarks page.</a>
</p>
"""
