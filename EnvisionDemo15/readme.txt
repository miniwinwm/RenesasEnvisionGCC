This example demonstrates how to put the processor in low power mode, in this
example the lowest of them all, deep software standby mode. In addition this
example shows how to detect the reason for a reset cycle and counts the number
of reset cycles since power-on, saving the count value in one of the deep cycle
backup registers.

Although putting the processor into a low power mode is a simple concept in 
practice it's not so simple. This is because a number of processor modules have
to be configured or used correctly. The modules affected are the reset handler,
the system module, the interrupt module, the clock module and user/supervisor mode.

The example displays what's happening on the LCD. To put the processor to sleep
the user push button should be pressed. To bring the processor back out of low
power mode there are 3 methods:

1) A low edge input on port 40 which is IRQ8-DS (this is different to the normal 
IRQ8 pin). This port on the Envision board is brought to pin 1 of CONN10. 

2) A reset signal on the RESET# line. On the Envision board this can be performed
by switching switch 2 on SW1 down and up.

3) A power cycle.

The instruction that puts the processor in power down mode after all the set up
has been completed (wait) is in an interrupt handler. This is because the wait 
instruction needs supervisor mode and interrupt handlers are run in this mode. A
software interrupt 1 is created to do this.

The code goes straight to registers.