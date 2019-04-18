#include <stdint.h>
#include <stdbool.h>
#include "iodefine.h"
#include "debug_io.h"

/* The declarations and code to write to the Renesas Debug Virtual Console is copied from lowlvl.c,
 * part of the Renesas r_bsp package for RX65N processor. */
struct st_dbg
{
    uint32_t   TX_DATA;     // Debug Virtual Console TX data
    char       wk1[12];     // spacer
    uint32_t   RX_DATA;     // Debug Virtual Console RX data
    char       wk2[44];     // spacer
    uint32_t   DBGSTAT;     // Debug Virtual Console Status
};

#define E1_DBG_PORT (*(volatile struct st_dbg *)0x84080)
#define TXFL0EN     0x00000100
#define RXFL0EN     0x00001000

int putchar(int c)
{
#ifdef DEBUG_IO_DESTINATION_CONSOLE
    /* wait for transmit buffer to be empty */
    while ((E1_DBG_PORT.DBGSTAT & TXFL0EN) != 0U)
    {
    }

    /* write the character out */
    E1_DBG_PORT.TX_DATA = c;
#else
    /* write the character out */
    SCI9.TDR = c;

    /* wait until it has gone */
    while (SCI9.SSR.BIT.TDRE == 0)
    {
    }
#endif

	return c;
}

int getchar(void)
{
#ifdef DEBUG_IO_DESTINATION_CONSOLE
    /* wait for receive buffer to be full */
    while ((E1_DBG_PORT.DBGSTAT & RXFL0EN) == 0U)
    {
    }

    /* read the character */
    return E1_DBG_PORT.RX_DATA;
#else
    /* wait until a character has arrived */
    while (SCI9.SSR.BIT.RDRF == 0)
    {
    }

    /* read and return the character */
    return SCI9.RDR;
#endif
}

void debug_io_init(void)
{
#ifdef DEBUG_IO_DESTINATION_CONSOLE
#else
    /* disable SCI9 TX and RX interrupts */
    IEN(SCI9, TXI9) = 0U;
    IEN(SCI9, RXI9) = 0U;

    /* disable protection */
    SYSTEM.PRCR.WORD = 0xa502U;

    /* power on SCI9 */
    MSTP(SCI9) = 0U;

    /* enable protection */
    SYSTEM.PRCR.WORD = 0xA500U;

    /**
     * SCR
     */

    /* set on chip baud rate generator, no clock output */
    SCI9.SCR.BIT.CKE = 0U;

    /* disable transmit end interrupt */
    SCI9.SCR.BIT.TEIE = 0U;

    /* disable reception */
    SCI9.SCR.BIT.RE = 0U;

    /* disable transmission */
    SCI9.SCR.BIT.TE = 0U;

    /* disable receive interrupt */
    SCI9.SCR.BIT.RIE = 0U;

    /* disable transmit interrupt */
    SCI9.SCR.BIT.TIE = 0U;

    /**
     * SMR
     */

    /* clock divider = 1 */
    SCI9.SMR.BIT.CKS = 0U;

    /* no multiprocessor mode */
    SCI9.SMR.BIT.MP = 0U;

    /* 1 stop bit */
    SCI9.SMR.BIT.STOP = 0U;

    /* no parity */
    SCI9.SMR.BIT.PE = 0U;

    /* 8 bit data */
    SCI9.SMR.BIT.CHR = 0U;

    /* asynchronous or simple iic mode */
    SCI9.SMR.BIT.CM = 0U;

    /**
     * BRR
     */

    /* set baud rate to 19200 */
    SCI9.BRR = 0xc2U;

    /**
     * SIMR1
     */

    /* asynchronous mode */
    SCI9.SIMR1.BIT.IICM = 0U;

    /**
     * SCMR
     */

    /* non smartcard mode */
    SCI9.SCMR.BIT.SMIF = 0U;

    /* 8 bit data */
    SCI9.SCMR.BIT.CHR1 = 1U;

    /* no data inversion */
    SCI9.SCMR.BIT.SINV = 0U;

    /**
     * SEMR
     */

    /* bit rate modulation disabled */
    SCI9.SEMR.BIT.BRME = 0U;

    /* base clock select 8 clock cycles per bit */
    SCI9.SEMR.BIT.ABCS = 1U;

    /* no noise filter */
    SCI9.SEMR.BIT.NFEN = 0U;

    /* don't double baud rate */
    SCI9.SEMR.BIT.BGDM = 0U;

    /* start receiving on falling edge */
    SCI9.SEMR.BIT.RXDESEL = 1U;

    /**
     * SMPR
     */

    /* these bits all required to be 0 when not using simple SPI mode */
    SCI9.SPMR.BIT.MSS = 0U;
    SCI9.SPMR.BIT.CKPH = 0U;
    SCI9.SPMR.BIT.CKPOL = 0U;

    /* set SCI9 RX interrupt priority */
    IPR(SCI9, RXI9) = 15;

    /* set SCI9 TX interrupt priority */
    IPR(SCI9, TXI9) = 15;

    /* set PB6 pin mode to peripheral */
    PORTB.PMR.BIT.B6 = 1U;

    /* set PB7 pin mode to peripheral */
	PORTB.PMR.BIT.B7 = 1U;

	/* enable writing to MPC */
    MPC.PWPR.BIT.B0WI = 0U;
    MPC.PWPR.BIT.PFSWE = 1U;

    /* set PB6 to SCI9 RXD */
    MPC.PB6PFS.BYTE = 0x0AU;

    /* set PB7 to SCI9 TXD */
    MPC.PB7PFS.BYTE = 0x0AU;

    /* disable writing MPC */
    MPC.PWPR.BIT.PFSWE = 0U;
    MPC.PWPR.BIT.B0WI = 1U;
#endif
}
