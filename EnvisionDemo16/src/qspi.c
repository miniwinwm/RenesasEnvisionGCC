#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "iodefine.h"
#include "qspi.h"

/**
 * Disable transmit and receive and clear status flags
 */
static void qspi_disable(void);

/**
 * Clear transmit buffer empty flag if it is set
 */
static void qspi_clear_tx_buffer_empty_flag(void);

/**
 * Clear receive buffer full flag if it is set
 */
static void qspi_clear_rx_buffer_full_flag(void);

/**
 * Clear QSSL negation flag if it is set
 */
static void qspi_clear_qssl_negation_flag(void);

/**
 * Call all the above flag clearing functions
 */
static void qspi_clear_status_flags(void);

/**
 * Reset transmit and receive buffers ready for use
 */
static void qspi_reset_buffers(void);

/**
 * Wait until transmit buffer empty flag is set
 *
 * @return QSPI_OK if the flag is set in expected time or QSPI_ERROR if timed out
 */
static qspi_status_t qspi_wait_transmit_buffer_empty_flag(void);

/**
 * Wait until receive buffer empty empty is set
 *
 * @return QSPI_OK if the flag is set in expected time or QSPI_ERROR if timed out
 */
static qspi_status_t qspi_wait_receive_buffer_full(void);

/**
 * Wait until there is any data in the receive buffer
 *
 * @return QSPI_OK if data is received in expected time or QSPI_ERROR if timed out
 */
static qspi_status_t qspi_wait_receive_buffer_has_data(void);

/**
 * Wait until QSSL negation flag is set
 *
 * @return QSPI_OK if the flag is set in expected time or QSPI_ERROR if timed out
 */
static qspi_status_t qspi_wait_qssl_negation_flag(void);


