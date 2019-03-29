#include <stdint.h>
#include <stdbool.h>
#include "iodefine.h"

void system_clock_config(void);
void timer_init(void);

int main(void)
{
	system_clock_config();
	timer_init();

	/* set up led gpio output */
	PORT7.PMR.BIT.B0 = 0U;	/* mode to gpio */
	PORT7.ODR0.BIT.B0 = 0U;	/* cmos output type */
	PORT7.PDR.BIT.B0 = 1U;	/* output */

    while (true)
    {
    	// user led state is changed in interrupt handler for CMT0
    }

    return 0U;
}

void timer_init(void)
{
	/* set the protect register to write enable and key */
	SYSTEM.PRCR.WORD = 0xa502U;

	/* Bring module out of stop state */
	MSTP(CMT0) = 0U;

	/* stop counter */
	CMT.CMSTR0.BIT.STR0 = 0U;

	/* set the match value */
	CMT0.CMCOR = 5859U;

	/* set the clock divider to 512 */
	CMT0.CMCR.BIT.CKS = 3U;

	/* enable interrupt on match */
	CMT0.CMCR.BIT.CMIE = 1U;

	/* clear counter. */
	CMT0.CMCNT = 0U;

	/* clear any previously pending interrupts */
	IR(CMT0, CMI0) = 0U;

	/* set interrupt priority */
	IPR(CMT0, CMI0) = 5U;

	/* enable compare match interrupt */
	IEN(CMT0, CMI0) = 1U;

	/* start counter. */
	CMT.CMSTR0.BIT.STR0 = 1U;

	/* set the protect register to write disable and key */
    SYSTEM.PRCR.WORD = 0xa500U;
}

/**
 * Configure the system and peripheral clocks
 */
void system_clock_config(void)
{
	/* enable all protect register bits */
	SYSTEM.PRCR.WORD = 0xa50bU;

	/* forcibly oscillate the main clock */
	SYSTEM.MOFCR.BIT.MOFXIN = 0U;

	/* select resonator as main clock source */
	SYSTEM.MOFCR.BIT.MOSEL = 0U;

	/* stop the HOCO, not using it */
	SYSTEM.HOCOCR.BIT.HCSTP = 1U;

	/* disable the sub-clock as no crystal input on hw */
	RTC.RCR3.BIT.RTCEN = 0U;

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
