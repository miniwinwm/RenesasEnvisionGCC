This demonstration shows how to create an interrupt on a pin change.

The code goes straight to I/O registers setting up an interrupt on
either edge change of the GPIO connected to the user button.

On an interrupt being triggered in the interrupt handler the button
input line state is read and the user led set to the read value. This
causes the led to switch on when the button is pressed and switch off when
the button is released.