void qspi_open(void)
{
	uint8_t dummy_read_byte;

	/* set up qssl pin */

	/* cmos output */
	PORTD.ODR1.BIT.B0 = 0U;

	/* high drive output */
	PORTD.DSCR.BIT.B4 = 1U;

	/* output */
	PORTD.PDR.BIT.B4 = 1U;

	/* set to on */
	PORTD.PODR.BIT.B4 = 1U;

	/* set up qspi clock pin */

	/* cmos output */
	PORTD.ODR1.BIT.B2 = 0U;

	/* high drive output */
	PORTD.DSCR.BIT.B5 = 1U;

	/* output */
	PORTD.PDR.BIT.B5 = 1U;

	/* set to on */
	PORTD.PODR.BIT.B5 = 1U;

	/* set up qspi QIO0 pin */

	/* cmos output */
	PORTD.ODR1.BIT.B4 = 0U;

	/* pull up off */
	PORTD.PCR.BIT.B6 = 0U;

	/* high drive output */
	PORTD.DSCR.BIT.B6 = 1U;

	/* set up qspi QIO1 pin */

	/* cmos output */
	PORTD.ODR1.BIT.B6 = 0U;

	/* pull up off */
	PORTD.PCR.BIT.B7 = 0U;

	/* high drive output */
	PORTD.DSCR.BIT.B7 = 1U;

	/* set up qspi QIO2 pin */

	/* cmos output */
	PORTD.ODR0.BIT.B4 = 0U;

	/* pull up off */
	PORTD.PCR.BIT.B2 = 0U;

	/* high drive output */
	PORTD.DSCR.BIT.B2 = 1U;

	/* set up qspi QIO3 pin */

	/* cmos output */
	PORTD.ODR0.BIT.B6 = 0U;

	/* pull up off */
	PORTD.PCR.BIT.B3 = 0U;

	/* high drive output */
	PORTD.DSCR.BIT.B3 = 1U;


    /* enable writing to MPC */
	SYSTEM.PRCR.WORD = 0xa502U;

	/* release QSPI from stop */
    MSTP(QSPI) = 0U;

    /* disable writing to MPC */
	SYSTEM.PRCR.WORD = 0xa500U;

	/* disable QSPI module */
	QSPI.SPCR.BIT.SPE = 0U;

	/* disable transmit interrupt */
	QSPI.SPCR.BIT.SPTIE = 0U;

	/* disable receive interrupt */
	QSPI.SPCR.BIT.SPRIE = 0U;

	/* disable QSSL negation interrupt */
	QSPI.SPCR.BIT.SPSSLIE = 0U;

	/* set master mode */
	QSPI.SPCR.BIT.MSTR = 1U;

	/* set QSSL asserted low */
	QSPI.SSLP.BIT.SSLP = 0U;

	/* set unused QSPIO2 and QSPIO3 pins high */
	QSPI.SPPCR.BIT.IO2FV = 1U;
	QSPI.SPPCR.BIT.IO3FV = 1U;

	/* data output pin idle state high */
	QSPI.SPPCR.BIT.MOIFE = 1U;

	/* data output pin idle state defined by MOIFE */
	QSPI.SPPCR.BIT.MOIFV = 1U;

	/* set baud rate to 30 Mbps */
	QSPI.SPBR.BYTE = 1U;

	/* set clock delay to 1.5 QSPI clock period */
	QSPI.SPCKD.BIT.SCKDL = 0U;

	/* set negation delay to 1 QSPI clock period */
	QSPI.SSLND.BIT.SLNDL = 0U;

	/* set next access delay to 1 QSPI clock period */
	QSPI.SPND.BIT.SPNDL = 0U;

	/* disable dummy data transmission when receiving */
	QSPI.SPDCR.BIT.TXDMY = 0U;

	qspi_clear_status_flags();

	/* set SPI mode 3 */
	QSPI.SPCMD0.BIT.CPOL = 1U;
	QSPI.SPCMD0.BIT.CPHA = 1U;

	/* set baud rate divider to 1 */
	QSPI.SPCMD0.BIT.BRDV = 0U;

	/* set single spi operation */
	QSPI.SPCMD0.BIT.SPIMOD = 0U;

	/* msb first */
	QSPI.SPCMD0.BIT.LSBF = 0U;

	/* next access delay set in register */
	QSPI.SPCMD0.BIT.SPNDEN = 1U;

	/* qssl negation delay set in register */
	QSPI.SPCMD0.BIT.SLNDEN = 1U;

	/* clock delay set in register */
	QSPI.SPCMD0.BIT.SCKDEN = 1U;

	/* duplicate settings for sequence 1 */
	QSPI.SPCMD1.WORD = QSPI.SPCMD0.WORD;

	/* duplicate settings for sequence 2 */
	QSPI.SPCMD2.WORD = QSPI.SPCMD0.WORD;

	/* transfer data length 8 bits for sequence 0 */
	QSPI.SPCMD0.BIT.SPB = 0;

	/* transfer data length 8 bits for sequence 2 */
	QSPI.SPCMD2.BIT.SPB = 0;

	/* negate QSSL at end of transfer for sequence 2 */
	QSPI.SPCMD2.BIT.SSLKP = 0U;

    /* receive buffer triggers at 32 bytes */
    QSPI.SPBFCR.BIT.RXTRG = 7U;

    /* transmit buffer triggers at 32 bytes */
    QSPI.SPBFCR.BIT.TXTRG = 3U;
    QSPI.SPBFCR.BIT.TXTRGEX = 0U;

    /* required after a write to SPBFCR register */
    dummy_read_byte = QSPI.SPDR.BYTE.HH;

	/* enable writing to MPC */
	MPC.PWPR.BIT.B0WI = 0U;
	MPC.PWPR.BIT.PFSWE = 1U;

	/* set all qspi B pins to qspi function */
	MPC.PD2PFS.BIT.PSEL = 0x1bU;
	MPC.PD3PFS.BIT.PSEL = 0x1bU;
	MPC.PD4PFS.BIT.PSEL = 0x1bU;
	MPC.PD5PFS.BIT.PSEL = 0x1bU;
	MPC.PD6PFS.BIT.PSEL = 0x1bU;
	MPC.PD7PFS.BIT.PSEL = 0x1bU;

	/* disable writing to MPC */
	MPC.PWPR.BIT.PFSWE = 0U;
	MPC.PWPR.BIT.B0WI = 1U;

	/* set mode of all qspi pins to peripheral */
	PORTD.PMR.BIT.B2 = 1U;
	PORTD.PMR.BIT.B3 = 1U;
	PORTD.PMR.BIT.B4 = 1U;
	PORTD.PMR.BIT.B5 = 1U;
	PORTD.PMR.BIT.B6 = 1U;
	PORTD.PMR.BIT.B7 = 1U;
}

