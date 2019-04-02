This demonstration shows how to use the Multi-Function Timer Pulse Unit 3
module (MTU3) for PWM generation to control the level of the backlight.

Although this module can do simple PWM output that's not available on the 
pin that the backlight enable line is on (P66). This pin is only available
for MTU3 PWM using one PWM output line of complementary PWM or reset synchronized
PWM. Reset synchronized PWM is simpler, so that is used. The MTU3 knows this
output as MTIOC7D, and is one of multiple output lines controlled in reset synchronized
PWM. The other lines in this example are unused.

Reset synchronized PWM requires 2 timers to be used, which for this output pin
to be controlled need to be timers 6 and 7.

Buffering of the PWM timing registers is used to keep the timing and counting 
consistent, although the cycle period is fixed, so it's only the duty value that 
is changed. 

The code goes straight to I/O registers but a simple API is available
to set the backlight percentage.