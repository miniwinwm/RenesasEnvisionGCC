#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "iodefine.h"
#include "debug_io.h"

void system_clock_config(void);

void main(void)
{
	int i;

	system_clock_config();

	debug_io_init();

    printf("Hello, world\r\n");
    printf("Enter a number...");

    scanf("%d", &i);

    printf("The number you entered was: %d\r\n", i);

    while (true);
}

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
