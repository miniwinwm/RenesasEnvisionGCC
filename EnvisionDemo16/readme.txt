This example provides a driver for the Envision Kit's on-board flash memory chip
which uses a QSPI interface. The driver is at 2 levels, the QSPI level which sets
up, reads and writes to the RX65N's QSPI peripheral, and a higher level which
prepares and sends commands to the MX25L QSPI flash chip on the Envision Kit
board. The test code for this example erases, writes, reads back and compares
3 sections of the flash chip's memory.

Only a small sub-set of the commands the MX25L supports is implemented in the 
MX25L layer. It will be easy to extend this for other commands.

A feature of the MX25L chip to be noted is that writes can be a maximum of 256
bytes at a time and must not cross a 256 page boundary, although apart from that 
restriction, writes can start at any byte address. For example, writing 10 bytes
starting at address 240 is allowed. Writing 20 bytes starting at address 240 is
not allowed. The MX25L layer write command enforces this restriction.

To keep things simple and understandable as an example this code only uses QSPI
in single SPI mode and does not use DTC or DMA. However, those extra features 
can be added if required by building on this example.

This module does things differently from the FIT QSPI module. The RX QSPI 
peripheral can control the QSPI chip select line (QSSL) in hardware. This 
example uses that feature, the FIT module does not, and requires the user to 
control the QSSL line from software using a GPIO. The reason for this is 
explained here, and it's to do with the style of the API.

First, a brief explanation of QSPI as used to communicate with a QSPI flash 
chip as found on the Envision Kit. QSPI even in single SPI mode is slightly 
different from standard SPI. In standard SPI the chip select (CS) line really is 
that. If you only have one SPI slave on your SPI lines you can select CS to 
asserted and leave it at that. You can even tie it to asserted in hardware.

In QSPI the QSSL line isn't used only for chip selecting purpose, it's used for 
signalling the end of transmission as well. A typical QSPI operation is...

	- Assert QSSL
	- Send command details
	- Send data, keep on sending unspecified amount of data until...
	- De-assert QSSL which signals that the operation has ended

Now for how this example's API and the FIT QSPI module's API vary. The example 
operation described here is to write a page of data. This requires a 4 byte 
command and 256 bytes of data, 260 bytes in all. The FIT module allows you to 
write arbitrary sized chunks of that data across multiple calls, for example 
write 10, 128, 50, 72 bytes to reach the total of 260. This example's API requires
one call, giving a pointer to the command, it's length, a pointer to the data, and 
it's length.

Given the order of operations above, whether the API is used as an enforced 
single call or optional multiple calls the QSSL line must be asserted at the 
beginning and de-asserted at the end. With multiple calls of arbitrary length data 
sections (as the FIT module allows) - that's a problem, and it's to do with 
sequences, which control how the data is transferred.

The RX QSPI module uses the concept of sequences. A sequence defines the size of 
data to be transferred (byte/short/long) and how many bytes/shorts/longs in the 
sequence. A complete QSPI operation comprises 1 or more (up to 4) sequences. If a 
large amount of data is being transferred then the operation should transfer most 
of it as longs for efficiency, so an operation will comprise a sequence of longs 
followed by an optional sequence of bytes if there are any bytes left over. 

These sequences must be defined before the QSPI enable flag in the RX QSPI 
peripheral is asserted which allows transmission to start. Once the QSPI enable 
flag has been asserted the manual says don't touch these sequences until the 
operation is finished and the QSPI enable flag is de-asserted. So here's the 
problem. If you have an API like the FIT module which allows arbitrary length 
chunks to be sent you cannot configure your sequences once at the start - you 
have to reconfigure your sequences after every API call to transfer the next chunk 
of data. That requires the QSSPI peripheral to be de-asserted - and that 
de-asserts QSSL signalling the chip that the transfer has completed. So the 
only options are give details of all the data in one single API call (as this
example does), or require the user to operate the QSSL line via software using 
a GPIO (as the FIT module does).

The driver here uses 1 to 3 sequences to implement a complete operation. The first 
sequence is always present and sends the command in byte chunks. If there is any
data following (there may not be, some operations are command only) a following
sequence is used. If the amount of data is >= 32 bytes a sequence of longs is
used to transfer the data. If there is any data remaining (< 32 bytes) a final
sequence of bytes is used. If the data is present but < 32 bytes the sequence 
of longs in the middle is not used.