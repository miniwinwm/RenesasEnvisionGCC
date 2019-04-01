This demonstration shows how to create a simple PWM output using an
8 bit timer.

The code goes straight to I/O registers. TMR3 is setup to perform
the PWM fully in hardware, once set up no code is required other than
to set the required duty cycle, which the code slowly changes from 0
to 100% over 2.5 seconds, then repeating the cycle.

The output PWM signal appears on P55. This has a connection to the Pmod
connector CN14 (pin 8). A LED with a 470 Ohm resistor between P55 and the ground
connection on the Pmod connector (pin 11) will show the LED output 
brightness changing according to the duty cycle set in main().