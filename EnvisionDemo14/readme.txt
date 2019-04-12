This example demonstrates the event link controller. This module allows events
to be communicated between hardware modules independent of interrupts or code.

This simple example sets up an 8 bit timer, TMR0, and on it's match A value
sends an event to the ELC that toggles port B6.

Port B6 is output on pin 10 of the Envision Kit's Pmod connector. Connecting
a scope to this pin will show a square wave output controlled by TMR0 via the 
ELC.

The code goes straight to registers.