This example allows you to redirect normal console input/output to either the 
Renesas Debug Virtual Console in e2studio or the serial port available on the
Pmod connector on the Envision Kit (SCI9). If the serial port on the CN14 Pmod 
connector is used a small TTL to RS-232 level converter board is required,
available on EBay for a few $. A level converter board can be powered
from the power lines also available on the Pmod connector. 

All input/output routines available in stdio.h that eventually use puchar() &
getchar() can have their i/o redirected using this example code. These 
include printf(), puts(), scanf(), gets() etc., but not the functions that take
a FILE * parameter (i.e. fprintf()).

To choose the destination of the redirected input/output modify one of the 
#defines at the top of debug_io.h...

	#define DEBUG_IO_DESTINATION_SERIAL
	#define DEBUG_IO_DESTINATION_CONSOLE

This example works by replacing the standard puchar() and getchar() routines 
in the standard library with versions in debug_io.c. To facilitate this these
linker flags are required...

	-fno-builtin-putchar
	-fno-builtin-getchar

These linker flags need adding in Properties|C/C++ Build|Settings|Linker. Append 
them to the list of existing flags in the box labeled 'Expert settings: Command
line pattern'.

Some cautions: all the stdio routines used in this example are blocking, not 
thread safe, not suitable for use in interrupt handlers, and scanf() and gets()
are well known as a security risk from buffer overflow. Use only in test or 
debug code.

The declarations and code to write to the Renesas Debug Virtual Console is copied 
from lowlvl.c, part of the Renesas r_bsp package for RX65N processor. 