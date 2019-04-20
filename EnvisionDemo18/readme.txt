This example demonstrates the DTC in conjunction with the serial port SCI9 available
on the Envision Kit's Pmod connector (CN14).

The serial port is set up in asynchronous mode (19200, 8, N, 1) using no flow
control. Interrupts are enabled for transmit, transmit end and receive. Two DTC
channels are created linked to interrupts 102 and 103 for Rx and Tx for SCI9. These
DTC channels intercept the interrupt requests and perform data transfer autonomously,
only reaching the interrupt handlers' code after the configured number of bytes have
been transferred. When a Tx interrupt is serviced the transmit end interrupt is
enabled to detect when transmission of the last byte has been completed.

Although only 2 DTC channels are used space is allocated for an array of 256 DTC 
vectors and slots 102 and 103 contain pointers to the configuration structures for 
these 2 channels. In this example the DTC vector array is located at the top of the 
RX65N's extended RAM at address 0x0085FC00. This is after both the display buffers 
used in EnvisionDemo3.

To position the vector array at this location the generated linker script .ld file
needs amending. In the MEMORY list at the top where RAM, ROMS and OFS regions
are defined a new region needs adding:

	ERAM : ORIGIN = 0x800000, LENGTH = 0x60000	
	
In the SECTIONS list a new section needs adding like this:

	.dtc_vectors 0x0085FC00: AT(0x0085FC00)
	{
		KEEP(*(.dtc_vectors))
	} > ERAM
	
To define the array in C source to be located at this address a GCC __attribute needs 
to be used when declaraing the array like this:

static volatile uint32_t __attribute__((section(".dtc_vectors"))) dtc_vectors[256];

Looking in the .map file after linking shows that this array is at the required 
address:

	.dtc_vectors   0x0085fc00      0x400 ./src/serial_dtc.o
	
When the example is run and connected to a serial terminal the code does an endless
loop of writing and reading fixed sized packets of data to/from the serial port.