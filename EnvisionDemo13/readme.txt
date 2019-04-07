This example demonstrates the independent watchdog in window mode. The watchdog
cycles is set to approximately 4 seconds and the window in which the watchdog
is allowed to be kicked is the middle 2 seconds of that period (after 1s, before
3s). Kicking the watchdog outside of that window, or not kicking it at all, 
causes a reset. After a reset the reset cause is determined and displayed.

The watchdog is set up to start running automatically at reset. This requires
configuring the watchdog in the Option Setting Memory, in register OFS0. The 
value of the register is hard coded in the e2studio generated file vects.c near 
the bottom of the file. This file has been edited manually to set this register
value.

The watchdog in this example is kicked by pressing the user button. The state of
the watchdog cycle is shown on the LCD. Pressing the user button at the appropriate
time according to the message displayed will allow tha application to continue.
Pressing the button too early or too late according to the messages will cause a 
reset and restart.

The text displayed does not scroll. When it reached the bottom of the display - that's
it.