static qspi_status_t qspi_wait_transmit_buffer_empty_flag(void)
{
	uint32_t counter = 0U;

	while (QSPI.SPSR.BIT.SPTEF == 0U)
	{
		__asm("NOP");
		__asm("NOP");
		__asm("NOP");
		__asm("NOP");

		counter++;
		if (counter == 1000000U)
		{
			return QSPI_ERROR;
		}
	}

	return QSPI_OK;
}

static qspi_status_t qspi_wait_receive_buffer_full(void)
{
	uint32_t counter = 0U;

	while (QSPI.SPSR.BIT.SPRFF == 0U)
	{
		__asm("NOP");
		__asm("NOP");
		__asm("NOP");
		__asm("NOP");

		counter++;
		if (counter == 1000000U)
		{
			return QSPI_ERROR;
		}
	}

	return QSPI_OK;
}

static qspi_status_t qspi_wait_receive_buffer_has_data(void)
{
	uint32_t counter = 0U;

    while (QSPI.SPBDCR.BIT.RXBC == 0U)
    {
		__asm("NOP");
		__asm("NOP");
		__asm("NOP");
		__asm("NOP");

		counter++;
		if (counter == 1000000U)
		{
			return QSPI_ERROR;
		}
	}

	return QSPI_OK;
}

static qspi_status_t qspi_wait_qssl_negation_flag(void)
{
	uint32_t counter = 0U;

	while (QSPI.SPSR.BIT.SPSSLF == 0U)
	{
		__asm("NOP");
		__asm("NOP");
		__asm("NOP");
		__asm("NOP");

		counter++;
		if (counter == 1000000U)
		{
			return QSPI_ERROR;
		}
	}

	return QSPI_OK;
}

static void qspi_clear_status_flags(void)
{
	qspi_clear_tx_buffer_empty_flag();
	qspi_clear_rx_buffer_full_flag();
	qspi_clear_qssl_negation_flag();
}

static void qspi_reset_buffers(void)
{
	uint8_t dummy_read_byte;

	/* reset receive and transmit buffers */
    QSPI.SPBFCR.BIT.RXRST = 1U;
    QSPI.SPBFCR.BIT.TXRST = 1U;

    /* required after a write to SPBFCR register */
    dummy_read_byte = QSPI.SPDR.BYTE.HH;

    /* enable receive and transmit buffers */
    QSPI.SPBFCR.BIT.RXRST = 0U;
    QSPI.SPBFCR.BIT.TXRST = 0U;

    /* required after a write to SPBFCR register */
    dummy_read_byte = QSPI.SPDR.BYTE.HH;
}

static void qspi_disable(void)
{
	/* disable transmit and receive */
    QSPI.SPCR.BIT.SPE = 0U;

    /* clear status flags */
    qspi_clear_status_flags();
}

static void qspi_clear_tx_buffer_empty_flag(void)
{
	if (QSPI.SPSR.BIT.SPTEF == 1U)
	{
		QSPI.SPSR.BIT.SPTEF = 0U;
	}
}

static void qspi_clear_rx_buffer_full_flag(void)
{
	if (QSPI.SPSR.BIT.SPRFF == 1U)
	{
		QSPI.SPSR.BIT.SPRFF = 0U;
	}
}

static void qspi_clear_qssl_negation_flag(void)
{
	if (QSPI.SPSR.BIT.SPSSLF == 1U)
	{
		QSPI.SPSR.BIT.SPSSLF = 0U;
	}
}

void qspi_close(void)
{
    /* enable writing to MPC */
    SYSTEM.PRCR.WORD = 0xa502U;

    /* power off QSPI module */
    MSTP(QSPI) = 1U;

    /* disable writing to MPC */
    SYSTEM.PRCR.WORD = 0xa500U;

	/* set mode of all qspi pins to gpio */
	PORTD.PMR.BIT.B2 = 0U;
	PORTD.PMR.BIT.B3 = 0U;
	PORTD.PMR.BIT.B4 = 0U;
	PORTD.PMR.BIT.B5 = 0U;
	PORTD.PMR.BIT.B6 = 0U;
	PORTD.PMR.BIT.B7 = 0U;
}

static void qspi_write_32b(uint8_t *data)
{
	uint8_t i;

	for (i = 0U; i < 8U; i++)
	{
		QSPI.SPDR.LONG = __builtin_bswap32(*((uint32_t *)data));
		data += 4;
	}
}

