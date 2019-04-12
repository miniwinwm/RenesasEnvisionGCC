#include <stdint.h>
#include <stdbool.h>
#include "iodefine.h"

void system_clock_config(void);
void delay_us(uint32_t us);

/* Volts per degree C from hardware manual */
const float slope = 0.004f;

int main(void)
{
	float temperature;
	float Vs;
	float V1;

	system_clock_config();

	/* read calibration voltage at 128 degrees C recorded when chip manufactured */
	V1 = 3.3f * (float)TEMPSCONST.TSCDR / 4096.0f;

	/* enable writing to power control bit */
	SYSTEM.PRCR.WORD = 0xa502U;

	/* release the S12AD1 module from stop */
	MSTP(S12AD1) = 0U;

	/* release the TEMPS module from stop */
	MSTP(TEMPS) = 0U;

	/* disable writing to power control bit */
	SYSTEM.PRCR.WORD = 0xa500;

	/* select 12 bit resolution, right justified format */
	S12AD1.ADCER.BIT.ADPRC = 0U;
	S12AD1.ADCER.BIT.ADRFMT = 0U;

	/* temperature addition/average mode off */
	S12AD1.ADEXICR.BIT.TSSAD = 0U;

	/* require temperature measurement in single scan mode */
	S12AD1.ADEXICR.BIT.TSSA = 1U;

	/* set single scan mode */
	S12AD1.ADCSR.BIT.ADCS = 0U;

	/* start the temperature sensor, wait 30us to stabilize */
	TEMPS.TSCR.BIT.TSEN = 1U;
	delay_us(30U);

	/* enable temperature sensor output */
	TEMPS.TSCR.BIT.TSOE = 1U;

	/* start conversion */
	S12AD1.ADCSR.BIT.ADST = 1U;

	/* disable temperature sensor output */
	TEMPS.TSCR.BIT.TSOE = 0U;

	/* stop temperature sensor output */
	TEMPS.TSCR.BIT.TSEN = 0U;

	/* wait until conversion finished */
	while (S12AD1.ADCSR.BIT.ADST == 1U)
	{
		__asm("NOP");
	}

	/* calculate voltage from temperature sensor converted by ADC */
	Vs = 3.3f * (float)S12AD1.ADTSDR / 4096.0f;

	/* calculate temperature from voltage using formula from 'TEMPS' section of hardware manual */
	temperature = (Vs - V1) / slope + 128.0f;

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

void __attribute__((optimize("O3"))) delay_us(uint32_t us)
{
	uint32_t i;
	uint32_t d = us * 60U;

	for (i = 0U; i < d; i++)
	{
		__asm("NOP");
	}
}
