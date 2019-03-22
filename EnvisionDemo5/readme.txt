This demonstration shows how to use the real time clock.

No crystal is fitted on the Envision Kit board to the 32.768kHz
real time clock input pins (the sub-clock), so instead the main 
clock is used as the RTC cloc source.

This example goes directly to registers but a simple API is provided.