static void qspi_read_32b(uint8_t *data)
{
	uint8_t i;

	for (i = 0U; i < 8U; i++)
	{
		*((uint32_t *)data) = __builtin_bswap32(QSPI.SPDR.LONG);
		data += 4;
	}
}

static void qspi_dummy_write_32b(void)
{
	uint8_t i;

	for (i = 0U; i < 8U; i++)
	{
	    QSPI.SPDR.LONG = 0xffffffffU;
	}
}

static void qspi_dummy_read_32b(void)
{
    volatile uint32_t dummy_read_long;
	uint8_t i;

	for (i = 0U; i < 8U; i++)
	{
	    dummy_read_long = QSPI.SPDR.LONG;
	}
}

static void qspi_configure_sequences(uint32_t command_length, uint32_t data_length)
{
    /**
     * Configure command sequence
     */

	/* set transfer data length to byte in sequence 0 */
	QSPI.SPCMD0.BIT.SPB = 0U;

	/* number of bytes to transfer in sequence 0 */
	QSPI.SPBMUL0 = command_length;

	if (data_length == 0U)
	{
		/* use sequences 0 (command) only */
		QSPI.SPSCR.BIT.SPSC = 0U;

		/* negate QSSL signal upon completion of sequence 0 */
		QSPI.SPCMD0.BIT.SSLKP = 0U;

		return;
	}

	/**
	 * Configure data sequences
	 */

	if ((data_length >= 32U) && ((data_length % 32U) != 0U))
    {
    	/* size of data > 32 but not divisible by 32, 2 sequences needed for data, long and byte transfers */

        /* set transfer data length to long in sequence 1 */
        QSPI.SPCMD1.BIT.SPB = 2U;

        /* number of longs to transfer in sequence 1 */
        QSPI.SPBMUL1 = (data_length / 32U) * 8U;

        /* set transfer data length to byte in sequence 2 */
        QSPI.SPCMD2.BIT.SPB = 0U;

        /* number of bytes to transfer in sequence 2 */
        QSPI.SPBMUL2 = data_length % 32U;

    	/* use sequences 0 (command), 1 and 2 (data) */
        QSPI.SPSCR.BIT.SPSC = 2U;

       	/* don't negate QSSL signal upon completion of sequence 0 */
        QSPI.SPCMD0.BIT.SSLKP = 1U;

       	/* don't negate QSSL signal upon completion of sequence 1 */
        QSPI.SPCMD1.BIT.SSLKP = 1U;

       	/* negate QSSL signal upon completion of sequence 2 */
       	QSPI.SPCMD2.BIT.SSLKP = 0U;
    }
    else
    {
    	if ((data_length >= 32U) && ((data_length % 32U) == 0U))
		{
			/* size of data >= 32 and divisible by 32, 1 sequence needed for data, long transfers */

			/* set transfer data length to long */
			QSPI.SPCMD1.BIT.SPB = 2U;

			/* number of longs to transfer */
			QSPI.SPBMUL1 = data_length / 4U;
		}
		else
		{
			/* size of data <32, 1 sequence needed for data, byte transfers */

			/* set transfer data length to byte in sequence 1 */
			QSPI.SPCMD1.BIT.SPB = 0U;

			/* number of bytes to transfer in sequence 1 */
			QSPI.SPBMUL1 = data_length;
		}

    	/* use sequences 0 (command) and 1 (data) */
        QSPI.SPSCR.BIT.SPSC = 1U;

       	/* don't negate QSSL signal upon completion of sequence 0 */
        QSPI.SPCMD0.BIT.SSLKP = 1U;

       	/* negate QSSL signal upon completion of sequence 1 */
        QSPI.SPCMD1.BIT.SSLKP = 0U;
    }
}

