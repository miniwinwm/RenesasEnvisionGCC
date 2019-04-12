#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "iodefine.h"
#include "lcd_driver.h"

#define KITTEN_BITMAP_WIDTH_PIXELS	150U
#define KITTEN_BITMAP_HEIGHT_PIXELS	137U

void system_clock_config(void);
void setup_dma_button_trigger(void);

volatile uint32_t sections;											/* how many sections to complete the dma transfer */
volatile uint32_t section_offset = LCD_DISPLAY_WIDTH_PIXELS * sizeof(uint16_t);		/* offset from destination section start address to next section start address, in bytes */
volatile bool dma_complete;											/* flag indicating when all sections have been transferred */
volatile uint32_t section_transfers = KITTEN_BITMAP_WIDTH_PIXELS;	/* number of transfers of byte/short/word in one section */

extern const uint8_t kitten[];

int main(void)
{
	/* init system clock */
	system_clock_config();

	/* init and clear display */
	lcd_init();
	lcd_filled_rectangle(0, 0, LCD_DISPLAY_WIDTH_PIXELS, LCD_DISPLAY_HEIGHT_PIXELS, 0U);

	/* set up user push button to fire IRQ13 which is used by dma controller to start dma transfer of first section */
	setup_dma_button_trigger();

	/* enable writing to power control bit */
	SYSTEM.PRCR.WORD = 0xa502U;

	/* release the DMAC and DMAC0 module from stop */
	MSTP(DMAC0) = 0U;

	/* disable writing to power control bit */
	SYSTEM.PRCR.WORD = 0xa500U;

	/* disable DMA transfer */
	DMAC0.DMCNT.BIT.DTE = 0U;

	/* destination address is incremented */
	DMAC0.DMAMD.BIT.DM = 2U;

	/* source address is incremented */
	DMAC0.DMAMD.BIT.SM = 2U;

	/* no destination extended repeating */
	DMAC0.DMAMD.BIT.DARA = 0U;

	/* no source extended repeating */
	DMAC0.DMAMD.BIT.SARA = 0U;

	/* peripheral interrupt initiates first transfer, subsequent section initiated by software in isr */
	DMAC0.DMTMD.BIT.DCTG = 1U;

	/* 16 bit transfer size */
	DMAC0.DMTMD.BIT.SZ = 1U;

	/* no repeating */
	DMAC0.DMTMD.BIT.DTS = 2U;

	/* normal transfer */
	DMAC0.DMTMD.BIT.MD = 0U;

	/* set source address */
	DMAC0.DMSAR = (void *)kitten;

	/* set destination address to graphics buffer */
	DMAC0.DMDAR = (void *)0x00800000U;

	/* set number of transfers in one section */
	DMAC0.DMCRA = section_transfers;

	/* transfer end interrupt enable */
	DMAC0.DMINT.BIT.DTIE = 1U;

	/* clear any previously pending interrupts */
	IR(DMAC, DMAC0I) = 0U;

	/* set interrupt priority */
	IPR(DMAC, DMAC0I) = 5U;

	/* enable DMAC0 interrupt */
	IEN(DMAC, DMAC0I) = 1U;

	/* start DMA module */
	DMAC.DMAST.BIT.DMST = 1U;

	/* software start bit auto cleared */
	DMAC0.DMREQ.BIT.CLRS = 1U;

	/* set dma start peripheral trigger to IRQ13 */
	ICU.DMRSR0 = VECT_ICU_IRQ13;

	/* prepare for first section */
	dma_complete = false;
	sections = KITTEN_BITMAP_HEIGHT_PIXELS;
	sections--;		/* first section is initiated by peripheral interrupt so one less to be initiated from software */

	/* go... */
	DMAC0.DMCNT.BIT.DTE = 1U;

	/* endless loop */
	uint32_t i = 0U;

    while (true)
    {
    	/* wait until a dma operation complete */
    	while (!dma_complete)
    	{
    	}

    	/* prepare for next dma transfer */
    	dma_complete = false;
    	sections = KITTEN_BITMAP_HEIGHT_PIXELS;
    	sections--;	/* first section is initiated by peripheral interrupt so one less to be initiated from software */
    	i++;

    	/* update destination address to move bitmap across display */
    	DMAC0.DMDAR = (void *)(0x00800000U + KITTEN_BITMAP_WIDTH_PIXELS * sizeof(uint16_t) * (i % 3U));

    	/* reset source address to start of bitmap */
    	DMAC0.DMSAR = (void *)kitten;

    	/* reset dma start request to peripheral, this is for the first section, subsequent section initiated by software in isr */
		DMAC0.DMTMD.BIT.DCTG = 1U;

		/* reset number of sections to transfer */
		DMAC0.DMCRA = section_transfers;

		/* go... */
		DMAC0.DMCNT.BIT.DTE = 1U;
    }

    return 0;
}

