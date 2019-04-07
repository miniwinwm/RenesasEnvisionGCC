This demonstration shows how to use the DMA to copy from memory to
memory. The LCD is set up to show 16 bit RGB565 data. A bitmap is
included that is in the same format as the display. DMA is used to 
copy the bitmap data to the display buffer.

As the bitmap is less wide than the display, to copy the bitmap to
the display buffer non-contiguous DMA is required. The copy process
is split into sections where one section is a horizontal line of the
bitmap.

The copy process is initiated by pressing the user button. DMA channel
0 is initialized to start its transfer on a peripheral interrupt. The 
user button is initialized to create IRQ13 when it is pressed and the
DMA channel is configured to be triggered by this interrupt.

When a line has been copied by DMA a DMA transfer complete interrupt 
is generated. This updates the destination address to the next line in 
the display buffer and the next DMA transfer is initiated by software. 
All subsequent transfers for the remaining bitmap lines are software 
initiated.

The example goes directly to registers. When the application is 
running press the user button. On each press the bitmap is copied to 
the display buffer (for 3 presses after which the display is full).
