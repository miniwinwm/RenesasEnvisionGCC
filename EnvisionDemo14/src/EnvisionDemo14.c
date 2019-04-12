#include <stdint.h>
#include <stdbool.h>
#include "iodefine.h"

void system_clock_config(void);

int main(void)
{
	/* init system clock */
	system_clock_config();

	/**
	 *  enable TMR0 and ELC modules
	 */

	/* enable writing to power control bit */
	SYSTEM.PRCR.WORD = 0xa502U;

	/* release the TMR0 module from stop */
	MSTP(TMR0) = 0U;

	/* release the ELC module from stop */
	MSTP(ELC) = 0U;

	/* disable writing to power control bit */
	SYSTEM.PRCR.WORD = 0xa500;

	/**
	 * set up gpio b6 as output
	 */

	/* mode to gpio */
	PORTB.PMR.BIT.B6 = 0U;

	/* cmos output type */
	PORTB.ODR1.BIT.B4 = 0U;

	/* output */
	PORTB.PDR.BIT.B6 = 1U;

	/* initial state on */
	PORTB.PODR.BIT.B6 = 1U;

	/**
	 * set up 8 bit timer 0
	 */

    /* set clock source select to internal clock */
	TMR0.TCCR.BIT.CSS = 1U;

    /* set clock select to PCLK/8192 */
	TMR0.TCCR.BIT.CKS = 6U;

	/* cleared by compare match B */
	TMR0.TCR.BIT.CCLR = 2U;

	/* compare match A interrupt disabled */
	TMR0.TCR.BIT.CMIEA = 0U;

	/* compare match B interrupt disabled */
	TMR0.TCR.BIT.CMIEB = 0U;

	/* set match A count */
	TMR0.TCORA = 128U;

	/* set match B count */
	TMR0.TCORB = 255U;

	/**
	 * set up event link, single port 0 output on timer match input
	 */

	/* in single port 0 event destination set port B */
	ELC.PEL0.BIT.PSP = 1U;

	/* in single port 0 event destination set port pin 6 */
	ELC.PEL0.BIT.PSB = 6U;

	/* in single port 0 event destination set pin toggled on event */
	ELC.PEL0.BIT.PSM = 2U;

	/* set event source: TMR0 compare match A, set event destination: single port 0  */
	ELC.ELSR24.BIT.ELS = 0x22U;

	/* enable event linking */
	ELC.ELCR.BIT.ELCON = 1U;

    while (true)
    {
    }

    return 0;
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