/**
 * Configure user button to create an IRQ13 interrupt request
 */
void setup_dma_button_trigger(void)
{
   /* enable writing to MPC pin function control registers */
	MPC.PWPR.BIT.B0WI = 0U;
	MPC.PWPR.BIT.PFSWE = 1U;

	/* set P05 to IRQ13 */
	MPC.P05PFS.BYTE = 0x40U;

	/* disable writing to MPC pin function control registers */
	MPC.PWPR.BIT.PFSWE = 0U;
	MPC.PWPR.BIT.B0WI = 1U;

	/* disable IRQ13 interrupt */
	IEN(ICU, IRQ13) = 0U;

	/* set up push button gpio input */
	PORT0.PMR.BIT.B5 = 0U;	/* mode to gpio */
	PORT0.PDR.BIT.B5 = 0U;	/* input */
	PORT0.PCR.BIT.B5 = 0U;  /* pull-up disable */

	/* disable IRQ13 digital filter */
	ICU.IRQFLTE1.BIT.FLTEN13 = 0U;

	/* trigger interrupt on low level */
	ICU.IRQCR[13].BIT.IRQMD = 0U;

	/* clear any outstanding IRQ13 interrupt */
	IR(ICU, IRQ13) = 0U;

	/* set IRQ13 priority level */
	IPR(ICU, IRQ13) = 2U;

	/* enable IRQ13 interrupt */
	IEN(ICU, IRQ13) = 1U;
}

/**
 * Configure the system and peripheral clocks
 */
void system_clock_config(void)
{
	/* enable all protect register bits */
	SYSTEM.PRCR.WORD = 0xa50bU;

	/* select resonator as main clock source */
	SYSTEM.MOFCR.BIT.MOSEL = 0U;

	/* stop the HOCO, not using it */
	SYSTEM.HOCOCR.BIT.HCSTP = 1U;

	/* turn off power to HOCO */
	SYSTEM.HOCOPCR.BIT.HOCOPCNT = 1U;

    /* disable the sub-clock oscillator as no crystal fitted */
    SYSTEM.SOSCCR.BIT.SOSTP = 1U;

	/* not using USB clock */
	SYSTEM.SCKCR2.WORD = 0x0001U;

	/* set drive level for 12MHz input */
	SYSTEM.MOFCR.BIT.MODRV2 = 2U;

	/* set wait time for starting main clock oscillator */
	SYSTEM.MOSCWTCR.BYTE = 0x53U;

	/* start main clock oscillator */
    SYSTEM.MOSCCR.BIT.MOSTP = 0U;

	/* set rom wait state appropriate to main clock speed */
	SYSTEM.ROMWT.BIT.ROMWT = 2U;

	/* set PLL division to 1 */
	SYSTEM.PLLCR.BIT.PLIDIV = 0U;

	/* set PLL multiplication to 20 */
	SYSTEM.PLLCR.BIT.STC = 39U;

	/* start the PLL */
	SYSTEM.PLLCR2.BIT.PLLEN = 0U;

	/* wait for clock stabilization */
	while (0 == SYSTEM.OSCOVFSR.BIT.PLOVF)
	{
		__asm("NOP");
	}

	/* ICLK to 120MHz */
	SYSTEM.SCKCR.BIT.ICK = 1U;

	/* FCLK to 60 MHz */
	SYSTEM.SCKCR.BIT.FCK = 2U;

	/* PCLKA to 120MHz */
	SYSTEM.SCKCR.BIT.PCKA = 1U;

	/* PCLKB to 60MHz */
	SYSTEM.SCKCR.BIT.PCKB = 2U;

	/* PCLKC to 60MHz */
	SYSTEM.SCKCR.BIT.PCKC = 2U;

	/* PCLKD to 60MHz */
	SYSTEM.SCKCR.BIT.PCKD = 2U;

	/* SDCLK not used */
	SYSTEM.SCKCR.BIT.PSTOP0 = 1U;

	/* BCLK not used */
	SYSTEM.SCKCR.BIT.PSTOP1 = 1U;

	/* switch main clock source to PLL */
	SYSTEM.SCKCR3.BIT.CKSEL = 4U;

	/* stop LOCO as finished with it now */
	SYSTEM.LOCOCR.BYTE = 1U;

	/* disable all protect register bits */
	SYSTEM.PRCR.WORD = 0xa500U;
}
