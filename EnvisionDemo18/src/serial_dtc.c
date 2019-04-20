#include <stdint.h>
#include <stdbool.h>
#include "iodefine.h"
#include "serial_dtc.h"

#define DTC_MRA_SHIFT	24
#define DTC_MRB_SHIFT	16
#define DTC_MRC_SHIFT	8

/**
 * DTC channel transfer configuration when using 32 bit addressing
 */
typedef struct
{
    uint32_t mra_mrb_mrc;
    uint32_t sar;
    uint32_t dar;
    uint32_t cra_crb;
} dtc_transfer_config_t;

static volatile uint32_t __attribute__((section(".dtc_vectors"))) dtc_vectors[256];
static volatile dtc_transfer_config_t dtc_transferdata_vector102;
static volatile dtc_transfer_config_t dtc_transferdata_vector103;

volatile bool receive_complete;
volatile bool send_complete;

static void serial_dtc_init(void);
static void serial_send_sart(void);
static void serial_send_stop(void);
static void serial_receive_start(void);
static void serial_receive_stop(void);

void serial_send(uint8_t *const tx_buf, uint16_t tx_num)
{
	send_complete = false;

	dtc_transferdata_vector103.sar = (uint32_t)tx_buf;
	dtc_transferdata_vector103.cra_crb = (uint32_t)(tx_num) << 16;

	serial_send_sart();

	/* enable transmit interrupt */
	SCI9.SCR.BIT.TIE = 1U;

	/* enable transmit */
	SCI9.SCR.BIT.TE = 1U;
}

void serial_receive(uint8_t *const rx_buf, uint16_t rx_num)
{
	receive_complete = false;

	dtc_transferdata_vector102.dar = (uint32_t)rx_buf;
	dtc_transferdata_vector102.cra_crb = (uint32_t)(rx_num) << 16;

	serial_receive_start();

	/* enable receive interrupt */
	SCI9.SCR.BIT.RIE = 1U;

	/* enable receive */
	SCI9.SCR.BIT.RE = 1U;
}

bool serial_get_send_complete(void)
{
	return send_complete;
}

bool serial_get_receive_complete(void)
{
	return receive_complete;
}

void serial_init(void)
{
    /* disable SCI9 interrupts */
    IEN(SCI9, TXI9) = 0U;
    IEN(SCI9, RXI9) = 0U;
	IEN(ICU, GROUPBL1) = 0U;

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

    /* set TX end and error interrupt priority */
	IPR(ICU, GROUPBL1) = 2U;

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

    /* clear interrupt flag */
    IR(SCI9, TXI9) = 0U;
    IR(SCI9, RXI9) = 0U;
	IR(ICU, GROUPBL1) = 0U;

    /* enable SCI interrupts */
    IEN(SCI9, TXI9) = 1U;
    IEN(SCI9, RXI9) = 1U;
	IEN(ICU, GROUPBL1) = 1U;

	/* enable transmit end interrupt */
    ICU.GENBL1.BIT.EN26 = 1U;

    /* initialise the DTC part of serial driver */
    serial_dtc_init();
}

static void serial_dtc_init(void)
{
    uint8_t MRA;
    uint8_t MRB;
    uint8_t MRC;

    /* enable writing to MPC */
    MPC.PWPR.BIT.B0WI = 0U;
    MPC.PWPR.BIT.PFSWE = 1U;

    /* power up DTC module */
    MSTP(DTC) = 0U;

    /* disable writing to MPC */
    MPC.PWPR.BIT.PFSWE = 0U;
    MPC.PWPR.BIT.B0WI = 1U;

    /* disable transfer data read skip */
    DTC.DTCCR.BIT.RRS = 0U;

    /* set up mode registers' values for RX */
    MRA = 0x00U; /* MD = normal, SZ = 8 bit, SM = fixed, WBDIS = write back */
    MRB = 0x08U; /* CHNE = off, CHNS = n/a, DISEL = interrupt at end of transfer, DTS = n/a, DM = incremented, INDX = no indexing, SQEND = n/a */
    MRC = 0x00U; /* DISPE = not added */

    /* configure DTC mode registers for SCI9 RX */
    dtc_transferdata_vector102.mra_mrb_mrc = ((uint32_t)MRA) << DTC_MRA_SHIFT | ((uint32_t)MRB) << DTC_MRB_SHIFT | ((uint32_t)MRC) << DTC_MRC_SHIFT;

    /* set source of data to SCI9 read register */
    dtc_transferdata_vector102.sar = (uint32_t)(&SCI9.RDR);

    /* set SCI9 RX transfer data start address in DTC vector table */
    dtc_vectors[102] = (uint32_t)&dtc_transferdata_vector102;

    /* set up mode registers' values for TX */
    MRA = 0x08U; /* MD = normal, SZ = 8 bit, SM = incremented, WBDIS = write back */
    MRB = 0x00U; /* CHNE = off, CHNS = n/a, DISEL = interrupt at end of transfer, DTS = n/a, DM = fixed, INDX = no indexing, SQEND = n/a */
    MRC = 0x00U; /* DISPE = not added */

    /* configure DTC mode registers for SCI9 TX */
    dtc_transferdata_vector103.mra_mrb_mrc = ((uint32_t)MRA) << DTC_MRA_SHIFT | ((uint32_t)MRB) << DTC_MRB_SHIFT | ((uint32_t)MRC) << DTC_MRC_SHIFT;

    /* set destination of data to SCI9 transmit register */
    dtc_transferdata_vector103.dar = (uint32_t)(&SCI9.TDR);

    /* set SCI9 TX transfer data start address in DTC vector table */
    dtc_vectors[103] = (uint32_t)&dtc_transferdata_vector103;

    /* set address mode to 32 bit */
    DTC.DTCADMOD.BIT.SHORT = 0U;

    /* set DTC vector table base address */
    DTC.DTCVBR = (void *)0x0085FC00U;

    /* start DTC module */
    DTC.DTCST.BIT.DTCST = 1U;
}

static void serial_send_sart(void)
{
    /* set send dtc interrupt */
    ICU.DTCER[103].BIT.DTCE = 1U;
}

static void serial_send_stop(void)
{
    /* clear send dtc interrupt */
    ICU.DTCER[103].BIT.DTCE = 0U;
}

static void serial_receive_start(void)
{
    /* set receive dtc interrupt */
    ICU.DTCER[102].BIT.DTCE = 1U;
}

static void serial_receive_stop(void)
{
    /* clear receive dtc interrupt */
    ICU.DTCER[102].BIT.DTCE = 0U;
}

void serial_close(void)
{
	serial_send_stop();
	serial_receive_stop();

    /* disable protection */
    SYSTEM.PRCR.WORD = 0xa502U;

    /* power off SCI9 */
    MSTP(SCI9) = 1U;

    /* enable protection */
    SYSTEM.PRCR.WORD = 0xa500U;
}