qspi_status_t qspi_command(uint8_t *command,
		uint32_t command_length,
		qspi_operation_t operation,
		uint8_t *data,
		uint32_t data_length)
{
	uint8_t dummy_read_byte;

	/* check parameters */
	if (operation != QSPI_COMMAND_AND_READ && operation != QSPI_COMMAND_AND_WRITE && operation != QSPI_COMMAND_ONLY)
	{
		return QSPI_ERROR;
	}

	if (command_length == 0U)
	{
		return QSPI_ERROR;
	}

	if (operation == QSPI_COMMAND_ONLY)
	{
		if (data != NULL || data_length > 0U)
		{
			return QSPI_ERROR;
		}
	}
	else
	{
		if (data == NULL || data_length == 0U)
		{
			return QSPI_ERROR;
		}
	}

	/* clear status flags */
	qspi_clear_status_flags();

	/* reset receive and transmit buffers */
	qspi_reset_buffers();

	/* configure sequence 0 and 1 and 2 if required */
	qspi_configure_sequences(command_length, data_length);

	/* enable QSPI function */
	QSPI.SPCR.BIT.SPE = 1U;

	/* send command */
    do
    {
    	/* transmit next byte of command */
        QSPI.SPDR.BYTE.HH = *command;
        command++;
        command_length--;

        /* wait for dummy response byte in receive buffer*/
        if (qspi_wait_receive_buffer_has_data() == QSPI_ERROR)
        {
			/* wait timed out, disable QSPI transmission and reception and clear flags */
			qspi_disable();

			return QSPI_ERROR;
        }

        /* read dummy response byte from buffer */
        dummy_read_byte = QSPI.SPDR.BYTE.HH;
    }
    while (command_length > 0U);

    /* send data if length > 0 */
    if (data_length > 0U)
    {
        /* check if any 32 byte blocks need transferring */
    	if (data_length >= 32U)
    	{
    		do
    		{
    			/* wait until transmit buffer empty */
    	        if (qspi_wait_transmit_buffer_empty_flag() == QSPI_ERROR)
    			{
    				/* wait timed out, disable QSPI transmission and reception and clear flags */
    				qspi_disable();

    				return QSPI_ERROR;
    			}

    			/* write real data or dummy data to data register */
    			if (operation == QSPI_COMMAND_AND_WRITE)
    			{
    				/* real data */
    				qspi_write_32b(data);
    				data += 32;
    			}
    			else
    			{
    				/* dummy data */
    				qspi_dummy_write_32b();
    			}

    			/* update remaining bytes to write */
    			data_length -= 32U;

    			/* clear transmit buffer empty flag */
    			qspi_clear_tx_buffer_empty_flag();

    			/* wait until receive buffer has 32 bytes in it */
    	        if (qspi_wait_receive_buffer_full() == QSPI_ERROR)
    			{
    				/* wait timed out, disable QSPI transmission and reception and clear flags */
    				qspi_disable();

    				return QSPI_ERROR;
    			}

    			/* read real data or dummy data from data register */
    			if (operation == QSPI_COMMAND_AND_READ)
    			{
    				/* read 32 bytes of real data */
    				qspi_read_32b(data);

    				/* update data buffer pointer */
    				data += 32;
    			}
    			else
    			{
    				/* read 32 bytes of dummy data */
    				qspi_dummy_read_32b();
                }

                /* clear receive buffer full flag */
                qspi_clear_rx_buffer_full_flag();
            }
            while (data_length >= 32U);
    	}

    	/* check if any data remaining to be transferred as single bytes after block transfers */
        if (data_length > 0U)
        {
            do
            {
            	if (operation == QSPI_COMMAND_AND_WRITE)
            	{
            		/* write a byte of data */
                    QSPI.SPDR.BYTE.HH = *data;

                    /* update data buffer pointer */
                    data++;
            	}
            	else
            	{
    				/* dummy byte write */
    				QSPI.SPDR.BYTE.HH = 0xffU;
                }

                /* wait for response byte in receive buffer*/
                if (qspi_wait_receive_buffer_has_data() == QSPI_ERROR)
                {
        			/* wait timed out, disable QSPI transmission and reception and clear flags */
        			qspi_disable();

        			return QSPI_ERROR;
                }

                if (operation == QSPI_COMMAND_AND_READ)
                {
    				/* receive a byte of data */
    				*data = QSPI.SPDR.BYTE.HH;

    				/* update data buffer pointer */
    				data++;
                }
                else
                {
    				/* dummy byte read */
    				dummy_read_byte = QSPI.SPDR.BYTE.HH;
                }

                /* update count of received data */
                data_length--;
            }
            while (data_length > 0U);
        }
    }

	/* wait for QSSL negation */
	if (qspi_wait_qssl_negation_flag() == QSPI_ERROR)
	{
		/* wait timed out, disable QSPI transmission and reception and clear flags */
		qspi_disable();

		return QSPI_ERROR;
	}

    /* disable QSPI transmission and reception and clear flags */
    qspi_disable();

	return QSPI_OK;
}
