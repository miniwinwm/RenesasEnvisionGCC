#ifndef DEBUG_IO_H_
#define DEBUG_IO_H_

/* Comment in one of these lines */
//#define DEBUG_IO_DESTINATION_SERIAL
#define DEBUG_IO_DESTINATION_CONSOLE

#if defined(DEBUG_IO_DESTINATION_SERIAL) && defined (DEBUG_IO_DESTINATION_CONSOLE)
#error "Define only 1 debug i/o destination"
#endif
#if !defined(DEBUG_IO_DESTINATION_SERIAL) && !defined(DEBUG_IO_DESTINATION_CONSOLE)
#error "Define a debug i/o destination"
#endif

#ifdef DEBUG_IO_DESTINATION_SERIAL
#define printf(...) SCI9.SCR.BIT.TE = 1U; printf(__VA_ARGS__); while (SCI9.SSR.BIT.TEND == 0); SCI9.SCR.BIT.TE = 0U;
#define scanf(...) SCI9.SCR.BIT.RE = 1U; scanf(__VA_ARGS__); SCI9.SCR.BIT.RE = 0U;
#endif

void debug_io_init(void);

#endif
