This demonstration shows how to use both buffers of the display and
how to create an interrupt on a GPIO input edge change.

Drivers are included going straight to I/O registers.

The display is set up to use buffer 1 in 1 bit monochrome format and
buffer 2 in 16 bit RGB565 format.

Periodically, text is written to buffer 1 and scrolled up and the display
and a moving graphic is plotted to buffer 2. The display normally shows
buffer 2 but when the user push button on the board is held down buffer 1
and its text is shown on the LCD.
